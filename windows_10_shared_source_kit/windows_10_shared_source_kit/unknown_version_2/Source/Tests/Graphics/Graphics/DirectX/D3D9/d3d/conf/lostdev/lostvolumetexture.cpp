// ============================================================================
//
//  LostVolumeTexture.cpp
//
// ============================================================================

#include "LostDev.h"
#include "CTextures.h"
#include "CBuffers.h"

// ----------------------------------------------------------------------------
// ****************************	CLostVolumeTexture *****************************
// ----------------------------------------------------------------------------

CLostVolumeTexture::CLostVolumeTexture()
{
	m_szTestName = _T("LostVolumeTexture");
	m_szCommandKey = _T("LostVolumeTexture");
	m_TestName = _T("Lost Devices with CreateVolumeTexture");

	m_pVolumeTexA = NULL;
	m_pVolumeTexB = NULL;
}

// ----------------------------------------------------------------------------

bool CLostVolumeTexture::CapsCheck()
{
	if (! CLostDev::CapsCheck() )
		return false;

	if ( ! (m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP))
		return false;

	m_TexFormat = GetValidFormat(RTYPE_VOLUMETEXTURE);

	if (m_TexFormat == FMT_ANY)
	{
		DPF(1, "No VolumeTexture format supported\n");
		return false;
	}

	m_bMipMapSupported = (0 != (m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP) );

	return true;
}

// ----------------------------------------------------------------------------

UINT CLostVolumeTexture::TestInitialize()
{
	UINT uResult = CLostDev::TestInitialize();

	/*
	t-blakep
	Same problem as in LostCubeMap.  VolumeVertex is untransformed
	so VolumeVertex positions need to end up in [-1.0, 1.0] to be on the screen
	*/

	// Fill the triangle points
	m_Vertices[0].x = -1.0f;
	m_Vertices[0].y =  0.0f;
	m_Vertices[0].z = 0.5f;
	m_Vertices[0].color = 0xffffffff;
	m_Vertices[0].tu = 1.0f;
	m_Vertices[0].tv = 1.0f;
	m_Vertices[0].tw = 0.0f;

	m_Vertices[1].x = 0.0f;
	m_Vertices[1].y = 1.0f;
	m_Vertices[1].z = 0.5f;
	m_Vertices[1].color = 0xffffffff;
	m_Vertices[1].tu = 0.0f;
	m_Vertices[1].tv = 1.0f;
	m_Vertices[1].tw = 0.5f;

	m_Vertices[2].x = 0.0f;
	m_Vertices[2].y = -1.0f;
	m_Vertices[2].z = 0.5f;
	m_Vertices[2].color = 0xffffffff;
	m_Vertices[2].tu = 1.0f;
	m_Vertices[2].tv = 0.0f;
	m_Vertices[2].tw = 0.0f;

	m_Vertices[3].x = 1.0;
	m_Vertices[3].y = 0.5f;
	m_Vertices[3].z = 0.5f;
	m_Vertices[3].color = 0xffffffff;
	m_Vertices[3].tu = 0.0f;
	m_Vertices[3].tv = 0.0f;
	m_Vertices[3].tw = 1.0f;

 


	return uResult;
}

// ----------------------------------------------------------------------------

bool CLostVolumeTexture::CreateResource(ULONG lDev)
{
	//
	// Validate members.
	//

	// Verify that we're not about to leak a volume texture.
	assert( m_pVolumeTexA == NULL );
	if ( m_pVolumeTexA != NULL )
	{
		DPF( 1, _T("CLostVolumeTexture::CreateResource:  m_pVolumeTexA should be NULL.\n") );
		return false;
	};


	return CreateResource(m_pVolumeTexA, lDev);
}

// ----------------------------------------------------------------------------

bool CLostVolumeTexture::CreateResourceB(ULONG lDev)
{
	return CreateResource(m_pVolumeTexB, lDev);
}

// ----------------------------------------------------------------------------


bool CLostVolumeTexture::CreateResource
(
	LPVOLUMETEXTURES & pTexture,
	ULONG lDev
)
{
	//
	// Validate parameters.
	//

	assert( pTexture == NULL );
	if ( pTexture != NULL )
	{
		DPF( 1, _T("CLostVolumeTexture::CreateResource:  pTexture should be NULL.\n") );
		return false;
	};


	//
	// Create the volume texture.
	//
	// N.B. [t-blakep]:
	// Made depth a power of 2 so that it works on devices which
	// only support pow2 volumes.  Otherwise we would need to
	// to check the device caps to make sure nonpow2 volume textures
	// are supported.
	//

	if
	(
		!CreateVolumeTexture
		(
			16,
			16,
			16,
			1,
			0,
			m_TexFormat,
			m_Pool,
			&pTexture,
			lDev
		) ||
		!pTexture
	)
	{
		DPF( 1, _T("CLostVolumeTexture::CreateResource:  CreateVolumeTexture failed.\n"));
		return false;
	};

	return true;
}

// ----------------------------------------------------------------------------

bool CLostVolumeTexture::LockData(ULONG lDev)
{
	// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
	// gain access to the vertices. This mechanism is required becuase vertex
	// buffers may be in device memory.
	/*if( ! m_pVB->Lock( 0, sizeof(m_Vertices), (BYTE**)&m_pVertices, (m_bLockReadOnly ? D3DLOCK_READONLY : 0), lDev ) )
	{
		DPF( 1, _T("LockData() VB->Lock() failed\n"));
		return false;
	}*/

	// Lock the volume texture 
	if( ! m_pVolumeTexA->LockBox( 0, &m_LockedBox, NULL, (m_bLockReadOnly ? D3DLOCK_READONLY : 0), lDev ) )
	{
		DPF( 1, _T("LockData() LockBox failed\n"));
		return false;
	}

   return true;
}

// ----------------------------------------------------------------------------

bool CLostVolumeTexture::FillAndUnlockData(ULONG lDev)
{
	// Fill the volume texture
	for( UINT w=0; w<16; w++ )
	{
		BYTE* pSliceStart = (BYTE*)m_LockedBox.pBits;

		for( UINT v=0; v<16; v++ )
		{
			for( UINT u=0; u<16; u++ )
			{
				FLOAT du = (u-7.5f)/7.5f;
				FLOAT dv = (v-7.5f)/7.5f;
				FLOAT dw = (w-7.5f)/7.5f;
				FLOAT fScale = sqrtf( du*du + dv*dv + dw*dw ) / sqrtf(1.0f);

				if( fScale > 1.0f ) fScale = 0.0f;
				else                fScale = 1.0f - fScale;

									DWORD r = (DWORD)((w<<4)*fScale);
									DWORD g = (DWORD)((v<<4)*fScale);
									DWORD b = (DWORD)((u<<4)*fScale);

				((DWORD*)m_LockedBox.pBits)[u] = 0xff000000 + (r<<16) + (g<<8) + (b);
			}
			m_LockedBox.pBits = (BYTE*)m_LockedBox.pBits + m_LockedBox.RowPitch;
		}
		m_LockedBox.pBits = pSliceStart + m_LockedBox.SlicePitch;
	}

	if (! m_pVolumeTexA->UnlockBox( 0, lDev ) )
	{
		DPF( 1, _T("FillAndUnlockData() UnlockBox failed\n"));
		return false;
	}
		

	// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
	// gain access to the vertices. This mechanism is required becuase vertex
	// buffers may be in device memory.
	/*if (! memcpy( m_pVertices, m_Vertices, sizeof(m_Vertices) ) )
	{
		DPF( 1, _T("FillAndUnlockData() memcpy VB failed\n"));
		return false;
	}
	if (! m_pVB->Unlock(lDev) )
	{
		DPF( 1, _T("FillAndUnlockData() VB->Unlock() failed\n"));
		return false;
	}*/

	return true;
}

// ----------------------------------------------------------------------------

bool CLostVolumeTexture::DrawResource(LPVOLUMETEXTURES pTexture, ULONG lDev)
{
	//render the shape
	if (BeginScene(lDev))
	{
		if (! SetTexture( 0, pTexture, lDev ) )
		{
			DPF( 1, _T("DrawResource() SetTexture() failed\n"));
			goto Exit;
		}

		/*
		t-blakep
		Same m_pVB problem as in cube map.  To use them, you need a m_pVB_A and a m_pVB_B
        
		Also, because VOLUMEVERTEX is untransformed, by default D3D will try to light the vertices.
		So turn lighting off.
		*/

		if(! SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE, lDev))
		{
			DPF( 1, _T("Failed to turn lighting off\n"));
			goto Exit;
		}

		if (! RenderPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VOLUMEVERTEX, m_Vertices, sizeof(m_Vertices)/sizeof(m_Vertices[0]), NULL, 0, NULL, lDev))
		{
			DPF( 1, _T("RenderPrimitive failed\n"));
			goto Exit;
		}


		/*if (! SetStreamSource( 0, m_pVB, sizeof(VOLUMEVERTEX), 0, lDev ) )
		{
			DPF( 1, _T("DrawResource() SetStreamSource() failed\n"));
			goto Exit;
		}

		if (! SetVertexShader( D3DFVF_VOLUMEVERTEX, lDev ) )
		{
			DPF( 1, _T("DrawResource() SetVertexShader() failed\n"));
			goto Exit;
		}

		if (! DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2, lDev) )
		{
			DPF( 1, _T("DrawResource() DrawPrimitive() failed\n"));
			goto Exit;
		}*/

		if (! EndScene(lDev) )
		{
			DPF( 1, _T("DrawResource() EndScene() failed\n"));
			goto Exit;
		}
	}
	else
	{
		DPF( 1, _T("DrawResource() BeginScene() failed\n"));
		goto Exit;
	}

	return true;

Exit:
	EndScene(lDev);

	return false;
}

// ----------------------------------------------------------------------------

bool CLostVolumeTexture::DrawResource(ULONG lDev)
{
	return DrawResource(m_pVolumeTexA, lDev);
}

// ----------------------------------------------------------------------------

bool CLostVolumeTexture::DrawResourceB(ULONG lDev)
{
	return DrawResource(m_pVolumeTexB, lDev);
}

// ----------------------------------------------------------------------------

bool CLostVolumeTexture::ReleaseResource(ULONG lDev)
{
	bool bOverallResult = true;

	__try
	{
		RELEASE(m_pVolumeTexA);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing volume texture A.\n") );
		bOverallResult = false;
	};

	__try
	{
		RELEASE(m_pVolumeTexB);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing volume texture B.\n") );
		bOverallResult = false;
	};

	return bOverallResult;
}

// ----------------------------------------------------------------------------
bool CLostVolumeTexture::CopyResAToResB(ULONG lDev)
{
	bool bRes = true;


	for(UINT nMipLevel = 0; nMipLevel < m_pVolumeTexA->GetLevelCount(); nMipLevel++)
	{
		LOCKED_BOX SrcBox;
		LOCKED_BOX DestBox;

		bRes &= m_pVolumeTexA->LockBox(nMipLevel, &SrcBox, NULL, 0, lDev);
		bRes &= m_pVolumeTexB->LockBox(nMipLevel, &DestBox, NULL, 0, lDev);
        

		VOLUME_DESC Desc;
		bRes &= m_pVolumeTexA->GetLevelDesc(nMipLevel, &Desc, lDev);

		UINT nBytesPerPixel = Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8;

		if(bRes)
		{
			for(UINT nSlice = 0; nSlice < Desc.Depth; nSlice++)
			{
				for(UINT nRow = 0; nRow < Desc.Height; nRow++)
				{
					void* pSrc = ((BYTE*)SrcBox.pBits) + 
									(nSlice * SrcBox.SlicePitch) + 
									(nRow * SrcBox.RowPitch);

					void* pDest = ((BYTE*)DestBox.pBits) + 
									(nSlice * DestBox.SlicePitch) + 
									(nRow * DestBox.RowPitch);

					memcpy(pDest, pSrc, Desc.Width * nBytesPerPixel);
				}
			}
		}

		bRes &= m_pVolumeTexA->UnlockBox(nMipLevel, lDev);
		bRes &= m_pVolumeTexB->UnlockBox(nMipLevel, lDev);

	}

	return bRes;
}


/*t-blakep
There is no equialent for CopyRects() for volume textures, so we can't copy pool_default volumes
*/
void CLostVolumeTexture::SetTestFactors()
{
	m_rCasesManager = m_rLostWays *
						  (	m_rPoolManaged + m_rPoolSystemMem );
}
