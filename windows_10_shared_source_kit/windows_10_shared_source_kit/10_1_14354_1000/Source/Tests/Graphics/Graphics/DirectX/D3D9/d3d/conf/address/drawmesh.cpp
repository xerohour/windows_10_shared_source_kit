#define D3D_OVERLOADS
#include <d3d.h>
#include "Address.h"

//************************************************************************
// Internal API:    DrawGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CAddressTest::DrawGrid(int nTest)
{
	DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
	DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
	int nRot, nTile, nGroup;
	float fZero, fOne;
	bool bOverride = false;
	static DWORD dwDefault = 99;

	// Decide which group of tests we are in
	nGroup = nTest % 20;

	if (nGroup == 0)
		nGroup = 20;

	// Decide which rotation to use
	nRot = (nGroup-1) / 5;

	// Decide how much tiling to use
	nTile = nGroup - (nRot * 5);
 
	if (((nTest-1) / 20) == 0)
	{
		// Use (0,0) to (1,1) mapping
		fOne =  nTile * 1.0f;
		fZero = 0.0f;
	}
	else
	{
		// Use (-1,-1) to (1,1) mapping
		fOne =  nTile * 1.0f;
		fZero = nTile * -1.0f;
	}

	if (bBorder)
	{
		// Special case these formats when using border color...use white for formats that contain unused channels and A16B16G16R16
		if ((m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_G16R16)		|| 
			(m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_A16B16G16R16)||
			(m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_R16F)		||
			(m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_G16R16F)		||
			(m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_R32F)		||
			(m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_G32R32F))
		{
			bOverride = true;

			sprintf(msgString, "%sWhite, ",msgString);
			SetRenderState(D3DRENDERSTATE_BORDERCOLOR, RGBA_MAKE(255,255,255,255));
		}
	}

	// Setup the mesh
	switch (nRot)
	{
		case (0): // Use ((1,0)(1,1)(0,0)(0,1)) mapping, White Mesh
			if (bBorder && !bOverride)
			{
				if (dwDefault == 99)
				{
					if (m_dwVersion >= 0x0700)
						GetTextureStageState(0, D3DTSS_BORDERCOLOR, &dwDefault, CD3D_REF);
					else
						GetRenderState(D3DRENDERSTATE_BORDERCOLOR, &dwDefault, CD3D_REF);
				}

				sprintf(msgString, "%sDefault, ",msgString);
				SetRenderState(D3DRENDERSTATE_BORDERCOLOR, dwDefault);
			}

			if (fZero == 0.0f)
				sprintf(msgString, "%sMap: (%d,0)(%d,%d)(0,0)(0,%d), ",msgString,nTile,nTile,nTile,nTile);
			else
				sprintf(msgString, "%sMap: (%d,%d)(%d,%d)(%d,%d)(%d,%d), ",msgString,nTile,-nTile,nTile,nTile,-nTile,-nTile,-nTile,nTile);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			break;
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
			if (bBorder && !bOverride)
			{
				sprintf(msgString, "%sBlue, ",msgString);
				SetRenderState(D3DRENDERSTATE_BORDERCOLOR, RGBA_MAKE(0,0,255,255));
			}

			if (fZero == 0.0f)
				sprintf(msgString, "%sMap: (0,0)(%d,0)(0,%d)(%d,%d), ",msgString,nTile,nTile,nTile,nTile);
			else
				sprintf(msgString, "%sMap: (%d,%d)(%d,%d)(%d,%d)(%d,%d), ",msgString,-nTile,-nTile,nTile,-nTile,-nTile,nTile,nTile,nTile);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping, White Mesh
			if (bBorder && !bOverride)
			{
				sprintf(msgString, "%sRed, ",msgString);
				SetRenderState(D3DRENDERSTATE_BORDERCOLOR, RGBA_MAKE(255,0,0,255));
			}

			if (fZero == 0.0f)
				sprintf(msgString, "%sMap: (0,%d)(0,0)(%d,%d)(%d,0), ",msgString,nTile,nTile,nTile,nTile);
			else
				sprintf(msgString, "%sMap: (%d,%d)(%d,%d)(%d,%d)(%d,%d), ",msgString,-nTile,nTile,-nTile,-nTile,nTile,nTile,nTile,-nTile);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping, White Mesh
			if (bBorder && !bOverride)
			{
				sprintf(msgString, "%sGreen, ",msgString);
				SetRenderState(D3DRENDERSTATE_BORDERCOLOR, RGBA_MAKE(0,255,0,255));
			}

			if (fZero == 0.0f)
				sprintf(msgString, "%sMap: (%d,%d)(0,%d)(%d,0)(0,0), ",msgString,nTile,nTile,nTile,nTile);
			else
				sprintf(msgString, "%sMap: (%d,%d)(%d,%d)(%d,%d)(%d,%d), ",msgString,nTile,nTile,-nTile,nTile,-nTile,nTile,-nTile,-nTile);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne,fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			break;
	}
}

//************************************************************************
// Internal API:    DrawWrapGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CAddressTest::DrawWrapGrid(int nTest)
{
	DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
	DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
	float fZero, fOne;

	// Use (0,0) to (1,1) mapping
	fOne =  0.9f;
	fZero = 0.1f;

	// Setup the mesh
	switch (nTest)
	{
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
			sprintf(msgString, "%sMap: (0,0)(1,0)(0,1)(1,1), ",msgString);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping, White Mesh
			sprintf(msgString, "%sMap: (0,1)(0,0)(1,1)(1,0), ",msgString);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping, White Mesh
			sprintf(msgString, "%sMap: (1,1)(0,1)(1,0)(0,0), ",msgString);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			break;
		case (4): // Use ((1,0)(1,1)(0,0)(0,1)) mapping, White Mesh
			sprintf(msgString, "%sMap: (1,0)(1,1)(0,0)(0,1), ",msgString);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			break;
	}
}
