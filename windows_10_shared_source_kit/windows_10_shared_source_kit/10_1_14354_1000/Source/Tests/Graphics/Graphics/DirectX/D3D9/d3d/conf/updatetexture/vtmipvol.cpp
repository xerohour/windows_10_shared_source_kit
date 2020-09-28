#include <d3d9.h>
#include "VTVolume.h"
#include "VTMatrix.h"

#include "UtilFormats.h"

CMipVolTest::CMipVolTest()
{
	m_szTestName = _T("UpdateTexture Mip-Volume Tests");
	m_szCommandKey = _T("Volume");
	(m_Options.ModeOptions).dwTexExclude = PF_YUV;
	(m_Options.ModeOptions).TextureFmt = FMT_ALL;
}

CMipVolTest::~CMipVolTest()
{
}

// AndrewLu; 2001/09/26; Auxiliary function: used to look for "-RetailTestCasesOnly".
bool CMipVolTest::IsSubString(LPTSTR szSearchString, LPTSTR szSourceString)
{
	if (*szSourceString)
	{
		unsigned int uiSearchLength = strlen(szSearchString);

		do
		{
                if (!_strnicmp(szSearchString, szSourceString, uiSearchLength))
				return true;

			++szSourceString;
		}
		while (*szSourceString);
	}

	return false;
}

bool CMipVolTest::CapsCheck()
{
	DEVICE_CREATION_PARAMETERS dcp;
	DISPLAYMODE Mode;

	m_pSrcDevice -> GetCreationParameters(&dcp);
	m_pD3D -> GetAdapterDisplayMode(dcp.AdapterOrdinal, &Mode);

	//if (GetFormatRenderCategory(GetCurrentTextureFormat() -> d3dfFormat) == rcMultiElement)
	//	return false;

	if (SUCCEEDED(m_pD3D -> CheckDeviceFormat(dcp.AdapterOrdinal, dcp.DeviceType, Mode.Format, 0, RTYPE_VOLUMETEXTURE, *(GetCurrentTextureFormat()))))
	if (SUCCEEDED(m_pD3D -> CheckDeviceFormat(dcp.AdapterOrdinal, dcp.DeviceType, Mode.Format, USAGE_QUERY_WRAPANDMIP, RTYPE_VOLUMETEXTURE, *(GetCurrentTextureFormat()))))
		return true;
	return false;
}

UINT CMipVolTest::TestInitialize(void)
{
    int i;
    bool bFormats = true;

	HRESULT hresult;

	// Determine the format for this set of test cases, as determined by -tex or
	// the framework's format cycling.
	formatCurrent = *(GetCurrentTextureFormat());

	pVolTexture=NULL;
	pSysTex=NULL;
	pBogusTex=NULL;

	// AndrewLu:
	CAPS VTSrcDeviceCaps;

 	// Make sure we are on at lease DX8
	if (m_dwVersion <= 0x0700)
	{
		WriteToLog("This test requires at least Dx8.\n");
		SetTestRange(1, 1);
		return D3DTESTINIT_SKIPALL;
	}

	hresult = m_pSrcDevice -> GetDeviceCaps(&VTSrcDeviceCaps);
	DWORD dwTextureCaps = VTSrcDeviceCaps.TextureCaps;
	DWORD dwRasterCaps = VTSrcDeviceCaps.RasterCaps;
	DWORD dwVolumeTextureFilterCaps = VTSrcDeviceCaps.VolumeTextureFilterCaps;

	if (!(dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP))
	{
		WriteToLog("Device capability not supported: Texture VolumeMap.\n");
		SetTestRange(1, 1);
		return D3DTESTINIT_SKIPALL;
	}

    // Check if MipVolumes are available and if the device support mipmaplodbias
	if (!(dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP) || !(dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS )) 
	{
		WriteToLog("Device does not supported Texture MipVolumeMap.\n");
		m_bMipVolTex=FALSE;
		m_nTotalLevels=1;
	}
	else
	{
		m_bMipVolTex=TRUE;
		m_nTotalLevels=VTGENERICTEXLEVEL;
	}

	// Setup the Test range 
	SetTestRange(1, VTTOTALCASES * m_nTotalLevels * 3);

    // Check if any volume formats were available
	if (!bFormats) 
    {
		WriteToLog("No volume texture formats found.\n");
		SetTestRange(1, 1);
		return D3DTESTINIT_SKIPALL;
    }

    // Load sphere vertices
	LoadSphere();
	m_pFramework -> GetImageCmp() -> SetPassRequired(0.85f);

	return D3DTESTINIT_RUN;
}

bool CMipVolTest::ExecuteTest(UINT uTestNum)
{

	CMatrix Matrix;
	msgString[0] = '\0';
	szStatus[0] = '\0';
	int nCaseNum;
	int	nFormat;
	float  fLOD;
	DWORD dwWidth, dwHeight, dwDepth, dwMipLevels;
    D3DVECTOR ZVector = {0.0f,0.0f,3.0f};
	PVTTESTCASE	pTC;
	HRESULT		hr,h1,h2;
	bool		bOperationResult;		// AndrewLu
	BOOL		bRet;
	float		fAngle;

	// AndrewLu
	UINT uiFormatIndex;

	fAngle					= (float)(((uTestNum - 1) % 3) * 120);
	nCaseNum				=((uTestNum-1)/3)%VTTOTALCASES;
	fLOD					= (float)((uTestNum - 1) / (VTTOTALCASES * 3));

	pTC=(PVTTESTCASE)(&(tc[nCaseNum]));

	// Cleanup previous texture
	SetTexture(0, NULL);
	RELEASE(pVolTexture);
	RELEASE (pSysTex);
	RELEASE (pBogusTex);


    //sprintf(msgString,"%sTex:%s,%s,LOD=%1.0f,Angle=%d",msgString,(hackformats[uiFormatIndex]).szFMTName,pTC->szTitle,fLOD,(int)(fAngle));
    sprintf(msgString,"VolumeTex: %s, %d, LOD=%1.0f,Angle=%d", pTC->szTitle, (UINT)(formatCurrent.d3dfFormat), fLOD,(int)(fAngle));

	// Tell the log that we are starting
	//::SetWindowText (vt_g_pD3DFramework->m_hWindow, msgString);
	BeginTestCase(msgString, uTestNum);

	// skip the mip vol texture test if the device doesn't support mipvoltex
	if (!m_bMipVolTex &&(pTC->uSrcLevels!=0 || pTC->uDestLevels !=0 ))
	{
		Skip();
		return false;
	}

	if (!(formatCurrent.ddpfPixelFormat.dwRGBBitCount))
	{
		Skip();
		return false;
	}

	// AndrewLu; 2001/09/26; If the test is being executed as part of the DCT suite, skip the
	// test case labelled "Verify failure when bottom level texture's size doesn't match".
	if ((nCaseNum == 10) && IsSubString(TEXT("-RetailTestCasesOnly"), GetCommandLine()))
	{
		Skip();
		return false;
	}

	// Create the mipmap volume texture in sys memory

	VTCreateVolumeTexture(pTC->uSrcSize, pTC->uSrcSize, pTC->uSrcSize, pTC->uSrcLevels, 0/*Hard code usage 0, for now*/, formatCurrent, POOL_SYSTEMMEM, &pSysTex, true);

	// Create the mipmap volume texture
	VTCreateVolumeTexture(pTC->uDestSize, pTC->uDestSize, pTC->uDestSize, pTC->uDestLevels, 0/*Hard code usage 0, for now*/, formatCurrent, POOL_DEFAULT, &pVolTexture, true);

	// Create the bogus vol texture1.
	VTCreateVolumeTexture(pTC->uSrcSize, pTC->uSrcSize, pTC->uSrcSize, pTC->uSrcLevels, 0/*Hard code usage 0, for now*/, formatCurrent, POOL_DEFAULT, &pBogusTex, true);

	// Tell log about CreateVolumeTexture failure
	if (NULL == pVolTexture || NULL == pSysTex || NULL==pBogusTex) 
	{
		//WriteToLog("CreateVolumeTexture() failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());

		// Cleanup current textures.
		SetTexture(0, NULL);
		RELEASE(pVolTexture);
		RELEASE (pSysTex);
		RELEASE (pBogusTex);

		Skip();
		//SkipTestRange(uTestNum, uTestNum);
        //UpdateStatus(false);
		return false;
	}

    if (pVolTexture->IsPalettized())
        pVolTexture->SetPalette(NULL);

	// make the source tex clean

	try
	{
		// UpdateTexture
		bOperationResult = UpdateTexture(pSysTex, pBogusTex);

		if (!bOperationResult)
		{
//			WriteToLog ("UpdateTexture failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
			UpdateStatus (false);
			return true;
		}
	}
	catch (...)
	{
		//WriteToLog ("UpdateTexture causes exception with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
		UpdateStatus (false);
		return true;
	}

	m_bSkipInTheMiddle=FALSE;

	//Run The PreFunc() against the textures on the "src" and "ref" devices.
	// AndrewLu: Since the test now uses LPVOLUMETEXTURES, etc., one call should be sufficient.
	m_pSrcTex = pSysTex;
	m_pDestTex = pVolTexture;
	bRet=(this->*(pTC->PreUpdateFunc))();

	if (m_bSkipInTheMiddle)
	{
		Skip();
		return false;
	}

	if (!bRet)
	{
		WriteToLog("PreUpdateFunc() failed ");
		UpdateStatus (false);
		return true;
	}

	try
	{
		// UpdateTexture
		bOperationResult = UpdateTexture(pSysTex, pVolTexture);

		if (pTC->bExpectSucceed && (!bOperationResult) /*(FAILED(h1)||FAILED(h2))*/)
		{
			//WriteToLog ("UpdateTexture failed with HResult = %s when expect to succeed\n",GetHResultString(GetLastError()).c_str());
			UpdateStatus (false);
			return true;
		}

		if (!pTC->bExpectSucceed && (bOperationResult))
		{
			WriteToLog ("UpdateTexture succeeded when expect to fail\n");
			UpdateStatus (false);
			return true;
		}

	}
	catch (...)
	{
		//WriteToLog ("UpdateTexture causes exception with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
		UpdateStatus (false);
		return true;
	}

	if (m_bMipVolTex)
	{
		UINT uiMipMapLoDBias = (UINT)fLOD;

		//WriteToLog("LOD: %d\n", uiMipMapLoDBias);

		SetSamplerState(0, SAMP_MIPMAPLODBIAS, (DWORD)uiMipMapLoDBias);
		SetSamplerState(0, SAMP_MAXMIPLEVEL, (DWORD)uiMipMapLoDBias);
	}

    // Set the valid texture
	SetTexture(0,pVolTexture);
    SetTextureStageState(0,(D3DTEXTURESTAGESTATETYPE)D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR /* 2004/07/15 */);

    // Setup the world matrix for the translating/rotating sphere
    Matrix.Identity();									
	Matrix.Translate(ZVector);		                       
	Matrix.RotateYaw(fAngle);
	if (!SetTransform(D3DTS_WORLD,&Matrix.m_Matrix))   
	{
        // Tell the logfile about the SetTransform failure
		//WriteToLog("SetTransform(WorldMatrix) failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
        UpdateStatus(false);
        return true;
	}

    return true;
}


bool CMipVolTest::TestTerminate(void)
{
    CVolumeTest::TestTerminate();

	RELEASE(pVolTexture);
	RELEASE (pSysTex);
	RELEASE (pBogusTex);


	return true;
}




//
// Case1 is to Lock with NULL, verify the entire surface has been updated
//
BOOL CMipVolTest::Case1()
{
	BOOL				bRet;

	bRet=LockWithNULL(m_pSrcTex);

	if (!bRet)
	{
		WriteToLog ("Lock failed in Case1\n");
	}
	return bRet;
}

//
// Case2 is to Lock with differnt sub box, totally less than 6
//
BOOL CMipVolTest::Case2()
{
	BOOL					bRet;

	bRet=LockManyAreas(m_pSrcTex,5);

	if (!bRet)
	{
		WriteToLog ("LockManyArea failed in Case2\n");
	}
	return bRet;
}

//
// Case3 is to Lock with differnt sub box, totally more than 6
//
BOOL CMipVolTest::Case3()
{
	BOOL					bRet;

	bRet=LockManyAreas(m_pSrcTex,7);

	if (!bRet)
	{
		WriteToLog ("LockManyArea failed in Case3\n");
	}
	return bRet;
}

//
// Case 4 is to Call AddDirtyXXX (NULL)
//

BOOL CMipVolTest::Case4()
{
	BOOL					bRet;

	bRet=AddDirtyWithNULL(m_pSrcTex);

	if (!bRet)
	{
		WriteToLog ("AddDirtyXXX failed in Case4\n");
	}
	return bRet;

}

//
// Case5 is to call AddDirtyBox with differnt sub boxes, totally <=6
//
BOOL CMipVolTest::Case5()
{
	BOOL					bRet;

	bRet=AddDirtyManyAreas(m_pSrcTex,5);

	if (!bRet)
	{
		WriteToLog ("LockManyArea failed in Case5\n");
	}
	return bRet;

}

//
// Case6 is to call AddDirtyBoxes with differnt sub rect, totally more than 6
//

BOOL CMipVolTest::Case6()
{
	BOOL					bRet;

	bRet=AddDirtyManyAreas(m_pSrcTex,7);

	if (!bRet)
	{
		WriteToLog ("\nAddDirtyManyArea failed in Case6\n");
	}
	return bRet;

}

//
// Case7 is to Lock with D3DLOCK_NO_DIRTY_UPDATE, verify the entire surface has NOT been updated
//

BOOL CMipVolTest::Case7()
{
	BOOL					bRet;

	bRet=LockWithNULL(m_pSrcTex,D3DLOCK_NO_DIRTY_UPDATE );

	if (!bRet)
	{
		WriteToLog ("Lock failed in Case7\n");
	}
	return bRet;

}

//
// Case7_2 is to Lock with D3DLOCK_READONLY, verify the entire surface has NOT been updated
//
BOOL CMipVolTest::Case7_2()
{
	BOOL					bRet;

	bRet=LockWithNULL(m_pSrcTex,D3DLOCK_READONLY );

	if (!bRet)
	{
		WriteToLog ("Lock failed in Case7_2\n");
	}
	return bRet;
}

//
// Case8 is to call UpdateTexture immediately after another UpdateTexture
//
BOOL CMipVolTest::Case8()
{
	BOOL					bRet;
	bool					bOperationResult;	// AndrewLu

	bRet=TRUE;
	try
	{
		bOperationResult = UpdateTexture(pSysTex, pBogusTex);

		if (!bOperationResult)
		{
			WriteToLog ("UpdateTexture fails with m_pBogusTex in Case8\n");
			bRet=FALSE;
		}
	}
	catch (...)
	{
		WriteToLog  ("Unhandled exception when calling UpdateTexture m_pBogusTex\n");
		bRet=FALSE;
	}

	return bRet;

}

//
// Case9 is to make sublevels dirty without touching the top level one, check to see no update
//

BOOL CMipVolTest::Case9()
{
	if (!m_bMipVolTex)
	{
		m_bSkipInTheMiddle=TRUE;
		return TRUE;
	}
	else
		return LockSubLevel(m_pSrcTex);
}


BOOL CMipVolTest::DoNothing()
{
	return TRUE;
}


BOOL CMipVolTest::ModifySysMemBoxes(CnVolTexture * pTex, BOX *pBox)
{
	VOLUME_DESC vd;
	bool bGetLevelDescriptorResult = pTex->GetLevelDesc(0, &vd);
	if (bGetLevelDescriptorResult != true)
		WriteToLog("GetLevelDesc failed in CMipVolTest::ModifySysMemBoxes\n");
	else
		if (vd.Pool == POOL_DEFAULT)
			WriteToLog("CMipVolTest::ModifySysMemBoxes called for a POOL_DEFAULT volume.\n");
		else
		{
			UINT uiLevels;

			if (!(uiLevels = pTex -> GetLevelCount()))
				WriteToLog("CMipVolTest::ModifySysMemBoxes: GetLevelCount returned zero.\n");
			else
			{
				UINT uiBitsPerHorizontalCoordinate = //GetBitsPerHorizontalCoordinate(vd.Format);
				vd.Format.ddpfPixelFormat.dwRGBBitCount;

				if (!(uiBitsPerHorizontalCoordinate & 7))
				{
					BOX boxDest;
					UINT uiLevelIndex = 0;

					if (pBox)
						boxDest = *pBox;
					else
					{
						boxDest.Left = 0;
						boxDest.Top = 0;
						boxDest.Right = vd.Width;
						boxDest.Bottom = vd.Height;
						boxDest.Front = 0;
						boxDest.Back = vd.Depth;
					}

					for (;;)
					{
						VOLUME_DESC vdCurrentLevel;
						BOX boxScaled;

						if (pTex -> GetLevelDesc(uiLevelIndex, &vdCurrentLevel) != true)
							WriteToLog("GetLevelDesc failed in CMipVolTest::ModifySysMemBoxes\n");

						// Scale the box coordinates on the basis of the level index.
						// left, top and front are "rounded down".
						boxScaled.Left = boxDest.Left >> uiLevelIndex;
						boxScaled.Top = boxDest.Top >> uiLevelIndex;
						boxScaled.Front = boxDest.Front >> uiLevelIndex;

						// right, bottom and back are "rounded up".
						boxScaled.Right = (boxDest.Right + ((1 << uiLevelIndex) - 1)) >> uiLevelIndex;
						boxScaled.Bottom = (boxDest.Bottom + ((1 << uiLevelIndex) - 1)) >> uiLevelIndex;
						boxScaled.Back = (boxDest.Back + ((1 << uiLevelIndex) - 1)) >> uiLevelIndex;

						if (IsDXTnFormat((vd.Format).d3dfFormat))
						// Adjust the boxScaled coordinates for DXTn (4x4) alignment.
						{
							boxScaled.Left &= ~3;
							boxScaled.Top &= ~3;
							boxScaled.Right = (boxScaled.Right + 3) & (~3);
							boxScaled.Bottom = (boxScaled.Bottom + 3) & (~3);
						}
						else
							if (IsUYVYFormat((vd.Format).d3dfFormat))
							// Adjust the boxScaled coordinates for YUV (2x1) alignment.
							{
								boxScaled.Left &= ~1;
								boxScaled.Right = (boxScaled.Right + 1) & ~1;
							}

						// Verify that the box is valid and properly contained within
						// the volume.
						if
						(
							(boxScaled.Right <= vdCurrentLevel.Width) &&
							(boxScaled.Bottom <= vdCurrentLevel.Height) &&
							(boxScaled.Back <= vdCurrentLevel.Depth) &&
							(boxScaled.Left < boxScaled.Right) &&
							(boxScaled.Top < boxScaled.Bottom) &&
							(boxScaled.Front < boxScaled.Back)
						)
						{
							//DPF(1, _T("LockBox 04 ...\n"));

							LOCKED_BOX lb;
							if (pTex->LockBox(uiLevelIndex, &lb, &boxScaled, LOCK_NO_DIRTY_UPDATE) != true)
								break;

							UINT uiBytesPerRow = (uiBitsPerHorizontalCoordinate *
								(boxScaled.Right - boxScaled.Left)) >> 3;

							// For DXTn formats, the "number of rows" means the number of
							// rows of adjacent 4x4 tiles.  For other formats, it is simply
							// the height of rectScaled.
							UINT uiNoOfRows = (boxScaled.Bottom - boxScaled.Top) /
								vd.Format.ddpfPixelFormat.dwRowsPerByteSequence;

							UINT uiSlicesRemaining = boxScaled.Back - boxScaled.Front;

							BYTE * pSliceStart = (BYTE *)(lb.pBits);

							do
							{
								UINT uiRowsRemaining = uiNoOfRows;

								BYTE * pRowStart = pSliceStart;

								do
								{
									BYTE *pRowByte = pRowStart;
									UINT uiBytesRemaining = uiBytesPerRow;

									//do {*(pRowByte) ^= 255; ++pRowByte;} while (--uiBytesRemaining);
									do {*(pRowByte) >>= 1; ++pRowByte;} while (--uiBytesRemaining);
									pRowStart += lb.RowPitch;
								}
								while (--uiRowsRemaining);

								pSliceStart += lb.SlicePitch;
							}
							while (--uiSlicesRemaining);

							pTex->UnlockBox(uiLevelIndex);
						}

						if ((++uiLevelIndex) == uiLevels)
							return TRUE;
					}
				}
			}
		}

	return FALSE;
}


BOOL CMipVolTest::LockWithNULL(CnVolTexture * pTex, DWORD flag)
{
	//DPF(1, _T("LockBox 05 ...\n"));

	LOCKED_BOX LockedBox;
	bool bLockBoxResult = pTex->LockBox(
		0,
		&LockedBox,
		NULL,
		flag
		);

	if (bLockBoxResult == true)
	{
		pTex->UnlockBox(0);

		ModifySysMemBoxes(pTex, NULL);

		return TRUE;
	}
	else 
	{
		WriteToLog ("lockbox failed in LockWithNULL\n");
		return FALSE;
	}
}	


BOOL CMipVolTest::LockManyAreas(CnVolTexture * pTex,UINT TotalBoxes)
{
	BOOL bRet = FALSE;
	BOX Box;

	VOLUME_DESC Desc;
	bool bGetLevelDescriptorResult = pTex->GetLevelDesc (0,&Desc);
	if (bGetLevelDescriptorResult != true)
	{
		WriteToLog ("GetLevelDesc failed in CMipVolTest::LockManyAreas\n");
		return FALSE;
	}

	assert(TotalBoxes <= VTTOTALBOXES);

	bRet = TRUE;
	for (UINT iBox = 0; iBox < TotalBoxes; iBox++)
	{
		if (IsDXTnFormat((D3DFORMAT)((Desc.Format).d3dfFormat)))
			Box = VTCDXTnMULTIBOXES[iBox];
		else
			Box = VTCMULTIBOXES[iBox];

		//DPF(1, _T("LockBox 06 ...\n"));

		LOCKED_BOX LockedBox;
		bool bLockBoxResult = pTex->LockBox(
			0,
			&LockedBox,
			&Box,
			0
			);

		if (bLockBoxResult != true)
		{
			WriteToLog ("LockBox failed in LockWithManyArea\n");
			bRet = FALSE;
			break;
		}

		pTex->UnlockBox(0);		

		ModifySysMemBoxes(pTex, &Box);
	}

	return bRet;
}


BOOL CMipVolTest::AddDirtyWithNULL(CnVolTexture * pTex)
{
	bool bAddDirtyBoxResult = pTex->AddDirtyBox(NULL);

	ModifySysMemBoxes(pTex, NULL);

	if (bAddDirtyBoxResult != true)
	{
		WriteToLog ("AddDirtyBox failed in AddDirtyWithNULL\n");
		return FALSE;
	}

	return TRUE;
}


BOOL CMipVolTest::AddDirtyManyAreas(CnVolTexture * pTex,UINT TotalBoxes)
{
	BOOL bRet = FALSE;

	BOX Box;

	VOLUME_DESC	Desc;
	bool bGetLevelDescriptorResult = pTex->GetLevelDesc(0, &Desc);
	if (bGetLevelDescriptorResult != true)
	{
		WriteToLog ("GetLevelDesc failed in CMipVolTest::AddDirtyManyArea\n");
		return FALSE;
	}

	assert(TotalBoxes <= VTTOTALBOXES);

	bRet = TRUE;
	for (UINT iBox = 0; iBox < TotalBoxes; iBox++)
	{
		if (IsDXTnFormat((D3DFORMAT)((Desc.Format).d3dfFormat)))
			Box = VTCDXTnMULTIBOXES[iBox];
		else
			Box = VTCMULTIBOXES[iBox];

		bool bAddDirtyBoxResult = pTex->AddDirtyBox(&Box);

		ModifySysMemBoxes(pTex, &Box);

		if (bAddDirtyBoxResult != true)
		{
			WriteToLog ("\nAddDirtyBox failed in AddDirtyManyArea\n");
			bRet = FALSE;
			break;
		}
	}

	return bRet;
}


BOOL CMipVolTest::LockSubLevel (CnVolTexture * pTex)
{
	DWORD cLevels = pTex->GetLevelCount();
	for (UINT iLevel = 1; iLevel < cLevels; iLevel++)
	{
		LOCKED_BOX LockedBox;
		bool bLockBoxResult = pTex->LockBox(
			iLevel,
			&LockedBox,
			NULL,
			0
			);

		if (bLockBoxResult != true)
		{
			WriteToLog("\nlockbox failed in LockSubLevel\n");
			return FALSE;
		}

		pTex->UnlockBox(iLevel);
	}

	return TRUE;
}


BOOL CMipVolTest::IsDXTnFormat (D3DFORMAT Format)
{
	return (Format == D3DFMT_DXT1 || Format == D3DFMT_DXT2 || Format == D3DFMT_DXT3 || Format == D3DFMT_DXT4 || Format == D3DFMT_DXT5);
}


bool CMipVolTest::VTCreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CnVolTexture ** ppVolumeTexture, bool bStripes, DWORD dwFlags /*= CD3D_ALL*/)
{
	bool bCreateResult;

	UINT uiSliceInitialize;

	if (!(bCreateResult = CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, dwFlags)))
		return false;

	// Setup the colors for the Stripes array
	D3DCOLOR		Stripes[8];

	Stripes[0] = 0xffffffff;    // White
	Stripes[1] = 0xffff0000;	// Red
	Stripes[2] = 0xff00ff00;	// Green
	Stripes[3] = 0xff0000ff;	// Blue
	Stripes[4] = 0xffffff00;	// Yellow
	Stripes[5] = 0xffff00ff;	// Magenta
	Stripes[6] = 0xff00ffff;	// Cyan
	Stripes[7] = 0xff808080;    // Grey

	// Setup the colors for the Mip array
	D3DCOLOR		RedStripes[4];

	RedStripes[0] = 0xffffffff; // White
	RedStripes[1] = 0xffff0000; // Red
	RedStripes[2] = 0xffffffff; // White
	RedStripes[3] = 0xffff0000; // Red

	// Setup the colors for the Mip array
	D3DCOLOR		GreenStripes[4];

	GreenStripes[0] = 0xffffffff;   // White
	GreenStripes[1] = 0xff00ff00;	// Green
	GreenStripes[2] = 0xffffffff;   // White
	GreenStripes[3] = 0xff00ff00;   // Green

	// Setup the colors for the Mip array
	D3DCOLOR		BlueStripes[4];

	BlueStripes[0] = 0xffffffff;    // White
	BlueStripes[1] = 0xff0000ff;    // Blue
	BlueStripes[2] = 0xffffffff;    // White
	BlueStripes[3] = 0xff0000ff;    // Blue

	D3DCOLOR		BlackStripes[4];

	BlackStripes[0]=0xff000000;	//Black
	BlackStripes[1]=0xff000000;	//Black
	BlackStripes[2]=0xff000000;	//Black
	BlackStripes[3]=0xff000000;	//Black

	// "Model" initialization call:
	// (*ppVolumeTexture) -> LoadStripes(UINT uLevel, UINT uSlice, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal=false, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);

	if (bStripes)
	{
		for (uiSliceInitialize = 0; uiSliceInitialize < Depth; ++uiSliceInitialize)
		{
			(*ppVolumeTexture) -> LoadStripes(0, uiSliceInitialize, VTGENERICTEXSIZE,VTGENERICTEXSIZE,4,RedStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
			if (Levels >= 2) (*ppVolumeTexture) -> LoadStripes(1, uiSliceInitialize, VTGENERICTEXSIZE/2,VTGENERICTEXSIZE/2,4,GreenStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
			if (Levels >= 3) (*ppVolumeTexture) -> LoadStripes(2, uiSliceInitialize, VTGENERICTEXSIZE/4,VTGENERICTEXSIZE/4,4,BlueStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
			if (Levels >= 4) (*ppVolumeTexture) -> LoadStripes(3, uiSliceInitialize, VTGENERICTEXSIZE/8,VTGENERICTEXSIZE/8,4,BlueStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
			if (Levels >= 5) (*ppVolumeTexture) -> LoadStripes(4, uiSliceInitialize, VTGENERICTEXSIZE/16,VTGENERICTEXSIZE/16,4,BlueStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
			if (Levels >= 6) (*ppVolumeTexture) -> LoadStripes(5, uiSliceInitialize, VTGENERICTEXSIZE/32,VTGENERICTEXSIZE/32,4,BlueStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
		}
	}
	else
	{
		for (uiSliceInitialize = 0; uiSliceInitialize < Depth; ++uiSliceInitialize)
		{
			(*ppVolumeTexture) -> LoadStripes(0, uiSliceInitialize, VTGENERICTEXSIZE,VTGENERICTEXSIZE,4,BlackStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
			if (Levels >= 2) (*ppVolumeTexture) -> LoadStripes(1, uiSliceInitialize, VTGENERICTEXSIZE/2,VTGENERICTEXSIZE/2,4,BlackStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
			if (Levels >= 3) (*ppVolumeTexture) -> LoadStripes(2, uiSliceInitialize, VTGENERICTEXSIZE/4,VTGENERICTEXSIZE/4,4,BlackStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
			if (Levels >= 4) (*ppVolumeTexture) -> LoadStripes(3, uiSliceInitialize, VTGENERICTEXSIZE/8,VTGENERICTEXSIZE/8,4,BlackStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
			if (Levels >= 5) (*ppVolumeTexture) -> LoadStripes(4, uiSliceInitialize, VTGENERICTEXSIZE/16,VTGENERICTEXSIZE/16,4,BlackStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
			if (Levels >= 6) (*ppVolumeTexture) -> LoadStripes(5, uiSliceInitialize, VTGENERICTEXSIZE/32,VTGENERICTEXSIZE/32,4,BlackStripes,false, false, TEXLOAD_DEFAULT, dwFlags); 
		}
	}

	return true;
}
