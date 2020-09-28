// ============================================================================
//
//  TriThing.cpp
//
// ============================================================================

#include "TestShapesPCH.h"

#include "TestShapes.h"
#include "d3d.h"

bool CTestShapes::GenerateTriThing()
{
	const int   nTriangles = 18;
	const int   nVertices = nTriangles * 3;
	D3DVERTEX * pVertices = NULL;
	const int   nIndices = nVertices;
	WORD *      pIndices = NULL;
	int         i;

	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * nIndices);

	if(pVertices && pIndices)
	{
		memset(pVertices, 0, sizeof(D3DVERTEX) * nVertices);
		memset(pIndices, 0, sizeof(WORD) * nIndices);

		// set up the indices
		for(i = 0; i < nIndices; i++)
			pIndices[i] = (WORD)i; 

		// these are the points for the triangles we'll be drawing.
		D3DVERTEX VertexList[nVertices] = {
			// Triangle group 1  (top strip)
			{ 0.11f, 0.85f, 0.0f, 0.0f, 0.0f, -1.0f, 0.11f, 0.85f },  // tri 1
			{ 0.18f, 0.90f, 0.0f, 0.0f, 0.0f, -1.0f, 0.18f, 0.90f },
			{ 0.25f, 0.85f, 0.0f, 0.0f, 0.0f, -1.0f, 0.25f, 0.85f },
			{ 0.27f, 0.85f, 0.0f, 0.0f, 0.0f, -1.0f, 0.27f, 0.85f },  // tri 2
			{ 0.34f, 0.90f, 0.0f, 0.0f, 0.0f, -1.0f, 0.34f, 0.90f },
			{ 0.41f, 0.85f, 0.0f, 0.0f, 0.0f, -1.0f, 0.41f, 0.85f },
			{ 0.43f, 0.85f, 0.0f, 0.0f, 0.0f, -1.0f, 0.43f, 0.85f },  // tri 3
			{ 0.50f, 0.90f, 0.0f, 0.0f, 0.0f, -1.0f, 0.50f, 0.90f },
			{ 0.57f, 0.85f, 0.0f, 0.0f, 0.0f, -1.0f, 0.57f, 0.85f },
			{ 0.59f, 0.85f, 0.0f, 0.0f, 0.0f, -1.0f, 0.59f, 0.85f },  // tri 4
			{ 0.66f, 0.90f, 0.0f, 0.0f, 0.0f, -1.0f, 0.66f, 0.90f },
			{ 0.73f, 0.85f, 0.0f, 0.0f, 0.0f, -1.0f, 0.73f, 0.85f },
			{ 0.75f, 0.85f, 0.0f, 0.0f, 0.0f, -1.0f, 0.75f, 0.85f },  // tri 5
			{ 0.82f, 0.90f, 0.0f, 0.0f, 0.0f, -1.0f, 0.82f, 0.90f },
			{ 0.89f, 0.85f, 0.0f, 0.0f, 0.0f, -1.0f, 0.89f, 0.85f },
			// Triangle group 2  (left middle)
			{ 0.14f, 0.80f, 0.0f, 0.0f, 0.0f, -1.0f, 0.14f, 0.80f },  // tri 6
			{ 0.36f, 0.80f, 0.0f, 0.0f, 0.0f, -1.0f, 0.36f, 0.80f },
			{ 0.36f, 0.20f, 0.0f, 0.0f, 0.0f, -1.0f, 0.36f, 0.20f },
			{ 0.10f, 0.80f, 0.0f, 0.0f, 0.0f, -1.0f, 0.10f, 0.80f },  // tri 7
			{ 0.32f, 0.20f, 0.0f, 0.0f, 0.0f, -1.0f, 0.32f, 0.20f },
			{ 0.10f, 0.20f, 0.0f, 0.0f, 0.0f, -1.0f, 0.10f, 0.20f },
			// Triangle group 3  (center group)
			{ 0.44f, 0.80f, 0.0f, 0.0f, 0.0f, -1.0f, 0.44f, 0.80f },  // tri 8
			{ 0.46f, 0.50f, 0.0f, 0.0f, 0.0f, -1.0f, 0.46f, 0.50f },
			{ 0.40f, 0.28f, 0.0f, 0.0f, 0.0f, -1.0f, 0.40f, 0.28f },
			{ 0.48f, 0.80f, 0.0f, 0.0f, 0.0f, -1.0f, 0.48f, 0.80f },  // tri 9
			{ 0.60f, 0.76f, 0.0f, 0.0f, 0.0f, -1.0f, 0.60f, 0.76f },
			{ 0.50f, 0.54f, 0.0f, 0.0f, 0.0f, -1.0f, 0.50f, 0.54f },
			{ 0.54f, 0.50f, 0.0f, 0.0f, 0.0f, -1.0f, 0.54f, 0.50f },  // tri 10
			{ 0.60f, 0.72f, 0.0f, 0.0f, 0.0f, -1.0f, 0.60f, 0.72f },
			{ 0.56f, 0.20f, 0.0f, 0.0f, 0.0f, -1.0f, 0.56f, 0.20f },
			{ 0.52f, 0.20f, 0.0f, 0.0f, 0.0f, -1.0f, 0.52f, 0.20f },  // tri 11
			{ 0.40f, 0.24f, 0.0f, 0.0f, 0.0f, -1.0f, 0.40f, 0.24f },
			{ 0.50f, 0.46f, 0.0f, 0.0f, 0.0f, -1.0f, 0.50f, 0.46f },
			// Triangle group 4  (right middle)
			{ 0.64f, 0.80f, 0.0f, 0.0f, 0.0f, -1.0f, 0.64f, 0.80f },  // tri 12
			{ 0.90f, 0.80f, 0.0f, 0.0f, 0.0f, -1.0f, 0.90f, 0.80f },
			{ 0.90f, 0.24f, 0.0f, 0.0f, 0.0f, -1.0f, 0.90f, 0.24f },
			{ 0.64f, 0.76f, 0.0f, 0.0f, 0.0f, -1.0f, 0.64f, 0.76f },  // tri 13
			{ 0.90f, 0.20f, 0.0f, 0.0f, 0.0f, -1.0f, 0.90f, 0.20f },
			{ 0.64f, 0.20f, 0.0f, 0.0f, 0.0f, -1.0f, 0.64f, 0.20f },
			// Triangle group 5  (bottom strip)
			{ 0.11f, 0.15f, 0.0f, 0.0f, 0.0f, -1.0f, 0.11f, 0.15f },  // tri 14
			{ 0.25f, 0.15f, 0.0f, 0.0f, 0.0f, -1.0f, 0.25f, 0.15f },
			{ 0.18f, 0.10f, 0.0f, 0.0f, 0.0f, -1.0f, 0.18f, 0.10f },
			{ 0.27f, 0.15f, 0.0f, 0.0f, 0.0f, -1.0f, 0.27f, 0.15f },  // tri 15
			{ 0.41f, 0.15f, 0.0f, 0.0f, 0.0f, -1.0f, 0.41f, 0.15f },
			{ 0.34f, 0.10f, 0.0f, 0.0f, 0.0f, -1.0f, 0.34f, 0.10f },
			{ 0.43f, 0.15f, 0.0f, 0.0f, 0.0f, -1.0f, 0.43f, 0.15f },  // tri 16
			{ 0.57f, 0.15f, 0.0f, 0.0f, 0.0f, -1.0f, 0.57f, 0.15f },
			{ 0.50f, 0.10f, 0.0f, 0.0f, 0.0f, -1.0f, 0.50f, 0.10f },
			{ 0.59f, 0.15f, 0.0f, 0.0f, 0.0f, -1.0f, 0.59f, 0.15f },  // tri 17
			{ 0.73f, 0.15f, 0.0f, 0.0f, 0.0f, -1.0f, 0.73f, 0.15f },
			{ 0.66f, 0.10f, 0.0f, 0.0f, 0.0f, -1.0f, 0.66f, 0.10f },
			{ 0.75f, 0.15f, 0.0f, 0.0f, 0.0f, -1.0f, 0.75f, 0.15f },  // tri 18
			{ 0.89f, 0.15f, 0.0f, 0.0f, 0.0f, -1.0f, 0.89f, 0.15f },
			{ 0.82f, 0.10f, 0.0f, 0.0f, 0.0f, -1.0f, 0.82f, 0.10f },
		};

		for(i = 0; i < nVertices; i++)
		{
			pVertices[i].x = VertexList[i].x;
			pVertices[i].y = VertexList[i].y;
			pVertices[i].z = VertexList[i].z;
			pVertices[i].nx = VertexList[i].nx;
			pVertices[i].ny = VertexList[i].ny;
			pVertices[i].nz = VertexList[i].nz;
			pVertices[i].tu = VertexList[i].tu;
			pVertices[i].tv = VertexList[i].tv;
		}

		// assign the class data members
		m_Type      = D3DPT_TRIANGLELIST;
		m_primCount = nTriangles;
		m_nVertices = nVertices;
		m_pVertices = pVertices;
		m_nIndices  = nIndices;
		m_pIndices  = pIndices;

		m_pCenter->x = 0.5f;
		m_pCenter->y = 0.5f;
		m_pCenter->z = 0.0f;

		D3DVECTOR pos = {0.0f, 0.0f, 0.0f};
		MoveTo(pos);

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
