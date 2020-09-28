// ============================================================================
//
//  LostCubeTexture.cpp
//
// ============================================================================

#include "LostDev.h"
#include "CTextures.h"
#include "CBuffers.h"

/*VOLUMEVERTEX CLostCubeTexture::m_Vertices[18] = 
{
	{50,  150, 0.5, 0xffffff00, 1.0, 0.0, 1.0},
	{100, 50,  0.5, 0xffffffff, 0.0, 0.5, 1.0},
	{150, 150, 0.5, 0xffffffff, 1.0, 1.0, 1.0},

	{100, 50,  0.5, 0xffffffff, 1.0, 0.0, 0.0},
	{200, 50,  0.5, 0xffffff00, 0.0, 0.5, 0.0},
	{150, 150, 0.5, 0xffffffff, 1.0, 1.0, 0.0},

	{150, 150, 0.5, 0xffffffff, 1.0, 0.0, 0.0},
	{200, 50,  0.5, 0xffffffff, 0.0, 0.5, 1.0},
	{250, 150, 0.5, 0xf00fffff, 1.0, 1.0, 0.0},

	{50,  150, 0.5, 0xffffffff, 0.0, -1.0, 0.0},
	{150, 150, 0.5, 0xffffffff, 0.0, -1.0, 0.0},
	{100, 250, 0.5, 0xffff00ff, 0.0, -1.0, 0.0},

	{100, 250, 0.5, 0xffff000f, 0.0, 1.0, 0.0},
	{150, 150, 0.5, 0xffffffff, 0.0, 1.0, 0.0},
	{200, 250, 0.5, 0xffffffff, 0.0, 1.0, 0.0},

	{150, 150, 0.5, 0xf00fffff, 0.0, 0.0, -1.0},
	{250, 150, 0.5, 0xffffffff, 0.0, 0.0, -1.0},
	{200, 250, 0.5, 0xffffffff, 0.0, 0.0, -1.0}
};*/


/*
t-blakep
The VOLUMEVERTEX type defined in LostDev.h is untransformed (does not contain rhw or w)
Therefore after it is multipied by the world, view, and projection matrices (in this case all are identity),
the position or each vertex should be bounded by [-1.0, 1.0] so that all the vertices fit on the screen.
In the m_Vertices array I commented out above, the vertex positions look like they were meant for a 
transformed vertex type and thus the vertex coordinates are bounded by [0,PixelsInScreen].
*/
VOLUMEVERTEX CLostCubeTexture::m_Vertices[18] = 
{
	{0.4f, 0.0f, 0.5f, 0xffffff00, 1.0f, 0.0f, 1.0f},
	{-.5f, -.5f,  0.5f, 0xffffffff, 0.0f, 0.5f, 1.0f},
	{-.5f, .0f, 0.5f, 0xffffffff, 1.0f, 1.0f, 1.0f},

	{0.4f, 0.0f,  0.5f, 0xffffffff, 1.0f, 0.0f, 0.0},
	{.75, -.5,  0.5f, 0xffffff00, 0.0f, 0.5f, 0.0},
	{-.5, -.5, 0.5f, 0xffffffff, 1.0f, 1.0f, 0.0},

	{0.4f, 0.0f, 0.5f, 0xffffffff, 1.0f, 0.0f, 0.0},
	{1.0f, 0.0f,  0.5f, 0xffffffff, 0.0f, 0.5f, 1.0f},
	{0.75, -.5, 0.5f, 0xf00fffff, 1.0f, 1.0f, 0.0},

	{0.4f,  0.0f, 0.5f, 0xffffffff, 0.0f, -1.0f, 0.0},
	{0.75, .5, 0.5f, 0xffffffff, 0.0f, -1.0f, 0.0},
	{1.0f, 0.0f, 0.5f, 0xffff00ff, 0.0f, -1.0f, 0.0},

	{0.4f, 0.0f, 0.5f, 0xffff000f, 0.0f, 1.0f, 0.0},
	{-.5, .5, 0.5f, 0xffffffff, 0.0f, 1.0f, 0.0},
	{.75, .5, 0.5f, 0xffffffff, 0.0f, 1.0f, 0.0},

	{0.4f, 0.0f, 0.5f, 0xf00fffff, 0.0f, 0.0f, -1.0f},
	{-.5, 0.0f, 0.5f, 0xffffffff, 0.0f, 0.0f, -1.0f},
	{-.5, .5, 0.5f, 0xffffffff, 0.0f, 0.0f, -1.0f}
};

// ----------------------------------------------------------------------------
// ****************************	CLostCubeTexture *****************************
// ----------------------------------------------------------------------------

CLostCubeTexture::CLostCubeTexture()
{
	m_szTestName = _T("LostCubeTexture");
	m_szCommandKey = _T("LostCubeTexture");
	m_TestName = _T("Lost Devices with CreateCubeTexture");

	m_Options.D3DOptions.fMinDXVersion = 7.f;

	m_pVB = NULL; 
	m_pCubeTexA = NULL;
	m_pCubeTexB = NULL;
}

// ----------------------------------------------------------------------------

bool CLostCubeTexture::CapsCheck()
{
	if (! CLostBaseTexture::CapsCheck() )
		return false;

	if ( ! (m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP))
		return false;

	m_bMipMapSupported = (0 != (m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP));

	return true;
}

// ----------------------------------------------------------------------------

UINT CLostCubeTexture::TestInitialize()
{
	UINT uResult = CLostDev::TestInitialize();

	// Fill the triangle points
/*    m_Vertices[0].x = 150.0f;
	m_Vertices[0].y = 250.0f;
	m_Vertices[0].z = 0.5f;
	m_Vertices[0].rhw = 1.0f;
	m_Vertices[0].color = 0xffff0000;

	m_Vertices[1].x = 100.0f;
	m_Vertices[1].y = 100.0f;
	m_Vertices[1].z = 0.5f;
	m_Vertices[1].rhw = 1.0f;
	m_Vertices[1].color = 0xff00ff00;

	m_Vertices[2].x = 200.0f;
	m_Vertices[2].y = 100.0f;
	m_Vertices[2].z = 0.5f;
	m_Vertices[2].rhw = 1.0f;
	m_Vertices[2].color = 0xff0000ff;

*/ /*   m_Vertices[0].x = 250.0f;
	m_Vertices[0].y =  50.0f;
	m_Vertices[0].z = 0.5f;
	m_Vertices[0].color = 0xff00ffff;
	m_Vertices[0].tu = 1.0f;
	m_Vertices[0].tv = 1.0f;
	m_Vertices[0].tw = 0.0f;

	m_Vertices[1].x = 50.0f;
	m_Vertices[1].y = 50.0f;
	m_Vertices[1].z = 0.5f;
	m_Vertices[1].color = 0xffffff00;
	m_Vertices[1].tu = 0.0f;
	m_Vertices[1].tv = 1.0f;
	m_Vertices[1].tw = 0.0f;

	m_Vertices[2].x = 50.0f;
	m_Vertices[2].y = 250.0f;
	m_Vertices[2].z = 0.5f;
	m_Vertices[2].color = 0xffffffff;
	m_Vertices[2].tu = 1.0f;
	m_Vertices[2].tv = 0.0f;
	m_Vertices[2].tw = 0.0f;

/*    m_Vertices[3].x = 250.0f;
	m_Vertices[3].y = 250.0f;
	m_Vertices[3].z = 0.5f;
	m_Vertices[3].color = 0xffffffff;
	m_Vertices[3].tu = 0.0f;
	m_Vertices[3].tv = 0.0f;
	m_Vertices[3].tw = 0.0f;
*/
	return uResult;
}

// ----------------------------------------------------------------------------

bool CLostCubeTexture::CreateResource
(
	LPCUBETEXTURES & pCubeTex,
	ULONG lDev
)
{
	//
	// Validate parameters.
	//

	// Verify that we're not about to leak a texture.
	assert( pCubeTex == NULL );
	if ( pCubeTex != NULL )
	{
		DPF( 1, _T("CLostCubeTexture::CreateResource:  pCubeTex should be NULL.\n") );
		return false;
	};


	/*
	//
	// Check the device format.
	//

	m_pD3D->CheckDeviceFormat
	(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_PresParams.BackBufferFormat,
		0,
		RTYPE_CUBETEXTURE,
		m_PresParams.BackBufferFormat
	);
	*/


	//
	// Create the vertex buffer.
	//
	// N.B.:
	// Here we are allocating enough memory (from the managed
	// pool) to hold our 3 custom vertices.  We also specify
	// the FVF, so the vertex buffer knows what data it contains.
	//

	if
	(
		!CreateVertexBuffer
		(
			18 * sizeof(VOLUMEVERTEX),
			0,
			D3DFVF_VOLUMEVERTEX,
			POOL_MANAGED,
			&m_pVB,
			lDev
		)
	)
	{
		DPF( 1, _T("CLostCubeTexture::CreateResource:  CreateVertexBuffer failed.\n") );
		return false;
	};


	//
	// Create the cube texture.
	//

	if
	(
		!CreateCubeTexture
		(
			64,
			m_nMipLevels,
			0,
			m_TexFormat,
			m_Pool,
			&pCubeTex,
			lDev
		)
	)
	{
		DPF( 1, _T("CLostCubeTexture::CreateResource:  CreateCubeTexture failed.\n") );
		return false;
	};

	return true;
}

// ----------------------------------------------------------------------------

bool CLostCubeTexture::CreateResource(ULONG lDev)
{
	// Create the texture
	return CreateResource(m_pCubeTexA, lDev);
}

// ----------------------------------------------------------------------------

bool CLostCubeTexture::CreateResourceB(ULONG lDev)
{
	// Create the texture
	if (! CreateResource(m_pCubeTexB, lDev) )
	{
		DPF( 1, _T("CreateResource() B failed\n"));
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostCubeTexture::LockData(ULONG lDev)
{
	// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
	// gain access to the vertices. This mechanism is required because vertex
	// buffers may be in device memory.
	if( ! m_pVB->Lock( 0, sizeof(m_Vertices), (BYTE**)&m_pVertices, (m_bLockReadOnly ? D3DLOCK_READONLY : 0), lDev ) )
	{
		DPF( 1, _T("LockData() Vertex failed\n") );
		return false;
	}

	RECT* pRect = NULL;
	RECT rect;

	for (int iLevel=0; iLevel<m_nMipLevels; iLevel++)
	{
		if (m_bUseSubRectLock)
		{
			SURFACEDESC	Desc;
			if ( ! m_pCubeTexA->GetLevelDesc(iLevel, &Desc, lDev) )
			{
				DPF( 1, _T("LockData() GetLevelDesc failed\n"));
				return false;
			}
			rect.top = Desc.dwHeight - (Desc.dwHeight / 4);
			rect.left = Desc.dwWidth - (Desc.dwWidth / 4);
			rect.bottom = Desc.dwHeight - 1;
			rect.right = Desc.dwWidth - 1;
			pRect = &rect;
		}
		// Lock all the face of the cube texture 
		for (int iCubeFace=0; iCubeFace<6; iCubeFace++)
		{
			if( ! m_pCubeTexA->LockRect((CUBEMAP_FACES)iCubeFace, iLevel, &m_LockedRect[iCubeFace][iLevel],NULL, (m_bLockReadOnly ? D3DLOCK_READONLY : 0), lDev ) )
			{
				DPF( 1, _T("LockData() LockRect failed for face %d\n"), iCubeFace );
				return false;
			}
		}
	}

   return true;
}

// ----------------------------------------------------------------------------

bool CLostCubeTexture::FillAndUnlockData(ULONG lDev)
{
	int iCubeFace;
	SURFACEDESC	Desc;

	for (int iLevel=0; iLevel<m_nMipLevels; iLevel++)
	{
		if ( ! m_pCubeTexA->GetLevelDesc(iLevel, &Desc, lDev) )
		{
			DPF( 1, _T("FillAndUnLockData() GetLevelDesc failed\n") );
			return false;
		}

		for (iCubeFace=0;iCubeFace<6;iCubeFace++)
		{
			if ( ! FillSurface(&Desc, &m_LockedRect[iCubeFace][iLevel], m_bUseSubRectLock) )
			{
				DPF( 1, _T("FillAndUnLockData() FillSurface failed\n"));
				return false;
			}

			if (! m_pCubeTexA->UnlockRect((CUBEMAP_FACES)iCubeFace, iLevel, lDev ) )
			{
				DPF( 1, _T("FillAndUnlockData() UnlockRect failed for face %d\n"), iCubeFace);
				return false;
			}
		}
	}

	//TO REMOVE
/*	// UnLock all the face of the cube texture 
	for (iCubeFace=0; iCubeFace<6; iCubeFace++)
	{
		if (! m_pCubeTex->UnlockRect((CUBEMAP_FACES)iCubeFace, 0, lDev ) )
		{
			DPF( 1, _T("FillAndUnlockData() UnlockRect failed for face %d\n"), iCubeFace);
			return false;
		}
	}
*/		

	// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
	// gain access to the vertices. This mechanism is required because vertex
	// buffers may be in device memory.
	if (! memcpy( m_pVertices, m_Vertices, sizeof(m_Vertices) ) )
	{
		DPF( 1, _T("FillAndUnlockData() memcpy VB failed\n"));
		return false;
	}
	if (! m_pVB->Unlock(lDev) )
	{
		DPF( 1, _T("FillAndUnlockData() Unlock failed\n") );
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostCubeTexture::CopyResAToResB(ULONG lDev)
{
	for (int iLevel=0; iLevel<m_nMipLevels; iLevel++)
	{
		for (int iCubeFace=0;iCubeFace<6;iCubeFace++)
		{
			if ( ! m_bUseCopyRects )
			{
				LOCKED_RECT lockRectA;
				LOCKED_RECT lockRectB;

				SURFACEDESC	Desc;

				if ( ! m_pCubeTexA->GetLevelDesc(iLevel, &Desc, lDev) )
				{
					DPF( 1, _T("CopyResAToResB() GetLevelDesc failed\n"));
					return false;
				}

				//Lock the 2 textures
				if ( !m_pCubeTexA->LockRect( (CUBEMAP_FACES)iCubeFace, iLevel, &lockRectA, NULL, (m_bLockReadOnly ? D3DLOCK_READONLY : 0), lDev) )
				{
					DPF( 1, _T("CopyResAToResB() LockRect A failed\n"));
					return false;
				}

				if ( !m_pCubeTexB->LockRect( (CUBEMAP_FACES)iCubeFace, iLevel, &lockRectB, NULL, (m_bLockReadOnly ? D3DLOCK_READONLY : 0), lDev) )
				{
					DPF( 1, _T("CopyResAToResB() LockRect B failed\n"));
					return false;
				}

				//Copy A to B
				if ( !CopyFromAToB(&lockRectA, &lockRectB, &Desc) )
				{
					DPF( 1, _T("CopyResAToResB() memcpy failed\n"));
					return false;
				}


				//Unlock the 2 textures
				if ( !m_pCubeTexA->UnlockRect( (CUBEMAP_FACES)iCubeFace, iLevel, lDev) )
				{
					DPF( 1, _T("CopyResAToResB() UnLockRect A failed\n"));
					return false;
				}

				if ( !m_pCubeTexB->UnlockRect( (CUBEMAP_FACES)iCubeFace, iLevel, lDev) )
				{
					DPF( 1, _T("CopyResAToResB() UnLockRect B failed\n"));
					return false;
				}
			}
			else
			{
				bool bRes = true;

				LPSURFACES pSurfA = NULL;
				LPSURFACES pSurfB = NULL;

				if (! m_pCubeTexA->GetCubeMapSurface( (CUBEMAP_FACES)iCubeFace, iLevel, &pSurfA, lDev) ||
					! m_pCubeTexB->GetCubeMapSurface( (CUBEMAP_FACES)iCubeFace, iLevel, &pSurfB, lDev) )
				{
					DPF( 1, _T("CopyResAToResB() GetCubeMapSurface Failed\n"));
					bRes = false;
					goto Error;
				}

				if (! CopyRects(pSurfA, NULL, 0, pSurfB, NULL, lDev) )
				{
					DPF( 1, _T("CopyResAToResB() CopyRects Failed\n"));
					bRes = false;
					goto Error;
				}

			Error:
				RELEASE(pSurfA);
				RELEASE(pSurfB);
				if (!bRes)
					return false;
			}
		}
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostCubeTexture::DrawResource(LPCUBETEXTURES pCubeTex, ULONG lDev)
{
	//render the shape
	if (BeginScene(lDev))
	{
		/*
		// Turn on texture-coord generation for cubemapping
//        SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU  );
			SetRenderState(D3DRS_LASTPIXEL, (DWORD)TRUE);
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
			SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO);
			SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
			SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);
			SetRenderState(D3DRS_WRAP0, (DWORD)0);
			SetSamplerState(0, SAMP_MINFILTER, (DWORD)TEXF_LINEAR);
			SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE );
			//SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_TEXTURE );
			SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);//
			SetSamplerState(0, SAMP_ADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
			SetSamplerState(0, SAMP_ADDRESSV, (DWORD)D3DTADDRESS_MIRROR);

		//SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
  //      SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE );*/




		/*
		t-blakep
		Because VOLUMEVERTEX is untransformed, by default D3D will try to light the vertices.
		So turn lighting off.
		*/
		if(!SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE))
		{
			DPF( 1, _T("Failed to turn lighting off\n") );
			goto Exit;
            
		}

		//Set the Texture for the triangle as the cube Texture
		if (! SetTexture( 0, pCubeTex, lDev ) )
		{
			DPF( 1, _T("DrawResource() SetTexture() failed\n") );
			goto Exit;
		}

		/*if (m_fDXVersion >= 8.0f)
		{
			if (! SetStreamSource( 0, m_pVB, sizeof(VOLUMEVERTEX), 0, lDev ) )
			{
				DPF( 1, _T("DrawResource() SetStreamSource() failed\n") );
				goto Exit;
			}

			if (! SetVertexShader( D3DFVF_VOLUMEVERTEX, lDev ) )
			{
				DPF( 1, _T("DrawResource() SetVertexShader() failed\n") );
				goto Exit;
			}

			if (! DrawPrimitive( D3DPT_TRIANGLELIST, 0, 6, lDev) )
			{
				DPF( 1, _T("DrawResource() DrawPrimitive() failed\n") );
				goto Exit;
			}
		}
		else
		{*/
		/*
		t-blakep
		The m_pVB scheme is fishy.  
		I believe to use VBs in this case you need a m_pVB_A and a m_pVB_B instead of just one vertex buffer
		*/
			if(!RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VOLUMEVERTEX, m_Vertices, sizeof(m_Vertices)/sizeof(m_Vertices[0]), NULL, 0, NULL, lDev))
			{
				DPF( 1, _T("RenderPrimtiive failed\n"));
				goto Exit;
			}
		//}

		if (! EndScene(lDev) )
		{
			DPF( 1, _T("DrawResource() EndScene() failed\n") );
			goto Exit;
		}

		if (! SetTexture( 0, NULL, lDev ) )
		{
			DPF( 1, _T("DrawResource() SetTexture(NULL) failed\n") );
			goto Exit;
		}

	}
	else
	{
		DPF( 1, _T("DrawResource() BeginScene() failed\n") );
		goto Exit;
	}

	return true;

Exit:
	if (! EndScene(lDev) )
	{
		DPF( 1, _T("DrawResource() EndScene() failed\n") );
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CLostCubeTexture::DrawResource(ULONG lDev)
{
	if(! DrawResource(m_pCubeTexA, lDev) )
	{
		DPF( 1, _T("DrawResource() DrawResource failed\n"));
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostCubeTexture::DrawResourceB(ULONG lDev)
{
	if(! DrawResource(m_pCubeTexB, lDev) )
	{
		DPF( 1, _T("DrawResourceB() DrawResource B failed\n"));
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CLostCubeTexture::ReleaseResource(ULONG lDev)
{
	bool bOverallResult = true;

	__try
	{
		RELEASE(m_pVB);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing vertex buffer.\n") );
		bOverallResult = false;
	};

	__try
	{
		RELEASE(m_pCubeTexA);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing cube texture A.\n") );
		bOverallResult = false;
	};

	__try
	{
		RELEASE(m_pCubeTexB);
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
		DPF( 1, _T("ReleaseResource() - Access violation caught while releasing cube texture B.\n") );
		bOverallResult = false;
	};

	return bOverallResult;
}


