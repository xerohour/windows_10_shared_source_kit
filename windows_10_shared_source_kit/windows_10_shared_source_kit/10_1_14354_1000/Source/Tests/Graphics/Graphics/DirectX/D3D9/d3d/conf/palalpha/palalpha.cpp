#define D3D_OVERLOADS
#include "d3d.h"
#include "PalAlpha.h"
#include "3dmath.h"

CD3DWindowFramework		theApp;
CPalTest				TestOne;


D3DTLVERTEX		CPalTest::vertices1[] = {
	D3DTLVERTEX(D3DVECTOR(0.f,  0.f, 0.f), 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), 0.01f, 0.99f),
	D3DTLVERTEX(D3DVECTOR(10.f, 0.f, 0.f), 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), 0.99f, 0.99f),
	D3DTLVERTEX(D3DVECTOR(10.f,10.f, 0.f), 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), 0.99f, 0.01f),
	D3DTLVERTEX(D3DVECTOR(0.f, 10.f, 0.f), 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), 0.01f, 0.01f),
};

D3DTLVERTEX		CPalTest::vertices2[] = {
	D3DTLVERTEX(D3DVECTOR(0.f,  0.f, 0.f), 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), -1.f,  2.f),
	D3DTLVERTEX(D3DVECTOR(10.f, 0.f, 0.f), 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00),  2.f,  2.f),
	D3DTLVERTEX(D3DVECTOR(10.f,10.f, 0.f), 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00),  2.f, -1.f),
	D3DTLVERTEX(D3DVECTOR(0.f, 10.f, 0.f), 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), -1.f, -1.f),
};

WORD			CPalTest::indices[]  = {0, 1, 2, 0, 2, 3};

CPalTest::CPalTest()
{
	bTest4BitPalette = true;
	bTest8BitPalette = true;
	bTestNonAlphaPalette = true;
	bTestAlphaPalette = true;
	dwCurrentTextureType = UNDEFINED_TEXTURE;
	bCreateNewTexture = false;
	lpszTextureType = NULL;
	
	bTestTextureCreate = true;
	bTestTextureLoad = true;
	bTestPaletteCreate = true;
	bTestSetEntries = true;
	bTestWrapMode = true;
	bTestMirrorMode = true;
	bTestClampMode = true;
	bTestBorderMode = true;
	bTestColorKey = true;
    bTestColorKeyBlend = true;
	lpszTestType = NULL;

	dwTextureTypesNumber = 4;
	dwTestTypesNumber = 10;
	dwSameFramesNumber = 64;
	dwTotalTestsNumber = dwTextureTypesNumber*dwTestTypesNumber*dwSameFramesNumber;

	dwPaletteType = 3;
	bAlphaPalette = true;
	bCreateTexture = false;
	bLoadTexture = false;
	bCreatePalette = false;
	bSetPaletteEntries = true;
	dwAddressMode = 0;
	bUseColorKey = false;
    bUseColorKeyBlend = false;
	dwFrameParam = 0;
	bLastTestFailed = false;
	dwCurrentTexturePalette = 0;

	pPalette = NULL;
	dwNumFail = 0;

    pVertices = vertices1;
    dwVertexCount=4;
	pIndices = indices;
	dwIndexCount = 6;

	m_pTexture = 0;
	m_pDestTexture = 0;

	SetTestType(TESTTYPE_CONF);
	m_szTestName = "Palettized texture test";
	m_szCommandKey = "PalAlpha";

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_7;
}

CPalTest::~CPalTest() {
    RELEASE(m_pTexture);
	RELEASE(m_pDestTexture);
}


UINT CPalTest::TestInitialize(void) {

    if (m_dwVersion >= 0x0700)
    {
        bTestPaletteCreate = true;
    }
    else
    {
        if (KeySet("RECREATEPALETTE"))
            bTestPaletteCreate = true;
        else
            bTestPaletteCreate = false;
    }

    if (m_dwVersion >= 0x0800)
    {
        bTestTextureLoad = false;
        bTestColorKeyBlend = false;
        bTestColorKey = false;
        bTest4BitPalette = false;
    }

	// Check the device supports paletted textures
	if (bTest8BitPalette) {
		bTest8BitPalette = false;
		for (int i = 0; i < m_uCommonTextureFormats; i++) {
			if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
				bTest8BitPalette = true;
		}
	}

	if (bTest4BitPalette) {
		bTest4BitPalette = false;
		for (int i = 0; i < m_uCommonTextureFormats; i++) {
			if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
				bTest4BitPalette = true;
		}
	}
	
	// Check the device supports alpha in texture palette

	if (bTestAlphaPalette && 
		m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE &&
		m_pRefDevice->GetCaps()->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE )
		bTestAlphaPalette = true;
	else
		bTestAlphaPalette = false;

	// Check supported addressing modes
	if (bTestWrapMode && 
		m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps & D3DPTADDRESSCAPS_WRAP &&
		m_pRefDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps & D3DPTADDRESSCAPS_WRAP )
		bTestWrapMode = true;
	else
		bTestWrapMode = false;
	
	if (bTestMirrorMode && 
		m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps & D3DPTADDRESSCAPS_MIRROR &&
		m_pRefDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps & D3DPTADDRESSCAPS_MIRROR )
		bTestMirrorMode = true;
	else
		bTestMirrorMode = false;

	if (bTestClampMode && 
		m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps & D3DPTADDRESSCAPS_CLAMP &&
		m_pRefDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps & D3DPTADDRESSCAPS_CLAMP )
		bTestClampMode = true;
	else
		bTestClampMode = false;
	
	if (bTestBorderMode && 
		m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps & D3DPTADDRESSCAPS_BORDER &&
		m_pRefDevice->GetCaps()->dpcTriCaps.dwTextureAddressCaps & D3DPTADDRESSCAPS_BORDER )
		bTestBorderMode = true;
	else
		bTestBorderMode = false;

	if (bTestColorKey && 
		m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_TRANSPARENCY &&
		m_pRefDevice->GetCaps()->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_TRANSPARENCY )
		bTestColorKey = true;
	else
		bTestColorKey = false;

	if (bTestColorKeyBlend && 
		m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_COLORKEYBLEND &&
		m_pRefDevice->GetCaps()->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_COLORKEYBLEND )
		bTestColorKeyBlend = true;
	else
		bTestColorKeyBlend = false;

	if (m_dwVersion >= 0x0800)
        SetTestRange(1,(dwTotalTestsNumber>>1));
    else
        SetTestRange(1,dwTotalTestsNumber);
		
	// Return error if device do not support paletted textures
	if (!bTest4BitPalette && !bTest8BitPalette) {
		WriteToLog("PalAlpha: Device does not support palettized textures.\n");
		return D3DTESTINIT_SKIPALL;
	}

	if (!bTestAlphaPalette && !bTestNonAlphaPalette) {
		WriteToLog("PalAlpha: Device does not support alpha in texture palette.\n");
		return D3DTESTINIT_SKIPALL;
	}
 
	vertices1[1].sx = vertices2[1].sx = float(m_pCurrentMode->nBackBufferWidth);
	vertices1[2].sx = vertices2[2].sx = float(m_pCurrentMode->nBackBufferWidth);
	vertices1[2].sy = vertices2[2].sy = float(m_pCurrentMode->nBackBufferHeight);
	vertices1[3].sy = vertices2[3].sy = float(m_pCurrentMode->nBackBufferHeight);

	return D3DTESTINIT_RUN;
}


bool CPalTest::ExecuteTest(UINT iTestNum) {
	char		name_buf[80];
    bool		bColorKeyFill;

	dwCurrentTestNumber = iTestNum;
	ResolveTestNumber(iTestNum);
	sprintf(name_buf, "%s (%s)", lpszTestType, lpszTextureType);
	if (!ValidateTest(iTestNum))
		return false;

	BeginTestCase(name_buf, iTestNum);

    bColorKeyFill = (bUseColorKey || bUseColorKeyBlend) ? true : false;

	if (m_dwVersion >= 0x0800)
	{
        if (bCreateNewTexture) {
		    SetTexture(0,0);
		    RELEASE(m_pTexture);
            m_pTexture = CreateTexture(256, 256, dwPaletteType, true, bColorKeyFill);
		    SetPaletteEntries(0, bAlphaPalette, dwFrameParam);
            SetCurrentTexturePalette(0);
		    SetTexture(0,m_pTexture);
		    bCreateNewTexture = false;
	    }

	    if (bCreateTexture) {
		    SetTexture(0,0);
		    RELEASE(m_pTexture);
		    m_pTexture = CreateTexture(256, 256, dwPaletteType, true, bColorKeyFill);
		    SetTexture(0,m_pTexture);
	    }

	    if (bCreatePalette) {
		    SetPaletteEntries(dwFrameParam, bAlphaPalette, dwFrameParam);
            SetCurrentTexturePalette(dwFrameParam);
	    }

	    if (bSetPaletteEntries) {
		    SetPaletteEntries(dwCurrentTexturePalette, bAlphaPalette, dwFrameParam);
	    }
	}
    else
    {

        if (bCreateNewTexture) {
		    SetTexture(0,0);
		    RELEASE(m_pTexture);
			m_pTexture = CreateTexture(256, 256, dwPaletteType, true, bColorKeyFill);
		    CreateTexturePalette(m_pTexture, dwPaletteType, bAlphaPalette, dwFrameParam, bColorKeyFill);
		    SetTexture(0,m_pTexture);
		    bCreateNewTexture = false;
	    }

	    if (bCreateTexture) {
		    SetTexture(0,0);
		    RELEASE(m_pTexture);
			m_pTexture = CreateTexture(256, 256, dwPaletteType, true, bColorKeyFill);
//		    SetTexture(0,m_pTexture);
	    }

	    if (bCreatePalette) {
		    CreateTexturePalette(m_pTexture, dwPaletteType, bAlphaPalette, dwFrameParam, bColorKeyFill);
		    SetTexture(0,m_pTexture);
	    }

	    if (bSetPaletteEntries) {
		    ChangeTexturePalette(m_pTexture, dwPaletteType, bAlphaPalette, dwFrameParam, bColorKeyFill);
		    SetTexture(0,m_pTexture);
	    }

	    if (bLoadTexture) {
			SetTexture(0,0);
		    RELEASE(m_pDestTexture);
			m_pDestTexture = CreateTexture(256, 256, dwPaletteType, false, bColorKeyFill);
		    CreateTexturePalette(m_pDestTexture, dwPaletteType, bAlphaPalette, 0, bColorKeyFill);
		    UpdateTexture(m_pTexture, m_pDestTexture);
/*
			
			if (m_dwVersion > 0x0600)
		    {
			    SetLastError(m_pSrcDevice->Load7(pSrcDestTexture->GetD3DTexture7(), &pt, pSrcTexture->GetD3DTexture7(), &rect, 0));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Texture load failed\n");
                    Fail();
				    return false;
			    }
			    SetLastError(m_pRefDevice->Load7(pRefDestTexture->GetD3DTexture7(), &pt, pRefTexture->GetD3DTexture7(), &rect, 0));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Texture load failed\n");
                    Fail();
				    return false;
			    }
		    }
		    else
		    {
			    LPDIRECT3DTEXTURE2	pSrcTexture2, pDstTexture2;

			    SetLastError(m_pTexture->m_pSrcSurface->m_pSurface4->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pSrcTexture2));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Can't QI texture interface\n");
                    Fail();
				    return false;
			    }
			    SetLastError(m_pTextureSource->m_pSrcSurface->m_pSurface4->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pDstTexture2));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Can't QI texture interface\n");
                    Fail();
				    return false;
			    }
			    SetLastError(pDstTexture2->Load(pSrcTexture2));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Texture load failed\n");
                    Fail();
				    return false;
			    }
			    RELEASE(pSrcTexture2);
			    RELEASE(pDstTexture2);

			    SetLastError(m_pTexture->m_pRefSurface->m_pSurface4->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pSrcTexture2));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Can't QI texture interface\n");
                    Fail();
				    return false;
			    }
			    SetLastError(m_pTextureSource->m_pRefSurface->m_pSurface4->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pDstTexture2));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Can't QI texture interface\n");
                    Fail();
				    return false;
			    }
			    SetLastError(pDstTexture2->Load(pSrcTexture2));
			    RELEASE(pSrcTexture2);
			    RELEASE(pDstTexture2);
		    }
*/
		    SetTexture(0,m_pDestTexture);
	    }

	    if (bUseColorKey || bUseColorKeyBlend) {
		    DDCOLORKEY	key;

		    if (dwPaletteType == 3) {
			    key.dwColorSpaceLowValue = 120;
			    key.dwColorSpaceHighValue = 120;
		    }
		    else {
			    key.dwColorSpaceLowValue = 8;
			    key.dwColorSpaceHighValue = 8;
		    }

		    if (bUseColorKey)
                SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)TRUE);
            else
                SetRenderState(D3DRENDERSTATE_COLORKEYBLENDENABLE, (DWORD)TRUE);

		    if (!m_pTexture->SetColorKey(DDCKEY_SRCBLT, (DWORD*)&key))
		    {
			    WriteToLog("PalAlpha: SetColorKey failed\n");
                Fail();
			    return false;
		    }
	    }
	    else {
		    SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)FALSE);
            SetRenderState(D3DRENDERSTATE_COLORKEYBLENDENABLE, (DWORD)FALSE);
	    }
    }

	switch (dwAddressMode) {
	case 0:
		pVertices = vertices1;
//		SetTextureStageState(0, D3DTSS_ADDRESS, (DWORD)D3DTADDRESS_WRAP);
		SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_WRAP);
		SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_WRAP);
		break;
	case D3DTADDRESS_WRAP:
		pVertices = vertices2;
//		SetTextureStageState(0, D3DTSS_ADDRESS, (DWORD)D3DTADDRESS_WRAP);
		SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_WRAP);
		SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_WRAP);
		break;
	case D3DTADDRESS_MIRROR:
		pVertices = vertices2;
//		SetTextureStageState(0, D3DTSS_ADDRESS, (DWORD)D3DTADDRESS_MIRROR);
		SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
		SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRROR);
		break;
	case D3DTADDRESS_CLAMP:
		pVertices = vertices2;
//		SetTextureStageState(0, D3DTSS_ADDRESS, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
		break;
	case D3DTADDRESS_BORDER:
		pVertices = vertices2;
//		SetTextureStageState(0, D3DTSS_ADDRESS, (DWORD)D3DTADDRESS_BORDER);
		SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_BORDER);
		SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_BORDER);
		SetTextureStageState(0, D3DTSS_BORDERCOLOR, RGBA_MAKE(100, 100, 100, 100));
		break;
	}

	return true;
}

bool CPalTest::ClearFrame()
{
	CSurface	*pSurface;
	if (FAILED(m_pSrcDevice->GetBackBufferCached(0, 0, &pSurface)) || !ColorSurface(pSurface))
		return false;
	RELEASE(pSurface);
	if (FAILED(m_pRefDevice->GetBackBufferCached(0, 0, &pSurface)) || !ColorSurface(pSurface))
		return false;
	RELEASE(pSurface);
    return true;
}

void CPalTest::SceneRefresh() {
	D3DMATRIX	Matrix;

	Matrix = IdentityMatrix();
  
	if (BeginScene())  
	{
		SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix);
		RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, pVertices, 4, pIndices, 6, NULL);

		EndScene();
    }

    return;
}

bool CPalTest::TestTerminate()
{
    RELEASE(m_pTexture);
	RELEASE(m_pDestTexture);
    return true;
}

void CPalTest::ResolveTestNumber(int iTestNum) {
	DWORD	num = iTestNum - 1;
	
	dwFrameParam = num % dwSameFramesNumber;

	num /= dwSameFramesNumber;
	dwCurrentTestType = num%10;
	switch(dwCurrentTestType) {
	case 0:
		// Texture create test
		bCreateTexture = true;
		bLoadTexture = false;
		bCreatePalette = true;
		bSetPaletteEntries = false;
		dwAddressMode = 0;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = "Create texture";
		break;
	case 1:
		// Texture load test
		bCreateTexture = false;
		bLoadTexture = true;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = 0;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = "Load texture";
		break;
	case 2:
		// Palette create test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = true;
		bSetPaletteEntries = false;
		dwAddressMode = 0;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = "Create palette";
		break;
	case 3:
		// SetEntries test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = 0;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = "Set entries";
		break;
	case 4:
		// WrapMode test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = D3DTADDRESS_WRAP;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = "Wrap addressing mode";
		break;
	case 5:
		// MirrorMode test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = D3DTADDRESS_MIRROR;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = "Mirror addressing mode";
		break;
	case 6:
		// ClampMode test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = D3DTADDRESS_CLAMP;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = "Clamp addressing mode";
		break;
	case 7:
		// BorderMode test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = D3DTADDRESS_BORDER;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = "Border addressing mode";
		break;
	case 8:
		// ColorKey test
		bCreateNewTexture = true;
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = 0;
		bUseColorKey = true;
        bUseColorKeyBlend = false;
		lpszTestType = "color key";
		break;
    case 9:
        // ColorKeyBlend test
		bCreateNewTexture = true;
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = 0;
		bUseColorKey = false;
        bUseColorKeyBlend = true;
		lpszTestType = "color key blend";
		break;
	} 

	num /= 10;
	switch (num) {
	case 0:
		dwPaletteType = 3;
		bAlphaPalette = true;
		lpszTextureType = "8 bit alpha";
		if (dwCurrentTextureType != PAL8ALPHA) {
			dwCurrentTextureType = PAL8ALPHA;
			bCreateNewTexture = true;
			bCreateTexture = false;
			bCreatePalette = false;
		}
		break;
	case 1:
		dwPaletteType = 3;
		bAlphaPalette = false;
		lpszTextureType = "8 bit";
		if (dwCurrentTextureType != PAL8) {
			dwCurrentTextureType = PAL8;
			bCreateNewTexture = true;
//			bCreateTexture = false;
//			bCreatePalette = false;
		}
		break;
	case 2:
		dwPaletteType = 2;
		bAlphaPalette = true;
		lpszTextureType = "4 bit alpha";
		if (dwCurrentTextureType != PAL4ALPHA) {
			dwCurrentTextureType = PAL4ALPHA;
			bCreateNewTexture = true;
			bCreateTexture = false;
			bCreatePalette = false;
		}
		break;
	case 3:
		dwPaletteType = 2;
		bAlphaPalette = false;
		lpszTextureType = "4 bit";
		if (dwCurrentTextureType != PAL4) {
			dwCurrentTextureType = PAL4;
			bCreateNewTexture = true;
			bCreateTexture = false;
			bCreatePalette = false;
		}
		break;
	}
	WriteStatus("$wTexture type$g", lpszTextureType);
	WriteStatus("$wTest type$g", lpszTestType);
}

bool CPalTest::ValidateTest(int iTestNum) {
	int	test = iTestNum - 1;

	if (bLastTestFailed) {	
        bLastTestFailed = false;
		bCreateNewTexture = true;
//		SkipTests(dwSameFramesNumber - test%dwSameFramesNumber);
//		return false;
	}

	// Validate texture type
	if ( (dwPaletteType == 3 && !bTest8BitPalette) || (dwPaletteType == 2 && !bTest4BitPalette) ) {
		int num = dwSameFramesNumber*dwTestTypesNumber*2;
		SkipTests(num - test%num);
		return false;
	}
	
	if ( (bAlphaPalette && !bTestAlphaPalette) || (!bAlphaPalette && !bTestNonAlphaPalette) ) {
		int num = dwSameFramesNumber*dwTestTypesNumber;
		SkipTests(num - test%num);
		return false;
	}

	// Validate test type
	if ((dwCurrentTestType == 0 && !bTestTextureCreate)	||
		(dwCurrentTestType == 1 && !bTestTextureLoad)	||
		(dwCurrentTestType == 2 && !bTestPaletteCreate)	||
		(dwCurrentTestType == 3 && !bTestSetEntries)	||
		(dwCurrentTestType == 4 && !bTestWrapMode)		||
		(dwCurrentTestType == 5 && !bTestMirrorMode)	||
		(dwCurrentTestType == 6 && !bTestClampMode)		||
		(dwCurrentTestType == 7 && !bTestBorderMode)	||
		(dwCurrentTestType == 8 && !bTestColorKey)      ||
		(dwCurrentTestType == 9 && !bTestColorKeyBlend)	) 
	{
		SkipTests(dwSameFramesNumber - test%dwSameFramesNumber);
		return false;
	}

    // Don't run color key test on texture with alpha information on RGB device
    if (dwCurrentTestType == 8 && bAlphaPalette && 
		(m_pSrcDevice->GetDevType() == DEVICETYPE_SW || m_pRefDevice->GetDevType() == DEVICETYPE_SW ))
    {
		SkipTests(dwSameFramesNumber - test%dwSameFramesNumber);
		return false;
    }

	return true;
}


bool CPalTest::SetDefaultRenderStates(void) {
    SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
    SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD)D3DFILTER_LINEAR);
    SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD)D3DFILTER_LINEAR);
    if (m_dwVersion >= 0x0700) {
		SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)FALSE);
	}
	SetRenderState(D3DRENDERSTATE_LASTPIXEL, (DWORD)TRUE);
  	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD) D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD) D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD) TRUE);

    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
    
    return true;
}
