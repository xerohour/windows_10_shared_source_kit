#define D3D_OVERLOADS
#include <d3d.h>
#include "Perspective.h"

//************************************************************************
// Internal API:    DrawTexGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CPerspectiveTest::DrawTexGrid(int nTest)
{
	int nMesh, nScale, nMapping;
	float fScale, fZValue[4];
    DWORD dwColor = RGBA_MAKE(255,255,255,255);

	// Initialize the interesting variables
  	nMesh = ((nTest-1) / 16) + 1;

    nMapping = ((nTest-1) / 4) + 1;
	nMapping = nMapping % 4;

	if (nMapping == 0)
		nMapping = 4;

	nScale = nTest;
	nScale = nScale % 4;

	if (nScale == 0)
		nScale = 4;

	fScale = 1.0f * nScale;

    // Setup the mesh
	switch (nMesh)
	{
		case (1): 
			fZValue[0] = 0.1f;
			fZValue[1] = 0.9f;
			fZValue[2] = 0.1f;
			fZValue[3] = 0.9f;
			break;
		case (2): 
			fZValue[0] = 0.1f;
			fZValue[1] = 0.5f;
			fZValue[2] = 0.5f;
			fZValue[3] = 0.9f;
			break;
		case (3): 
			fZValue[0] = 0.1f;
			fZValue[1] = 0.1f;
			fZValue[2] = 0.9f;
			fZValue[3] = 0.9f;
			break;
		case (4): 
			fZValue[0] = 0.5f;
			fZValue[1] = 0.1f;
			fZValue[2] = 0.9f;
			fZValue[3] = 0.5f;
			break;
		case (5): 
			fZValue[0] = 0.9f;
			fZValue[1] = 0.1f;
			fZValue[2] = 0.9f;
			fZValue[3] = 0.1f;
			break;
		case (6): 
			fZValue[0] = 0.9f;
			fZValue[1] = 0.5f;
			fZValue[2] = 0.5f;
			fZValue[3] = 0.1f;
			break;
		case (7): 
			fZValue[0] = 0.9f;
			fZValue[1] = 0.9f;
			fZValue[2] = 0.1f;
			fZValue[3] = 0.1f;
			break;
		case (8): 
			fZValue[0] = 0.5f;
			fZValue[1] = 0.9f;
			fZValue[2] = 0.1f;
			fZValue[3] = 0.5f;
			break;
	}

	sprintf(msgString, "%sZval:(%.1f)(%.1f)(%.1f)(%.1f), ",msgString,fZValue[2],fZValue[0],fZValue[3],fZValue[1]);

	switch (nMapping)
	{
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping
   			sprintf(msgString, "%sMap:(0,0)(%d,0)(0,%d)(%d,%d), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor, 0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor, 0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f, 265.5f, fZValue[3]),(1.0f / fZValue[3]), dwColor, 0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor, 0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,   9.5f, fZValue[0]),(1.0f / fZValue[0]), dwColor, 0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor, 0, fScale,fScale);
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping
   			sprintf(msgString, "%sMap:(0,%d)(0,0)(%d,%d)(%d,0), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor, 0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor, 0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f, 265.5f, fZValue[3]),(1.0f / fZValue[3]), dwColor, 0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor, 0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,   9.5f, fZValue[0]),(1.0f / fZValue[0]), dwColor, 0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor, 0, fScale,  0.0f);
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping
   			sprintf(msgString, "%sMap:(%d,%d)(0,%d)(%d,0)(0,0), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor, 0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor, 0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f, 265.5f, fZValue[3]),(1.0f / fZValue[3]), dwColor, 0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor, 0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,   9.5f, fZValue[0]),(1.0f / fZValue[0]), dwColor, 0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor, 0, 0.0f,    0.0f);
			break;
		case (4): // Use ((1,0)(1,1)(0,0)(0,1)) mapping
   			sprintf(msgString, "%sMap:(%d,0)(%d,%d)(0,0)(0,%d), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor, 0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor, 0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f, 265.5f, fZValue[3]),(1.0f / fZValue[3]), dwColor, 0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor, 0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,   9.5f, fZValue[0]),(1.0f / fZValue[0]), dwColor, 0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor, 0, 0.0f,  fScale);
			break;
	}
}

//************************************************************************
// Internal API:    DrawColorGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CPerspectiveTest::DrawColorGrid(int nTest)
{
    DWORD dwColor[4] = {RGBA_MAKE(0,0,0,255),RGBA_MAKE(0,0,0,255),RGBA_MAKE(0,0,0,255),RGBA_MAKE(0,0,0,255)};
	float fZValue[4] = {0.0f,0.0f,0.0f,0.0f};
	int nMesh, nRotate;
	TCHAR szColor0[20], szColor1[20], szColor2[20], szColor3[20];

	// Initialize the interesting variables
   	nMesh = ((nTest-1) / 4) + 1;

    nRotate = nTest;
	nRotate = nRotate % 4;

	if (nRotate == 0)
		nRotate = 4;

    // Clear out the color strings
    szColor0[0] = '\0';
    szColor1[0] = '\0';
    szColor2[0] = '\0';
    szColor3[0] = '\0';

    // Setup the mesh
	switch (nMesh)
	{
		case (1): 
			fZValue[0] = 0.1f;
			fZValue[1] = 0.9f;
			fZValue[2] = 0.1f;
			fZValue[3] = 0.9f;

            sprintf(szColor0, "%sWhite",szColor0);
            dwColor[0] = RGBA_MAKE(255,255,255,255);

            sprintf(szColor1, "%sRed",szColor1);
            dwColor[1] = RGBA_MAKE(255,0,0,255);

            sprintf(szColor2, "%sGreen",szColor2);
            dwColor[2] = RGBA_MAKE(0,255,0,255);

            sprintf(szColor3, "%sBlue",szColor3);
            dwColor[3] = RGBA_MAKE(0,0,255,255);
			break;
		case (2): 
			fZValue[0] = 0.1f;
			fZValue[1] = 0.5f;
			fZValue[2] = 0.5f;
			fZValue[3] = 0.9f;

            sprintf(szColor0, "%sYellow",szColor0);
            dwColor[0] = RGBA_MAKE(255,255,0,255);

            sprintf(szColor1, "%sMagenta",szColor1);
            dwColor[1] = RGBA_MAKE(255,0,255,255);

            sprintf(szColor2, "%sCyan",szColor2);
            dwColor[2] = RGBA_MAKE(0,255,255,255);

            sprintf(szColor3, "%sBlack",szColor3);
            dwColor[3] = RGBA_MAKE(0,0,0,255);
			break;
		case (3): 
			fZValue[0] = 0.1f;
			fZValue[1] = 0.1f;
			fZValue[2] = 0.9f;
			fZValue[3] = 0.9f;

            sprintf(szColor0, "%sWhite",szColor0);
            dwColor[0] = RGBA_MAKE(255,255,255,255);

            sprintf(szColor1, "%sRed",szColor1);
            dwColor[1] = RGBA_MAKE(255,0,0,255);

            sprintf(szColor2, "%sYellow",szColor2);
            dwColor[2] = RGBA_MAKE(255,255,0,255);

            sprintf(szColor3, "%sMagenta",szColor3);
            dwColor[3] = RGBA_MAKE(255,0,255,255);
			break;
		case (4): 
			fZValue[0] = 0.5f;
			fZValue[1] = 0.1f;
			fZValue[2] = 0.9f;
			fZValue[3] = 0.5f;

            sprintf(szColor0, "%sGreen",szColor0);
            dwColor[0] = RGBA_MAKE(0,255,0,255);

            sprintf(szColor1, "%sBlue",szColor1);
            dwColor[1] = RGBA_MAKE(0,0,255,255);

            sprintf(szColor2, "%sCyan",szColor2);
            dwColor[2] = RGBA_MAKE(0,255,255,255);

            sprintf(szColor3, "%sBlack",szColor3);
            dwColor[3] = RGBA_MAKE(0,0,0,255);
			break;
		case (5): 
			fZValue[0] = 0.9f;
			fZValue[1] = 0.1f;
			fZValue[2] = 0.9f;
			fZValue[3] = 0.1f;

            sprintf(szColor0, "%sWhite",szColor0);
            dwColor[0] = RGBA_MAKE(255,255,255,255);

            sprintf(szColor1, "%sRed",szColor1);
            dwColor[1] = RGBA_MAKE(255,0,0,255);

            sprintf(szColor2, "%sCyan",szColor2);
            dwColor[2] = RGBA_MAKE(0,255,255,255);

            sprintf(szColor3, "%sBlack",szColor3);
            dwColor[3] = RGBA_MAKE(0,0,0,255);
			break;
		case (6): 
			fZValue[0] = 0.9f;
			fZValue[1] = 0.5f;
			fZValue[2] = 0.5f;
			fZValue[3] = 0.1f;

            sprintf(szColor0, "%sGreen",szColor0);
            dwColor[0] = RGBA_MAKE(0,255,0,255);

            sprintf(szColor1, "%sBlue",szColor1);
            dwColor[1] = RGBA_MAKE(0,0,255,255);

            sprintf(szColor2, "%sYellow",szColor2);
            dwColor[2] = RGBA_MAKE(255,255,0,255);

            sprintf(szColor3, "%sMagenta",szColor3);
            dwColor[3] = RGBA_MAKE(255,0,255,255);
			break;
		case (7): 
			fZValue[0] = 0.9f;
			fZValue[1] = 0.9f;
			fZValue[2] = 0.1f;
			fZValue[3] = 0.1f;

            sprintf(szColor0, "%sWhite",szColor0);
            dwColor[0] = RGBA_MAKE(255,255,255,255);

            sprintf(szColor1, "%sGreen",szColor1);
            dwColor[1] = RGBA_MAKE(0,255,0,255);

            sprintf(szColor2, "%sYellow",szColor2);
            dwColor[2] = RGBA_MAKE(255,255,0,255);

            sprintf(szColor3, "%sCyan",szColor3);
            dwColor[3] = RGBA_MAKE(0,255,255,255);
			break;
		case (8): 
			fZValue[0] = 0.5f;
			fZValue[1] = 0.9f;
			fZValue[2] = 0.1f;
			fZValue[3] = 0.5f;

            sprintf(szColor0, "%sRed",szColor0);
            dwColor[0] = RGBA_MAKE(255,0,0,255);

            sprintf(szColor1, "%sBlue",szColor1);
            dwColor[1] = RGBA_MAKE(0,0,255,255);

            sprintf(szColor2, "%sMagenta",szColor2);
            dwColor[2] = RGBA_MAKE(255,0,255,255);

            sprintf(szColor3, "%sBlack",szColor3);
            dwColor[3] = RGBA_MAKE(0,0,0,255);
			break;
	}

	sprintf(msgString, "%sZval:(%.1f)(%.1f)(%.1f)(%.1f), ",msgString,fZValue[2],fZValue[0],fZValue[3],fZValue[1]);

	switch (nRotate)
	{
		case (1): // Rotate vertex colors
   			sprintf(msgString, "%sColor:(%s)(%s)(%s)(%s)",msgString,szColor0,szColor1,szColor2,szColor3);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor[0], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor[3], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f, 265.5f, fZValue[3]),(1.0f / fZValue[3]), dwColor[2], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor[0], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,   9.5f, fZValue[0]),(1.0f / fZValue[0]), dwColor[1], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor[3], 0, 0.0f,0.0f);
			break;
		case (2): // Rotate vertex colors
   			sprintf(msgString, "%sColor:(%s)(%s)(%s)(%s)",msgString,szColor2,szColor0,szColor3,szColor1);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor[2], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor[1], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f, 265.5f, fZValue[3]),(1.0f / fZValue[3]), dwColor[3], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor[2], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,   9.5f, fZValue[0]),(1.0f / fZValue[0]), dwColor[0], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor[1], 0, 0.0f,0.0f);
			break;
		case (3): // Rotate vertex colors
   			sprintf(msgString, "%sColor:(%s)(%s)(%s)(%s)",msgString,szColor1,szColor3,szColor0,szColor2);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor[1], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor[2], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f, 265.5f, fZValue[3]),(1.0f / fZValue[3]), dwColor[0], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor[1], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,   9.5f, fZValue[0]),(1.0f / fZValue[0]), dwColor[3], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor[2], 0, 0.0f,0.0f);
			break;
		case (4): // Rotate vertex colors
   			sprintf(msgString, "%sColor:(%s)(%s)(%s)(%s)",msgString,szColor3,szColor2,szColor1,szColor0);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor[3], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor[0], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f, 265.5f, fZValue[3]),(1.0f / fZValue[3]), dwColor[1], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  9.5f,   9.5f, fZValue[2]),(1.0f / fZValue[2]), dwColor[3], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f,   9.5f, fZValue[0]),(1.0f / fZValue[0]), dwColor[2], 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(265.5f, 265.5f, fZValue[1]),(1.0f / fZValue[1]), dwColor[0], 0, 0.0f,0.0f);
			break;
	}
}

//************************************************************************
// Internal API:    DrawTriangle
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CPerspectiveTest::DrawTriangle(int nTest)
{
	float fZValue[3];

	// Setup the mesh
	switch (nTest)
	{
		case 1:
			fZValue[0] = 0.99f;
			fZValue[1] = 0.01f;
			fZValue[2] = 0.01f;
			break;
		case 11:
			fZValue[0] = 0.01f;
			fZValue[1] = 0.99f;
			fZValue[2] = 0.99f;
			break;
		default:
			fZValue[0] = 1.0f - (0.1f * (nTest-1));
			fZValue[1] = 0.1f * (nTest-1);
			fZValue[2] = 0.1f * (nTest-1);
			break;
	}

	sprintf(msgString, "%sZval:(%.1f)(%.1f)(%.1f), ",msgString,fZValue[0],fZValue[2],fZValue[1]);

    if (pTexture)
    {
	    int nScale = 1;
	    float fScale= (1.0f * nScale) - 0.001f;

        sprintf(msgString, "%sMap:(0,0.5)(1,0)(1,1)",msgString);

	    VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(128.0f,  0.0f,fZValue[0]),(1.0f / fZValue[0]), RGBA_MAKE(255,255,255,255),0, 0.000f,(fScale/2));
	    VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(256.0f,256.0f,fZValue[1]),(1.0f / fZValue[1]), RGBA_MAKE(255,255,255,255),0, fScale,0.000f);
	    VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.0f,256.0f,fZValue[2]),(1.0f / fZValue[2]), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
    }
    else
    {
        sprintf(msgString, "%sColor:(Green)(Red)(Blue)",msgString);

        VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(128.0f,  0.0f,fZValue[0]),(1.0f / fZValue[0]), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
	    VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(256.0f,256.0f,fZValue[1]),(1.0f / fZValue[1]), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
	    VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(  0.0f,256.0f,fZValue[2]),(1.0f / fZValue[2]), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
    }
}
