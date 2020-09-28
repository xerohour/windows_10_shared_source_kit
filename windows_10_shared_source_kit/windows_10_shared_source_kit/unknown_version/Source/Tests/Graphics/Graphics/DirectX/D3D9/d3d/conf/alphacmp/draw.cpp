#define D3D_OVERLOADS
#include <d3d.h>
#include "AlphaCmp.h"

//************************************************************************
// Internal API:    DrawTriangle
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CAlphaCmpTest::DrawTriangle(int nAlpha)
{
	if (bGouraud)
	{
		// Gouraud Green Triangle
//		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(140.0f, 10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,nAlpha-1),0, 0.0f,0.0f);
//		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(310.0f,270.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,nAlpha+1),0, 1.0f,0.0f);
//		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,270.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,nAlpha+1),0, 1.0f,1.0f);

		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(140.0f, 10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,0)  ,0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(310.0f,270.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, 1.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,270.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
	}
	else
	{
		// Flat Green Triangle
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(140.0f, 10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,nAlpha),0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(310.0f,270.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,nAlpha),0, 1.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,270.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,nAlpha),0, 1.0f,1.0f);
	}
}
