//
//  Sphere.cpp
//
//  CShapes class routines for generating and drawing Spheres.
//

#include "CShapesPCH.h"

#define D3D_OVERLOADS
#include "CShapes.h"
#include "math.h"
#include "d3d.h"

// ----------------------------------------------------------------------------

bool CShapes::GenerateSphere(const int nDivisions)  // called by NewShape
{
	bool bInvertFaces = FALSE;
	bool bInvertNormal = FALSE;
    const int   nVertices = (2 * (nDivisions+1) * nDivisions) + 2;
	const int   nIndices = 12 * nDivisions * nDivisions;
	const float fRadius = 0.5f;
	D3DVERTEX * pVertices = NULL;
	WORD *      pIndices = NULL;


	int     i, j;
	float   dj = (float)(pi/(nDivisions+1.0f));
	float	di = (float)(pi/nDivisions);
	int	    v;		// vertex offset
	int     ind;	// indices offset

	// allocate the needed memory
	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * nIndices);

	if(pVertices && pIndices)
	{
		// generate the sphere data, note the random texture coords
		// vertices 0 and 1 are the north and south poles
		pVertices[0] = D3DVERTEX(D3DVECTOR(0.0f, fRadius, 0.0f), Normalize(D3DVECTOR(0.0f, 1.0f, 0.0f)), 0.0f, 0.0f);
		pVertices[1] = D3DVERTEX(D3DVECTOR(0.0f, -fRadius, 0.0f), Normalize(D3DVECTOR(0.0f, -1.0f, 0.0f)), 1.0f, 1.0f);

		for (j=0; j<nDivisions; j++) 
		{
            // Wrap all the way around horizontally.  The "+1" ensures that
            // we create an additional vertex that's at the same position as
            // the first but uses different texture coordinates.
            for (i=0; i<(nDivisions+1)*2; i++) 
			{
				D3DVECTOR	p;
				D3DVECTOR	n;
				float		tu, tv;

				// Calculate points and normal vectors
				n.y=p.y = (float) cos((j+1) * dj);
				n.x=p.x = (float) sin(i * di) * (float) sin((j+1) * dj);
				n.z=p.z = (float) cos(i * di) * (float) sin((j+1) * dj);

				// apply radius
				p.y *= fRadius;
				p.x *= fRadius;
				p.z *= fRadius;

				// Determine normal direction
				if(bInvertNormal)
				{
					n.x *= -1.0f;
					n.y *= -1.0f;
					n.z *= -1.0f;
				}

                tu = ((float)i/(nDivisions+1)) / 2.0f;

				if (tu>1.0f) 
					tu -= 1.0f;

				tu = 1.0f - tu;	// flip so texture is not mirrored
				tv = (float)j/nDivisions;

                pVertices[2+i+j*(nDivisions+1)*2] = D3DVERTEX(p, n, tu, tv);
			}
		}

		// now generate the triangle indices
		// strip around north pole first
		for (i=0; i<nDivisions*2; i++) 
		{
			if (bInvertFaces)
			{
				pIndices[3*i] = (WORD)(i+3);
				pIndices[3*i+1] = (WORD)(i+2);
				pIndices[3*i+2] = (WORD)0;
			}
			else
			{
				pIndices[3*i] = (WORD)0;
				pIndices[3*i+1] = (WORD)(i+2);
				pIndices[3*i+2] = (WORD)(i+3);
			}

		}

		// now all the middle strips
		for (j=0; j<nDivisions-1; j++) 
		{
            v = 2+j*(nDivisions+1)*2;
			ind = 3*nDivisions*2 + j*6*nDivisions*2;

			for (i=0; i<nDivisions*2; i++) 
			{
				if (bInvertFaces)
				{
                    pIndices[6*i+ind]   = (WORD)(v + i + (nDivisions+1) * 2);
					pIndices[6*i+2+ind] = (WORD)(v + i + 1);
					pIndices[6*i+1+ind] = (WORD)(v + i);

					pIndices[6*i+ind+3]   = (WORD)(v + i + 1);
                    pIndices[6*i+2+ind+3] = (WORD)(v + i + (nDivisions+1) * 2);
                    pIndices[6*i+1+ind+3] = (WORD)(v + i + (nDivisions+1) * 2 + 1);

				}
				else
				{
					pIndices[6*i+ind] = (WORD)(v + i);
					pIndices[6*i+2+ind] = (WORD)(v + i + 1);
                    pIndices[6*i+1+ind] = (WORD)(v + i + (nDivisions+1) * 2);

                    pIndices[6*i+ind+3] = (WORD)(v + i + (nDivisions+1) * 2);
					pIndices[6*i+2+ind+3] = (WORD)(v + i + 1);
                    pIndices[6*i+1+ind+3] = (WORD)(v + i + (nDivisions+1) * 2 + 1);

				}
			}
		}

		// finally strip around south pole
        v = nVertices - (nDivisions+1) * 2;
		ind = nIndices - 3 * nDivisions * 2;

		for (i=0; i<nDivisions*2; i++) 
		{
			if (bInvertFaces)
			{
				pIndices[3*i+ind] = (WORD)(v + i);
				pIndices[3*i+1+ind] = (WORD)(v + i + 1);
				pIndices[3*i+2+ind] = (WORD)1;
			}
			else
			{
				pIndices[3*i+ind] = (WORD)1;
				pIndices[3*i+1+ind] = (WORD)(v + i + 1);
				pIndices[3*i+2+ind] = (WORD)(v + i);
			}

        }

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

// ----------------------------------------------------------------------------
