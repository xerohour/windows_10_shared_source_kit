#define D3D_OVERLOADS
#include <d3d.h>
#include "SubPixel.h"

// Global Test and App instance
CD3DWindowFramework	App;
CSubPixelTest		SubPixel;

CSubPixelTest::CSubPixelTest()
{
    m_szTestName = "Texture SubPixel";
	m_szCommandKey = "SubPixel";

    // Init Framework options
    SetTestType(TESTTYPE_CONF);

    // Init texture variable
    pTexture = NULL;
}

CSubPixelTest::~CSubPixelTest()
{
}

bool CSubPixelTest::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE);
	return true;
}

UINT CSubPixelTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1, MAX_TESTS);

    if (m_dwVersion <= 0x0700)
    {
	    // Check the caps we are interested in
	    DWORD dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;

	    // Check the SubPixel caps
	    if (!(dwRasterCaps & D3DPRASTERCAPS_SUBPIXEL) &&
		    !(dwRasterCaps & D3DPRASTERCAPS_SUBPIXELX))
	    {
		    WriteToLog("Device capability not supported: Raster Subpixel.\n");
		    return D3DTESTINIT_ABORT;
	    }
    }

	// Setup the colors for the Stripes array
	bool bResult;
	D3DCOLOR Stripes[8];

	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

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

    // Load a stripe image into the texture
    if (!pTexture->LoadStripes(1,64,64,8,Stripes,false))
	{
		WriteToLog("LoadStripes() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
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

bool CSubPixelTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,0,255), 1.0f, 0);
}

bool CSubPixelTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = ((uTestNum-1) / 4) + 1; 
	int nGroup = (nTest-1) / 10;
	int nWDiv = (nTest - (nGroup*10)) + 1;
	int nHDiv = nGroup + 2;
    TCHAR szBuffer[80];
 	dwVertexCount = 0;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Let's build some triangles.
	DrawGrid(uTestNum-1, nWDiv, nHDiv, 1); 

    // Tell log what format and load method we are using
    FmtToString(FORMAT_TEXTURE,&m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],szBuffer);
  	sprintf(msgString, "%sTex:%s",msgString,szBuffer);

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

void CSubPixelTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

void CSubPixelTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

bool CSubPixelTest::TestTerminate()
{
	// Cleanup texture 
	SetTexture(0, NULL);
    RELEASE(pTexture);

	return true;
}
