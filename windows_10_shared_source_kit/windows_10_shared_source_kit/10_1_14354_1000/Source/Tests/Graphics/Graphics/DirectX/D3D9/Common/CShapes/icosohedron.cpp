//
//  Icosohedron.cpp
//
//  CShapes class routine for generating a triangle-list Icosohedron.
//
//  Icosohedron added by cliffo May 16, 1998

#include "CShapesPCH.h"

#define D3D_OVERLOADS
#include "CShapes.h"
#include "math.h"
#include "d3d.h"

#pragma intrinsic(sqrt)

// ----------------------------------------------------------------------------

bool CShapes::GenerateIcosohedron()  // called by NewShape
{
    D3DVERTEX   VertexList[12];
    WORD        wIndexList[]={6,5,0,   6,4,10, 5,6,10, 2,4,6, 0,2,6, 
                              4,2,8,   8,2,1,  1,2,0,  1,0,7, 7,0,5,
                              5,10,11, 5,11,7, 7,11,3, 7,3,1, 11,9,3,
                              1,3,8,   9,4,8,  4,9,10, 9,11,10, 9,8,3};
    LPD3DVERTEX pVertices = NULL;
    PWORD       pIndices = NULL;
    float       t = (float)((sqrt(5.0f)-1.0f)/2.0f);

	pVertices = (D3DVERTEX *)malloc(sizeof(VertexList));
	pIndices = (WORD *)malloc(sizeof(wIndexList));

    if (NULL == pVertices || NULL == pIndices)
    {
        if (NULL != pVertices)
            free(pVertices);

        if (NULL != pIndices)
            free(pIndices);

        return false;
    }

    // Generate the vertex list

    VertexList[0] =  D3DVERTEX(Normalize(D3DVECTOR( 1.f, 0.f,   t)),Normalize(D3DVECTOR( 1.f, 0.f,   t)),0.0f,0.0f);
    VertexList[1] =  D3DVERTEX(Normalize(D3DVECTOR( 0.f,   t, 1.f)),Normalize(D3DVECTOR( 0.f,   t, 1.f)),0.0f,0.0f);
    VertexList[2] =  D3DVERTEX(Normalize(D3DVECTOR(   t, 1.f, 0.f)),Normalize(D3DVECTOR(   t, 1.f, 0.f)),0.0f,0.0f);
    VertexList[3] =  D3DVERTEX(Normalize(D3DVECTOR(-1.f, 0.f,   t)),Normalize(D3DVECTOR(-1.f, 0.f,   t)),0.0f,0.0f);
    VertexList[4] =  D3DVERTEX(Normalize(D3DVECTOR( 0.f,   t,-1.f)),Normalize(D3DVECTOR( 0.f,   t,-1.f)),0.0f,0.0f);
    VertexList[5] =  D3DVERTEX(Normalize(D3DVECTOR(   t,-1.f, 0.f)),Normalize(D3DVECTOR(   t,-1.f, 0.f)),0.0f,0.0f);
    VertexList[6] =  D3DVERTEX(Normalize(D3DVECTOR( 1.f, 0.f,  -t)),Normalize(D3DVECTOR( 1.f, 0.f,  -t)),0.0f,0.0f);
    VertexList[7] =  D3DVERTEX(Normalize(D3DVECTOR( 0.f,  -t, 1.f)),Normalize(D3DVECTOR( 0.f,  -t, 1.f)),0.0f,0.0f);
    VertexList[8] =  D3DVERTEX(Normalize(D3DVECTOR(  -t, 1.f, 0.f)),Normalize(D3DVECTOR(  -t, 1.f, 0.f)),0.0f,0.0f);
    VertexList[9] =  D3DVERTEX(Normalize(D3DVECTOR(-1.f, 0.f,  -t)),Normalize(D3DVECTOR(-1.f, 0.f,  -t)),0.0f,0.0f);
    VertexList[10] = D3DVERTEX(Normalize(D3DVECTOR( 0.f,  -t,-1.f)),Normalize(D3DVECTOR( 0.f,  -t,-1.f)),0.0f,0.0f);
    VertexList[11] = D3DVERTEX(Normalize(D3DVECTOR(  -t,-1.f, 0.f)),Normalize(D3DVECTOR(  -t,-1.f, 0.f)),0.0f,0.0f);

	// scale the shape to be unit length and normalize vectors
	// (a-eronh 5/18/98)
	for(int i = 0; i < 12; i++)
	{
		VertexList[i].x *= 0.5f;
		VertexList[i].y *= 0.5f;
		VertexList[i].z *= 0.5f;
	}

    memcpy(pVertices,VertexList,sizeof(VertexList));
    memcpy(pIndices,wIndexList,sizeof(wIndexList));

	m_pVertices = pVertices;
	m_nVertices = sizeof(VertexList)/sizeof(D3DVERTEX);
	m_pIndices = pIndices;
	m_nIndices = sizeof(wIndexList)/sizeof(WORD);
	m_bWrapU = true;
	m_bWrapV = true;

	// calculate texture coordinates
	// (a-eronh 6/01/98)
	TextureMapSpherical(m_pVertices, m_nVertices);

    return true;
}
