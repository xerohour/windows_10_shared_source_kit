// ============================================================================
//
//  Lines.cpp
//
// ============================================================================

#include "TestShapesPCH.h"

#include "TestShapes.h"
#include "d3d.h"

bool CTestShapes::GenerateLines(const int nLines)
{
	// can't have zero for any members
	if(!nLines)
		return false;

	const int    nVertices = nLines * 2;
	D3DVERTEX *  pVertices = NULL;
	const int    nIndices = nVertices;
	WORD *       pIndices = NULL;
	int          nCurVtx = 0;
	float        xPos = (nLines == 1) ? 0.0f : -0.5f;
	bool         bTopDown = true;
	const float  yTop = 0.5f;
	const float  yBottom = -0.5f;
	const float  xStep = (nLines == 1) ? 0.0f : (1.0f / nLines);

	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * nIndices);

	if(NULL == pVertices || NULL == pIndices)
	{
		if(NULL != pVertices ) free(pVertices);
		if(NULL != pIndices ) free(pIndices);
		return false;
	}

	memset(pVertices, 0, sizeof(D3DVERTEX) * nVertices);
	memset(pIndices, 0, sizeof(WORD) * nIndices);

	// set up the indices
	for(int i = 0; i < nIndices; i++)
		pIndices[i] = (WORD)i; 

	// set up some vertical lines
	for(int nCurLine = 0; nCurLine < nLines; nCurLine++)
	{
		// point 1 of the line
		pVertices[nCurVtx].x = xPos;

		if(bTopDown)
			pVertices[nCurVtx].y = yTop;
		else
			pVertices[nCurVtx].y = yBottom;

		pVertices[nCurVtx].z = 0.0f;
		pVertices[nCurVtx].nx = 0.0f;
		pVertices[nCurVtx].ny = 0.0f;
		pVertices[nCurVtx].nz = -1.0f;

		nCurVtx++;

		// point 2 of the line
		pVertices[nCurVtx].x = D3DVAL(xPos);

		if(bTopDown)
			pVertices[nCurVtx].y = yBottom;
		else
			pVertices[nCurVtx].y = yTop;

		pVertices[nCurVtx].z = 0.0f;
		pVertices[nCurVtx].nx = 0.0f;
		pVertices[nCurVtx].ny = 0.0f;
		pVertices[nCurVtx].nz = -1.0f;

		nCurVtx++;
		xPos += xStep;
		bTopDown = !bTopDown;
	}

	// assign the class data members
	m_Type      = D3DPT_LINELIST;
	m_primCount = nLines;
	m_nVertices = nVertices;
	m_pVertices = pVertices;
	m_nIndices  = nIndices;
	m_pIndices  = pIndices;
	return true;

}
