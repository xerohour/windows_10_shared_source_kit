//*************************************************************************************************
// File: Filter.cpp
// 
// Copyright (C) 1994-2004 Microsoft Corporation.  All Rights Reserved.
//
// Author:      Cliff Owen (cliffo)
//*************************************************************************************************
#define D3D_OVERLOADS
#include <d3d9.h>
#include "Filter.h"

// Global Test and App instance
CD3DWindowFramework		App;

//*************************************************************************************************
// CFilterTest: Main class description
//
// Constructor
//
//  Author          Date            History
//   cliffo         4-09-04         Implementation
//*************************************************************************************************

CFilterTest::CFilterTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);

	// Force test to use relevant DirectX versions for WHQL

	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81;

    // Texture formats this test cannot do
    
    if (KeySet("YUV"))    
        m_Options.ModeOptions.dwTexInclude = PF_YUV;
    else
	    m_Options.ModeOptions.dwTexExclude = PF_ALPHA | PF_METN | PF_ALPHAPREMULT | PF_PALETTEINDEXED8 | PF_PALETTEINDEXED4 | PF_PALETTEINDEXEDTO8 | PF_YUV;
	    
	m_Options.ModeOptions.TextureFmt = FMT_ALL;

    // Init texture variable
    m_pTexture = NULL;
    
    m_pFilterList = NULL;
    
    // New test requires 95% conformance
    
    m_pFramework->GetImageCmp()->SetPassRequired(0.95f);
}

//*************************************************************************************************
// Destructor
//
//  Author          Date            History
//   cliffo         4-09-04         Implementation
//*************************************************************************************************

CFilterTest::~CFilterTest()
{
    if (m_pFilterList != NULL)
        delete m_pFilterList;
}

//*************************************************************************************************
// SetFilterList
//
// Provide a list of mesh sizes, texture size to use, and min/mag filter types to the test. This
// is then stored in an allocated buffer.
//
//  Author          Date            History
//   cliffo         4-09-04         Implementation
//*************************************************************************************************
#if FAST_TEST
void CFilterTest::SetFilterList(PFILTERLIST pList, UINT uCount)
{
    static const UINT testCount = 3;
    m_pFilterList = new FILTERLIST[testCount];


    m_uFilterCount = min(testCount, uCount);

    m_pFilterList[0] = pList[0];
    if (m_uFilterCount >= 1)
    m_pFilterList[1] = pList[uCount/2];
    if (m_uFilterCount >= 2)
    m_pFilterList[2] = pList[uCount - 1];
}
#else // FAST_TEST
void CFilterTest::SetFilterList(PFILTERLIST pList, UINT uCount)
{
	m_uFilterCount = uCount;

    m_pFilterList = new FILTERLIST[m_uFilterCount];
    
    memcpy(m_pFilterList,pList,sizeof(FILTERLIST) * uCount);
}
#endif //FAST_TEST

//*************************************************************************************************
// CapsCheck
//
// Provide the basic checking required to determine if the test is valid on this device, on this
// version of Direct3D.
//
//  Author          Date            History
//   cliffo         4-09-04         Implementation
//*************************************************************************************************

bool CFilterTest::CapsCheck(void)
{
	DWORD dwTextureFilterCaps = m_pSrcDevice->GetCaps()->dwTextureFilterCaps;
	
	// Set the test range here so the proper skip count is reported.
	
	SetTestRange(1, m_uFilterCount);
	
	// Check the filters that are going to be set for the length of this test
	// pass. NOTE: Need to check more than the 1st set of filters because
	// filters may change... especially for the mixed cases.
	
	bool bCheckDevice = false;
	
	for (UINT uCount=0; uCount<m_uFilterCount; ++uCount)
	{
        //*****************************************************************
        // Validate the minification states
        //*****************************************************************

        switch(m_pFilterList[uCount].dwMinState)
        {
            case    TEXF_POINT:
                    {
                        if (0 == (dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))
                        {
                            WriteToLog("Min Point filtering flag not set. Flag required for this test.");
                            return  false;
                        }
                    }
                    break;

            case    TEXF_LINEAR:
                    {
                        if (0 == (dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))
                        {
                            WriteToLog("Min Linear filtering flag not set. Flag required for this test.");
                            return  false;
                        }
                            
                        bCheckDevice = true;
                    }
                    break;
                    
            case    TEXF_PYRAMIDALQUAD:
                    {
                        if (0 == (dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPYRAMIDALQUAD))
                        {
                            WriteToLog("Min Pyramidal Quad filtering flag not set. Flag required for this test.");
                            return  false;
                        }
                            
                        bCheckDevice = true;
                    }
                    break;
                    
            case    TEXF_GAUSSIANQUAD:
                    {
                        if (0 == (dwTextureFilterCaps & D3DPTFILTERCAPS_MINFGAUSSIANQUAD))
                        {
                            WriteToLog("Min Gaussian Quad filtering flag not set. Flag required for this test.");
                            return  false;
                        }
                            
                        bCheckDevice = true;
                    }
                    break;
        }

        //*****************************************************************
        // Validate the magnification states
        //*****************************************************************
            
        switch(m_pFilterList[uCount].dwMagState)
        {
            case    TEXF_POINT:
                    {
                        if (0 == (dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))
                        {
                            WriteToLog("Mag Point filtering flag not set. Flag required for this test.");
                            return  false;
                        }
                    }
                    break;
                    
            case    TEXF_LINEAR:
                    {
                        if (0 == (dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))
                        {
                            WriteToLog("Mag Linear filtering flag not set. Flag required for this test.");
                            return  false;
                        }
                            
                        bCheckDevice = true;
                    }
                    break;            
                    
            case    TEXF_PYRAMIDALQUAD:
                    {
                        if (0 == (dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD))
                        {
                            WriteToLog("Mag Pyramidal Quad filtering flag not set. Flag required for this test.");
                            return  false;
                        }
                            
                        bCheckDevice = true;
                    }
                    break;

            case    TEXF_GAUSSIANQUAD:
                    {
                        if (0 == (dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFGAUSSIANQUAD))
                        {
                            WriteToLog("Mag Gaussian Quad filtering flag not set. Flag required for this test.");
                            return  false;
                        }

                        bCheckDevice = true;
                    }
                    break;
        }
    }

	// If running on DX 9 or higher, CheckDeviceFormat can tell us if the requested state
	// is (or should be) valid with the texture format that will be used.

	if( m_dwVersion >= 0x900 && bCheckDevice)
	{
        // If not doing point filtering, ask to see if it's ok.
        
        LRESULT lResult = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), 
                                                    m_pSrcDevice->GetDevType(),
                                                    m_pCurrentMode->pDisplay->Format,
											        USAGE_QUERY_FILTER, 
											        RTYPE_TEXTURE, 
											        m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat]);

	    if (FAILED(lResult))
        {
            // This texture format doesn't support filtering with something other than point.
            
            WriteToLog("This texture format only supports point filtering.");

            return  false;
        }
    }
    
    return true;
}

//*************************************************************************************************
// TestInitialize
//
// Initialize the test to prepare it to run.
//
//  Author          Date            History
//   cliffo         4-09-04         Implementation
//*************************************************************************************************

UINT CFilterTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}
        
    // Setup the Test range based on the number of filters set by the
    // test.
    
    SetTestRange(1, m_uFilterCount);

	return D3DTESTINIT_RUN;
}

//*************************************************************************************************
// ExecuteTest
//
// Execute 1 test using the filter list previously provided by the test.
//
//  Author          Date            History
//   cliffo         4-09-04         Implementation
//*************************************************************************************************

bool CFilterTest::ExecuteTest(UINT uTestNum)
{
    FORMAT        * pFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
	TCHAR			msgString[80];
	bool            bResult;
	UINT            uIndex = uTestNum-1;

	// Cleanup previous texture
	SetTexture(0, NULL);
    RELEASE(m_pTexture);

    //*****************************************************************
    // Build the test friendly strings
    //*****************************************************************

    BuildStatusString(uIndex);

	sprintf(msgString, "Mesh: %dx%d",m_pFilterList[uIndex].uMeshWidth,m_pFilterList[uIndex].uMeshHeight);

	// Assign Width & Height based on test case

    sprintf(msgString, "%s Tex: %dx%d",msgString,m_pFilterList[uIndex].uTextureSize,m_pFilterList[uIndex].uTextureSize);

	//*****************************************************************
	// Tell the log that we are starting
	//*****************************************************************
	
	BeginTestCase(msgString,uTestNum);

    //*****************************************************************
    // Texturing Setup
    //*****************************************************************

    // Create the texture
    bResult = CreateTexture(m_pFilterList[uIndex].uTextureSize,m_pFilterList[uIndex].uTextureSize,1,FMWK_DEFAULT,*pFormat,POOL_MANAGED,&m_pTexture);
    
	// Tell log about CreateTexture failure
	if (!bResult || (NULL == m_pTexture))
	{
		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

	// Load image to sample into the texture.

	LPTSTR szTexture = "Grid128.bmp";

    if (m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == D3DFMT_L8 ||
        m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == D3DFMT_A8L8 ||
        m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == D3DFMT_A4L4 ||
        m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == D3DFMT_L16)
    {
        szTexture = "Grid128bw.bmp";
        
	    if (m_pFilterList[uIndex].uTextureSize == 64)
	        szTexture = "Grid64BW.bmp";
	    else
        if (m_pFilterList[uIndex].uTextureSize == 32)
            szTexture = "Grid32BW.bmp";    
    }
    else
    {
	    if (m_pFilterList[uIndex].uTextureSize == 64)
	        szTexture = "Grid64.bmp";
	    else
        if (m_pFilterList[uIndex].uTextureSize == 32)
            szTexture = "Grid32.bmp";
    }
    
    if (!m_pTexture->LoadTexture(1,szTexture))
    {
        Fail();
        return false;
    }

    // Set the valid texture
	if (!SetTexture(0,m_pTexture))
	{
		WriteToLog("SetTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
		return false;
	}

    //*****************************************************************
    // Prepare for rendering
    //*****************************************************************
    
    // Construct the primitive to be used to render the mesh

	float   fScale = 1.0f;
    float   fPos = 9.5f;
    float   fZ = 0.9f;
    float   fRHW = 0.05f;
	float   fWidth = fPos + m_pFilterList[uIndex].uMeshWidth;
	float   fHeight = fPos + m_pFilterList[uIndex].uMeshHeight;
	
	// Bias the position according to the texture size and required texture precision
	
	// Adjust the texture coordinates to the accepted precision amount, which is 6 bits of
	// sub-texel precision required. 1/2^6 = 1/64. 6 bits is clearly spec'ed in the WGF spec,
	// but it's also the amount that RefRast uses. By forcing a clamping to 6 bits, it ensures
	// that any part with at least 6 bits of sub-texel precision is going to pass anything
	// that we throw at it. This is why the conformance value is 95%.
	
	float fPrecTexMin = (1.0f / 64.0f) / (float)m_pFilterList[uIndex].uTextureSize;
	float fPrecTexMax = 1.0f - fPrecTexMin;
	
	fPrecTexMin = 0.0f;
	
    TLVERTEX  Primitive[] =   { { fPos,     fPos,   fZ, fRHW, 0xFFFFFFFF, fPrecTexMin, fPrecTexMin},
                                { fWidth,   fHeight,fZ, fRHW, 0xFFFFFFFF, fPrecTexMax, fPrecTexMax},
                                { fPos,     fHeight,fZ, fRHW, 0xFFFFFFFF, fPrecTexMin, fPrecTexMax},
                                { fPos,     fPos,   fZ, fRHW, 0xFFFFFFFF, fPrecTexMin, fPrecTexMin},
                                { fWidth,   fPos,   fZ, fRHW, 0xFFFFFFFF, fPrecTexMax, fPrecTexMin},
                                { fWidth,   fHeight,fZ, fRHW, 0xFFFFFFFF, fPrecTexMax, fPrecTexMax}};
	
	// Set the sampler state
	
	if (m_pFilterList[uIndex].dwMinState != TEXF_NONE)
	    SetSamplerState(0,SAMP_MINFILTER,m_pFilterList[uIndex].dwMinState);
	    
	if (m_pFilterList[uIndex].dwMagState != TEXF_NONE)
	    SetSamplerState(0,SAMP_MAGFILTER,m_pFilterList[uIndex].dwMagState);


	// Point sampled filtering needs to use 85% conformance to allow for different texture coordinate rounding modes

	if ((m_pFilterList[uIndex].dwMinState == TEXF_POINT) || (m_pFilterList[uIndex].dwMagState == TEXF_POINT))
		m_pFramework->GetImageCmp()->SetPassRequired(0.85f);
	else
		m_pFramework->GetImageCmp()->SetPassRequired(0.95f);

    //*****************************************************************
    // Render the scene
    //*****************************************************************

    DWORD dwPass;
    
    LRESULT lResult = m_pSrcDevice->ValidateDevice(&dwPass);
    
    if (FAILED(lResult))
    {
        WriteToLog("Device doesn't support this texture format with desired operation (%s)",m_pD3D->HResultToString(GetLastError()));
        SkipTests(GetNumTests());   // Skip this entire test range
        return true;
    }
    
    Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,0,255), 1.0f, 0);

    // Invoke the test specific setup

    TestSpecificSetup(uTestNum);
    
    // Finally, render the scene

	if (BeginScene())
	{
		// Render the data.
		
		SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		
		DrawPrimitiveUP(D3DPT_TRIANGLELIST,(sizeof(Primitive)/sizeof(TLVERTEX)) / 3,Primitive,sizeof(TLVERTEX));
		
		EndScene();
	}
	
	TestSpecificCleanup();

    return true;
}

//*************************************************************************************************
// UpdateStatus
//
// Update the text on the status window to indicate this sampler states are being used.
//
//  Author          Date            History
//   cliffo         4-09-04         Implementation
//*************************************************************************************************

void CFilterTest::UpdateStatus()
{
    CD3DTest::UpdateStatus();
    WriteStatus("Samp State:",m_szStatusString);
}

//*************************************************************************************************
// TestTerminate
//
// Cleanup the test
//
//  Author          Date            History
//   cliffo         4-09-04         Implementation
//*************************************************************************************************

bool CFilterTest::TestTerminate()
{
	// Cleanup texture
	
	SetTexture(0, NULL);
    RELEASE(m_pTexture);

	// Reset back to 95% conformance when exiting so we don't affect subsequent groups

	m_pFramework->GetImageCmp()->SetPassRequired(0.95f);
    
	return true;
}

//*************************************************************************************************
// BuildStatusString
//
// Construct the user friendly status string that will inform the user of the present sampler
// states being used with the test.
//
//  Author          Date            History
//   cliffo         4-09-04         Implementation
//*************************************************************************************************

void CFilterTest::BuildStatusString(UINT uIndex)
{
    m_szStatusString[0] = 0;
    
    // Build the minify state first
    
    switch(m_pFilterList[uIndex].dwMinState)
    {
        case    TEXF_POINT:
                lstrcpy(m_szStatusString,"$aMin: $wPoint  ");
                break;                
        case    TEXF_LINEAR:
                lstrcpy(m_szStatusString,"$aMin: $wLinear  ");
                break;
        case    TEXF_PYRAMIDALQUAD:
                lstrcpy(m_szStatusString,"$aMin: $wPyramidal  ");
                break;
        case    TEXF_GAUSSIANQUAD:
                lstrcpy(m_szStatusString,"$aMin: $wGaussian  ");
                break;                
    }
    
    // Build the magnify state second

    switch(m_pFilterList[uIndex].dwMagState)
    {
        case    TEXF_POINT:
                lstrcat(m_szStatusString,"$aMag: $wPoint");
                break;
        case    TEXF_LINEAR:
                lstrcat(m_szStatusString,"$aMag: $wLinear");
                break;
        case    TEXF_PYRAMIDALQUAD:
                lstrcat(m_szStatusString,"$aMag: $wPyramidal");
                break;
        case    TEXF_GAUSSIANQUAD:
                lstrcat(m_szStatusString,"$aMag: $wGaussian");
                break;
    }
}

#if FAST_TEST
bool CFilterTest::AddFormatModes(DISPLAYMODE *pMode, bool bWindowed)
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
            if (m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A8R8G8B8 || m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_X4R4G4B4
                || m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_DXT1 || m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A2B10G10R10)
            {
                if (!AddMode(pMode, bWindowed, nTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

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