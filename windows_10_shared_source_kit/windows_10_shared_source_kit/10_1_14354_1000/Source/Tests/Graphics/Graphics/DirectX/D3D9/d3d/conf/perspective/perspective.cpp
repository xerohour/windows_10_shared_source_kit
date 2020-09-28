#define D3D_OVERLOADS
#include <d3d.h>
#include "Perspective.h"

// Global Test and App instance
CD3DWindowFramework	App;
CTextureTest	    Texture;
CColorTest	        Color;

CPerspectiveTest::CPerspectiveTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);

    // Init texture variable
    pTexture = NULL;

#if FAST_TEST
    // Add some more space to fit all test cases in one back buffer
    m_Options.D3DOptions.nBackBufferWidth = (DWORD)1024;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD)1024;
    // Set compare function to require 98.5%
    m_pFramework->GetImageCmp()->SetPassRequired(0.985f);
#endif
}

CPerspectiveTest::~CPerspectiveTest()
{
}

bool CPerspectiveTest::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
	return true;
}

bool CPerspectiveTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

#if FAST_TEST
bool CPerspectiveTest::ExecuteTest(UINT uTestNum)
{
    // Tell the log that we are starting
    BeginTestCase("Perspective Test", uTestNum);

    auto& drawLamda = [=](UINT testIndex) -> bool
    {
        // Initialize some variables
        dwVertexCount = 0;
        msgString[0] = '\0';
        szStatus[0] = '\0';

        if (pTexture)
        {
            TCHAR szBuffer[80];

            // Let's build some triangles.
            if (testIndex <= MAX_GRID_TESTS * 4)
            {
                // Build the texturing grid
                DrawTexGrid(testIndex);
            }
            else
            {
                // Build the triangle
                DrawTriangle(testIndex - (MAX_GRID_TESTS * 4));
            }

            // Indicate which load option was used
            switch (m_pSrcDevice->GetTexLoad())
            {
            case TEXLOAD_UPDATETEXTURE:
            case TEXLOAD_COPYRECTS:
            case TEXLOAD_LOCKCOPY:
            case TEXLOAD_UPDATESURFACE:
                break;
            default:
                Fail();
                return false;
            }
        }
        else
        {
            // Let's build some triangles.
            if (testIndex <= MAX_GRID_TESTS)
            {
                // Build the color grid
                DrawColorGrid(testIndex);
            }
            else
            {
                // Build the triangle
                DrawTriangle(testIndex - MAX_GRID_TESTS);
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

                float scaleX = (widthPerOp / float(dwWidth)) + 0.25f;
                float scaleY = (widthPerOp / float(dwHeight)) + 0.25f;

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
            RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, VertexList, dwVertexCount, NULL, 0, 0);

            EndScene();
        }
    }

    return pass;
}
#else//FAST_TEST
bool CPerspectiveTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
    dwVertexCount = 0;
	msgString[0] = '\0';
	szStatus[0] = '\0';

    if (pTexture)
    {
        TCHAR szBuffer[80];

	    // Let's build some triangles.
	    if (uTestNum <= MAX_GRID_TESTS * 4)
	    {
		    // Build the texturing grid
		    DrawTexGrid(uTestNum); 
	    }
	    else
	    {
		    // Build the triangle
		    DrawTriangle(uTestNum - (MAX_GRID_TESTS * 4)); 
	    }

        // Tell log what format and load method we are using
        FmtToString(FORMAT_TEXTURE,&m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],szBuffer);
  	    sprintf(msgString, "%sTex: %s",msgString,szBuffer);

		// Tell the log that we are starting
		BeginTestCase(msgString, uTestNum);

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
	}
    else
    {
	    // Let's build some triangles.
	    if (uTestNum <= MAX_GRID_TESTS)
	    {
		    // Build the color grid
		    DrawColorGrid(uTestNum); 
	    }
	    else
	    {
		    // Build the triangle
		    DrawTriangle(uTestNum - MAX_GRID_TESTS); 
	    }
	    
		// Tell the log that we are starting
		BeginTestCase(msgString, uTestNum);
    }

    return true;
}
#endif

void CPerspectiveTest::SceneRefresh()
{
#ifndef FAST_TEST
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
#endif
}

void CPerspectiveTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
    if (pTexture)
		WriteStatus("$wTexture Load",szStatus);
}

bool CPerspectiveTest::TestTerminate()
{
	// Cleanup texture
    if (pTexture)
    {
	    SetTexture(0, NULL);
        RELEASE(pTexture);
    }

	return true;
}
