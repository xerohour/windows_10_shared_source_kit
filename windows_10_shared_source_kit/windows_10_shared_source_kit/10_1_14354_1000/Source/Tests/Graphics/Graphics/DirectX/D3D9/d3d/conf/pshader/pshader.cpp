// FILE:        PShader.cpp
// DESC:        pixel shader conformance tests
// AUTHOR:	    Anthony Clarke
// Modified:	Ver2.0 added  Bobby Paulose

#include <ddraw.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "PShader.h"
#include "assert.h"
CD3DWindowFramework App;

CPSRegister PSRegisterTest;
CPSAddress PSAddressTest;
CPSAddress_Bump PSAddress_BumpTest;
CPSAddress_Cube PSAddress_CubeTest;
CPSAddress_Vol PSAddress_VolTest;
CPSAddress_BumpCube PSAddress_BumpCubeTest;
CPSDestMod PSDestModTest;
CPSOps PSOpModTest;
CPSOps_Bump PSOpMod_BumpTest;
CPSSrcMod PSSrcModTest;
CReadShader PSReaderTest;
CPSVertShader PSVertShaderTest;
CPSVertShader_Bump PSVertShader_BumpTest;
CPSVertShader_Cube PSVertShader_CubeTest;
CPSVertShader_Vol PSVertShader_VolTest;
CPSVertShader_BumpCube PSVertShader_BumpCubeTest;
CPSLimits PSLimitsTest;
CPSLegacy PSLegacyTest;

#define AlmostOne 0.99f

MYTLVERTEXPERSP g_FourColorVerticesPerspective[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 1.0, 0.0, 0.25 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 0.0, 0.0, 0.5 },
	{ 1, 1, 1, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 1.0, 0.0, 0.25 },
	{ 1, 0, 1, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 0.0, 0.0, 0.5 }
};


MYTLVERTEXPERSP g_FourColorVerticesMipBias[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 1.0, 1.0, 0.0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 0), D3DRGBA(0, 0, 0, 1), 0.0, 0.0, 0.0, 3.0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 0), D3DRGBA(1, 1, 1, 1), 1.0, 1.0, 0.0, 0.0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 0.0, 1.0, 3.0 }
};


MYVOLTLVERTEX g_VolTexVertices[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 1.0, 0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 0, 0.33f },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 1.0f, 0.66f },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 0, 1.0}
};


MYTLVERTEX g_FourColorVertices[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 1.0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 0.0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 1.0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 0.0 }
};


MYTLVERTEXTEX2 g_FourColorVerticesTex2[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 1.0, 0.0, 1.0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 0.0, 0.0, 0.0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 1.0, 1.0, 1.0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 0.0, 1.0, 0.0 }
};


MYTLVERTEXTEX4 g_FourColorVerticesTex4[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0 }
};


MYTLVERTEXTEX6 g_FourColorVerticesTex6[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0 }
};


MYTLVERTEXTEX8 g_FourColorVerticesTex8[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0 }
};


MYTLVERTEX g_FourColorVertices2[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(0.5f, 0,	  0,    1), D3DRGBA(0,    0,    0,    1), 0.0, 1.0 },
	{ 0, 0, 0, 1, D3DRGBA(0,    0.5f, 0,    1), D3DRGBA(0,    0,    0,    1), 0.0, 0.0 },
	{ 1, 1, 0, 1, D3DRGBA(0.5f, 0.5f, 0.5f, 1), D3DRGBA(0.5f, 0.5f, 0.5f, 1), 1.0, 1.0 },
	{ 1, 0, 0, 1, D3DRGBA(0,    0,    0.5f, 1), D3DRGBA(0.5f, 0.5f, 0.5f, 1), 1.0, 0.0 }
};


MYTLVERTEX g_FourBrickVertices[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 4.0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 0.0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 4.0, 4.0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 4.0, 0.0 }
};


MYTLVERTEX g_FourColorAlphaVertices[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 0.4), D3DRGBA(0, 1, 1, 0.8), 0.0, 1.0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 0.1), D3DRGBA(1, 0, 1, 0.1), 0.0, 0.0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 0.4), D3DRGBA(0, 0, 0, 0.8), 1.0, 1.0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 0.1), D3DRGBA(1, 0, 0, 0.1), 1.0, 0.0 }
};


MYTLVERTEXTEX2 g_FourColorAlphaVerticesTex2[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 0.4), D3DRGBA(0, 1, 1, 0.8), 0.0, 1.0, 0.0, 1.0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 0.1), D3DRGBA(1, 0, 1, 0.1), 0.0, 0.0, 0.0, 0.0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 0.4), D3DRGBA(0, 0, 0, 0.8), 1.0, 1.0, 1.0, 1.0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 0.1), D3DRGBA(1, 0, 0, 0.1), 1.0, 0.0, 1.0, 0.0 }
};


MYVOLTLVERTEX g_TexKillVertices[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1),  0.0,  2.0, 0.0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), -2.0, -2.0, 0.0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1),  2.0,  2.0, 0.0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1),  2.0,  0.0, 0.0 }
};


// 0.1x, 0.5y, 0.7z creates a vector that points more in y,z than x
// This means that when it is used for indexing cube environment maps 
// in vspec op the + and - z faces of the cubes will be display/addressed.
MYCUBETLVERTEX3 g_Cube3Vertices[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 1,	0.5f, 0, 0, 0.1f,	0, 0.5f, 0, 0.5f,	0, 0, 0.5f, 0.7f},
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 0,	0.5f, 0, 0, 0.1f,	0, 0.5f, 0, 0.5f,	0, 0, 0.5f, 0.7f},
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 1,	0.5f, 0, 0, 0.1f,	0, 0.5f, 0, 0.5f,	0, 0, 0.5f, 0.7f},
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 0,	0.5f, 0, 0, 0.1f,	0, 0.5f, 0, 0.5f,	0, 0, 0.5f, 0.7f}
};


MYCUBETLVERTEX2 g_Texm2Vertices[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 2, AlmostOne, 	1, 0, 0,	0, 1, 0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 0, AlmostOne,	1, 0, 0,	0, 1, 0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 2, 2, AlmostOne,	1, 0, 0,	0, 1, 0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 2, 0, AlmostOne,	1, 0, 0,	0, 1, 0 }
};


MYCUBETLVERTEX2 g_DP3[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 1, 0, 			1, 0, 0,	0, 1, 0 },
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 0, 0,			1, 0, 0,	0, 1, 0 },
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 1, AlmostOne,	1, 0, 0,	0, 1, 0 },
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 0, AlmostOne,	1, 0, 0,	0, 1, 0 }
};


MYTLVERTEX3 g_Texm3Vertices[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 1, 0.5, 0.25,		1, 0, 0,	0, 1, 0,	0, 3, 0,	0, 1,	0, 4, 0},
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 0, 0.5, 0.25,		1, 0, 0,	0, 1, 0,	0, 0, 0,	0, 1,	0, 0, 0},
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 1, 0.0, 0.25,		1, 0, 0,	0, 1, 0,	3, 3, 0,	0, 1,	4, 4, 0},
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 0, 0.0, 0.25,		1, 0, 0,	0, 1, 0,	3, 0, 0,	0, 1,	4, 0, 0}
};


MYCUBETLVERTEX g_TexmZtests[8] =
{
	{ 0, 1, 0.5f, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1),  0, 1, 1,	1, 0, 0,	0, 1, 0,	0, 0, 1 },
	{ 0, 0, 0.5f, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1),  0, 0, 1,	1, 0, 0,	0, 1, 0,	0, 0, 1 },
	{ 1, 1, 0.5f, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1),  1, 1, 1,	1, 0, 0,	0, 1, 0, 	0, 0, 1 },
	{ 1, 0, 0.5f, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1),  1, 0, 1,	1, 0, 0,	0, 1, 0, 	0, 0, 1 },

	{ 0, 1, 0.25f, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(0, 0, 0, 1), 0, 1, 1,	1, 0, 0,	0, 1, 0, 	0, 0, 1 },
	{ 0, 0, 0.25f, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(0, 0, 0, 1), 0, 0, 1,	1, 0, 0,	0, 1, 0, 	0, 0, 1 },
	{ 1, 1, 0.25f, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 1, 1,	1, 0, 0,	0, 1, 0, 	0, 0, 1 },
	{ 1, 0, 0.25f, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 0, 1,	1, 0, 0,	0, 1, 0, 	0, 0, 1 }
};


//For this vertex set the main thing to not is the direction of
//change for x,y,z components of the texture coordinates (TC)
//TC 1 changes x from Bottom to Top (Bottom being smaller)
//TC 2 changes y from Right to Left (Right being smaller)
//TC 3 changes z from Top to Bottom (Top being smaller)
MYCUBETLVERTEX3 g_VolumeVertices[4] =
{
	{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 1,	0.2f, 0, 0, 0.0f,	    0, 1.75f, 0, 0.0f,	0, 0, AlmostOne, 0.0f},
	{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 0,	AlmostOne, 0, 0,0.0f,   0, 1.75f, 0, 0.0f,	0, 0, 0.1f, 0.0f},
	{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 1,	0.2f, 0, 0, 0.0f,	    0, 0, 0, 0.0f,	    0, 0, AlmostOne, 0.0f},
	{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 0,	AlmostOne, 0, 0, 0.0f,	0, 0, 0, 0.0f,	    0, 0, 0.1f, 0.0f}
};


CPixelShader::CPixelShader()
:
	m_bShaderAdded(false)
{
	SetTestType(TESTTYPE_CONF);

	//Textures for shaders
	m_pDXLogoTex = NULL;
	m_pCheckerTex = NULL;
	m_pColorCheckerTex = NULL;
	m_pBrickTex = NULL;

	// This in the constructor so that it can be over written before
	// TestInitialize if the test needs to.
	m_byFormats = TEXFORMAT;

	m_nDisplaySizePow2 = 1;

#ifdef FAST_TEST
	m_Options.ModeOptions.TextureFmt = FMT_A8R8G8B8;
	m_Options.ModeOptions.CubeFmt = FMT_A8R8G8B8;
	m_Options.ModeOptions.VolumeFmt = FMT_A8R8G8B8;
	m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
#else
	m_Options.ModeOptions.TextureFmt = FMT_ANY;
	m_Options.ModeOptions.CubeFmt = FMT_ANY;
	m_Options.ModeOptions.VolumeFmt = FMT_ANY;
	m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
#endif


	//This means that we will test the device with the following behaviors 
	// if available PURE or HWVP and a SWVP 
	m_Options.D3DOptions.fMinDXVersion = 8.0f;

	//This smaller resolution should speed up the test
	//without hiding or introducing failure.
	m_Options.D3DOptions.nBackBufferWidth = MODEOPT_BACKBUFFERWIDTH/4;
	m_Options.D3DOptions.nBackBufferHeight = MODEOPT_BACKBUFFERHEIGHT/4;

	m_pFramework->GetImageCmp()->SetMinPixels(0);

	m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_SWVP | DEVICECREATE_MIXED;

	return;
}


void CPixelShader::CommandLineHelp(void)
{
	WriteCommandLineHelp(_T("$yVer: $wShader version to test $c(default = All versions <= Supported)"));
	WriteCommandLineHelp(_T("$yVerSpec: $wShaders Specific to this version not capable \n\t\t\t\t of running in lower versions $c(There is no default)"));
	WriteCommandLineHelp(_T("$yNoCheckVer: $wWill turn off supported version check \n\t\t\t\t $c(default is version supported must be <= versions tested )"));
}


bool CPixelShader::CreateShaderVersionArray()
{
	DWORD dwShaderVer = m_dwMinPSVersion;
	bool bAllOk = true; 

	m_uNumShaderVersions = 0;
	if (m_dwRequestedVersion != PSVERSION(0,0))
	{
		m_vShaderVersions.push_back(m_dwRequestedVersion);
		m_uNumShaderVersions++;
	}
	else
	{		
		while(1)
		{
#ifdef FAST_TEST
			m_vShaderVersions.push_back(PSVERSION(2, 0));
			dwShaderVer = MAGIC_SHADER; // This will end the loop.
			m_uNumShaderVersions++;
#else
			switch (dwShaderVer)
			{
				case PSVERSION(1,0):
				{	
					m_vShaderVersions.push_back(PSVERSION(1,0));
					dwShaderVer = PSVERSION(1,1);
					m_uNumShaderVersions++;
					break;
				}

				case PSVERSION(1,1):
				{
					m_vShaderVersions.push_back(PSVERSION(1,1));
					dwShaderVer = PSVERSION(1,2);
					m_uNumShaderVersions++;
					break;
				}

				case PSVERSION(1,2):
				{
					m_vShaderVersions.push_back(PSVERSION(1,2));
					dwShaderVer = PSVERSION(1,3);
					m_uNumShaderVersions++;
					break;
				}

				case PSVERSION(1,3):
				{
					m_vShaderVersions.push_back(PSVERSION(1,3));
					dwShaderVer = PSVERSION(1,4);
					m_uNumShaderVersions++;
					break;
				}

				case PSVERSION(1,4):
				{
					m_vShaderVersions.push_back(PSVERSION(1,4));
					dwShaderVer = PSVERSION(2,0);
					m_uNumShaderVersions++;
					break;
				}

				case PSVERSION(2,0):
				{
					m_vShaderVersions.push_back(PSVERSION(2,0));
					dwShaderVer = MAGIC_SHADER; // This will end the loop.
					m_uNumShaderVersions++;
					break;
				}
			}
#endif

			//only add versions to be tested if they are supported
			if (dwShaderVer > m_dwSupported)
			{				 
				if ((m_dwSupported != PSVERSION(3,0)) && (m_dwSupported != m_vShaderVersions[m_vShaderVersions.size() - 1]))
				{
					if (MAGIC_SHADER == dwShaderVer)
						WriteToLog( _T("Shader version is beyond the range of this kit.\n Please use latest version.\n") );
					else
						WriteToLog( _T("Shader version is invalid.\n") );
					bAllOk = false;
				}

				break;
			}
		}
	}

	return bAllOk;
}


UINT CPixelShader::TestInitialize()
{
	static bool bStretch = true;

	m_nVolFormat = m_nCubeFormat = m_nTexFormat = m_nBumpFormat = -1;
	m_uPSCommonVolFormats = m_uPSCommonCubeFormats = 0;
	m_uPSCommonBumpFormats = m_uPSCommonTexFormats = 0;
	m_uNumShaderVersions = 0;
	m_uTotalFormats = 0;
	m_bSupports255 = false;
	m_dwTested255 = 0;	
	m_Flags.Replace(0);
	m_bFormatChanged = true;
	m_uiLastTest = 0;
	m_bLastForward = true;
	m_uiShaderNotRendered = 0;

	// PSVersion 1.1 is the minimum for Dx9
	m_dwMinPSVersion = (m_dwVersion >= 0x0900) ? PSVERSION(1,1) : PSVERSION(1,0);

	ProcessArgs();

	//See if pixel shaders are supported at all.
	if ( PSVERSION(0, 0) == m_pSrcDevice->GetCaps()->dwPixelShaderVersion )
	{
		WriteToLog(_T("Pixel Shaders are not supported!\n") );
		SetTestRange(1,1);
		return D3DTESTINIT_SKIPALL;
	}

	//See if the supported version is less than the requested.
	if ( m_pSrcDevice->GetCaps()->dwPixelShaderVersion < m_dwRequestedVersion)
	{
		WriteToLog(_T("Requested pixel shader version %d.%d is not supported!\n"), D3DSHADER_VERSION_MAJOR(m_dwRequestedVersion), D3DSHADER_VERSION_MINOR(m_dwRequestedVersion) );                                    
		SetTestRange(1,1);
		return D3DTESTINIT_SKIPALL;
	}
	
	//See if pixel shaders 3.0 are being requested and are supported.
	if ( (PSVERSION(3, 0) == m_dwRequestedVersion) && (PSVERSION(3, 0) == m_pSrcDevice->GetCaps()->dwPixelShaderVersion) )
	{
		WriteToLog(_T("Pixel Shaders 3.0 are not allowed with this test!\n") );
		SetTestRange(1,1);
		return D3DTESTINIT_ABORT;
	}

	if (!DoCapsCheck())
	{
		WriteToLog(_T("Failed due to incorrect caps being set!\n") );
		SetTestRange(1,1);
		return D3DTESTINIT_ABORT;
	}

	if (!CreateShaderVersionArray())
	{
		//This means that the shader version is either invalid or 
		//one that is beyond that which is testable by this version of the test.
		WriteToLog(_T("Failed to create shader version array!") );
		SetTestRange(1,1);
		return D3DTESTINIT_ABORT;
	}
    
	//Generate the data used to populate shader textures.
	if (!GenerateTextureData())
	{
		WriteToLog(_T("GenerateTextureData failed."));
		SetTestRange(1,1);
		return D3DTESTINIT_ABORT;
	}

	//Set the number of common bump and other texture formats.
	if (!SetTextureFormat())
	{
		WriteToLog(_T("(ERROR) No common texture formats found."));
		SetTestRange(1,1);
		return D3DTESTINIT_ABORT;
	}
    
	DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
	DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;        

	m_nDisplaySizePow2 = (dwWidth < dwHeight) ? dwWidth : dwHeight;
	int nPow2 = 1;
	while (m_nDisplaySizePow2 >= nPow2) nPow2 = nPow2 * 2;
	m_nDisplaySizePow2 = nPow2/2;

	// Adjust for rendering resolution
	if (bStretch)
	{
		UINT i;
		for (i = 0; i < 4; i++)
		{
			g_FourColorVerticesPerspective[i].x *= (float) dwWidth;
			g_FourColorVerticesPerspective[i].y *= (float) dwHeight;
			g_FourColorVerticesPerspective[i].x -= OFFSET_FOR_TEXTURE;
			g_FourColorVerticesPerspective[i].y -= OFFSET_FOR_TEXTURE;

			g_FourColorVerticesMipBias[i].x *= (float) dwWidth;
			g_FourColorVerticesMipBias[i].y *= (float) dwHeight;
			g_FourColorVerticesMipBias[i].x -= OFFSET_FOR_TEXTURE;
			g_FourColorVerticesMipBias[i].y -= OFFSET_FOR_TEXTURE;

			g_FourColorVerticesTex2[i].x *= (float) dwWidth;
			g_FourColorVerticesTex2[i].y *= (float) dwHeight;
			g_FourColorVerticesTex2[i].x -= OFFSET_FOR_TEXTURE;
			g_FourColorVerticesTex2[i].y -= OFFSET_FOR_TEXTURE;

			g_FourColorVerticesTex4[i].x *= (float) dwWidth;
			g_FourColorVerticesTex4[i].y *= (float) dwHeight;
			g_FourColorVerticesTex4[i].x -= OFFSET_FOR_TEXTURE;
			g_FourColorVerticesTex4[i].y -= OFFSET_FOR_TEXTURE;

			g_FourColorVerticesTex6[i].x *= (float) dwWidth;
			g_FourColorVerticesTex6[i].y *= (float) dwHeight;
			g_FourColorVerticesTex6[i].x -= OFFSET_FOR_TEXTURE;
			g_FourColorVerticesTex6[i].y -= OFFSET_FOR_TEXTURE;

			g_FourColorVerticesTex8[i].x *= (float) dwWidth;
			g_FourColorVerticesTex8[i].y *= (float) dwHeight;
			g_FourColorVerticesTex8[i].x -= OFFSET_FOR_TEXTURE;
			g_FourColorVerticesTex8[i].y -= OFFSET_FOR_TEXTURE;

			g_VolTexVertices[i].x *= (float) dwWidth;
			g_VolTexVertices[i].y *= (float) dwHeight;
			g_VolTexVertices[i].x -= OFFSET_FOR_TEXTURE;
			g_VolTexVertices[i].y -= OFFSET_FOR_TEXTURE;

			g_FourColorVertices[i].x *= (float) dwWidth;
			g_FourColorVertices[i].y *= (float) dwHeight;
			g_FourColorVertices[i].x -= OFFSET_FOR_TEXTURE;
			g_FourColorVertices[i].y -= OFFSET_FOR_TEXTURE;

			g_FourColorVertices2[i].x *= (float) dwWidth;
			g_FourColorVertices2[i].y *= (float) dwHeight;
			g_FourColorVertices2[i].x -= OFFSET_FOR_TEXTURE;
			g_FourColorVertices2[i].y -= OFFSET_FOR_TEXTURE;

			g_FourBrickVertices[i].x *= (float) dwWidth;
			g_FourBrickVertices[i].y *= (float) dwHeight;
			g_FourBrickVertices[i].x -= OFFSET_FOR_TEXTURE;
			g_FourBrickVertices[i].y -= OFFSET_FOR_TEXTURE;

			g_FourColorAlphaVertices[i].x *= (float) dwWidth;
			g_FourColorAlphaVertices[i].y *= (float) dwHeight;
			g_FourColorAlphaVertices[i].x -= OFFSET_FOR_TEXTURE;
			g_FourColorAlphaVertices[i].y -= OFFSET_FOR_TEXTURE;

			g_FourColorAlphaVerticesTex2[i].x *= (float) dwWidth;
			g_FourColorAlphaVerticesTex2[i].y *= (float) dwHeight;
			g_FourColorAlphaVerticesTex2[i].x -= OFFSET_FOR_TEXTURE;
			g_FourColorAlphaVerticesTex2[i].y -= OFFSET_FOR_TEXTURE;

			g_TexKillVertices[i].x *= (float) dwWidth;
			g_TexKillVertices[i].y *= (float) dwHeight;
			g_TexKillVertices[i].x -= OFFSET_FOR_TEXTURE;
			g_TexKillVertices[i].y -= OFFSET_FOR_TEXTURE;

			g_Cube3Vertices[i].x *= (float) dwWidth;
			g_Cube3Vertices[i].y *= (float) dwHeight;
			g_Cube3Vertices[i].x -= OFFSET_FOR_TEXTURE;
			g_Cube3Vertices[i].y -= OFFSET_FOR_TEXTURE;

			g_Texm2Vertices[i].x *= (float) dwWidth;
			g_Texm2Vertices[i].y *= (float) dwHeight;
			g_Texm2Vertices[i].x -= OFFSET_FOR_TEXTURE;
			g_Texm2Vertices[i].y -= OFFSET_FOR_TEXTURE;

			g_Texm3Vertices[i].x *= (float) dwWidth;
			g_Texm3Vertices[i].y *= (float) dwHeight;
			g_Texm3Vertices[i].x -= OFFSET_FOR_TEXTURE;
			g_Texm3Vertices[i].y -= OFFSET_FOR_TEXTURE;
			
			g_DP3[i].x *= (float) dwWidth;
			g_DP3[i].y *= (float) dwHeight;
			g_DP3[i].x -= OFFSET_FOR_TEXTURE;
			g_DP3[i].y -= OFFSET_FOR_TEXTURE;

			g_VolumeVertices[i].x *= (float) dwWidth;
			g_VolumeVertices[i].y *= (float) dwHeight;
			g_VolumeVertices[i].x -= OFFSET_FOR_TEXTURE;
			g_VolumeVertices[i].y -= OFFSET_FOR_TEXTURE;
		}

		for (i = 0; i < 8; i++)
		{
			g_TexmZtests[i].x = ( g_TexmZtests[i].x * (float)dwWidth ) - OFFSET_FOR_TEXTURE;
			g_TexmZtests[i].y = ( g_TexmZtests[i].y * (float)dwHeight ) - OFFSET_FOR_TEXTURE;            
		}

		bStretch = false;
	}

	return D3DTESTINIT_RUN;
}


bool CPixelShader::TestTerminate()
{
	ReleaseTextures();
	ReleaseTextureData();
	SAFEDELETEA(m_pTexFormats);
	SAFEDELETEA(m_pBumpFormats);

	for
	(
		SHADERVECTOR::iterator iShader = m_vShaders.begin();
		iShader != m_vShaders.end();
		++iShader
	)
	{
		DeleteShaderMemory(*iShader);

		delete *iShader;
		*iShader = NULL;
	}

	m_vShaders.clear();

	m_vShaderVersions.clear();
	CleanUpReg();

	return true;
}


void CPixelShader::DeleteShaderMemory(MYSHADER * pShader)
{
	RELEASE(pShader->pVBs);
	if (pShader->pCode)
	{
		delete []pShader->pCode;
		pShader->pCode = NULL;
	}
	if (pShader->pName)
	{
		delete []pShader->pName;
		pShader->pName = NULL;
	}
	if (pShader->pVertices)
	{
		delete []pShader->pVertices;
		pShader->pVertices = NULL;
	}
	if (pShader->dwVSHandle)
	{
		DeleteVertexShader(pShader->dwVSHandle);
		pShader->dwVSHandle = 0;
	}
	if (pShader->dwHandle)
	{
		DeletePixelShader(pShader->dwHandle);
		pShader->dwHandle = 0;
	}
	return;
}


void CPixelShader::AddShaderDef(TCHAR *pName, TCHAR *pCode, DWORD dwFVF, void *pVertices, bool bAlpha, ShaderType Type, DWORD dwRequired)
{
	m_bShaderAdded = false;

	MYSHADER * pNewShader = NULL;


	//Check if the hardware supports the version required for this shader.
	//or if the shader version matches that which was specifically requested.
	if ( (m_dwSupported < dwRequired && dwRequired != MAGIC_SHADER) ||
			(m_bSpecificVerRequest && dwRequired != m_dwRequestedVersion) )
	{
		goto Cleanup;
	}
	
	switch (m_dwRequestedVersion)
	{
		case PSVERSION(1,0):
		case PSVERSION(1,1):
		case PSVERSION(1,2):
		case PSVERSION(1,3):
		{
			if (dwRequired > m_dwRequestedVersion)
			{
				goto Cleanup;
			}
		} break;

		case PSVERSION(1,4):
		{
			if (dwRequired != m_dwRequestedVersion)
			{
				goto Cleanup;
			}
		} break;

		case PSVERSION(2,0):
		{
			if (dwRequired != m_dwRequestedVersion)
			{
				goto Cleanup;
			}
		} break;

		default:
		{
			//Do nothing.
			//This occurs when there is no requested version i.e m_dwRequestedVersion == 0.0
		} break;
	}


	pNewShader = new MYSHADER;
	if (pNewShader == NULL)
	{
		goto Cleanup;
	}

	pNewShader->bSetup = true;

	pNewShader->pName = new TCHAR[_tcslen(pName) + 1];
	if (pNewShader->pName == NULL)
	{
		goto Cleanup;
	}
	_tcscpy(pNewShader->pName, pName);

	pNewShader->Type = Type;

	pNewShader->pCode = new TCHAR[_tcslen(pCode)+1];
	if (pNewShader->pCode == NULL)
	{
		goto Cleanup;
	}
	_tcscpy(pNewShader->pCode, pCode);

	pNewShader->dwConstantsStart = 0;
	pNewShader->dwHandle = 0;
	pNewShader->dwVSHandle = 0;
	pNewShader->pVBs = NULL;
	pNewShader->dwVertexFormat = dwFVF;
	pNewShader->dwVertexFormatSize = GetVertexSize(dwFVF);
	pNewShader->dwRequired = dwRequired;
	pNewShader->bAlpha = bAlpha;
	pNewShader->bWrapAndMip = false;
	memset( &pNewShader->rgnCoordsForStage, 0, sizeof(pNewShader->rgnCoordsForStage) );

	if (pNewShader->Type == SHADE_NEEDZ)
	{
		pNewShader->pVertices = new BYTE[8 * GetVertexSize(dwFVF)];
		if (pNewShader->pVertices == NULL)
		{
			goto Cleanup;
		}

		memcpy(pNewShader->pVertices, pVertices, 8*GetVertexSize(dwFVF));
	}
	else
	{
		pNewShader->pVertices = new BYTE[4 * GetVertexSize(dwFVF)];
		if (pNewShader->pVertices == NULL)
		{
			goto Cleanup;
		}

		memcpy(pNewShader->pVertices, pVertices, 4*GetVertexSize(dwFVF));
	}

	pNewShader->uBumpStage = DEFAULT_BUMPMATIXSTAGE;
	pNewShader->vstPrimType = VST_RENDERPRIM;
    
	// Formats used can be set later using the SetLastShaderTexFormats function.
	pNewShader->byTexFormatsUsed = TEXFORMAT;
	pNewShader->d3dfTexFormat = FMT_UNKNOWN;
	pNewShader->d3dfBumpFormat = FMT_UNKNOWN;
	pNewShader->d3dfCubeFormat = FMT_UNKNOWN;
	pNewShader->d3dfVolFormat = FMT_UNKNOWN;
	pNewShader->Flags.Replace(0);
    
	ReCreateShaderAndBuffers(pNewShader);


	m_vShaders.push_back(pNewShader);

	m_bShaderAdded = true;


Cleanup:
	if (m_bShaderAdded != true)
	{
		if (pNewShader != NULL)
		{
			if (pNewShader->pName != NULL)
			{
				delete [] pNewShader->pName;
				pNewShader->pName = NULL;
			}

			if (pNewShader->pCode != NULL)
			{
				delete [] pNewShader->pCode;
				pNewShader->pCode = NULL;
			}

			if (pNewShader->pVertices != NULL)
			{
				delete [] pNewShader->pVertices;
				pNewShader->pVertices = NULL;
			}

			delete pNewShader;
			pNewShader = NULL;
		}
	}

	return;
}


void CPixelShader::ReCreateShaderAndBuffers(MYSHADER *pShader)
{	
	DWORD dwFlags = 0;
	LPD3DXBUFFER pXCode = NULL;
	LPD3DXBUFFER pXError = NULL;
	HRESULT hResult;
	char szCode[MAX_CODES];
	
	pShader->bSetup = true;
	sprintf(szCode, "ps.%d.%d\n%s", D3DSHADER_VERSION_MAJOR(pShader->dwRequired),
									D3DSHADER_VERSION_MINOR(pShader->dwRequired), pShader->pCode);
    
	hResult = D3DXAssembleShader(szCode, strlen(szCode), dwFlags, NULL, &pXCode, &pXError);

	if (FAILED(hResult))
	{
		WriteToLog("(ERROR) Pixel shader assembly failed:  %08X\n", hResult);
		WriteToLog("        Shader Name: %s\n", pShader->pName);
		WriteToLog("        Shader Code:\n%s\n", szCode);
		WriteToLog("        Error: %s\n", pXError->GetBufferPointer());
		pShader->bSetup = false;
	}
	else
	{
		if (pShader->dwHandle)
		{
			DeletePixelShader(pShader->dwHandle);
			pShader->dwHandle = 0;
		}
		if (!CreatePixelShader((DWORD *) pXCode->GetBufferPointer(), &(pShader->dwHandle)))
		{
			if (D3DSHADER_VERSION_MAJOR(pShader->dwRequired) != 255)
			{
				WriteToLog("(ERROR) CreatePixelShader failed for shader '%s' - '%s'\n",
							pShader->pName, GLE(GetLastError()));
			}
			pShader->bSetup = false;
		}
	}
    
	RELEASE(pShader->pVBs);
	if (pShader->vstPrimType != VST_RENDERPRIM)
	{
		if (!CreateLoadedVertexBuffers(pShader))
		{
			pShader->bSetup = false;
		}
	}

	ResetShaderTexFormats(pShader);
	RELEASE(pXCode);
	RELEASE(pXError);	
	return;
}


bool CPixelShader::CreateLoadedVertexBuffers(MYSHADER * pShader)
{
	DWORD dwNumVerts, dwVBUsage = 0;
	BYTE *pbVertexTarget = NULL;    
	DWORD dwPool = POOL_DEFAULT;
	DWORD dwUsage = (DWORD) 0;

	if (pShader->Type == SHADE_NEEDZ)
		dwNumVerts = 8;	
	else
		dwNumVerts = 4;

	if (!m_Flags.Check(VBHARDWARE))
		dwVBUsage = D3DUSAGE_SOFTWAREPROCESSING | D3DUSAGE_WRITEONLY;
	else
		dwVBUsage = D3DUSAGE_WRITEONLY;

	if ( !CreateVertexBuffer(dwNumVerts * pShader->dwVertexFormatSize, dwVBUsage, pShader->dwVertexFormat, dwPool, &pShader->pVBs) )
	{
		WriteToLog(_T("(ERROR) Failed to create vertex buffers. (%08X)\n"), GLE(GetLastError()) );
		return false;
	}

	if ( pShader->pVBs->Lock(0, 0, &pbVertexTarget, LOCK_NOSYSLOCK) )
	{
		memcpy(pbVertexTarget, pShader->pVertices, dwNumVerts*pShader->dwVertexFormatSize);
		pShader->pVBs->Unlock();
		pbVertexTarget = NULL;
	}
	else
	{
		WriteToLog("(ERROR) Failed to lock vertex buffer.\n");
		RELEASE(pShader->pVBs);
		return false;
	}

	return true;
}


void CPixelShader::AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices)
{
	AddShaderDef(pName, pCode, dwFVF, pVertices, false, SHADE_PLAIN, m_dwMinPSVersion);
}


void CPixelShader::AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, bool bAlpha)
{
	AddShaderDef(pName, pCode, dwFVF, pVertices, bAlpha, SHADE_PLAIN, m_dwMinPSVersion);
} 


void CPixelShader::AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, ShaderType Type, DWORD dwRequired)
{
	AddShaderDef(pName, pCode, dwFVF, pVertices, false, Type, dwRequired);
} 


void CPixelShader::AddShaderDef(TCHAR *pName, char *pCode, DWORD dwFVF, void *pVertices, DWORD dwRequired)
{
	AddShaderDef(pName, pCode, dwFVF, pVertices, false, SHADE_PLAIN, dwRequired);
}


//Operations that work on last added shader.
void CPixelShader::AddTextureToLastShaderDef(CnBaseTexture **ppTexture)
{
	if (!m_bShaderAdded)
	{
		return;
	}

	if (m_vShaders.size() < 1)
	{
		return;
	}

	MYSHADER * pShader = m_vShaders.back();
	pShader->vTextures.push_back(ppTexture);
}


void CPixelShader::AddTextureToLastShaderDef(CnBaseTexture **ppTexture, UINT uiStage)
{
	if (!m_bShaderAdded)
	{
		return;
	}

	if (m_vShaders.size() < 1)
	{
		return;
	}

	MYSHADER * pShader = m_vShaders.back();

	if ( uiStage >= pShader->vTextures.size() )
	{	
		for (UINT i = pShader->vTextures.size(); i < uiStage; ++i)
		{
			AddTextureToLastShaderDef(NULL);
		}

		AddTextureToLastShaderDef(ppTexture);
	}
	else
	{
		pShader->vTextures[uiStage] = ppTexture;
	}
}


void CPixelShader::SetLastShaderWrapAndMip(void)
{
	if (!m_bShaderAdded)
	{
		return;
	}

	if (m_vShaders.size() < 1)
	{
		return;
	}

	MYSHADER * pShader = m_vShaders.back();
	pShader->bWrapAndMip = true;
}


void CPixelShader::SetCoordsForLastPSStages(DWORD dwStage, DWORD dwCoordNum)
{
	if (!m_bShaderAdded)
	{
		return;
	}

	if (m_vShaders.size() < 1)
	{
		return;
	}

	MYSHADER * pShader = m_vShaders.back();
	pShader->rgnCoordsForStage[dwStage] = dwCoordNum;
}


void CPixelShader::SetLastShaderTexFormats(BYTE byFormats)
{
	if (!m_bShaderAdded)
	{
		return;
	}

	if (m_vShaders.size() < 1)
	{
		return;
	}

	MYSHADER * pShader = m_vShaders.back();
	pShader->byTexFormatsUsed = byFormats;    
}


void CPixelShader::SetLastShaderFlags(DWORD dwFlags)
{
	if (!m_bShaderAdded)
	{
		return;
	}

	if (m_vShaders.size() < 1)
	{
		return;
	}

	MYSHADER * pShader = m_vShaders.back();
	pShader->Flags.Replace(dwFlags);
}


void CPixelShader::SetConstantsStart(DWORD dwStart)
{
	if (!m_bShaderAdded)
	{
		return;
	}

	if (m_vShaders.size() < 1)
	{
		return;
	}

	MYSHADER * pShader = m_vShaders.back();
	pShader->dwConstantsStart = dwStart;
}


void CPixelShader::AddConstantsToLastShaderDef(float a, float b, float c, float d)
{
	if (!m_bShaderAdded)
	{
		return;
	}

	if (m_vShaders.size() < 1)
	{
		return;
	}

	MYSHADER * pShader = m_vShaders.back();
	pShader->vConstants.push_back(a);
	pShader->vConstants.push_back(b);
	pShader->vConstants.push_back(c);
	pShader->vConstants.push_back(d);
}


void CPixelShader::AddConstantsToLastShaderDef(float f[4])
{
	if (!m_bShaderAdded)
	{
		return;
	}

	if (m_vShaders.size() < 1)
	{
		return;
	}

	MYSHADER * pShader = m_vShaders.back();
	pShader->vConstants.push_back(f[0]);
	pShader->vConstants.push_back(f[1]);
	pShader->vConstants.push_back(f[2]);
	pShader->vConstants.push_back(f[3]);
}


void CPixelShader::AddConstantsToLastShaderDef(float *f, DWORD dwCount)
{
	if (!m_bShaderAdded)
	{
		return;
	}

	if (m_vShaders.size() < 1)
	{
		return;
	}

	MYSHADER * pShader = m_vShaders.back();

	for (DWORD d = 0; d < dwCount; d++)
	{
		pShader->vConstants.push_back(f[d]);
	}
}


void CPixelShader::AddBumpStageForLastShaderDef(UINT uStageNum)
{
	if (!m_bShaderAdded)
	{
		return;
	}

	if (m_vShaders.size() < 1)
	{
		return;
	}

	MYSHADER * pShader = m_vShaders.back();
	pShader->uBumpStage = uStageNum;
}


void CPixelShader::ResetAllShaderTexFormats()
{
	for
	(
		SHADERVECTOR::iterator iShader = m_vShaders.begin();
		iShader != m_vShaders.end();
		++iShader
	)
	{
		ResetShaderTexFormats(*iShader);
	}
}


void CPixelShader::ResetShaderTexFormats(MYSHADER * pShader)
{
	pShader->d3dfTexFormat = FMT_UNKNOWN;
	pShader->d3dfBumpFormat = FMT_UNKNOWN;
	pShader->d3dfCubeFormat = FMT_UNKNOWN;
	pShader->d3dfVolFormat = FMT_UNKNOWN;
}


bool CPixelShader::SetupBasicScene()
{
	DWORD dwPasses = 0;
	MYSHADER * pShader = m_vShaders[m_dwCurrentShader];	

	if (!SetPixelShader(pShader->dwHandle))
	{
		WriteToLog("(ERROR) Failed to set pixel shader. (%08X)\n", GetLastError());
		return false;
	}

	//If we are not using Render primitive but are using FVF's
	if (pShader->vstPrimType == VST_FVF)
	{
		if (!SetVertexShader(pShader->dwVertexFormat))
		{
			WriteToLog("(ERROR) SetVertexShader failed. (%08X)\n", GetLastError());
			return false;
		}

		if (!SetStreamSource((DWORD) 0, pShader->pVBs, pShader->dwVertexFormatSize)) 
		{
			WriteToLog("(ERROR) SetStreamSource failed. (%08X)\n", GetLastError());
			return false;
		}
	}
	//If we are not using Render primitive but are using a vertex shader
	else if (pShader->vstPrimType == VST_PROG)
	{
		// need to grab vertex shader.
		if (!SetVertexShader(pShader->dwVSHandle))
		{
			WriteToLog("(ERROR) SetVertexShader failed. (%08X)\n", GetLastError());
			return false;
		}

		if (!SetStreamSource(DEFAULT_VSSTREAM, pShader->pVBs, pShader->dwVertexFormatSize)) 
		{
			WriteToLog("(ERROR) SetStreamSource failed. (%08X)\n", GetLastError());
			return false;
		}
	}
	//If we are using Render primitive
	else
	{
		if (!SetFVF(pShader->dwVertexFormat))
		{
			WriteToLog("(ERROR) SetVertexShader failed. (%08X)\n", GetLastError());
			return false;
		}
	}

	// Default to use filtering
	bool bPointFilter = false;

	// On Dx9, need to check for UsageQueryFilter
	if (m_dwVersion >= 0x0900)
	{
		// Check the regular texture format
		if (!bPointFilter && (pShader->d3dfTexFormat != FMT_UNKNOWN))
		{
			FORMAT TexFormat(pShader->d3dfTexFormat);

			if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
												USAGE_QUERY_FILTER, RTYPE_TEXTURE, TexFormat)))
			{
				bPointFilter = true;
			}
		}

		// Check the regular texture format used for bump maps
		if (!bPointFilter && (pShader->d3dfBumpFormat != FMT_UNKNOWN))
		{
			FORMAT BumpFormat(pShader->d3dfBumpFormat);

			if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
												USAGE_QUERY_FILTER, RTYPE_TEXTURE, BumpFormat)))
			{
				bPointFilter = true;
			}
		}

		// Check the cube texture format
		if (!bPointFilter && (pShader->d3dfCubeFormat != FMT_UNKNOWN))
		{
			FORMAT CubeFormat(pShader->d3dfCubeFormat);

			if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
												USAGE_QUERY_FILTER, RTYPE_CUBETEXTURE, CubeFormat)))
			{
				bPointFilter = true;
			}
		}

		// Check the volume texture format
		if (!bPointFilter && (pShader->d3dfVolFormat != FMT_UNKNOWN))
		{
			FORMAT VolFormat(pShader->d3dfVolFormat);

			if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
												USAGE_QUERY_FILTER, RTYPE_VOLUMETEXTURE, VolFormat)))
			{
				bPointFilter = true;
			}
		}
	}

	///////////
	// Need to check for problematic shaders that can't handle filtering

	// Check a special key to help debug failures
	if (KeySet("CubeLookup"))
	{

		if ( !bPointFilter &&
				(pShader->byTexFormatsUsed & CUBEFORMAT) &&
				((_tcsnccmp(pShader->pName, _T("Address: texm3x3tex"), _tcslen(_T("Address: texm3x3tex"))) == 0) ||
				(_tcsnccmp(pShader->pName, _T("Address: Arbitrary"), _tcslen(_T("Address: Arbitrary"))) == 0)   ||
				(_tcsnccmp(pShader->pName, _T("Address: texreg2rgb_bx2"), _tcslen(_T("Address: texreg2rgb_bx2"))) == 0))
			)
		{
			// Need at least 3 channels to do a cubemap lookup, force point sampling.
			if (pShader->byTexFormatsUsed & TEXFORMAT)
			{
				if ( (m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_A8)     ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_G16R16) ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_L8)     ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_A8L8)   ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_A4L4)   ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_L16)    ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_R16F)   ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_G16R16F)||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_R32F)   ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_G32R32F)    
					)
				{
					bPointFilter = true;
				}
			}
		}

	}

	if ( (m_dwVersion < 0x0900) && !bPointFilter  &&
			(pShader->byTexFormatsUsed & VOLUMEFORMAT) &&
			(_tcsnccmp(pShader->pName, _T("Address: texm3x3tex"), _tcslen(_T("Address: texm3x3tex"))) == 0)
		)
	{
		// Need to avoid Dx8 YUV precision problems when using as a lookup, force point sampling.
		if (pShader->byTexFormatsUsed & TEXFORMAT)
		{
			if ( (m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_UYVY) ||
					(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_YUY2)
				)
			{
				bPointFilter = true;
			}
		}
	}

	// Forced point sampling to avoid texture sampling precision problems on dependent texld shader
	if ( !bPointFilter &&
			(_tcsnccmp(pShader->pName, _T("Dependent"), _tcslen(_T("Dependent"))) == 0)
		)
	{
		bPointFilter = true;
	}

	TEXTUREVECTOR &TexVec = pShader->vTextures;
	TEXTUREVECTOR::iterator iCurrentTexture = TexVec.begin();
	DWORD dwMaxSampler = (m_dwVersion >= 0x0900) ? MAXTEXSAMPLER : MAXTEXSTAGE; 
	for (DWORD dwCurrentTexIndex = 0;  dwCurrentTexIndex < dwMaxSampler; ++dwCurrentTexIndex)
	{
		if (iCurrentTexture != TexVec.end())
		{
			if (NULL != *iCurrentTexture)
				SetTexture(dwCurrentTexIndex, **iCurrentTexture);
			else
				SetTexture(dwCurrentTexIndex, NULL);

			iCurrentTexture++;            
		}
		else
		{
			//Clear all possible texture stages.
			SetTexture(dwCurrentTexIndex, NULL);
		}

		//reset mipmapping to none
		SetTextureStageState(dwCurrentTexIndex, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);

		if (bPointFilter)
		{
			// Forced to use point sampling because texture cannot be filtered
			SetTextureStageState(dwCurrentTexIndex, D3DTSS_MINFILTER, (DWORD)D3DTEXF_POINT);
			SetTextureStageState(dwCurrentTexIndex, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_POINT);
		}
		else
		{
			// Use Bi-linear filtering unless requested otherwise
			SetTextureStageState(dwCurrentTexIndex, D3DTSS_MINFILTER, (DWORD)D3DTEXF_LINEAR);
			SetTextureStageState(dwCurrentTexIndex, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_LINEAR);
		}

		// Need to obey the maximum number of texture stages
		if (dwCurrentTexIndex >= MAXTEXSTAGE)
			continue;

		if (pShader->vstPrimType != VST_PROG)
			SetTextureStageState( dwCurrentTexIndex, D3DTSS_TEXCOORDINDEX, (DWORD) pShader->rgnCoordsForStage[dwCurrentTexIndex] );
		else
			SetTextureStageState( dwCurrentTexIndex, D3DTSS_TEXCOORDINDEX, (DWORD) dwCurrentTexIndex );
        
		//Reset the TextureStageStates to defaults.
		//This is done to reset the fact that the last shader may have setup bump
		//transforms on any stage.
		SetTextureStageState(dwCurrentTexIndex, D3DTSS_BUMPENVMAT00, 0);
		SetTextureStageState(dwCurrentTexIndex, D3DTSS_BUMPENVMAT01, 0);
		SetTextureStageState(dwCurrentTexIndex, D3DTSS_BUMPENVMAT10, 0);
		SetTextureStageState(dwCurrentTexIndex, D3DTSS_BUMPENVMAT11, 0);
		SetTextureStageState(dwCurrentTexIndex, D3DTSS_BUMPENVLSCALE, 0);
		SetTextureStageState(dwCurrentTexIndex, D3DTSS_BUMPENVLOFFSET, 0);
	}

	//Reset the projected render state
	SetTextureStageState(DEFAULT_PROJSTAGE, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)D3DTTFF_DISABLE);

	if (pShader->vConstants.size() > 0)
	{
		if (((pShader->vConstants.size() % 4) != 0))
		{
			WriteToLog("(ERROR) Invalid number of constants specified\n");
			return false;
		}
		if (!SetPixelShaderConstant(pShader->dwConstantsStart, &(pShader->vConstants[0]), pShader->vConstants.size()/4))
		{
			WriteToLog("(ERROR) SetPixelShaderConstant failed. (%08X)\n", GetLastError());
			return false;
		}
	}

	if (pShader->bAlpha)
		SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD) TRUE);
	else
		SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD) FALSE);

	switch (pShader->Type)
	{
		case SHADE_LBUMP:
			if (m_nBumpFormat >= 0 && !(m_pBumpFormats[m_nBumpFormat].ddpfPixelFormat.dwFlags & PF_BUMPLUMINANCE) )
				pShader->Flags.Set(SH_INVALIDFORMAT);
			else
				pShader->Flags.Clear(SH_INVALIDFORMAT);

		case SHADE_BUMP:
		{
			SetTextureStageState(pShader->uBumpStage, D3DTSS_BUMPENVMAT00, *((DWORD*)&DEFAULT_Du));
			SetTextureStageState(pShader->uBumpStage, D3DTSS_BUMPENVMAT01, 0 );
			SetTextureStageState(pShader->uBumpStage, D3DTSS_BUMPENVMAT10, 0 );
			SetTextureStageState(pShader->uBumpStage, D3DTSS_BUMPENVMAT11, *((DWORD*)&DEFAULT_Dv));
			SetTextureStageState(pShader->uBumpStage, D3DTSS_BUMPENVLSCALE, *((DWORD*)&DEFAULT_LSCALE));
			SetTextureStageState(pShader->uBumpStage, D3DTSS_BUMPENVLOFFSET, *((DWORD*)&DEFAULT_LOFFSET));
			break;
		}
		case SHADE_NEEDZ:
		{
			//SetRenderState(D3DRS_ZENABLE, (DWORD) TRUE);
			//Don't need to do anything as Z is enabled by default.
			//SetTextureStageState(0,D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

			break;
		}
		case SHADE_PROJ:
		{
			//If programmable vertex shaders are being used then the
			// D3DTTFF_COUNTX cannot be specified. Division is always done by the
			// last component when the D3DTTFF_PROJECTED is set.
			if ( m_Flags.Check(PROJECTED) )
			{
				if ( pShader->vstPrimType != VST_PROG )
				{
					SetTextureStageState(DEFAULT_PROJSTAGE, D3DTSS_TEXTURETRANSFORMFLAGS, 
										(DWORD) D3DTTFF_COUNT4 | D3DTTFF_PROJECTED );
				}
				else
				{
					SetTextureStageState(DEFAULT_PROJSTAGE, D3DTSS_TEXTURETRANSFORMFLAGS, 
										(DWORD) D3DTTFF_PROJECTED );
				}
			}
			break;
		}
		case SHADE_NOLORA:
		{
			pShader->Flags.Clear(SH_INVALIDFORMAT);
			if ( m_nTexFormat >= 0 && (pShader->byTexFormatsUsed & TEXFORMAT) )
			{
				if ( (m_pTexFormats[m_nTexFormat].ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
						(m_pTexFormats[m_nTexFormat].ddpfPixelFormat.dwFlags & PF_ALPHA)  )
				{
					pShader->Flags.Set(SH_INVALIDFORMAT);
				}
			}

			if ( m_nBumpFormat >= 0 && (pShader->byTexFormatsUsed & BUMPFORMAT) )
			{     
				if ( m_pBumpFormats[m_nBumpFormat].ddpfPixelFormat.dwFlags & PF_BUMPLUMINANCE )
				{
					pShader->Flags.Set(SH_INVALIDFORMAT);
				}
			}
			break;
		}
		case SHADE_PLAIN_NEEDMIPMAP:
		{
			SetTextureStageState(0, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_POINT);
			break;
		}
	}

	SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);
	SetRenderState(D3DRS_CLIPPING, (DWORD) FALSE);
	SetRenderState(D3DRS_LIGHTING, (DWORD) FALSE);    
	SetRenderState(D3DRS_SRCBLEND, (DWORD) D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, (DWORD) D3DBLEND_INVSRCALPHA);

	// Private key to check the Query Wrap and Mip flag
	if (KeySet("ForceClamp"))
	{
		SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(0, D3DTSS_ADDRESSW, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(1, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(1, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(1, D3DTSS_ADDRESSW, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(2, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(2, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(2, D3DTSS_ADDRESSW, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(3, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(3, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(3, D3DTSS_ADDRESSW, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(4, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(4, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(4, D3DTSS_ADDRESSW, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(5, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(5, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(5, D3DTSS_ADDRESSW, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(6, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(6, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(6, D3DTSS_ADDRESSW, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(7, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(7, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(7, D3DTSS_ADDRESSW, (DWORD)D3DTADDRESS_CLAMP);

		SetTextureStageState(0, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);
		SetTextureStageState(1, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);
		SetTextureStageState(2, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);
		SetTextureStageState(3, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);
		SetTextureStageState(4, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);
		SetTextureStageState(5, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);
		SetTextureStageState(6, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);
		SetTextureStageState(7, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);
	}

	if (!ValidateDevice(&dwPasses))
	{
		if (m_dwVersion < 0x0900)
		{
			// On DX8, there's no way to determine if the problem is with
			// the filtering model selected. Switch to point filtering and
			// try again. If it continues to fail, then continue with old
			// behavior and fail.

			for (DWORD dwCurrentTexIndex = 0;  dwCurrentTexIndex < dwMaxSampler; ++dwCurrentTexIndex)
			{
				SetTextureStageState(dwCurrentTexIndex, D3DTSS_MINFILTER, (DWORD)D3DTEXF_POINT);
				SetTextureStageState(dwCurrentTexIndex, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_POINT);
			}

			if (ValidateDevice(&dwPasses))
			{ 
				GetLastError();
				return true;
			}
		}


		WriteToLog("Validate Device failed! Src requires %d passes:  %s.\n",
						dwPasses, GLE(GetLastError()));
		return false;
	}

	return true;
}


bool CPixelShader::ExecuteTest(UINT uiTest)
{
	TCHAR sztBuffer[BUFFER_LENGTH], sztBump[BUFFER_LENGTH];
	TCHAR sztCube[BUFFER_LENGTH], sztTex[BUFFER_LENGTH], sztVol[BUFFER_LENGTH];
	UINT uiTestsPerVersion;
	CFlags FormatsUsed, FormatsChanged;
	int nTestFormat;

	//Shader changes for each test
	if (0 == m_vShaders.size())
	{
		SetTestRange(1, 1);
		return false;
	}

	//if we change direction of the test then reset the shader texture formats
	//so that they will be redrawn each time we step over them.
	if ( m_uiLastTest <= uiTest && m_bLastForward )
	{
		m_bLastForward = false;
		ResetAllShaderTexFormats();
	}
	else if ( m_uiLastTest >= uiTest && !m_bLastForward )
	{
		m_bLastForward = true;
		ResetAllShaderTexFormats();
	}
	m_uiLastTest = uiTest;
	m_dwCurrentShader = (uiTest-1) % m_vShaders.size();
	uiTestsPerVersion = m_uTotalFormats*m_vShaders.size();
	m_dwCurrentVersion = (uiTest-1) / uiTestsPerVersion;
    
	//Its important to increment this here so that if we exit before the formats
	//are check i.e. if we are not testing this version, the count for shaders
	//not rendered will relate to the size of the shader array.
	if ( 0 == m_dwCurrentShader && m_uiShaderNotRendered < m_vShaders.size() )
		m_uiShaderNotRendered = 1; //Simply reset the count
	else
		m_uiShaderNotRendered++;

	if (m_vShaders[m_dwCurrentShader]->dwRequired != m_vShaderVersions[m_dwCurrentVersion])
	{
		if (m_vShaderVersions[m_dwCurrentVersion] < PSVERSION(1, 4) &&
			m_vShaders[m_dwCurrentShader]->dwRequired < m_vShaderVersions[m_dwCurrentVersion])
		{
			//This is done because all 1.0, 1.1 and 1.2 shaders are valid 1.3 shaders.
			//This is not true for 1.4
			m_vShaders[m_dwCurrentShader]->dwRequired = m_vShaderVersions[m_dwCurrentVersion];
			ReCreateShaderAndBuffers(m_vShaders[m_dwCurrentShader]);
		}
		else
		{
			//These shaders will be tested when we get to there version.
			// This way version ordering is always 1.0 -> 1.1 ..... etc.
			return false;
		}
	}
	// Check if we need to skip 255 shader
	if (MAGIC_SHADER == m_vShaders[m_dwCurrentShader]->dwRequired && !(m_vShaders[m_dwCurrentShader]->bSetup))
	{
		SkipTests(1);
		return false;
	}

	//Select a non-bump format to test
	nTestFormat = ((uiTest-1) / m_vShaders.size()) % m_uPSCommonTexFormats;

	if ( m_nTexFormat != nTestFormat )
	{
		int nCycles; 
		m_nTexFormat = nTestFormat;
		m_nBumpFormat = m_nCubeFormat = m_nVolFormat = -1; //Reset all others;
        		
		//Need to change the texture format!
		//Clear all possible texture stages.
		for (DWORD i = 0;  i < 8; ++i)
			SetTexture(i, NULL);
		ReleaseTextures();
		
		//Assume that we will always have common standard texture formats.
		nCycles = m_vShaders.size() * m_uPSCommonTexFormats;
		if (m_uPSCommonBumpFormats)
		{
			m_nBumpFormat = ((uiTest-1) / nCycles) % m_uPSCommonBumpFormats;
			nCycles *= m_uPSCommonBumpFormats;
		}
		if (m_uPSCommonCubeFormats)
		{            
			m_nCubeFormat = ((uiTest-1) / nCycles) % m_uPSCommonCubeFormats;
			nCycles *= m_uPSCommonCubeFormats;
		}
		if (m_uPSCommonVolFormats)
		{            
			m_nVolFormat = ((uiTest-1) / nCycles) % m_uPSCommonVolFormats;
		}
		m_bFormatChanged = true;
	}

	//////////////////////////////////////////
	////This is the time to skip problematic test cases

	// On Dx9c, need to check for UsageQueryWrapAndMip on certain shader cases
	if ((m_pD3D->GetRuntimeVersion() >= 9.029f) && m_vShaders[m_dwCurrentShader]->bWrapAndMip)
	{
		HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
												USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, m_pTexFormats[m_nTexFormat].d3dfFormat);
		if (FAILED(Result))
			return false;
	}

	// Check a special key to help debug failures
	if (!KeySet("CubeLookup"))
	{

		if (  (m_vShaders[m_dwCurrentShader]->byTexFormatsUsed & CUBEFORMAT) &&
				((_tcsnccmp(m_vShaders[m_dwCurrentShader]->pName, _T("Address: texm3x3tex"), _tcslen(_T("Address: texm3x3tex"))) == 0) ||
				(_tcsnccmp(m_vShaders[m_dwCurrentShader]->pName, _T("Address: Arbitrary"), _tcslen(_T("Address: Arbitrary"))) == 0)   ||
				(_tcsnccmp(m_vShaders[m_dwCurrentShader]->pName, _T("Address: texreg2rgb_bx2"), _tcslen(_T("Address: texreg2rgb_bx2"))) == 0))
			)
		{
			// Need at least 3 channels to do a cubemap lookup.
			if (m_vShaders[m_dwCurrentShader]->byTexFormatsUsed & TEXFORMAT)
			{
				if ( (m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_A8)     ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_G16R16) ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_L8)     ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_A8L8)   ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_A4L4)   ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_L16)    ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_R16F)   ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_G16R16F)||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_R32F)   ||
						(m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_G32R32F)    
					)
				{
					return false;
				}
			}
		}

	}

	if (_tcsnccmp(m_vShaders[m_dwCurrentShader]->pName, _T("pow"), _tcslen(_T("pow"))) == 0)
	{
		// Components without an 'r' value cause a divide by zero.
		if ((m_pTexFormats[m_nTexFormat].d3dfFormat == FMT_A8))
		{
			return false;
		}
	}

	////////////////////////////////////////


	sprintf(sztBuffer,"%s:%d.%d, ",m_vShaders[m_dwCurrentShader]->pName, 
									D3DSHADER_VERSION_MAJOR(m_vShaderVersions[m_dwCurrentVersion]),
									D3DSHADER_VERSION_MINOR(m_vShaderVersions[m_dwCurrentVersion]));

	//Set this equal to all formats.
	FormatsUsed.Replace(TEXFORMAT | BUMPFORMAT | CUBEFORMAT | VOLUMEFORMAT);
	//set this equal to no formats.
	FormatsChanged.Replace(0);
	//Now check if any of the formats that this shader uses have changed.
	if ( (m_vShaders[m_dwCurrentShader]->byTexFormatsUsed & TEXFORMAT) && m_nTexFormat >= 0 )
	{
		if ( m_vShaders[m_dwCurrentShader]->d3dfTexFormat != m_pTexFormats[m_nTexFormat].d3dfFormat )
		{       
			FormatsChanged.Set(TEXFORMAT);
			m_vShaders[m_dwCurrentShader]->d3dfTexFormat = (FMT) m_pTexFormats[m_nTexFormat].d3dfFormat;
		}
		FmtToString(FORMAT_TEXTURE, &m_pTexFormats[m_nTexFormat], sztTex);
		sprintf(sztBuffer,"%sTex:%s ", sztBuffer, sztTex);        
	}
	else if ( m_nTexFormat <= 0)
	{
		//Now its a format that we dont care about.
		//in this case we only want to say that the format is unused if it is 
		//the first format.
		FormatsUsed.Clear(TEXFORMAT);
		//This should not occur if it does then the flags are set incorrectly
		assert( m_vShaders[m_dwCurrentShader]->d3dfTexFormat == FMT_UNKNOWN );
	}
	//bump
	if ( (m_vShaders[m_dwCurrentShader]->byTexFormatsUsed & BUMPFORMAT) &&  m_nBumpFormat >= 0 )
	{
		if ( m_vShaders[m_dwCurrentShader]->d3dfBumpFormat != m_pBumpFormats[m_nBumpFormat].d3dfFormat )
		{
			FormatsChanged.Set(BUMPFORMAT);
			m_vShaders[m_dwCurrentShader]->d3dfBumpFormat = (FMT) m_pBumpFormats[m_nBumpFormat].d3dfFormat;
		}
		FmtToString(FORMAT_TEXTURE, &m_pBumpFormats[m_nBumpFormat], sztBump);
		sprintf(sztBuffer,"%sBump:%s ", sztBuffer, sztBump);
	}
	else if ( m_nBumpFormat <= 0)
	{        
		FormatsUsed.Clear(BUMPFORMAT);
		//This should not occur if it does then the flags are set incorrectly.
		assert(m_vShaders[m_dwCurrentShader]->d3dfBumpFormat == FMT_UNKNOWN);
	}
	//cube
	if ( (m_vShaders[m_dwCurrentShader]->byTexFormatsUsed & CUBEFORMAT) && m_nCubeFormat >= 0)
	{
		if ( m_vShaders[m_dwCurrentShader]->d3dfCubeFormat != m_pCommonCubeFormats[m_nCubeFormat].d3dfFormat )
		{
			FormatsChanged.Set(CUBEFORMAT);
			m_vShaders[m_dwCurrentShader]->d3dfCubeFormat = (FMT) m_pCommonCubeFormats[m_nCubeFormat].d3dfFormat;
		}
		FmtToString(FORMAT_TEXTURE, &m_pCommonCubeFormats[m_nCubeFormat], sztCube);
		sprintf(sztBuffer,"%sCube:%s ", sztBuffer, sztCube);
	}
	else if ( m_nCubeFormat <= 0)
	{
		FormatsUsed.Clear(CUBEFORMAT);
		//This should not occur if it does then the flags are set incorrectly.
		assert(m_vShaders[m_dwCurrentShader]->d3dfCubeFormat == FMT_UNKNOWN);
	}
	//volume
	if ( (m_vShaders[m_dwCurrentShader]->byTexFormatsUsed & VOLUMEFORMAT) && m_nVolFormat >= 0)
	{
		if ( m_vShaders[m_dwCurrentShader]->d3dfVolFormat != m_pCommonVolumeFormats[m_nVolFormat].d3dfFormat )
		{
			FormatsChanged.Set(VOLUMEFORMAT);
			m_vShaders[m_dwCurrentShader]->d3dfVolFormat = (FMT) m_pCommonVolumeFormats[m_nVolFormat].d3dfFormat;
		}
		FmtToString(FORMAT_TEXTURE, &m_pCommonVolumeFormats[m_nVolFormat], sztVol);
		sprintf(sztBuffer,"%sVol:%s ", sztBuffer, sztVol);
	}
	else if ( m_nVolFormat <= 0)
	{
		FormatsUsed.Clear(VOLUMEFORMAT);
		//This should not occur if it does then the flags are set incorrectly.
		assert(m_vShaders[m_dwCurrentShader]->d3dfVolFormat == FMT_UNKNOWN);
	}

	//This means that no changes have occured to the formats used by this shader
	//There is no point running the shader again.
	//This will not cause a failure in the test.
	if ( ( m_vShaders[m_dwCurrentShader]->byTexFormatsUsed != FormatsUsed.Get() ) || 
			!( m_vShaders[m_dwCurrentShader]->byTexFormatsUsed & FormatsChanged.Get() ) )
	{        
		if ( (m_uiShaderNotRendered-1) >= m_vShaders.size() )
		{
			UINT uiNextBump = 0, uiNextCube = 0, uiNextVol = 0;
			UINT uiBumpCycle = 0, uiCubeCycle = 0, uiVolCycle = 0;
			int nCycles;
			//Skip to next change of texture format boundary
			nCycles = m_vShaders.size() * m_uPSCommonTexFormats;
			if (m_uPSCommonBumpFormats)
			{
				uiNextBump =  ( (int)((uiTest-1) / nCycles) ) * nCycles;
				uiNextBump += nCycles;
				uiBumpCycle = nCycles; 
				nCycles *= m_uPSCommonBumpFormats;
			}
			if (m_uPSCommonCubeFormats)
			{            
				uiNextCube = ( (int)((uiTest-1) / nCycles) )  * nCycles;
				uiNextCube += nCycles;
				uiCubeCycle = nCycles;
				nCycles *= m_uPSCommonCubeFormats;
			}
			if (m_uPSCommonVolFormats)
			{            
				uiNextVol = ( (int)((uiTest-1) / nCycles) )  * nCycles;
				uiNextVol += nCycles;
				uiVolCycle = nCycles;
			}
            
			if ( (m_uiShaderNotRendered-1) >= ( nCycles + m_vShaders.size() ) )
			{
				//Test is over skip to next version
				SkipTests( ( (m_dwCurrentVersion+1) * m_uTotalFormats * m_vShaders.size() ) - uiTest );
				m_uiShaderNotRendered = 0;
			}
			else if ( uiVolCycle && 
					( (m_uiShaderNotRendered-1) >= ( uiCubeCycle + m_vShaders.size() ) ) &&
					( m_uiShaderNotRendered < uiVolCycle ) )
			{
//                SkipTests(uiNextVol - uiTest);
				m_uiShaderNotRendered = uiVolCycle;
			}
			else if ( uiCubeCycle && 
					( (m_uiShaderNotRendered-1) >= ( uiBumpCycle + m_vShaders.size() ) ) &&
					( m_uiShaderNotRendered < uiCubeCycle ) )
			{
//                SkipTests(uiNextCube - uiTest);
				m_uiShaderNotRendered = uiCubeCycle;
			}
			else if ( uiBumpCycle && 
					( (m_uiShaderNotRendered-1) >= m_vShaders.size() ) &&
					( m_uiShaderNotRendered < uiBumpCycle ) )
			{
//                SkipTests(uiNextBump - uiTest);
				m_uiShaderNotRendered = uiBumpCycle;
			}
		}
		return false;
	}
	else
	{
		m_uiShaderNotRendered = 0;
	}

	BeginTestCase(sztBuffer, uiTest);

	if (MAGIC_SHADER != m_vShaders[m_dwCurrentShader]->dwRequired)
	{
		if (m_vShaders[m_dwCurrentShader]->dwRequired > m_dwSupported)
		{
			WriteToLog("(SKIP) Shader version %d.%d required, %d.%d supported.\n",
						D3DSHADER_VERSION_MAJOR(m_vShaders[m_dwCurrentShader]->dwRequired),
						D3DSHADER_VERSION_MINOR(m_vShaders[m_dwCurrentShader]->dwRequired),
						D3DSHADER_VERSION_MAJOR(m_dwSupported),
						D3DSHADER_VERSION_MINOR(m_dwSupported));
			SkipTests(1);
			return false;
		}

		if (!(m_vShaders[m_dwCurrentShader]->bSetup))
		{
			WriteToLog("(FAIL) Shader setup failed.\n");
			Fail();
			return false;
		}
	}
  
	if ( m_bFormatChanged )
	{	
		if ( !LoadTextures() )
		{
			WriteToLog(_T("Failed to load textures.\n") );
			Fail();
			ReleaseTextures();
			return false;
		}
		m_bFormatChanged = false;
	}

	if (!SetupBasicScene())
	{
		WriteToLog("(FAIL) Basic scene setup failed.\n");
		Fail();
		return false;
	}

	if ( m_vShaders[m_dwCurrentShader]->Flags.Check(SH_INVALID) ||
			m_vShaders[m_dwCurrentShader]->Flags.Check(SH_INVALIDFORMAT) )
	{
		SetCompareSrcRef(false);
		WriteStatus(_T(""), _T("$g**INVALID PARAMETER TEST **"));
	}
	else
	{
		SetCompareSrcRef(true);
		WriteStatus(_T(""), _T(""));
	}

	return true;
}


void CPixelShader::SceneRefresh()
{
	assert(m_vShaders.size() > 0);
	MYSHADER * pShader = m_vShaders[m_dwCurrentShader];

	if (BeginScene())
	{
		if (pShader->vstPrimType == VST_RENDERPRIM)
		{
			if (!RenderPrimitive(D3DPT_TRIANGLESTRIP, pShader->dwVertexFormat, pShader->pVertices, 4, NULL, 0, NULL))
			{
				WriteToLog(_T("RenderPrimitive failed.\n"));
				Fail();
			}
			
			// We need to draw two primitive for this type of shader test.
			if (pShader->Type == SHADE_NEEDZ)
			{
				//Turn off any current pixel shader
				SetPixelShader((DWORD)NULL);
				if (!RenderPrimitive(D3DPT_TRIANGLESTRIP, pShader->dwVertexFormat, ((BYTE*)pShader->pVertices) + (4 * pShader->dwVertexFormatSize), 4, NULL, 0, NULL))
				{
					WriteToLog(_T("RenderPrimitive failed.\n"));
					Fail();
				}
			}
		}
		else // (pShader->vstPrimType != VST_RENDERPRIM)
		{
			// This can be used if we create a vertex shader
			if (!DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2))
			{ 
				WriteToLog(_T("(ERROR) DrawPrimitive on the source failed. (%s)\n"), GLE(GetLastError()));
				Fail();
			}			

			// We need to draw two primitive for this type of shader test.
			if (pShader->Type == SHADE_NEEDZ)
			{
				//Turn off any current pixel shader
				SetPixelShader((DWORD)NULL);
				if (!DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2))
				{ 
					WriteToLog(_T("(ERROR) DrawPrimitive on the source failed. (%s)\n"), GLE(GetLastError()));
					Fail();
				}
			}
		}		

		if (!EndScene())
		{
			WriteToLog("(ERROR) EndScene() failed: (%08X)\n", GetLastError());
			Fail();
		}
	}
	else
	{
		WriteToLog("(ERROR) BeginScene() failed: (%08X)\n", GetLastError());
		Fail();
	}
}


bool CPixelShader::PreModeSetup()
{
	HKEY hRegKey;
	TCHAR ShaderRange[LINE_LENGTH];

	if ( m_pSrcDevice->GetCaps()->dwPixelShaderVersion > PSVERSION(0, 0) )
	{
		//Now we need to set the ref MaxPixelShaderValue as this defines the range of
		//values that can be used within a pixel shader.
		//This is an un documented reg key
		_stprintf(ShaderRange, "%f", m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue);
				
		if (SUCCEEDED ( RegCreateKeyEx(HKEY_LOCAL_MACHINE,
										_T("SOFTWARE\\Microsoft\\Direct3D\\ReferenceDevice"),
										0,
										_T(""),
										REG_OPTION_NON_VOLATILE,
										KEY_WRITE,
										NULL,
										&hRegKey,
										NULL) ) )
		{
			if (m_dwVersion >= 0x0900)
			{
				if ( FAILED ( RegSetValueEx(hRegKey, _T("PixelShader1xMaxValue"), 0, REG_SZ,  
								(CONST BYTE*) ShaderRange, 
								(_tcsclen(ShaderRange)+1)*sizeof(TCHAR) ) ) )
				{
					WriteToLog(_T("PShader: Unable to set Dx9 ref rast reg key ***Abort this test run***\n"));
					RegCloseKey(hRegKey);
					return false;
				}
			}
			else
			{
				if ( FAILED ( RegSetValueEx(hRegKey, _T("MaxPixelShaderValue"), 0, REG_SZ,  
								(CONST BYTE*) ShaderRange, 
								(_tcsclen(ShaderRange)+1)*sizeof(TCHAR) ) ) )
				{
					WriteToLog(_T("PShader: Unable to set Dx8 ref rast reg key ***Abort this test run***\n"));
					RegCloseKey(hRegKey);
					return false;
				}
			}

			RegCloseKey(hRegKey);
		}
		else
		{
			WriteToLog(_T("PShader: Unable to set ref rast reg key ***Abort this test run***\n"));
			return false;
		}
	}
	return true;
}


void CPixelShader::CleanUpReg(void)
{
	HKEY hRegKey;

	if (SUCCEEDED (RegOpenKeyEx(HKEY_LOCAL_MACHINE,								// handle to open key
						_T("SOFTWARE\\Microsoft\\Direct3D\\ReferenceDevice"),	// subkey name
						0,														// reserved
						KEY_ALL_ACCESS,											// security access mask
						&hRegKey) )	)											// handle to open key
	{		
		if (m_dwVersion >= 0x0900)
		{
			if ( FAILED  ( RegDeleteValue(hRegKey,				// handle to open key
							_T("PixelShader1xMaxValue") ) ) )		// value name
			{
				WriteToLog(_T("PShader: Unable to restore Dx9 ref rast reg key\n"));
			}
		}
		else
		{
			if ( FAILED  ( RegDeleteValue(hRegKey,				// handle to open key
							_T("MaxPixelShaderValue") ) ) )		// value name
			{
				WriteToLog(_T("PShader: Unable to restore Dx8 ref rast reg key\n"));
			}
		}
		RegCloseKey(hRegKey);
	}
	return;
}


bool CPixelShader::DoCapsCheck()
{	
	HRESULT		hr;
	CAPS D3DRawCapsSrc, D3DRawCapsRef; // raw device caps

	// Check if caps from 2 GetDeviceCaps() functions are in agreement
	// Source device
	hr = m_pD3D->GetDeviceCaps(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), &D3DRawCapsSrc);
	if (FAILED(hr))
	{
		WriteToLog("PShader: GetDeviceCaps() call failed\n");
		return false;
	}
	else
	{
		if (D3DRawCapsSrc.PixelShaderVersion != m_pSrcDevice->GetCaps()->dwPixelShaderVersion)
		{
			WriteToLog("PShader: Caps are not consistent on source device\n");
			return false;
		}
	}

	// Reference device
	hr = m_pD3D->GetDeviceCaps(m_pRefDevice->GetAdapterID(), m_pRefDevice->GetDevType(), &D3DRawCapsRef);
	if (FAILED(hr))
	{
		WriteToLog("PShader: GetDeviceCaps() call failed\n");
		return false;
	}
	else
	{
		if (D3DRawCapsRef.PixelShaderVersion != m_pRefDevice->GetCaps()->dwPixelShaderVersion)
		{
			WriteToLog("PShader: Caps are not consistent on reference device\n");
			return false;
		}
	}
	
	m_dwSupported = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
	WriteToLog( _T("Pixel shader version:  %8X\n"), m_dwSupported);
	WriteToLog( _T("Pixel shader version:  %d.%d\n"), D3DSHADER_VERSION_MAJOR(m_dwSupported),
													D3DSHADER_VERSION_MINOR(m_dwSupported));
	if ( FLT_MAX == m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue )
		WriteToLog( _T("Pixel shader max value: FLT_MAX\n"));
	else
		WriteToLog( _T("Pixel shader max value: %f\n"), m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue);

	if (0xFFFF0000 != (m_dwSupported & 0xFFFF0000))
	{
		WriteToLog( _T("ERROR: High word of version (%4X) should be FFFF.\n"),
					(m_dwSupported >> 16));
	}

	if ((D3DSHADER_VERSION_MAJOR(m_dwSupported) < 1))
	{
		WriteToLog( _T("Minimum pixel shader version of 1.0 required for this test.\n") );
		return false;
	}
	else if (MAGIC_SHADER == m_dwSupported)
	{
		WriteToLog( _T("Pixel shader version of 255.255 are not allowed.\n") );
		return false;
	}
	else if ( m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue < 1)
	{
		//Since we now know that the device has a major pixel shader version of > 1 and !255 and !1.4
		//we need to make sure that MaxPixelShaderValue is > 0.
		WriteToLog(_T("MaxPixelShaderValue must be >= 1.0.\n"));
		return false;
	}

	//Need to check for cubmap support 
	//If cubemaps are not supported the we need to fail.
	if ( !(m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP) )
	{
		WriteToLog(_T("TextureCaps_CubeMap not set, this is required for pixel shaders.\n"));
		return false;
	}

	//Need to check for volume texture capability
	if ( m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP )
		m_Flags.Set(VOLUMECAP);
	else
		m_Flags.Clear(VOLUMECAP);

	//Need to check the D3DPTEXTURECAPS_PROJECTED cap
	if ( m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_PROJECTED )
		m_Flags.Set(PROJECTED);
	else
		m_Flags.Clear(PROJECTED);

	if (!m_Flags.Check(NOCHECKVER))
	{
		if ( m_dwSupported != PSVERSION(1, 0) && m_dwSupported != PSVERSION(1, 1) &&
				m_dwSupported != PSVERSION(1, 2) && m_dwSupported != PSVERSION(1, 3) &&
				m_dwSupported != PSVERSION(1, 4) && m_dwSupported != PSVERSION(2, 0) &&
				m_dwSupported != PSVERSION(3, 0))
		{
				WriteToLog(_T("The Pixel Shader version being exposed was not\n"));
				WriteToLog(_T("defined when this test was released.\n"));
				WriteToLog(_T("The latest version of this test must be\n"));
				WriteToLog(_T("used to validate this driver.\n"));
				return false;
		}
	}

	//Need to check for HW_TRANS
	if ( m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		m_Flags.Set(HWTNL);
	else
		m_Flags.Clear(HWTNL);

	//Check for vertex shader support.
	if ( D3DRawCapsSrc.VertexShaderVersion >= VSVERSION(1, 0) )
		m_Flags.Set(HWVERTSHADER);
	else
		m_Flags.Clear(HWVERTSHADER);

	//Setup where the vertex buffers should be created.
	//Since the ref has a mixed behavior then set the refs vp 
	//to match the src.
	if ( (m_pSrcDevice->GetBehaviors() & DEVICECREATE_HWVP) ||
			(m_pSrcDevice->GetBehaviors() & DEVICECREATE_PURE)  )
	{
		m_pRefDevice->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, false);
		m_Flags.Set(VBHARDWARE);
	}
	else
	{        
		m_pRefDevice->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, true);
		m_Flags.Clear(VBHARDWARE);
	}      
	return true;
}


// NAME:        ProcessArgs()
// DESC:        process test arguments
// INPUT:       none
// OUTPUT:      none

void CPixelShader::ProcessArgs(void)
{
	TCHAR sztPSVersion[BUFFER_LENGTH], *ptcTemp;
	int nMajorVersion, nMinorVersion;

	m_bSpecificVerRequest = false;
	m_dwRequestedVersion = PSVERSION(0, 0);

	if (KeySet("NOCHECKVER"))
	{
		m_Flags.Set(NOCHECKVER);
	}
	//Get a version if it is specified
	ReadString(_T("Ver"), _T("0.0"), sztPSVersion, ARRAYSIZE(sztPSVersion));
	nMajorVersion = atoi(sztPSVersion);
	ptcTemp = _tcsstr(sztPSVersion, _T("."));
	nMinorVersion = atoi(&ptcTemp[1]);
	m_dwRequestedVersion = PSVERSION( nMajorVersion, nMinorVersion );

	if ( PSVERSION(0, 0) == m_dwRequestedVersion )
	{
		ReadString(_T("VerSpec"), _T("0.0"), sztPSVersion, ARRAYSIZE(sztPSVersion));
		nMajorVersion = atoi(sztPSVersion);
		ptcTemp = _tcsstr(sztPSVersion, _T("."));
		nMinorVersion = atoi(&ptcTemp[1]);
		m_dwRequestedVersion = PSVERSION( nMajorVersion, nMinorVersion );
		if ( m_dwRequestedVersion != PSVERSION(0, 0) )
			m_bSpecificVerRequest = true;
	}
	return;
}


//The following operator overload and SortShaderInfo
//functions are used to sort the shaders by version.
/*bool operator<(const MYSHADER& shaderx, const MYSHADER& shadery)
{
	return shaderx.dwRequired < shadery.dwRequired;
}
void CPixelShader::SortShaderInfo()
{
	std::stable_sort(m_vShaders.begin(), m_vShaders.end());
	return;
}*/


bool CPixelShader::SetTextureFormat(BYTE byFormat)
{    
	bool bNeedPal = false;

	switch (byFormat)
	{
		//This is the initialization case.
		case 0:            
			//Request all bump formats.
			if (m_byFormats & BUMPFORMAT)
			{
				SAFEDELETEA(m_pBumpFormats);
				if(!FilterFormats(m_pCommonTextureFormats, PF_BUMPDUDV | PF_BUMPLUMINANCE, 0, &m_pBumpFormats, &m_uPSCommonBumpFormats))
				{		            
					m_uPSCommonBumpFormats = 0;
					WriteToLog( _T("(WARNING) No common BumpDuDv formats.\n") );
					SAFEDELETEA(m_pBumpFormats);
					return false;
				}
				bNeedPal |= NeedPalette(m_uPSCommonBumpFormats, m_pBumpFormats);
			}

			//Request all cube formats.
			if (m_byFormats & CUBEFORMAT)
			{	
				// Use all cube formats.
				m_uPSCommonCubeFormats = m_uCommonCubeFormats;
				bNeedPal |= NeedPalette(m_uPSCommonCubeFormats, m_pCommonCubeFormats);
			}
			
			//Request all volume formats.
			if (m_byFormats & VOLUMEFORMAT)
			{	
				// Use all volume formats.
				m_uPSCommonVolFormats = m_uCommonVolumeFormats;
				bNeedPal |= NeedPalette(m_uPSCommonVolFormats, m_pCommonVolumeFormats);
			}

			// Request all other texture formats
			SAFEDELETEA(m_pTexFormats);			
			if(!FilterFormats(m_pCommonTextureFormats, PF_RGB | PF_DXTN | PF_YUV | PF_LUMINANCE | PF_ALPHA, 0, &m_pTexFormats, &m_uPSCommonTexFormats))
			{		            
				m_uPSCommonTexFormats = 0;
				WriteToLog("No texture formats found.\n");
				SAFEDELETEA(m_pTexFormats);
				return false;
			}
			bNeedPal |= NeedPalette(m_uPSCommonTexFormats, m_pTexFormats);

#ifdef FAST_TEST
			m_uTotalFormats = 1;
#else
			m_uTotalFormats = m_uPSCommonTexFormats;			
			if (m_uPSCommonBumpFormats)
			{
				m_uTotalFormats *= m_uPSCommonBumpFormats;
			}
			if (m_uPSCommonCubeFormats)
			{
				m_uTotalFormats *= m_uPSCommonCubeFormats;
			}
			if (m_uPSCommonVolFormats)
			{
				m_uTotalFormats *= m_uPSCommonVolFormats;
			}
#endif

			//Setup palettes for the devices these will be used by any palettized textures.    
			if (bNeedPal)
			{
				if ( !SetPaletteEntries(0, m_pSrcDevice->GetDefault8bitPalette() ) )
				{
					WriteToLog(_T("Failed to set texture palette entries."));
					return false;
				}
				if ( !SetCurrentTexturePalette(0) )
				{
					WriteToLog(_T("Failed to set current texture palette."));
					return false;
				}
			}
			break;

		case TEXFORMAT:
			if (m_nTexFormat >= m_uPSCommonTexFormats)
				return false;		
			//Set the format that will be used for texture creation
			m_pCurrentMode->nTextureFormat = m_nTexFormat;
			break;
        
		case BUMPFORMAT:
			if (m_nBumpFormat >= m_uPSCommonBumpFormats)
				return false;
			//This is just a sanity check for the test
			if (!(m_byFormats & BUMPFORMAT))
			{
				WriteToLog( _T("(ERROR) No common Bump Formats were requested.\n") );
				return false;
			}
			//Set the format that will be used for texture creation
			m_pCurrentMode->nTextureFormat = m_nBumpFormat;
			break;

		case CUBEFORMAT:
			if (m_nCubeFormat >= m_uPSCommonCubeFormats)
				return false;
			//This is just a sanity check for the test
			if (!(m_byFormats & CUBEFORMAT))
			{				
				WriteToLog( _T("(ERROR) No common Cube Formats were requested.\n") );
				return false;
			}			
			//Set the format that will be used for texture creation
			m_pCurrentMode->nCubeFormat = m_nCubeFormat;
			break;
		
		case VOLUMEFORMAT:
			if (m_nVolFormat >= m_uPSCommonVolFormats)
				return false;
			//This is just a sanity check for the test
			if (!(m_byFormats & VOLUMEFORMAT))
			{				
				WriteToLog( _T("(ERROR) No common Volume Formats were requested.\n") );
				return false;
			}			
			//Set the format that will be used for texture creation
			m_pCurrentMode->nVolumeFormat = m_nVolFormat;
			break;
		
		default:
			//This is bad should not enter here.
			assert(_T("Bad texture format requested"));
			return false;
	}

	return true;
}


bool CPixelShader::NeedPalette(UINT uNumFormats, FORMAT *pFormats)
{
	for(int i = 0; i < uNumFormats; ++i)
	{
		if ( pFormats[i].d3dfFormat == D3DFMT_A8P8 || 
				pFormats[i].d3dfFormat == D3DFMT_P8 )
			return true;
	}
	return false;
}


bool CPixelShader::GenerateBrickTexture(CImage *pImage)
{
	FColor	*Data = new FColor[64 * 64];
	int			i, j;

	// Fill with (0, 0, -1) - (128, 128, 0)
	for (i = 0; i < (64*64); i++)
		Data[i] = (256 << 24) | (128 << 16) | (128 << 8) | (0 << 0);

	// Make "up" lines (0, 0.5, -0.866025) - (128, 191, 17)
	for (i = 0; i < 8; i++)
		for (j = 0; j < 64; j++)
			Data[i*8*64 + j] = (256 << 24) | (128 << 16) | (191 << 8) | (17 << 0);

	// Make "down" lines (0, -0.5, -0.866025) - (0, 64, 17)
	for (i = 0; i < 8; i++)
		for (j = 0; j < 64; j++)
			Data[(i*8+7)*64 + j] = (256 << 24) | (128 << 16) | (64 << 8) | (17 << 0);

	// Make "left" lines (-0.5, 0, -0.866025) - (64, 128, 17)
	for (i = 0; i < 8; i++)
		for (j = 0; j < 64; j++)
			if ( ((i & 0x01) && j & 0x08) || ( 0 == (i & 0x01) && 0 == (j & 0x08)))
				Data[i*8 + j*64] = (256 << 24) | (64 << 16) | (128 << 8) | (17 << 0);

	// Make "right" lines (0.5, 0, -0.866025) - (191, 128, 17)
	for (i = 0; i < 8; i++)
		for (j = 0; j < 64; j++)
			if ( ((i & 0x01) && 0 == (j & 0x08)) || ( 0 == (i & 0x01) && (j & 0x08)))
				Data[(i*8+7) + j*64] = (256 << 24) | (191 << 16) | (128 << 8) | (17 << 0);

	if(false == pImage->SetData(64, 64, Data))
	{
		return false;
	}
	delete [] Data;
	return true;
}


// NAME:        fBumpDu()
// DESC:        bumpenvmap du
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fx = df/dx = cos(x)*sin(y)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-DEFAULT_BUMPS*pi/2,DEFAULT_BUMPS*pi/2]
//              [-1,1] maps to [-127,127] in LoadAlgorithms()

FLOAT CPixelShader::fBumpDu(FLOAT u, FLOAT v)
{
	FLOAT fTemp, x, y;

	fTemp = (FLOAT) (DEFAULT_BUMPS*pi); 
	x = fTemp*(u - 0.5f);
	y = fTemp*(v - 0.5f);
	return (FLOAT) (0.5f*(cos(x)*sin(y) + 1.0f));
}


// NAME:        fBumpDv()
// DESC:        bumpenvmap dv
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fy = df/dy = sin(x)*cos(y)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-DEFAULT_BUMPS*pi/2,DEFAULT_BUMPS*pi/2]
//              [-1,1] maps to [-127,127] in LoadAlgorithms()

FLOAT CPixelShader::fBumpDv(FLOAT u, FLOAT v)
{
	FLOAT fTemp, x, y;

	fTemp = (FLOAT) (DEFAULT_BUMPS*pi); 
	x = fTemp*(u - 0.5f);
	y = fTemp*(v - 0.5f);
	return (FLOAT) (0.5f*(sin(x)*cos(y) + 1.0f));
}


// NAME:        fBumpLuminance()
// DESC:        bumpenvmap luminance
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      f= 0.2*sin(x)*sin(y)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-DEFAULT_BUMPS*pi/2,DEFAULT_BUMPS*pi/2]
//              [-1,1] maps to [-127,127] in LoadAlgorithms()

FLOAT CPixelShader::fBumpLuminance(FLOAT u, FLOAT v)
{
	FLOAT fTemp, x, y;

	fTemp = (FLOAT) (DEFAULT_BUMPS*pi);
	x = fTemp*(u - 0.5f);
	y = fTemp*(v - 0.5f);
	return (FLOAT) (0.5f*(sin(x)*sin(y) + 1.0f));
}


// NAME:        GenerateBumpMap()
// DESC:        bumpenvmap luminance
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      f= 0.2*sin(x)*sin(y)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-DEFAULT_BUMPS*pi/2,DEFAULT_BUMPS*pi/2]
//              [-1,1] maps to [-127,127] in LoadAlgorithms()

bool CPixelShader::GenerateBumpMap(CImage* pImage)
{
	CHANNELFUNCS Data = { 0 };
	
	if (!pImage)
	{
		WriteToLog("Unable to create bumps image.\n");
		return false;
	}

	Data.fAlpha = fBumpLuminance;
	Data.fRed   = fBumpDu;
	Data.fGreen = fBumpDv;
	Data.fBlue  = fBumpLuminance;
 
	if (!(pImage->LoadAlgorithms(LARGE_TEXSIZE, LARGE_TEXSIZE, &Data)))
	{
		WriteToLog(_T("Unable to load bumps image."));
		return false;
	}
	return true;
}


void CPixelShader::ReleaseTextures()
{
	RELEASE(m_pDXLogoTex);
	RELEASE(m_pCheckerTex);
	RELEASE(m_pColorCheckerTex);
	RELEASE(m_pBrickTex);
	return;
}


void CPixelShader::ReleaseTextureData()
{
	//Since we use CImage rather that *CImage the class
	//handles the delete.
	return;
}


// NAME:        GenerateTextureData()
// DESC:        Loads a default set of texture image data for use by all types
//              of pixel shaders
// INPUT:       
// OUTPUT:      Data is loaded and ready for use
// COMMENTS:    This can be overloaded to load additional image data.
bool CPixelShader::GenerateTextureData()
{
	// Dx logo texture
	if ( !m_DXLogoImage.Load("DXLogo.bmp", 0.8f) )
	{
		WriteToLog("(ERROR) Failed to load DXLogo.bmp.  Aborting.\n");
		return false;
	}
	
	//black and white checker board.
	if ( !m_CheckerImage.Load("checker.bmp", 1.0f) )
	{
		WriteToLog("(ERROR) Failed to load checker.bmp.  Aborting.\n");
		return false;
	}

	//Colored checker board
	if ( !m_ColorCheckerImage.Load("colorchecker.bmp", 1.0f) )
	{
		WriteToLog("(ERROR) Failed to load Colorchecker.bmp.  Aborting.\n");
		return false;
	}

	// Bricks
	if ( !GenerateBrickTexture(&m_BrickImage) )
	{
		WriteToLog("(ERROR) Failed to generate brick data.  Aborting.\n");
		return false;
	}

	return true;
}


// NAME:        LoadTextures()
// DESC:        Loads a default set of textures for use by all types
//              of pixel shaders
// INPUT:       
// OUTPUT:      Textures loaded and ready for use.
// COMMENTS:    This can be overloaded to load additional textures.
bool CPixelShader::LoadTextures()
{
	if (!SetTextureFormat(TEXFORMAT))
		return false;
	DWORD dwLevels = (DWORD) 1;
	DWORD dwPool = POOL_DEFAULT;
	DWORD dwUsage = (DWORD) 0;
	FORMAT *pFormat = &m_pTexFormats[m_pCurrentMode->nTextureFormat];

	// Dx logo texture
	if (!CreateTexture(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pDXLogoTex))
	{
		WriteToLog("(ERROR) Failed to create logo texture.  Aborting.\n");
		return false;
	}
	if ( !m_pDXLogoTex->LoadImage(dwLevels, &m_DXLogoImage) )
	{
		WriteToLog("Unable to load logo texture.\n");
		return false;
	}

	//black and white checker board.
	if (!CreateTexture(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pCheckerTex))
	{
		WriteToLog("(ERROR) Failed to create checker texture.  Aborting.\n");
		return false;
	}
	if ( !m_pCheckerTex->LoadImage(dwLevels, &m_CheckerImage) )
	{
		WriteToLog("Unable to load checker texture.\n");
		return false;
	}

	//Colored checker board
	if (!CreateTexture(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pColorCheckerTex))
	{
		WriteToLog("(ERROR) Failed to create color checker texture.  Aborting.\n");
		return false;
	}
	if ( !m_pColorCheckerTex->LoadImage(dwLevels, &m_ColorCheckerImage) )
	{
		WriteToLog("Unable to load color checker texture.\n");
		return false;
	}

	// Bricks
	if (!CreateTexture(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pBrickTex))
	{
		WriteToLog("(ERROR) Failed to create brick texture.  Aborting.\n");
		return false;
	}
	if ( !m_pBrickTex->LoadImage(dwLevels, &m_BrickImage) )
	{
		WriteToLog("Unable to load bricks texture.\n");
		return false;
	}

	return true;
}
