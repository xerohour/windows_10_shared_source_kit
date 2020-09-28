#define D3D_OVERLOADS
#include <d3d.h>
#include "anisotropic.h"

//************************************************************************
// Internal API:    DrawGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************
 
void CAnisoTest::DrawGrid(float fMesh, int nMap)
{
	// Initialize the interesting variables
#ifdef FAST_TEST
	float fZ0[MAX_GRIDS] = { 0.9f, 0.1f};
	float fZ1[MAX_GRIDS] = { 0.1f, 0.1f};
	float fZ2[MAX_GRIDS] = { 0.1f, 0.9f};
	float fZ3[MAX_GRIDS] = { 0.9f, 0.9f};
#else
    float fZ0[MAX_GRIDS] = {0.9f, 0.1f, 0.9f, 0.1f};
    float fZ1[MAX_GRIDS] = {0.1f, 0.1f, 0.9f, 0.9f};
    float fZ2[MAX_GRIDS] = {0.1f, 0.9f, 0.1f, 0.1f};
    float fZ3[MAX_GRIDS] = {0.9f, 0.9f, 0.1f, 0.9f};
#endif

	fMesh += 9.5f;

    // Tell log and status window what values we picked
	sprintf(msgString, "%sZval:(%.1f)(%.1f)(%.1f)(%.1f), ",msgString,fZ0[nMap],fZ1[nMap],fZ2[nMap],fZ3[nMap]);

    // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f,  9.5f, fZ0[nMap]),(1.0f / fZ0[nMap]), RGBA_MAKE(255,255,255,255),0, 0.0f,                                                       0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, fZ3[nMap]),(1.0f / fZ3[nMap]), RGBA_MAKE(255,255,255,255),0, fZ3[nMap] == 0.9f ? 1.0f : 0.0f, fZ3[nMap] == 0.9f ? 1.0f : 0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f, fMesh, fZ2[nMap]),(1.0f / fZ2[nMap]), RGBA_MAKE(255,255,255,255),0, 0.0f,                            fZ2[nMap] == 0.9f ? 1.0f : 0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 9.5f,  9.5f, fZ0[nMap]),(1.0f / fZ0[nMap]), RGBA_MAKE(255,255,255,255),0, 0.0f,                                                       0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh,  9.5f, fZ1[nMap]),(1.0f / fZ1[nMap]), RGBA_MAKE(255,255,255,255),0, fZ1[nMap] == 0.9f ? 1.0f : 0.0f,                            0.0f);
    VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fMesh, fMesh, fZ3[nMap]),(1.0f / fZ3[nMap]), RGBA_MAKE(255,255,255,255),0, fZ3[nMap] == 0.9f ? 1.0f : 0.0f, fZ3[nMap] == 0.9f ? 1.0f : 0.0f);
}
