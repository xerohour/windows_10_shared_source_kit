/*
Implmentation of a test which creates and uses DXT textures of strange sizes
*/

#include "DXTSize.h"

#include "CTextures.h"
#include "CBuffers.h"

#include "D3D9.h"//for D3DFVF_XXX


CDXTSizeTest::CDXTSizeTest()
{
	SetTestType(TESTTYPE_CONF);
	m_Options.D3DOptions.fMinDXVersion = 8.0f;
	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_DXTN;

	m_szTestName = _T("DXTSize");
	m_szCommandKey = _T("DXTSize");
}

CDXTSizeTest::~CDXTSizeTest()
{
}


bool CDXTSizeTest::CapsCheck()
{
	//we would like to make 8x256 textures and 256x8 textures
	//but the caps might not allow that
	const UINT DESIRED_MAX_SIZE = 256;
	const UINT DESIRED_MIN_SIZE = 8;

	//make sure the device supports non-square textures
	const DEVICEDESC* pCaps = m_pSrcDevice->GetCaps();

	if(D3DPTEXTURECAPS_SQUAREONLY == (pCaps->dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY))
	{
		return false;//the device only supports square textures
	}

	m_nMaxTextureWidth = MIN(DESIRED_MAX_SIZE, pCaps->dwMaxTextureWidth);
	m_nMaxTextureHeight = MIN(DESIRED_MAX_SIZE, pCaps->dwMaxTextureHeight);

	//check to make sure they atleast support 8x8 textures
	if(pCaps->dwMaxTextureWidth < DESIRED_MIN_SIZE)
	{
		return false;
	}

	if(pCaps->dwMaxTextureHeight < DESIRED_MIN_SIZE)
	{
		return false;
	}

	m_nMinTextureWidth = DESIRED_MIN_SIZE;
	m_nMinTextureHeight = DESIRED_MIN_SIZE;

	return true;
}

UINT CDXTSizeTest::TestInitialize()
{
	SetTestRange(1, 2);
	return D3DTESTINIT_RUN;
}

bool CDXTSizeTest::ExecuteTest(UINT nTest)
{
	UINT nWidth;
	UINT nHeight;

	if(1 == nTest)
	{
		nWidth = m_nMaxTextureWidth;
		nHeight = m_nMinTextureHeight;
	}
	else
	{
		nWidth = m_nMinTextureHeight;
		nHeight = m_nMaxTextureWidth;
	}


	//Create the texture
	CnTexture* pTexture;
	
	if(!CreateTexture(nWidth,nHeight,0,0,m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],POOL_DEFAULT,&pTexture))
	{
		WriteToLog(_T("CreateTexture Failed\n"));
		Fail();
		return false;
	}

	bool bRet = true;

	//put some data into the texture
	DWORD dwStripes[] = 
	{
		0x00FF00FF,
		0xFF123456,
		0xDEADBEAF,
		0xFFFFFFFF,
	};

	const DWORD nStripes = sizeof(dwStripes) / sizeof(dwStripes[0]);


	bRet = (bRet && pTexture->LoadStripes(0, nWidth/4, nHeight/4, nStripes, dwStripes, false, false));


	//use it
	if(!SetTexture(0, pTexture))
	{
		WriteToLog(_T("SetTexture Failed\n"));
		bRet = false;
	}

	if(!DrawTriangle())
	{
		bRet = false;
	}

	if(pTexture)
	{
		pTexture->Release();
		pTexture = NULL;
	}

	if(!bRet)
	{
		Fail();
	}


	m_nLastTextureWidth = nWidth;
	m_nLastTextureHeight = nHeight;
	return bRet;
}


void CDXTSizeTest::UpdateStatus()
{
	CD3DTest::UpdateStatus();

	WriteStatus(_T("$gTexture Size"), _T("%ux%u"), m_nLastTextureWidth, m_nLastTextureHeight);
}


struct SVertex
{
	float fPos[4];
	DWORD dwDiffuse;
	float fTex[2];
};
#define DXTSIZE_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

const SVertex g_rgVertices[] = 
{
	{ {0.0f, 0.0f, 0.5f, 1.0f}, {0xFFFFFFFF}, {0.0f, 0.0f} },
	{ {300.0f, 0.0f, 0.5f, 1.0f}, {0xFFFFFF00}, {0.0f, 1.0f} },
	{ {150.0f, 300.0f, 0.5f, 1.0f}, {0xFFFF00FF}, {1.0f, 1.0f} },
};

const unsigned short g_rgIndices[] = {0, 1, 2};

bool CDXTSizeTest::DrawTriangle()
{
	CnVertexBuffer* pVB;
	CnIndexBuffer* pIB;


	if(!CreateVertexBuffer(sizeof(g_rgVertices), 0, DXTSIZE_FVF, POOL_MANAGED, &pVB))
	{
		return false;
	}

	if(!CreateIndexBuffer(sizeof(g_rgIndices), 0, FMT_INDEX16, POOL_MANAGED, &pIB))
	{
		return false;
	}

	bool bRet = true;

	void* pVertices;
	if(!pVB->Lock(0, 0, (BYTE**)&pVertices, 0))
	{
		bRet = false;
	}
	else
	{
		memcpy(pVertices, g_rgVertices, sizeof(g_rgVertices));
		bRet = (bRet && pVB->Unlock());
	}



	
	
	BYTE* pIndices;
	if(!pIB->Lock(0, 0, &(pIndices), 0))
	{
		bRet = false;
	}
	else
	{
		memcpy(pIndices, g_rgIndices, sizeof(g_rgIndices));
		bRet = (bRet && pIB->Unlock());
	}

	if(!BeginScene())
	{
		bRet = false;
	}
	else
	{
		bRet = (bRet && SetStreamSource(0, pVB, sizeof(g_rgVertices[0])));
		bRet = (bRet && SetIndices(pIB, 0));
		bRet = (bRet && SetVertexShader(DXTSIZE_FVF));

		bRet = (bRet && DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 3, 0, 1, 0));

		bRet = (bRet && EndScene());
	}



	pVB->Release();
	pIB->Release();	

	return bRet;

}
