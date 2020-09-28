// ============================================================================
//
//  WaveStrip.cpp
//
// ============================================================================

#include "TestShapesPCH.h"

#define D3D_OVERLOADS
#include "TestShapes.h"
#include "d3d.h"

bool CTestShapes::GenerateWaveStrip(const DWORD dwDivisions, const DWORD dwWaves)
{
	const float  fWaveSize = 2 * pi * (float)dwWaves;
	const float  fXStride = fWaveSize / (float)dwDivisions;
	const DWORD  dwVertices = (dwDivisions * 2) + 2;
	D3DVERTEX   *pVertices = NULL;
	const DWORD  dwIndices = dwVertices;
	WORD        *pIndices = NULL;
	float        fXPos, fYPos, fZPos, fZLast;
	D3DVECTOR    v1, v2, v3;
	DWORD        i;

	if(dwWaves >= dwDivisions)
		return false;

	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * dwVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * dwIndices);

	if(pVertices && pIndices)
	{
		ZeroMemory(pVertices, sizeof(D3DVERTEX) * dwVertices);
		ZeroMemory(pIndices, sizeof(WORD) * dwIndices);

		fXPos = fZPos = fZLast = 0;
		fYPos = -0.5f;
		ZeroMemory(&v1, sizeof(D3DVECTOR));
		ZeroMemory(&v2, sizeof(D3DVECTOR));
		ZeroMemory(&v3, sizeof(D3DVECTOR));
		
		for(i = 0; i < dwDivisions; i++)
		{
			// set vertex position
			pVertices[i].x = fXPos;
			pVertices[i].y = fYPos;
			pVertices[i].z = fZPos = (float)sin(fXPos) * 0.5f;
			pVertices[i].tu = fXPos;
			pVertices[i].tv = fYPos + 0.5f;

			v3.x = fXStride;
			v3.z = fZLast - fZPos;

			v1 = Normalize(CrossProduct(v2, v3));

			pVertices[i].nx = v1.x;
			pVertices[i].ny = v1.y;
			pVertices[i].nz = v1.z;

			if(fYPos == 0.5f)
			{
				fYPos = -0.5f;
				fXPos += fXStride;
			}
			else
				fYPos = 0.5f;

			fZLast = fZPos;
		}

		// set the indices
		for(i = 0; i < dwIndices; i++)
			pIndices[i] = (WORD)i;

		// compress the x data into unit size.
		for(i = 0; i < dwVertices; i++)
			pVertices[i].x = (pVertices[i].x / fWaveSize) - 0.5f;

		// assign the class data members
		m_Type      = D3DPT_TRIANGLESTRIP;
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
