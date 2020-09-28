// FILE:        zfight.cpp
// DESC:        zfight class methods for z fighting conformance test
// AUTHOR:      Todd M. Frost
// COMMENTS:    attempts to expose order dependent, mult-pass z fighting

#include "zfight.h"

CD3DWindowFramework App;    // global instance of D3D window framework class
CZFight             ZFight; // global instance of ZFight class

// NAME:        CZFight()
// DESC:        constructor for z fight class
// INPUT:       none
// OUTPUT:      none

CZFight::CZFight(VOID)
{
    SetTestType(TESTTYPE_CONF);

    m_szTestName="ZFight";
    m_szCommandKey="ZFight";

    ClearStatus();

    m_dwPasses = PDEF;

	m_nAPI = (UINT) 0;

    m_Options.ModeOptions.TextureFmt = FMT_ANY;
    m_Options.ModeOptions.CubeFmt = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt = FMT_ANY;

    if (KeySet("D16"))
        m_Options.ModeOptions.ZBufferFmt = FMT_D16;
    else if (KeySet("D15S1"))
        m_Options.ModeOptions.ZBufferFmt = FMT_D15S1;
    else if (KeySet("D24X4S4"))
        m_Options.ModeOptions.ZBufferFmt = FMT_D24X4S4;
    else if (KeySet("D24X8"))
        m_Options.ModeOptions.ZBufferFmt = FMT_D24X8;
    else if (KeySet("D24S8"))
        m_Options.ModeOptions.ZBufferFmt = FMT_D24S8;
    else if (KeySet("D32"))
        m_Options.ModeOptions.ZBufferFmt = FMT_D32;
    else if (KeySet("D16_LOCKABLE"))
        m_Options.ModeOptions.ZBufferFmt = FMT_D16_LOCKABLE;
	else
#ifdef FAST_TEST
		m_Options.ModeOptions.ZBufferFmt = FMT_D16;
#else
		m_Options.ModeOptions.ZBufferFmt = FMT_ALL;
#endif

    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.fMinDXVersion = KeySet("allow") ? 6.0f : 8.0f;
    
	m_pFramework->GetImageCmp()->SetPassRequired(0.99f);
}

// NAME:        ~CZFight()
// DESC:        destructor for z fight class
// INPUT:       none
// OUTPUT:      none

CZFight::~CZFight(VOID)
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CZFight::CommandLineHelp(VOID)
{
    WriteCommandLineHelp("$yfan: $wfixed primitive type (triangle fan) $c($wOn$c/Off)");
    WriteCommandLineHelp("$ylist: $wfixed primitive type (triangle list) $c($wOn$c/Off)");
    WriteCommandLineHelp("$ystrip: $wfixed primitive type (triangle strip) $c($wOn$c/Off)");
    WriteCommandLineHelp("$ystatic: $wstatic vertex orientation $c($wOn$c/Off)");
    WriteCommandLineHelp("$ywireframe: $wwireframe mode $c($wOn$c/Off)");
    WriteCommandLineHelp("$ypasses: $wpasses per API $c(default=4, min=1, max=4)");
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CZFight::CapsCheck(VOID)
{
    return true;
}

// NAME:        Setup()
// DESC:        setup
// INPUT:       none
// OUTPUT:      true.....if setup is successful
//              false....otherwise

bool CZFight::Setup(VOID)
{
    return true;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CZFight::TestInitialize(VOID)
{
    ProcessArgs();

    TCHAR szBuffer[MAXBUFFER];
    FORMAT *pFormat = &m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat];
    FmtToString(FORMAT_ZBUFFER, pFormat, szBuffer);
    wsprintf(m_szBuffer, "Z Buffer Format: %s.\n", szBuffer);
    WriteToLog(m_szBuffer);

    m_dwRPAPICyclingFrequency = m_dwPasses;

    if (0x0600 == m_dwVersion)    
        m_nAPI = sizeof(m_pRPFullDX6APIList)/sizeof(m_pRPFullDX6APIList[0]);
    else if (0x0700 == m_dwVersion)    
        m_nAPI = sizeof(m_pRPFullDX7APIList)/sizeof(m_pRPFullDX7APIList[0]);
    else
        m_nAPI = sizeof(m_pRPFullDX8APIList)/sizeof(m_pRPFullDX8APIList[0]);

    SetTestRange((UINT) 1, (UINT) (m_nAPI*m_dwPasses));
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CZFight::ExecuteTest(UINT uTest)
{
	CHAR szTempBuffer[2][MAXBUFFER];
    bool bFound = false, bResult = true;
    DWORD dwColors[] = 
    {
        RGB_MAKE(255,   0,   0),
        RGB_MAKE(  0, 255,   0),
        RGB_MAKE(  0,   0, 255),
        RGB_MAKE(255, 255, 255)
    };
    UINT i, j, k;
    UINT uTemp = (UINT) 1;
    UINT nPasses;

    for (i = 0; i < m_nAPI; i++)
        for (j = 1; (j <= m_dwPasses) && !bFound; j++, uTemp++)
            if (uTemp == uTest)
            {
                nPasses = j;
                bFound = true;
            }

    if (!bFound)
        return false;

    sprintf(m_szBuffer, "Rendering iterations: %d", nPasses);
    BeginTestCase(m_szBuffer, uTest);

    if (BeginScene())
    {
        D3DPRIMITIVETYPE m_dwSequence[PMAX] =
        {
            D3DPT_TRIANGLEFAN,
            D3DPT_TRIANGLELIST,
            D3DPT_TRIANGLESTRIP,
            D3DPT_TRIANGLEFAN
        };
        D3DPRIMITIVETYPE dwType;
        DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
        DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
        FLEXVERT Fixed[4];
        FLEXVERT pLeft[3];
        FLEXVERT pRight[3];
        UINT puLeft[3] = { 0, 1, 3 };
        UINT puRight[3] = { 1, 2, 3 };

        Fixed[0].fPx = 0.0f;
        Fixed[0].fPy = (D3DVALUE) (dwHeight >> 1);
        Fixed[0].fPz = 0.0f;
        Fixed[0].fRHW = 1.0f;

        Fixed[1].fPx = (D3DVALUE) (dwWidth >> 1);
        Fixed[1].fPy = 0.0f;
        Fixed[1].fPz = 1.0f;
        Fixed[1].fRHW = 1.0f;

        Fixed[2].fPx = (D3DVALUE) dwWidth;
        Fixed[2].fPy = (D3DVALUE) (dwHeight >> 1);
        Fixed[2].fPz = 1.0f;
        Fixed[2].fRHW = 1.0f;

        Fixed[3].fPx = (D3DVALUE) (dwWidth >> 1);
        Fixed[3].fPy = (D3DVALUE) dwHeight;
        Fixed[3].fPz = 0.0f;
        Fixed[3].fRHW = 1.0f;

        SetRenderState(D3DRS_FILLMODE, m_Flags.Check(CF_WIREFRAME) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

        for (i = 0; i < nPasses; i++)
        {
            k = (UINT) m_Flags.Check(CF_STATIC) ? 0 : i;

            memcpy(&pLeft[0], &Fixed[puLeft[(k+0) % 3]], sizeof(FLEXVERT));
            memcpy(&pLeft[1], &Fixed[puLeft[(k+1) % 3]], sizeof(FLEXVERT));
            memcpy(&pLeft[2], &Fixed[puLeft[(k+2) % 3]], sizeof(FLEXVERT));

            memcpy(&pRight[0], &Fixed[puRight[(k+0) % 3]], sizeof(FLEXVERT));
            memcpy(&pRight[1], &Fixed[puRight[(k+1) % 3]], sizeof(FLEXVERT));
            memcpy(&pRight[2], &Fixed[puRight[(k+2) % 3]], sizeof(FLEXVERT));
        
            for (j = 0; j < 3; j++)
            {
                pLeft[j].dcDiffuse = dwColors[i];
                pRight[j].dcDiffuse = dwColors[i];
            }

            if (m_Flags.Check(CF_TRIFAN))
                dwType = D3DPT_TRIANGLEFAN;
            else if (m_Flags.Check(CF_TRILIST))
                dwType = D3DPT_TRIANGLELIST;
            else if (m_Flags.Check(CF_TRISTRIP))
                dwType = D3DPT_TRIANGLESTRIP;
            else
                dwType = m_dwSequence[i];

            switch (dwType)
            {
                case D3DPT_TRIANGLEFAN:
                    sprintf(m_szStatus[0][1], "$yFan");
					sprintf(szTempBuffer[1], "Fan");
                break;

                case D3DPT_TRIANGLELIST:
                    sprintf(m_szStatus[0][1], "$yList");
					sprintf(szTempBuffer[1], "List");
                break;

                case D3DPT_TRIANGLESTRIP:
                    sprintf(m_szStatus[0][1], "$yStrip");
					sprintf(szTempBuffer[1], "Strip");
                break;
            }

            if (m_Flags.Check(CF_TRITYPES))
			{
                sprintf(m_szStatus[0][0], "$gType (static)");
				sprintf( szTempBuffer[0], "Type (static): %s", szTempBuffer[1] );
			}
            else
			{
                sprintf(m_szStatus[0][0], "$gType (dynamic)");
				sprintf( szTempBuffer[0], "Type (dynamic): %s", szTempBuffer[1] );
			}

            sprintf(m_szStatus[1][1], "$y%d", k);

            if (m_Flags.Check(CF_STATIC))
			{
                sprintf(m_szStatus[1][0], "$gOffset (static)");
				sprintf( szTempBuffer[1], "Offset (static): %d", k );
			}
            else
			{
                sprintf(m_szStatus[1][0], "$gOffset (dynamic)");
				sprintf( szTempBuffer[1], "Offset (dynamic): %d", k );
			}

			WriteToLog( "%s, %s\n", szTempBuffer[0], szTempBuffer[1] );

            SetRenderState(D3DRS_ZFUNC, (DWORD) ((0 == i) ? D3DCMP_LESSEQUAL : D3DCMP_EQUAL));

            if (!RenderPrimitive(dwType, FLEXFMT, (LPVOID) pLeft, 3, NULL, 0, NULL))
            {
                WriteToLog("RenderPrimitive() failed (left): %s.\n", GLE(GetLastError()));
                Fail();
                bResult = false;
            }

            if (!RenderPrimitive(dwType, FLEXFMT, (LPVOID) pRight, 3, NULL, 0, NULL))
            {
                WriteToLog("RenderPrimitive() failed (right): %s.\n", GLE(GetLastError()));
                Fail();
                bResult = false;
            }
        }

        if (!EndScene())
        {
            WriteToLog("EndScene() failed: %s.\n", GLE(GetLastError()));
            Fail();
            bResult = false;
        }
    }
    else
    {
        WriteToLog("BeginScene() failed: %s.\n", GLE(GetLastError()));
        Fail();
        bResult = false;
    }

    return bResult;
}

// NAME:        Cleanup()
// DESC:        cleanup
// INPUT:       none
// OUTPUT:      none

VOID CZFight::Cleanup(VOID)
{
}

// NAME:        UpdateStatus()
// DESC:        update status
// INPUT:       none
// OUTPUT:      none

VOID CZFight::UpdateStatus(VOID)
{
    CD3DTest::UpdateStatus();

    for (UINT i = 0; i < NSTATUS; i++)
        WriteStatus(m_szStatus[i][0], m_szStatus[i][1]);
}

// NAME:        ClearStatus()
// DESC:        clear status
// INPUT:       none
// OUTPUT:      none

VOID CZFight::ClearStatus(VOID)
{
    for (UINT i = 0; i < NSTATUS; i++)
    {
        sprintf(m_szStatus[i][0], "$gNA\t");
        sprintf(m_szStatus[i][1], "$y-");
    }
}

// NAME:        ProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CZFight::ProcessArgs(VOID)
{
    int iDefault, iResult;

    // fan
    // default:  false

    if (KeySet("fan"))
    {
        m_Flags.Clear(CF_TRITYPES);
        m_Flags.Set(CF_TRIFAN);
    }

    // list
    // default:  false

    if (KeySet("list"))
    {
        m_Flags.Clear(CF_TRITYPES);
        m_Flags.Set(CF_TRILIST);
    }

    // strip
    // default:  false

    if (KeySet("strip"))
    {
        m_Flags.Clear(CF_TRITYPES);
        m_Flags.Set(CF_TRISTRIP);
    }

    // static
    // default:  false

    if (KeySet("static"))
        m_Flags.Set(CF_STATIC);

    // wireframe
    // default: false

    if (KeySet("wireframe"))
        m_Flags.Set(CF_WIREFRAME);

    // passes
    // default:  PDEF

    iDefault = (int) PDEF;
    ReadInteger("passes", iDefault, &iResult);
    m_dwPasses = (DWORD) ((iResult < PMIN) ? PMIN : ((iResult >= PMAX) ? PMAX: iResult));
}
