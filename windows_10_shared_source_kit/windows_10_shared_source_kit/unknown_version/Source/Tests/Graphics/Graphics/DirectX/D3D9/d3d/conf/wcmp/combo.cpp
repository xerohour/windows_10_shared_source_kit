#define D3D_OVERLOADS
#include <d3d.h>
#include "WCmp.h"

//************************************************************************
// Internal API:    ComboTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************
void CWCmpTest::ComboTests(float Zval)
{
	// Red Triangle
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f, 10.0f,0.0f),  0.015f, RGBA_MAKE(255,0,0,255),0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(210.0f,210.0f,0.0f),  1.0f,   RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,210.0f,0.0f),  1.0f,   RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
	// Green Triangle
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f, 10.0f,Zval), (1.0f - Zval), RGBA_MAKE(0,255,0,255),0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(210.0f,210.0f,Zval), (1.0f - Zval), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,210.0f,Zval), (1.0f - Zval), RGBA_MAKE(0,255,0,255),0, 0.0f,1.0f);
}
