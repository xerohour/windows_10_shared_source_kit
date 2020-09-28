#define D3D_OVERLOADS
#include <d3d.h>
#include "BasicRst.h"

//************************************************************************
// MeshTL Test functions

CMeshTLTest::CMeshTLTest()
{
	m_szTestName = "BasicRst Mesh TL Vertex";
	m_szCommandKey = "MeshTL";
}

CMeshTLTest::~CMeshTLTest()
{
}

UINT CMeshTLTest::TestInitialize(void)
{
	// Initialize the Width and Height
	dwWidth = m_pCurrentMode->nBackBufferWidth;
	dwHeight = m_pCurrentMode->nBackBufferHeight;

	// Setup the Test range
	SetTestRange(1,MESH_TL_TESTS);

    // Set compare function to require 85%
	m_pFramework->GetImageCmp()->SetPassRequired(0.85f);

	return D3DTESTINIT_RUN;
}

bool CMeshTLTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build some triangles.
	if (uTestNum < MESH_TL_TESTS)
		DrawMeshTests(uTestNum);
	else
		sprintf(msgString, "%sTriangle Fan Circles: Various Colors", msgString);

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

	if (BeginScene())
	{
		if (uTestNum < 16)
		{
			// Use Triangle Strips
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);
		}
		else if (uTestNum < 18)
		{
			// Use Triangle Strips
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,VertexList,dwVertexCount/2,NULL,0,0);
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,VertexList+8,dwVertexCount/2,NULL,0,0);
		}
		else if (uTestNum < 20)
		{
			// Use Triangle Fans
			RenderPrimitive(D3DPT_TRIANGLEFAN,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);
		}
		else
		{
			D3DVECTOR Center;
			float     fRadius;
			D3DCOLOR  ColorArray[] = {RGBA_MAKE(255,255,255,255),
									  RGBA_MAKE(255,0,0,255), RGBA_MAKE(200,200,0,255),
									  RGBA_MAKE(0,255,0,255), RGBA_MAKE(0,200,200,255),
									  RGBA_MAKE(0,0,255,255), RGBA_MAKE(255,0,255,255)};

			fRadius = ((dwWidth < dwHeight) ? dwWidth : dwHeight) * 0.2f;

			Center.z = 0.5f;

			for (int i=0; i<6; i++)
			{
				Center.x = (dwWidth / 2.0f) + (float)(fRadius * cos(1.0472f * i));
				Center.y = (dwHeight / 2.0f) + (float)(fRadius * sin(1.0472f * i));

				DrawCircle(&Center, fRadius, 360, ColorArray[i+1]);
			}

			Center.x = (dwWidth / 2.0f);
			Center.y = (dwHeight / 2.0f);
			DrawCircle(&Center, (fRadius / 2.0f), 360, ColorArray[0]);
		}

		EndScene();
	}	

    return true;
}

void CMeshTLTest::SceneRefresh(void)
{
}

//************************************************************************
// Internal API:    DrawCircle
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CMeshTLTest::DrawCircle(LPD3DVECTOR pCenter, float Radius, int nDivisions, D3DCOLOR Color)
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

//************************************************************************
// Internal API:    DrawMeshTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CMeshTLTest::DrawMeshTests(int nTest)
{
	switch (nTest)
	{
		// Straight Triangle Strip (Horizontal), D3DPT_TRIANGLESTRIP
		//
		case (1):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Red", msgString);

			// Red Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.26f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.42f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.58f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.74f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.90f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.90f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			break;
		case (2):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Green", msgString);

			// Green Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.26f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.42f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.58f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.74f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.90f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.90f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			break;
		case (3):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Blue", msgString);

			// Blue Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.26f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.42f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.58f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.74f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.90f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.90f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			break;
		case (4):	
			sprintf(msgString, "%sHorizontal Triangle Strip: White", msgString);

			// White Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.26f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.42f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.58f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.74f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.90f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.90f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			break;
		case (5):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Multicolor", msgString);

			// Multicolor Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),  0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.10f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,128,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.26f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(128,128,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.42f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(128,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.58f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,128,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.74f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,128,128,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.90f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,128,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.90f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),  0, 0.0f,0.0f);
			break;
		//
		// Straight Triangle Strip (Vertical), D3DPT_TRIANGLESTRIP
		//
		case (6):	
			sprintf(msgString, "%sVertical Triangle Strip: Red", msgString);

			// Red Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.10f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.10f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.42f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.26f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.74f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.58f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.90f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.90f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			break;
		case (7):	
			sprintf(msgString, "%sVertical Triangle Strip: Green", msgString);

			// Green Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.10f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.10f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.42f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.26f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.74f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.58f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.90f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.90f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			break;
		case (8):	
			sprintf(msgString, "%sVertical Triangle Strip: Blue", msgString);

			// Blue Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.10f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.10f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.42f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.26f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.74f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.58f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.90f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.90f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			break;
		case (9):	
			sprintf(msgString, "%sVertical Triangle Strip: White", msgString);

			// White Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.10f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.10f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.42f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.26f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.74f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.58f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.90f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.90f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			break;
		case (10):	
			sprintf(msgString, "%sVertical Triangle Strip: Multicolor", msgString);

			// Multicolor Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.10f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),  0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.10f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,128,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.42f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(128,128,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.26f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(128,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.74f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,128,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.58f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,128,128,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.90f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,128,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.90f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),  0, 0.0f,0.0f);
			break;
		//
		// Bent Triangle Strip, D3DPT_TRIANGLESTRIP
		//
		case (11):	
			sprintf(msgString, "%sBent Triangle Strip: Red", msgString);

			// Red Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.8f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.6f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.8f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.4f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.4f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.4f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.4f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.5f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.8f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			break;
		case (12):	
			sprintf(msgString, "%sBent Triangle Strip: Green", msgString);

			// Green Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.8f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.6f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.8f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.4f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.4f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.4f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.4f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.5f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.8f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			break;
		case (13):	
			sprintf(msgString, "%sBent Triangle Strip: Blue", msgString);

			// Blue Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.8f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.6f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.8f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.4f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.4f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.4f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.4f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.5f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.8f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			break;
		case (14):	
			sprintf(msgString, "%sBent Triangle Strip: White", msgString);

			// White Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.8f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.6f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.8f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.4f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.4f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.4f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.4f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.5f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.8f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			break;
		case (15):	
			sprintf(msgString, "%sBent Triangle Strip: Multicolor", msgString);

			// Multicolor Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.8f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(128,0,0,255),  0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.2f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,0,0,255),  0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.6f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,128,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(128,128,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.8f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.4f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(128,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.4f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.4f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,0,255),  0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,255,128,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,128,128,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.4f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,128,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,255,255),  0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.5f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.8f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,128,255),  0, 0.0f,0.0f);
			break;
		//
		// Two crossed strips, D3DPT_TRIANGLESTRIP
		//
		case (16):	
			sprintf(msgString, "%sCrossed Triangle Strips: Red & Green", msgString);

			// Red Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.1000f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.1f * dwHeight)), 0.9000f), (1.0f / 0.9000f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.1000f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.7857f), (1.0f / 0.7857f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.3667f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.6714f), (1.0f / 0.6714f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.3667f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5571f), (1.0f / 0.5571f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.6333f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.4429f), (1.0f / 0.4429f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.6333f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.3286f), (1.0f / 0.3286f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.9000f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.2143f), (1.0f / 0.2143f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.9000f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.9f * dwHeight)), 0.1000f), (1.0f / 0.1000f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			// Green Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.9f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.1000f * dwHeight)), 0.1000f), (1.0f / 0.1000f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.1000f * dwHeight)), 0.2143f), (1.0f / 0.2143f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3667f * dwHeight)), 0.3286f), (1.0f / 0.3286f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.5f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3667f * dwHeight)), 0.4429f), (1.0f / 0.4429f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.5f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6333f * dwHeight)), 0.5571f), (1.0f / 0.5571f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.3f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6333f * dwHeight)), 0.6714f), (1.0f / 0.6714f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.3f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.9000f * dwHeight)), 0.7857f), (1.0f / 0.7857f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.1f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.9000f * dwHeight)), 0.8000f), (1.0f / 0.8000f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			break;
		case (17):	
			sprintf(msgString, "%sCrossed Triangle Strips: Red & Black", msgString);

			// Red Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.1000f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.1f * dwHeight)), 0.9000f), (1.0f / 0.9000f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.1000f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.7857f), (1.0f / 0.7857f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.3667f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3f * dwHeight)), 0.6714f), (1.0f / 0.6714f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.3667f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.5571f), (1.0f / 0.5571f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.6333f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.5f * dwHeight)), 0.4429f), (1.0f / 0.4429f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.6333f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.3286f), (1.0f / 0.3286f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.9000f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.7f * dwHeight)), 0.2143f), (1.0f / 0.2143f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.9000f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.9f * dwHeight)), 0.1000f), (1.0f / 0.1000f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			// Black Mesh
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.9f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.1000f * dwHeight)), 0.1000f), (1.0f / 0.1000f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.1000f * dwHeight)), 0.2143f), (1.0f / 0.2143f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.7f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3667f * dwHeight)), 0.3286f), (1.0f / 0.3286f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.5f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.3667f * dwHeight)), 0.4429f), (1.0f / 0.4429f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.5f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6333f * dwHeight)), 0.5571f), (1.0f / 0.5571f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.3f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.6333f * dwHeight)), 0.6714f), (1.0f / 0.6714f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.3f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.9000f * dwHeight)), 0.7857f), (1.0f / 0.7857f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.1f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.9000f * dwHeight)), 0.8000f), (1.0f / 0.8000f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			break;
		//
		// Triangle fan Square, D3DPT_TRIANGLEFAN
		//
		case (18):	
			sprintf(msgString, "%sSquare Triangle Fan: Red & Blue", msgString);

			{
				int i, red, green, blue;

				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.8f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);

				for (i = 1; i < 10; i++)
				{
					VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(((float)i / 10.0f) * dwWidth),(D3DVALUE)(int)(dwHeight - (0.9f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = 8; i > 0; i--)
				{
					VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.9f * dwWidth),(D3DVALUE)(int)(dwHeight - (((float)i / 10.0f) * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = 8; i > 0; i--)
				{
					VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(((float)i / 10.0f) * dwWidth),(D3DVALUE)(int)(dwHeight - (0.1f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = 2; i < 10; i++)
				{
					VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.1f * dwWidth),(D3DVALUE)(int)(dwHeight - (((float)i / 10.0f) * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}

				for (i = 1; i < 34; i++)
				{
					red = (255 - (255 * i / 33));
					green = 0;
					blue = (255 * i / 33);
					VertexList[i].color = RGBA_MAKE(red, green, blue, 255);
				}
			}
			break;
		//
		// Jagged Triangle fan Square, D3DPT_TRIANGLEFAN
		//
		case (19):	
			sprintf(msgString, "%sJagged Triangle Fan: Red & Blue", msgString);

			{
				int i, red, green, blue;
				bool bEven;

				VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(0.2f * dwWidth),(D3DVALUE)(int)(dwHeight - (0.8f * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);

				bEven = true;
				for (i = 1; i < 10; i++)
				{
					bEven = (bEven ? false : true);
					VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(((float)i / 10.0f) * dwWidth),(D3DVALUE)(int)(dwHeight - ((bEven ? 0.85f : 0.9f) * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = 8; i > 0; i--)
				{
					bEven = (bEven ? false : true);
					VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)((bEven ? 0.7f : 0.9f) * dwWidth),(D3DVALUE)(int)(dwHeight - (((float)i / 10.0f) * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = 8; i > 0; i--)
				{
					bEven = (bEven ? false : true);
					VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)(((float)i / 10.0f) * dwWidth),(D3DVALUE)(int)(dwHeight - ((bEven ? 0.3f : 0.1f) * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = 2; i < 10; i++)
				{
					bEven = (bEven ? false : true);
					VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((D3DVALUE)(int)((bEven ? 0.15f : 0.1f) * dwWidth),(D3DVALUE)(int)(dwHeight - (((float)i / 10.0f) * dwHeight)), 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}

				for (i = 1; i < 34; i++)
				{
					red = (255 - (255 * i / 33));
					green = 0;
					blue = (255 * i / 33);
					VertexList[i].color = RGBA_MAKE(red, green, blue, 255);
				}
			}
			break;
	}
}

