#define D3D_OVERLOADS
#include <d3d.h>
#include "ColorKey.h"

//************************************************************************
// Internal API:    DrawGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CColorKeyTest::DrawGrid(int nTest)
{
	DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
	DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
	int	  nMapping;
	float fScale;

	/*
	 * Initialize the interesting variables
	 */
	nMapping = nTest % 4;

	if (nMapping == 0)
		nMapping = 4;

	fScale = 1.0f;

	switch (nMapping)
	{
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping, White Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping, White Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			break;
		case (4): // Use ((1,0)(1,1)(0,0)(0,1)) mapping, White Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			break;
	}
}
