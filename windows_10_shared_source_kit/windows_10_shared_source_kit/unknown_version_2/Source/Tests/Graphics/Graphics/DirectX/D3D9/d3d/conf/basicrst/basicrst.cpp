#define D3D_OVERLOADS
#include <d3d.h>
#include "BasicRst.h"

// Global Test and App instance
CD3DWindowFramework	App;
CCullingTest	Culling;
CDitherTest		Dither;
CFillTest		Fill;
CShadingTest	Shading;
CZWriteTest		ZWrite;
CMeshTLTest		MeshTL;
CMeshLTest		MeshL;
CMeshD3DTest	MeshD3D;
CColorWriteTest ColorWrite;
CNoDiffuseTest  NoDiffuse;

CBasicRstTest::CBasicRstTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
    m_pFramework->GetImageCmp()->SetMinPixels(0);

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;

	// Force test to use relevant DirectX versions
	m_Options.D3DOptions.dwIncVersions = DXVERSION_91 | DXVERSION_9 | DXVERSION_8_OR_81 | DXVERSION_7 | DXVERSION_6;
}

CBasicRstTest::~CBasicRstTest()
{
}

bool CBasicRstTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,0,255), 1.0f, 0);
}

void CBasicRstTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,NULL);

		EndScene();
	}	
}
