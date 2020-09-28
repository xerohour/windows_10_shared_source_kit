// ============================================================================
//
//  PointGrid.cpp
//
// ============================================================================

#include "TestShapesPCH.h"

#include "TestShapes.h"
#include "d3d.h"

bool CTestShapes::GeneratePointGrid(const int xDiv, const int yDiv, const int zDiv)
{
	// none of the members must be less than one.
	if((xDiv < 1) || (yDiv < 1) || (zDiv < 1))
		return false;

	const int   nVertices = xDiv * yDiv * zDiv;
	D3DVERTEX  *pVertices = NULL;
	int         nCurVtx = 0;
	int         nx, ny, nz;
	float       xPos, yPos, zPos;
	const float xStep = (xDiv == 1) ? 0.0f : (1.0f / xDiv);
	const float yStep = (yDiv == 1) ? 0.0f : (1.0f / yDiv);
	const float zStep = (zDiv == 1) ? 0.0f : (1.0f / zDiv);

	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);

	if(pVertices)
	{
		memset(pVertices, 0, sizeof(D3DVERTEX) * nVertices);

		xPos = (xDiv == 1) ? 0.0f : -0.5f;
		for(nx = 0; nx < xDiv; nx++)
		{
			yPos = (yDiv == 1) ? 0.0f : -0.5f;
			for(ny = 0; ny < yDiv; ny++)
			{
				zPos = (zDiv == 1) ? 0.0f : -0.5f;
				for(nz = 0; nz < zDiv; nz++)
				{
					pVertices[nCurVtx].x = xPos;
					pVertices[nCurVtx].y = yPos;
					pVertices[nCurVtx].z = zPos;
					pVertices[nCurVtx].nx = 0.0f;
					pVertices[nCurVtx].ny = 0.0f;
					pVertices[nCurVtx].nz = -1.0f;
					nCurVtx++;
					zPos += zStep;
				}
				yPos += yStep;
			}
			xPos += xStep;
		}

		// assign the class data members
		m_Type      = D3DPT_POINTLIST;
		m_primCount = nVertices;
		m_nVertices = nVertices;
		m_pVertices = pVertices;
		m_nIndices  = 0;
		m_pIndices  = NULL;
		return true;
	}
	else
		return false;
}
