// ============================================================================
//
//  FanHorn.cpp
//
// ============================================================================

#include "TestShapesPCH.h"

#define D3D_OVERLOADS
#include "TestShapes.h"
#include "d3d.h"

bool CTestShapes::GenerateFanHorn(const DWORD dwDivisions, const DWORD dwRotations)
{
	const DWORD  dwVertices = dwDivisions + 1;
	D3DVERTEX *  pVertices = NULL;
	const DWORD  dwIndices = dwVertices;
	WORD *       pIndices = NULL;
	float        fTheta, fZPos, fRadius;
	const float  fChangeTheta = 2.0f * pi * (float)dwRotations / (float)dwDivisions;
	DWORD        i;
	D3DVECTOR    v1, v2, v3;

	if(dwRotations >= dwDivisions)
		return false;

	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * dwVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * dwIndices);

	if(pVertices && pIndices)
	{
		ZeroMemory(pVertices, sizeof(D3DVERTEX) * dwVertices);
		ZeroMemory(pIndices, sizeof(WORD) * dwIndices);

		pVertices[0].z = -0.5f;

		pVertices++; // skip the first vertex

		fTheta = 0.0f;
		for(i = 0; i < dwDivisions; i++)
		{
			fZPos = 0.5f - ((float)(dwDivisions - i) / (float)(dwDivisions));
			fRadius = 0.5f - fZPos; // radius increases as fZPos => -0.5f.

			// set vertex position
			pVertices[i].x = fRadius * (float)cos(fTheta);
			pVertices[i].y = fRadius * (float)sin(fTheta);
			pVertices[i].z = fZPos;
			pVertices[i].tu = fRadius * 2.0f;
			pVertices[i].tv = fZPos + 0.5f;

			// calculate & set vertex normal
			if((i > 0) && (i < (dwDivisions - 1)))
			{
				v1.x = pVertices[i - 1].x - pVertices[i].x; 
				v1.y = pVertices[i - 1].y - pVertices[i].y; 
				v1.z = pVertices[i - 1].z - pVertices[i].z; 
				v2.x = -pVertices[i].x;
				v2.y = -pVertices[i].y;
				v2.z = -pVertices[i].z;
				v3 = Normalize(CrossProduct(v1, v2));
				pVertices[i].nx = v3.x;
				pVertices[i].ny = v3.y;
				pVertices[i].nz = v3.z;
			}
			else // first and last non-origin vertex get special attention
			{
				pVertices[i].nx = 0.0f;
				pVertices[i].ny = 0.0f;
				pVertices[i].nz = 1.0f;
			}

			fTheta += fChangeTheta;
		}

		pVertices--; // roll back to point at the first vertex again.
		
		// set the indices
		for(i = 0; i < dwIndices; i++)
			pIndices[i] = (WORD)i;

		// assign the class data members
		m_Type      = D3DPT_TRIANGLEFAN;
		m_primCount = dwVertices-2;
		m_nVertices = (int)dwVertices;
		m_pVertices = pVertices;
		m_nIndices  = (int)dwIndices;
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
