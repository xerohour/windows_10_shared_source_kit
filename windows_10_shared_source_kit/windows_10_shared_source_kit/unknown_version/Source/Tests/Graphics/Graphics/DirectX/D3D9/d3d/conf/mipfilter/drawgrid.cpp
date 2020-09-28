#define D3D_OVERLOADS
#include <d3d.h>
#include "MipFilter.h"

//************************************************************************
// Internal API:    DrawGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************
 
void CMipFilterTest::DrawGrid(float fWidth, float fHeight)
{
	// Initialize the interesting variables
	float fScale = 1.0f;

	fWidth += 9.5f;
	fHeight += 9.5f;

	// Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,    9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,   0.0f,   0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fWidth, fHeight, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale, fScale);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f, fHeight, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,   0.0f, fScale);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,    9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,   0.0f,   0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fWidth,    9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,   0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fWidth, fHeight, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale, fScale);
}

