#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "Volume.h"

// Global Test and App instance
CD3DWindowFramework	        App;
CVolUpdateTexTest           VolUpdateTex;   // UpdateTex volume texture test cases
CVolLockCopyTest            VolLockCopy;    // LockCopy volume texture test cases
CMipVolUTTest               MipVolUT;       // UpdateTex volume mipmap texture test cases
CMipVolLCTest               MipVolLC;       // LockCopy volume mipmap texture test cases
CVolTexGenTest              VolTexGen;      // Volume texture generation test cases
CVolFilterTest              VolFilter;      // Volume texture filtering test cases
CVolBlendTest               VolBlend;       // Volume texture blending test cases
CVolAddressWrapTest         VolAddressWrap;         // Volume texture wrap addressing test cases
CVolAddressMirrorTest       VolAddressMirror;       // Volume texture mirror addressing test cases
CVolAddressClampTest        VolAddressClamp;        // Volume texture clamp addressing test cases
CVolAddressBorderTest       VolAddressBorder;       // Volume texture border addressing test cases
CVolAddressMirrorOnceTest   VolAddressMirrorOnce;   // Volume texture mirror once addressing test cases
        
CVolumeTest::CVolumeTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
	m_Options.D3DOptions.fMinDXVersion = 8.0f;
	m_Options.ModeOptions.VolumeFmt = FMT_ALL;
    m_Options.ModeOptions.dwVolInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE;
	m_Options.ModeOptions.dwVolExclude = PF_METN;
#if defined(_ARM_) || defined(_ARM64_) // temp. check for arm64
	// On ARM architectures exclude DXT formats due to a D3DX9 bug that ignores pitch on DXT resources affecting some ARM partners
	// See Windows Blue Bugs 24275
	m_Options.ModeOptions.dwVolExclude = m_Options.ModeOptions.dwVolExclude|PF_DXTN;
#endif // _ARM_

    // Initialize some variables
	pMaterial = NULL;
	pTVertex = NULL;
	pTGVertex = NULL;
	pVolTexture = NULL;
	nVertexMax = 0;
	bMipMap = false;
    bTexGen = false;
}

CVolumeTest::~CVolumeTest()
{
}

bool CVolumeTest::SetDefaultMaterials(void)
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

bool CVolumeTest::SetDefaultMatrices(void)
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

bool CVolumeTest::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRS_AMBIENT,(DWORD)0xFFFFFFFF);

    if (bTexGen)
        SetRenderState(D3DRS_NORMALIZENORMALS,(DWORD)TRUE);

	return true;
}

bool CVolumeTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

void CVolumeTest::SceneRefresh(void)
{
	if (BeginScene())
	{
        if (bTexGen)
            RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_XYZ|D3DFVF_NORMAL,pTGVertex,nVertexMax,NULL,0,NULL);
        else if (bMipMap)
            RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0),pTVertex,nVertexMax,NULL,0,NULL);
        else            
            RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0),VertexList,6,NULL,0,NULL);

        EndScene();
	}	
}

void CVolumeTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

bool CVolumeTest::TestTerminate(void)
{
    // Cleanup vertex data
    SAFEDELETEA(pTVertex);
    SAFEDELETEA(pTGVertex);

	// Cleanup material
    SAFEDELETE(pMaterial);

    // Cleanup texture
	SetTexture(0, NULL);
    RELEASE(pVolTexture);

	return true;
}
