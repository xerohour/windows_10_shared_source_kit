#define D3D_OVERLOADS
#include <d3d.h>
#include "Address.h"

// Global Test and App instance
CD3DWindowFramework			App;
CWrapUTest					WrapU;
CWrapVTest					WrapV;
CWrapUVTest					WrapUV;
CWrapTest					Wrap;
CMirrorTest					Mirror;
CClampTest					Clamp;
CBorderTest					Border;
CMirrorOnceTest				MirrorOnce;
//CHalfBorderTest				HalfBorder;
//CHalfBorderPointTest		HalfBorderPoint;
CWrapMirrorTest				WrapMirror;
CWrapClampTest				WrapClamp;
CWrapBorderTest				WrapBorder;
CWrapMirrorOnceTest			WrapMirrorOnce;
//CWrapHalfBorderTest			WrapHalfBorder;
CMirrorWrapTest				MirrorWrap;
CMirrorClampTest			MirrorClamp;
CMirrorBorderTest			MirrorBorder;
CMirrorMirrorOnceTest		MirrorMirrorOnce;
//CMirrorHalfBorderTest		MirrorHalfBorder;
CClampWrapTest				ClampWrap;
CClampMirrorTest			ClampMirror;
CClampBorderTest			ClampBorder;
CClampMirrorOnceTest		ClampMirrorOnce;
//CClampHalfBorderTest		ClampHalfBorder;
CBorderWrapTest				BorderWrap;
CBorderMirrorTest			BorderMirror;
CBorderClampTest			BorderClamp;
CBorderMirrorOnceTest		BorderMirrorOnce;
//CBorderHalfBorderTest		BorderHalfBorder;
CMirrorOnceWrapTest			MirrorOnceWrap;
CMirrorOnceClampTest		MirrorOnceClamp;
CMirrorOnceMirrorTest		MirrorOnceMirror;
CMirrorOnceBorderTest		MirrorOnceBorder;
//CMirrorOnceHalfBorderTest	MirrorOnceHalfBorder;
//CHalfBorderWrapTest			HalfBorderWrap;
//CHalfBorderMirrorTest		HalfBorderMirror;
//CHalfBorderClampTest		HalfBorderClamp;
//CHalfBorderBorderTest		HalfBorderBorder;
//CHalfBorderMirrorOnceTest	HalfBorderMirrorOnce;

CAddressTest::CAddressTest()
{
    // Init framework options
    SetTestType(TESTTYPE_CONF);
#if FAST_TEST
    // Add some more space to fit all test cases in one back buffer
    m_Options.D3DOptions.nBackBufferWidth = (DWORD) 320;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD) 512;
#else //FAST_TEST
    m_Options.D3DOptions.nBackBufferWidth = (DWORD) 160;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD) 140;
#endif //FAST_TEST

	// Force test to use relevant DirectX versions
	m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81 | DXVERSION_7 | DXVERSION_6;
    
    // Init variables
    pTexture = NULL;
	bBorder = false;
	bWrapUV = false;
	bIndependentUV = false;

	// Init the AddrU & AddrV strings to empty
	szAddrU = "";
	szAddrV = "";
}

CAddressTest::~CAddressTest()
{
}

bool CAddressTest::CapsCheck(void)
{
	// Check the caps we are interested in
	DWORD dwTextureAddressCaps = m_pSrcDevice->GetCaps()->dwTextureAddressCaps;

	// Check the appropriate Address caps
	if (bIndependentUV)
	{
		if (!(dwTextureAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV))
		{
			WriteToLog("Device capability not supported: Texture Address IndependentUV.\n");
			return false;
		}

		if (!(dwTextureAddressCaps & dwAddressCapU))
		{
			WriteToLog("Device capability not supported: Texture Address %s.\n",szAddrU);
			return false;
		}

		if (!(dwTextureAddressCaps & dwAddressCapV))
		{
			WriteToLog("Device capability not supported: Texture Address %s.\n",szAddrV);
			return false;
		}
	}
	else if (!(dwTextureAddressCaps & dwAddressCapU) && !(dwTextureAddressCaps & dwAddressCapV))
	{
		WriteToLog("Device capability not supported: %s.\n",m_szTestName);
        return false;
	}

	return true;
}

bool CAddressTest::Setup(void)
{
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
        return false;
	}

	// Set the palette if needed
	if (pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

    // Load a gradient image into the texture
    if (!pTexture->LoadGradient(1,64,64,Gradient))
	{
		WriteToLog("LoadGradient() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    // Set the valid texture
	if (!SetTexture(0,pTexture))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

	return true;
}

UINT CAddressTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#if FAST_TEST
    // Setup the Test range
        SetTestRange(1, 1);
#else //FAST_TEST
    // Setup the Test range
	if (bWrapUV)
		SetTestRange(1, 4);
	else
		SetTestRange(1, MAX_TESTS);
#endif //FAST_TEST

	// On Dx9c, need to check for UsageQueryWrapAndMip for non-clamp address modes
	if ((m_pD3D->GetRuntimeVersion() >= 9.029f) && (!bIndependentUV) && (dwAddressCapU != D3DPTADDRESSCAPS_CLAMP))
	{
		HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
											 USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat]);
		if (FAILED(Result))
		{
			WriteToLog("Unable to use current address mode on this format. CheckDeviceFormat(WrapAndMip) failed: %s.\n",m_pD3D->HResultToString(Result));
			return D3DTESTINIT_SKIPALL;
		}
	}

    return D3DTESTINIT_RUN;
}

bool CAddressTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);
}

bool CAddressTest::ExecuteTest(UINT uTestNum)
{
    // Initialize some variables
    TCHAR szBuffer[80];
    dwVertexCount = 0;
    szStatus[0] = '\0';
    msgString[0] = '\0';

#if FAST_TEST
    // Let's build some triangles.
    {
        BeginScene();
        int testIndex = uTestNum;

        const DWORD dwWidth = m_pCurrentMode->nBackBufferWidth;
        const DWORD dwHeight = m_pCurrentMode->nBackBufferHeight;
        UINT numColumns = (bWrapUV) ? 2 : 5;
        UINT numRows = (bWrapUV) ? 2 : 8;
        const float widthPerOp = float(dwWidth) / float(numColumns);
        const float heightPerOp = float(dwHeight) / float(numRows);

        float scaleX = widthPerOp / float(dwWidth);
        float scaleY = widthPerOp / float(dwHeight);

        UINT numTests = (bWrapUV) ? 4 : MAX_TESTS;
        for (size_t i = 0; i < numTests; i++)
        {
            // Per Test initialization
            {
                dwVertexCount = 0;
            }
            (bWrapUV) ? DrawWrapGrid(testIndex) : DrawGrid(testIndex);

            float transX = (i % numColumns) * widthPerOp;
            float transY = (i / numColumns) * heightPerOp;

            // Scale and translate the quad so they all fit on the screen
            for (size_t i = 0; i < 6; i++)
            {
                VertexList[i].sx *= scaleX;
                VertexList[i].sx += transX;

                VertexList[i].sy *= scaleY;
                VertexList[i].sy += transY;
            }

            // Render the data.
            RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, VertexList, dwVertexCount, NULL, 0, 0);
            testIndex++;
        }

        EndScene();
    }
#else // FAST_TEST
	// Let's build some triangles.
	if (bWrapUV)
		DrawWrapGrid(uTestNum);
	else
		DrawGrid(uTestNum);
#endif // FAST_TEST

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

void CAddressTest::SceneRefresh()
{
#ifndef FAST_TEST
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
#endif //FAST_TEST
}

void CAddressTest::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what load method we are using
	WriteStatus("$wTexture Load",szStatus);
}

void CAddressTest::Cleanup(void)
{
	// Cleanup texture & image data
	SetTexture(0, NULL);
    RELEASE(pTexture);
}

#if FAST_TEST
bool CAddressTest::AddFormatModes(DISPLAYMODE *pMode, bool bWindowed)
{
    UINT uTexture = 0;
    UINT uZBuffer = 0;
    UINT uCube = 0;
    UINT uVolume = 0;
    UINT uMS = 0;
    UINT uBB = 0;

    // Convert the formats into indexes into the common lists
    if (!GetFormatIndex(m_Options.ModeOptions.TextureFmt, m_pCommonTextureFormats, &uTexture) ||
        !GetZBufferFormatIndex(m_Options.ModeOptions.ZBufferFmt, m_pCommonZBufferFormats, &(pMode->Format), &uZBuffer) ||
        !GetFormatIndex(m_Options.ModeOptions.CubeFmt, m_pCommonCubeFormats, &uCube) ||
        !GetFormatIndex(m_Options.ModeOptions.VolumeFmt, m_pCommonVolumeFormats, &uVolume) ||
        !GetFormatIndex(m_Options.ModeOptions.MultisampType, m_pCommonMultiSampleFormats, m_uCommonMultiSampleFormats, &uMS) ||
        !GetBackBufferFormatIndex(m_Options.ModeOptions.BackBufferFmt, m_pCommonBackBufferFormats, &(pMode->Format), &uBB) ||
        // We need to validate MS here.  If the device doens't support this MS type, we don't want to add it as a mode
        (m_pCommonMultiSampleFormats && m_pCommonZBufferFormats &&
        (FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)) ||
        FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[uZBuffer], bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)))))
    {
        DPF(2, _T("Requested fmt not supported by device.\n"));
        return false;
    }

    // If only one mode was requested, add it and return
    if (m_Options.ModeOptions.TextureFmt != FMT_ALL &&
        m_Options.ModeOptions.ZBufferFmt != FMT_ALL &&
        m_Options.ModeOptions.CubeFmt != FMT_ALL &&
        m_Options.ModeOptions.VolumeFmt != FMT_ALL &&
        m_Options.ModeOptions.BackBufferFmt != FMT_ALL &&
        m_Options.ModeOptions.MultisampType != MULTISAMPLE_ALL)
    {
        return AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB);
    }

    if (m_Options.ModeOptions.BackBufferFmt == FMT_ALL)
    {
        for (UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
        {
            // Filter out unsupported BB modes based on ModeOptions
            if (m_pD3D->GetVersion() < 8.f)
            {
                if (pMode->Format.d3dfFormat != m_pCommonBackBufferFormats[nBB].d3dfFormat)
                    continue;
            }
            else if (FAILED(m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format,
                m_pCommonBackBufferFormats[nBB], bWindowed)))
                continue;

            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, nBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.TextureFmt == FMT_ALL)
    {
        for (UINT nTexture = 0; nTexture < m_uCommonTextureFormats; nTexture++)
        {
            if (m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A8R8G8B8)
            {
                if (!AddMode(pMode, bWindowed, nTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
                break;
            }
        }
    }

    //static const FMT formats[] = { FMT_A8R8G8B8, FMT_DXT1 };
    //
    //for (FMT fmt : formats)
    //{
    //    if (!AddMode(pMode, bWindowed, fmt, uZBuffer, uCube, uVolume, uMS, 0, uBB))
    //    {
    //        DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
    //        return false;
    //    }
    //}

    if (m_Options.ModeOptions.ZBufferFmt == FMT_ALL)
    {
        for (UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, nZBuffer, uCube, uVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.CubeFmt == FMT_ALL)
    {
        for (UINT nCube = 0; nCube < m_uCommonCubeFormats; nCube++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, nCube, uVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
    {
        for (UINT nVolume = 0; nVolume < m_uCommonVolumeFormats; nVolume++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, nVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.MultisampType == FMT_ALL)
    {
        DWORD dwMSQuality = 0;

        for (UINT nMS = 0; nMS < m_uCommonMultiSampleFormats; nMS++)
        {
            int nValidBB = -1, nValidZBuffer = -1;

            // For each MS type, find a BB and ZBuffer that support that type
            for (UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
            for (UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
            {
                // If specific modes are requested, only allow those modes to be matched.
                if (m_Options.ModeOptions.BackBufferFmt != FMT_ALL && m_Options.ModeOptions.BackBufferFmt != FMT_ANY &&
                    nBB != uBB)
                    continue;
                if (m_Options.ModeOptions.ZBufferFmt != FMT_ALL && m_Options.ModeOptions.ZBufferFmt != FMT_ANY &&
                    nZBuffer != uZBuffer)
                    continue;

                // Filter out unsupported BB modes based on ModeOptions
                if (SUCCEEDED(m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, m_pCommonBackBufferFormats[nBB], bWindowed)) &&
                    SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonBackBufferFormats[nBB], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)) &&
                    SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[nZBuffer], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)))
                {
                    nValidBB = nBB;
                    nValidZBuffer = nZBuffer;
                    break;
                }
            }

            if (nValidBB == -1 || nValidZBuffer == -1)
                continue;

            for (UINT nMSQ = 0; nMSQ < dwMSQuality; nMSQ++)
            {
                if (!AddMode(pMode, bWindowed, uTexture, nValidZBuffer, uCube, uVolume, nMS, nMSQ, nValidBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

    return true;
}
#endif //FAST_TEST