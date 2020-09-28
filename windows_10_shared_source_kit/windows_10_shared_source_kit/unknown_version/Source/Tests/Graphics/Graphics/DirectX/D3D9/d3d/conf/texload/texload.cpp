#define D3D_OVERLOADS
#include <d3d.h>
#include "TexLoad.h"

// Global Test and App instance
CD3DWindowFramework	App;
CSystemTest			System;
CVideoTest			Video;
CAGPTest			AGP;
CManageTest			Manage;
CMipMapSystemTest	MipMapSystem;
CMipMapVideoTest	MipMapVideo;
CMipMapAGPTest		MipMapAGP;
CMipMapManageTest	MipMapManage;

CTexLoadTest::CTexLoadTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
    m_Options.D3DOptions.fMinDXVersion = 7.0f;
    m_Options.D3DOptions.fMaxDXVersion = 7.0f;

    // Only initialize the source
    m_Options.D3DOptions.bReference = false;

	// Initialize some variables
	bSystem = false;
	bVideo = false;
	bAGP = false;
	bManage = false;
	bMipMap = false;

	// Default size array values
	SizeArray[0] =  248.0f;
	SizeArray[1] =    1.0f;
	SizeArray[2] =    2.0f;
	SizeArray[3] =    3.0f;
	SizeArray[4] =    4.0f;
	SizeArray[5] =    6.0f; 
	SizeArray[6] =    7.0f; 
	SizeArray[7] =   12.0f; 
	SizeArray[8] =   14.0f; 
	SizeArray[9] =   24.0f; 
	SizeArray[10] =  30.0f; 
	SizeArray[11] =  48.0f;
	SizeArray[12] =  56.0f;
	SizeArray[13] =  96.0f;
	SizeArray[14] = 104.0f;
	SizeArray[15] = 120.0f;
	SizeArray[16] = 192.0f;
	SizeArray[17] = 216.0f;
}

CTexLoadTest::~CTexLoadTest()
{
}

bool CTexLoadTest::SetDefaultRenderStates(void)
{
	// Turn off Texture Perspective for all tests
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);

	if (bMipMap)
	{
		// Setup our default Texture Stage State
		SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
		SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
		SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
	}
	
	return true;
}

UINT CTexLoadTest::TestInitialize(void)
{
	// Setup the Test range
	if (bMipMap)
		SetTestRange(1, MAX_MIP_TESTS);
	else
		SetTestRange(1, MAX_TESTS * 4);

	// Make sure we are on DX7
	if ((m_dwVersion <= 0x0600) || (m_dwVersion >= 0x0800)) 
	{
		WriteToLog("This test is only valid on Dx7.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Only need to check DDI for Hardware devices
	if (m_pSrcDevice->GetDevType() == DEVICETYPE_HAL)
	{
		// Make sure that the DDI supports TexBlt
		if (m_pSrcDevice->GetDDI() < 7)
		{
			WriteToLog("This test requires a Dx7 DDI.\n");
			return D3DTESTINIT_SKIPALL;
		}
	}

	// Check the MipMap caps for MipMap tests
	if (bMipMap)
	{
		DWORD dwTextureFilterCaps = m_pSrcDevice->GetCaps()->dwTextureFilterCaps;

		// Check the Mip Filter cap
		if (!(dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))
		{
			WriteToLog("Device capability not supported: (Mip) Texture Filter Point.\n");
			return D3DTESTINIT_SKIPALL;
		}

		// Check the Min Filter cap
		if (!(dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))
		{
			WriteToLog("Device capability not supported: (Min) Texture Filter Point.\n");
			return D3DTESTINIT_SKIPALL;
		}

		// Check the Mag Filter cap
		if (!(dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))
		{
			WriteToLog("Device capability not supported: (Mag) Texture Filter Point.\n");
			return D3DTESTINIT_SKIPALL;
		}
	}

	dwDevCaps = m_pSrcDevice->GetCaps()->dwDevCaps;

	// Check the NonLocalVidMem cap for AGP tests
	if (bAGP)
	{
		if (!(dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM))
		{
			WriteToLog("Device capability not supported: TextureNonLocalVidMem.\n");
			return D3DTESTINIT_SKIPALL;
		}
	}

    // Get the current render target
    if (FAILED(m_pSrcDevice->GetRenderTarget(&pSrcTarget)))
    {
		WriteToLog("GetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pSrcTarget);
    	return D3DTESTINIT_ABORT;
    }

	// Setup the color arrays
	D3DCOLOR White[1]	= {RGBA_MAKE(255,255,255,255)};
	D3DCOLOR Yellow[1]	= {RGBA_MAKE(255,255,0,255)};
	D3DCOLOR Magenta[1]	= {RGBA_MAKE(255,0,255,255)};
	D3DCOLOR Cyan[1]	= {RGBA_MAKE(0,255,255,255)};

	Red[0]		= RGBA_MAKE(255,0,0,255);
	Green[0]	= RGBA_MAKE(0,255,0,255);
	Blue[0]		= RGBA_MAKE(0,0,255,255);

	// Load images for the full level cases
    pMipImage[0] = new CImage;
    pMipImage[0]->LoadStripes(256,256,1,White,false);
    pMipImage[1] = new CImage;
    pMipImage[1]->LoadStripes(128,128,1,Yellow,false);
    pMipImage[2] = new CImage;
    pMipImage[2]->LoadStripes(64,64,1,Magenta,false);
    pMipImage[3] = new CImage;
    pMipImage[3]->LoadStripes(32,32,1,Red,false);
    pMipImage[4] = new CImage;
    pMipImage[4]->LoadStripes(16,16,1,Cyan,false);
    pMipImage[5] = new CImage;
    pMipImage[5]->LoadStripes(8,8,1,Green,false);
    pMipImage[6] = new CImage;
    pMipImage[6]->LoadStripes(4,4,1,Blue,false);
    pMipImage[7] = new CImage;
    pMipImage[7]->LoadStripes(2,2,1,White,false);
    pMipImage[8] = new CImage;
    pMipImage[8]->LoadStripes(1,1,1,Yellow,false);

	return D3DTESTINIT_RUN;
}

bool CTexLoadTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,0,255), 1.0f, 0);
}

bool CTexLoadTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
    FORMAT *pFormat = m_pSrcDevice->GetTexFormats();
	int nTest = ((uTestNum-1) / 1) + 1;
	int nSrc  = ((uTestNum-1) % MAX_TESTS) + 1;
	int nDest = ((uTestNum-1) / MAX_TESTS) + 1;
	HRESULT hrSrc, hrSrc2, hrDest;
    TCHAR szBuffer[80];
	float fSize;

	if (bMipMap)
		fSize = SizeArray[nTest % 18];
	else
		fSize = (nSrc == 1) ? 256.0f : 128.0f;

	nSize = (int)fSize;
 	dwVertexCount = 0;
	msgString[0] = '\0';
 	szStatus[0]  = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
    RELEASE(pSrcTexture);
    RELEASE(pSrc2Texture);
    RELEASE(pDestTexture);

	// Let's build some triangles.
	if (nSize == 128)
		DrawGrid(fSize, false);
	else
		DrawGrid(fSize, true);

	if (bMipMap)
	{	
		DWORD dwSrcCaps = 0;

		sprintf(msgString, "%sMesh:%dx%d, ",msgString,nSize,nSize);

		// Determine the source surface type
		if (bSystem)
		{
			dwSrcCaps = POOL_SYSTEMMEM;

			sprintf(msgString, "%sSrc:System",msgString);
			WriteStatus("$gSource Memory","System");

			sprintf(msgString, "%s, Dest:Video",msgString);
			WriteStatus("$gDestination Memory","Video");
		}
		else if (bVideo)
		{
			dwSrcCaps = POOL_VIDEO;

			sprintf(msgString, "%sSrc:Video",msgString);
			WriteStatus("$gSource Memory","Video");

			sprintf(msgString, "%s, Dest:Video",msgString);
			WriteStatus("$gDestination Memory","Video");
		}
		else if (bAGP)
		{
			dwSrcCaps = POOL_AGP;

			sprintf(msgString, "%sSrc:AGP",msgString);
			WriteStatus("$gSource Memory","AGP");

			sprintf(msgString, "%s, Dest:Video",msgString);
			WriteStatus("$gDestination Memory","Video");
		}
		else if (bManage)
		{
			dwSrcCaps = POOL_MANAGED;

			sprintf(msgString, "%sSrc:Manage",msgString);
			WriteStatus("$gSource Memory","Manage");

			sprintf(msgString, "%s, Dest:Video",msgString);
			WriteStatus("$gDestination Memory","Video");
		}
		else
		{
			WriteToLog("Error: Mipmap memory location not set.\n");
			TestFinish(D3DTESTFINISH_ABORT);
			return false;
		}

		// Get texture format being used
		FmtToString(FORMAT_TEXTURE,pFormat,szBuffer);
		sprintf(msgString, "%s, Tex:%s",msgString,szBuffer);

		// Tell the log that we are starting
		BeginTestCase(msgString, uTestNum);

		// Create the texture
		hrSrc = m_pSrcDevice->CreateTexture(256,256,0,0,*pFormat,dwSrcCaps,&pSrcTexture);
		hrDest = m_pSrcDevice->CreateTexture(64,64,0,0,*pFormat,FMWK_DEFAULT,&pDestTexture);

		// Make sure we got valid texture pointers
		if (FAILED(hrSrc) || (NULL == pSrcTexture))
		{
			WriteToLog("Source CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(hrSrc));
			Fail();
			return false;
		}

		// Set the palette if needed
		if (pSrcTexture->IsPalettized())
			pSrcTexture->SetPalette(NULL);

        // Load image into the texture
        if (FAILED(pSrcTexture->LoadImage(0,pMipImage,TEXLOAD_LOCKCOPY)))
	    {
		    WriteToLog("LoadImage() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
            Fail();
		    return false;
	    }
	}
	else
	{
		DWORD dwSrcCaps  = 0;
		DWORD dwSrc2Caps = 0;
		DWORD dwDestCaps = 0;

		// Determine the source surface type
		if (bSystem)
		{
			dwSrcCaps = POOL_SYSTEMMEM;
			sprintf(msgString, "%sSrc:System",msgString);
			sprintf(szStatus,"%sSystem",szStatus);
		}
		else if (bVideo)
		{
			dwSrcCaps = POOL_VIDEO;
			sprintf(msgString, "%sSrc:Video",msgString);
			sprintf(szStatus,"%sVideo",szStatus);
		}
		else if (bAGP)
		{
			dwSrcCaps = POOL_AGP;
			sprintf(msgString, "%sSrc:AGP",msgString);
			sprintf(szStatus,"%sAGP",szStatus);
		}
		else if (bManage)
		{
			dwSrcCaps = POOL_MANAGED;
			sprintf(msgString, "%sSrc:Manage",msgString);
			sprintf(szStatus,"%sManage",szStatus);
		}
		else 
		{
			WriteToLog("Error: Non mipmap memory location not set.\n");
			TestFinish(D3DTESTFINISH_ABORT);
			return false;
		}

		// Determine the second source surface type
		switch (nSrc)
		{
			case 1: 
				dwSrc2Caps = 0;
				break;
			case 2:
				dwSrc2Caps = POOL_SYSTEMMEM;
				sprintf(msgString, "%s/System",msgString);
				sprintf(szStatus,"%s/System",szStatus);
				break;
			case 3:
				dwSrc2Caps = POOL_VIDEO;
				sprintf(msgString, "%s/Video",msgString);
				sprintf(szStatus,"%s/Video",szStatus);
				break;
			case 4:
				if (!(dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM))
				{
					WriteToLog("Device capability not supported: TextureNonLocalVidMem.\n");
					SkipTests(1);
					return false;
				}

				dwSrc2Caps = POOL_AGP;
				sprintf(msgString, "%s/AGP",msgString);
				sprintf(szStatus,"%s/AGP",szStatus);
				break;
			case 5:
				dwSrc2Caps = POOL_MANAGED;
				sprintf(msgString, "%s/Manage",msgString);
				sprintf(szStatus,"%s/Manage",szStatus);
				break;
		}

		// Determine the destination surface type
		switch (nDest)
		{
			case 1:
				if (!(dwDevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY))
				{
					WriteToLog("Device capability not supported: TextureSystemMemory.\n");
					SkipTests(MAX_TESTS);
					return false;
				}				

				dwDestCaps = POOL_SYSTEMMEM;
				sprintf(msgString, "%s, Dest:System",msgString);

				WriteStatus("$gSource Memory",szStatus);
				WriteStatus("$gDestination Memory","System");
				break;
			case 2:
				if (!(dwDevCaps & D3DDEVCAPS_TEXTUREVIDEOMEMORY))
				{
					WriteToLog("Device capability not supported: TextureVideoMemory.\n");
					SkipTests(MAX_TESTS);
					return false;
				}				

				dwDestCaps = POOL_VIDEO;
				sprintf(msgString, "%s, Dest:Video",msgString);

				WriteStatus("$gSource Memory",szStatus);
				WriteStatus("$gDestination Memory","Video");
				break;
			case 3:
				if (!(dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM))
				{
					WriteToLog("Device capability not supported: TextureNonLocalVidMem.\n");
					SkipTests(MAX_TESTS);
					return false;
				}

				dwDestCaps = POOL_AGP;
				sprintf(msgString, "%s, Dest:AGP",msgString);

				WriteStatus("$gSource Memory",szStatus);
				WriteStatus("$gDestination Memory","AGP");
				break;
			case 4:
				if (m_pSrcDevice->GetDevType() != DEVICETYPE_HAL)
				{
					WriteToLog("Managed textures not support by software devices.\n");
					SkipTests(MAX_TESTS);
					return false;
				}

				dwDestCaps = POOL_MANAGED;
				sprintf(msgString, "%s, Dest:Manage",msgString);

				WriteStatus("$gSource Memory",szStatus);
				WriteStatus("$gDestination Memory","Manage");
				break;
		}

		// Get texture format being used
		FmtToString(FORMAT_TEXTURE,pFormat,szBuffer);
		sprintf(msgString, "%s, Tex:%s",msgString,szBuffer);

		// Tell the log that we are starting
		BeginTestCase(msgString, uTestNum);

		// Create the textures
		if (nSrc == 1)
		{
			hrSrc = m_pSrcDevice->CreateTexture(256,256,1,0,*pFormat,dwSrcCaps,&pSrcTexture);
			hrDest = m_pSrcDevice->CreateTexture(256,256,1,0,*pFormat,dwDestCaps,&pDestTexture);
		}
		else
		{
			hrSrc = m_pSrcDevice->CreateTexture(128,128,1,0,*pFormat,dwSrcCaps,&pSrcTexture);
			hrSrc2 = m_pSrcDevice->CreateTexture(128,128,1,0,*pFormat,dwSrc2Caps,&pSrc2Texture);
			hrDest = m_pSrcDevice->CreateTexture(128,128,1,0,*pFormat,dwDestCaps,&pDestTexture);
		}
		
		// Make sure we got a valid texture pointers
		if (FAILED(hrSrc) || (NULL == pSrcTexture))
		{
			WriteToLog("Source CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(hrSrc));
			Fail();
			return false;
		}

		// Set the palette if needed
		if (pSrcTexture->IsPalettized())
			pSrcTexture->SetPalette(NULL);

		// Load the texture with color
		if (nSrc == 1)
		{
			if (FAILED(pSrcTexture->LoadStripes(1,256,256,1,Red,false,false,TEXLOAD_LOCKCOPY)))
			{
				WriteToLog("LoadStripes() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
				Fail();
				return false;
			}
		}
		else
		{
			if (FAILED(pSrcTexture->LoadStripes(1,128,128,1,Green,false,false,TEXLOAD_LOCKCOPY)))
			{
				WriteToLog("LoadStripes() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
				Fail();
				return false;
			}

			// Make sure we got a valid texture pointers
			if (FAILED(hrSrc2) || (NULL == pSrc2Texture))
			{
				WriteToLog("Source2 CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(hrSrc2));
				Fail();
				return false;
			}

			// Set the palette if needed
			if (pSrc2Texture->IsPalettized())
				pSrc2Texture->SetPalette(NULL);

			// Load the texture with color
			if (FAILED(pSrc2Texture->LoadStripes(1,128,128,1,Blue,false,false,TEXLOAD_LOCKCOPY)))
			{
				WriteToLog("LoadStripes() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
				Fail();
				return false;
			}
		}
	}

	// Make sure we got a valid texture pointers
	if (FAILED(hrDest) || (NULL == pDestTexture))
	{
		WriteToLog("Destination CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(hrDest));
        Fail();
		return false;
	}

	// Tell the framework what texture to use
    if (FAILED(m_pSrcDevice->SetTexture(0,pDestTexture)))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		Fail();
		return false;
	}

    // Use Framework UpdateTexture....to get Load()
	// Load from Source surface to Destination surface
	if (FAILED(m_pSrcDevice->UpdateTexture(pSrcTexture, pDestTexture)))
	{
		WriteToLog("Load() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		Fail();
		return false;
	}

	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,6,NULL,0,0);

		if (!bMipMap && (nSize == 128))
		{
			// Use Framework UpdateTexture....to get Load()
			// Load from Source2 surface to Destination surface
		    if (FAILED(m_pSrcDevice->UpdateTexture(pSrc2Texture, pDestTexture)))
			{
				WriteToLog("Second Load() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
				Fail();
				return false;
			}

			RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList+6,6,NULL,0,0);
		}

		EndScene();
	}	

    return true;
}

bool CTexLoadTest::ProcessFrame(void)
{
    HRESULT		hr;
	bool		bResult;
	RECT		rSurf;
	void		*pColor;
    int			nWidth = nSize;
    int			nHeight = nSize;
    int			nTotal = nSize * nSize;
    LOCKED_RECT LockRect;

	// Build the subrect we want
	if (nSize == 128)
	{
		rSurf.top = 10;
		rSurf.left = 10;
		rSurf.bottom = (rSurf.top + nSize) * 2;
		rSurf.right = (rSurf.left + nSize) * 2;

        nWidth = 10 + (nSize * 2);
        nHeight = 10 + (nSize * 2);
	}
	else
	{
		rSurf.top = 10;
		rSurf.left = 10;
		rSurf.bottom = rSurf.top + nSize;
		rSurf.right = rSurf.left + nSize;
	}

	// Get the Surface data pointer for our subrect
    hr = pSrcTarget->LockRect(&LockRect,&rSurf,NULL);

    if (FAILED(hr))
	{
		WriteToLog("ProcessFrame: LockRect() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
        Fail();
        return false;
	}

	// Initialize variables
	int nWhitePixels = 0;
	int nYellowPixels = 0;
	int nMagentaPixels = 0;
	int nRedPixels = 0;
	int nCyanPixels = 0;
	int nGreenPixels = 0;
	int nBluePixels = 0;

	// Look for any colored pixels
	for (int j=0; j < nHeight; j++)		// Height loop
	{
		pColor = (char*)LockRect.pBits + (LockRect.iPitch * j);

		for (int i=0; i < nWidth; i++)	// Width loop
		{
			DWORD dwColor = pSrcTarget->ReadColor(&pColor);

			DWORD red   = RGBA_GETRED(dwColor);
			DWORD green = RGBA_GETGREEN(dwColor);
			DWORD blue  = RGBA_GETBLUE(dwColor);

			// Count the White pixels in the surface
			if (red && green && blue)
				nWhitePixels++;

			// Count the Yellow pixels in the surface
			if (red && green && !blue)
				nYellowPixels++;

			// Count the Magenta pixels in the surface
			if (red && !green && blue)
				nMagentaPixels++;

			// Count the Red pixels in the surface
			if (red && !green && !blue)
				nRedPixels++;

			// Count the Cyan pixels in the surface
			if (!red && green && blue)
				nCyanPixels++;

			// Count the Green pixels in the surface
			if (!red && green && !blue)
				nGreenPixels++;

			// Count the Blue pixels in the surface
			if (!red && !green && blue)
				nBluePixels++;
		}
	}

    hr = pSrcTarget->UnlockRect();

    if (FAILED(hr))
	{
		WriteToLog("ProcessFrame: UnlockRect() failed with HResult = %s.\n",m_pD3D->HResultToString(hr));
        Fail();
        return false;
	}

	// Tell the logfile how the compare went
	switch (nSize)
	{
		case 1:{
				if (nYellowPixels != nTotal)
				{
					WriteToLog("Found %d Yellow pixels, expected %d.\n", nYellowPixels, nTotal);
					bResult = false;
				}
				else
					bResult = true;
                }
			break;
		case 2:{
				if (nWhitePixels != nTotal)
				{
					WriteToLog("Found %d White pixels, expected %d.\n", nWhitePixels, nTotal);
					bResult = false;
				}
				else
					bResult = true;
                }
			break;
		case 3:
		case 4:{
				if (nBluePixels != nTotal)
				{
					WriteToLog("Found %d Blue pixels, expected %d.\n", nBluePixels, nTotal);
					bResult = false;
				}
				else
					bResult = true;
                }
			break;
		case 6:
		case 7:{
				if (nGreenPixels != nTotal)
				{
					WriteToLog("Found %d Green pixels, expected %d.\n", nGreenPixels, nTotal);
					bResult = false;
				}
				else
					bResult = true;
                }
			break;
		case 12:
		case 14:{
				if (nCyanPixels != nTotal)
				{
					WriteToLog("Found %d Cyan pixels, expected %d.\n", nCyanPixels, nTotal);
					bResult = false;
				}
				else
					bResult = true;
                }
			break;
		case 24:
		case 30:
		case 256:{
				if (nRedPixels != nTotal)
				{
					WriteToLog("Found %d Red pixels, expected %d.\n", nRedPixels, nTotal);
					bResult = false;
				}
				else
					bResult = true;
                }
			break;
		case 48:
		case 56:
		case 96:
		case 104:
		case 120:
		case 192:
		case 216:
		case 248:{
				if (nMagentaPixels != nTotal)
				{
					WriteToLog("Found %d Magenta pixels, expected %d.\n", nMagentaPixels, nTotal);
					bResult = false;
				}
				else
					bResult = true;
			    }
			break;
		case 128:{
                if ((nGreenPixels != nTotal) || (nBluePixels != nTotal))
                {
				    if (nGreenPixels != nTotal)
    					WriteToLog("Found %d Green pixels, expected %d.\n", nGreenPixels, nTotal);

                    if (nBluePixels != nTotal)
    					WriteToLog("Found %d Blue pixels, expected %d.\n", nBluePixels, nTotal);

					bResult = false;
				}
				else
					bResult = true;
                }
			break;
        default:{
					WriteToLog("Invalid size (%d) found.\n", nSize);
					bResult = false;
                }
            break;
                
	};

	if (bResult)
	{
		m_fPassPercentage = 1.0;
		Pass();
	}
	else
	{
		m_fPassPercentage = 0.0f;
		Fail();
	}

	return bResult;
}

void CTexLoadTest::UpdateStatus(void)
{
    TCHAR szBuffer[80];

    CD3DTest::UpdateStatus();

	// Tell the logfile how they did
	sprintf(szBuffer,"$yPass: %d, Fail: %d", GetPass(), GetFail());
	
	WriteStatus("$gOverall Results",szBuffer);
}

bool CTexLoadTest::TestTerminate(void)
{
	// Cleanup texture & image data
	SetTexture(0, NULL);

    RELEASE(pSrcTexture);
    RELEASE(pSrc2Texture);
    RELEASE(pDestTexture);

    // Cleanup image data for mips
	for (int i=0; i < 9; i++)
	    SAFEDELETE(pMipImage[i]);

	// Cleanup buffers that we created
	RELEASE(pSrcTarget);

	return true;
}
