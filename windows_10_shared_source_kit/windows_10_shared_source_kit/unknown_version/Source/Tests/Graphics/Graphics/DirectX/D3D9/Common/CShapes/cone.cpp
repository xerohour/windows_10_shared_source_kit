// Cone.cpp
//
//  CShapes class routine for generating a triangle-list cone.
//

#include "CShapesPCH.h"

#define D3D_OVERLOADS
#include "cshapes.h"
#include "d3dtypes.h"
#include "d3d.h"

bool CShapes::GenerateCone(const int nDivisions)
{
	const float fRadius = 0.5f;
	const float fTheta = 360.0f / nDivisions;

	const int   nVertices = (3 * nDivisions + 1);
	const int   nIndices = (6 * nDivisions); // 3*nDivisions (top) + 3*nDivisions (bottom) 
	D3DVERTEX * pVertices = NULL;
	WORD *      pIndices = NULL;
	
	float       fRads;

	// allocate memory for the vertices and indices
	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * nIndices);
	
	if(pVertices && pIndices)
	{
		int i, n;
		float fX, fZ, fU;
		D3DVECTOR Normal;

		memset(pVertices, 0, sizeof(D3DVERTEX) * nVertices);
		memset(pIndices, 0, sizeof(WORD) * nIndices);

		// 
		// Set Vertex Data
		//
		
		// Initialize the bottom center point
		pVertices[nVertices - 1].dvX = 0.0f;
		pVertices[nVertices - 1].dvY = -0.5f;
		pVertices[nVertices - 1].dvZ = 0.0f;
		pVertices[nVertices - 1].dvNX = 0.0f;
		pVertices[nVertices - 1].dvNY = -1.0f;
		pVertices[nVertices - 1].dvNZ = 0.0f;
		pVertices[nVertices - 1].dvTU = 0.5f;
		pVertices[nVertices - 1].dvTV = 1.0f;

		// generate the cone vertices
		for (i=0; i < (nDivisions); i++)
		{
			// cos takes a radian value, so convert.
			fRads = (fTheta * (float)i * pi) / 180.0f;
			fX = (float)(fRadius * cos(fRads));
			fZ = (float)(fRadius * sin(fRads));
			fU = (float)(i+1) / (float)nDivisions;

			Normal = Normalize(D3DVECTOR(fX, 0.5f, fZ));

			// top -- these are the same physical point, but
			// with normals facing outward.
			pVertices[i].dvX = 0.0f;
			pVertices[i].dvY = 0.5f;
			pVertices[i].dvZ = 0.0f;
			pVertices[i].dvNX = Normal.dvX;
			pVertices[i].dvNY = Normal.dvY;
			pVertices[i].dvNZ = Normal.dvZ;
			pVertices[i].dvTU = fU;
			pVertices[i].dvTV = 0.0f;

			// bottom side
			pVertices[nDivisions + i].dvX = fX;
			pVertices[nDivisions + i].dvY = -0.5f;
			pVertices[nDivisions + i].dvZ = fZ;
			pVertices[nDivisions + i].dvNX = Normal.dvX;
			pVertices[nDivisions + i].dvNY = Normal.dvY;
			pVertices[nDivisions + i].dvNZ = Normal.dvZ;
			pVertices[nDivisions + i].dvTU = fU;
			pVertices[nDivisions + i].dvTV = 0.75f;

			// bottom
			pVertices[(nDivisions * 2) + i].dvX = fX;
			pVertices[(nDivisions * 2) + i].dvY = -0.5f;
			pVertices[(nDivisions * 2) + i].dvZ = fZ;
			pVertices[(nDivisions * 2) + i].dvNX = 0.0f;
			pVertices[(nDivisions * 2) + i].dvNY = -1.0f;
			pVertices[(nDivisions * 2) + i].dvNZ = 0.0f;
			pVertices[(nDivisions * 2) + i].dvTU = fU;
			pVertices[(nDivisions * 2) + i].dvTV = 0.75f;
		}

		//
		// Set Index Data
		//
		n = 0;
		// top
		for(i = 0; i < nDivisions; i++)
		{
			pIndices[n] = (WORD)(nDivisions + i);
			pIndices[n+1] = (WORD)i;
			if(i < (nDivisions - 1))
				pIndices[n+2] = (WORD)(nDivisions + i + 1);
			else
				pIndices[n+2] = (WORD)nDivisions;  // the last index on the top.
			n += 3;
		}

		// bottom
		for(i = 0; i < nDivisions; i++)
		{
			if(i < (nDivisions - 1))
				pIndices[n] = (WORD)((nDivisions * 2) + i + 1);
			else
				pIndices[n] = (WORD)(nDivisions * 2);
			pIndices[n+1] = (WORD)(nVertices - 1);
			pIndices[n+2] = (WORD)((nDivisions * 2) + i);
			n += 3;
		}

		//
		// set class data
		//
		m_pVertices = pVertices;
		m_nVertices = nVertices;
		m_pIndices = pIndices;
		m_nIndices = nIndices;
		m_bWrapU = true;
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
