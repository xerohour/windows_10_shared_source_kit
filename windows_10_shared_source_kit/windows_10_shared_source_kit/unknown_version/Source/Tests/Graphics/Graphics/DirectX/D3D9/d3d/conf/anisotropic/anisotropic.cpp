#define D3D_OVERLOADS
#include <d3d.h>
#include "Anisotropic.h"

// Global Test and App instance
CD3DWindowFramework		App;
//CPointAnisoTest			PointAniso;
CLinearAnisoTest		LinearAniso;
//CAnisoPointTest			AnisoPoint;
CAnisoLinearTest		AnisoLinear;
CAnisoAnisoTest			AnisoAniso;

CAnisoTest::CAnisoTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);

	// Setup image compare code to be like the old style
	m_pFramework->GetImageCmp()->SetPassRequired(.80f);
	m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.28f);
	m_pFramework->GetImageCmp()->SetTolPerPixel(1.0f);

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_7;

	// Init texture variable
    pTexture = NULL;

    // Initialize the Size array
	SizeArray[0] = 248.0f;
	SizeArray[1] =  48.0f;
	SizeArray[2] =  64.0f; 
	SizeArray[3] =  96.0f; 
	SizeArray[4] = 168.0f; 
	SizeArray[5] = 208.0f; 
}

CAnisoTest::~CAnisoTest()
{
}

UINT CAnisoTest::TestInitialize(void)
{
	bool bResult;
	int nMaxAnisotropy = m_pSrcDevice->GetCaps()->dwMaxAnisotropy;
	DWORD dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;
	DWORD dwTextureFilterCaps = m_pSrcDevice->GetCaps()->dwTextureFilterCaps;

	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

	// Make sure we get a valid nMaxAnisotropy value
	if (nMaxAnisotropy == 0)
	{
		WriteToLog("MaxAnisotropy value is 0, must be at least 1 (even if anisotropic is not supported).\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Setup the Test range
	SetTestRange(1, MAX_TESTS * MAX_GRIDS * nMaxAnisotropy);

    // Check the appropriate Raster cap
    if (!(dwRasterCaps & D3DPRASTERCAPS_ANISOTROPY))
    {
        if (dwTextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC)
        {
	        WriteToLog("Device capability inconsistency (MinFilter): Raster Anisotropy is not set.\n");
	        return D3DTESTINIT_ABORT;
        }

        if (dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC)
        {
	        WriteToLog("Device capability inconsistency (MagFilter): Raster Anisotropy is not set.\n");
	        return D3DTESTINIT_ABORT;
        }

        WriteToLog("Device capability not supported: Raster Anisotropy.\n");
		return D3DTESTINIT_SKIPALL;
    }

    // Check the appropriate Min Filter cap
    if (!(dwTextureFilterCaps & dwMinFilterCap))
    {
        switch (dwMinFilterCap)
        {
            case D3DPTFILTERCAPS_MINFPOINT: 
		        WriteToLog("Device capability not supported: Texture Filter MinFPoint.\n");
		        return D3DTESTINIT_ABORT;

            case D3DPTFILTERCAPS_MINFLINEAR: 
		        WriteToLog("Device capability not supported: Texture Filter MinFLinear.\n");
		        return D3DTESTINIT_SKIPALL;

            case D3DPTFILTERCAPS_MINFANISOTROPIC: 
                if (!(dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC))
                {
            	    WriteToLog("Device capability inconsistency: Raster Anisotropy is set without Anisotropic filter.\n");
		            return D3DTESTINIT_ABORT;
                }

		        WriteToLog("Device capability not supported: Texture Filter MinFAnisotropic.\n");
		        return D3DTESTINIT_SKIPALL;
        }
    }

	// Check the appropriate Mag Filter cap
	if (!(dwTextureFilterCaps & dwMagFilterCap))
	{
        switch (dwMagFilterCap)
        {
            case D3DPTFILTERCAPS_MAGFPOINT: 
		        WriteToLog("Device capability not supported: Texture Filter MagFPoint.\n");
		        return D3DTESTINIT_ABORT;

            case D3DPTFILTERCAPS_MAGFLINEAR: 
		        WriteToLog("Device capability not supported: Texture Filter MagFLinear.\n");
		        return D3DTESTINIT_SKIPALL;

            case D3DPTFILTERCAPS_MAGFANISOTROPIC: 
                if (!(dwTextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC))
                {
            	    WriteToLog("Device capability inconsistency: Raster Anisotropy is set without Anisotropic filter.\n");
		            return D3DTESTINIT_ABORT;
                }

                WriteToLog("Device capability not supported: Texture Filter MagFAnisotropic.\n");
		        return D3DTESTINIT_SKIPALL;
        }
	}

    // Check the Max Anisotropy setting
    if (nMaxAnisotropy < 2)
    {
	    WriteToLog("Device capability inconsistency: MaxAnisotropy value is %d, must be at least 2.\n",nMaxAnisotropy);
	    return D3DTESTINIT_ABORT;
    }

    // Create the texture
    bResult = CreateTexture(128,128,1,0,m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],FMWK_DEFAULT,&pTexture);

	// Indicate a CreateTexture failure
	if (!bResult || (NULL == pTexture))
	{
		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Set the palette if needed
	if (pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

    // Load the bitmap into the texture
    if (!pTexture->LoadTexture(1,"hifreq.bmp"))
	{
		WriteToLog("LoadTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

    // Set the valid texture
	if (!SetTexture(0,pTexture))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}
                          
	return D3DTESTINIT_RUN;
}

bool CAnisoTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CAnisoTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	float fSize = SizeArray[uTestNum % MAX_TESTS];
    int nGrid = ((uTestNum-1) / MAX_TESTS) + 1;
    int nMax = ((uTestNum-1) / (MAX_TESTS*MAX_GRIDS)) + 1;
    TCHAR szBuffer[80];
    DWORD dwPasses;
 	dwVertexCount = 0;
	msgString[0] = '\0';
  	szStatus[0]  = '\0';

	// Let's build some triangles.
	DrawGrid(fSize, nGrid % MAX_GRIDS);

    // Setup the render states according to test case
	SetTextureStageState(0,D3DTSS_MINFILTER, dwMinFilter);
	SetTextureStageState(0,D3DTSS_MAGFILTER, dwMagFilter);
    SetTextureStageState(0,D3DTSS_MAXANISOTROPY, (DWORD)nMax);

    // Check with ValidateDevice to make sure we are fine
	if (!ValidateDevice(&dwPasses))
	{
        HRESULT hResult = GetLastError();
        
        WriteToLog("ValidateDevice() failed with Passes = %d and HResult = %s.\n",dwPasses,m_pD3D->HResultToString(hResult));
        TestFinish(D3DTESTFINISH_SKIPREMAINDER);
		return false;
	}

    // Describe the test for the log
    sprintf(msgString, "%sMaxAniso:%d, Mesh:%dx%d, ",msgString,nMax,(int)fSize,(int)fSize);

    // Tell log what format and load method we are using
    FmtToString(FORMAT_TEXTURE,&m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],szBuffer);
    sprintf(msgString, "%sTex:%s,128x128",msgString,szBuffer);

    // Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

	// Indicate which load option was used
    switch (m_pSrcDevice->GetTexLoad())
    {    
        case TEXLOAD_UPDATETEXTURE:
        	WriteToLog("TexLoad: UpdateTex\n");
        	sprintf(szStatus, "%s$aUpdateTex",szStatus);
            break;
        case TEXLOAD_COPYRECTS:
        	WriteToLog("TexLoad: CopyRects\n");
        	sprintf(szStatus, "%s$aCopyRects",szStatus);
            break;
        case TEXLOAD_LOCKCOPY:
        	WriteToLog("TexLoad: LockCopy\n");
        	sprintf(szStatus, "%s$aLockCopy",szStatus);
            break;
        case TEXLOAD_UPDATESURFACE:
        	WriteToLog("TexLoad: UpdateSurf\n");
        	sprintf(szStatus, "%s$aUpdateSurf",szStatus);
            break;
        default:
		    WriteToLog("Error: Unknown texture load option (%x).\n",m_pSrcDevice->GetTexLoad());
        	sprintf(szStatus, "%s$rUnknown (%x)",szStatus,m_pSrcDevice->GetTexLoad());
            Fail();
		    return false;
    }

    return true;
}

void CAnisoTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

void CAnisoTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

bool CAnisoTest::TestTerminate()
{
	// Cleanup texture
	SetTexture(0, NULL);
    RELEASE(pTexture);

	return true;
}
