#define D3D_OVERLOADS
#include <d3d.h>
#include "TexLoad.h"

//************************************************************************
// Internal API:    DrawGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************
 
void CTexLoadTest::DrawGrid(float fMesh, bool bSingle)
{
	// Initialize the interesting variables
	fMesh += 10.0f;

	if (bSingle)
	{
		// Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, 10.0f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,1.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,0.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, 10.0f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, 10.0f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,1.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,1.0f,1.0f);
	}
	else
	{
		// Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, 10.0f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,1.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,0.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, 10.0f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, 10.0f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,1.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,1.0f,1.0f);

		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh+10.0f, fMesh+10.0f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh+fMesh, fMesh+fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,1.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh+10.0f, fMesh+fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,0.0f,1.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh+10.0f, fMesh+10.0f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,0.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh+fMesh, fMesh+10.0f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,1.0f,0.0f);
		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh+fMesh, fMesh+fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0,1.0f,1.0f);
	}
}

