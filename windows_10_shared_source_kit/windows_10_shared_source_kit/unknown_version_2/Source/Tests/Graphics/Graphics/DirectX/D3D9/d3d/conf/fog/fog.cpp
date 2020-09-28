#define D3D_OVERLOADS
#include <d3d.h>
#include "Fog.h"

// Global Test and App instance
CD3DWindowFramework		App;

// Non Texture test cases
CVertexTest				Vertex;
CTableLinearTest		TableLinear;
CTableExpTest			TableExp;
CTableExp2Test			TableExp2;
CWLinearTest			WLinear;
CWExpTest				WExp;
CWExp2Test				WExp2;

// Texture test cases
CTextureVertexTest		TextureVertex;
CTextureTableLinearTest	TextureTableLinear;
CTextureTableExpTest	TextureTableExp;
CTextureTableExp2Test	TextureTableExp2;
CTextureWLinearTest		TextureWLinear;
CTextureWExpTest		TextureWExp;
CTextureWExp2Test		TextureWExp2;

CFogTest::CFogTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);

    // Init variables
    pTexture = NULL;
	bCapAbort = false;

#if FAST_TEST
    // Add some more space to fit all test cases in one back buffer
    m_Options.D3DOptions.nBackBufferWidth = (DWORD)512;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD)512;
    // Set compare function to require 98.5%
    m_pFramework->GetImageCmp()->SetPassRequired(0.985f);
#endif
}
 
CFogTest::~CFogTest()
{
}

bool CFogTest::CapsCheck(void)
{
	// Check the caps we are interested in
	DWORD dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;

	// Check the appropriate Fog caps according to fog type
	switch (dwFog)
	{
		case VERTEX:
			if (!(dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX))
			{
				WriteToLog("Device capability not supported: Fog Vertex.\n");
				bCapAbort = true;
				return true;
			}
			break;

		case TABLE_LINEAR:
		case TABLE_EXP:
		case TABLE_EXP2:
			if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
			{
				if (m_dwVersion >= 0x0700)
				{
					if (dwRasterCaps & D3DPRASTERCAPS_ZFOG)
					{
	            	    WriteToLog("Device capability inconsistency: Z Fog is set without Fog Table.\n");
						bCapAbort = true;
						return true;
					}

					WriteToLog("Device capability not supported: Z Fog.\n");
					return false;
				}

				WriteToLog("Device capability not supported: Fog Table.\n");
				return false;
			}

			if (m_dwVersion >= 0x0700)
			{
				if (!(dwRasterCaps & D3DPRASTERCAPS_ZFOG))
				{
					if (!(dwRasterCaps & D3DPRASTERCAPS_WFOG))
					{
	            	    WriteToLog("Device capability inconsistency: Fog Table is set without Z Fog or W Fog.\n");
						bCapAbort = true;
						return true;
					}

					WriteToLog("Device capability not supported: Z Fog.\n");
					return false;
				}
			}
			break;

		case W_LINEAR:
		case W_EXP:
		case W_EXP2:
			if (!(dwRasterCaps & D3DPRASTERCAPS_WFOG))
			{
				WriteToLog("Device capability not supported: W Fog.\n");
				return false;
			}

			if (m_dwVersion >= 0x0700)
			{
				if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
				{
            	    WriteToLog("Device capability inconsistency: W Fog is set without Fog Table.\n");
					bCapAbort = true;
					return true;
				}
			}
			break;
	}

	return true;
}

bool CFogTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

#if FAST_TEST
bool CFogTest::ExecuteTest(UINT uTestNum)
{
    // Tell the log that we are starting
    BeginTestCase("Fog Test", uTestNum);

    auto& drawLamda = [=](UINT testIndex) -> bool
    {
	    // Initialize some variables
        int nTest = testIndex;
	    TCHAR szBuffer[80];
	    dwVertexCount = 0;
	    msgString[0] = '\0';
	    szStatus[0] = '\0';

        if (m_Options.ModeOptions.dwTexInclude)
        {
            // Reset test number based on texture formats
            nTest = ((testIndex - 1) / m_uCommonTextureFormats) + 1;

		    // Set the texture blend mode
		    SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD) D3DTBLEND_DECAL);

            // Cleanup previous texture
	        SetTexture(0, NULL);
	        RELEASE(pTexture);
        }

        // Let's build some triangles.
	    switch (dwFog)
	    {
		    case VERTEX:
                if (m_Options.ModeOptions.dwTexInclude)
			        TextureVertexTests(nTest);
                else
                    VertexTests(nTest);
			    break;
		    case TABLE_LINEAR:
			    {
   				    float	fStart, fEnd;

                    // Build the Table Fog triangles
				    TableTests();

                    if (m_Options.ModeOptions.dwTexInclude)
                    {
                        fStart = (float) (1 - (nTest / (TEXTURE_TABLE_TESTS - 1)));
                        fEnd = (float) (1 - (nTest / TEXTURE_TABLE_TESTS));
                    }
                    else
                    {
       				    int nDepth = (TABLE_LINEAR_TESTS / 2) - nTest;

				        if (nDepth >= 0)
				        {
					        fStart = 0.0f;
					        fEnd = fStart + (nDepth / 10.0f);
				        }
				        else
				        {
					        fEnd = 1.0f;
					        fStart = fEnd - ((nDepth + 11) / 10.0f);
				        }
                    }

				    // Update the logfile
				    sprintf(msgString, "%sFog: Blue, Start: %.2f, End: %.2f",msgString, fStart, fEnd);

				    SetRenderState(D3DRENDERSTATE_FOGTABLESTART, *((DWORD*)(&fStart)));
				    SetRenderState(D3DRENDERSTATE_FOGTABLEEND, *((DWORD*)(&fEnd)));
			    }
			    break;
		    case TABLE_EXP:
		    case TABLE_EXP2:
			    {
                    float fDensity;

				    if (m_Options.ModeOptions.dwTexInclude)
                        fDensity = 1 - (TEXTURE_TABLE_TESTS - nTest) / 2.0f;
                    else
                        fDensity = (TABLE_EXP_TESTS - nTest) / 10.0f;

				    // Build the Table Fog triangles
				    TableTests();

				    // Update the logfile
				    sprintf(msgString, "%sFog: Blue, Density: %.2f",msgString, fDensity);

				    SetRenderState(D3DRENDERSTATE_FOGTABLEDENSITY, *((DWORD*)(&fDensity)));
			    }
			    break;
		    case W_LINEAR:
			    {
				    float	fStart, fEnd;

				    // Build the W Fog triangles
				    WTests();

                    if (m_Options.ModeOptions.dwTexInclude)
                    {
                        if (nTest == 1)
                            fStart = fEnd = 10.0f;
                        else if (nTest == 3)
                            fStart = fEnd = 1.0f;
                        else
                        {
                            fStart = 1.0f;
                            fEnd = 5.0f;
                        }
                    }
                    else
                    {
   				        int	nDepth = (W_LINEAR_TESTS / 2) - nTest;

				        if ((nTest / 20) == 0)
				        {
					        fStart = 1.0f;
					        fEnd = fStart + ((nTest-1) * 0.5f);
				        }
				        else
				        {
					        fEnd = 10.0f;
					        fStart = fEnd - ((nTest-20) * 0.5f);
				        }
                    }

				    // Update the logfile
				    sprintf(msgString, "%sFog: Red, Start: %.2f, End: %.2f",msgString, fStart, fEnd);

				    SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE);
				    SetRenderState(D3DRENDERSTATE_FOGTABLESTART, *((DWORD*)(&fStart)));
				    SetRenderState(D3DRENDERSTATE_FOGTABLEEND, *((DWORD*)(&fEnd)));
			    }
			    break;
		    case W_EXP:
		    case W_EXP2:
			    {
                    float fDensity;

				    if (m_Options.ModeOptions.dwTexInclude)
                        fDensity = 1 - (TEXTURE_W_TESTS - nTest) / 2.0f;
                    else
                        fDensity = (W_EXP_TESTS - nTest) / 10.0f;

				    // Build the W Fog triangles
				    WTests();

				    // Update the logfile
				    sprintf(msgString, "%sFog: Red, Density: %.2f",msgString, fDensity);

				    SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE);
				    SetRenderState(D3DRENDERSTATE_FOGTABLEDENSITY, *((DWORD*)(&fDensity)));
			    }
			    break;
	    }

	    // Set the fog color
	    SetRenderState(D3DRENDERSTATE_FOGCOLOR, FogColor);

        if (m_Options.ModeOptions.dwTexInclude)
        {
		    bool bResult;
            FORMAT *pFormat;

	        // Cycle the common texture formats
            m_pCurrentMode->nTextureFormat = (testIndex - 1) % m_uCommonTextureFormats;

            pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

            // Create the texture
            bResult = CreateTexture(64,64,1,0,*pFormat,FMWK_DEFAULT,&pTexture);

	        // Tell log about CreateTexture failure
	        if (!bResult || (NULL == pTexture))
	        {
		        WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
                Fail();
		        return false;
	        }

	        // Set the palette if needed
	        if (pTexture->IsPalettized())
		        pTexture->SetPalette(NULL);

            // Load a stripe image into the texture
            if (!pTexture->LoadStripes(1,64,64,8,Stripes,false))
 	        {
		        WriteToLog("LoadStripes() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
                Fail();
		        return false;
	        }

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

            // Set the valid texture
		    if (!SetTexture(0,pTexture))
		    {
			    WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			    Fail();
			    return false;
		    }
        }

        return true;
    };

    bool pass = true;

    const DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
    const DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
    UINT numColumns = (UINT)ceil(sqrt((float)m_totalNumTestCases));
    UINT numRows = (UINT)ceil(sqrt((float)m_totalNumTestCases));;
    const float widthPerOp = float(dwWidth) / float(numColumns);
    const float heightPerOp = float(dwHeight) / float(numRows);

    for (size_t i = 0; i < m_totalNumTestCases; i++)
    {
        pass = drawLamda(i + 1);//Plus 1 because tests are 1 indexed
        if (pass == false)
        {
            break;
        }

        if (BeginScene())
        {
            // Transform and scale the vertices so they all fit on the screen
            {
                const DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
                const DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
                UINT numColumns = (UINT)ceil(sqrt((float)m_totalNumTestCases));
                UINT numRows = numColumns;
                const float widthPerOp = float(dwWidth) / float(numColumns);
                const float heightPerOp = float(dwHeight) / float(numRows);

                float scaleX = (widthPerOp / float(dwWidth));
                float scaleY = (widthPerOp / float(dwHeight));

                float transX = (i % numColumns) * widthPerOp;
                float transY = (i / numColumns) * heightPerOp;
                for (size_t i = 0; i < dwVertexCount; i++)
                {
                    VertexList[i].sx *= scaleX;
                    VertexList[i].sx += transX;

                    VertexList[i].sy *= scaleY;
                    VertexList[i].sy += transY;
                }
            }

            // Render the data.
            if (dwFog == VERTEX)
            {
                RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, VertexList, dwVertexCount, NULL, 0, 0);
            }
            else
            {
                RenderPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, VertexList, 8, NULL, 0, 0);
                RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, VertexList + 8, dwVertexCount - 8, NULL, 0, 0);
            }

            EndScene();
        }
    }

    return pass;
}
#else //FAST_TEST
bool CFogTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
    int nTest = uTestNum;
	TCHAR szBuffer[80];
	dwVertexCount = 0;
	msgString[0] = '\0';
	szStatus[0] = '\0';

    if (m_Options.ModeOptions.dwTexInclude)
    {
        // Reset test number based on texture formats
 	    nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;

		// Set the texture blend mode
		SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD) D3DTBLEND_DECAL);

        // Cleanup previous texture
	    SetTexture(0, NULL);
	    RELEASE(pTexture);
    }

    // Let's build some triangles.
	switch (dwFog)
	{
		case VERTEX:
            if (m_Options.ModeOptions.dwTexInclude)
			    TextureVertexTests(nTest);
            else
                VertexTests(nTest);
			break;
		case TABLE_LINEAR:
			{
   				float	fStart, fEnd;

                // Build the Table Fog triangles
				TableTests();

                if (m_Options.ModeOptions.dwTexInclude)
                {
                    fStart = (float) (1 - (nTest / (TEXTURE_TABLE_TESTS - 1)));
                    fEnd = (float) (1 - (nTest / TEXTURE_TABLE_TESTS));
                }
                else
                {
       				int nDepth = (TABLE_LINEAR_TESTS / 2) - nTest;

				    if (nDepth >= 0)
				    {
					    fStart = 0.0f;
					    fEnd = fStart + (nDepth / 10.0f);
				    }
				    else
				    {
					    fEnd = 1.0f;
					    fStart = fEnd - ((nDepth + 11) / 10.0f);
				    }
                }

				// Update the logfile
				sprintf(msgString, "%sFog: Blue, Start: %.2f, End: %.2f",msgString, fStart, fEnd);

				SetRenderState(D3DRENDERSTATE_FOGTABLESTART, *((DWORD*)(&fStart)));
				SetRenderState(D3DRENDERSTATE_FOGTABLEEND, *((DWORD*)(&fEnd)));
			}
			break;
		case TABLE_EXP:
		case TABLE_EXP2:
			{
                float fDensity;

				if (m_Options.ModeOptions.dwTexInclude)
                    fDensity = 1 - (TEXTURE_TABLE_TESTS - nTest) / 2.0f;
                else
                    fDensity = (TABLE_EXP_TESTS - nTest) / 10.0f;

				// Build the Table Fog triangles
				TableTests();

				// Update the logfile
				sprintf(msgString, "%sFog: Blue, Density: %.2f",msgString, fDensity);

				SetRenderState(D3DRENDERSTATE_FOGTABLEDENSITY, *((DWORD*)(&fDensity)));
			}
			break;
		case W_LINEAR:
			{
				float	fStart, fEnd;

				// Build the W Fog triangles
				WTests();

                if (m_Options.ModeOptions.dwTexInclude)
                {
                    if (nTest == 1)
                        fStart = fEnd = 10.0f;
                    else if (nTest == 3)
                        fStart = fEnd = 1.0f;
                    else
                    {
                        fStart = 1.0f;
                        fEnd = 5.0f;
                    }
                }
                else
                {
   				    int	nDepth = (W_LINEAR_TESTS / 2) - nTest;

				    if ((nTest / 20) == 0)
				    {
					    fStart = 1.0f;
					    fEnd = fStart + ((nTest-1) * 0.5f);
				    }
				    else
				    {
					    fEnd = 10.0f;
					    fStart = fEnd - ((nTest-20) * 0.5f);
				    }
                }

				// Update the logfile
				sprintf(msgString, "%sFog: Red, Start: %.2f, End: %.2f",msgString, fStart, fEnd);

				SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE);
				SetRenderState(D3DRENDERSTATE_FOGTABLESTART, *((DWORD*)(&fStart)));
				SetRenderState(D3DRENDERSTATE_FOGTABLEEND, *((DWORD*)(&fEnd)));
			}
			break;
		case W_EXP:
		case W_EXP2:
			{
                float fDensity;

				if (m_Options.ModeOptions.dwTexInclude)
                    fDensity = 1 - (TEXTURE_W_TESTS - nTest) / 2.0f;
                else
                    fDensity = (W_EXP_TESTS - nTest) / 10.0f;

				// Build the W Fog triangles
				WTests();

				// Update the logfile
				sprintf(msgString, "%sFog: Red, Density: %.2f",msgString, fDensity);

				SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE);
				SetRenderState(D3DRENDERSTATE_FOGTABLEDENSITY, *((DWORD*)(&fDensity)));
			}
			break;
	}

	// Set the fog color
	SetRenderState(D3DRENDERSTATE_FOGCOLOR, FogColor);

    if (m_Options.ModeOptions.dwTexInclude)
    {
		bool bResult;
        FORMAT *pFormat;

	    // Cycle the common texture formats
	    m_pCurrentMode->nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;

        pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

        // Tell log what format we are using
        FmtToString(FORMAT_TEXTURE,pFormat,szBuffer);
        sprintf(msgString, "%s, Tex: %s",msgString,szBuffer);

	    // Tell the log that we are starting
	    BeginTestCase(msgString,uTestNum);

        // Create the texture
        bResult = CreateTexture(64,64,1,0,*pFormat,FMWK_DEFAULT,&pTexture);

	    // Tell log about CreateTexture failure
	    if (!bResult || (NULL == pTexture))
	    {
		    WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
            Fail();
		    return false;
	    }

	    // Set the palette if needed
	    if (pTexture->IsPalettized())
		    pTexture->SetPalette(NULL);

        // Load a stripe image into the texture
        if (!pTexture->LoadStripes(1,64,64,8,Stripes,false))
 	    {
		    WriteToLog("LoadStripes() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
            Fail();
		    return false;
	    }

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

        // Set the valid texture
		if (!SetTexture(0,pTexture))
		{
			WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
    }
    else
    {
	    // Tell the log that we are starting
	    BeginTestCase(msgString,uTestNum);
    }

    return true;
}
#endif

void CFogTest::SceneRefresh()
{
#ifndef FAST_TEST
	if (BeginScene())
	{
		// Render the data.
		if (dwFog == VERTEX)
		{
			RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);
		}
		else
		{
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,VertexList,8,NULL,0,0);
			RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList+8,dwVertexCount-8,NULL,0,0);
		}

		EndScene();
	}	
#endif
}

void CFogTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
    if (m_Options.ModeOptions.dwTexInclude)
		WriteStatus("$wTexture Load",szStatus);
}

bool CFogTest::TestTerminate(void)
{
	// Cleanup texture
	SetTexture(0, NULL);
    RELEASE(pTexture);

	return true;
}
