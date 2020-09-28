#define D3D_OVERLOADS
#include <d3d.h>
#include "Gradient.h"

//************************************************************************
// Internal API:    DrawGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************
 
void CGradientTest::DrawGrid(int nTest)
{
	int		nScale, nMapping;
	float	fScale, fMesh;

	// Initialize the interesting variables
	nMapping = ((nTest-1) / 60) + 1;

	nScale = ((nTest-1) / 15) + 1;
	nScale = nScale % nMaxScale;

	if (nScale == 0)
		nScale = nMaxScale;
 
	fScale = 1.0f * nScale;

	fMesh = ((nTest % 15) + 1) * 16.0f;

	if (fMesh == 16.0f)
		fMesh = 256.0f;

	sprintf(msgString, "%sMesh:%dx%d, ",msgString,(int)fMesh,(int)fMesh);

	fMesh += 9.5f;

	switch (nMapping)
	{
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
			sprintf(msgString, "%sMap:(0,0)(%d,0)(0,%d)(%d,%d), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping, White Mesh
			sprintf(msgString, "%sMap:(0,%d)(0,0)(%d,%d)(%d,0), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping, White Mesh
			sprintf(msgString, "%sMap:(%d,%d)(0,%d)(%d,0)(0,0), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			break;
		case (4): // Use ((1,0)(1,1)(0,0)(0,1)) mapping, White Mesh
			sprintf(msgString, "%sMap:(%d,0)(%d,%d)(0,0)(0,%d), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh,  9.5f, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			break;
	}
}