#define D3D_OVERLOADS
#include <d3d.h>
#include "TexFormats.h"

#define FVF3_VERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0) )

//************************************************************************
// FVF3 Test functions

CFVF3Test::CFVF3Test()
{
	m_szTestName = "FVF3 Texture Formats";
	m_szCommandKey = "FVF3";

    // Init Framework options
    m_Options.D3DOptions.fMinDXVersion = 7.0f;

    // Tell parent that we a the FVF test
    bFVF = true;
}

UINT CFVF3Test::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}
 
#if FAST_TEST
    m_totalNumTestCases = m_uCommonTextureFormats;
    // Setup the Test range
    SetTestRange(1, 1);
#else
	// Setup the Test range
   	SetTestRange(1, m_uCommonTextureFormats);
#endif

	// Make sure we are on at lease DX7
	if (m_dwVersion <= 0x0600)
	{
		WriteToLog("This test requires at least Dx7.\n");
		return D3DTESTINIT_SKIPALL;
	}

     // Don't check caps if -invalid is given
    if (!KeySet("Invalid"))
    {
        // For DDI <= 7, need to check additional caps
        if (m_pSrcDevice->GetDDI() <= 7)
	    {
	        DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;

	        if (!(dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP) && 
                !(dwTextureCaps & D3DPTEXTURECAPS_PROJECTED))
	        {
		        WriteToLog("3D Tex coords not supported: Cubemap or Projected.\n");
		        return D3DTESTINIT_SKIPALL;
	        }
	    }
    }

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

void CFVF3Test::SceneRefresh(void)
{
#ifndef FAST_TEST
	if (BeginScene())
	{
		// Render the data.
	    RenderPrimitive(D3DPT_TRIANGLELIST,FVF3_VERTEX,FVF_VertexList,dwVertexCount,NULL,0,NULL);
 
		EndScene();
	}
#endif
}

#if FAST_TEST
void CFVF3Test::TransformAndDraw(float scaleX, float scaleY, float transX, float transY)
{
    for (size_t i = 0; i < dwVertexCount; i++)
    {
        FVF_VertexList[i].x *= scaleX;
        FVF_VertexList[i].x += transX;

        FVF_VertexList[i].y *= scaleY;
        FVF_VertexList[i].y += transY;
    }

    RenderPrimitive(D3DPT_TRIANGLELIST, FVF3_VERTEX, FVF_VertexList, dwVertexCount, NULL, 0, NULL);
}
#endif

void CFVF3Test::DrawFVFGrid(void)
{
	// Tell log what our FVF we are using
	sprintf(msgString, "%sUsing FVF: 0x%08x, ",msgString,FVF3_VERTEX);

	FVF_VertexList[0].x = 9.5f;
	FVF_VertexList[0].y = 9.5f;
	FVF_VertexList[0].z = 0.9f;
	FVF_VertexList[0].rhw = 1.0f / 0.9f;
	FVF_VertexList[0].color = 0xffffffff;
	FVF_VertexList[0].tu = 0.0f;
	FVF_VertexList[0].tv = 0.0f;
	FVF_VertexList[0].tw = 1.0f;

	FVF_VertexList[1].x = 265.5f;
	FVF_VertexList[1].y = 9.5f;
	FVF_VertexList[1].z = 0.9f;
	FVF_VertexList[1].rhw = 1.0f / 0.9f;
	FVF_VertexList[1].color = 0xffffffff;
	FVF_VertexList[1].tu = 1.0f;
	FVF_VertexList[1].tv = 0.0f;
	FVF_VertexList[1].tw = 0.5f;

	FVF_VertexList[2].x = 265.5f;
	FVF_VertexList[2].y = 265.5f;
	FVF_VertexList[2].z = 0.9f;
	FVF_VertexList[2].rhw = 1.0f / 0.9f;
	FVF_VertexList[2].color = 0xffffffff;
	FVF_VertexList[2].tu = 1.0f;
	FVF_VertexList[2].tv = 1.0f;
	FVF_VertexList[2].tw = 0.0f;

	FVF_VertexList[3].x = 9.5f;
	FVF_VertexList[3].y = 9.5f;
	FVF_VertexList[3].z = 0.9f;
	FVF_VertexList[3].rhw = 1.0f / 0.9f;
	FVF_VertexList[3].color = 0xffffffff;
	FVF_VertexList[3].tu = 0.0f;
	FVF_VertexList[3].tv = 0.0f;
	FVF_VertexList[3].tw = 1.0f;

	FVF_VertexList[4].x = 265.5f;
	FVF_VertexList[4].y = 265.5f;
	FVF_VertexList[4].z = 0.9f;
	FVF_VertexList[4].rhw = 1.0f / 0.9f;
	FVF_VertexList[4].color = 0xffffffff;
	FVF_VertexList[4].tu = 1.0f;
	FVF_VertexList[4].tv = 1.0f;
	FVF_VertexList[4].tw = 0.5f;

	FVF_VertexList[5].x = 9.5f;
	FVF_VertexList[5].y = 265.5f;
	FVF_VertexList[5].z = 0.9f;
	FVF_VertexList[5].rhw = 1.0f / 0.9f;
	FVF_VertexList[5].color = 0xffffffff;
	FVF_VertexList[5].tu = 0.0f;
	FVF_VertexList[5].tv = 1.0f;
	FVF_VertexList[5].tw = 0.0f;

    dwVertexCount = 6;
}

