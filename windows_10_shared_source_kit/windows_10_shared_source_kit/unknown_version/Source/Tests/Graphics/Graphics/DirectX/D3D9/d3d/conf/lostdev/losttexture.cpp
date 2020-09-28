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
