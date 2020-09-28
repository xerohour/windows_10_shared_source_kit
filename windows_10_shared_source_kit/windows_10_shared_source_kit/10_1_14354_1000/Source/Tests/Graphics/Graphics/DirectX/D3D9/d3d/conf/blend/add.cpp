#define D3D_OVERLOADS
#include <d3d.h>
#include "Blend.h"

//************************************************************************
// Add Test functions

CAddTest::CAddTest()
{
	m_szTestName = "Texture Blend Add";
	m_szCommandKey = "Add";

	// Tell parent class what cap to look for
	dwBlendCap = D3DPTBLENDCAPS_ADD;
    dwTextureOpCap = D3DTEXOPCAPS_SELECTARG2;
    dwTextureOpCap2 = D3DTEXOPCAPS_ADD;

  	// Give parent a printable name for the caps
    szTextureOp = "SelectArg2";
    szTextureOp2 = "Add";

	// Tell parent what state we are
	dwBlendState = D3DTBLEND_ADD;
}
 
CAddTest::~CAddTest()
{
}

bool CAddTest::SetDefaultRenderStates(void)
{
	// Turn on Add blending
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);

	if (m_dwVersion <= 0x0600)
		SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_ADD);

	return true;
}

HRESULT CAddTest::DrawBasicGrid(int nTest, int nTexFormats)
{
	DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
	DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
	int	  nScale, nMapping;
	float fScale;

	// Initialize the interesting variables
	nMapping = (nTest-1) / (4*nTexFormats);

	nScale = ((nTest-1) / nTexFormats) + 1;
	nScale = nScale % 4;

	if (nScale == 0)
		nScale = 4;

	fScale = 1.0f * nScale;

	switch (nMapping)
	{
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, Black Mesh
			sprintf(msgString, "%sBlack, Map:(0,0)(%d,0)(0,%d)(%d,%d), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255),0, fScale,fScale);
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping, Red Mesh
			sprintf(msgString, "%sRed, Map:(0,%d)(0,0)(%d,%d)(%d,0), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, fScale,  0.0f);
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping, Green Mesh
			sprintf(msgString, "%sGreen, Map:(%d,%d)(0,%d)(%d,0)(0,0), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, 0.0f,    0.0f);
			break;
		case (4): // Use ((1,0)(1,1)(0,0)(0,1)) mapping, Blue Mesh
			sprintf(msgString, "%sBlue, Map:(%d,0)(%d,%d)(0,0)(0,%d), ",msgString,nScale,nScale,nScale,nScale);

			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 0.0f,  fScale);
			break;
	}

	// On Dx9c, need to check for UsageQueryWrapAndMip for Scale > 1
	if ((m_pD3D->GetRuntimeVersion() >= 9.029f) && (nScale > 1))
	{
		return m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
										 USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat]);
	}

	return S_OK;
}

void CAddTest::DrawBlendingGrid(int nAlpha)
{
	DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
	DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;

	// Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, Black Mesh
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,nAlpha),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255)   ,0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,nAlpha),0, 0.0f,1.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.03f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,nAlpha),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.97f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255)   ,0, 1.0f,0.0f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DVECTOR((0.83f * dwWidth), (dwHeight - (0.05f * dwHeight)), 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255)   ,0, 1.0f,1.0f);
}
 