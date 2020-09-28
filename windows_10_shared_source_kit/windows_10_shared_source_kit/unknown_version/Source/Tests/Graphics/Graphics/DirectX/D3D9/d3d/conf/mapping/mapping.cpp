#define D3D_OVERLOADS
#include <d3d.h>
#include "Mapping.h"

// Global Test and App instance
CD3DWindowFramework	App;
CMapPointTest		MapPoint;
CMapLinearTest		MapLinear;

CMappingTest::CMappingTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
    m_Options.D3DOptions.nBackBufferWidth = (DWORD) 160;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD) 140;
    
    // Init texture variable
    pTexture = NULL;

    // Initialize the UV array's values
	float fZero = 0.0f;
	float fOne = 1.0f - 0.001f;

	TopLeft[0] = UV(fZero, fZero);
	TopLeft[1] = UV(fZero, fOne);
	TopLeft[2] = UV(fOne,  fOne);
	TopLeft[3] = UV(fOne,  fZero);

	TopRight[0] = UV(fOne,  fZero);
	TopRight[1] = UV(fZero, fZero);
	TopRight[2] = UV(fZero, fOne);
	TopRight[3] = UV(fOne,  fOne);

	BottomLeft[0] = UV(fZero, fOne);
	BottomLeft[1] = UV(fOne,  fOne);
	BottomLeft[2] = UV(fOne,  fZero);
	BottomLeft[3] = UV(fZero, fZero);

	BottomRight[0] = UV(fOne,  fOne);
	BottomRight[1] = UV(fOne,  fZero);
	BottomRight[2] = UV(fZero, fZero);
	BottomRight[3] = UV(fZero, fOne);
}

CMappingTest::~CMappingTest()
{
}
 
UINT CMappingTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#ifdef FAST_TEST
	m_uCommonTextureFormats = 1;
#endif

    // Setup the Test range
	SetTestRange(1, MAX_TESTS);

	// Check the caps we are interested in
	DWORD dwTextureFilterCaps = m_pSrcDevice->GetCaps()->dwTextureFilterCaps;

	// Check the appropriate Min Filter cap
	if (!(dwTextureFilterCaps & dwMinFilterCap))
	{
		WriteToLog("Device capability not supported: (Min) Texture Filter %s.\n",m_szCommandKey);
		return D3DTESTINIT_SKIPALL;
	}

	// Check the appropriate Mag Filter cap
	if (!(dwTextureFilterCaps & dwMagFilterCap))
	{
		WriteToLog("Device capability not supported: (Mag) Texture Filter %s.\n",m_szCommandKey);
		return D3DTESTINIT_SKIPALL;
	}

	// Setup the colors for the Gradient array
	bool bResult;
	D3DCOLOR Gradient[4];

	Gradient[0] = RGBA_MAKE(0,0,0,255);		// Black
	Gradient[1] = RGBA_MAKE(255,0,0,255);	// Red
	Gradient[2] = RGBA_MAKE(0,255,0,255);	// Green
	Gradient[3] = RGBA_MAKE(255,255,0,255);	// Yellow

    // Create the texture
    bResult = CreateTexture(64,64,1,0,m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],FMWK_DEFAULT,&pTexture);

	// Indicate a CreateTexture failure
	if (!bResult || (NULL == pTexture))
	{
		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Set the palette if needed
	if (pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

    // Load a gradient image into the texture
    if (!pTexture->LoadGradient(1,64,64,Gradient))
 	{
		WriteToLog("LoadGradient() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
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

bool CMappingTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CMappingTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
    TCHAR szBuffer[80];
 	dwVertexCount = 0;
	szStatus[0] = '\0';
	msgString[0] = '\0';

	// Let's build some triangles.
	DrawGrid(uTestNum, 1); 

    // Tell log what format and load method we are using
    FmtToString(FORMAT_TEXTURE,&m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],szBuffer);
  	sprintf(msgString, "%sTex: %s",msgString,szBuffer);

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

void CMappingTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

void CMappingTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

bool CMappingTest::TestTerminate()
{
	// Cleanup texture & image data
	SetTexture(0, NULL);
    RELEASE(pTexture);

	return true;
}
