#define D3D_OVERLOADS
#include <d3d.h>
#include "Fog.h"

//************************************************************************
// Vertex Test functions

CVertexTest::CVertexTest()
{
	m_szTestName = "Vertex Fog";
	m_szCommandKey = "Vertex";

	// Tell parent class which Fog we are
	dwFog = VERTEX;

	// Don't request any texture formats
	m_Options.ModeOptions.dwTexInclude = 0;
}

bool CVertexTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Vertex Fog
	if (!bCapAbort)
	{
		SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_NONE);
	}
	return true;
}

UINT CVertexTest::TestInitialize(void)
{
#if FAST_TEST
    m_totalNumTestCases = VERTEX_TESTS;
    SetTestRange(1, 1);
#else
    // Setup the Test range
	SetTestRange(1, VERTEX_TESTS);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Black
	FogColor = RGB_MAKE(0,0,0);

    return D3DTESTINIT_RUN;
}

//************************************************************************
// Texture Vertex Test functions

CTextureVertexTest::CTextureVertexTest()
{
	m_szTestName = "Textured Vertex Fog";
	m_szCommandKey = "TextureVertex";

	// Tell parent class which Fog we are
	dwFog = VERTEX;

#if FAST_TEST
    m_Options.ModeOptions.TextureFmt = FMT_A8R8G8B8;
#else
	// Request all texture formats except bumpmap
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE;
	m_Options.ModeOptions.dwTexExclude = PF_METN;
#endif
}

bool CTextureVertexTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Vertex Fog
	if (!bCapAbort)
	{
		SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_NONE);
		SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD) D3DTBLEND_DECAL);
	}
	return true;
}

UINT CTextureVertexTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#if FAST_TEST
    m_totalNumTestCases = TEXTURE_VERTEX_TESTS * m_uCommonTextureFormats;
    SetTestRange(1, 1);
#else
    // Setup the Test range
	SetTestRange(1, TEXTURE_VERTEX_TESTS * m_uCommonTextureFormats);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Black
	FogColor = RGB_MAKE(0,0,0);

	// Setup the colors for the Stripes array
	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

    return D3DTESTINIT_RUN;
}

//************************************************************************
// Internal API:    VertexTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CFogTest::VertexTests(int nTest)
{
	int			FogValue[] = { 255, 128, 0 };
	int			nFog;
	D3DCOLOR	Color = RGBA_MAKE(0,0,0,255);

	// Calculate the Fog Value
	nFog = nTest % 4;

	if (nTest <= 4)
	{
		// Black Fog
		FogColor = RGB_MAKE(0, 0, 0);

		// White Triangle
		Color = RGBA_MAKE(255,255,255,255);

		// Update the logfile
		sprintf(msgString, "%sFog: Black, Tri: White",msgString);
	}
	else if (nTest <= 8)
	{
		// Green Fog
		FogColor = RGB_MAKE(0, 255, 0);

		// White Triangle
		Color = RGBA_MAKE(255,255,255,255);

		// Update the logfile
		sprintf(msgString, "%sFog: Green, Tri: White",msgString);
	}
	else
	{
		// White Fog
		FogColor = RGB_MAKE(255, 255, 255);

		// Red Triangle
		if (nTest == 9)
		{
			Color = RGBA_MAKE(255,0,0,255);

			// Update the logfile
			sprintf(msgString, "%sFog: White, Tri: Red",msgString);
		}
		// Green Triangle
		else if (nTest == 10)
		{
			Color = RGBA_MAKE(0,255,0,255);

			// Update the logfile
			sprintf(msgString, "%sFog: White, Tri: Green",msgString);
		}
		// Blue Triangle
		else if (nTest == 11)
		{
			Color = RGBA_MAKE(0,0,255,255);

			// Update the logfile
			sprintf(msgString, "%sFog: White, Tri: Blue",msgString);
		}
	}

	if (nFog)
	{
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.5f,  5.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.5f,  5.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 1.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.5f,245.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 1.0f,1.0f);

		// Update the logfile
		sprintf(msgString, "%s, Fog Values: %d, %d, %d",msgString, FogValue[nFog-1], FogValue[nFog-1], FogValue[nFog-1]);
	}
	else
	{
		if (nTest == 12)
		{
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.5f,  5.5f,0.1f),(1.0f / 0.1f), RGBA_MAKE(255,0,0,255), RGBA_MAKE(0, 0, 0, FogValue[2]), 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.5f,  5.5f,0.1f),(1.0f / 0.1f), RGBA_MAKE(0,255,0,255), RGBA_MAKE(0, 0, 0, FogValue[1]), 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.5f,245.5f,0.1f),(1.0f / 0.1f), RGBA_MAKE(0,0,255,255), RGBA_MAKE(0, 0, 0, FogValue[0]), 1.0f,1.0f);

			// Update the logfile
			sprintf(msgString, "%sFog: White, Tri: RGB, Fog Values: %d, %d, %d",msgString, FogValue[2], FogValue[1], FogValue[0]);
		}
		else
		{
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.5f,  5.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[2]), 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.5f,  5.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[1]), 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.5f,245.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[0]), 1.0f,1.0f);

			// Update the logfile
			sprintf(msgString, "%s, Fog Values: %d, %d, %d",msgString, FogValue[2], FogValue[1], FogValue[0]);
		}

	}
}

//************************************************************************
// Internal API:    TextureVertexTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CFogTest::TextureVertexTests(int nTest)
{
	int			FogValue[] = { 255, 128, 0 };
	int			nFog;
	D3DCOLOR	Color;

	// Calculate the Fog Value
	nFog = nTest % 4;

	// Green Fog
	FogColor = RGB_MAKE(0, 255, 0);

	// White Triangle
	Color = RGBA_MAKE(255,255,255,255);

	// Update the logfile
	sprintf(msgString, "%sFog: Green, Tri: White",msgString);

	if (nFog)
	{
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.5f,  5.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.5f,  5.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 1.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.5f,245.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 1.0f,1.0f);

		// Update the logfile
		sprintf(msgString, "%s, Fog Values: %d, %d, %d",msgString, FogValue[nFog-1], FogValue[nFog-1], FogValue[nFog-1]);
	}
	else
	{
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.5f,  5.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[2]), 0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.5f,  5.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[1]), 1.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(240.5f,245.5f,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[0]), 1.0f,1.0f);

		// Update the logfile
		sprintf(msgString, "%s, Fog Values: %d, %d, %d",msgString, FogValue[2], FogValue[1], FogValue[0]);
	}
}

