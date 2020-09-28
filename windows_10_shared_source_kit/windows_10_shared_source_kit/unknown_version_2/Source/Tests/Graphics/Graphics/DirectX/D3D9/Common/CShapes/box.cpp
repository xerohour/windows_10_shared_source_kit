//
//  Box.cpp
//
//  CShapes class routine for generating a triangle-list box.
//

#include "CShapesPCH.h"

#include "CShapes.h"
#include "d3d.h"

// ----------------------------------------------------------------------------

bool CShapes::GenerateBox()  // called by NewShape
{
	D3DVERTEX * pVertices = NULL;
	WORD *      pIndices = NULL;
	const int   nVertices = 24;
	const int   nIndices = 36;

	// define a basic cube, centered at the origin.
	D3DVERTEX VertexList[nVertices] = {
		//  x      y      z      Nx     Ny     Nz    u     v
		{  0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f }, // face 1 (front)
		{ -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f },
		{  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f },

		{  0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f }, // face 2 (right)
		{  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f },
		{  0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
		{  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f },

		{ -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f }, // face 3 (back)
		{  0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f },
		{  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f },
		{ -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f },

		{ -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f }, // face 4 (left)
		{ -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
		{ -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f },

		{  0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f }, // face 5 (top)
		{ -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f },
		{  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f },
		
		{  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f }, // face 6 (bottom)
		{ -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f },
		{ -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f },
		{  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f },
	};

	WORD IndexList[nIndices] = {
		 0,  1,  2,  0,  2,  3,  // face 1
		 4,  5,  6,  4,  6,  7,  // face 2
		 8,  9, 10,  8, 10, 11,  // face 3
		12, 13, 14, 12, 14, 15,  // face 4
		16, 17, 18, 16, 18, 19,  // face 5
		20, 21, 22, 20, 22, 23,  // face 6
	};

	// allocate the needed memory
	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * nIndices);

	if(pVertices && pIndices)
	{
		int i;
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
		m_bWrapU = false;
		m_bWrapV = false;

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

// ----------------------------------------------------------------------------
