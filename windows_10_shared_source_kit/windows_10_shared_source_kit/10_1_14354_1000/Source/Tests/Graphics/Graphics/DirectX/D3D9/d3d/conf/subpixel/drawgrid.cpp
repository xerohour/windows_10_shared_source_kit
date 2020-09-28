#define D3D_OVERLOADS
#include <d3d.h>
#include "SubPixel.h"

//************************************************************************
// Internal API:    DrawGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CSubPixelTest::DrawGrid(int nTest, int nWDiv, int nHDiv, int nTexFormats)
{
	bool			bTop;
	int				r, nMapping;
	float			x, y, u, v, fRow;
	float			fRandX[100], fRandY[100];
	float			fWDiv, fHDiv, fWidth = 256.0f, fHeight = 256.0f;

	fWDiv = fWidth / nWDiv;
	fHDiv = fHeight / nHDiv;

	sprintf(msgString, "%sDiv: %dx%d, ",msgString,nWDiv,nHDiv);

	nMapping = nTest / nTexFormats;
	nMapping = (nMapping % 4) + 1;

	switch (nMapping)
	{
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping
			sprintf(msgString, "%sMap:(0,0)(1,0)(0,1)(1,1), ",msgString);
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping
			sprintf(msgString, "%sMap:(0,1)(0,0)(1,1)(1,0), ",msgString);
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping
			sprintf(msgString, "%sMap:(1,1)(0,1)(1,0)(0,0), ",msgString);
			break;
		case (4): // Use ((1,0)(1,1)(0,0)(0,1)) mapping
			sprintf(msgString, "%sMap:(1,0)(1,1)(0,0)(0,1), ",msgString);
			break;
	}

	srand(4321*(nTest+1));
	for (fRow=0.0f; fRow < fHeight; fRow+=fHDiv)
	{
		for (y=fRow; (int)y <= (int)(fRow+fHDiv); y+=fHDiv)
		{
			// Initialize variables 
			bTop = (y == fRow);

			// The bottom half needs to handle the left edge
			if (!bTop)
			{
				u = CalcU(0.0f, y-fHDiv, fWidth, nMapping);
				v = CalcV(0.0f, y-fHDiv, fHeight, nMapping);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(0.0f+9.5f, y-fHDiv+9.5f, 0.9f),(1.0f/0.9f), RGBA_MAKE(255,255,255,255),0, u, v);
			}

			for (r=0, x=0.0f; x < fWidth; r++, x+=fWDiv)
			{
				u = CalcU(x, y, fWidth, nMapping);
				v = CalcV(x, y, fHeight, nMapping);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x+9.5f, y+9.5f, 0.9f),(1.0f/0.9f), RGBA_MAKE(255,255,255,255),0, u, v);

				// Random point
				if (bTop)
				{
					// Generate a pseudo random number for the random point values
					fRandX[r] = fWDiv * ((float)rand()/(float)RAND_MAX);
					fRandY[r] = fHDiv * ((float)rand()/(float)RAND_MAX);

					u = CalcU(x+fRandX[r], y+fRandY[r], fWidth, nMapping);
					v = CalcV(x+fRandX[r], y+fRandY[r], fHeight, nMapping);
					VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x+fRandX[r]+9.5f, y+fRandY[r]+9.5f, 0.9f),(1.0f/0.9f), RGBA_MAKE(255,255,255,255),0, u,v);
				}
				else
				{
					// For bottom, get random point values generated with top strip
					u = CalcU(x+fRandX[r], y-fHDiv+fRandY[r], fWidth, nMapping);
					v = CalcV(x+fRandX[r], y-fHDiv+fRandY[r], fHeight, nMapping);
					VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x+fRandX[r]+9.5f, y-fHDiv+fRandY[r]+9.5f, 0.9f),(1.0f/0.9f), RGBA_MAKE(255,255,255,255),0, u,v);
				}
			}

			u = CalcU(x, y, fWidth, nMapping);
			v = CalcV(x, y, fHeight, nMapping);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x+9.5f, y+9.5f, 0.9f),(1.0f/0.9f), RGBA_MAKE(255,255,255,255),0, u,v);

			// The top half needs to handle the right edge
			if (bTop)
			{
				u = CalcU(x, y+fHDiv, fWidth, nMapping);
				v = CalcV(x, y+fHDiv, fHeight, nMapping);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(x+9.5f, y+fHDiv+9.5f, 0.9f),(1.0f/0.9f), RGBA_MAKE(255,255,255,255),0, u,v);
			}
		}
	}
}

float CSubPixelTest::CalcU(float xVal, float yVal, float fPixels, int nMapping)
{
	int nScale;
	float fScale, u = 0.0f;

	nScale = 1;
	fScale = 1.0f * nScale;

	switch (nMapping)
	{
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping
			u = (fScale / fPixels) * xVal;
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping
			u = (fScale / fPixels) * yVal;
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping
			u = 1.0f - ((fScale / fPixels) * xVal);
			break;
		case (4): // Use ((1,0)(1,1)(0,0)(0,1)) mapping
			u = 1.0f - ((fScale / fPixels) * yVal);
			break;
	}

	return u;
}

float CSubPixelTest::CalcV(float xVal, float yVal, float fPixels, int nMapping)
{
	int nScale;
	float fScale, v = 0.0f;

	nScale = 1;
	fScale = 1.0f * nScale;

	switch (nMapping)
	{
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping
			v = (fScale / fPixels) * yVal;
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping
			v = 1.0f - ((fScale / fPixels) * xVal);
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping
			v = 1.0f - ((fScale / fPixels) * yVal);
			break;
		case (4): // Use ((1,0)(1,1)(0,0)(0,1)) mapping
			v = (fScale / fPixels) * xVal;
			break;
	}

	return v;
}
