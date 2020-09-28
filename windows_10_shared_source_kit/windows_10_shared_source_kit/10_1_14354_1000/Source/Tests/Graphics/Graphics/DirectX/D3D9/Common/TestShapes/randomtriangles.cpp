// ============================================================================
//
//  RandomTriangles.cpp
//
// ============================================================================

#include "TestShapesPCH.h"

#define D3D_OVERLOADS
#include "TestShapes.h"
#include "rnumber.h"
#include "d3d.h"

bool CTestShapes::GenerateRandomTriangles(const DWORD dwTriangles)
{
	const float  fChangeTheta = (2.0f * pi / 3.0f);
	const DWORD  dwVertices = dwTriangles * 3;
	D3DVERTEX *  pVertices = NULL;
	const DWORD  dwIndices = dwVertices;
	const float  fTriRadius = 1.0f / dwTriangles;    
	WORD *       pIndices = NULL;
	DWORD        i, j, dwCurVtx;
	D3DVECTOR    vCenter, v1, v2, vFace, vTriVerts[3];
	float        fTheta;
	D3DMATRIX    mTransform;

	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * dwVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * dwIndices);

	if(pVertices && pIndices)
	{
		ZeroMemory(pVertices, sizeof(D3DVERTEX) * dwVertices);
		ZeroMemory(pIndices, sizeof(WORD) * dwIndices);

		for(i = 0; i < dwTriangles; i++)
		{
			ZeroMemory(vTriVerts, sizeof(D3DVECTOR) * 3);

			fTheta = 0.0f;
			// create a triangle of radius fTriRadius
			for(j = 0; j < 3; j++)
			{
				vTriVerts[j].x = fTriRadius * (float)cos(fTheta);
				vTriVerts[j].y = fTriRadius * (float)sin(fTheta);
				fTheta += fChangeTheta;
			}
			
			// generate a random center point
			vCenter.x = (float)GetRandomNumber(-5000, 5000) / 10000.0f;
			vCenter.y = (float)GetRandomNumber(-5000, 5000) / 10000.0f;
			vCenter.z = (float)GetRandomNumber(-5000, 5000) / 10000.0f;

			// move the triangle to its new home.
			mTransform = TranslateMatrix(vCenter);
			// apply a random rotation to the triangle.
			mTransform = MatrixMult(mTransform, RotateXMatrix((float)GetRandomNumber(0, (int)(2.0f * pi * 1000.0f)) / 1000.0f));
			mTransform = MatrixMult(mTransform, RotateYMatrix((float)GetRandomNumber(0, (int)(2.0f * pi * 1000.0f)) / 1000.0f));
			mTransform = MatrixMult(mTransform, RotateZMatrix((float)GetRandomNumber(0, (int)(2.0f * pi * 1000.0f)) / 1000.0f));

			// now transform the vertices in the triangle
			for(j = 0; j < 3; j++)
				vTriVerts[j] = TransformVector(vTriVerts[j], mTransform);
			
			// find the direction the triangle is "facing"
			v1.x = vTriVerts[1].x - vTriVerts[0].x;
			v1.y = vTriVerts[1].y - vTriVerts[0].y;
			v1.z = vTriVerts[1].z - vTriVerts[0].z;
			v2.x = vTriVerts[2].x - vTriVerts[0].x;
			v2.y = vTriVerts[2].y - vTriVerts[0].y;
			v2.z = vTriVerts[2].z - vTriVerts[0].z;
			vFace = Normalize(CrossProduct(v1, v2));

			for(j = 0; j < 3; j++) // set the triangle data in the list
			{
				dwCurVtx = (i * 3) + j;
				pVertices[dwCurVtx].x = vTriVerts[j].x;
				pVertices[dwCurVtx].y = vTriVerts[j].y;
				pVertices[dwCurVtx].z = vTriVerts[j].z;
				pVertices[dwCurVtx].nx = vFace.x;
				pVertices[dwCurVtx].ny = vFace.y;
				pVertices[dwCurVtx].nz = vFace.z;
			}
		}

		// set the indices
		for(i = 0; i < dwIndices; i++)
			pIndices[i] = (WORD)i;

		// assign the class data members
		m_Type      = D3DPT_TRIANGLELIST;
		m_primCount = dwTriangles;
		m_nVertices = dwVertices;
		m_pVertices = pVertices;
		m_nIndices  = dwIndices;
		m_pIndices  = pIndices;
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
