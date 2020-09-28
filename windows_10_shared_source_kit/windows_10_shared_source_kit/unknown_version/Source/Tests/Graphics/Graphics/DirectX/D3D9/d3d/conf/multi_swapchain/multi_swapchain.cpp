//////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 2000.
//
// multi_swapchain.cpp
//
// CSwpChainTest class implementation - Renders different views to different swap chains.
//
// History: 12/18/2000 Bobby Paulose     - Created
//
//////////////////////////////////////////////////////////////////////

#include "multi_swapchain.h"


//////////////////////////////////////////////////////////////////////

// Global Test and App instance
CD3DWindowFramework	App;
CSwpChainTest ThisTest;

//////////////////////////////////////////////////////////////////////

///////////GLOBALS/////////////
LPTSTR  szTags[NUM_SWPCHN] = {"FRONT VIEW", "SIDE VIEW", "TOP VIEW"};
bool    fTag = true;
///////////////////////////////


CSwpChainTest::CSwpChainTest()
{

	SetTestType(TESTTYPE_CONF);

	m_Options.D3DOptions.nBackBufferWidth = IMGWIDTH;
	m_Options.D3DOptions.nBackBufferHeight = IMGHEIGHT;

	//m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
	//m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD;

	m_Options.D3DOptions.fMinDXVersion = 8.0;

	m_szTestName = "Multiple Swapchains";
	m_szCommandKey = "Range";

	m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.22f);
	m_pFramework->GetImageCmp()->SetMinPixels(0);

}

CSwpChainTest::~CSwpChainTest()
{
}


/*
void CSwpChainTest::DisplaySetup(void)
{
	g_pFramework->SetDisplaySize(IMGWIDTH * 4, IMGHEIGHT * 2);
	return;
}

*/


bool CSwpChainTest::Setup()
{
	int i;
	
	if( !CD3DTest::Setup() )
		return false;
	
	////////////create additional swap chains//////////

	for (i=0; i<NUM_SWPCHN; i++)
	{
		pAdditionalSrcSwpChn[i] = NULL;
		pAdditionalRefSwpChn[i] = NULL;
	}

    PRESENT_PARAMETERS   Param;
	
    // Fill presentation parameters
    Param.uWidth = IMGWIDTH;
    Param.uHeight = IMGHEIGHT;
	memcpy(&(Param.BackBufferFormat), &(m_pCurrentMode->pDisplay->Format), sizeof(FORMAT));
    Param.uBackBufferCount = 1;
    Param.MultiSample = (m_pCommonMultiSampleFormats) ? (MULTISAMPLE_TYPE)m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample] : (MULTISAMPLE_TYPE)0;
	Param.MultiSampleQuality = m_pCurrentMode->nMltsmpQuality;
	if (Param.MultiSample == MULTISAMPLE_NONE)
		Param.SwapEffect = D3DSWAPEFFECT_COPY;
	else
        Param.SwapEffect = D3DSWAPEFFECT_DISCARD;

	Param.hDeviceWindow = m_pFramework->m_pDisplayWindow->m_hWindow;
	Param.bWindowed = m_pCurrentMode->bWindowed ? TRUE : FALSE;
	if(m_Options.D3DOptions.bZBuffer)
	{
		if(m_uCommonZBufferFormats > 0)
		{
			Param.bAutoDepthStencil 	= true;
			memcpy(&(Param.AutoDepthStencilFormat), &(m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat]), sizeof(FORMAT));
		}
		else
		{
			Param.bAutoDepthStencil 	= false;
		}
	}
    Param.uFullscreenRefresh = m_pCurrentMode->bWindowed ? 0 : m_pCurrentMode->pDisplay->dwRefreshRate;
    Param.uFullscreenPresentInterval = 0;
    Param.dwFlags = 0;
	if (Param.MultiSample == MULTISAMPLE_NONE)
		Param.dwFlags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	else
		Param.dwFlags = 0;


	for (i=0; i<NUM_SWPCHN; i++)
	{
		if ( (D3D_OK != (m_pSrcDevice->CreateAdditionalSwapChain(&Param, &pAdditionalSrcSwpChn[i]))) ||
			 (D3D_OK != (m_pRefDevice->CreateAdditionalSwapChain(&Param, &pAdditionalRefSwpChn[i]))) )
		{
			for (int j=0; j<=i; j++)
			{
				RELEASE(pAdditionalSrcSwpChn[j]);
				RELEASE(pAdditionalRefSwpChn[j]);
			}
			return false;
		}
	}

	// Create textures	
	numTextures = 0;
	
	CreateTexture(128, 128, 1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], POOL_DEFAULT, &pTextures[numTextures]);
	pTextures[numTextures]->LoadTexture(0, "texture6.bmp");
	numTextures++;

	if (  (m_pSrcDevice->GetCaps()->wMaxTextureBlendStages > 1) &&
		  (m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures > 1)  )
	{
		CreateTexture(128, 128, 1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], POOL_DEFAULT, &pTextures[numTextures]);
		pTextures[numTextures]->LoadTexture(0, "texture6.bmp");
	}
	numTextures++;
	
	CreateTexture(128, 128, 1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], POOL_DEFAULT, &pTextures[numTextures]);
	pTextures[numTextures]->LoadTexture(0, "texture2.bmp");
	numTextures++;
	
	CreateTexture(128, 128, 1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], POOL_DEFAULT, &pTextures[numTextures]);
	pTextures[numTextures]->LoadTexture(0, "texture11.bmp");
	numTextures++;
	
	if (  (m_pSrcDevice->GetCaps()->wMaxTextureBlendStages > 1) &&
		  (m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures > 1)  )
	{
		CreateTexture(128, 128, 1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], POOL_DEFAULT, &pTextures[numTextures]);
		pTextures[numTextures]->LoadTexture(0, "texture14.bmp");
	}
	numTextures++;
	
	CreateTexture(64, 64, 1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], POOL_DEFAULT, &pTextures[numTextures]);
	pTextures[numTextures]->LoadTexture(0, "texture11_2.bmp");
	numTextures++;
	
	CreateTexture(64, 64, 1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], POOL_DEFAULT, &pTextures[numTextures]);
	pTextures[numTextures]->LoadTexture(0, "texture2_2.bmp");
	numTextures++;

	CreateTexture(16, 16, 1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], POOL_DEFAULT, &pTextures[numTextures]);
	pTextures[numTextures]->LoadTexture(0, "metal.bmp");
	numTextures++;

	return true;
}


void CSwpChainTest::Cleanup()
{
	for ( ; numTextures > 0; )
	{
		--numTextures;
		RELEASE( pTextures[numTextures]);
	}

	for ( UINT i=0; i<NUM_SWPCHN; i++)
	{
		RELEASE(pAdditionalSrcSwpChn[i]);
		RELEASE(pAdditionalRefSwpChn[i]);
	}
	
	CD3DTest::Cleanup();
}


UINT CSwpChainTest::TestInitialize(void)
{
	numObjects = 0;

	// set the additional swap chain views.

	D3DVECTOR From, At, Up;

	Up.x = 0.0f;
	Up.y = 1.0f;
	Up.z = 0.0f;

	// front view

	From.x = 128.0f;
	From.y = 128.0f;
	From.z = -200.0f;

	At.x = 64.0f;
	At.y = 64.0f;
	At.z = 153.0f;

	//ViewSwpChn[0] = ViewMatrix(From, At, Up);
	D3DXMatrixLookAtLH((D3DXMATRIX *)(&ViewSwpChn[0]), (D3DXVECTOR3 *)(&From), (D3DXVECTOR3 *)(&At), (D3DXVECTOR3 *)(&Up));

	// side view

	From.x = 350.0f;
	From.y = 64.0f;
	From.z = 0.0f;

	At.x = 128.0f;
	At.y = 64.0f;
	At.z = 25.0f;

	//ViewSwpChn[1] = ViewMatrix(From, At, Up);
	D3DXMatrixLookAtLH((D3DXMATRIX *)(&ViewSwpChn[1]), (D3DXVECTOR3 *)(&From), (D3DXVECTOR3 *)(&At), (D3DXVECTOR3 *)(&Up));

	// top view

	From.x = 64.0f;
	From.y = 350.0f;
	From.z = 50.0f;

	At.x = 64.0f;
	At.y = 128.0f;
	At.z = 25.0f;

	//ViewSwpChn[2] = ViewMatrix(From, At, Up);
	D3DXMatrixLookAtLH((D3DXMATRIX *)(&ViewSwpChn[2]), (D3DXVECTOR3 *)(&From), (D3DXVECTOR3 *)(&At), (D3DXVECTOR3 *)(&Up));


#ifdef FAST_TEST
	SetTestRange(1, 2);
#else
    SetTestRange(1, 30);
#endif

    if (0x0800 > m_dwVersion)
        return D3DTESTINIT_SKIPALL;

    if(KeySet("!Tags"))
        fTag = false;

	g_pFramework->SetDisplaySize(IMGWIDTH * 4, IMGHEIGHT * 2);


	///////////////////////////////////////////////////


	UINT primNum;
	WORD indx[24];

	typedef struct { float x,y,z; D3DCOLOR d; float u, v;} VRTX1;


	//////////////////////////////////////////////////
	//Make the "Block" object.
	//////////////////////////////////////////////////

	
	VRTX1 ObjectVertices[24]=
	{
		{ 0.0f,  0.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.0f, 0.0f
		 },
		{ 32.0f,  0.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.250f, 0.0f
		 },
		{ 96.0f,  0.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.750f, 0.0f
		 },
		{ 128.0f,  0.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 1.0f, 0.0f
		 },
		{ 128.0f,  128.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 1.0f, 1.0f
		 },
		{ 96.0f,  128.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.750f, 1.0f
		 },
		{ 32.0f,  128.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.250f, 1.0f
		 },
		{ 0.0f,  128.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.0f, 1.0f
		 },
		{ 32.0f,  32.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.250f, 0.250f
		 },
		{ 96.0f,  32.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.750f, 0.250f
		 },
		{ 96.0f,  96.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.750f, 0.750f
		 },
		{ 32.0f,  96.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.250f, 0.750f
		 },



		{ 0.0f,  0.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.0f, 1.0f
		 },
		{ 32.0f,  0.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.0f, 0.750f
		 },
		{ 96.0f,  0.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.0f, 0.250f
		 },
		{ 128.0f,  0.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.0f, 0.0f
		 },
		{ 128.0f,  128.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 1.0f, 0.0f
		 },
		{ 96.0f,  128.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 1.0f, 0.250f
		 },
		{ 32.0f,  128.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 1.0f, 0.750f
		 },
		{ 0.0f,  128.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 1.0f, 1.0f
		 },
		{ 32.0f,  32.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.250f, 0.750f
		 },
		{ 96.0f,  32.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.250f, 0.250f
		 },
		{ 96.0f,  96.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.750f, 0.250f
		 },
		{ 32.0f,  96.0f, 128.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.750f, 0.750f
		 },


	};


	objects[0].pObject = new CBase3dObject(24, ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 ), ObjectVertices, this);
	numObjects++;

	//construct the object using primitives.


	// outer covering as a triangle strip /////////////////////////

	indx[0] = 0;
	indx[1] = 12;
	indx[2] = 3;
	indx[3] = 15;
	indx[4] = 4;
	indx[5] = 16;
	indx[6] = 7;
	indx[7] = 19;
	indx[8] = 0;
	indx[9] = 12;
	primNum = objects[0].pObject->AddIndxPrimitive(indx, 10, D3DPT_TRIANGLESTRIP);
	objects[0].pObject->SetRenderStatePrimitive(primNum, D3DRS_LIGHTING, FALSE);
	//objects[0].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_CW);
	objects[0].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_NONE);
	objects[0].pObject->SetTexturePrimitiveIndex(primNum, 0, 0 );
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	// check multitexturing, if supported.
	if (  (m_pSrcDevice->GetCaps()->wMaxTextureBlendStages > 1) &&
		  (m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures > 1)  )
	{
		objects[0].pObject->SetTexturePrimitiveIndex(primNum, 1, 1);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLOROP, D3DTOP_MODULATE);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_TEXCOORDINDEX, (DWORD)0);
	}
	else
	{

		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	}



	// front face as a triangle list ////////////////////////////
	indx[0] = 0;
	indx[1] = 6;
	indx[2] = 7;
	indx[3] = 0;
	indx[4] = 1;
	indx[5] = 6;
	indx[6] = 11;
	indx[7] = 5;
	indx[8] = 6;
	indx[9] = 11;
	indx[10] = 10;
	indx[11] = 5;
	indx[12] = 1;
	indx[13] = 9;
	indx[14] = 8;
	indx[15] = 1;
	indx[16] = 2;
	indx[17] = 9;
	indx[18] = 2;
	indx[19] = 4;
	indx[20] = 5;
	indx[21] = 2;
	indx[22] = 3;
	indx[23] = 4;
	primNum = objects[0].pObject->AddIndxPrimitive(indx, 24, D3DPT_TRIANGLELIST);
	objects[0].pObject->SetRenderStatePrimitive(primNum, D3DRS_LIGHTING, FALSE);
	//objects[0].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_CW);
	objects[0].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_NONE);
	objects[0].pObject->SetTexturePrimitiveIndex(primNum, 0, 2);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);



	// inner covering as a triangle strip /////////////////////////

	indx[0] = 8;
	indx[1] = 20;
	indx[2] = 9;
	indx[3] = 21;
	indx[4] = 10;
	indx[5] = 22;
	indx[6] = 11;
	indx[7] = 23;
	indx[8] = 8;
	indx[9] = 20;
	primNum = objects[0].pObject->AddIndxPrimitive(indx, 10, D3DPT_TRIANGLESTRIP);
	objects[0].pObject->SetRenderStatePrimitive(primNum, D3DRS_LIGHTING, FALSE);
	//objects[0].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_CW);
	objects[0].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_NONE);
	objects[0].pObject->SetTexturePrimitiveIndex(primNum, 0, 3);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	objects[0].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	// check multitexturing, if supported.
	if (  (m_pSrcDevice->GetCaps()->wMaxTextureBlendStages > 1) &&
		  (m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures > 1)  )
	{
		objects[0].pObject->SetTexturePrimitiveIndex(primNum, 1, 4);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLOROP, D3DTOP_MODULATE);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_TEXCOORDINDEX, (DWORD)0);
	}
	else
	{

		objects[0].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	}



	//////////////////////////////////////////////////
	//Make the "Slider" object.
	//////////////////////////////////////////////////

	
	VRTX1 ObjectVertices2[13]=
	{
		{ 0.0f,  0.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.0f, 0.0f
		 },
		{ 64.0f,  0.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 1.0f, 0.0f
		 },
		{ 64.0f,  64.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 1.0f, 1.0f
		 },
		{ 0.0f,  64.0f, 0.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.0f, 1.0f
		 },



		{ 0.0f,  0.0f, 64.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.0f, 1.0f
		 },
		{ 64.0f,  0.0f, 64.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.0f, 0.0f
		 },
		{ 64.0f,  64.0f, 64.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 1.0f, 0.0f
		 },
		{ 0.0f,  64.0f, 64.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 1.0f, 1.0f
		 },



		{ 32.0f,  80.0f, 18.0f,
		 D3DCOLOR_RGBA( 0xff, 0xff, 0xff, 0xff ),
		 0.0f, 0.0f
		 },
		{ 24.0f,  64.0f, 10.0f,
		 D3DCOLOR_RGBA( 0x44, 0x44, 0x44, 0xff ),
		 0.0f, 1.0f
		 },
		{ 40.0f,  64.0f, 10.0f,
		 D3DCOLOR_RGBA( 0xee, 0xee, 0xee, 0xff ),
		 1.0f, 1.0f
		 },
		{ 40.0f,  64.0f, 26.0f,
		 D3DCOLOR_RGBA( 0x44, 0x44, 0x44, 0xff ),
		 1.0f, 0.0f
		 },
		{ 24.0f,  64.0f, 26.0f,
		 D3DCOLOR_RGBA( 0xee, 0xee, 0xee, 0xff ),
		 1.0f, 1.0f
		 },


	};


	objects[1].pObject = new CBase3dObject(13, ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 ), ObjectVertices2, this);
	numObjects++;

	//construct the object using primitives.


	// outer covering as a triangle strip /////////////////////////

	indx[0] = 0;
	indx[1] = 4;
	indx[2] = 1;
	indx[3] = 5;
	indx[4] = 2;
	indx[5] = 6;
	indx[6] = 3;
	indx[7] = 7;
	indx[8] = 0;
	indx[9] = 4;
	primNum = objects[1].pObject->AddIndxPrimitive(indx, 10, D3DPT_TRIANGLESTRIP);
	objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_LIGHTING, FALSE);
	//objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_CW);
	objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_NONE);
	objects[1].pObject->SetTexturePrimitiveIndex(primNum, 0, 5);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	objects[1].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);


	// front face as a triangle list ////////////////////////////
	indx[0] = 0;
	indx[1] = 2;
	indx[2] = 3;
	indx[3] = 0;
	indx[4] = 1;
	indx[5] = 2;
	primNum = objects[1].pObject->AddIndxPrimitive(indx, 6, D3DPT_TRIANGLELIST);
	objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_LIGHTING, FALSE);
	//objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_CW);
	objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_NONE);
	objects[1].pObject->SetTexturePrimitiveIndex(primNum, 0, 6);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	objects[1].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);


	// back face as a triangle list too ////////////////////////////
	indx[0] = 4;
	indx[1] = 7;
	indx[2] = 6;
	indx[3] = 4;
	indx[4] = 6;
	indx[5] = 5;
	primNum = objects[1].pObject->AddIndxPrimitive(indx, 6, D3DPT_TRIANGLELIST);
	objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_LIGHTING, FALSE);
	//objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_CW);
	objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_NONE);
	objects[1].pObject->SetTexturePrimitiveIndex(primNum, 0, 6);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	objects[1].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);


	// "marker" as a triangle fan ////////////////////////////
	indx[0] = 8;
	indx[1] = 9;
	indx[2] = 10;
	indx[3] = 11;
	indx[4] = 12;
	indx[5] = 9;
	primNum = objects[1].pObject->AddIndxPrimitive(indx, 6, D3DPT_TRIANGLEFAN);
	objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_LIGHTING, FALSE);
	//objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_CW);
	objects[1].pObject->SetRenderStatePrimitive(primNum, D3DRS_CULLMODE, D3DCULL_NONE);
	objects[1].pObject->SetTexturePrimitiveIndex(primNum, 0, 7);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	objects[1].pObject->SetTSSPrimitive(primNum, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	objects[1].pObject->SetTSSPrimitive(primNum, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);



	////////////// Object Transformations. //////////////////////
	// transformation for "Block" object ////////////

	//objects[0].World = TranslateMatrix(0.0f, 0.0f, 25.0f);
	D3DXMatrixTranslation((D3DXMATRIX *)(&objects[0].World), 0.0f, 0.0f, 25.0f);


	From.x = 250.0f;
	From.y = 250.0f;
	From.z = -125.0f;

	//From.x = 300.0f;
	//From.y = 300.0f;
	//From.z = -175.0f;

	At.x = 0.0f;
	At.y = 0.0f;
	At.z = 125.0f;

	Up.x = 0.0f;
	Up.y = 1.0f;
	Up.z = 0.0f;

	//objects[0].View = ViewMatrix(From, At, Up);
	D3DXMatrixLookAtLH((D3DXMATRIX *)(&objects[0].View), (D3DXVECTOR3 *)(&From), (D3DXVECTOR3 *)(&At), (D3DXVECTOR3 *)(&Up));

	//objects[0].Projection = ProjectionMatrix(1.0, 1000.0, PI/4.0f);    
    D3DXMatrixPerspectiveFovLH((D3DXMATRIX *)(&objects[0].Projection), PI/4.0f, (float)IMGHEIGHT/(float)IMGWIDTH, 1.0, 1000.0);




	// transformation for "Slider" object ////////////

	objects[1].View = objects[0].View;
	objects[1].Projection = objects[0].Projection;

		



    return D3DTESTINIT_RUN;
}


bool CSwpChainTest::ExecuteTest(UINT uiTest)
{

	// transformation change for "Slider" object ////////////

	//objects[1].World = TranslateMatrix(32.0f, 32.0f, -3.0*uiTest);
	D3DXMatrixTranslation((D3DXMATRIX *)(&objects[1].World), 32.0f, 32.0f, -3.0f*uiTest);

	BeginTestCase("Object position", uiTest);

	return true;
}






void CSwpChainTest::SceneRefresh(void)
{
	UINT i;

	if (BeginScene())
	{

		for (i=0; i<numObjects; i++)
		{
			if (!(objects[i].pObject->Render(&(objects[i].World), &(objects[i].View), &(objects[i].Projection), pTextures )))
				WriteToLog("(ERROR): Render Object failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
		}




		// Render the rest of the views to the swap chains

		CSurface *pSrcZBuffer=NULL,*pRefZBuffer=NULL;
		if ( (D3D_OK != (m_pSrcDevice->GetDepthStencilSurface(&pSrcZBuffer))) )
		{
			WriteToLog("(ERROR): Src GetDepthStencilSurface failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
		}
		if ( (D3D_OK != (m_pRefDevice->GetDepthStencilSurface(&pRefZBuffer))) )
		{
			WriteToLog("(ERROR): Ref GetDepthStencilSurface failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
		}

		for (int j=0; j<NUM_SWPCHN; j++)
		{
			CSurface *pNewSrcRT=NULL,*pNewRefRT=NULL;

			if ( (D3D_OK != (pAdditionalSrcSwpChn[j]->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pNewSrcRT))) )
			{
				WriteToLog("(ERROR): GetBackBuffer failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
			if ( (D3D_OK != (pAdditionalRefSwpChn[j]->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pNewRefRT))) )
			{
				WriteToLog("(ERROR): GetBackBuffer failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}

			if ( (D3D_OK != (m_pSrcDevice->SetRenderTarget(pNewSrcRT, pSrcZBuffer))) )
			{
				WriteToLog("(ERROR): SetRenderTarget failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
			if ( (D3D_OK != (m_pRefDevice->SetRenderTarget(pNewRefRT, pRefZBuffer))) )
			{
				WriteToLog("(ERROR): SetRenderTarget failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}

			RELEASE(pNewSrcRT);
			RELEASE(pNewRefRT);

			//Clear(m_dwClearColor);
			Clear(0, NULL, FMWK_DEFAULT, (0x00000055)<<(j*8), 1.f, 0);

			for (i=0; i<numObjects; i++)
			{
				if (!(objects[i].pObject->Render(&(objects[i].World), &(ViewSwpChn[j]), &(objects[i].Projection), pTextures )))
					WriteToLog("(ERROR): Render Object failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
		}

		CSurface *pSrcTarget=NULL,*pRefTarget=NULL;
		if ( (D3D_OK != (m_pSrcDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSrcTarget))) )
		{
			WriteToLog("(ERROR): Src GetBackBufferCached failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
		}
		if ( (D3D_OK != (m_pRefDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pRefTarget))) )
		{
			WriteToLog("(ERROR): Ref GetBackBufferCached failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
		}

		// Restore the original render target.
		if ( (D3D_OK != (m_pSrcDevice->SetRenderTarget(pSrcTarget, pSrcZBuffer))) )
		{
			WriteToLog("(ERROR): SetRenderTarget failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
		}
		if ( (D3D_OK != (m_pRefDevice->SetRenderTarget(pRefTarget, pRefZBuffer))) )
		{
			WriteToLog("(ERROR): SetRenderTarget failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
		}

		RELEASE(pSrcTarget);
		RELEASE(pRefTarget);

		RELEASE(pSrcZBuffer);
		RELEASE(pRefZBuffer);

        if (!EndScene())
            WriteToLog("(ERROR): EndScene failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
	}

	return;
}


bool CSwpChainTest::ProcessFrame()
{
    TCHAR   szBuffer[80];
    static  int nPass = 0;
    static  int nFail = 0;
    bool    bResult = true;
	float fLeastPassPercentage = 1.0f;

	float fPassRequired = 1.0f - 0.15f;

#pragma warning( disable : 6201 )
	for (int i=0; i<=NUM_SWPCHN; i++)
	{
		m_fPassPercentage = m_pFramework->GetImageCmp()->GetCompareResult(); 
		if (fLeastPassPercentage > m_fPassPercentage) fLeastPassPercentage = m_fPassPercentage;
		if (m_fPassPercentage < fPassRequired)
		{
			bResult = false;
		}

		WriteStatus(_T((i==0)?"FULL VIEW":szTags[i-1]), _T("$yConforms: $w%0.2f%% $yRequired: $w%0.2f%%"), m_fPassPercentage * 100.0f, fPassRequired * 100.0f);

		if (i < NUM_SWPCHN)
		{
			if (m_Options.D3DOptions.bReference)
			{
				CSurface *pNewSrcRT=NULL,*pNewRefRT=NULL;

				if ( (D3D_OK != (pAdditionalSrcSwpChn[i]->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pNewSrcRT))) )
				{
					WriteToLog("(ERROR): GetBackBuffer failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
				}
				if ( (D3D_OK != (pAdditionalRefSwpChn[i]->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pNewRefRT))) )
				{
					WriteToLog("(ERROR): GetBackBuffer failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
				}

				//The change in clear color must be informed to CompareImages.
				//Clear((0x00000055)<<(i*8));
				m_pFramework->GetImageCmp()->SetClearColor((0x00000055)<<(i*8));
	            m_pFramework->GetImageCmp()->CompareImages(pNewSrcRT,pNewRefRT);

				RELEASE(pNewSrcRT);
				RELEASE(pNewRefRT);
			}
		}
	}
#pragma warning( default : 6201 )


	m_fPassPercentage = fLeastPassPercentage;


    // Tell the logfile how the compare went
    if (bResult)
    {
        nPass++;
        sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Pass();
    }
    else
    {
        nFail++;
        sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Fail();

		//log the failure result.
		WriteToLog("TEST FAILED!");
    }

    WriteStatus("$gOverall Results",szBuffer);

    return bResult;
}


bool CSwpChainTest::DisplayFrame(void)
{
	HRESULT hr;
	CSwapChain **pAdditionalSwpChn=pAdditionalSrcSwpChn;
	bool bRes = true;

	DPF(4, _T("CD3DTest::DisplayFrame\n"));

	if(m_pCurrentMode == NULL)
		return false;

    // Full screen drawing
    if(!m_pCurrentMode->bWindowed)
    {		
		// Present all fullscreen devices
		for(UINT nDevices=0; nDevices < m_uDeviceList; nDevices++)
		{
			if(m_DeviceList[nDevices]->IsFullscreen())
			{
				bRes &= DrawStats(m_pCurrentMode->nBackBufferWidth, m_pCurrentMode->nBackBufferHeight, m_DeviceList[nDevices]);
				hr = m_DeviceList[nDevices]->Present(NULL, NULL, NULL, NULL);
				if(FAILED(hr))
				{
					bRes = false;
				}
			}
		}
    }
	else
	{
        int nDevHeight = 15;

		HDC hDC = GetDC(m_pFramework->m_pDisplayWindow->m_hWindow);
		if(hDC == NULL)
		{
			DPF(1, _T("CD3DTest::DisplayFrame() - GetDC failed.\n"));
			return false;
		}

		for(UINT nDevices=0; nDevices < m_uDeviceList; nDevices++)
		{
            TCHAR tcsBuffer[256] = "\0";
			int nOffset = IMGWIDTH*(nDevices*2);
			UINT nBlue = 128;// * (1.f - (float)nDevices / (float)m_uDeviceList);
			UINT nGreen = ( 128 * nDevices ) / m_uDeviceList;

            if(m_DeviceList[nDevices]->GetDevType() != DEVICETYPE_REF)
			{
				_stprintf(tcsBuffer, _T("%s(%s)"),
						  m_DeviceList[nDevices]->GetAdapter()->sDescription,
						  m_DeviceList[nDevices]->GetAdapter()->sVersion);
				pAdditionalSwpChn = pAdditionalSrcSwpChn;
			}
			else
			{
				pAdditionalSwpChn = pAdditionalRefSwpChn;
			}

			// Present the devices
            RECT rSrc = {0, 0, IMGWIDTH, IMGHEIGHT};
			RECT rDst = {nOffset, 0, nOffset + IMGWIDTH, IMGHEIGHT};
			m_DeviceList[nDevices]->Present(&rSrc, &rDst, NULL, NULL);

			// Draw the device tag
            rDst.bottom = nDevHeight;

			ASSERT(g_pFramework != NULL);
			SelectObject(hDC, g_pFramework->GetLargeFont());
			SetBkColor(hDC,RGB(0,nGreen,nBlue));
			SetTextColor(hDC,RGB(255,255,0));

#pragma prefast(suppress: 528, "SHExtTextOut does not appear in publics.")
			ExtTextOut(hDC,10 + rDst.left,1 + rDst.top,ETO_OPAQUE | ETO_CLIPPED,&rDst,
					   m_DeviceList[nDevices]->GetDescription(),
					   (UINT) _tcslen(m_DeviceList[nDevices]->GetDescription()),NULL);

			// Draw the adapter tag
			int nDriverHeight = m_nTagHeight - nDevHeight;
			rDst.top = nDevHeight; rDst.bottom = nDriverHeight + nDevHeight;
			SelectObject(hDC, g_pFramework->GetSmallFont());
			SetBkColor(hDC,RGB(0,nGreen,nBlue));
			SetTextColor(hDC,RGB(255,255,0));

#pragma prefast(suppress: 528, "SHExtTextOut does not appear in publics.")
            ExtTextOut(hDC,10 + rDst.left,1 + rDst.top,ETO_OPAQUE |ETO_CLIPPED, &rDst,tcsBuffer, (UINT) _tcslen(tcsBuffer),NULL);


			//present the Additional Swapchains
			for (int i=0; i<NUM_SWPCHN; i++)
			{
				rDst.left = nOffset + ( IMGWIDTH * ((i+1) % 2) );
				rDst.top = IMGHEIGHT * (int)((i+1) / 2);
				rDst.right = rDst.left + IMGWIDTH;
				rDst.bottom = rDst.top + IMGHEIGHT;
				SetLastError(hr = pAdditionalSwpChn[i]->Present(&rSrc, &rDst, NULL, NULL));
				if (FAILED(hr))
					WriteToLog("(ERROR): Present failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

				if (fTag)
				{
					// Draw the device tag
					rDst.bottom = rDst.top + nDevHeight;
					SelectObject(hDC, g_pFramework->GetLargeFont());
					SetBkColor(hDC,RGB(0,nGreen,nBlue));
					SetTextColor(hDC,RGB(255,255,0));

#pragma prefast(suppress: 528, "SHExtTextOut does not appear in publics.")
					ExtTextOut(hDC,10 + rDst.left,1 + rDst.top,ETO_OPAQUE | ETO_CLIPPED,&rDst,
							   szTags[i], (UINT) _tcslen(szTags[i]),NULL);
						   
				}
			}



		}


/*
		// Show the Compare Window
        if (m_bShowCompareWindow && m_pCmpSwapChain)
		{
			TCHAR *pcsDesc = _T("Image Compare\0");
			nDevHeight = 20;
            int nOffset = IMGWIDTH*m_uDeviceList;
			rRect.left = nOffset; rRect.top = 0; rRect.right = nOffset + IMGWIDTH; rRect.bottom = nDevHeight;
			SelectObject(hDC, g_pFramework->GetLargeFont());
			SetBkColor(hDC,RGB(128,0,0));
			SetTextColor(hDC,RGB(255,255,0));
			ExtTextOut(hDC,10 + nOffset,1,ETO_OPAQUE | ETO_CLIPPED,&rRect,pcsDesc,_tcslen(pcsDesc),NULL);

			pcsDesc = _T("\0");
			int nDriverHeight = m_nTagHeight - nDevHeight;
			rRect.left = nOffset; rRect.top = nDevHeight; rRect.right = nOffset + IMGWIDTH; rRect.bottom = nDriverHeight + nDevHeight;
			SelectObject(hDC, g_pFramework->GetSmallFont());
			SetBkColor(hDC,RGB(128,0,0));
			SetTextColor(hDC,RGB(255,255,0));
            ExtTextOut(hDC,10 + nOffset,nDevHeight+1,ETO_OPAQUE |ETO_CLIPPED, &rRect,pcsDesc,_tcslen(pcsDesc),NULL);

			// Present the compare
			RECT rSrc = {0, 0, IMGWIDTH, IMGHEIGHT};
			RECT rDst = {nOffset, m_nTagHeight, nOffset + IMGWIDTH, m_nTagHeight + IMGHEIGHT};
			m_pCmpSwapChain->Present(&rSrc, &rDst, NULL, NULL);
		}
*/


		// Release the DC
		if(hDC)
			ReleaseDC(m_pFramework->m_pDisplayWindow->m_hWindow,hDC);
	}

    return true;
}



bool CSwpChainTest::TestTerminate(void)
{
	for (UINT i=0; i<numObjects; i++)
	{
		delete objects[i].pObject;
	}

	return CD3DTest::TestTerminate();
}
