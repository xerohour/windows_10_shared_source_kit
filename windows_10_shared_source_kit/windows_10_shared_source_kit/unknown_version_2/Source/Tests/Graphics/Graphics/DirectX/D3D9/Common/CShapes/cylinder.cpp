// Cylinder.cpp
//
//  CShapes class routine for generating a triangle-list cylinder.
//

#include "CShapesPCH.h"

#define D3D_OVERLOADS
#include "cshapes.h"
#include "d3dtypes.h"
#include "d3d.h"

bool CShapes::GenerateCylinder(const int nDivisions, const int hDivisions)
{
	const float fRadius = 0.5f;
	const float fTheta = 360.0f / nDivisions;

	const int   nVertices = nDivisions * (3 + hDivisions) + 2;
	const int   nIndices = 6 * nDivisions * (1 + hDivisions); // 3*nDivisions (top) + 6 * nDivisions * hDivisions(middle) + 3*nDivisions (bottom) 
	D3DVERTEX * pVertices = NULL;
	WORD *      pIndices = NULL;
	
	float       fRads;

	// allocate memory for the vertices and indices
	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * nIndices);
	
	if(pVertices && pIndices)
	{
		int i, j, n, idx;
		float fX, fZ, fU;
		D3DVECTOR Normal;

		memset(pVertices, 0, sizeof(D3DVERTEX) * nVertices);
		memset(pIndices, 0, sizeof(WORD) * nIndices);

		// 
		// Set Vertex Data
		//
		
		// Initialize the two center points
		// top
		pVertices[0].x  = 0.0f;
		pVertices[0].y  = 0.5f;
		pVertices[0].z  = 0.0f;
		pVertices[0].nx = 0.0f;
		pVertices[0].ny = 1.0f;
		pVertices[0].nz = 0.0f;
		pVertices[0].tu = 0.5f;
		pVertices[0].tv = 0.0f;
		// bottom
		pVertices[nVertices - 1].x  = 0.0f;
		pVertices[nVertices - 1].y  = -0.5f;
		pVertices[nVertices - 1].z  = 0.0f;
		pVertices[nVertices - 1].nx = 0.0f;
		pVertices[nVertices - 1].ny = -1.0f;
		pVertices[nVertices - 1].nz = 0.0f;
		pVertices[nVertices - 1].tu = 0.5f;
		pVertices[nVertices - 1].tv = 0.0f;

		// generate the outer points
		for(i=1; i <= nDivisions; i++)
		{
			// cos takes a radian value, so convert.
			fRads = (fTheta * (float)(i-1) * pi) / 180.0f;
			fX = (float)(fRadius * cos(fRads));
			fZ = (float)(fRadius * sin(fRads));
			fU = (float)(i+1) / (float)nDivisions;

			Normal = Normalize(D3DVECTOR(fX, 0.0f, fZ));

			// top vertex
			pVertices[i].x = fX;
			pVertices[i].y = 0.5f;
			pVertices[i].z = fZ;
			pVertices[i].nx = 0.0f;
			pVertices[i].ny = 1.0f;
			pVertices[i].nz = 0.0f;
			pVertices[i].tu = fU;
			pVertices[i].tv = 1.0f;

			// side vertices
			for(j = 0; j <= hDivisions; j++)
			{
				idx = (nDivisions * (j + 1)) + i;
				pVertices[idx].x  = fX;
				pVertices[idx].y  = 0.5f - ((1.0f / (float)hDivisions) * (float)j);
				pVertices[idx].z  = fZ;
				pVertices[idx].nx = Normal.dvX;
				pVertices[idx].ny = 0.0f;
				pVertices[idx].nz = Normal.dvZ;
				pVertices[idx].tu = fU;
				pVertices[idx].tv = (1.0f / (float)hDivisions) * (float)j;
			}

			// bottom vertex
			idx = (nVertices - nDivisions - 2 + i);
			pVertices[idx].x  = fX;
			pVertices[idx].y  = -0.5f;
			pVertices[idx].z  = fZ;
			pVertices[idx].nx = 0.0f;
			pVertices[idx].ny = -1.0f;
			pVertices[idx].nz = 0.0f;
			pVertices[idx].tu = fU;
			pVertices[idx].tv = 1.0f;
		}

		//
		// Set Index Data
		//
		n = 0;
		// top
		for(i = 0; i < nDivisions; i++)
		{
			if(i < (nDivisions - 1))
				pIndices[n] = (WORD)(i+2);
			else
				pIndices[n] = (WORD)1;  // the last index on the top.
			pIndices[n+1] = (WORD)(i+1);
			pIndices[n+2] = (WORD)0;
			n += 3;
		}
		// middle
		for(j = 1; j <= hDivisions; j++)
		{
			for(i = 0; i < nDivisions; i++)
			{
				// triangle 1
				if(i < (nDivisions - 1))
					pIndices[n] = (WORD)((nDivisions * (j+1)) + i + 2);
				else
					pIndices[n] = (WORD)((nDivisions * (j+1)) + 1);
				pIndices[n+1] = (WORD)((nDivisions * (j+1)) + i + 1);
				pIndices[n+2] = (WORD)((nDivisions * j) + i + 1);
				// triangle 2
				pIndices[n+3] = pIndices[n];
				pIndices[n+4] = pIndices[n+2];
				if(i < (nDivisions - 1))
					pIndices[n+5] = (WORD)((nDivisions * j) + i + 2);
				else
					pIndices[n+5] = (WORD)((nDivisions * j) + 1); // last index on the side
				n += 6;
			}
		}
		// bottom
		for(i = 0; i < nDivisions; i++)
		{
			pIndices[n] = (WORD)((nDivisions * (2 + hDivisions)) + i + 1);
			if(i < (nDivisions - 1))
				pIndices[n+1] = (WORD)((nDivisions * (2 + hDivisions)) + i + 2);
			else
				pIndices[n+1] = (WORD)((nDivisions * (2 + hDivisions)) + 1);
			pIndices[n+2] = (WORD)(nVertices - 1);
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
