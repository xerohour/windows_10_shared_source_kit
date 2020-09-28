// ============================================================================
//
//  LostBaseTexture.cpp
//
// ============================================================================

#include "LostDev.h"
#include "CTextures.h"

//No need to test DepthStencil
TEXTURE_USAGE g_usages[] =
{
	// Win8:[TestBug]344026
	// Remove this scenario to reduce the test execution time by 75%
	// Reduced by 75% with the combination of remvoing g_lockFlags[] scenario { 6.0f, 9.0f, 0, _T("") }
	//{ 6.0f, 9.0f, 0, T("") },
	{ 8.0f, 9.0f, USAGE_RENDERTARGET,		_T("RenderTarget") },
	//{ 8.0f, 9.0f, USAGE_DEPTHSTENCIL,		_T("DepthStencil") },
	{ 8.1f, 9.0f, USAGE_DYNAMIC,			_T("Dynamic") },
	{ 9.0f, 9.0f, USAGE_AUTOGENMIPMAP,		_T("AutoGenMipMap") },

	{ 9.0f, 9.0f, USAGE_AUTOGENMIPMAP | USAGE_RENDERTARGET,		_T("AutoGenMipMap | RenderTarget") },
	//{ 9.0f, 9.0f, USAGE_AUTOGENMIPMAP | USAGE_DEPTHSTENCIL,		_T("AutoGenMipMap | DepthStencil") },
	{ 9.0f, 9.0f, USAGE_AUTOGENMIPMAP | USAGE_DYNAMIC,			_T("AutoGenMipMap | Dynamic") },
};	  
const UINT cUsage = sizeof(g_usages) / sizeof(g_usages[0]);

TEXTURE_LOCKFLAG g_lockFlags[] =
{
	// Win8:[TestBug]344026
	//Remove this scenario to reduce the test execution time
	// Reduced by 75% with the combination of remvoing g_usages[] scenario {6.0f,9.0f,0,T("")}
	//{ 6.0f, 9.0f, 0, _T("") },
	{ 7.0f, 9.0f, LOCK_NOSYSLOCK,	_T("NoSysLock") },
	{ 8.0f, 9.0f, LOCK_DISCARD,		_T("Discard") },
	{ 8.0f, 9.0f, LOCK_NOSYSLOCK | LOCK_DISCARD, _T("NoSysLock | Discard") },
};	  
const UINT cLockFlag = sizeof(g_lockFlags) / sizeof(g_lockFlags[0]);

//No need to test DepthStencil
TEXTURE_FORMAT g_formats[] =
{
	{ 6.0f, 9.0f, FMT_R5G6B5               	,	_T("FMT_R5G6B5")	 },
	{ 6.0f, 9.0f, FMT_X8R8G8B8             	,	_T("FMT_X8R8G8B8")	 }
	// Win8:[TestBug]344026
	//Remove this scenario to reduce the test execution time
	//{ 9.0f, 9.0f, FMT_A2R10G10B10           ,	_T("FMT_A2R10G10B10")	 }
	//{ 8.0f, 9.0f, FMT_D16_LOCKABLE          ,	_T("FMT_D16_LOCKABLE")	 },
	//{ 8.0f, 9.0f, FMT_D16	             	,	_T("FMT_D16")	 }
};	  
const UINT cFormat = sizeof(g_formats) / sizeof(g_formats[0]);

// ----------------------------------------------------------------------------
// ******************************** CLostBaseTexture ******************************
// ----------------------------------------------------------------------------


void CLostBaseTexture::SetFlags()
{
	m_nResForCase = 1;
	if (m_rCasesManaged2Res->Active() || m_rCasesSystemMem2Res->Active() || m_rCasesScratch2Res->Active())
		m_nResForCase = 2;

	if (m_rCasesDefault->Active())
	{
		m_pCurrentTexCase = (TEXTURE_CASE*)m_listCasesDefault[ m_rCasesDefault->Value() ];
	}
	else if (m_rCasesManaged1Res->Active())
	{
		m_pCurrentTexCase = (TEXTURE_CASE*)m_listCasesManaged1Res[ m_rCasesManaged1Res->Value() ];
	}
	else if (m_rCasesManaged2Res->Active())
	{
		m_pCurrentTexCase = (TEXTURE_CASE*)m_listCasesManaged2Res[ m_rCasesManaged2Res->Value() ];
	}
	else if (m_rCasesSystemMem1Res->Active())
	{
		m_pCurrentTexCase = (TEXTURE_CASE*)m_listCasesSystemMem1Res[ m_rCasesSystemMem1Res->Value() ];
	}
	else if (m_rCasesSystemMem2Res->Active())
	{
		m_pCurrentTexCase = (TEXTURE_CASE*)m_listCasesSystemMem2Res[ m_rCasesSystemMem2Res->Value() ];
	}
	else if (m_rCasesScratch1Res->Active())
	{
		m_pCurrentTexCase = (TEXTURE_CASE*)m_listCasesScratch1Res[ m_rCasesScratch1Res->Value() ];
	}
	else if (m_rCasesScratch2Res->Active())
	{
		m_pCurrentTexCase = (TEXTURE_CASE*)m_listCasesScratch2Res[ m_rCasesScratch2Res->Value() ];
	}

	m_dwUsage = m_pCurrentTexCase->dwUsage;
	m_dwLockFlags = m_pCurrentTexCase->dwLockFlag;
	m_TexFormat = m_pCurrentTexCase->format;
	m_bUseCopyRects = m_pCurrentTexCase->bUseCopyRects;
	m_bUseSubRectLock = m_pCurrentTexCase->bUseSubRectLock;
	m_bUseMipMaps = m_pCurrentTexCase->bUseMipLevels;

	m_nMipLevels = m_bUseMipMaps ? 5 : 1;
}

// ----------------------------------------------------------------------------

void CLostBaseTexture::SetTestFactors()
{
	GenerateCaseList();

	//These factors are used to test the copy methods
	if(	!CHECKREF(m_rCasesDefault = NewFactor( m_listCasesDefault.size() )) ||
		!CHECKREF(m_rCasesManaged1Res = NewFactor( m_listCasesManaged1Res.size() )) ||
		!CHECKREF(m_rCasesManaged2Res = NewFactor( m_listCasesManaged2Res.size() )) ||
		!CHECKREF(m_rCasesSystemMem1Res = NewFactor( m_listCasesSystemMem1Res.size() )) ||
		!CHECKREF(m_rCasesSystemMem2Res = NewFactor( m_listCasesSystemMem2Res.size() )) ||
		!CHECKREF(m_rCasesScratch1Res = NewFactor( m_listCasesScratch1Res.size() )) ||
		!CHECKREF(m_rCasesScratch2Res = NewFactor( m_listCasesScratch2Res.size() )) )
	{
		DPF(0, "ERROR setting the cases in LostBaseTexture()");
	}

	m_rCasesManager = m_rLostWays *
						  (	(m_rPoolScratch2Res * m_rCasesScratch2Res) +
							(m_rPoolScratch1Res * m_rCasesScratch1Res) +
						    (m_rPoolSystemMem2Res * m_rCasesSystemMem2Res) + 
							(m_rPoolSystemMem1Res * m_rCasesSystemMem1Res) +
							(m_rPoolManaged2Res * m_rCasesManaged2Res) +
							(m_rPoolManaged1Res * m_rCasesManaged1Res) +
							(m_rPoolDefault * m_rCasesDefault)
						  );

}

// ----------------------------------------------------------------------------

bool CLostBaseTexture::TestTerminate()
{
	std::vector <TEXTURE_CASE *>::iterator caseIter;

	m_listCasesDefault.clear();
	m_listCasesSystemMem1Res.clear();
	m_listCasesSystemMem2Res.clear();
	m_listCasesManaged1Res.clear();
	m_listCasesManaged2Res.clear();
	m_listCasesScratch1Res.clear();
	m_listCasesScratch2Res.clear();

	for (int i=0; i<4; i++)
	{
		for (caseIter = m_listCasesPool[i].begin(); caseIter != m_listCasesPool[i].end(); caseIter++)
		{
			TEXTURE_CASE* pTexCase = (TEXTURE_CASE*)*caseIter;
			delete pTexCase;
		}
		m_listCasesPool[i].clear();
	}

	return CLostDev::TestTerminate();
}

// ----------------------------------------------------------------------------

bool CLostBaseTexture::CapsCheck()
{
	if (! CLostDev::CapsCheck() )
		return false;

	m_TexFormat = GetValidFormat(RTYPE_TEXTURE);

	if (m_TexFormat == FMT_ANY)
	{
		DPF(1, "No Texture format supported\n");
		return false;
	}

	return true;
}

void CLostBaseTexture::DumpTestDetails(void)
{
	CLostDev::DumpTestDetails();

	DPF(1, "    Usage : %s\n", m_pCurrentTexCase->szUsage);
	DPF(1, "    Lock : %s\n", m_pCurrentTexCase->szLockFlag);
	DPF(1, "    Format : %s\n", m_pCurrentTexCase->szFormat);

	if (m_bUseMipMaps)
	{
		DPF(1, "    Using MipMaps\n");
	}

	if (m_bUseSubRectLock)
	{
		DPF(1, "    Using SubRect Lock\n");
	}

	if (m_nResForCase == 1)
	{
		//DrawResource difference only for SystemMem and Scratch
		if ((m_Pool == POOL_SYSTEMMEM) || (m_Pool == POOL_SCRATCH && m_pD3D->GetVersion() < 9.0f))
		{
			if (m_bUseCopyRects)
			{
				if (m_pD3D->GetVersion() < 9.0f)
				{
					DPF(1, "    Using CopyRects\n");
				}
				else
				{
					DPF(1, "    Using UpdateSurface\n");
				}
			}
			else
			{
				DPF(1, "    Using UpdateTexture\n");
			}
		}
	}
	if (m_nResForCase == 2)
	{
		//On DX9, we always use memcpy.
		if (m_pD3D->GetVersion() < 9.0f)
		{
			if (m_bUseCopyRects)
			{
				DPF(1, "    A to B using CopyRects\n");
			}
			else
			{
				DPF(1, "    A to B using Memcpy\n");
			}
		}
	}
	//else if ( !m_rCopyFromAToB->Active() && !m_rCopy->Active())
		//This is for managed 1 textures, don't need to display anything else
}


void CLostBaseTexture::GenerateCaseList()
{
	std::vector <TEXTURE_CASE *>::iterator caseIter;

	TEXTURE_CASE *pTexCase = NULL;

	for (int iPool=0; iPool < 4; iPool++)
	{
		//POOL_SCRATCH not available on DX8.0
		if ( (iPool == POOL_SCRATCH) && (m_pD3D->GetVersion() < 8.1f) )
			continue;

        //managed dx7 textures on ref do not work
        if ( (POOL_MANAGED == iPool) && (m_pD3D->GetVersion() < 8.0) )
            continue;


		for (int iUsage=0; iUsage<cUsage; iUsage++)
		{
			//Make sure the usage is available on this DX version
			if ( m_pD3D->GetVersion() < g_usages[iUsage].fMinDX)
				continue;

			//POOL_SCRATCH can't have any usage
			if ( (iPool == POOL_SCRATCH) && (g_usages[iUsage].dwUsage != 0) )
				continue;

			//Dynamic textures not allowed on POOL_MANAGED
			if ( (g_usages[iUsage].dwUsage & USAGE_DYNAMIC) && (iPool == POOL_MANAGED) )
				continue;

			//AutoGenMipMap textures not allowed on POOL_SYSTEMMEM
			if ( (g_usages[iUsage].dwUsage & USAGE_AUTOGENMIPMAP) && (iPool == POOL_SYSTEMMEM) )
				continue;

			//RenderTarget and DepthStencil must be in POOL_DEFAULT
			if ( (g_usages[iUsage].dwUsage & (USAGE_RENDERTARGET | USAGE_DEPTHSTENCIL)) && (iPool != POOL_DEFAULT) )
				continue;

			//Make sure that the hardware supports Dynamic textures
			if ( (g_usages[iUsage].dwUsage & USAGE_DYNAMIC) && (0 == (m_pSrcDevice->GetCaps()->dwCaps2 & D3DCAPS2_DYNAMICTEXTURES)) )
				continue;

			//Make sure that the hardware supports mipmaps for AutoGenMipMap textures
			if ( (g_usages[iUsage].dwUsage & USAGE_AUTOGENMIPMAP) && (!m_bMipMapSupported) )
				continue;

			//Make sure that the hardware supports AutoGenMipMap textures
			if ( (g_usages[iUsage].dwUsage & USAGE_AUTOGENMIPMAP) && (0 == (m_pSrcDevice->GetCaps()->dwCaps2 & D3DCAPS2_CANAUTOGENMIPMAP)) )
				continue;

			for (int iFormat=0; iFormat<cFormat; iFormat++)
			{
				//Make sure the format is available on this DX version
				if ((m_pD3D->GetVersion() < 9.0f) && (g_formats[iFormat].format == FMT_A2R10G10B10))
					continue;


                if(m_pD3D->GetVersion() >= 8.0f)
                {
				    //Check if these four formats are supported (R5G6B5, X1R5G5B5, X8R8G8B8, A2R10G10B10)
				    //(Check D16 and D16_LOCKABLE for DS)
				    HRESULT hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(),
														    m_pSrcDevice->GetDevType(),
														    m_pCurrentMode->pDisplay->Format,
														    g_usages[iUsage].dwUsage,
														    RTYPE_TEXTURE,
														    g_formats[iFormat].format);

				    if ( FAILED(hr) && (g_formats[iFormat].format == FMT_R5G6B5) )
				    {
					    //Let's try the other 16bits format
					    g_formats[iFormat].format = FMT_X1R5G5B5;
					    _tcscpy(g_formats[iFormat].szFormat, _T("FMT_X1R5G5B5"));

					    hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(),
													    m_pSrcDevice->GetDevType(),
													    m_pCurrentMode->pDisplay->Format,
													    g_usages[iUsage].dwUsage,
													    RTYPE_TEXTURE,
													    g_formats[iFormat].format);
				    }

				    //If the format is not available for this usage, don't test it
				    if (FAILED(hr))
					    continue;
                }
                else
                {
                    bool bFound = false;

                    for(UINT nFmt = 0; nFmt < m_uCommonTextureFormats; nFmt++)
                    {
                        if(((FORMAT)g_formats[iFormat].format).d3dfFormat == m_pCommonTextureFormats[nFmt].d3dfFormat)
                        {
                            bFound = true;
                        }
                    }

                    if(!bFound)
                    {
                        continue;
                    }
                }



				for (int iLock=0; iLock<cLockFlag; iLock++)
				{
					//Make sure the lock flag is available on this DX version
					if (m_pD3D->GetVersion() < g_lockFlags[iLock].fMinDX)
						continue;

					for (int iMipLevels=0; iMipLevels<2; iMipLevels++)
					{
						//if AutoGenMipMap, do not use MipMap in the test case
						if ( (iMipLevels == 1) && (g_usages[iUsage].dwUsage & USAGE_AUTOGENMIPMAP) )
							continue;

						//Make sure that the hardware supports mipmaps textures
						if ( (iMipLevels == 1) && (!m_bMipMapSupported) )
							continue;

						for (int iCopyRects=0; iCopyRects<2; iCopyRects++)
						{
                            if((m_pD3D->GetVersion() < 8.0f) && (1 != iCopyRects))
                            {
                                continue;
                            }

							//if Lightweight texture, do not use CopyRects in the test case
							//if ( (iCopyRects == 1) && (0 == (g_usages[iUsage].dwUsage & USAGE_HEAVYWEIGHT)) )
							//	continue;

							//No need to test CopyRects on POOL_DEFAULT
							if ( (iCopyRects == 1) && (iPool == POOL_DEFAULT) )
								continue;

							for (int iSubRectLock=0; iSubRectLock<2; iSubRectLock++)
							{
								//Create a new Texture case
								pTexCase = new TEXTURE_CASE;
								ZeroMemory(pTexCase, sizeof(pTexCase));
								pTexCase->bUseMipLevels = (iMipLevels == 1);
								pTexCase->bUseCopyRects = (iCopyRects == 1);
								pTexCase->bUseSubRectLock = (iSubRectLock == 1);
								pTexCase->dwUsage = g_usages[iUsage].dwUsage;
								_tcscpy(pTexCase->szUsage, g_usages[iUsage].szUsage);
								pTexCase->dwLockFlag = g_lockFlags[iLock].dwLockFlag;
								_tcscpy(pTexCase->szLockFlag, g_lockFlags[iLock].szLockFlag);
								pTexCase->format = g_formats[iFormat].format;
								_tcscpy(pTexCase->szFormat, g_formats[iFormat].szFormat);

								//Add the case to the list
								m_listCasesPool[iPool].push_back(pTexCase);
							}
						}
					}
				}
			}
		}
	}

	//Add the Default cases
	for (caseIter = m_listCasesPool[POOL_DEFAULT].begin(); caseIter != m_listCasesPool[POOL_DEFAULT].end(); caseIter++)
	{
		pTexCase = (TEXTURE_CASE*)*caseIter;
		if ( pTexCase->bUseSubRectLock )
			continue;

		m_listCasesDefault.push_back( *caseIter );
	}

	//Add the SystemMem 1 Res cases
	for (caseIter = m_listCasesPool[POOL_SYSTEMMEM].begin(); caseIter != m_listCasesPool[POOL_SYSTEMMEM].end(); caseIter++)
	{
		//Only test CopyRects on DX7 and DX6
		pTexCase = (TEXTURE_CASE*)*caseIter;
		if (!pTexCase->bUseCopyRects && m_pD3D->GetVersion() <= 7.0f)
			continue;

		m_listCasesSystemMem1Res.push_back( *caseIter );
	}

	//Add the SystemMem 2 Res cases
	for (caseIter = m_listCasesPool[POOL_SYSTEMMEM].begin(); caseIter != m_listCasesPool[POOL_SYSTEMMEM].end(); caseIter++)
	{
		//Can't test CopyRects on DX9+
		pTexCase = (TEXTURE_CASE*)*caseIter;
		if (pTexCase->bUseCopyRects && m_pD3D->GetVersion() >= 9.0f)
			continue;

		m_listCasesSystemMem2Res.push_back( *caseIter );
	}

	//Add the Scratch 1 Res cases
	for (caseIter = m_listCasesPool[POOL_SCRATCH].begin(); caseIter != m_listCasesPool[POOL_SCRATCH].end(); caseIter++)
	{
		//Can't test CopyRects (AV in d3d8.dll from scratch to default (and UpdateSurface for scratch pool)
		pTexCase = (TEXTURE_CASE*)*caseIter;
		if (pTexCase->bUseCopyRects)
			continue;

		m_listCasesScratch1Res.push_back( *caseIter );
	}

	//Add the Scratch 2 Res cases
	for (caseIter = m_listCasesPool[POOL_SCRATCH].begin(); caseIter != m_listCasesPool[POOL_SCRATCH].end(); caseIter++)
	{
		//Can't test CopyRects on DX9+
		pTexCase = (TEXTURE_CASE*)*caseIter;
		if (pTexCase->bUseCopyRects && m_pD3D->GetVersion() >= 9.0f)
			continue;

		m_listCasesScratch2Res.push_back( *caseIter );
	}

	//Add the Managed 1 Res cases
	for (caseIter = m_listCasesPool[POOL_MANAGED].begin(); caseIter != m_listCasesPool[POOL_MANAGED].end(); caseIter++)
	{
		//No need to test CopyRects for 1 Resource case
		pTexCase = (TEXTURE_CASE*)*caseIter;
		if (pTexCase->bUseCopyRects)
			continue;

		m_listCasesManaged1Res.push_back( *caseIter );
	}

	//Add the Managed 2 Res cases
	for (caseIter = m_listCasesPool[POOL_MANAGED].begin(); caseIter != m_listCasesPool[POOL_MANAGED].end(); caseIter++)
	{
		//Can't test CopyRects on DX9+
		pTexCase = (TEXTURE_CASE*)*caseIter;
		if (pTexCase->bUseCopyRects && m_pD3D->GetVersion() >= 9.0f)
			continue;

		m_listCasesManaged2Res.push_back( *caseIter );
	}
}


