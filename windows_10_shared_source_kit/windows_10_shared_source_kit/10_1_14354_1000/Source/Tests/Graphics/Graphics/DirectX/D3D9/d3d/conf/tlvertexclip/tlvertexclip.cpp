// FILE:        TLVertexClip.cpp
// DESC:        TL vertex clip conformance tests

#include "TLVertexClip.h"
#include "CSurface.h"

CD3DWindowFramework App;
CTLVertexClipEdge   TLVertexClipEdge;
CTLVertexClipQuad   TLVertexClipQuad;

// NAME:        CTLVertexClip()
// DESC:        TL vertex clip class constructor
// INPUT:       none
// OUTPUT:      none

CTLVertexClip::CTLVertexClip()
{
    SetTestType(TESTTYPE_CONF);

    m_bShow = false;

    ClearStatus();

    m_Port.SetPort((long) 0, (long) 0, (long) 320, (long) 280);

    m_Options.ModeOptions.CubeFmt = FMT_ANY;
    m_Options.ModeOptions.TextureFmt = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt = FMT_ANY;
    m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
    m_Options.ModeOptions.MultisampType = MULTISAMPLE_NONE; // render target lock required

    m_Options.D3DOptions.bReference = false;
    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.nBackBufferWidth = (DWORD) 160;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD) 140;

	// Force test to use relevant DirectX versions
	m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81 | DXVERSION_7 | DXVERSION_6;
}

// NAME:        ~CTLVertexClip()
// DESC:        TL vertex clip class destructor
// INPUT:       none
// OUTPUT:      none

CTLVertexClip::~CTLVertexClip()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CTLVertexClip::CommandLineHelp(void)
{
    WriteCommandLineHelp("$yline: $wline color $c(default(ARGB)=0x0000FFFF)");
    WriteCommandLineHelp("$yport: $wport color $c(default(ARGB)=0x00000000)");
    WriteCommandLineHelp("$yshow: $wshow port $c($wOn$c/Off)");
}

// NAME:        SetupViewport()
// DESC:        set up viewport
// INPUT:       none
// OUTPUT:      none

bool CTLVertexClip::SetViewport(UINT nX, UINT nY, UINT nWidth, UINT nHeight)
{
    VIEWPORT Viewport;

    Viewport.X = (DWORD) nX;
    Viewport.Y = (DWORD) nY;
    Viewport.Width = (DWORD) nWidth;
    Viewport.Height = (DWORD) nHeight;
    Viewport.MinZ = 0.0f;
    Viewport.MaxZ = 1.0f;

	m_Port.SetPort(nX, nY, nWidth, nHeight);
    return CD3DTest::SetViewport(&Viewport);
}

// NAME:        ClearFrame()
// DESC:        clear frame
// INPUT:       none
// OUTPUT:      Clear()
// COMMENTS:    clear color must be black (0, 0, 0) see AnyExtraPixels()

bool CTLVertexClip::ClearFrame(void)
{
    bool bResult;
    D3DVALUE dvZ = 1.0f;
    DWORD dwColor = RGBA_MAKE(0, 0, 0, 255);
    DWORD dwFlags = (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER);
    DWORD dwStencil = (DWORD) 0;

    SetViewport(0, 0, m_pCurrentMode->nBackBufferWidth, m_pCurrentMode->nBackBufferHeight);
    bResult = Clear(0, NULL, dwFlags, dwColor, dvZ, dwStencil);
    SetViewport(PORT_X, PORT_Y, PORT_WIDTH, PORT_HEIGHT);
    return bResult;
}

// NAME:        TestInitialize()
// DESC:        test initialize
// INPUT:       none
// OUTPUT:      D3DTESTINIT_RUN

UINT CTLVertexClip::TestInitialize(void)
{
    CHAR szBuffer[32];
    UINT uTemp;

	m_Type = D3DPT_LINELIST;

	m_dwLineColor = RGBA_MAKE(  0, 255, 255, 255);
	m_dwPortColor = RGBA_MAKE(255,   0,   0,   0);

    // line color
    // default:  white

    lstrcpy(szBuffer, "");
    ReadString("line", szBuffer, ARRAYSIZE(szBuffer));
    m_dwLineColor = (DWORD) (sscanf(szBuffer, "%x", &uTemp) == 1) ? uTemp : m_dwLineColor;
    m_dwLineColor &= 0x00ffffff;

    // port color
    // default:  red

	lstrcpy(szBuffer, "");
	ReadString("port", szBuffer, ARRAYSIZE(szBuffer));
    m_dwPortColor = (DWORD) (sscanf(szBuffer, "%x", &uTemp) == 1) ? uTemp : m_dwPortColor;
    m_dwPortColor &= 0x00ffffff;

    if (KeySet("show"))	
        m_bShow = true;

    // 2x, one for each primitive (D3DPT_LINELIST & D3DPT_LINESTRIP)
	SetTestRange((UINT) 1, (UINT) (2*m_nTests));

    // TL vertex clipping is mandatory thru Dx6 aand Dx7, no need to check caps
    // TL vertex clipping is capped for Dx8 devices thru Dx8 and may be emulated by Dx8.1

	if (m_dwVersion >= 0x0800)
	{		
        CAPS Caps; // raw device caps
        m_pD3D->GetDeviceCaps(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), &Caps);

		if (!(Caps.PrimitiveMiscCaps & D3DPMISCCAPS_CLIPTLVERTS))
		{
			// As of Dx8 TLVertices are not clipped by the runtime driver must specify the
			// D3DPMISCCAPS_CLIPTLVERTS flag or it is the app's responsibility.
			WriteToLog("CTLVertexClip::TestInitialize() - D3DPMISCCAPS_CLIPTLVERTS is not set ...\n");
			DEVICEDESC *pDesc; // device description after vertex processing behavior applied
			pDesc = m_pSrcDevice->GetCaps();

			if (pDesc->dwPrimitiveMiscCaps & D3DPMISCCAPS_CLIPTLVERTS)
			{
				//As of 8.1 we will emulate this functionality
				WriteToLog("CTLVertexClip::TestInitialize() - Testing emulation...\n");
			}
			else
			{
				//In this case we switch the test to being an invalid params test.
				m_bInvalid = true;
			}
		}
	}

    if (m_dwVersion >= 0x0700)
	    SetRenderState(D3DRS_LIGHTING, (DWORD) FALSE);

	// set up clip port

	D3DCOLOR dcDiffuse = m_dwPortColor;
	D3DVECTOR A, B;

    A.x = (float) m_Port.Left();
    A.y = (float) m_Port.Top();
    A.z = 0.5f;

	B.x = (float) m_Port.Right();
    B.y = (float) m_Port.Top();
    B.z = 0.5f;

	// since test line uses points [0] & [1], start clip port's line list at [2]

    m_Vertices[2].fPx = A.x;
    m_Vertices[2].fPy = A.y;
    m_Vertices[2].fPz = A.z;
    m_Vertices[2].fRHW = 2.0f;
    m_Vertices[2].dcDiffuse = dcDiffuse;

    m_Vertices[3].fPx = B.x;
    m_Vertices[3].fPy = B.y;
    m_Vertices[3].fPz = B.z;
    m_Vertices[3].fRHW = 2.0f;
    m_Vertices[3].dcDiffuse = dcDiffuse;

	A.x = (float) m_Port.Right();
	A.y = (float) m_Port.Bottom();

    m_Vertices[4].fPx = B.x;
    m_Vertices[4].fPy = B.y;
    m_Vertices[4].fPz = B.z;
    m_Vertices[4].fRHW = 2.0f;
    m_Vertices[4].dcDiffuse = dcDiffuse;

    m_Vertices[5].fPx = A.x;
    m_Vertices[5].fPy = A.y;
    m_Vertices[5].fPz = A.z;
    m_Vertices[5].fRHW = 2.0f;
    m_Vertices[5].dcDiffuse = dcDiffuse;

	B.x = (float) m_Port.Left();
	B.y = (float) m_Port.Bottom();

    m_Vertices[6].fPx = B.x;
    m_Vertices[6].fPy = B.y;
    m_Vertices[6].fPz = B.z;
    m_Vertices[6].fRHW = 2.0f;
    m_Vertices[6].dcDiffuse = dcDiffuse;

    m_Vertices[7].fPx = A.x;
    m_Vertices[7].fPy = A.y;
    m_Vertices[7].fPz = A.z;
    m_Vertices[7].fRHW = 2.0f;
    m_Vertices[7].dcDiffuse = dcDiffuse;

	A.x = (float) m_Port.Left();
	A.y = (float) m_Port.Top();

    m_Vertices[8].fPx = A.x;
    m_Vertices[8].fPy = A.y;
    m_Vertices[8].fPz = A.z;
    m_Vertices[8].fRHW = 2.0f;
    m_Vertices[8].dcDiffuse = dcDiffuse;

    m_Vertices[9].fPx = B.x;
    m_Vertices[9].fPy = B.y;
    m_Vertices[9].fPz = B.z;
    m_Vertices[9].fRHW = 2.0f;
    m_Vertices[9].dcDiffuse = dcDiffuse;

	WriteToLog("Using a clip rect of (Left, Top, Right, Bottom) \n (%ld, %ld, %ld, %ld).\n",
               m_Port.Left(), m_Port.Top(), m_Port.Right(), m_Port.Bottom() );
	return (UINT) D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test index
// OUTPUT:      true

bool CTLVertexClip::ExecuteTest(UINT uTest)
{
	D3DCOLOR dcDiffuse(m_dwLineColor);
	D3DVECTOR A, B;

	uTest--; // test coords are zero based

	if (m_nTests <= uTest)
	{
		m_Type = D3DPT_LINESTRIP;
		lstrcpy(m_szBuffer, "D3DPT_LINESTRIP" );
		uTest -= m_nTests; // so that the range is still 0..m_nTests
	}
	else
	{
		m_Type = D3DPT_LINELIST;
		lstrcpy(m_szBuffer, "D3DPT_LINELIST");
	}

	SetEndPoints(&A, &B, uTest);

    m_Vertices[0].fPx = A.x;
    m_Vertices[0].fPy = A.y;
    m_Vertices[0].fPz = A.z;
    m_Vertices[0].fRHW = 2.0f;
    m_Vertices[0].dcDiffuse = dcDiffuse;

    m_Vertices[1].fPx = B.x;
    m_Vertices[1].fPy = B.y;
    m_Vertices[1].fPz = B.z;
    m_Vertices[1].fRHW = 2.0f;
    m_Vertices[1].dcDiffuse = dcDiffuse;

	wsprintf(m_szBuffer, "%s from %.2f, %.2f to %.2f, %.2f ", m_szBuffer, A.x, A.y, B.x, B.y);
	BeginTestCase(m_szBuffer);
    return true;
}

// NAME:        UpdateStatus()
// DESC:        update status
// INPUT:       none
// OUTPUT:      none

VOID CTLVertexClip::UpdateStatus(VOID)
{
    CD3DTest::UpdateStatus();

    for (UINT i = 0; i < NSTATUS; i++)
        WriteStatus(m_szStatus[i][0], m_szStatus[i][1]);
}

// NAME:        ClearStatus()
// DESC:        clear status
// INPUT:       none
// OUTPUT:      none

VOID CTLVertexClip::ClearStatus(VOID)
{
    for (UINT i = 0; i < NSTATUS; i++)
    {
        lstrcpy(m_szStatus[i][0], "$gNA\t");
        lstrcpy(m_szStatus[i][1], "$y-");
    }
}

HRESULT CTLVertexClip::GetLastError(void)
{
	if (m_bInvalid)
	{
		return NOERROR;
	}
	else
	{
		return CD3DTest::GetLastError();
	}
}
// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      true.....if any extra pixels are present
//              false....otherwise

bool CTLVertexClip::ProcessFrame(void)
{
    bool bResult;
    static int nPass = 0;
    static int nFail = 0;

    bResult = !AnyExtraPixels();

	if (m_bInvalid)
    {
        nPass++;
        wsprintf(m_szBuffer, "$yPass(invalid param): %d, Fail: %d", nPass, nFail);
        Pass();
    }
    else if (bResult)
    {
        nPass++;
        wsprintf(m_szBuffer, "$yPass: %d, Fail: %d", nPass, nFail);
        Pass();
    }
    else
    {
        nFail++;
        wsprintf(m_szBuffer, "$yPass: %d, Fail: %d", nPass, nFail);
        Fail();
    }

    lstrcpy(m_szStatus[0][0], "$gOverall Results");
    lstrcpy(m_szStatus[0][1], m_szBuffer);
    return bResult;
}

// NAME:        SceneRefresh()
// DESC:        scene refresh
// INPUT:       none
// OUTPUT:      none

void CTLVertexClip::SceneRefresh(void)
{
    if (BeginScene())
    {
		if (m_bShow)
            RenderPrimitive(D3DPT_LINELIST, FLEXFMT, &m_Vertices[2], 8, NULL, 0, 0);

        RenderPrimitive(m_Type, FLEXFMT, m_Vertices, 2, NULL, 0, 0);        
        EndScene();
    }
}


// NAME:        LargestDist()
// DESC:        checks for the greatest distance from the viewport edge
// INPUT:       lx.....current pixel x
//              ly.....current pixel y
//				plDx..largest error so far in x
//              plDy..largest error so far in y
// OUTPUT:      none

void CTLVertexClip::LargestDist(long lx, long ly, long *plDx, long *plDy)
{
	if (lx < m_Port.Left())
    {
        if ((m_Port.Left() - lx) > *plDx)
            *plDx = m_Port.Left() - lx;
    }
    else
    {
        if ((lx - m_Port.Right()) > *plDx)
            *plDx = lx - m_Port.Right();
    }

    if (ly < m_Port.Top())
    {
        if ( (m_Port.Top() - ly) > *plDy)
            *plDy = m_Port.Top() - ly;
    }
    else
    {
        if ((ly - m_Port.Bottom()) > *plDy)
            *plDy = ly - m_Port.Bottom();
    }

	return;
}

// NAME:        AnyExtraPixels()
// DESC:        extra pixel check
// INPUT:       none
// OUTPUT:      true.....if any extra pixels are present
//              false....otherwise
// COMMENTS:    clear color must be black (0, 0, 0) see ClearFrame()

bool CTLVertexClip::AnyExtraPixels(void)
{
	int nBad;
	long lDx, lDy;
    CSurface *pTarget;
    DWORD dwAlpha, dwColor, dwFilter, dwOffset;
    LOCKED_RECT Lock;
    HRESULT hResult;
    VOID *pvBuffer, *pvColor;

    if (DD_OK != (hResult = m_pSrcDevice->GetRenderTarget(&pTarget)))
    {
        WriteToLog("Unable get render target: %s.\n", GLE(hResult));
        return false;
    }

    while (DDERR_WASSTILLDRAWING == (hResult = pTarget->LockRect(&Lock, NULL, (DWORD) 0)));

    if (DD_OK != hResult)
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

    nBad = 0;
    lDx = lDy = 0;
    dwAlpha = pTarget->m_Mask.dwAlphaMask << 24;
    dwFilter = (pTarget->m_Mask.dwRedMask   << 16) |
               (pTarget->m_Mask.dwGreenMask <<  8) |
               (pTarget->m_Mask.dwBlueMask);

    switch (pTarget->m_Desc.Format.ddpfPixelFormat.dwRGBBitCount)
    {
        case 16:
            dwOffset = 2;
        break;

        case 24:
            dwOffset = 3;
        break;

        case 32:
            dwOffset = 4;
        break;

        default:
            dwOffset = 0;
            WriteToLog("Unsupported surface depth: %d\n", pTarget->m_Desc.Format.ddpfPixelFormat.dwRGBBitCount);
        break;
    }

    for (long j = 0; j < (long) pTarget->m_Desc.dwHeight; j++)
    {
        pvColor = (char *) pvBuffer + (j*Lock.iPitch);

        for (long i = 0; i < (long) pTarget->m_Desc.dwWidth; i++)
        {
            if (!m_Port.Inside(i, j))
            {
                dwColor = pTarget->ReadColor(&pvColor);
                //Remove the alpha component from this test.
                //The clear test should catch the case.
                //where alpha is written even though the masks say it should not be.
                if ( (dwColor & dwFilter) )//|| ((dwAlpha & dwColor) != dwAlpha))
                {
                    nBad++;
                    LargestDist(i, j, &lDx, &lDy);
                }
            }
            else
                pvColor = (PVOID) (((PBYTE) pvColor) + dwOffset);
        }
    }

    if (DD_OK != (hResult = pTarget->UnlockRect()))
    {
        WriteToLog("Unable to unlock render target: %s.\n", GLE(hResult));
        return false;
    }

    RELEASE(pTarget);

    if (0 != nBad)
        WriteToLog("\t%d bad pixels(s) with max distance of %ld, %ld\n", nBad, lDx, lDy);

	return (0 != nBad);
}

// NAME:        PxFromTest()
// DESC:        point x coord from test index
// INPUT:       nPoint...point index
// OUTPUT:      none

float CTLVertexClip::PxFromTest(UINT nPoint)
{
    switch(nPoint/POINTS_PER_SIDE)
    {
        case 0:
            return (float) ((CLIP_LARGER_THAN_XY*(nPoint % POINTS_PER_SIDE)) / POINTS_PER_SIDE);
        case 1:
            return CLIP_LARGER_THAN_XY;
        case 2:
            return (float) (CLIP_LARGER_THAN_XY - ((CLIP_LARGER_THAN_XY*(nPoint % POINTS_PER_SIDE)) / POINTS_PER_SIDE));
        case 3:
            return 0.0f;
        default:
            return CLIP_LARGER_THAN_XY;
    }

    return 0.0f;
}

// NAME:        PyFromTest()
// DESC:        point y from test index
// INPUT:       nPoint...point index
// OUTPUT:      none

float CTLVertexClip::PyFromTest(UINT nPoint)
{
    switch(nPoint/POINTS_PER_SIDE)
    {
        case 0:
            return 0.0f;
        case 1:
            return (float) ((CLIP_LARGER_THAN_XY*(nPoint % POINTS_PER_SIDE)) / POINTS_PER_SIDE);
        case 2:
            return CLIP_LARGER_THAN_XY;
        case 3:
            return (float) (CLIP_LARGER_THAN_XY - ((CLIP_LARGER_THAN_XY*(nPoint % POINTS_PER_SIDE)) / POINTS_PER_SIDE));
        default:
            return CLIP_LARGER_THAN_XY;
    }

    return 0.0f;
}
