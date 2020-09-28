       DiscoveryHandles = discoveryHandles;
        }

        public bool ScenarioSucceeded { get; private set; }
        public List<WFDSvcWrapperHandle> DiscoveryHandles { get; private set; }
    }

    internal class ServicesDiscoveryScenario
    {
        public ServicesDiscoveryScenario(
            WiFiDirectTestController discoveryTestController,
            WiFiDirectTestController advertiserTestController,
            ServicesDiscoveryParameters discoveryParameters
            )
        {
            this.discoveryTestController = discoveryTestController;
            this.advertiserTestController = advertiserTestController;
            this.discoveryParameters = discoveryParameters;
        }

        public ServicesDiscoveryScenarioResult Execute()
        {
            ExecuteInternal();

            return new ServicesDiscoveryScenarioResult(succeeded, discoveryHandles);
        }

        private bool succeeded = false;
        private List<WFDSvcWrapperHandle> discoveryHandles = null;
        private WiFiDirectTestController discoveryTestController;
        private WiFiDirectTestController advertiserTestController;
        private ServicesDiscoveryParameters discoveryParameters;

        private void ExecuteInternal()
        {
            try
            {
                WiFiDirectTestLogger.Log(
                    "Starting Discovery for \"{0}\" on device {1} ({2})",
                    discoveryParameters.Query,
                    discoveryTestController.DeviceAddress,
                    discoveryTestController.MachineName
                    );

                if (discoveryParameters.Type == ServicesDiscoveryType.FindAllAsync)
                {
                    DoFindAllDiscovery();
                }
                else if (discoveryParameters.Type == ServicesDiscoveryType.Watcher)
                {
                    DoWatcherDiscovery();
                }
            }
            catch (Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing service discovery scenario: {0}", e);
            }
        }

        private void DoFindAllDiscovery()
        {
            Stopwatch discoveryStopwatch = new Stopwatch();
            discoveryStopwatch.Start();

            discoveryHandles = discoveryTestController.DiscoverServices(
                discoveryParameters.Query,
                discoveryParameters.ExpectedMatchCount > 0,
                discoveryParameters.ServiceInfoRequest
                );

            discoveryStopwatch.Stop();
            WiFiDirectTestLogger.Log("Services Discovery (FindAllAsync) completed in {0} ms.", discoveryStopwatch.ElapsedMilliseconds);

            if (discoveryHandles.Count != discoveryParameters.ExpectedMatchCount)
            {
                WiFiDirectTestLogger.Error(
                    "Expected {0} devices but discovered {1} devices",
                    discoveryParameters.ExpectedMatchCount,
                    discoveryHandles.Count
                    );
                return;
            }

            if (discoveryHandles.Count > 0 &&
                discoveryParameters.AdvertisersToMatch != null &&
                discoveryParameters.AdvertisersToMatch.Count > 0)
            {
                WiFiDirectTestLogger.Log("Checking discovery results for expected services");
                bool foundAll = true;

                IList<DiscoveredServiceInfo> discoveredDevices = new List<DiscoveredServiceInfo>();
                foreach (var handle in discoveryHandles)
                {
                    DiscoveredServiceInfo discovery = discoveryTestController.GetDiscoveredServiceInfo(handle);
                    discoveredDevices.Add(discovery);

                    WiFiDirectTestLogger.Log(
                        "Discovered service: {0} with address: {1}",
                        discovery.ServiceName,
                        discovery.ServiceAddress.ToString()
                        );
                }

                int adIdx = 0;
                foreach (var handle in discoveryParameters.AdvertisersToMatch)
                {
                    ServiceAdvertiserInfo advertiser = advertiserTestController.GetAdvertiserInfo(handle);
                    bool found = false;

                    // Check discovered list for match, remove
                    for (int i = 0; i < discoveredDevices.Count; i++)
                    {
                        if (advertiser.ServiceName == discoveredDevices[i].ServiceName &&
                            advertiser.ServiceAddress == discoveredDevices[i].ServiceAddress)
                        {
                            WiFiDirectTestLogger.Log(
                                "Found Expected Service: {0} with address: {1}",
                                discoveredDevices[i].ServiceName,
                                discoveredDevices[i].ServiceAddress.ToString()
                                );

                            if (discoveryParameters.AdvertiserServiceInfoMatch != null &&
                                discoveryParameters.AdvertiserServiceInfoMatch.Count >= adIdx)
                            {
                                if (discoveryParameters.AdvertiserServiceInfoMatch[adIdx])
                                {
                                    WiFiDirectTestLogger.Log(
                                        "Expecting Service Info:\n\t{0}\nReceived:\n\t{1}",
                                        WiFiDirectTestUtilities.GetTruncatedString(advertiser.ServiceInfo, 32),
                                        WiFiDirectTestUtilities.GetTruncatedString(discoveredDevices[i].ServiceInfo, 32)
                                        );
                                    if (advertiser.ServiceInfo != discoveredDevices[i].ServiceInfo)
                                    {
                                        // Allow multiple services with same name/different service info
                                        // Skip if service info match fails, will fail if no service info found
                                        continue;
                                    }
                                }
                                else
                                {
                                    WiFiDirectTestLogger.Log(
                                        "Expecting No Service Info, Received:\n\t{0}",
                                        WiFiDirectTestUtilities.GetTruncatedString(discoveredDevices[i].ServiceInfo, 32)
                                        );
                                    if ("" != discoveredDevices[i].ServiceInfo)
                                    {
                                        // Allow multiple services with same name/different service info
                                        // Skip if service info match fails, will fail if no service info found
                                        continue;
                                    }
                                }
                            }

                            discoveredDevices.RemoveAt(i);

                            found = true;
                            break;
                        }
                    }

                    if (!found)
                    {
                        WiFiDirectTestLogger.Error(
                                "Did NOT Find Expected Service: {0} with address: {1}",
                                advertiser.ServiceName,
                                advertiser.ServiceAddress.ToString()
                                );
                        foundAll = false;
                        // Continue checking complete list
                    }

                    adIdx++;
                }

                if (discoveredDevices.Count > 0)
                {
                    WiFiDirectTestLogger.Error("Found unexpected services!");
                    foundAll = false;
                }

                if (!foundAll)
                {
                    return;
                }
            }

            succeeded = true;
        }

        private void DoWatcherDiscovery()
        {
            List<ServiceAdvertiserInfo> servicesExpected = new List<ServiceAdvertiserInfo>();

            int adIdx = 0;
            foreach (var handle in discoveryParameters.AdvertisersToMatch)
            {
                ServiceAdvertiserInfo advertiser = advertiserTestController.GetAdvertiserInfo(handle);
                if (discoveryParameters.AdvertiserServiceInfoMatch == null ||
                    discoveryParameters.AdvertiserServiceInfoMatch.Count < adIdx ||
                    !discoveryParameters.AdvertiserServiceInfoMatch[adIdx])
                {
                    // Clear the service info if not expected
                    advertiser.ServiceInfo = "";
                }
                servicesExpected.Add(advertiser);
                adIdx++;
            }

            Stopwatch discoveryStopwatch = new Stopwatch();
            discoveryStopwatch.Start();

            discoveryHandles = discoveryTestController.WatcherDiscoverServices(
                discoveryParameters.Query,
                servicesExpected,
                discoveryParameters.ServiceInfoRequest
                );

            discoveryStopwatch.Stop();
            WiFiDirectTestLogger.Log("Services Discovery (watcher) completed in {0} ms.", discoveryStopwatch.ElapsedMilliseconds);

            succeeded = true;
        }
    }
}
// ============================================================================
//
//  LostTexture.cpp
//
// ============================================================================

#include "LostDev.h"
#include "CTextures.h"

#define NBMIPSLEVELS	9
#define TEXSIZE			256

typedef struct _DISPLAYTEXTUREVERTEX 
{
	FLOAT x;
	FLOAT y;
	FLOAT z;
	FLOAT rhw;
	FLOAT tu;
	FLOAT tv;
} DISPLAYTEXTUREVERTEX;

// ----------------------------------------------------------------------------
// ******************************** CLostTexture ******************************
// ----------------------------------------------------------------------------

CLostTexture::CLostTexture()
:
	m_pTexA( NULL ),
	m_pTexB( NULL )
{
	m_szTestName = _T("LostTexture");
	m_szCommandKey = _T("LostTexture");
	m_TestName = _T("Lost Devices with CreateTexture");

	m_Options.D3DOptions.fMinDXVersion = 6.f;
}

// ----------------------------------------------------------------------------

bool CLostTexture::CapsCheck()
{
	if (! CLostBaseTexture::CapsCheck() )
		return false;

	m_bMipMapSupported = (0 != (m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_MIPMAP));

	return true;
}

// ----------------------------------------------------------------------------

bool CLostTexture::CreateResource(LPTEXTURES & pTex, ULONG lDev)
{
	//
	// Validate parameters.
	//

	// Verify that we're not about to leak a texture.
	assert( pTex == NULL );
	if ( pTex != NULL )
	{
		DPF( 1, _T("CLostTexture::CreateResource:  pTex should be NULL.\n") );
		return false;
	};


	//
	// Initialize locals.
	//

	bool bResult = false;


	//
	// Initialize members.
	//

	// Determine whether we're doing mip mapping.
	if (m_bUseMipMaps)
	{
		m_nMipLevels = NBMIPSLEVELS;
	};


	__try
	{
		//
		// Create the texture.
		//

		if
		(
			!CreateTexture
			(
				TEXSIZE,
				TEXSIZE,
				(m_dwUsage & USAGE_AUTOGENMIPMAP) ? 0 : m_nMipLevels,
				m_dwUsage,
				m_TexFormat,
				m_Pool,
				&pTex,
				lDev
			) ||
			pTex == NULL
		)
		{
			DPF( 1, _T("CLostTexture::CreateResource:  CreateTexture failed.") );
			goto Cleanup;
		};

		if (m_bUseSubRectLock)
		{
			//Need to initialize the texture to all black
			for (int iLevel=0; iLevel < m_nMipLevels; iLevel++)
			{
				SURFACEDESC	Desc;
				if ( ! pTex->GetLevelDesc(iLevel, &Desc, lDev) )
				{
					DPF( 1, _T("CLostTexture::CreateResource:  GetLevelDesc failed.\n"));
					goto Cleanup;
				};

				if (! pTex->LockRect(iLevel, &m_LockedRect[iLevel], NULL, 0, lDev) )
				{
					DPF( 1, _T("CLostTexture::CreateResource:  LockRect failed.\n"));
					goto Cleanup;
				};

				PBYTE   pSrc=(PBYTE)m_LockedRect[iLevel].pBits;
				DWORD	WidthSize = Desc.dwWidth * Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8;

				for (int y=0; y<Desc.dwHeight; y++)
				{
					memset(pSrc, 0, WidthSize);

					// Increment to the next line.
					pSrc += m_LockedRect[iLevel].iPitch;
				};

				if (! pTex->UnlockRect(iLevel, lDev) )
				{
					DPF( 1, _T("CLostTexture::CreateResource:  UnlockRect failed.\n"));
					goto Cleanup;
				};
			}
		}
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		DPF( 1, _T("CLostTexture::CreateResource:  Exception caught.\n"));
		goto Cleanup;
	};


	bResult = true;


Cleanup:
	if ( bResult != true )
	{
		RELEASE( pTex );
	};

	return bResult;
}

// ----------------------------------------------------------------------------

bool CLostTexture::CreateResource(ULONG lDev)
{
	// Create the texture
	if
	(
		!CreateResource
		(
			m_pTexA,
			lDev
		)
	)
	{
		DPF( 1, _T( "CLostTexture::CreateResource:  CreateResource on texture A failed.\n" ) );
		return false;
	};

	return true;
}

// ----------------------------------------------------------------------------

bool CLostTexture::CreateResourceB(ULONG lDev)
{
	// Create the texture
	if
	(
		!CreateResource
		(
			m_pTexB,
			lDev
		)
	)
	{
		DPF( 1, _T("CLostTexture::CreateResourceB:  CreateResource on texture B failed.\n"));
		return false;
	};

	return true;
}

// ----------------------------------------------------------------------------

bool CLostTexture::LockData(ULONG lDev) //NEED to add NOSYSLOCK
{
	assert( m_pTexA != NULL );
	if ( m_pTexA == NULL )
	{
		DPF( 1, _T( "CLostTexture::LockData:  m_pTexA == NULL.\n" ) );
		return false;
	};


	//do not lock on DEFAULT if dynamic is not supported
	if (m_Pool == POOL_DEFAULT && !(m_dwUsage & USAGE_DYNAMIC))
	{
		return true;
	};

	RECT * pRect = NULL;
	RECT rect;
	SURFACEDESC	Desc;

	__try
	{
		for (int iLevel=0; iLevel<m_nMipLevels; iLevel++)
		{
			if (m_bUseSubRectLock)
			{
				if ( ! m_pTexA->GetLevelDesc(iLevel, &Desc, lDev) )
				{
					DPF( 1, _T("CLostTexture::LockData:  GetLevelDesc failed.\n"));
					return false;
				}
				if (Desc.dwHeight > 4)
				{
					rect.top = Desc.dwHeight - (Desc.dwHeight / 4);
					rect.left = Desc.dwWidth - (Desc.dwWidth / 4);
					rect.bottom = Desc.dwHeight - 1;
					rect.right = Desc.dwWidth - 1;
					pRect = &rect;
				}
				else
				{
					pRect = NULL;
				}
			}

			if (! m_pTexA->LockRect(iLevel, &m_LockedRect[iLevel], pRect, (m_bLockReadOnly ? D3DLOCK_READONLY : 0), lDev) )
			{
				DPF( 1, _T("CLostTexture::LockData:  LockRect failed.\n"));
				return false;
			}
		}
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		DPF( 1, _T("CLostTexture::LockData:  Exception caught.\n"));
		return false;
	};


	return true;
}

// ----------------------------------------------------------------------------

bool CLostTexture::FillAndUnlockData(ULONG lDev)
{
	assert( m_pTexA != NULL );
	if ( m_pTexA == NULL )
	{
		DPF( 1, _T( "CLostTexture::FillAndUnlockData:  m_pTexA == NULL.\n" ) );
		return false;
	};


	//the surface is not locked on DEFAULT if dynamic is not supported
	if (m_Pool == POOL_DEFAULT && !(m_dwUsage & USAGE_DYNAMIC))
	{
		return true;
	};


	SURFACEDESC	Desc;

	__try
	{
		for (int iLevel=0; iLevel<m_nMipLevels; iLevel++)
		{
			if ( ! m_pTexA->GetLevelDesc(iLevel, &Desc, lDev) )
			{
				DPF
				(
					1,
					_T("CLostTexture::FillAndUnLockData:  GetLevelDesc on mip level %d failed.\n"),
					iLevel
				);

				return false;
			}

			if ( ! FillSurface(&Desc, &m_LockedRect[iLevel], m_bUseSubRectLock) )
			{
				DPF( 1, _T("CLostTexture::FillAndUnLockData:  FillSurface failed.\n"));
				return false;
			}

			if (! m_pTexA->UnlockRect(iLevel, lDev) )
			{
				DPF( 1, _T("CLostTexture::FillAndLockData:  UnlockRect failed.\n"));
				return false;
			}
		}
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		DPF( 1, _T("CLostTexture::FillAndUnlockData:  Exception caught.\n"));
		return false;
	};


	return true;
}

// ----------------------------------------------------------------------------

bool CLostTexture::CopyResAToResB(ULONG lDev)
{
	assert( m_pTexA != NULL );
	if ( m_pTexA == NULL )
	{
		DPF( 1, _T( "CLostTexture::CopyResAToResB:  m_pTexA == NULL.\n" ) );
		return false;
	};


	assert( m_pTexB != NULL );
	if ( m_pTexB == NULL )
	{
		DPF( 1, _T( "CLostTexture::FillAndUnlockData:  m_pTexB == NULL.\n" ) );
		return false;
	};


	__try
	{
		for (int iLevel=0; iLevel<m_nMipLevels; iLevel++)
		{
			if ( ! m_bUseCopyRects )
			{
				LOCKED_RECT lockRectA;
				LOCKED_RECT lockRectB;

				SURFACEDESC	Desc;

				if ( ! m_pTexA->GetLevelDesc(iLevel, &Desc, lDev) )
				{
					DPF( 1, _T("CLostTexture::CopyResAToResB:  GetLevelDesc failed.\n"));
					return false;
				}

				//Lock the 2 textures
				if ( !m_pTexA->LockRect(iLevel, &lockRectA, NULL, (m_bLockReadOnly ? D3DLOCK_READONLY : 0), lDev) )
				{
					DPF( 1, _T("CLostTexture::CopyResAToResB:  LockRect A failed.\n"));
					return false;
				}

				if ( !m_pTexB->LockRect(iLevel, &lockRectB, NULL, (m_bLockReadOnly ? D3DLOCK_READONLY : 0), lDev) )
				{
					DPF( 1, _T("CLostTexture::CopyResAToResB:  LockRect B failed.\n"));
					return false;
				}

				//Copy A to B
				if ( !CopyFromAToB(&lockRectA, &lockRectB, &Desc) )
				{
					DPF( 1, _T("CLostTexture::CopyResAToResB:  CopyFromAToB failed.\n"));
					return false;
				}


				//Unlock the 2 textures
				if ( !m_pTexA->UnlockRect(iLevel, lDev) )
				{
					DPF( 1, _T("CLostTexture::CopyResAToResB:  UnlockRect A failed.\n"));
					return false;
				}

				if ( !m_pTexB->UnlockRect(iLevel, lDev) )
				{
					DPF( 1, _T("CLostTexture::CopyResAToResB:  UnLockRect B failed.\n"));
					return false;
				}

				return true;
			}
			else
			{
				bool bRes = true;
				LPSURFACES pSurfA = NULL;
				LPSURFACES pSurfB = NULL;

				if (! m_pTexA->GetSurfaceLevel(iLevel, &pSurfA, lDev) )
				{
					DPF( 1, _T("CLostTexture::CopyResAToResB:  GetSurfaceLevel A failed.\n"));
					bRes = false;
					goto Error;
				}

				if (! m_pTexB->GetSurfaceLevel(iLevel, &pSurfB, lDev) )
				{
					DPF( 1, _T("CLostTexture::CopyResAToResB:  GetSurfaceLevel B failed.\n"));
					bRes = false;
					goto Error;
				}

				if (! CopyRects(pSurfA, NULL, 0, pSurfB, NULL, lDev) )
				{
					DPF( 1, _T("CLostTexture::CopyResAToResB:  CopyRects failed.\n"));
					bRes = false;
					goto Error;
				}

			Error:
				RELEASE(pSurfA);
				RELEASE(pSurfB);
				return bRes;
			}
		}
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		DPF( 1, _T("CLostTexture::CopyResAToResB:  Exception caught.\n"));
		return false;
	};


	return false;
}

// ----------------------------------------------------------------------------

bool CLostTexture::DrawResource(LPTEXTURES pTex, ULONG lDev)
{
	assert( pTex != NULL );
	if ( pTex == NULL )
	{
		DPF( 1, _T( "CLostTexture::DrawResource:  pTex == NULL.\n" ) );
		return false;
	};


	LPTEXTURES pDestTex = NULL;
	LPTEXTURES pTempTex = NULL;
	LPSURFACES pSrcSurf[NBMIPSLEVELS];
	LPSURFACES pDestSurf[NBMIPSLEVELS];
	bool bRes = true;
	SURFACEDESC desc;


	//Initialize surfaces to NULL
	for (int iLevel=0; iLevel<m_nMipLevels; iLevel++)
	{
		pSrcSurf[iLevel] = NULL;
		pDestSurf[iLevel] = NULL;
	}

	__try
	{
		switch ( m_Pool )
		{
			case D3DPOOL_SYSTEMMEM:
				//Create the destination texture.
				if(!CreateTexture(TEXSIZE, TEXSIZE, (m_dwUsage & USAGE_AUTOGENMIPMAP) ? 0 : m_nMipLevels, m_dwUsage, m_TexFormat, D3DPOOL_DEFAULT, &pDestTex, lDev) ||
					pDestTex == NULL)
				{
					DPF( 1, _T("CLostTexture::DrawResource:  CreateTexture failed.\n"));
					bRes = false;
					goto Error;
				}

				//Copy the texture using the method given on the creation of the test
				if (m_bUseCopyRects)
				{
					for (int iLevel=0; iLevel<m_nMipLevels; iLevel++)
					{
						if (! pTex->GetSurfaceLevel(iLevel, &pSrcSurf[iLevel], lDev) ||
							! pDestTex->GetSurfaceLevel(iLevel, &pDestSurf[iLevel], lDev) )
						{
							DPF( 1, _T("CLostTexture::DrawResource:  GetSurfaceLevel failed.\n"));
							bRes = false;
							goto Error;
						}

						if (m_pD3D->GetVersion() < 9.0f)
						{
							if (! CopyRects(pSrcSurf[iLevel], NULL, 0, pDestSurf[iLevel], NULL, lDev) )
							{
								DPF( 1, _T("CLostTexture::DrawResource:  CopyRects failed.\n"));
								bRes = false;
								goto Error;
							}
						}
						else
						{
							if (! UpdateSurface(pSrcSurf[iLevel], NULL, pDestSurf[iLevel], NULL, lDev) )
							{
								DPF( 1, _T("CLostTexture::DrawResource:  UpdateSurface failed.\n"));
								bRes = false;
								goto Error;
							}
						}
					}
					break;
				}
				else
				{
					//Add a dirty region so that UpdateTexture has something to update
					if ( (! pTex->AddDirtyRect(NULL, lDev) ) ||
						(! pDestTex->AddDirtyRect(NULL, lDev) ) )
					{
						DPF( 1, _T("CLostTexture::DrawResource:  AddDirtyRect failed.\n"));
						bRes = false;
						goto Error;
					}

					if (! UpdateTexture(pTex, pDestTex, lDev) )
					{
						DPF( 1, _T("CLostTexture::DrawResource:  UpdateTexture failed.\n"));
						bRes = false;
						goto Error;
					}
					break;
				}


			case D3DPOOL_SCRATCH:
				// Create the destination texture.
				if(!CreateTexture(TEXSIZE, TEXSIZE, m_nMipLevels, m_dwUsage, m_TexFormat, D3DPOOL_DEFAULT, &pDestTex, lDev) ||
					pDestTex == NULL)
				{
					DPF( 1, _T("CLostTexture::DrawResource:  CreateTexture on destination texture failed.\n"));
					bRes = false;
					goto Error;
				}

				// Create the temporary sysmem memory texture.
				if(!CreateTexture(TEXSIZE, TEXSIZE, m_nMipLevels, m_dwUsage, m_TexFormat, D3DPOOL_SYSTEMMEM, &pTempTex, lDev) ||
					pDestTex == NULL)
				{
					DPF( 1, _T("CLostTexture::DrawResource:  CreateTexture on temporary texture failed.\n"));
					bRes = false;
					goto Error;
				}


				// Copy the scratch texture to the temporary texture.
				LOCKED_RECT lockScratch, lockSysmem;
				//TODO: Copy all levels
				if (!pTex->LockRect(0, &lockScratch, NULL, 0, lDev))
				{
					DPF( 1, _T("CLostTexture::DrawResource:  LockRect on source texture failed.\n"));
					bRes = false;
					goto Error;
				}
				if (!pTempTex->LockRect(0, &lockSysmem, NULL, 0, lDev))
				{
					DPF( 1, _T("CLostTexture::DrawResource:  LockRect on temporary texture failed.\n"));
					bRes = false;
					goto Error;
				}
				if (!pTempTex->GetLevelDesc(0, &desc, lDev))
				{
					DPF( 1, _T("CLostTexture::DrawResource:  GetLevelDesc on temporary texture failed.\n"));
					bRes = false;
					goto Error;
				}

				if ( !CopyFromAToB(&lockScratch, &lockSysmem, &desc) )
				{
					DPF( 1, _T("CLostTexture::DrawResource:  CopyFromAToB failed.\n"));
					bRes = false;
					goto Error;
				}

				if (!pTex->UnlockRect(0, lDev))
				{
					DPF( 1, _T("CLostTexture::DrawResource:  UnlockRect on source texture failed.\n"));
					bRes = false;
					goto Error;
				}
				if (!pTempTex->UnlockRect(0, lDev))
				{
					DPF( 1, _T("CLostTexture::DrawResource:  UnlockRect on temporary texture failed.\n"));
					bRes = false;
					goto Error;
				}

				// Add a dirty region so that UpdateTexture has something to update.
				if ( (! pTempTex->AddDirtyRect(NULL, lDev) ) )
				{
					DPF( 1, _T("CLostTexture::DrawResource:  AddDirtyRect on temporary texture failed.\n"));
					bRes = false;
					goto Error;
				}

				if (! pDestTex->AddDirtyRect(NULL, lDev) )
				{
					DPF( 1, _T("CLostTexture::DrawResource:  AddDirtyRect on destination texture failed.\n"));
					bRes = false;
					goto Error;
				}

				if (! UpdateTexture(pTempTex, pDestTex, lDev) )
				{
					DPF( 1, _T("CLostTexture::DrawResource:  UpdateTexture failed.\n"));
					bRes = false;
					goto Error;
				}
				break;


			case D3DPOOL_MANAGED:
				pDestTex = pTex;
				break;
		}

		bRes = RenderToBackbuffer(pDestTex, lDev);
		if (!bRes)
		{
			DPF( 1, _T("CLostTexture::DrawResource:  RenderToBackbuffer failed.\n"));
			bRes = false;
			goto Error;
		}
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		DPF( 1, _T("CLostTexture::DrawResource:  Exception caught.\n"));
		bRes = false;
		goto Error;
	}


Error:
	if (D3DPOOL_SYSTEMMEM == m_Pool )
	{
		for (int iLevel=0; iLevel<m_nMipLevels; iLevel++)
		{
			RELEASE(pSrcSurf[iLevel]);
			RELEASE(pDestSurf[iLevel]);
		}
		RELEASE(pDestTex);
		assert(pTempTex == NULL);
	}
	else if (D3DPOOL_SCRATCH == m_Pool )
	{
		for (int iLevel=0; iLevel<m_nMipLevels; iLevel++)
		{
			RELEASE(pSrcSurf[iLevel]);
			RELEASE(pDestSurf[iLevel]);
		}
		RELEASE(pDestTex);
		RELEASE(pTempTex);
	};


	return bRes;
}

// ----------------------------------------------------------------------------

bool CLostTexture::RenderToBackbuffer(LPTEXTURES pTex, ULONG lDev)
{
	bool bRes = true;

	DISPLAYTEXTUREVERTEX v[4];

	float left = 0;
	float right = 256;
	float top = 0;
	float bottom = 256;

	float minUV = 0.5f / 256;
	float maxUV = (256 + 0.5f) / 256;

	v[0].x=left;   v[0].y =bottom;  v[0].z=0;  v[0].rhw=1.0; v[0].tu=minUV; v[0].tv=maxUV;  
	v[1].x=left;   v[1].y =top;     v[1].z=0;  v[1].rhw=1.0; v[1].tu=minUV; v[1].tv=minUV;  
	v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].rhw=1.0; v[2].tu=maxUV; v[2].tv=minUV;  
	v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].rhw=1.0; v[3].tu=maxUV; v[3].tv=maxUV;  

	SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1, lDev);
	SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE, lDev);

	SetTexture(0, pTex, lDev);

	if (! BeginScene())
	{
		DPF( 1, _T("CLostTexture::DrawResource:  BeginScene failed.\n"));
		return false;
	}

	bRes = RenderPrimitive
	(
		D3DPT_TRIANGLEFAN, 
		D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0),
		v,
		4,
		NULL,
		0,
		NULL,
		lDev
	);

	if ( bRes != true )
	{
		DPF( 1, _T("CLostTexture::DrawResource:  RenderPrimitive failed.\n" ) );
	};

	if (! EndScene() )
	{
		DPF( 1, _T("CLostTexture::DrawResource:  EndScene failed.\n"));
	};

	bRes &= SetTexture(0, NULL, lDev);

	return bRes;
}


// ----------------------------------------------------------------------------

bool CLostTexture::DrawResource(ULONG lDev)
{
	if(! DrawResource(m_pTexA, lDev) )
	{
		DPF( 1, _T("CLostTexture::DrawResource:  DrawResource on texture A failed.\n"));
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostTexture::DrawResourceB(ULONG lDev)
{
	if(! DrawResource(m_pTexB, lDev) )
	{
		DPF( 1, _T("CLostTexture::DrawResourceB:  DrawResource on texture B failed.\n"));
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostTexture::ReleaseResource(ULONG lDev)
{
	bool bOverallResult = true;

	__try
	{
		RELEASE(m_pTexA);
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		DPF( 1, _T("CLostTexture::ReleaseResource:  Access violation caught while releasing texture A.\n"));
		bOverallResult = false;
	};


	__try
	{
		RELEASE(m_pTexB);
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		DPF( 1, _T("CLostTexture::ReleaseResource:  Access violation caught while releasing texture B.\n"));
		bOverallResult = false;
	};


	return bOverallResult;
}
/*++
Copyright (c) Microsoft Corporation

Module Name:

    driver.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

typedef
NTSTATUS
(*PFN_KSE_QUERY_DEVICE_FLAGS)(
    __in LPCWSTR DeviceKey,
    __in LPCWSTR DeviceClass,
    __out PULONG64 DeviceFlags
    );

typedef
BOOLEAN
(*PFN_IO_TRY_QUEUE_WORKITEM)(
    __inout PIO_WORKITEM IoWorkItem,
    __in PIO_WORKITEM_ROUTINE_EX WorkerRoutine,
    __in WORK_QUEUE_TYPE QueueType,
    __in_opt __drv_aliasesMem PVOID Context
    );

typedef struct _WDFDRIVER_USBXHCI_CONTEXT {
    LIST_ENTRY  ControllerListHead;
    KSPIN_LOCK  ControllerListLock;
    ULONG       ControllerListCount;
    BOOLEAN     TestModeOn;
} WDFDRIVER_USBXHCI_CONTEXT, *PWDFDRIVER_USBXHCI_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WDFDRIVER_USBXHCI_CONTEXT, WdfDriverGetUsbXhciContext);

extern PWDFDRIVER_USBXHCI_CONTEXT   g_WdfDriverUsbXhciContext;
extern PDRIVER_OBJECT               g_UsbXhciDriverObject;

PFN_KSE_QUERY_DEVICE_FLAGS          g_KseQueryDeviceFlags;
PFN_IO_TRY_QUEUE_WORKITEM           g_IoTryQueueWorkItem;

DRIVER_INITIALIZE                   DriverEntry;
EVT_WDF_OBJECT_CONTEXT_CLEANUP      DriverCleanup;

#if TEST_MODE
NTSTATUS
Xhci_ShouldFail(
    ULONG FailPercent
    );

ULONG
Xhci_NonZeroRandomWithMax(
    ULONG   MaxRandom
    );

extern volatile BOOLEAN g_RANDOM_FAILURE_TEST_CHANGE_COMMAND_TYPE;
extern volatile BOOLEAN g_RANDOM_FAILURE_TEST_REDUCE_RINGSEGMENTS;
extern volatile BOOLEAN g_SKIP_LAST_TD_COMPLETION;

#endif
#include "dxpool.h"

D3DPOOL_RECORD DXPoolTypes[] =
{
	{POOL_DEFAULT, "D3DPOOL_DEFAULT"},
	//{POOL_MANAGED, "D3DPOOL_MANAGED"},
	{POOL_SYSTEMMEM, "D3DPOOL_SYSTEMMEM"}
	//{POOL_SCRATCH, "D3DPOOL_SCRATCH"}
};

CONST UINT cuiNoOfPoolTypes = (sizeof(DXPoolTypes) / sizeof(DXPoolTypes[0]));

const D3DPOOL_RECORD* FindPoolRecord(DWORD dwPool)
{
	UINT uiPoolRecordIndex;

	for
	(
		uiPoolRecordIndex = 0;
		uiPoolRecordIndex < cuiNoOfPoolTypes;
		++uiPoolRecordIndex
	)
	{
			if (DXPoolTypes[uiPoolRecordIndex].dwFWPool == dwPool)
				return &(DXPoolTypes[uiPoolRecordIndex]);
	}

	return NULL;
}
#include "testcomponentspch.h"
#include "DisplayModeStructures.h"

/*
 * These structures are used internally to store and sort different display modes by 
 * the CRandomModeChange and CRandomMultiMon test components
 */


bool DisplayMode::operator==(const DisplayMode &rhs) const
{
    if((DisplayNumber == rhs.DisplayNumber) &&
        (ModeIndex == rhs.ModeIndex) &&
        (Width == rhs.Width) &&
        (Height == rhs.Height) &&
        (ColorDepth == rhs.ColorDepth) &&
        (Frequency == rhs.Frequency))
        return true;
    else
        return false;
}


bool FullDisplayMode::operator<(const FullDisplayMode &rhs) const
{
    for(DWORD i = 0; i < DisplayModes.size(); i++)
    {
        if(i >= rhs.DisplayModes.size())
            return false;

        if(DisplayModes[i].DisplayNumber < rhs.DisplayModes[i].DisplayNumber)
            return true;
        else if(DisplayModes[i].DisplayNumber > rhs.DisplayModes[i].DisplayNumber)
            return false;

        if(DisplayModes[i].Width < rhs.DisplayModes[i].Width)
            return true;
        else if(DisplayModes[i].Width > rhs.DisplayModes[i].Width)
            return false;

        if(DisplayModes[i].Height < rhs.DisplayModes[i].Height)
            return true;
        else if(DisplayModes[i].Height > rhs.DisplayModes[i].Height)
            return false;

        if(DisplayModes[i].ColorDepth < rhs.DisplayModes[i].ColorDepth)
            return true;
        else if(DisplayModes[i].ColorDepth > rhs.DisplayModes[i].ColorDepth)
            return false;

        if(DisplayModes[i].Frequency < rhs.DisplayModes[i].Frequency)
            return true;
        else if(DisplayModes[i].Frequency > rhs.DisplayModes[i].Frequency)
            return false;
    }

    return false;
}

bool FullDisplayMode::operator==(const FullDisplayMode &rhs) const
{
    return DisplayModes == rhs.DisplayModes;
}


/////////////////////////////////////////////////////////////////////////
//  Multisample.cpp
//  created:	2005/06/29
//
//  purpose: Tests multisample related Shader5x issues
//////////////////////////////////////////////////////////////////////////

#include "Multisample.h"
#include <D3DMSHelper.h>
#include <MinMaxRange.h>

struct VERTEX
{
	D3DXVECTOR2 pos;
	D3DXVECTOR4 data;
};

// Vertex shader code
//  input: float2 position, float4 texcoord
//	output: float4 position, float4 texcoord
//
//  description: takes a vertex in screen space and applies an inverse
//				 viewport transformation
//
const char szVS[] =
	"dcl_input v[0].xy"										"\n"
	"dcl_input v[1].xyzw"									"\n"
	"dcl_output_sv  o[0].xyzw, position"					"\n"
	"dcl_output o[1].xyzw"									"\n"
	"dcl_constantBuffer  c0[1], immediateIndexed"			"\n"
	"mad o[0].xy, v[0].xyxx, c0[0].zwzz, c0[0].xyxx"		"\n"
	"mov o[0].zw, vec4( 0.0, 0.0, 0.0, 1.0 )"				"\n"
	"mov o[1].xyzw, v[1].xyzw"								"\n"
;

// Vertex shader code
//  input: float4 position, float4 texcoord
//	output: float4 position, float4 texcoord, float rt_array_index
//
//  description: passes through VS results and chooses rt array index
//
const char szGS[] =
	"dcl_inputPrimitive point"		"\n"
	"dcl_outputTopology pointlist"	"\n"
	"dcl_maxOutputVertexCount 1"	"\n"
	"dcl_constantBuffer c0[2], immediateIndexed"		"\n"
	"dcl_temps 6"					"\n"
	"dcl_input_sv v[1][0].xyzw, position"	"\n" // Result from previous stage
	"dcl_input v[1][1].xyzw"		"\n"		 // Result from previous stage
	"dcl_output_sv o[0].xyzw, position"			"\n"
	"dcl_output o[1].xyzw"			"\n"
	"dcl_output_sv o[2].x, renderTargetArrayIndex"	"\n" // render target array index
	"mov o[0].xyzw, v[0][0].xyzw"	"\n" // copy pos
	"mov o[1].xyzw, v[0][1].xyzw"	"\n" // copy data
	"mov o[2].x, c0[1].x"			"\n" // get array index from constant data
	"emit"							"\n"
	;

// Pixel shader code - needs to  be constructed
//  input: float4 texcoord
//	string (mov, ftou, ftoi)
const char szPSPass[] =
	"dcl_input_ps v[1].xyzw, constant"  "\n"
	"dcl_output o[0].xyzw"				"\n"
	"%s o[0].xyzw, v[1].xyzw"			"\n"
;

// Pixel shader code - needs to  be constructed
//  output: float4
//	UINT (optional sample count enclosed in ()), string (view dimension), string (type),
//  4x uint (srcaddress), offsetstring (mov, utof, itof), (sampleIndex or r1.x)
const char szPS[] =
	"dcl_output o[0].xyzw"								"\n"
	"dcl_resource%s t0, %s, %s"							"\n"
	"dcl_resource t1, Texture2D, UINT"					"\n"
	"dcl_temps 2"										"\n"
	"ld r1.xyzw, vec4(0,0,0,0), t1.xyzw"				"\n"
	"ld2dms r0.xyzw, vec4(%d, %d, %d, %d), t0.xyzw, %s"	"\n"
	"%s o[0].xyzw, r0.xyzw"								"\n"
;

const char szPSResidencyFeedback[] =
	"dcl_output o[0].xyzw"								"\n"
	"dcl_resource%s t0, %s, %s"							"\n"
	"dcl_resource t1, Texture2D, UINT"					"\n"
	"dcl_temps 3"										"\n"
	"ld r1.xyzw, vec4(0,0,0,0), t1.xyzw"				"\n"
	"ldms_s r0.xyzw, r2.x, vec4(%d, %d, %d, %d), t0.xyzw, %s"	"\n"
	"check_access_fully_mapped r2.y, r2.x"				"\n" // This line and the next two force the driver not to optimize out the otherwise unused residency feedback result
	"xor r2.y, r2.y, 0xffffffff"						"\n" // Invert the output of check_access_fully_mapped
	"xor r0.x, r0.x, r2.y"								"\n" // When fully resident r2.y should be 0x00000000 (leaving r0.x unchanged)
	"%s o[0].xyzw, r0.xyzw"								"\n"
;

DXGI_FORMAT MultisampleFormatsSmall[] =
{
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R16G16B16A16_SNORM,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R16_SINT
};

DXGI_FORMAT MultisampleFormats[] =
{
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_SINT,
	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32_SINT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R16G16B16A16_SNORM,
	DXGI_FORMAT_R16G16B16A16_SINT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_SINT,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R10G10B10A2_UINT,
	DXGI_FORMAT_R11G11B10_FLOAT,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_SNORM,
	DXGI_FORMAT_R8G8B8A8_SINT,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_R16G16_SINT,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_SINT,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R8G8_UINT,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_R8G8_SINT,
	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R16_SINT,
	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8_UINT,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R8_SINT
};

UINT Widths[] =
{
	1,
	7,
	11,
	128
};

UINT Heights[] =
{
	1,
	3,
	17,
	1
};

UINT ArraySizes[] =
{
	2,
	5
};

UINT SrcAddressList[][4] =
{
	{ 1, 0, 0, 0 },
	{ 7, 0, 0, 0 },
	{ 0, 3, 0, 0 },
	{ 11, 4, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 1, 2, 0 },
	{ 7, 0, 1, 0 },
	{ 1, 2, 5, 0 }
};

CMSLoadTest::CMSLoadTest()
: m_pGS(NULL),
  m_pMSRT(NULL),
  m_pRT(NULL),
  m_pRTStaging(NULL),
  m_pTex2DIndex(NULL),
  m_pMSRTV(NULL),
  m_pRTV(NULL),
  m_pSRV(NULL),
  m_pSRVIndex(NULL),
  m_pPSPass(NULL),
  m_pVBSamples(NULL),
  m_pVBQuad(NULL),
  m_pLayout(NULL),
  m_uSampleIndex(0)
{

}

void CMSLoadTest::InitTestParameters()
{
	//
	// Get Sample Count
	//

	// Parse the SampleCount from the group name.
	// Later this will be used to set the default value for
	// parameter "SampleCount"
	tstring szPath = GetPath().ToString( GetPath().Depth() - 2, 1);
	UINT uSampleCount = atoi(szPath.c_str());
	m_Tex2DDesc.SampleDesc.Count = uSampleCount;

	szPath = GetPath().ToString( GetPath().Depth() - 1 );
	UINT uMaxQualityLevel = atoi(szPath.c_str());

	//
	// Format
	//

	CTableValueSet< DXGI_FORMAT > *pVSFormat;
	// !Speedup
	if (m_Tex2DDesc.SampleDesc.Count == 2)
		pVSFormat = new CTableValueSet< DXGI_FORMAT >( MultisampleFormats, sizeof(DXGI_FORMAT), ARRAY_SIZE(MultisampleFormats) );
	else
		pVSFormat = new CTableValueSet< DXGI_FORMAT >( MultisampleFormatsSmall, sizeof(DXGI_FORMAT), ARRAY_SIZE(MultisampleFormatsSmall) );
	CTestCaseParameter< DXGI_FORMAT > *pFormatParam = AddParameter( _T( "Format" ), &m_Tex2DDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet< DXGI_FORMAT > ( pFormatParam, pVSFormat );

	//
	// Texture Dimension
	//

	CTestCaseParameter<D3D11_RTV_DIMENSION> *pDimParam = AddParameter( _T( "Dimension" ), &m_RTVDesc.ViewDimension );
	testfactor::RFactor rfTex2DMS = AddParameterValue( pDimParam, D3D11_RTV_DIMENSION_TEXTURE2DMS );
	testfactor::RFactor rfTex2DMSArray = AddParameterValue( pDimParam, D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY );

	//
	// Texture Declaration
	//

	CTestCaseParameter<bool> *pDeclCountParam = AddParameter( _T( "SpecifyCountInDecl" ), &m_bDeclCount );
	testfactor::RFactor rfDeclCount = AddParameterValue( pDeclCountParam, true ) +
									  AddParameterValue( pDeclCountParam, false );

	//
	// Literal Sample Index
	//

	CTestCaseParameter<bool> *pLiteralIndexParam = AddParameter( _T( "LiteralSampleIndex" ), &m_bLiteralIndex );
	testfactor::RFactor rfLiteralIndex = AddParameterValue( pLiteralIndexParam, true ) +
										 AddParameterValue( pLiteralIndexParam, false );


	//
	// Sample Description
	//

	// sample count and index
	srand(0);
	CTestCaseParameter< UINT > *pSampleCountParam = AddParameter( _T("SampleCount"), &m_Tex2DDesc.SampleDesc.Count );
	// Set the default value of parameter SampleCount to the value
	// parsed from the test group name
	pSampleCountParam->SetDefaultValue(uSampleCount);
	CTestCaseParameter<UINT> *pSampleIndexParam = AddParameter( _T( "SampleIndex" ), &m_uSampleIndex );
	CUserValueSet<UINT> *pSampleIndexSet = new CUserValueSet<UINT>();
	// !Speedup
	pSampleIndexSet->AddValue(0);
	if (m_Tex2DDesc.SampleDesc.Count > 1)
		pSampleIndexSet->AddValue(m_Tex2DDesc.SampleDesc.Count-1);
	if (m_Tex2DDesc.SampleDesc.Count > 8)
		pSampleIndexSet->AddValue(rand()%(m_Tex2DDesc.SampleDesc.Count-2) + 1);
	testfactor::RFactor rfSampleIndex = AddParameterValueSet( pSampleIndexParam, pSampleIndexSet );

	// sample quality
	CTestCaseParameter< UINT > *pQualityParam = AddParameter( _T("SampleQuality"), &m_Tex2DDesc.SampleDesc.Quality );
	CRangeValueSet< UINT > *pQualitySet = new CRangeValueSet< UINT >( 0, uMaxQualityLevel-1, 1 );
	testfactor::RFactor rfSampleMaxQuality = AddParameterValueSet< UINT >( pQualityParam, pQualitySet );

	// special patterns
	CUserValueSet< UINT> *pQualityPatternSet = new CUserValueSet< UINT >();
	pQualityPatternSet->AddValue( D3D11_CENTER_MULTISAMPLE_PATTERN );
	pQualityPatternSet->AddValue( D3D11_STANDARD_MULTISAMPLE_PATTERN );
	testfactor::RFactor rfSamplePatternQuality = AddParameterValueSet< UINT >(pQualityParam, pQualityPatternSet);

	// special patterns only supported for specific sample counts
	testfactor::RFactor rfSampleQuality = rfSampleMaxQuality;
	UINT count = m_Tex2DDesc.SampleDesc.Count;
	if ((count == 1) || (count == 2) || (count == 4) || (count == 8) || (count == 16))
	{
		rfSampleQuality = rfSampleMaxQuality + rfSamplePatternQuality;
	}

	//
	// Width and Height
	//

	// !speedup
	UINT uSizes = ARRAY_SIZE( Heights );
	if (m_Tex2DDesc.SampleDesc.Count > 4)
		uSizes = min( 2, uSizes );

	// height
	CTestCaseParameter< UINT > *pHeightParam = AddParameter( _T("Height"), &m_Tex2DDesc.Height );
	CTableValueSet<UINT> *pHeightValues = new CTableValueSet< UINT >( Heights, sizeof(UINT), uSizes );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );

	// width
	CTestCaseParameter< UINT > *pWidthParam = AddParameter( _T("Width"), &m_Tex2DDesc.Width );
	CTableValueSet<UINT> *pWidthValues = new CTableValueSet< UINT >( Widths, sizeof(UINT), uSizes );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );

	//
	// Src Address
	//

	CTestCaseParameter<UINT> *pSrcAddressXParam = AddParameter( _T( "SrcAddress_x" ), &m_uSrcAddress[0] );
	CTestCaseParameter<UINT> *pSrcAddressYParam = AddParameter( _T( "SrcAddress_y" ), &m_uSrcAddress[1] );
	CTestCaseParameter<UINT> *pSrcAddressZParam = AddParameter( _T( "SrcAddress_z" ), &m_uSrcAddress[2] );
	testfactor::RFactor rfSrcAddressAll = AddParameterValue( pSrcAddressXParam, (UINT)0 ) *
										  AddParameterValue( pSrcAddressYParam, (UINT)0 ) *
										  AddParameterValue( pSrcAddressZParam, (UINT)0 );
	for (UINT i = 0; i < ARRAY_SIZE(SrcAddressList); ++i)
		rfSrcAddressAll = rfSrcAddressAll + (AddParameterValue( pSrcAddressXParam, SrcAddressList[i][0] ) *
											AddParameterValue( pSrcAddressYParam, SrcAddressList[i][1] ) *
											AddParameterValue( pSrcAddressZParam, SrcAddressList[i][2] ));

	//
	// Arrays
	//

	CTestCaseParameter<UINT> *pArraySizeParam = AddParameter( _T( "ArraySize" ), &m_Tex2DDesc.ArraySize );
	CTestCaseParameter<UINT> *pArraySliceParam = AddParameter( _T( "ArraySlice" ), &m_uArraySlice );

	pArraySizeParam->SetDefaultValue(1);
	pArraySliceParam->SetDefaultValue(0);

	UINT uArrayStart = 1;
	UINT uSliceStart = 0;
	// !Speedup
	if (m_Tex2DDesc.SampleDesc.Count == 2)
	{
		uArrayStart = 2;
		uSliceStart = 1;
	}
	testfactor::RFactor rfArrayAll = AddParameterValue( pArraySizeParam, uArrayStart ) * AddParameterValue( pArraySliceParam, uSliceStart );
	for (UINT i = 0; i < ARRAY_SIZE( ArraySizes ); ++i)
	{
		// !Speedup
		if ((m_Tex2DDesc.SampleDesc.Count > 8) && (i > 0)) continue;

		// !Speedup
		if (m_Tex2DDesc.SampleDesc.Count == 2) break;

		// add only a few slices
		UINT uBeg = 0;
		UINT uEnd = ArraySizes[i]-1;
		UINT uMid = (ArraySizes[i]-1)/2;

		// don't add dupes
		CUserValueSet<UINT> *pArraySliceSet = new CUserValueSet<UINT>();
		pArraySliceSet->AddValue( uBeg );
		if ( uEnd > uBeg )
			pArraySliceSet->AddValue( uEnd );
		if ( uMid > uBeg )
			pArraySliceSet->AddValue( uMid );

		rfArrayAll = rfArrayAll + ( AddParameterValue( pArraySizeParam, ArraySizes[i] ) *
									AddParameterValueSet( pArraySliceParam, pArraySliceSet ) );
	}
	testfactor::RFactor rfArrayCube = AddParameterValue( pArraySizeParam, (UINT)6 ) *
									  AddParameterValueSet( pArraySliceParam, new CRangeValueSet<UINT>( 0, 5, 1 ) );

	//
	// Create Root
	//

	// Utilize pairwise to reduce the number of testcases
	testfactor::RFactor Tex2DMS = (rfTex2DMS + (rfArrayAll * rfTex2DMSArray)) * (rfHeight % rfWidth) * rfLiteralIndex * rfSrcAddressAll;
	testfactor::XFACTOR pairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{rfFormat, 1, NULL, 0},
		{rfSampleQuality, 1, NULL, 0},
		{rfSampleIndex, 1, NULL, 0},
		{Tex2DMS, 1, NULL, 0},
	};

	size_t groupOrders[] =
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	testfactor::RFactor rfTestCases = rfDeclCount * NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "Pairwise", groupOrders, _countof(groupOrders));
	SetRootTestFactor( rfTestCases );

	//
	// Filters
	//

	CFilterHandle filter;
	// options include 10.0, 10.1 + force SM4.0, 10.1 + SM4.1
	if ( (GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0) || g_bForceSM40 )
	{
		// in shader4.0 you must specify the sample count in the shader decl
		filter = FilterEqual<bool>( _T("SpecifyCountInDecl"), false );

		// in shader4.0 the ld2dms sample index must be a literal
		filter = filter || FilterEqual<bool>( _T("LiteralSampleIndex"), false );

		//
		filter = filter || FilterEqual<UINT>( _T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN ) ||
						   FilterEqual<UINT>( _T("SampleQuality"), D3D11_STANDARD_MULTISAMPLE_PATTERN );
	}


	// speedup filters

	// summary: cylce on "specify count in decl" and "literal sample index" if:
	//			(width == 1) || (width == 7)
	SkipMatching( filter ||
				  (
					( FilterEqual<bool>( _T("SpecifyCountInDecl"), false ) || FilterEqual<bool>( _T("LiteralSampleIndex"), false ) ) &&
					( FilterNotEqual<UINT>( _T("Width"), 1 ) && FilterNotEqual<UINT>( _T("Width"), 7 ) )
				  )
				);

	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R32G32B32A32_FLOAT), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R10G10B10A2_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R8G8B8A8_TYPELESS), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R32G32B32A32_TYPELESS), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R10G10B10A2_TYPELESS), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<bool>(_T("LiteralSampleIndex"), true), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), 0), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), uMaxQualityLevel - 1), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), D3D11_STANDARD_MULTISAMPLE_PATTERN), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Width"), 128), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Height"), 17), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ArraySize"), 2), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 5.0f );
	DeclarePriorityLevelByPriorityValue( 2, 2.0f );
}

TEST_RESULT CMSLoadTestResidencyFeedback::Setup()
{
	auto const tr = CMSLoadTest::Setup();
	if (!TiledOpsSupported())
	{
		WriteToLog(_T("CMSLoadTestResidencyFeedback::Setup - Tiled resource ops not supported."));
		return RESULT_SKIP;
	}
	return tr;
}

//
// Setup - Creates shaders and buffers that remain as the same
//		   for each test case.
//
TEST_RESULT CMSLoadTest::Setup()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	try
	{

		//
		// Skip sample count if no formats support multisample
		//
		for (UINT uFormat = 0; uFormat < ARRAY_SIZE( MultisampleFormats ); ++uFormat)
		{
			UINT uLevels;
			GetDevice()->CheckMultisampleQualityLevels( MultisampleFormats[uFormat], m_Tex2DDesc.SampleDesc.Count, &uLevels);
			const bool bIsValidQualityRange = m_Tex2DDesc.SampleDesc.Quality < uLevels;
			const bool bIsD3D10_1SamplePattern = ( GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_1 ) &&
				( m_Tex2DDesc.SampleDesc.Quality == D3D11_STANDARD_MULTISAMPLE_PATTERN || m_Tex2DDesc.SampleDesc.Quality == D3D11_CENTER_MULTISAMPLE_PATTERN );

			if( uLevels == 0 ||											// If this method returns 0, the format and sample count combination is not supported for the installed adapter
				( !bIsValidQualityRange && !bIsD3D10_1SamplePattern ) )	// The valid range is between zero and one less than the level returned by ID3D11Device::CheckMultiSampleQualityLevels, or one of the sample patterns
			{
				SkipMatching( GetSkipMatchingFilter() || FilterEqual<DXGI_FORMAT>( _T("Format"), MultisampleFormats[uFormat] ) );
			}
		}

		//
		//Set Rasterizer state
		//
		D3D11_RASTERIZER_DESC rastDesc;
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.MultisampleEnable = true;
		rastDesc.FrontCounterClockwise = true;
		rastDesc.DepthBias = false;
		rastDesc.DepthBiasClamp = 0;
		rastDesc.SlopeScaledDepthBias = 0;
		rastDesc.DepthClipEnable = true;
		rastDesc.ScissorEnable = false;
		rastDesc.AntialiasedLineEnable = false;
		if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSEnabled ) ) )
			throw TRException("CreateRasterizerState for m_pRSEnabled failed.", hr, RESULT_FAIL);

		GetEffectiveContext()->RSSetState( m_pRSEnabled );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		//
		//Set DepthStencil state
		//
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		dsDesc.DepthEnable = FALSE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.StencilEnable = FALSE;
		if( FAILED( hr = GetDevice()->CreateDepthStencilState ( &dsDesc, &m_pDSDisabled ) ) )
			throw TRException("CreateDepthStencilState() failed.", hr, RESULT_FAIL);
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSDisabled, 0 );

		//
		// Blend State
		//
		GetEffectiveContext()->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );

		//
		// Create the constant buffer
		//
		D3D11_BUFFER_DESC bufdesc;
		bufdesc.ByteWidth = CONST_BUFFER_SIZE;
		bufdesc.Usage = D3D11_USAGE_DYNAMIC;
		bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufdesc.MiscFlags = 0;
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer ) ) )
			throw TRException("CreateBuffer for m_pConstBuffer failed.", hr, RESULT_FAIL);
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

		//
		// Create vertex buffer
		//
		D3D11_BUFFER_DESC bufferDesc =
		{
			6 * sizeof( VERTEX ),
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE,
			0
		};
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pVBQuad ) ) )
		{
			WriteToLog( _T( "CreateBuffer() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CMSLoadTest::SetupVB] CreateBuffer failed for vertex buffer.", hr, RESULT_FAIL);
		}
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		if (FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("[CMSLoadTest::SetupVB] ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMSLoadTest::Setup()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	CheckTiledSupport();

	return tRes;
}

//
// SetupTestCase - Prepares the MSRT with test data and sets the shaders
//				   for ld2dms.
//
TEST_RESULT CMSLoadTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	try
	{
		//
		// Textures
		//
		SetupTextures();

		//
		// Shaders
		//
		SetupShaders();

		//
		// Setup the default viewport
		//
		D3D11_VIEWPORT vp;
		vp.Height = (float)m_Tex2DDesc.Height;
		vp.Width = (float)m_Tex2DDesc.Width;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		GetEffectiveContext()->RSSetViewports( 1, &vp );

		//
		// setup const data
		//
		float bufData[4];
		// Inverse viewport scale.
		bufData[2] = 2 / (float)m_Tex2DDesc.Width;
		bufData[3] = -2 / (float)m_Tex2DDesc.Height;
		// Prescaled inverse viewport translation.
		bufData[0] = -1.0f;
		bufData[1] = 1.0f;
		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, bufData ) ) )
			throw TRException("Map on m_pConstBuffer Failed", hr, RESULT_FAIL);

		//
		// Create vertex buffer
		//
		D3D11_BUFFER_DESC bufferDesc =
		{
			m_Tex2DDesc.ArraySize * m_Tex2DDesc.Width * m_Tex2DDesc.Height * m_Tex2DDesc.SampleDesc.Count * sizeof( VERTEX ),
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE,
			0
		};
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pVBSamples ) ) )
		{
			WriteToLog( _T( "CreateBuffer() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CMSLoadTest::SetupVB] CreateBuffer failed for vertex buffer.", hr, RESULT_FAIL);
		}

		//
		// Fill VBQuad
		//
		VERTEX pVert[4];
		pVert[0].pos = D3DXVECTOR2( 0, 0 );
		pVert[1].pos = D3DXVECTOR2( (float)m_Tex2DDesc.Width, 0 );
		pVert[2].pos = D3DXVECTOR2( 0, (float)m_Tex2DDesc.Height );
		pVert[3].pos = D3DXVECTOR2( (float)m_Tex2DDesc.Width, (float)m_Tex2DDesc.Height );

		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pVBQuad, pVert ) ) )
		{
			WriteToLog( _T( "Map() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CMSLoadTest::SetupVB] Map on m_pVB failed.", hr, RESULT_FAIL);
		}

		if (FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T( "ExecuteEffectiveContext() failed '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CMSLoadTest::SetupVB] ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
		}
	}
	catch (TRException& exc)
	{
		CleanupTestCase();
		tRes = exc.GetResult();
		exc.Prepend("[CMSLoadTest::SetupTestCase()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

void CMSLoadTest::SetupShaders()
{
	HRESULT hr;
	// Create the shaders that are used often
	//
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;


	try
	{
		//
		// Vertex Shader
		//

		// shader profile, like vs_5_0
		tstring profile = g_TestApp.GetShaderProfile(
			D3D_SHADER_STAGE_VERTEX,
			g_TestApp.m_D3DOptions.FeatureLevel,
			g_TestApp.m_D3DOptions.FeatureLevel);

		profile = profile + " \n" + szVS;

		if( !AssembleShader( profile.c_str(), &pShaderBuf, &pErrorBuf ) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T("AssembleShader failed assembling VS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			throw TRException("");
		}
		if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pVS ) ) )
			throw TRException("CreateVertexShader() failed creating default VS", hr, RESULT_FAIL);
		GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );

		//
		// Set the input layout
		//
		D3D11_INPUT_ELEMENT_DESC DeclTL[] =
		{
			{"0_", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
			{"1_", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},  //texture coordinates
		};
		if(	FAILED(	hr = GetDevice()->CreateInputLayout( DeclTL, sizeof(DeclTL) / sizeof(DeclTL[ 0 ]), pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayout ) ) )
			throw TRException("CreateInputLayout() failed.", hr, RESULT_FAIL);
		GetEffectiveContext()->IASetInputLayout( m_pLayout );

		SAFE_RELEASE( pShaderBuf );
		SAFE_RELEASE( pErrorBuf );

		//
		// Geometry Shader
		//

		// shader profile, like vs_5_0
		profile = g_TestApp.GetShaderProfile(
			D3D_SHADER_STAGE_GEOMETRY,
			g_TestApp.m_D3DOptions.FeatureLevel,
			g_TestApp.m_D3DOptions.FeatureLevel);

		profile = profile + " \n" + szGS;

		if( !AssembleShader( profile.c_str(), &pShaderBuf, &pErrorBuf ) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T("AssembleShader failed assembling GS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			throw TRException("");
		}

		if( FAILED( hr = GetDevice()->CreateGeometryShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pGS ) ) )
			throw TRException("CreateGeometryShader() failed creating default GS", hr, RESULT_FAIL);

		SAFE_RELEASE( pShaderBuf );
		SAFE_RELEASE( pErrorBuf );

		//
		// Optionally add sample count to the resource declaration
		//
		tstring szDeclCount = _T("");
		if (m_bDeclCount)
			szDeclCount = _T("(") + ToString(m_Tex2DDesc.SampleDesc.Count) + _T(")");

		//
		//	Literal index
		//
		tstring szSampleIndex = ToString( m_uSampleIndex );
		if (!m_bLiteralIndex)
			szSampleIndex = _T("r1.x");  // results from texture load

		//
		// Based on format, insert casting instructions
		//

		// various instrs
		char szMov[] = "mov";
		char szFtoU[] = "ftou";
		char szFtoI[] = "ftoi";
		char szUtoF[] = "utof";
		char szItoF[] = "itof";

		// types
		char szFloat[] = "float";
		char szUint[] = "uint";
		char szInt[] = "sint";

		char *szInstr;
		char *szInstrPass;
		char *szType;

		DWORD flags = GetFlags( m_Tex2DDesc.Format );
		if ((flags & FF_FLOAT) || (flags & FF_UNORM) || (flags & FF_SNORM))
		{
			szInstr = szMov;
			szInstrPass = szMov;
			szType = szFloat;
		}
		else if (flags & FF_UINT)
		{
			szInstr = szUtoF;
			szInstrPass = szFtoU;
			szType = szUint;
		}
		else if (flags & FF_SINT)
		{
			szInstr = szItoF;
			szInstrPass = szFtoI;
			szType = szInt;
		}
		else
			throw TRException("Format type flags not found.");

		//
		// Select appropriate view dimension string
		//
		char szTex2DMS[] = "Texture2DMS";
		char szTex2DMSA[] = "Texture2DMSArray";

		char *szViewDim;

		if (m_RTVDesc.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DMS)
			szViewDim = szTex2DMS;
		else
			szViewDim = szTex2DMSA;

		// shader profile, like vs_5_0
		profile = g_TestApp.GetShaderProfile(
			D3D_SHADER_STAGE_PIXEL,
			g_TestApp.m_D3DOptions.FeatureLevel,
			g_TestApp.m_D3DOptions.FeatureLevel);

		tstring final = profile + " \n" + (UseResidencyFeedback() ? szPSResidencyFeedback : szPS);

		//
		// Fill in shader code
		//
		const UINT MAX_STR = 1000;
		char szPSPassFinal[MAX_STR];
		char szPSFinal[MAX_STR];
		_snprintf( szPSFinal, MAX_STR-1, final.c_str(),
				  szDeclCount.c_str(),
				  szViewDim,
				  szType,
				  m_uSrcAddress[0], m_uSrcAddress[1], m_uSrcAddress[2], 0,
				  szSampleIndex.c_str(),
				  szInstr );
		szPSFinal[sizeof( szPSFinal ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1

		tstring passFinal = profile + " \n" + szPSPass;

		_snprintf( szPSPassFinal, MAX_STR-1, passFinal.c_str(), szInstrPass );
		szPSPassFinal[sizeof( szPSPassFinal ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1

		//
		// Create pixel shader that outputs what in the texcoord
		//

		if ( !AssembleShader(szPSPassFinal, &pShaderBuf, &pErrorBuf) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T("AssembleShader failed assembling PS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			throw TRException("");
		}
		if (FAILED( hr = GetDevice()->CreatePixelShader((DWORD*)pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPSPass)))
			throw TRException("CreateVertexShader() failed creating default PS", hr, RESULT_FAIL);

		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );

		//
		//	Create pixel shader that uses ld2dms and outputs results
		//
		const UINT globalFlags = UseResidencyFeedback() ? AssemblyHelper::GF_USES_TILED_RESOURCES : 0;
		if ( !AssembleShader(szPSFinal, &pShaderBuf, &pErrorBuf, globalFlags) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T("AssembleShader failed assembling PS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			throw TRException("");
		}
		if (FAILED( hr = GetDevice()->CreatePixelShader((DWORD*)pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPS)))
			throw TRException("CreateVertexShader() failed creating default PS", hr, RESULT_FAIL);

		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
	}
	catch (TRException& exc)
	{
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
		exc.Prepend("[CMSLoadTest::SetupShaders()] ");
		throw(exc);
	}
}

//
// SetupMSRenderTarget
//
void CMSLoadTest::SetupTextures()
{
	D3D11_TEXTURE2D_DESC tex2DDesc;
	HRESULT hr;

	//
	// Check Support
	//

	UINT uCaps;
	if( FAILED( hr = GetDevice()->CheckFormatSupport( m_Tex2DDesc.Format, &uCaps ) ) )
		throw TRException("CheckFormatSupport failed.", hr, RESULT_FAIL);

	if (!(uCaps & D3D11_FORMAT_SUPPORT_TEXTURE2D) ||
		!(uCaps & D3D11_FORMAT_SUPPORT_RENDER_TARGET) ||
		!(uCaps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET) ||
		!(uCaps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD))
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual<DXGI_FORMAT>( _T("Format"),  m_Tex2DDesc.Format ) );
		throw TRException("Format not supported as render target or texture2d or doesn't support multisample load.", TEST_RESULT(RESULT_SKIP));
	}

	UINT uLevels;
	GetDevice()->CheckMultisampleQualityLevels( m_Tex2DDesc.Format, m_Tex2DDesc.SampleDesc.Count, &uLevels);
	const bool bIsValidQualityRange = m_Tex2DDesc.SampleDesc.Quality < uLevels;
	const bool bIsD3D10_1SamplePattern = ( GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_1 ) &&
		( m_Tex2DDesc.SampleDesc.Quality == D3D11_STANDARD_MULTISAMPLE_PATTERN || m_Tex2DDesc.SampleDesc.Quality == D3D11_CENTER_MULTISAMPLE_PATTERN );

	if( uLevels == 0 ||											// If this method returns 0, the format and sample count combination is not supported for the installed adapter
		( !bIsValidQualityRange && !bIsD3D10_1SamplePattern ) )	// The valid range is between zero and one less than the level returned by ID3D11Device::CheckMultiSampleQualityLevels, or one of the sample patterns
	{
		throw TRException("Quality level not supported for this format", TEST_RESULT(RESULT_SKIP));
	}

	//
	//	Textures
	//

	// multisample render target
	tex2DDesc.ArraySize			 = m_Tex2DDesc.ArraySize;
	tex2DDesc.BindFlags			 = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tex2DDesc.CPUAccessFlags	 = 0;
	tex2DDesc.Format			 = m_Tex2DDesc.Format;
	tex2DDesc.MipLevels		     = 1;
	tex2DDesc.MiscFlags			 = 0;
	tex2DDesc.Usage				 = D3D11_USAGE_DEFAULT;
	tex2DDesc.Width				 = m_Tex2DDesc.Width;
	tex2DDesc.Height			 = m_Tex2DDesc.Height;
	tex2DDesc.SampleDesc.Count   = m_Tex2DDesc.SampleDesc.Count;
	tex2DDesc.SampleDesc.Quality = m_Tex2DDesc.SampleDesc.Quality;
	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pMSRT ) ) )
	{
		WriteToLog( _T( "CreateTexture2D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateTexture2D failed for MS render target texture.", hr, RESULT_FAIL);
	}

	// render target
	tex2DDesc.Format			 = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex2DDesc.ArraySize			 = 1;
	tex2DDesc.BindFlags			 = D3D11_BIND_RENDER_TARGET;
	tex2DDesc.SampleDesc.Count	 = 1;
	tex2DDesc.SampleDesc.Quality = 0;
	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pRT ) ) )
	{
		WriteToLog( _T( "CreateTexture2D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateTexture2D failed for render target texture.", hr, RESULT_FAIL);
	}

	// staging resource desc
	tex2DDesc.BindFlags			= 0;
	tex2DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
	tex2DDesc.Usage				= D3D11_USAGE_STAGING;
	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pRTStaging ) ) )
	{
		WriteToLog( _T( "CreateTexture2D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateTexture2D failed for render target texture.", hr, RESULT_FAIL);
	}

	// 1x1 texture that contains the sample index
	D3D11_SUBRESOURCE_DATA srData;
	srData.pSysMem = &m_uSampleIndex;
	srData.SysMemPitch = sizeof(UINT);
	srData.SysMemSlicePitch = sizeof(UINT);

	// non-literal sample index tex
	tex2DDesc.Width				= 1;
	tex2DDesc.Height			= 1;
	tex2DDesc.Format			= DXGI_FORMAT_R32_UINT;
	tex2DDesc.CPUAccessFlags	= 0;
	tex2DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, &srData, &m_pTex2DIndex ) ) )
	{
		WriteToLog( _T( "CreateTexture2D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateTexture2D failed for sample index texture.", hr, RESULT_FAIL);
	}

	//
	//	Multisample Views
	//

	// rtv
	D3D11_RENDER_TARGET_VIEW_DESC rtv;
	rtv.Format = m_Tex2DDesc.Format;
	rtv.ViewDimension = m_RTVDesc.ViewDimension;

	// srv
	D3D11_SHADER_RESOURCE_VIEW_DESC srv;
	srv.Format = m_Tex2DDesc.Format;

	switch( m_RTVDesc.ViewDimension )
	{
	case D3D11_RTV_DIMENSION_TEXTURE2DMS:
		// srv
		srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
		// srv
		srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
		srv.Texture2DMSArray.ArraySize = m_Tex2DDesc.ArraySize - m_uArraySlice;
		srv.Texture2DMSArray.FirstArraySlice = m_uArraySlice;
		// rtv
		rtv.Texture2DMSArray.ArraySize = m_Tex2DDesc.ArraySize;
		rtv.Texture2DMSArray.FirstArraySlice = 0;
		break;
	}

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pMSRT, &rtv, &m_pMSRTV ) ) )
	{
		WriteToLog( _T( "CreateRenderTargetView() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateRenderTargetView failed.", hr, RESULT_FAIL);
	}

	if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pMSRT, &srv, &m_pSRV ) ) )
	{
		WriteToLog( _T( "CreateShaderResourceView() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateShaderResourceView failed.", hr, RESULT_FAIL);
	}

	srv.Format = DXGI_FORMAT_R32_UINT;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;
	srv.Texture2D.MostDetailedMip = 0;
	if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pTex2DIndex, &srv, &m_pSRVIndex ) ) )
	{
		WriteToLog( _T( "CreateShaderResourceView() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateShaderResourceView failed.", hr, RESULT_FAIL);
	}

	// render target view (non MS)
	rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtv.Texture2D.MipSlice = 0;
	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRT, &rtv, &m_pRTV ) ) )
	{
		WriteToLog( _T( "CreateRenderTargetView() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateRenderTargetView failed.", hr, RESULT_FAIL);
	}
}

//
// ExecuteTestCase - Runs ld2dms shader and reads back results
//
TEST_RESULT CMSLoadTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	try
	{
		//
		// Init Test Data
		//
		const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_Tex2DDesc.Format);
		DWORD uFlags = GetFlags( m_Tex2DDesc.Format );

		// rand float between -50 and 50
		srand(0);
		UINT uSize = m_Tex2DDesc.ArraySize * m_Tex2DDesc.Width * m_Tex2DDesc.Height * m_Tex2DDesc.SampleDesc.Count;
		std::vector<D3DXVECTOR4> vData(uSize);
		for( UINT i = 0; i < vData.size(); ++i )
		{
			vData[i] = D3DXVECTOR4( ((rand() % 1000) - 500.f) / 10.0f,
								   ((rand() % 1000) - 500.f) / 10.0f,
								   ((rand() % 1000) - 500.f) / 10.0f,
								   ((rand() % 1000) - 500.f) / 10.0f );

			// if its unorm or snorm data, make it between -2 and 2
			if ((uFlags & FF_UNORM) || (uFlags & FF_SNORM))
				vData[i] = vData[i] * 0.04f;
		}
		// fill vertex buffer
		FillBuffer(vData);

		//
		// Fill MSRT
		//

		// state
		FLOAT color[] = { 0, 0, 0, 0 };
		UINT strides[] = { sizeof( VERTEX ) };
		UINT offsets[] = { 0 };
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVBSamples, strides, offsets );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
		GetEffectiveContext()->PSSetShader( m_pPSPass, NULL, 0 );
		GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pMSRTV, NULL );

		// Draw
		GetEffectiveContext()->ClearRenderTargetView( m_pMSRTV, color );
		uSize = m_Tex2DDesc.Width*m_Tex2DDesc.Height;
		for (UINT uSlice = 0; uSlice < m_Tex2DDesc.ArraySize; ++uSlice)
		{
			SetSlice(uSlice);
			for (UINT uSample = 0; uSample < m_Tex2DDesc.SampleDesc.Count; ++uSample)
			{
				UINT uStart = uSample*uSize + uSlice*uSize*m_Tex2DDesc.SampleDesc.Count;
				GetEffectiveContext()->OMSetBlendState( NULL, 0, 1 << uSample);
				GetEffectiveContext()->Draw(uSize, uStart);
			}
		}

		//
		// MSLoad MSRT
		//

		// state
		ID3D11ShaderResourceView *pSRV[] = { m_pSRV, m_pSRVIndex };
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVBQuad, strides, offsets );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
		GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, NULL );
		GetEffectiveContext()->PSSetShaderResources( 0, 2, pSRV );
		GetEffectiveContext()->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );

		// draw
		GetEffectiveContext()->ClearRenderTargetView( m_pRTV, color );
		GetEffectiveContext()->Draw( 4, 0 );

		if( FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

		//
		// Verify results
		//

		// Readback results
		std::vector<D3DXVECTOR4> vResults(m_Tex2DDesc.Width*m_Tex2DDesc.Height);
		GetResults(vResults, m_pRT);

		// compute expected index
		D3DXVECTOR4 vInitial;
		UINT uExpectedIndex = m_uSrcAddress[0] +
							  m_uSrcAddress[1]*m_Tex2DDesc.Width +
							  m_uSampleIndex*m_Tex2DDesc.Width*m_Tex2DDesc.Height;
		// expected index only ignores array address if Texture2DMS
		if (m_RTVDesc.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY)
			uExpectedIndex += (m_uSrcAddress[2] + m_uArraySlice)*m_Tex2DDesc.Width*m_Tex2DDesc.Height*m_Tex2DDesc.SampleDesc.Count;

		// get initial data if src address is in bounds
		if ((m_uSrcAddress[0] < m_Tex2DDesc.Width) &&		// out of bounds width
			(m_uSrcAddress[1] < m_Tex2DDesc.Height) &&		// out of bounds height
			((m_uSrcAddress[2] < (m_Tex2DDesc.ArraySize - m_uArraySlice)) ||	// out of bound array only matters for array resource
			 (m_RTVDesc.ViewDimension != D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY)))
		{
			assert( uExpectedIndex < vData.size() );
			vInitial = vData[uExpectedIndex];
		}
		else
			vInitial = D3DXVECTOR4(0,0,0,0);	// out of bounds behavior

		// compute expected value
		D3DXVECTOR4 vExpected = vInitial;
		if (pTexFormatInfo->dwFlags & FF_UNORM)
		{
			vExpected.x = max( min( vExpected.x, 1.f ), 0.f );
			vExpected.y = max( min( vExpected.y, 1.f ), 0.f );
			vExpected.z = max( min( vExpected.z, 1.f ), 0.f );
			vExpected.w = max( min( vExpected.w, 1.f ), 0.f );
		}
		else if (pTexFormatInfo->dwFlags & FF_SNORM)
		{
			vExpected.x = max( min( vExpected.x, 1.f ), -1.f );
			vExpected.y = max( min( vExpected.y, 1.f ), -1.f );
			vExpected.z = max( min( vExpected.z, 1.f ), -1.f );
			vExpected.w = max( min( vExpected.w, 1.f ), -1.f );
		}
		else if ((pTexFormatInfo->dwFlags & FF_UINT) ||
				 (pTexFormatInfo->dwFlags & FF_SINT))
		{
			vExpected.x = (float)(int)vExpected.x;
			vExpected.y = (float)(int)vExpected.y;
			vExpected.z = (float)(int)vExpected.z;
			vExpected.w = (float)(int)vExpected.w;
			if (pTexFormatInfo->pBitsPerComponent[3] == 2)
				vExpected.w = min( vExpected.w, 3 );
			if (pTexFormatInfo->dwFlags & FF_UINT)
			{
				vExpected.x = max(vExpected.x, 0);
				vExpected.y = max(vExpected.y, 0);
				vExpected.z = max(vExpected.z, 0);
				vExpected.w = max(vExpected.w, 0);
			}
		}

		// andy told me this format doesn't have a sign bit
		if (m_Tex2DDesc.Format == DXGI_FORMAT_R11G11B10_FLOAT)
		{
			vExpected.x = max( vExpected.x, 0.f );
			vExpected.y = max( vExpected.y, 0.f );
			vExpected.z = max( vExpected.z, 0.f );
			vExpected.w = max( vExpected.w, 0.f );
		}

		// only use correct format for float comparison
		DXGI_FORMAT format = m_Tex2DDesc.Format;
		if ((pTexFormatInfo->dwFlags & FF_UINT) ||
			(pTexFormatInfo->dwFlags & FF_SINT))
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		const DXGIFormatInfo* pTexFormatInfoFake = CFormatInfoMap::Find(format);

		// test results
		// NOTE: to speed things up, only check the last 16 pixels on the last 16 rows
		UINT uStartY = max( (int)0, (int)m_Tex2DDesc.Height - 16 );
		UINT uStartX = max( (int)0, (int)m_Tex2DDesc.Width - 16);

		// num components
		UINT uMaxComp = 0;
		for (UINT comp = 0; comp < 4; ++comp)
		{
			if (pTexFormatInfo->pBitsPerComponent[comp]) uMaxComp++;
		}

		for (UINT y = uStartY; y < m_Tex2DDesc.Height; ++y)
			for (UINT x = uStartX; x < m_Tex2DDesc.Width; ++x)
				for (UINT comp = 0; comp < uMaxComp; ++comp)
				{
					// jump to a specific component of a d3dxvector4
					float fInitial = *((float*)&vInitial + comp);
					float fExpected = *((float*)&vExpected + comp);
					float fActual = *((float*)&vResults[x + y*m_Tex2DDesc.Width] + comp);

					float fUlpDiff;
					// NOTE: on ref, the 11 bit float has ulpdiff > 1, the rest are < 1.
					if (!MinMaxRange::TestFloatsSame(fExpected, fActual, pTexFormatInfoFake, comp, 2.0f, &fUlpDiff))
					{
						WriteToLog("Failure Info:");
						WriteToLog("Component: %d", comp);
						WriteToLog("Pixel x: %d, y: %d", x, y);
						WriteToLog("Initial: \t\t%f", fInitial);
						WriteToLog("Actual: \t\t%f", fActual);
						WriteToLog("Expected: \t%f", fExpected);
						if (fUlpDiff < 50) // spew ulpdiff only if it appears to be a precision issue
							WriteToLog("Ulp difference: \t%f", fUlpDiff);
						throw TRException("ld2dms returned invalid results.");
					}
				}

	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMSLoadTest::ExecuteTestCase()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

void CMSLoadTest::FillBuffer(std::vector<D3DXVECTOR4> &vData)
{
	// precondition
	assert( vData.size() == (m_Tex2DDesc.ArraySize*m_Tex2DDesc.Height*m_Tex2DDesc.Width*m_Tex2DDesc.SampleDesc.Count) );

	HRESULT hr;

	//
	// Fill VBSamples
	//
	D3D11_MAPPED_SUBRESOURCE mappedVB;
	if ( FAILED( hr = GetEffectiveContext()->Map(m_pVBSamples, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB) ) )
		throw TRException("m_pVertexBuffer Map failed.", hr, RESULT_FAIL);
	VERTEX* pVB = (VERTEX*)mappedVB.pData;

	UINT uSliceSize = m_Tex2DDesc.Height*m_Tex2DDesc.Width*m_Tex2DDesc.SampleDesc.Count;
	for (UINT uSlice = 0; uSlice < m_Tex2DDesc.ArraySize; ++uSlice)
		for (UINT sample = 0; sample < m_Tex2DDesc.SampleDesc.Count; ++sample)
			for (UINT y = 0; y < m_Tex2DDesc.Height; y++)
				for (UINT x = 0; x < m_Tex2DDesc.Width; x++)
				{
					pVB->pos = D3DXVECTOR2( x + 0.5f, y + 0.5f );
					pVB->data = vData[x + y*m_Tex2DDesc.Width + sample*m_Tex2DDesc.Width*m_Tex2DDesc.Height + uSlice*uSliceSize];
					++pVB;
				}
	GetEffectiveContext()->Unmap(m_pVBSamples, 0);
}

void CMSLoadTest::SetSlice(UINT uSlice)
{
	assert(m_pConstBuffer);
	assert(uSlice < m_Tex2DDesc.ArraySize);

	HRESULT hr;

	//
	// setup const data
	//
	float bufData[5];
	// Inverse viewport scale.
	bufData[2] = 2 / (float)m_Tex2DDesc.Width;
	bufData[3] = -2 / (float)m_Tex2DDesc.Height;
	// Prescaled inverse viewport translation.
	bufData[0] = -1.0f;
	bufData[1] = 1.0f;
	bufData[4] = *((float*)&uSlice);
	if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, bufData ) ) )
		throw TRException("Map on m_pConstBuffer Failed", hr, RESULT_FAIL);
}

void CMSLoadTest::GetResults(std::vector<D3DXVECTOR4> &vData, ID3D11Resource *pRes)
{
	assert( vData.size() == (m_Tex2DDesc.Width * m_Tex2DDesc.Height) );

	//copy
	GetImmediateContext()->CopyResource( m_pRTStaging, pRes );//m_pRT );

	// map
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE texmap;
	if( FAILED( hr = GetImmediateContext()->Map(m_pRTStaging, 0, D3D11_MAP_READ, 0, &texmap ) ) )
	{
		WriteToLog( _T( "Failed to map depth stencil texture, hr = 0x%x" ), hr );
		throw TRException("[CDepthTest::GetDepth] Map failed on 1D depth staging texture.", hr, RESULT_FAIL);
	}

	// copy data
	float *pData = (float*)texmap.pData;
	for (UINT y = 0; y < m_Tex2DDesc.Height; ++y)
	{
		memcpy( &vData[y*m_Tex2DDesc.Width], pData, m_Tex2DDesc.Width*sizeof(D3DXVECTOR4) );
		pData = (float*)((BYTE*)pData + texmap.RowPitch);
	}

	// unmap
	GetImmediateContext()->Unmap( m_pRTStaging, 0 );
}

void CMSLoadTest::CleanupTestCase()
{
	ID3D11Buffer *pNullBuffers[] = { NULL };
	UINT offsets[] = { 0 };
	ID3D11RenderTargetView* pRTNull[] = { NULL, NULL, NULL, NULL };
	ID3D11ShaderResourceView *pSRVNull[] = { NULL, NULL };
	GetEffectiveContext()->OMSetRenderTargets( 1, pRTNull, NULL );
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, pNullBuffers, offsets, offsets );
	GetEffectiveContext()->IASetInputLayout( NULL );
	GetEffectiveContext()->PSSetShaderResources( 0, 2, pSRVNull );
	GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	SAFE_RELEASE( m_pMSRT );
	SAFE_RELEASE( m_pRT );
	SAFE_RELEASE( m_pRTStaging );
	SAFE_RELEASE( m_pTex2DIndex );
	SAFE_RELEASE( m_pMSRTV );
	SAFE_RELEASE( m_pRTV );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pSRVIndex );

	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pPSPass );

	SAFE_RELEASE( m_pLayout );
	SAFE_RELEASE( m_pVBSamples );
}

void CMSLoadTest::Cleanup()
{
	ID3D11Buffer* pBufferNull[] = { NULL };
	if (GetEffectiveContext())
	{
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->RSSetState( NULL );
		GetEffectiveContext()->OMSetDepthStencilState( NULL, 0 );

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog("ExecuteEffectiveContext() failed");
		}
	}

	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pDSDisabled );
	SAFE_RELEASE( m_pConstBuffer );
	SAFE_RELEASE( m_pVBQuad );

	CShader5xTest::Cleanup();
}

//////////////////////////////////////////////////////////////////////////
// Multisample Instructions (10.1)
//////////////////////////////////////////////////////////////////////////

CMultisampleTest::CMultisampleTest()
: m_pVBQuad(NULL),
  m_pLayout(NULL),
  m_pSRView(NULL),
  m_pTex2DBackup(NULL),
  m_pRTVBackup(NULL)
{

}

void CMultisampleTest::InitTestParameters()
{
	//
	// Get Sample Count
	//
	tstring szPath = GetPath().ToString( GetPath().Depth() - 2, 1);
	UINT uSampleCount = atoi(szPath.c_str());
	m_Tex2DDesc.SampleDesc.Count = uSampleCount;

	szPath = GetPath().ToString( GetPath().Depth() - 1 );
	UINT uMaxQualityLevel = atoi(szPath.c_str());

	//
	// Format
	//

	CTableValueSet< DXGI_FORMAT > *pVSFormat;
	// !Speedup
	if (m_Tex2DDesc.SampleDesc.Count == 2)
		pVSFormat = new CTableValueSet< DXGI_FORMAT >( MultisampleFormats, sizeof(DXGI_FORMAT), ARRAY_SIZE(MultisampleFormats) );
	else
		pVSFormat = new CTableValueSet< DXGI_FORMAT >( MultisampleFormatsSmall, sizeof(DXGI_FORMAT), ARRAY_SIZE(MultisampleFormatsSmall) );
	CTestCaseParameter< DXGI_FORMAT > *pFormatParam = AddParameter( _T( "Format" ), &m_Tex2DDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet< DXGI_FORMAT > ( pFormatParam, pVSFormat );

	//
	// Texture Dimension
	//

	CTestCaseParameter<D3D11_RTV_DIMENSION> *pDimParam = AddParameter( _T( "Dimension" ), &m_RTVDesc.ViewDimension );
	testfactor::RFactor rfTex2DMS = AddParameterValue( pDimParam, D3D11_RTV_DIMENSION_TEXTURE2DMS );
	testfactor::RFactor rfTex2DMSArray = AddParameterValue( pDimParam, D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY );

	//
	// Sample Description
	//

	// sample count
	CTestCaseParameter< UINT > *pSampleCountParam = AddParameter( _T("SampleCount"), &m_Tex2DDesc.SampleDesc.Count );
	// Set the default value of parameter SampleCount to the value
	// parsed from the test group name
	pSampleCountParam->SetDefaultValue(uSampleCount);

	// sample quality
	CTestCaseParameter< UINT > *pQualityParam = AddParameter( _T("SampleQuality"), &m_Tex2DDesc.SampleDesc.Quality );
	CRangeValueSet< UINT > *pQualitySet = new CRangeValueSet< UINT >( 0, uMaxQualityLevel-1, 1 );
	testfactor::RFactor rfSampleMaxQuality = AddParameterValueSet< UINT >( pQualityParam, pQualitySet );

	// special patterns
	CUserValueSet< UINT> *pQualityPatternSet = new CUserValueSet< UINT >();
	pQualityPatternSet->AddValue( D3D11_CENTER_MULTISAMPLE_PATTERN );
	pQualityPatternSet->AddValue( D3D11_STANDARD_MULTISAMPLE_PATTERN );
	testfactor::RFactor rfSamplePatternQuality = AddParameterValueSet< UINT >(pQualityParam, pQualityPatternSet);

	// special patterns only supported for specific sample counts
	testfactor::RFactor rfSampleQuality = rfSampleMaxQuality;
	UINT count = m_Tex2DDesc.SampleDesc.Count;
	if ((count == 1) || (count == 2) || (count == 4) || (count == 8) || (count == 16))
	{
		rfSampleQuality = rfSampleMaxQuality + rfSamplePatternQuality;
	}

	//
	// Width and Height
	//

	// !speedup
	UINT uSizes = ARRAY_SIZE( Heights );
	if (m_Tex2DDesc.SampleDesc.Count > 4)
		uSizes = min( 2, uSizes );

	// height
	CTestCaseParameter< UINT > *pHeightParam = AddParameter( _T("Height"), &m_Tex2DDesc.Height );
	CTableValueSet<UINT> *pHeightValues = new CTableValueSet< UINT >( Heights, sizeof(UINT), uSizes );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );

	// width
	CTestCaseParameter< UINT > *pWidthParam = AddParameter( _T("Width"), &m_Tex2DDesc.Width );
	CTableValueSet<UINT> *pWidthValues = new CTableValueSet< UINT >( Widths, sizeof(UINT), uSizes );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );

	//
	// Arrays
	//

	CTestCaseParameter<UINT> *pArraySizeParam = AddParameter( _T( "ArraySize" ), &m_Tex2DDesc.ArraySize );
	CTestCaseParameter<UINT> *pArraySliceParam = AddParameter( _T( "ArraySlice" ), &m_uArraySlice );

	pArraySizeParam->SetDefaultValue(1);
	pArraySliceParam->SetDefaultValue(0);

	UINT uArrayStart = 1;
	UINT uSliceStart = 0;
	// !Speedup
	if (m_Tex2DDesc.SampleDesc.Count == 2)
	{
		uArrayStart = 2;
		uSliceStart = 1;
	}
	testfactor::RFactor rfArrayAll = AddParameterValue( pArraySizeParam, uArrayStart ) * AddParameterValue( pArraySliceParam, uSliceStart );
	for (UINT i = 0; i < ARRAY_SIZE( ArraySizes ); ++i)
	{
		// !Speedup
		if ((m_Tex2DDesc.SampleDesc.Count > 8) && (i > 0)) continue;

		// !Speedup
		if (m_Tex2DDesc.SampleDesc.Count == 2) break;

		// add only a few slices
		UINT uBeg = 0;
		UINT uEnd = ArraySizes[i]-1;
		UINT uMid = (ArraySizes[i]-1)/2;

		// don't add dupes
		CUserValueSet<UINT> *pArraySliceSet = new CUserValueSet<UINT>();
		pArraySliceSet->AddValue( uBeg );
		if ( uEnd > uBeg )
			pArraySliceSet->AddValue( uEnd );
		if ( uMid > uBeg )
			pArraySliceSet->AddValue( uMid );

		rfArrayAll = rfArrayAll + ( AddParameterValue( pArraySizeParam, ArraySizes[i] ) *
									AddParameterValueSet( pArraySliceParam, pArraySliceSet ) );
	}
	testfactor::RFactor rfArrayCube = AddParameterValue( pArraySizeParam, (UINT)6 ) *
									  AddParameterValueSet( pArraySliceParam, new CRangeValueSet<UINT>( 0, 5, 1 ) );

	//
	// Create Root
	//

	// Utilize pairwise to reduce the number of testcases
	testfactor::RFactor Tex2DMS = ( rfTex2DMS + (rfArrayAll * rfTex2DMSArray ) ) * (rfHeight % rfWidth);

	testfactor::XFACTOR pairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{rfFormat, 1, NULL, 0},
		{rfSampleQuality, 1, NULL, 0},
		{Tex2DMS, 1, NULL, 0},
	};

	size_t groupOrders[] =
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "Pairwise", groupOrders, _countof(groupOrders));
	SetRootTestFactor( rfTestCases );

	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R32G32B32A32_FLOAT), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R10G10B10A2_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R8G8B8A8_TYPELESS), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T