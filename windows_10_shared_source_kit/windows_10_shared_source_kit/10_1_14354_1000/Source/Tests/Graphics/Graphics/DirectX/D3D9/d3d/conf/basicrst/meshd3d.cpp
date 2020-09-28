#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "BasicRst.h"

//************************************************************************
// MeshD3D Test functions

CMeshD3DTest::CMeshD3DTest()
{
	m_szTestName = "BasicRst Mesh D3D Vertex";
	m_szCommandKey = "MeshD3D";
}

CMeshD3DTest::~CMeshD3DTest()
{
}

bool CMeshD3DTest::SetDefaultMaterials(void)
{
	// Init the material
    pMaterial = new MATERIAL;
	memset(pMaterial, NULL, sizeof(MATERIAL));

    return true;
}

bool CMeshD3DTest::SetDefaultMatrices(void)
{
	// Set the projection matrix    
	D3DXMATRIX Matrix( D3DVAL(pi/1.5f), D3DVAL(0.0),     D3DVAL(0.0),  D3DVAL(0.0),
					   D3DVAL(0.0),     D3DVAL(pi/1.5f), D3DVAL(0.0),  D3DVAL(0.0),
					   D3DVAL(0.0),     D3DVAL(0.0),     D3DVAL(1.0),  D3DVAL(1.0),
					   D3DVAL(0.0),     D3DVAL(0.0),     D3DVAL(-1.0), D3DVAL(0.0)
				     );

    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

	// Set the view matrix    
    D3DXMatrixIdentity(&Matrix);
	Matrix.m[3][2] = 2.0f;
    if (!SetTransform(D3DTS_VIEW, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ViewMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

	// Set the world matrix    
    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

UINT CMeshD3DTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1,MESH_D3D_TESTS);

    // Set compare function to require 85%
	m_pFramework->GetImageCmp()->SetPassRequired(0.85f);

	return D3DTESTINIT_RUN;
}

bool CMeshD3DTest::ExecuteTest(UINT uTestNum)
{
	D3DCOLOR	Color;

	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build some triangles.
	Color = DrawMeshTests(uTestNum);

	// Set up the material
	pMaterial->emissive.r = (float)RGBA_GETRED(Color);
	pMaterial->emissive.g = (float)RGBA_GETGREEN(Color);
	pMaterial->emissive.b = (float)RGBA_GETBLUE(Color);
	pMaterial->emissive.a = (float)RGBA_GETALPHA(Color);

	// Set the appropriate material
	SetMaterial(pMaterial);

	// Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

	if (BeginScene())
	{
		if (uTestNum < 13)
		{
			// Use Triangle Strips
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,D3DVertexList,dwVertexCount,NULL,0,0);
		}
		else if (uTestNum < 15)
		{
			// Use Triangle Strips
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,D3DVertexList+8,dwVertexCount/2,NULL,0,0);

			// Setup a red material
	        pMaterial->emissive.r = 255.0f;
	        pMaterial->emissive.g =   0.0f;
	        pMaterial->emissive.b =   0.0f;
	        pMaterial->emissive.a = 255.0f;

			// Set the appropriate material
			SetMaterial(pMaterial);

			// Use Triangle Strips
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,D3DVertexList,dwVertexCount/2,NULL,0,0);
		}
		else
		{
			// Use Triangle Fans
			RenderPrimitive(D3DPT_TRIANGLEFAN,D3DFVF_VERTEX,D3DVertexList,dwVertexCount,NULL,0,0);
		}

		EndScene();
	}	

    return true;
}

void CMeshD3DTest::SceneRefresh(void)
{
}

bool CMeshD3DTest::TestTerminate()
{
	// Cleanup material
    SAFEDELETE(pMaterial);

	return true;
}

//************************************************************************
// Internal API:    DrawMeshTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

D3DCOLOR CMeshD3DTest::DrawMeshTests(int nTest)
{
	D3DCOLOR	MaterialColor = RGBA_MAKE(0,0,0,255);

	switch (nTest)
	{
		// Straight Triangle Strip (Horizontal), D3DPT_TRIANGLESTRIP
		//
		case (1):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Red", msgString);

			// Red Mesh
			MaterialColor = RGBA_MAKE(255,0,0,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.80f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.80f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.48f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.16f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.16f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.48f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.80f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.80f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (2):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Green", msgString);

			// Green Mesh
			MaterialColor = RGBA_MAKE(0,255,0,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.80f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.80f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.48f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.16f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.16f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.48f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.80f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.80f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (3):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Blue", msgString);

			// Blue Mesh
			MaterialColor = RGBA_MAKE(0,0,255,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.80f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.80f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.48f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.16f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.16f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.48f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.80f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.80f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (4):	
			sprintf(msgString, "%sHorizontal Triangle Strip: White", msgString);

			// White Mesh
			MaterialColor = RGBA_MAKE(255,255,255,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.80f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.80f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.48f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.16f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.16f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.48f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.80f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.80f, 0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		//
		// Straight Triangle Strip (Vertical), D3DPT_TRIANGLESTRIP
		//
		case (5):	
			sprintf(msgString, "%sVertical Triangle Strip: Red", msgString);

			// Red Mesh
			MaterialColor = RGBA_MAKE(255,0,0,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.16f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.48f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.48f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.16f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (6):	
			sprintf(msgString, "%sVertical Triangle Strip: Green", msgString);

			// Green Mesh
			MaterialColor = RGBA_MAKE(0,255,0,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.16f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.48f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.48f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.16f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (7):	
			sprintf(msgString, "%sVertical Triangle Strip: Blue", msgString);

			// Blue Mesh
			MaterialColor = RGBA_MAKE(0,0,255,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.16f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.48f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.48f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.16f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (8):	
			sprintf(msgString, "%sVertical Triangle Strip: White", msgString);

			// White Mesh
			MaterialColor = RGBA_MAKE(255,255,255,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.16f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.48f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.48f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.16f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.80f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		//
		// Bent Triangle Strip, D3DPT_TRIANGLESTRIP
		//
		case (9):	
			sprintf(msgString, "%sBent Triangle Strip: Red", msgString);

			// Red Mesh
			MaterialColor = RGBA_MAKE(255,0,0,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.6f,-0.6f, 0.5f), D3DXVECTOR3(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.2f,-0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.6f,-0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2f,-0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.0f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.0f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2f, 0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.0f, 0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (10):	
			sprintf(msgString, "%sBent Triangle Strip: Green", msgString);

			// Green Mesh
			MaterialColor = RGBA_MAKE(0,255,0,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.6f,-0.6f, 0.5f), D3DXVECTOR3(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.2f,-0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.6f,-0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2f,-0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.0f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.0f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2f, 0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.0f, 0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (11):	
			sprintf(msgString, "%sBent Triangle Strip: Blue", msgString);

			// Blue Mesh
			MaterialColor = RGBA_MAKE(0,0,255,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.6f,-0.6f, 0.5f), D3DXVECTOR3(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.2f,-0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.6f,-0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2f,-0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.0f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.0f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2f, 0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.0f, 0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (12):	
			sprintf(msgString, "%sBent Triangle Strip: White", msgString);

			// White Mesh
			MaterialColor = RGBA_MAKE(255,255,255,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.6f,-0.6f, 0.5f), D3DXVECTOR3(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.2f,-0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f,-0.4f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.6f,-0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2f,-0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f, 0.0f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.0f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2f, 0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.2f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.0f, 0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		//
		// Two crossed strips, D3DPT_TRIANGLESTRIP
		//
		case (13):	
			sprintf(msgString, "%sCrossed Triangle Strips: Red & Green", msgString);

			// Red strip
			MaterialColor = RGBA_MAKE(255,0,0,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.8000f,-0.8f, 0.9000f), D3DXVECTOR3(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.8000f,-0.4f, 0.7857f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2666f,-0.4f, 0.6714f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2666f, 0.0f, 0.5571f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.2666f, 0.0f, 0.4429f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.2666f, 0.4f, 0.3286f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.8000f, 0.4f, 0.2143f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.8000f, 0.8f, 0.1000f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			// Green strip
			MaterialColor = RGBA_MAKE(0,255,0,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.8f,-0.8000f, 0.1000f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.8000f, 0.2143f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.2666f, 0.3286f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.0f,-0.2666f, 0.4429f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.0f, 0.2666f, 0.5571f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.4f, 0.2666f, 0.6714f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.4f, 0.8000f, 0.7857f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.8f, 0.8000f, 0.8000f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			break;
		case (14):	
			sprintf(msgString, "%sCrossed Triangle Strips: Red & Black", msgString);

			// Red strip
			MaterialColor = RGBA_MAKE(255,0,0,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.8000f,-0.8f, 0.9000f), D3DXVECTOR3(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.8000f,-0.4f, 0.7857f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2666f,-0.4f, 0.6714f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.2666f, 0.0f, 0.5571f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.2666f, 0.0f, 0.4429f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.2666f, 0.4f, 0.3286f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.8000f, 0.4f, 0.2143f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.8000f, 0.8f, 0.1000f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.0f,0.0f);
			// Black strip
			MaterialColor = RGBA_MAKE(0,0,0,255);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.8f,-0.8000f, 0.1000f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.8000f, 0.2143f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.4f,-0.2666f, 0.3286f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.0f,-0.2666f, 0.4429f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3( 0.0f, 0.2666f, 0.5571f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.4f, 0.2666f, 0.6714f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.4f, 0.8000f, 0.7857f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.8f, 0.8000f, 0.8000f), D3DXVECTOR3(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			break;
		//
		// Triangle fan Square, D3DPT_TRIANGLEFAN
		//
		case (15):	
		case (16):	
		case (17):	
		case (18):	
			{
				int i;

				D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,-1.0f), 0.0f,0.0f);

				for (i = -8; i < 9; i++)
				{
					D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3((float)i / 10.0f, 0.8f, 0.5f), D3DXVECTOR3(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(0.8f, (float)i / 10.0f, 0.5f), D3DXVECTOR3(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3((float)i / 10.0f,-0.8f, 0.5f), D3DXVECTOR3(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = -8; i < 9; i++)
				{
					D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.8f, (float)i / 10.0f, 0.5f), D3DXVECTOR3(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}

				switch (nTest)
				{
					case (15):	// Red fan
						sprintf(msgString, "%sSquare Triangle Fan: Red", msgString);
						MaterialColor = RGBA_MAKE(255,0,0,255);
						break;
					case (16):	// Green fan
						sprintf(msgString, "%sSquare Triangle Fan: Green", msgString);
						MaterialColor = RGBA_MAKE(0,255,0,255);
						break;
					case (17):	// Blue fan
						sprintf(msgString, "%sSquare Triangle Fan: Blue", msgString);
						MaterialColor = RGBA_MAKE(0,0,255,255);
						break;
					case (18):	// White fan
						sprintf(msgString, "%sSquare Triangle Fan: White", msgString);
						MaterialColor = RGBA_MAKE(255,255,255,255);
						break;
				}
			}
			break;
		//
		// Jagged Triangle fan Square, D3DPT_TRIANGLEFAN
		//
		case (19):	
		case (20):	
		case (21):	
		case (22):	
			{
				int i;
				bool bEven;

				D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3(-0.6f, 0.6f, 0.5f), D3DXVECTOR3(0.0f,0.0f,-1.0f), 0.0f,0.0f);

				bEven = true;
				for (i = -8; i < 9; i++)
				{
					bEven = (bEven ? false : true);
					D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3((float)i / 10.0f, (bEven ? 0.7f : 0.8f), (bEven ? 0.6f : 0.4f)), D3DXVECTOR3(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					bEven = (bEven ? false : true);
					D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3((bEven ? 0.3f : 0.8f), (float)i / 10.0f, (bEven ? 0.6f : 0.4f)), D3DXVECTOR3(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					bEven = (bEven ? false : true);
					D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3((float)i / 10.0f, (bEven ? -0.3f : -0.8f), (bEven ? 0.6f : 0.4f)), D3DXVECTOR3(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = -8; i < 9; i++)
				{
					bEven = (bEven ? false : true);
					D3DVertexList[dwVertexCount++] = D3DVERTEX(D3DXVECTOR3((bEven ? -0.7f : -0.8f), (float)i / 10.0f, (bEven ? 0.6f : 0.4f)), D3DXVECTOR3(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}

				switch (nTest)
				{
					case (19):	// Red fan
						sprintf(msgString, "%sJagged Triangle Fan: Red", msgString);
						MaterialColor = RGBA_MAKE(255,0,0,255);
						break;
					case (20):	// Green fan
						sprintf(msgString, "%sJagged Triangle Fan: Green", msgString);
						MaterialColor = RGBA_MAKE(0,255,0,255);
						break;
					case (21):	// Blue fan
						sprintf(msgString, "%sJagged Triangle Fan: Blue", msgString);
						MaterialColor = RGBA_MAKE(0,0,255,255);
						break;
					case (22):	// White fan
						sprintf(msgString, "%sJagged Triangle Fan: White", msgString);
						MaterialColor = RGBA_MAKE(255,255,255,255);
						break;
				}
			}
			break;
	}

	return MaterialColor;
}

