#define D3D_OVERLOADS
#include <d3d.h>
#include "BasicRst.h"

//************************************************************************
// Fill Test functions

CFillTest::CFillTest()
{
	m_szTestName = "BasicRst Fill";
	m_szCommandKey = "Fill";
}

CFillTest::~CFillTest()
{
}

UINT CFillTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1,FILL_TESTS);

    // Set compare function to require 95%
	m_pFramework->GetImageCmp()->SetPassRequired(0.95f);

	return D3DTESTINIT_RUN;
}

bool CFillTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build some triangles.
	DrawFillTests(uTestNum);

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

    return true;
}

//************************************************************************
// Internal API:    DrawFillTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CFillTest::DrawFillTests(int nTest)
{
	int i;

	// Color of triangles
	D3DCOLOR Color[] = { RGBA_MAKE(255,0,0,255),		// Red
						 RGBA_MAKE(0,255,0,255),		// Green
						 RGBA_MAKE(0,0,255,255),		// Blue
						 RGBA_MAKE(255,255,0,255),		// Yellow
						 RGBA_MAKE(255,0,255,255) };	// Magenta

	switch (nTest)
	{
		case (1):
			sprintf(msgString, "%sRed triangle (x = y = 0)", msgString);

			// Red triangle, Right hand edge from x & y equal zero.
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,1.0f);
			break;
		case (2):
			sprintf(msgString, "%sGreen triangle (x = 0)", msgString);

			// Green triangle, Left hand edge from x equal zero.
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
			break;
		case (3):
			sprintf(msgString, "%sBlue triangle (y = 0)", msgString);

			// Blue triangle, Right hand edge from y equal zero.
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 0.0f,1.0f);
			break;
		case (4):
			sprintf(msgString, "%sYellow triangle (x = y != 0)", msgString);

			// Yellow triangle, Left hand edge from x & y not equal zero.
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 0.0f,1.0f);
			break;
		case (5):
			sprintf(msgString, "%sMagenta triangle (x = 0)", msgString);

			// Magenta triangle, Left edge from x equal zero.
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,255,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, 5.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,255,255), 0, 1.0f,0.5f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,255,255), 0, 0.0f,1.0f);
			break;
		case (6):
			sprintf(msgString, "%sCyan triangle (y = 0)", msgString);

			// Cyan triangle, Top edge from y equal zero.
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,255,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,255,255), 0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 5.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,255,255), 0, 0.5f,1.0f);
			break;
		case (7):
			sprintf(msgString, "%sWhite triangle (y != 0)", msgString);

			// White triangle, Bottom edge from y not equal zero.
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 5.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255), 0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255), 0, 0.0f,1.0f);
			break;
		case (8):
			sprintf(msgString, "%sRed triangle (x != 0)", msgString);

			// Red triangle, Right edge from x not equal zero.
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f, 5.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,0.5f);
			break;
		case (9):
			sprintf(msgString, "%sGreen triangle (25x25 pixels)", msgString);

			// Green Triangle 25x25 pixels
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 0.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(25.0f, 0.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(25.0f,25.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);
			break;
		case (10): 
			sprintf(msgString, "%sLong left edge, Short bottom edge", msgString);

			// Thin Left Bottom triangles
			for (i=0; i<10; i++)
			{
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i),   10.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,0.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i)+i,260.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 1.0f,1.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i),  260.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,1.0f);
			}
			break;
		case (11): 
			sprintf(msgString, "%sLong left edge, Short top edge", msgString);

			// Thin Left Top triangles
			for (i=0; i<10; i++)
			{
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i),   10.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,0.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i)+i, 10.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 1.0f,1.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i),  260.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,1.0f);
			}
			break;
		case (12):
			sprintf(msgString, "%sLong top edge, Short right edge", msgString);

			// Thin Top Right triangles
			for (i=0; i<10; i++)
			{
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,10.0f+(20*i),  0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,0.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(290.0f,10.0f+(20*i),  0.9f),(1.0f / 0.9f), Color[i%5], 0, 1.0f,1.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(290.0f,10.0f+(20*i)+i,0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,1.0f);
			}
			break;
		case (13):
			sprintf(msgString, "%sLong top edge, Short left edge", msgString);

			// Thin Top Left triangles
			for (i=0; i<10; i++)
			{
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,10.0f+(20*i),  0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,0.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(290.0f,10.0f+(20*i),  0.9f),(1.0f / 0.9f), Color[i%5], 0, 1.0f,1.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,10.0f+(20*i)+i,0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,1.0f);
			}
			break;
		case (14):
			sprintf(msgString, "%sLong right edge, Short bottom edge", msgString);

			// Thin Right Bottom triangles
			for (i=0; i<10; i++)
			{
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i),   10.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,0.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i),  260.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 1.0f,1.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i)-i,260.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,1.0f);
			}
			break;
		case (15):
			sprintf(msgString, "%sLong right edge, Short top edge", msgString);

			// Thin Right Top triangles
			for (i=0; i<10; i++)
			{
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i)-i, 10.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,0.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i),   10.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 1.0f,1.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f+(20*i),  260.0f,0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,1.0f);
			}
			break;
		case (16):
			sprintf(msgString, "%sLong bottom edge, Short right edge", msgString);

			// Thin Bottom Right triangles
			for (i=0; i<10; i++)
			{
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,10.0f+(20*i),  0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,0.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(290.0f,10.0f+(20*i)-i,0.9f),(1.0f / 0.9f), Color[i%5], 0, 1.0f,1.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(290.0f,10.0f+(20*i),  0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,1.0f);
			}
			break;
		case (17):
			sprintf(msgString, "%sLong bottom edge, Short left edge", msgString);

			// Thin Bottom Left triangles
			for (i=0; i<10; i++)
			{
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,10.0f+(20*i)-i,0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,0.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(290.0f,10.0f+(20*i),  0.9f),(1.0f / 0.9f), Color[i%5], 0, 1.0f,1.0f);
				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 10.0f,10.0f+(20*i),  0.9f),(1.0f / 0.9f), Color[i%5], 0, 0.0f,1.0f);
			}
			break;
		case (18):
			sprintf(msgString, "%sEight triangles", msgString);

			// Right Angled with Left and Bottom - Red
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(50.0f,50.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f,50.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,1.0f);
			
			// Equaladiral with edge on Left - Green
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 60.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(100.0f,30.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 60.0f,50.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);
			
			// Right Angled with Left and Top - Blue
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,50.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 0.0f,1.0f);
			
			// Equaladiral with edge on Top - Yellow
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(160.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(200.0f,10.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(180.0f,50.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 0.0f,1.0f);
			
			// Right Angled with Right and Bottom - Red
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(50.0f,100.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(50.0f,140.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(10.0f,140.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,1.0f);
			
			// Equaladiral with edge on Right - Green
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(100.0f,100.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(100.0f,140.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR( 60.0f,120.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);
			
			// Right Angled with Right and Top - Blue
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(110.0f,100.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f,100.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(150.0f,140.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 0.0f,1.0f);
			
			// Equaladiral with edge on Bottom - Yellow
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(180.0f,100.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(200.0f,140.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(160.0f,140.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 0.0f,1.0f);
			break;
		case (19):
			sprintf(msgString, "%s5x5 grid of red & green trianges", msgString);

			{
				// Red and Green Grid
				int nRows=5, nCols=5, nPixels=50;

				for (int y=0; y < nRows*nPixels; y+=nPixels)
				{
					for (int x=0; x < nCols*nPixels; x+=nPixels)
					{
						// Red Triangle
						VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((float)x,        (float)y,        0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,0.0f);
						VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((float)x+nPixels,(float)y+nPixels,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 1.0f,1.0f);
						VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((float)x,        (float)y+nPixels,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,1.0f);
			
						// Green Triangle
						VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((float)x,        (float)y,        0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,0.0f);
						VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((float)x+nPixels,(float)y,        0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
						VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((float)x+nPixels,(float)y+nPixels,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);
					}
				}
			}
			break;
		case (20):
			sprintf(msgString, "%sSubPixel Wagon Wheel", msgString);

			// SubPixel Wagon Wheel test
			float fDelta = 50.0f;

			// Top - Red
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(0.5f,0.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(2.5f,0.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(1.5f,2.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,1.0f);
			
			// Left - Green
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(0.5f,0.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(1.5f,2.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(0.5f,4.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);
			
			// Right  - Blue
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(2.5f,0.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(2.5f,4.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(1.5f,2.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 0.0f,1.0f);
			
			// Bottom - Yellow
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(1.5f,2.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(2.5f,4.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(0.5f,4.5f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 0.0f,1.0f);

			// Top - Red
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+1.0f,1.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+3.0f,1.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+2.0f,3.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255), 0, 0.0f,1.0f);
			
			// Left - Green
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+1.0f,1.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+2.0f,3.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+1.0f,5.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);
			
			// Right  - Blue
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+3.0f,1.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+3.0f,5.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+2.0f,3.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255), 0, 0.0f,1.0f);
			
			// Bottom - Yellow
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+2.0f,3.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+3.0f,5.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR(fDelta+1.0f,5.0f,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,0,255), 0, 0.0f,1.0f);
			break;
	}
}
