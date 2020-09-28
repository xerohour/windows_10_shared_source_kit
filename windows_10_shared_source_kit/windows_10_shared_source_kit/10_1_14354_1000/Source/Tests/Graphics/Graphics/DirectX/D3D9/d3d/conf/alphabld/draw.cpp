#define D3D_OVERLOADS
#include <d3d.h>
#include "AlphaBld.h"

//************************************************************************
// Internal API:    DrawBackground
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CAlphaBldTest::DrawBackground(void)
{
	float			fWidth, fHeight, fX, fY, fAlpha;
	D3DCOLOR		Color[2];
	DWORD			dwCount;
	bool			bWhite;
	int				i, j;

	dwCount = 0;
	bWhite = true;
	fWidth = m_pCurrentMode->nBackBufferWidth / 10.0f;
	fHeight = m_pCurrentMode->nBackBufferHeight / 10.0f;
    fAlpha = 255 / 10.0f;

	for (i=0; i < 10; i++)
	{
		fX = fWidth*i;
		bWhite = !bWhite;

		for (j=0; j < 10; j++)
		{
            Color[0] = RGBA_MAKE(0,0,0,(int)fAlpha*j);
            Color[1] = RGBA_MAKE(255,255,255,(int)fAlpha*j);
			fY = fHeight*j;
			bWhite = !bWhite;

			BackgroundList[dwCount++] = D3DTLVERTEX(D3DVECTOR(fX+fWidth,fY,         0.9f), (1.0f / 0.9f), Color[bWhite],0, 0.0f,0.0f);
			BackgroundList[dwCount++] = D3DTLVERTEX(D3DVECTOR(fX+fWidth,fY+fHeight, 0.9f), (1.0f / 0.9f), Color[bWhite],0, 0.0f,0.0f);
			BackgroundList[dwCount++] = D3DTLVERTEX(D3DVECTOR(fX,       fY,         0.9f), (1.0f / 0.9f), Color[bWhite],0, 0.0f,0.0f);
			BackgroundList[dwCount++] = D3DTLVERTEX(D3DVECTOR(fX,       fY+fHeight, 0.9f), (1.0f / 0.9f), Color[bWhite],0, 0.0f,0.0f);
		}
	}
}

//************************************************************************
// Internal API:    DrawGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************
#ifdef FAST_TEST
void CAlphaBldTest::DrawGrid(int frontAlpha, int backAlpha, int gridLeft, int gridTop, int gridRight, int gridBottom)
{
	DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
	DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;

	// Multicolored grid
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(gridLeft), (D3DVALUE)(int)(dwHeight - gridTop), 0.6f), (1.0f / 0.6f), RGBA_MAKE(255, 0, 0, backAlpha), 0, 0.0f, 0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(gridLeft), (D3DVALUE)(int)(dwHeight - (gridBottom)), 0.6f), (1.0f / 0.6f), RGBA_MAKE(255, 255, 0, backAlpha), 0, 0.0f, 0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(gridRight), (D3DVALUE)(int)(dwHeight - (gridTop)), 0.6f), (1.0f / 0.6f), RGBA_MAKE(0, 128, 255, backAlpha), 0, 0.0f, 0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(gridRight), (D3DVALUE)(int)(dwHeight - (gridBottom)), 0.6f), (1.0f / 0.6f), RGBA_MAKE(0, 0, 255, backAlpha), 0, 0.0f, 0.0f);

	// Gray Rectangle
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(gridLeft), (D3DVALUE)(int)(dwHeight - gridTop), 0.6f), (1.0f / 0.6f), RGBA_MAKE(128, 128, 128, frontAlpha), 0, 0.0f, 0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(gridLeft), (D3DVALUE)(int)(dwHeight - (gridBottom)), 0.6f), (1.0f / 0.6f), RGBA_MAKE(128, 128, 128, frontAlpha), 0, 0.0f, 0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(gridRight), (D3DVALUE)(int)(dwHeight - (gridTop)), 0.6f), (1.0f / 0.6f), RGBA_MAKE(128, 128, 128, frontAlpha), 0, 0.0f, 0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(gridRight), (D3DVALUE)(int)(dwHeight - (gridBottom)), 0.6f), (1.0f / 0.6f), RGBA_MAKE(128, 128, 128, frontAlpha), 0, 0.0f, 0.0f);
}
#else
void CAlphaBldTest::DrawGrid(int nTest, int nMesh)
{
	DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
	DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;

	// Multicolored grid
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth), (D3DVALUE)(int)(dwHeight - (0.099f * dwHeight)), 0.6f), (1.0f / 0.6f), RGBA_MAKE(255,0,0,nAlphaValue[nMesh]),  0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth), (D3DVALUE)(int)(dwHeight - (0.9f * dwHeight)),   0.6f), (1.0f / 0.6f), RGBA_MAKE(255,128,0,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.26f * dwWidth), (D3DVALUE)(int)(dwHeight - (0.099f * dwHeight)), 0.6f), (1.0f / 0.6f), RGBA_MAKE(128,128,0,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.42f * dwWidth), (D3DVALUE)(int)(dwHeight - (0.9f * dwHeight)),   0.6f), (1.0f / 0.6f), RGBA_MAKE(128,255,0,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.58f * dwWidth), (D3DVALUE)(int)(dwHeight - (0.099f * dwHeight)), 0.6f), (1.0f / 0.6f), RGBA_MAKE(0,255,128,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.74f * dwWidth), (D3DVALUE)(int)(dwHeight - (0.9f * dwHeight)),   0.6f), (1.0f / 0.6f), RGBA_MAKE(0,128,128,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.901f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.099f * dwHeight)), 0.6f), (1.0f / 0.6f), RGBA_MAKE(0,128,255,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.901f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.9f * dwHeight)),   0.6f), (1.0f / 0.6f), RGBA_MAKE(0,0,255,nAlphaValue[nMesh]),  0, 0.0f,0.0f);

	// White triangle (Bottom Left)
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.03f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.03f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.20f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.37f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.37f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.03f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);

	// Black triangle (Bottom Right)
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.63f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.03f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.80f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.37f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.97f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.03f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);

	// White/Black rectangle (Center)
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.33f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.32f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.67f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.58f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.67f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.32f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);

	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.33f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.32f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.33f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.58f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.67f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.58f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);

	// Black triangle (Top Left)
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.03f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.63f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.20f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.97f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.37f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.63f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);

	// White triangle (Top Right)
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.63f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.63f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.80f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.97f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.97f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.63f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
}
#endif


//************************************************************************
// Internal API:    DrawCircle
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CAlphaBldTest::DrawCircle(LPD3DVECTOR pCenter, float Radius, int nDivisions, D3DCOLOR Color)
{
    int             i, NumVertices;
    float           Theta, Rads;

  	dwVertexCount = 0;
	NumVertices = nDivisions + 2;
    Theta = 360.0f / nDivisions;

    // Initialize the center point
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(pCenter->x, pCenter->y, pCenter->z), (1.0f / pCenter->z), Color, 0, 0.0f,0.0f);

    for (i=1; i<NumVertices; i++)
    {
        // cos takes a radian value, so convert.
        Rads = (Theta * (i-1) * pi)/180.0f;

		VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((pCenter->x + Radius * cos(Rads)), (pCenter->y + Radius * sin(Rads)), pCenter->z), (1.0f / pCenter->z), Color, 0, 0.0f,0.0f);
    }

	RenderPrimitive(D3DPT_TRIANGLEFAN,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);
}
