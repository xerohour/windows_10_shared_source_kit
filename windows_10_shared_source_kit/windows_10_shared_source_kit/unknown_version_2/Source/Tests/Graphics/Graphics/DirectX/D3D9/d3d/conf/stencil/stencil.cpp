// FILE:        stencil.cpp
// DESC:        stencil plane conformance tests
// AUTHOR:      Todd M. Frost

#include "stencil.h"
#include "CSurface.h"

CD3DWindowFramework App;

#if FAST_TEST
CStencilCap         StencilCap;
CStencilClear       StencilClear;
CStencilCullOne     StencilCullOne;
CStencilCullTwo     StencilCullTwo;
CStencilFunc        StencilFunc;
CStencilIgnore      StencilIgnore;
CStencilOp          StencilOp;
CStencilSCorrupt    StencilSCorrupt;
CStencilZCorrupt    StencilZCorrupt;
#else
CStencilCap         StencilCap;
CStencilClear       StencilClear;
CStencilClearCCW    StencilClearCCW;
CStencilCullOne     StencilCullOne;
CStencilCullTwo     StencilCullTwo;
CStencilFunc        StencilFunc;
CStencilFuncCCW     StencilFuncCCW;
CStencilIgnore      StencilIgnore;
CStencilIgnoreCCW   StencilIgnoreCCW;
CStencilOp          StencilOp;
CStencilOpCCW       StencilOpCCW;
CStencilSCorrupt    StencilSCorrupt;
CStencilSCorruptCCW StencilSCorruptCCW;
CStencilZCorrupt    StencilZCorrupt;
CStencilZCorruptCCW StencilZCorruptCCW;
#endif

// NAME:        CStencil()
// DESC:        stencil class constructor
// INPUT:       none
// OUTPUT:      none

CStencil::CStencil()
{
    SetTestType(TESTTYPE_CONF);

    m_szTestName = "Stencil";
    m_szCommandKey = "Stencil";

    m_dwFailColor = (D3DCOLOR) 0;
    m_dwPassColor = (D3DCOLOR) 0;

    m_dwStencilBitDepth = (DWORD) 0;
    m_dwStencilBitDepthReported = (DWORD) 0;
    m_dwStencilBitMask = (DWORD) 0;
    m_dwStencilCaps = (DWORD) 0;

    m_dwZCmpCaps = (DWORD) 0;

    m_uRange = (UINT) 1;

    m_Options.ModeOptions.CubeFmt = FMT_ANY;
    m_Options.ModeOptions.TextureFmt = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt = FMT_ANY;
    m_Options.ModeOptions.MultisampType = MULTISAMPLE_NONE; // render target lock required

	// Cycle through all z buffers with stencil support
    m_Options.ModeOptions.dwZInclude = PF_STENCILBUFFER;
    m_Options.ModeOptions.ZBufferFmt = FMT_ALL;

    m_Options.D3DOptions.bReference = false;
    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.nBackBufferWidth = (DWORD) 40;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD) 35;

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81 | DXVERSION_7;
}

// NAME:        ~CStencil()
// DESC:        stencil class destructor
// INPUT:       none
// OUTPUT:      none

CStencil::~CStencil()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencil::CommandLineHelp(VOID)
{
    WriteCommandLineHelp("$yreverse: $wreverse vertex winding order $c($wOn$c/Off)");
    WriteCommandLineHelp("$yverbose: $wverbose $c($wOn$c/Off)");
}

// NAME:        ClearFrame()
// DESC:        clear frame
// INPUT:       none
// OUTPUT:      true
// COMMENTS:    defeat framework's ClearFrame()

bool CStencil::ClearFrame(VOID)
{
    return true;
}

// NAME:        BeginScene()
// DESC:        begin scene
// INPUT:       dvZ.........clear z
//              dwStencil...clear stencil
//              dwColor.....clear color
//              dwFlags.....clear flags
// OUTPUT:      true.....if clear/begin scene successful
//              false....otherwise

bool CStencil::BeginScene(D3DVALUE dvZ, DWORD dwStencil, DWORD dwColor, DWORD dwFlags)
{
    if (dwFlags && !Clear(0, NULL, dwFlags, dwColor, dvZ, dwStencil))
    {
        WriteToLog("Unable to clear z buffer.\n");
        return false;
    }
    else if (!CD3DTest::BeginScene())
    {
        WriteToLog("Unable to clear z buffer.\n");
        return false;
    }

    return true;
}

// NAME:        SetTestRange()
// DESC:        set test range
// INPUT:       uStart...start test range
//              uEnd.....end test range
// OUTPUT:      true

bool CStencil::SetTestRange(UINT uStart, UINT uEnd)
{
    if (m_Flags.Check(CF_VERBOSE) && (uEnd > 0))
        WriteToLog("Current test range:  (%d,%d).\n", uStart, uEnd);

    if (uStart < uEnd)
        m_uRange = uEnd - uStart;
    else
        m_uRange = (UINT) 1;

    return CD3DTest::SetTestRange(uStart, uEnd);
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CStencil::CapsCheck(VOID)
{
    return true;
}

// NAME:        Setup()
// DESC:        setup
// INPUT:       none
// OUTPUT:      true.....if setup is successful
//              false....otherwise

bool CStencil::Setup(VOID)
{
    if (m_dwVersion > 0x0600)
        if (!SetRenderState(D3DRS_LIGHTING, (DWORD) FALSE))
        {
            WriteToLog("Unable to disable lighting.\n");
            return false;
        }

    return true;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      D3DTESTINIT_RUN
// COMMENTS:    pixel format bit depths can be unreliable, compute from bit masks

UINT CStencil::TestInitialize(VOID)
{
    ProcessArgs();

    TCHAR szBuffer[MAXBUFFER];
    FORMAT *pFormat = &m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat];
    FmtToString(FORMAT_ZBUFFER, pFormat, szBuffer);
    wsprintf(m_szBuffer, "Z Buffer Format: %s.\n", szBuffer);
    WriteToLog(m_szBuffer);

    DWORD dwDepth = (UINT) 0;
    DWORD dwCount = pFormat->ddpfPixelFormat.dwStencilBitMask;

    while (0 != dwCount)
    {
        dwDepth += (dwCount & 1);
        dwCount >>= 1;
    }

    m_dwStencilBitDepth = dwDepth;
    m_dwStencilBitDepthReported = pFormat->ddpfPixelFormat.dwStencilBitDepth;
    m_dwStencilBitMask = (DWORD) ((1 << m_dwStencilBitDepth) - 1);
    m_dwStencilCaps = m_pSrcDevice->GetCaps()->dwStencilCaps;
    m_dwZCmpCaps = m_pSrcDevice->GetCaps()->dpcTriCaps.dwZCmpCaps;

    if (!SetDefaultStencilState())
    {
        WriteToLog("Incorrect render state defaults.\n");
        return D3DTESTINIT_ABORT;
    }

	if (KeySet("ZWriteOff"))
		SetRenderState(D3DRS_ZWRITEENABLE, (DWORD) FALSE);

	if (KeySet("AlphaTest"))
		SetRenderState(D3DRS_ALPHATESTENABLE, (DWORD) TRUE);

    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true
// COMMENTS:    fail color always has max red component (255)
//              pass color always has min red component (  0)

bool CStencil::ExecuteTest(UINT uTest)
{
	HRESULT hResult = S_OK;
    D3DVALUE dvDelta = 0.0f;
    DWORD dwColor[3];
	CSurface *pTarget;

	if (D3D_OK != (hResult = m_pSrcDevice->GetRenderTarget(&pTarget)))
    {
        WriteToLog("Unable get render target: %s.\n", GLE(hResult));
        return false;
    }

    if (m_uRange > 1)
        dvDelta = ((D3DVALUE) (uTest - 1))/((D3DVALUE) (m_uRange - 1));
 
    dwColor[0] = (DWORD) 255;
    dwColor[1] = (DWORD) (255.0*(1.0f - dvDelta) + 0.5);
    dwColor[2] = (DWORD) (255.0*(0.0f + dvDelta) + 0.5);

    dwColor[0] = (dwColor[0] > 255) ? 255 : dwColor[0];
    dwColor[1] = (dwColor[1] > 255) ? 255 : dwColor[1];
    dwColor[2] = (dwColor[2] > 255) ? 255 : dwColor[2];

    m_dwFailColor = RGB_MAKE(dwColor[0], dwColor[1], dwColor[2]);
	m_dwPassColor = 0xFFFFFFFF;
    m_dwPassColor &= ~m_dwFailColor;

	RELEASE( pTarget );
    return true;
}

// NAME:        Cleanup()
// DESC:        cleanup
// INPUT:       none
// OUTPUT:      none

VOID CStencil::Cleanup(VOID)
{
}

// NAME:        CompareImages()
// DESC:        compare images
// INPUT:       pSrc.....address of CSurface source
//              pRef.....address of CSurface reference
//              pResult..address of CSurface result
// OUTPUT:      true
// COMMENTS:    defeat framework's CompareImages()

bool CStencil::CompareImages(CSurface *pSrc, CSurface *pRef, CSurface *pResult)
{
    return true;
}

// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      true

bool CStencil::ProcessFrame(VOID)
{
    return true;
}

// NAME:        Compare()
// DESC:        compare target to key
// INPUT:       none
// OUTPUT:      true.........if extract successful
//              false........otherwise

bool CStencil::Compare(VOID)
{
    int i, j;
    CSurface *pTarget;
    DWORD dwColor;
    DWORD dwFailed = (DWORD) 0, dwInspected = (DWORD) 0, dwPassed = (DWORD) 0;
    LOCKED_RECT Lock;
    VOID *pvBuffer, *pvColor;
    HRESULT hResult;

	if (D3D_OK != (hResult = m_pSrcDevice->GetRenderTarget(&pTarget)))
    {
        WriteToLog("Unable get render target: %s.\n", GLE(hResult));
        return false;
    }
	
    while (DDERR_WASSTILLDRAWING == (hResult = pTarget->LockRect(&Lock, NULL, (DWORD) 0)));

    if (D3D_OK != hResult)
    {
        WriteToLog("Unable to lock render target: %s.\n", GLE(hResult));
        return false;
    }
    else if (!(pvBuffer = Lock.pBits))
    {
        WriteToLog("Lock succeded but returned empty buffer.\n"); 
        return false;
    }
    else if ((m_pCurrentMode->nBackBufferWidth != pTarget->m_Desc.dwWidth) ||
             (m_pCurrentMode->nBackBufferHeight != pTarget->m_Desc.dwHeight))
    {
        WriteToLog("Render target and buffer dimensions don't match.\n");
        return false;
    }

    for (j = 0; j < (int) pTarget->m_Desc.dwHeight; j++)
    {
        pvColor = (char *) pvBuffer + (j*Lock.iPitch);

        for (i = 0; i < (int) pTarget->m_Desc.dwWidth; i++)
        {
            dwColor = pTarget->ReadColor(&pvColor);

			for ( UINT k = 0; k < 3 ; k++ )
			{
				if (abs(((BYTE*)&m_dwPassColor)[k] - ((BYTE*)&dwColor)[k] ) > 8 )
					dwFailed++;
				else
					dwPassed++;

				dwInspected++;
			}
        }
    }

    if (D3D_OK != (hResult = pTarget->UnlockRect()))
    {
        WriteToLog("Unable to unlock render target: %s.\n", GLE(hResult));
        return false;
    }

    RELEASE(pTarget);

    if (!dwInspected || (dwInspected != dwPassed))
    {
        m_fPassPercentage = (dwInspected) ? ((float) dwPassed)/((float) dwInspected) : 0.0f;
        return false;
    }
    else
    {
        m_fPassPercentage = 1.0f;
        return true;
    }
}

// NAME:        SetDefaultStencilState()
// DESC:        set default stencil state
// INPUT:       none
// OUTPUT:      true.....if default render states match specs
//              false....otherwise

bool CStencil::SetDefaultStencilState(VOID)
{
    bool bSuccess = true;

    if (m_Flags.Check(CF_VALIDATE))
    {
        DWORD dwState = (DWORD) 0;

        if (!(m_pSrcDevice->GetBehaviors() & DEVICECREATE_PURE))
        {
            GetRenderState(D3DRS_CULLMODE, &dwState);
            bSuccess = (D3DCULL_CCW == dwState) ? bSuccess : false;

            GetRenderState(D3DRS_ZFUNC, &dwState);
            bSuccess = (D3DCMP_LESSEQUAL == dwState) ? bSuccess : false;

            GetRenderState(D3DRS_STENCILENABLE, &dwState);
            bSuccess = (FALSE == dwState) ? bSuccess : false;
            GetRenderState(D3DRS_STENCILREF, &dwState);
            bSuccess = (0x00000000 == dwState) ? bSuccess : false;
            GetRenderState(D3DRS_STENCILMASK, &dwState);
            bSuccess = (0xffffffff == dwState) ? bSuccess : false;
            GetRenderState(D3DRS_STENCILWRITEMASK, &dwState);
            bSuccess = (0xffffffff == dwState) ? bSuccess : false;

            GetRenderState(D3DRS_STENCILFUNC, &dwState);
            bSuccess = (D3DCMP_ALWAYS == dwState) ? bSuccess : false;
            GetRenderState(D3DRS_STENCILFAIL, &dwState);
            bSuccess = (D3DSTENCILOP_KEEP == dwState) ? bSuccess : false;
            GetRenderState(D3DRS_STENCILZFAIL, &dwState);
            bSuccess = (D3DSTENCILOP_KEEP == dwState) ? bSuccess : false;
            GetRenderState(D3DRS_STENCILPASS, &dwState);
            bSuccess = (D3DSTENCILOP_KEEP == dwState) ? bSuccess : false;

            if (m_dwVersion > 0x0800)
            {
                GetRenderState(D3DRS_TWOSIDEDSTENCILMODE, &dwState);
                bSuccess = (FALSE == dwState) ? bSuccess : false;

                GetRenderState(D3DRS_CCW_STENCILFUNC, &dwState);
                bSuccess = (D3DCMP_ALWAYS == dwState) ? bSuccess : false;
                GetRenderState(D3DRS_CCW_STENCILFAIL, &dwState);
                bSuccess = (D3DSTENCILOP_KEEP == dwState) ? bSuccess : false;
                GetRenderState(D3DRS_CCW_STENCILZFAIL, &dwState);
                bSuccess = (D3DSTENCILOP_KEEP == dwState) ? bSuccess : false;
                GetRenderState(D3DRS_CCW_STENCILPASS, &dwState);
                bSuccess = (D3DSTENCILOP_KEEP == dwState) ? bSuccess : false;
            }
        }

        m_Flags.Clear(CF_VALIDATE);
    }

    SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_CCW);

    SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_LESSEQUAL);

    SetRenderState(D3DRS_STENCILENABLE, (DWORD) FALSE);
    SetRenderState(D3DRS_STENCILREF, (DWORD) 0x00000000);
    SetRenderState(D3DRS_STENCILMASK, (DWORD) 0xffffffff);
    SetRenderState(D3DRS_STENCILWRITEMASK, (DWORD) 0xffffffff);

    SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_ALWAYS);
    SetRenderState(D3DRS_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
    SetRenderState(D3DRS_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
    SetRenderState(D3DRS_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

    if (m_dwVersion > 0x0800)
    {
        SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, (DWORD) FALSE);

        SetRenderState(D3DRS_CCW_STENCILFUNC, (DWORD) D3DCMP_ALWAYS);
        SetRenderState(D3DRS_CCW_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_CCW_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_CCW_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);
    }

    return bSuccess;
}

// NAME:        SupportedOperation()
// DESC:        supported stencil operation indicator
// INPUT:       dwOp.....stencil operation
// OUTPUT:      true.....if stencil operation is supported
//              false....otherwise

bool CStencil::SupportedOperation(DWORD dwOp)
{
    bool bSupported = false;

    switch (dwOp)
    {
        case D3DSTENCILOP_KEEP:
            if (D3DSTENCILCAPS_KEEP & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_ZERO:
            if (D3DSTENCILCAPS_ZERO & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_REPLACE:
            if (D3DSTENCILCAPS_REPLACE & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_INCRSAT:
            if (D3DSTENCILCAPS_INCRSAT & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_DECRSAT:
            if (D3DSTENCILCAPS_DECRSAT & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_INVERT:
            if (D3DSTENCILCAPS_INVERT & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_INCR:
            if (D3DSTENCILCAPS_INCR & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_DECR:
            if (D3DSTENCILCAPS_DECR & m_dwStencilCaps)
                bSupported = true;
        break;

        default:
        break;
    }

    return bSupported;
}

// NAME:        Flood()
// DESC:        flood rendering target
// INPUT:       dvDepth..flood depth
//              dcColor..flood color
//              dwFlags..flags
// OUTPUT:      none

VOID CStencil::Flood(D3DVALUE dvDepth, D3DCOLOR dcColor, DWORD dwFlags)
{
    D3DVALUE fRHW = (dvDepth > 0.0) ? 1.0f/dvDepth : 1.0f;
    DWORD dwTemp;
    FLEXVERT pVertices[4];
    UINT i, j;

    if (m_Flags.Check(CF_REVERSE))
        dwFlags = (~dwFlags & FLOOD_BOTH);

    for (dwTemp = FLOOD_CCW; dwTemp >= FLOOD_CW; dwTemp >>= 1)
    {
        if (dwTemp & dwFlags)
        {
            i = (UINT) ((dwTemp & FLOOD_CW) ? 1 : 3);
            j = (UINT) ((dwTemp & FLOOD_CW) ? 3 : 1);
    
            pVertices[0].fPx = 0.0f;
            pVertices[0].fPy = (D3DVALUE) m_pCurrentMode->nBackBufferHeight;
            pVertices[0].fPz = dvDepth;
            pVertices[0].fRHW = fRHW;
            pVertices[0].dcDiffuse = dcColor;

            pVertices[i].fPx = 0.0f;
            pVertices[i].fPy = 0.0f;
            pVertices[i].fPz = dvDepth;
            pVertices[i].fRHW = fRHW;
            pVertices[i].dcDiffuse = dcColor;

            pVertices[2].fPx = (D3DVALUE) m_pCurrentMode->nBackBufferWidth;
            pVertices[2].fPy = 0.0f;
            pVertices[2].fPz = dvDepth;
            pVertices[2].fRHW = fRHW;
            pVertices[2].dcDiffuse = dcColor;

            pVertices[j].fPx = (D3DVALUE) m_pCurrentMode->nBackBufferWidth;
            pVertices[j].fPy = (D3DVALUE) m_pCurrentMode->nBackBufferHeight;
            pVertices[j].fPz = dvDepth;
            pVertices[j].fRHW = fRHW;
            pVertices[j].dcDiffuse = dcColor;

            if (!RenderPrimitive(D3DPT_TRIANGLEFAN, FLEXFMT, (LPVOID) pVertices, 4, NULL, 0, NULL))
                WriteToLog("RenderPrimitive() failed: %s\n.", GLE(GetLastError()));
        }
    }
}

// NAME:        Stipple()
// DESC:        stipple enable/disable
// INPUT:       bEnable..enable flag
// OUTPUT:      none

VOID CStencil::Stipple(bool bEnable)
{
    D3DRENDERSTATETYPE dwType;
    DWORD dwMask, dwState; 
    UINT i;

    // all stipple renderstates retired post DX6

    if (m_dwVersion > 0x0600)
        return;

    if (bEnable)
    {
        dwMask = 0xffffffff;
        dwState = (DWORD) TRUE;
    }
    else
    {
        dwMask = 0x00000000;
        dwState = (DWORD) FALSE;
    }

    SetRenderState(D3DRS_STIPPLEENABLE, dwState);

    for (i = 0; i < 32; i++)
    {
        dwType = (D3DRENDERSTATETYPE) D3DRS_STIPPLEPATTERN(i);
        SetRenderState(dwType, dwMask);
    }
}

// NAME:        ProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CStencil::ProcessArgs(VOID)
{
    m_Flags.Set(CF_VALIDATE);

    // reverse
    // default:  off

    if (KeySet("reverse"))
        m_Flags.Set(CF_REVERSE);

    // verbose
    // default:  off

    if (KeySet("verbose"))
        m_Flags.Set(CF_VERBOSE);
}
