//
//  Boid.cpp
//
//  CShapes class routine for generating a triangle-list Boid.
//  (Courtesy of the father-of-boid Steve Coy)
//

#include "CShapesPCH.h"

#include "CShapes.h"
#include "d3d.h"

// ----------------------------------------------------------------------------

bool CShapes::GenerateBoid()  // called by NewShape
{
	D3DVERTEX * pVertices = NULL;
	WORD *      pIndices = NULL;
	const int   nVertices = 16;
	const int   nIndices = 30;
	int         i;

	// the boid vertices
	D3DVERTEX VertexList[nVertices] = {
		//   x      y       z      Nx        Ny        Nz       u       v
		{   0.0f,  0.0f,  10.0f,  0.1961f,  0.9806f,  0.0000f, 0.000f, 0.500f }, // boid top
		{  10.0f,  0.0f, -10.0f,  0.0995f,  0.9950f,  0.0000f, 0.500f, 1.000f },
		{   3.0f,  3.0f,  -7.0f,  0.0000f,  1.0000f,  0.0000f, 0.425f, 0.575f },
		{  -3.0f,  3.0f,  -7.0f, -0.0995f,  0.9950f,  0.0000f, 0.425f, 0.425f },
		{ -10.0f,  0.0f, -10.0f, -0.1961f,  0.9806f,  0.0000f, 0.500f, 0.000f },

		{   0.0f,  0.0f,  10.0f,  0.1961f, -0.9806f,  0.0000f, 1.000f, 0.500f }, // boid bottom
		{  10.0f,  0.0f, -10.0f,  0.0995f, -0.9950f,  0.0000f, 0.500f, 1.000f },
		{   3.0f, -3.0f,  -7.0f,  0.0000f, -1.0000f,  0.0000f, 0.575f, 0.575f },
		{  -3.0f, -3.0f,  -7.0f, -0.0995f, -0.9950f,  0.0000f, 0.575f, 0.425f },
		{ -10.0f,  0.0f, -10.0f, -0.1961f, -0.9806f,  0.0000f, 0.500f, 0.000f },

		{  10.0f,  0.0f, -10.0f, -0.3714f,  0.0000f, -0.9285f, 0.500f, 1.000f }, // boid rear
		{   3.0f,  3.0f,  -7.0f, -0.1961f,  0.0000f, -0.9806f, 0.425f, 0.575f },
		{  -3.0f,  3.0f,  -7.0f,  0.1961f,  0.0000f, -0.9806f, 0.425f, 0.425f },
		{ -10.0f,  0.0f, -10.0f,  0.3714f,  0.0000f, -0.9285f, 0.500f, 0.000f },
		{  -3.0f, -3.0f,  -7.0f,  0.1961f,  0.0000f, -0.9806f, 0.575f, 0.425f },
		{   3.0f, -3.0f,  -7.0f, -0.1961f,  0.0000f, -0.9806f, 0.575f, 0.575f }
	};

	// scale the boid to be unit length and normalize vectors
	for(i = 0; i < 16; i++)
	{
		VertexList[i].x /= 20.0f;
		VertexList[i].y /= 20.0f;
		VertexList[i].z /= 20.0f;
	}

	WORD IndexList[nIndices] = {
		 0,  1,  2,  0,  2,  3,  0,  3,  4, // top
		 5,  7,  6,  5,  8,  7,  5,  9,  8, // bottom
		10, 15, 11, 11, 15, 12, 12, 15, 14, 12, 14, 13 // rear
	};

	// allocate the needed memory
	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * nIndices);

	if(pVertices && pIndices)
	{
		//
		// set vertex data
		//
		for(i = 0; i < nVertices; i++)
		{
			pVertices[i].dvX = VertexList[i].dvX;
			pVertices[i].dvY = VertexList[i].dvY;
			pVertices[i].dvZ = VertexList[i].dvZ;
			pVertices[i].dvNX = VertexList[i].dvNX;
			pVertices[i].dvNY = VertexList[i].dvNY;
			pVertices[i].dvNZ = VertexList[i].dvNZ;
			pVertices[i].dvTU = VertexList[i].dvTU;
			pVertices[i].dvTV = VertexList[i].dvTV;
		}
		
		//
		// set index data
		//
		for(i = 0; i < nIndices; i++)
			pIndices[i] = IndexList[i];
		
		//
		// set class data
		//
		m_pVertices = pVertices;
		m_nVertices = nVertices;
		m_pIndices = pIndices;
		m_nIndices = nIndices;
		m_bWrapU = true;
		m_bWrapV = true;

		return true;
	}
	else
	{
		if (pVertices)
			free(pVertices);
		if (pIndices)
			free(pIndices);
		return false;
	}
}