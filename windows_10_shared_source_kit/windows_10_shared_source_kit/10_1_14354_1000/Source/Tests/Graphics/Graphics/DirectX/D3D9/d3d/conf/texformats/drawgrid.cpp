#define D3D_OVERLOADS
#include <d3d.h>
#include "TexFormats.h"

void CTexFormatsTest::DrawGrid(int nTest)
{
	switch (nTest)
	{
		case (1):	// Use standard ((0,0)(1,0)(0,1)(1,1)) mapping	
			sprintf(msgString, "%sMap:(0,0)(1,0)(0,1)(1,1), ",msgString);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
			break;
		case (2):	// Use ((0,1)(0,0)(1,1)(1,0)) mapping
			sprintf(msgString, "%sMap:(0,1)(0,0)(1,1)(1,0), ",msgString);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
			break;
		case (3):	// Use ((1,1)(0,1)(1,0)(0,0)) mapping
			sprintf(msgString, "%sMap:(1,1)(0,1)(1,0)(0,0), ",msgString);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,0.0f);
			break;
		case (4):	// Use ((1,0)(1,1)(0,0)(0,1)) mapping
			sprintf(msgString, "%sMap:(1,0)(1,1)(0,0)(0,1), ",msgString);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,  9.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,265.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			break;
	}
}

void CTexFormatsTest::DrawFVFGrid(void)
{
}