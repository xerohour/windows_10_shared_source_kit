////////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Microsoft Corporation, 1999.
//
// CCubemap.cpp
//
// CCubemap class - Conformance test for cubemap textures
//
// History: 12/20/98   Jeff Vezina   - Created
//
////////////////////////////////////////////////////////////////////////////////////////////
//
// Test Description:
//    Test consists of 3 parts.  First part rotates yaw by 10 degrees increments (default)
//    for each test.  Next part rotates pitch by 10 degrees increments (default) for each
//    test.  Final part creates a mipmapped cubemap and rotates pitch by 45 degrees
//    increments for 3 distances within each of the 3 mipmap levels.
//
////////////////////////////////////////////////////////////////////////////////////////////

#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "Cubemap.h"

CD3DWindowFramework TheApp;
CCubeUpdateTexTest          CubeUpdateTex;  // UpdateTex cube texture test cases
CCubeCopyRectsTest          CubeCopyRects;  // CopyRects cube texture test cases
CCubeLockCopyTest           CubeLockCopy;   // LockCopy cube texture test cases
CCubeUpdateSurfTest         CubeUpdateSurf; // UpdateSurf cube texture test cases
CMipCubeUTTest              MipCubeUT;      // UpdateTex cube mipmap texture test cases
CMipCubeCRTest              MipCubeCR;      // CopyRects cube mipmap texture test cases
CMipCubeLCTest              MipCubeLC;      // LockCopy cube mipmap texture test cases
CMipCubeUSTest              MipCubeUS;      // UpdateSurf cube mipmap texture test cases
CCubeTexGenTest             CubeTexGen;		// Cube texture generation test cases
CCubeFilterTest				CubeFilter;		// Cube texture filtering test cases
CCubeBlendTest				CubeBlend;		// Cube texture blending test cases

CCubeTest::CCubeTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
	m_Options.D3DOptions.fMinDXVersion = 7.0f;
#ifdef FAST_TEST
	m_Options.ModeOptions.CubeFmt = FMT_A8B8G8R8;
#else
	m_Options.ModeOptions.CubeFmt = FMT_ALL;
#endif
    m_Options.ModeOptions.dwCubeInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE;
	m_Options.ModeOptions.dwCubeExclude = PF_METN;

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_7;

    // Initialize some variables
	pMaterial = NULL;
	pTVertex = NULL;
	pTGVertex = NULL;
	pCubeTexture = NULL;
	nVertexMax = 0;
    bTexGen = false;

	fDegInc = 0.0f;
	fZDepthMin = 3.0f;		// Min distance for yaw/pitch rotation and first mipmap level
	fZDepthMax = 15.0f;		// Max distance for last mipmap level
}

CCubeTest::~CCubeTest()
{
}

bool CCubeTest::SetDefaultMaterials(void)
{
	// Init the material
    pMaterial = new MATERIAL;
	memset(pMaterial, NULL, sizeof(MATERIAL));

    // No alpha
	pMaterial->Diffuse.a = 1.0f;

    // White 
    pMaterial->Ambient.r = 1.0f;
	pMaterial->Ambient.g = 1.0f;
	pMaterial->Ambient.b = 1.0f;

	// Set the appropriate material
	SetMaterial(pMaterial);

    return true;
}

bool CCubeTest::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRS_AMBIENT,(DWORD)0xFFFFFFFF);

    if (bTexGen)
        SetRenderState(D3DRS_NORMALIZENORMALS,(DWORD)TRUE);

	return true;
}

bool CCubeTest::SetDefaultMatrices(void)
{
    D3DXMATRIX Matrix;
    D3DVALUE dvAspect = ((float) m_pCurrentMode->nBackBufferWidth) /
						((float) m_pCurrentMode->nBackBufferHeight);

    D3DXMatrixPerspectiveFovLH(&Matrix, pi/2.0f, dvAspect, 1.0f, 50.0f);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_VIEW, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ViewMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    if (bTexGen)
    {
	    D3DXMatrixIdentity(&Matrix);
	    if (!SetTransform(D3DTS_TEXTURE0, (D3DMATRIX *) &Matrix))
	    {
    		WriteToLog("SetTransform(Texture0) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
            return false;
	    }
    }

	return true;
}

bool CCubeTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

void CCubeTest::SceneRefresh()
{
	if (BeginScene()) 
    {
        if (bTexGen)
            RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_XYZ|D3DFVF_NORMAL,pTGVertex,nVertexMax,NULL,0,NULL);
        else     
            RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0),pTVertex,nVertexMax,NULL,0,NULL);

		EndScene();
	}
}

void CCubeTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

bool CCubeTest::TestTerminate()
{
    // Cleanup vertex data
    SAFEDELETEA(pTVertex);
    SAFEDELETEA(pTGVertex);

	// Cleanup material
    SAFEDELETE(pMaterial);

    // Cleanup texture
	SetTexture(0, NULL);
    RELEASE(pCubeTexture);

    return true;
}
