Caps;
	pDevice->m_Desc.dwShadeCaps = DevCaps.ShadeCaps;
	pDevice->m_Desc.dwTextureCaps = DevCaps.TextureCaps;
	pDevice->m_Desc.dwTextureFilterCaps = DevCaps.TextureFilterCaps;
	pDevice->m_Desc.dwCubeTextureFilterCaps = DevCaps.CubeTextureFilterCaps;
	pDevice->m_Desc.dwVolumeTextureFilterCaps = DevCaps.VolumeTextureFilterCaps;
	pDevice->m_Desc.dwTextureAddressCaps = DevCaps.TextureAddressCaps;
	pDevice->m_Desc.dwVolumeTextureAddressCaps = DevCaps.VolumeTextureAddressCaps;
	pDevice->m_Desc.dwLineCaps = DevCaps.LineCaps;
	
	pDevice->m_Desc.dwMinTextureWidth = 1;
	pDevice->m_Desc.dwMinTextureHeight = 1;
	pDevice->m_Desc.dwMaxTextureWidth = DevCaps.MaxTextureWidth;
	pDevice->m_Desc.dwMaxTextureHeight = DevCaps.MaxTextureHeight;
	pDevice->m_Desc.dwMinVolumeExtent = 1;
	pDevice->m_Desc.dwMaxVolumeExtent = DevCaps.MaxVolumeExtent;
	pDevice->m_Desc.dwMaxTextureRepeat = DevCaps.MaxTextureRepeat;
	pDevice->m_Desc.dwMaxTextureAspectRatio = DevCaps.MaxTextureAspectRatio;
	pDevice->m_Desc.dwMaxAnisotropy = DevCaps.MaxAnisotropy;
	pDevice->m_Desc.dvMaxVertexW = DevCaps.MaxVertexW;
	pDevice->m_Desc.dvGuardBandLeft = DevCaps.GuardBandLeft;
	pDevice->m_Desc.dvGuardBandTop = DevCaps.GuardBandTop;
	pDevice->m_Desc.dvGuardBandRight = DevCaps.GuardBandRight;
	pDevice->m_Desc.dvGuardBandBottom = DevCaps.GuardBandBottom;
	pDevice->m_Desc.dvExtentsAdjust = DevCaps.ExtentsAdjust;
	pDevice->m_Desc.dwStencilCaps = DevCaps.StencilCaps;
	pDevice->m_Desc.dwFVFCaps = DevCaps.FVFCaps;
	pDevice->m_Desc.dwTextureOpCaps = DevCaps.TextureOpCaps;
	pDevice->m_Desc.wMaxTextureBlendStages = (WORD)DevCaps.MaxTextureBlendStages;
	pDevice->m_Desc.wMaxSimultaneousTextures = (WORD)DevCaps.MaxSimultaneousTextures;
	pDevice->m_Desc.dwVertexProcessingCaps = DevCaps.VertexProcessingCaps;
	pDevice->m_Desc.dwMaxActiveLights = DevCaps.MaxActiveLights;
	pDevice->m_Desc.wMaxUserClipPlanes = (WORD)DevCaps.MaxUserClipPlanes;
	pDevice->m_Desc.wMaxVertexBlendMatrices = (WORD)DevCaps.MaxVertexBlendMatrices;
	pDevice->m_Desc.dwMaxVertexBlendMatrixIndex = DevCaps.MaxVertexBlendMatrixIndex;
	pDevice->m_Desc.dvMaxPointSize = DevCaps.MaxPointSize;
	pDevice->m_Desc.dwMaxPrimitiveCount = DevCaps.MaxPrimitiveCount;
	pDevice->m_Desc.dwMaxVertexIndex = DevCaps.MaxVertexIndex;
	pDevice->m_Desc.dwMaxStreams = DevCaps.MaxStreams;
	pDevice->m_Desc.dwMaxStreamStride = DevCaps.MaxStreamStride;
	pDevice->m_Desc.dwVertexShaderVersion = DevCaps.VertexShaderVersion;
	pDevice->m_Desc.dwMaxVertexShaderConst = DevCaps.MaxVertexShaderConst;
	pDevice->m_Desc.dwPixelShaderVersion = DevCaps.PixelShaderVersion;
	pDevice->m_Desc.dvMaxPixelShaderValue = DevCaps.MaxPixelShaderValue;
	pDevice->m_Desc.MasterAdapterOrdinal = DevCaps.MasterAdapterOrdinal;
	pDevice->m_Desc.AdapterOrdinal = DevCaps.AdapterOrdinal;
	pDevice->m_Desc.AdapterOrdinalInGroup = DevCaps.AdapterOrdinalInGroup;
	pDevice->m_Desc.NumberOfAdaptersInGroup = DevCaps.NumberOfAdaptersInGroup;
	pDevice->m_Desc.NumSimultaneousRTs = DevCaps.NumSimultaneousRTs;
	pDevice->m_Desc.dwDeclTypes = DevCaps.DeclTypes;
	pDevice->m_Desc.VS20Caps.Caps = DevCaps.VS20Caps.Caps;
	pDevice->m_Desc.VS20Caps.DynamicFlowControlDepth = DevCaps.VS20Caps.DynamicFlowControlDepth;
	pDevice->m_Desc.VS20Caps.NumTemps = DevCaps.VS20Caps.NumTemps;
	pDevice->m_Desc.VS20Caps.StaticFlowControlDepth = DevCaps.VS20Caps.StaticFlowControlDepth;
	pDevice->m_Desc.PS20Caps.Caps = DevCaps.PS20Caps.Caps;
	pDevice->m_Desc.PS20Caps.DynamicFlowControlDepth = DevCaps.PS20Caps.DynamicFlowControlDepth;
	pDevice->m_Desc.PS20Caps.NumTemps = DevCaps.PS20Caps.NumTemps;
	pDevice->m_Desc.PS20Caps.StaticFlowControlDepth = DevCaps.PS20Caps.StaticFlowControlDepth;
	pDevice->m_Desc.PS20Caps.NumInstructionSlots = DevCaps.PS20Caps.NumInstructionSlots;
	pDevice->m_Desc.dwVertexTextureFilterCaps = DevCaps.VertexTextureFilterCaps;
	pDevice->m_Desc.dwMaxVShaderInstructionsExecuted = DevCaps.MaxVShaderInstructionsExecuted;
	pDevice->m_Desc.dwMaxPShaderInstructionsExecuted = DevCaps.MaxPShaderInstructionsExecuted;
	
	pDevice->m_Desc.dwStretchRectFilterCaps = DevCaps.StretchRectFilterCaps;
	pDevice->m_Desc.dwReserved5 = DevCaps.Reserved5;
	pDevice->m_Desc.MaxNpatchTessellationLevel = DevCaps.MaxNpatchTessellationLevel;
	pDevice->m_Desc.MaxPixelShader30InstructionSlots = DevCaps.MaxPixelShader30InstructionSlots;
	pDevice->m_Desc.MaxVertexShader30InstructionSlots = DevCaps.MaxVertexShader30InstructionSlots;

	// Decode the flag if ANY display mode is requested
	if(pAdapter->DesktopMode.Format.d3dfFormat == FMT_ANY)
	{
		FMT PreferredFormat = FMT_X8R8G8B8;
		D3DFORMAT PreferredD3DFormat = D3DFMT_X8R8G8B8;

		FMT AlternateFormat = FMT_R5G6B5;
		D3DFORMAT AlternateD3DFormat = D3DFMT_R5G6B5;

		DEVMODE CurrentMode;
		ZeroMemory( &CurrentMode, sizeof( CurrentMode) );
		CurrentMode.dmSize = sizeof( CurrentMode );
		CurrentMode.dmSpecVersion = DM_SPECVERSION;
		BOOL bGetCurrentModeResult = EnumDisplaySettingsEx
		(
			NULL,
			ENUM_CURRENT_SETTINGS,
			&CurrentMode,
			0
		);

		if
		(
			bGetCurrentModeResult == TRUE &&
			CurrentMode.dmBitsPerPel < 32
		)
		{
			PreferredFormat = FMT_R5G6B5;
			PreferredD3DFormat = D3DFMT_R5G6B5;

			AlternateFormat = FMT_X8R8G8B8;
			AlternateD3DFormat = D3DFMT_X8R8G8B8;
		};

		if
		(
			SUCCEEDED
			(
				CheckDeviceType
				(
					nAdapter,
					pDevice->GetDevType(),
					(FORMAT) PreferredFormat,
					(FORMAT) PreferredFormat,
					TRUE
				)
			) &&
			IsDisplayModeSupported
			(
				pAdapter,
				pAdapter->DesktopMode.dwWidth,
				pAdapter->DesktopMode.dwHeight,
				PreferredFormat
			)
		)
		{
			pAdapter->DesktopMode.Format.d3dfFormat = PreferredD3DFormat;
		}
		else if
		(
			SUCCEEDED
			(
				CheckDeviceType
				(
					nAdapter,
					pDevice->GetDevType(),
					(FORMAT) AlternateFormat,
					(FORMAT) AlternateFormat,
					TRUE
				)
			) &&
			IsDisplayModeSupported
			(
				pAdapter,
				pAdapter->DesktopMode.dwWidth,
				pAdapter->DesktopMode.dwHeight,
				AlternateFormat
			)
		)
		{
			pAdapter->DesktopMode.Format.d3dfFormat = AlternateD3DFormat;
		}
		else
		{
			DPF(1, _T("D3D Device is not hardware accelerated in any supported display modes.\n"));
			goto Error;
		};
	};

	d3df2ddpf( pAdapter->DesktopMode.Format.d3dfFormat, &pAdapter->DesktopMode.Format.ddpfPixelFormat);
	pAdapter->DesktopMode.dwBPP = pAdapter->DesktopMode.Format.ddpfPixelFormat.dwRGBBitCount;

	// Enumerate Z buffer / stencil formats
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pZBufferFormat);
	
		pFmt = &(pDevice->m_pZBufferFormat);
		for (dwCount = 0; dwCount < g_dwZBufferFormatListSize9; dwCount++)
		{
			FORMAT fmt(g_ZBufferFormatList9[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, USAGE_DEPTHSTENCIL, RTYPE_SURFACE, fmt);
			if (FAILED(hr))
				continue;
			
			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
				goto Error;
			}
			memcpy(*pFmt, &fmt, sizeof(FORMAT));
			(*pFmt)->pNext = NULL;

			pDevice->m_nZBufferFormat++;
			pFmt = &((*pFmt)->pNext);
		}
	}

	// Enumerate texture formats
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pTextureFormat);

		pFmt = &(pDevice->m_pTextureFormat);
		for (dwCount = 0; dwCount < g_dwTextureFormatListSize9; dwCount++)
		{
			FORMAT fmt(g_TextureFormatList9[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, 0, RTYPE_TEXTURE, fmt);
			if (FAILED(hr))
				continue;

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
				goto Error;
			}
			memcpy(*pFmt, &fmt, sizeof(FORMAT));
			(*pFmt)->pNext = NULL;

			pDevice->m_nTextureFormat++;
			pFmt = &((*pFmt)->pNext);
		}
	}

	// Enumerate cube texture formats
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pCubeFormat);

		pFmt = &(pDevice->m_pCubeFormat);
		for (dwCount = 0; dwCount < g_dwTextureFormatListSize9; dwCount++)
		{
			FORMAT fmt(g_TextureFormatList9[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, 0, RTYPE_CUBETEXTURE, (FORMAT)g_TextureFormatList9[dwCount]);
			if (FAILED(hr))
				continue;

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
				goto Error;
			}
			memcpy(*pFmt, &fmt, sizeof(FORMAT));
			(*pFmt)->pNext = NULL;

			pDevice->m_nCubeFormat++;
			pFmt = &((*pFmt)->pNext);
		}
	}

	// Enumerate volume texture formats
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pVolumeFormat);

		pFmt = &(pDevice->m_pVolumeFormat);
		for (dwCount = 0; dwCount < g_dwTextureFormatListSize9; dwCount++)
		{
			FORMAT fmt(g_TextureFormatList9[dwCount]);
			hr = CheckDeviceFormat(nAdapter, pDevice->GetDevType(), pAdapter->DesktopMode.Format, 0, RTYPE_VOLUMETEXTURE, fmt);
			if (FAILED(hr))
				continue;

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
				goto Error;
			}
			memcpy(*pFmt, &fmt, sizeof(FORMAT));
			(*pFmt)->pNext = NULL;

			pDevice->m_nVolumeFormat++;
			pFmt = &((*pFmt)->pNext);
		}
	}

	// Enumerate BackBuffer types - these require mode information(ModeFormat) so we'll add 
	// them all for now, we'll filter them out later when constructing the modelist
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pBackBufferFormat);

		pFmt = &(pDevice->m_pBackBufferFormat);
		for (dwCount = 0; dwCount < g_dwBackBufferFormats9; dwCount++)
		{
			FORMAT fmt(g_BackBufferFormats9[dwCount]);

			// Allocate the new format
			(*pFmt) = new FORMAT;
			if(*pFmt == NULL)
			{
				DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
				goto Error;
			}
			memcpy(*pFmt, &fmt, sizeof(FORMAT));
			(*pFmt)->pNext = NULL;

			pDevice->m_nBackBufferFormat++;
			pFmt = &((*pFmt)->pNext);
		}
	}

	// Enumerate multisample types - these require mode information(BackBufferFormat) so we'll add 
	// them all for now, we'll filter them out later when constructing the modelist
	if(bRawCaps)
	{
		SAFEDELETEA(pDevice->m_pMultisampType);

		pDevice->m_pMultisampType = new MULTISAMPLE_TYPE[g_dwMultiSampleTypeListSize9];
		if(NULL == pDevice->m_pMultisampType)
		{
			DPF(1, _T("CD3D::DetectCaps9() - Out of memory.\n"));
			goto Error;
		}
		pDevice->m_nMultisampType = g_dwMultiSampleTypeListSize9;
		memcpy(pDevice->m_pMultisampType, &g_MultiSampleTypeList9, sizeof(MULTISAMPLE_TYPE) * g_dwMultiSampleTypeListSize9);
	}

	// Create the device description
	pDevice->UpdateDescription();

	return true;

Error:
	// Device info will get released when the device is destroyed

	return false;
}

// --------------------------------------------------------------------------------------

void CD3D::ProcessError9(HRESULT hr)
{
	// Only set this flag - it'll get reset when the device recovers.
	if(hr == E_OUTOFMEMORY)
		m_bOutofMemory = true;

	if(hr == D3DERR_DEVICELOST)
		m_bDeviceLost = true;

	if(hr == D3DERR_DRIVERINTERNALERROR)
		m_bDriverInternalError = true;
}

// --------------------------------------------------------------------------------------

bool CD3D::CompareFormats9(FORMAT *pSrcFmt, FORMAT *pRefFmt)
{
	if(!pSrcFmt || !pRefFmt)
	{
		DPF(1, _T("CD3D::CompareFormats9() - Invalid parameters.\n"));
		return false;
	}

	return pSrcFmt->d3dfFormat == pRefFmt->d3dfFormat;
}

// --------------------------------------------------------------------------------------
// D3D9 object wrappers
// --------------------------------------------------------------------------------------

HRESULT CD3D::RegisterSoftwareDevice9(void* pInitializeFunction)
{
	return m_pSrcD3D9->RegisterSoftwareDevice( pInitializeFunction );
}

// --------------------------------------------------------------------------------------

UINT CD3D::GetAdapterCount9(void)
{
	return m_pSrcD3D9->GetAdapterCount();
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::GetAdapterIdentifier9(UINT Adapter,DWORD Flags,ADAPTER_IDENTIFIER* pIdentifier)
{
	return m_pSrcD3D9->GetAdapterIdentifier( Adapter,Flags,(D3DADAPTER_IDENTIFIER9*)pIdentifier );
}

// --------------------------------------------------------------------------------------

UINT CD3D::GetAdapterModeCountEx9( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter )
{
	return m_pSrcD3D9Ex->GetAdapterModeCountEx( Adapter, pFilter );
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::GetAdapterDisplayMode9(UINT Adapter,DISPLAYMODE* pMode)
{
	if( m_pSrcD3D9 == NULL )
	{
		DPF(1, _T("CD3D::GetAdapterDisplayMode9() - Invalid D3D pointer!\n"));
		return E_FAIL;
	}

	D3DDISPLAYMODE Mode;

	HRESULT hr = m_pSrcD3D9->GetAdapterDisplayMode( Adapter,pMode==0L?0L:&Mode );

	if( SUCCEEDED(hr) &&  pMode != NULL )
	{
		pMode->dwWidth  = Mode.Width;
		pMode->dwHeight = Mode.Height;
		pMode->dwRefreshRate = Mode.RefreshRate;
		pMode->Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format,&pMode->Format.ddpfPixelFormat);
		pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceType9(UINT Adapter,DWORD DeviceType,FORMAT DisplayFormat,
							   FORMAT BackBufferFormat,BOOL Windowed)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);
	D3DFORMAT  AdptFmt = DisplayFormat.d3dfFormat;

	// REF doesn't support A2R10G10B10 natively in windowed mode.  This hack
	// makes it think we're going to be rendering in a standard 32-bit mode
	// so that we can correctly get all supported formats.
	if ((DevType == D3DDEVTYPE_REF) && (AdptFmt == D3DFMT_A2R10G10B10 || AdptFmt == D3DFMT_A2B10G10R10_XR_BIAS))
		AdptFmt = D3DFMT_X8R8G8B8;

	return m_pSrcD3D9->CheckDeviceType( Adapter,DevType,AdptFmt,BackBufferFormat.d3dfFormat,Windowed );
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceFormat9(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,
								 DWORD Usage,RESOURCETYPE RType,FORMAT CheckFormat)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);
	D3DFORMAT  AdptFmt = AdapterFormat.d3dfFormat;

	// REF doesn't support A2R10G10B10 natively in windowed mode.  This hack
	// makes it think we're going to be rendering in a standard 32-bit mode
	// so that we can correctly get all supported formats.
	if ((DevType == D3DDEVTYPE_REF) && (AdptFmt == D3DFMT_A2R10G10B10 || AdptFmt == D3DFMT_A2B10G10R10_XR_BIAS))
		AdptFmt = D3DFMT_X8R8G8B8;

	return (m_pSrcD3D9->CheckDeviceFormat( Adapter,DevType,AdptFmt,Usage,(D3DRESOURCETYPE)RType,CheckFormat.d3dfFormat));

}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceMultiSampleType8To9(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,
										  BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);
	D3DFORMAT  AdptFmt = AdapterFormat.d3dfFormat;

	// REF doesn't support A2R10G10B10 natively in windowed mode.  This hack
	// makes it think we're going to be rendering in a standard 32-bit mode
	// so that we can correctly get all supported formats.
	if ((DevType == D3DDEVTYPE_REF) && (AdptFmt == D3DFMT_A2R10G10B10 || AdptFmt == D3DFMT_A2B10G10R10_XR_BIAS))
		AdptFmt = D3DFMT_X8R8G8B8;

	return (m_pSrcD3D9->CheckDeviceMultiSampleType( Adapter,DevType,AdptFmt,
												 Windowed,(D3DMULTISAMPLE_TYPE)MultiSampleType, NULL));
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceMultiSampleType9(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,
										  BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType, DWORD *dwQualityLevels)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);
	D3DFORMAT  AdptFmt = AdapterFormat.d3dfFormat;

	// REF doesn't support A2R10G10B10 natively in windowed mode.  This hack
	// makes it think we're going to be rendering in a standard 32-bit mode
	// so that we can correctly get all supported formats.
	if ((DevType == D3DDEVTYPE_REF) && (AdptFmt == D3DFMT_A2R10G10B10 || AdptFmt == D3DFMT_A2B10G10R10_XR_BIAS))
		AdptFmt = D3DFMT_X8R8G8B8;

	return (m_pSrcD3D9->CheckDeviceMultiSampleType( Adapter,DevType,AdptFmt,
												 Windowed,(D3DMULTISAMPLE_TYPE)MultiSampleType, dwQualityLevels));
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDepthStencilMatch9(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,
									  FORMAT RenderTargetFormat,FORMAT DepthStencilFormat)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);
	D3DFORMAT  AdptFmt = AdapterFormat.d3dfFormat;

	// REF doesn't support A2R10G10B10 natively in windowed mode.  This hack
	// makes it think we're going to be rendering in a standard 32-bit mode
	// so that we can correctly get all supported formats.
	if ((DevType == D3DDEVTYPE_REF) && (AdptFmt == D3DFMT_A2R10G10B10 || AdptFmt == D3DFMT_A2B10G10R10_XR_BIAS))
		AdptFmt = D3DFMT_X8R8G8B8;

	return (m_pSrcD3D9->CheckDepthStencilMatch( Adapter,DevType,AdptFmt,
											 RenderTargetFormat.d3dfFormat,
											 DepthStencilFormat.d3dfFormat ));
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::GetDeviceCaps9(UINT Adapter,DWORD DeviceType,CAPS* pCaps)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);

	if(pCaps)
		memset(pCaps, 0, sizeof(CAPS));

	return m_pSrcD3D9->GetDeviceCaps( Adapter,DevType,(D3DCAPS9*)pCaps );
}

// --------------------------------------------------------------------------------------

HMONITOR CD3D::GetAdapterMonitor9(UINT Adapter)
{
	return (m_pSrcD3D9->GetAdapterMonitor( Adapter ));
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::CheckDeviceFormatConversion9(UINT Adapter,DWORD DeviceType,FORMAT SourceFormat,FORMAT TargetFormat)
{
	D3DDEVTYPE DevType = ConvertDeviceType(DeviceType);

	return m_pSrcD3D9 -> CheckDeviceFormatConversion(Adapter, DevType, SourceFormat.d3dfFormat, TargetFormat.d3dfFormat);
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::EnumAdapterModes8To9(UINT Adapter,UINT nMode,DISPLAYMODE* pMode)
{
	D3DDISPLAYMODE Mode;

	HRESULT hr = m_pSrcD3D9->EnumAdapterModes( Adapter,GetAdapter(Adapter)->DesktopMode.Format.d3dfFormat, 
											nMode,pMode==0L?0L:&Mode );
	
	if( SUCCEEDED(hr) && pMode != NULL )
	{
		pMode->dwWidth  = Mode.Width;
		pMode->dwHeight = Mode.Height;
		pMode->dwRefreshRate = Mode.RefreshRate;
		pMode->Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format,&pMode->Format.ddpfPixelFormat);
		pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::EnumAdapterModes9(UINT Adapter, FORMAT Format, UINT nMode,DISPLAYMODE* pMode)
{
	D3DDISPLAYMODE Mode;

	HRESULT hr = m_pSrcD3D9->EnumAdapterModes( Adapter, Format.d3dfFormat, nMode,pMode==0L?0L:&Mode );
	
	if( SUCCEEDED(hr) && pMode != NULL )
	{
		pMode->dwWidth  = Mode.Width;
		pMode->dwHeight = Mode.Height;
		pMode->dwRefreshRate = Mode.RefreshRate;
		pMode->Format.d3dfFormat = Mode.Format;
		d3df2ddpf(Mode.Format,&pMode->Format.ddpfPixelFormat);
		pMode->dwBPP = pMode->Format.ddpfPixelFormat.dwRGBBitCount;
	}

	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CD3D::EnumAdapterModesEx9( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter,UINT Mode, D3DDISPLAYMODEEX* pMode )
{
	return m_pSrcD3D9Ex->EnumAdapterModesEx( Adapter, pFilter, Mode, pMode );
}

// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------

void CD3D::SetDebugMute( bool bMute )
{
	if( m_pDebugMute )
		m_pDebugMute( bMute );
}


///////////////////////////////////////////////////////
// Function:  GetD3DXBuildNumber
//
// Purpose:
// Determines the core and D3DX SDK versions.
//
// Notes:
// The GetD3DXBuildNumber function is separate from the
// CD3D9 class so that it can be called by
// DetectDXVersion.
///////////////////////////////////////////////////////
HRESULT GetD3DXBuildNumber
(
	float fDXVersion,
	float & fD3DXVersion
)
{
	HRESULT hr = S_OK;
	TCHAR* sShader = _T("float4  Tex() : COLOR { return float4(0,0,0,0); }"), *sTemp = NULL;
	LPD3DXBUFFER pShader = NULL;
	LPD3DXCONSTANTTABLE pConstantTable = NULL;
	D3DXCONSTANTTABLE_DESC desc;

	//
	// Validate parameters.
	//
	if(fDXVersion < 9.f)
	{
		return E_UNSUPPORTED;
	};


	// Need to initialize D3DX dll before we attempt to detect the version
	if(FAILED(hr = InitD3DX(fDXVersion)))
		goto Exit;
	
	// This will set the g_bStaticD3DX flag if dynamic
	D3DXCheckVersion(D3D_SDK_VERSION,D3DX_SDK_VERSION);
	
	// Fail out if we're loading dynamically
	if(!g_bStaticD3DX)
	{
		hr = E_UNSUPPORTED;
		goto Exit;
	}
	
	// We are statically linked, so we will get the build number through the ConstantTable interface
	if(FAILED(hr = D3DXCompileShader(sShader,_tcslen(sShader),NULL,NULL,_T("Tex"),_T("ps_1_1"),0,&pShader,NULL,&pConstantTable)))
		goto Exit;

	if (FAILED(hr = pConstantTable->GetDesc(&desc)))
		goto Exit;

	if (desc.Creator == NULL)
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Detect version for "new" build number scheme
	if(NULL == (sTemp = _tcschr(desc.Creator, _T('.'))) ||
	   NULL == (sTemp = _tcschr(sTemp+1, _T('.'))))
	{
		hr = E_UNSUPPORTED;
		goto Exit;
	}
	sTemp++;

	fD3DXVersion = (float)_tstof(sTemp);

	// Detect version for older SDK build numbers
	if(fD3DXVersion < 1.f)
	{
		sTemp = _tcsninc(desc.Creator, _tcslen(desc.Creator) - 4);
		fD3DXVersion = (float)_tstof(sTemp);	
	}

Exit:
	RELEASE(pShader);
	RELEASE(pConstantTable);
	return hr;
}


// ConvertDeviceType - Helper function to convert our framework DWORD DeviceType into a D3DDEVTYPE DeviceType
D3DDEVTYPE ConvertDeviceType(DWORD DeviceType)
{
	if (DeviceType == DEVICETYPE_HAL)
		return D3DDEVTYPE_HAL;
	else if((DeviceType == DEVICETYPE_REF)||(DeviceType == DEVICETYPE_NULLREF))
		return D3DDEVTYPE_REF;
	else if(DeviceType == DEVICETYPE_SW)
		return D3DDEVTYPE_SW;
	else
		return (D3DDEVTYPE) 0; // Invalid device type
}matrix g_WorldViewProj;
Texture2D  g_Texture;
SamplerState g_Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

void Transform(
   in float4 inPos   : POSITION,
   in float2 inTex   : TEXCOORD0,
   out float4 outPos : SV_POSITION,
   out float2 outTex : TEXCOORD0
) {
   //Position transform
   outPos = mul(inPos,g_WorldViewProj);
   
   //Pass tex coord through
   outTex = inTex;
}

void Color(
   in float4 pos     : SV_POSITION,
   in float2 tex     : TEXCOORD0,
   out float4 color  : SV_TARGET
) {
   color = g_Texture.Sample(g_Sampler,tex);
}

technique10 TransformColor
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0_level_9_1, Transform() ) );
        SetPixelShader( CompileShader( ps_4_0_level_9_1, Color() ) );
    }
}/*++

Copyright (c) 1999, 2000  Microsoft Corporation

Module Name:

    dbg.h

Abstract:

    debug macros

Environment:

    Kernel & user mode

Revision History:

    6-20-99 : created

--*/

#ifndef   __DBG_H__
#define   __DBG_H__

//
// Structure signatures
//

#define EHCI_TAG          'ehci'        //"EHCI"

#if DBG

#define DEBUG_LOG

// Triggers a break in the debugger in the registry key
// debugbreakOn is set.  These breakpoins are useful for
// debugging hardware/client software problems
//

#define DEBUG_BREAK(dd)  RegistrationPacket.USBPORTSVC_TestDebugBreak;

//
// This Breakpoint means we either need to test the code path
// somehow or the code is not implemented.  ie either case we
// should not have any of these when the driver is finished
//

#define TEST_TRAP(dd)     RegistrationPacket.USBPORTSVC_TestDebugBreak((dd))



#define ASSERT_TRANSFER(dd, t) EHCI_ASSERT((dd), (t)->Sig == SIG_EHCI_TRANSFER)

#define ASSERT_TD(dd, t) EHCI_ASSERT((dd), (t)->Sig == SIG_HCD_TD)
#define ASSERT_SITD(dd, t) EHCI_ASSERT((dd), (t)->Sig == SIG_HCD_SITD)
#define ASSERT_ITD(dd, t) EHCI_ASSERT((dd), (t)->Sig == SIG_HCD_ITD)

#define ASSERT_DUMMY_TD(dd, t) \
    EHCI_ASSERT((dd), (t)->NextHcdTD.Pointer == NULL);\
    EHCI_ASSERT((dd), (t)->AltNextHcdTD.Pointer == NULL);

ULONG
_cdecl
EHCI_KdPrintX(
    __in PVOID DeviceData,
    ULONG Level,
    __in PSTR Format,
    ...
    );

#define   EHCI_KdPrint(_x_) EHCI_KdPrintX _x_

#define EHCI_ASSERT(dd, exp ) {\
    NT_ASSERT(exp);\
    __pragma(warning(disable: 4127)) if (!(exp)) {__pragma(warning(default: 4127))\
        RegistrationPacket.USBPORTSVC_AssertFailure( (dd), #exp, __FILE__, __LINE__, NULL );\
    }\
}

#define EHCI_ASSERTMSG(dd, msg, exp ) {\
    NT_ASSERTMSG(msg, exp);\
    __pragma(warning(disable: 4127)) if (!(exp)) {__pragma(warning(default: 4127))\
        RegistrationPacket.USBPORTSVC_AssertFailure( (dd), #exp, __FILE__, __LINE__, NULL );\
    }\
}

#define EHCI_QHCHK(dd, ed)  EHCI_AssertQhChk(dd, ed);

#else
// keep debug logging on in retail
// so we can debug stress failires and such
#define DEBUG_LOG

// debug macros for retail build

#define TEST_TRAP(dd)
#define DEBUG_BREAK(dd)

#define ASSERT_TRANSFER(dd, t)

#define ASSERT_DUMMY_TD(dd, t)
#define ASSERT_TD(dd, t)
#define ASSERT_SITD(dd, t)
#define ASSERT_ITD(dd, t)


#define EHCI_ASSERT(dd, exp )
#define EHCI_ASSERTMSG(dd, msg, exp )

#define   EHCI_KdPrint(_x_)

#define EHCI_QHCHK(dd, ed)

#endif /* DBG */

// retail and debug

#ifdef DEBUG_LOG

#define SWIZZLE(sig) \
    ((((sig) >> 24) & 0x000000FF) | \
     (((sig) >>  8) & 0x0000FF00) | \
     (((sig) <<  8) & 0x00FF0000) | \
     (((sig) << 24) & 0xFF000000))

#define LOGENTRY(dd, mask, sig, info1, info2, info3)  \
    RegistrationPacket.USBPORTSVC_LogEntry( (dd), (mask), SWIZZLE(sig), \
        (ULONG_PTR)(info1), (ULONG_PTR)(info2), (ULONG_PTR)(info3) )

#else

#define LOGENTRY(dd, mask, sig, info1, info2, info3)

#endif


#endif /* __DBG_H__ */

#pragma once
#include "WGFInterfaces.h"
#include <tref.h>

const unsigned int PrimeNumbers[] = 
{3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167
,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349
,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547
,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733};

#define CallTreeMaxDepth 48
struct StructCallTree
{
	unsigned int CallTreeList[CallTreeMaxDepth];
	StructCallTree( ) { }
	StructCallTree(const unsigned int * pCallTreeList)
	{
		unsigned int i = 0;
		while (i < CallTreeMaxDepth  &&  pCallTreeList[i] != 0)
		{
			CallTreeList[i] = pCallTreeList[i];
			i++;
		}
		while (i < CallTreeMaxDepth)
		{
			CallTreeList[i] = 0;
			i++;
		}
	}
	bool operator==(const StructCallTree &other) const ;
};

tstring ToString( StructCallTree const& );
bool FromString( StructCallTree*, tstring const& );

class CFcallLimits : public CInterfacesTest 
{
	public:
		virtual void InitTestParameters();
		virtual TEST_RESULT Setup();
		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();
		virtual void CleanupTestCase();
		virtual void Cleanup();
		CFcallLimits(){}
		virtual ~CFcallLimits(){}
	
	protected:
		const char* GetEntryPoint();
		TEST_RESULT CompileTestShader( const std::string& shaderSource );
		TEST_RESULT CompilePassthroughShaders();
		TEST_RESULT CreateOutputBuffers();
		TEST_RESULT ReadOutputBuffers(unsigned int result[16]);
		void BindResources( ID3D11ClassInstance** pInstances );
		const char* GetAssemblyHeader();

		unsigned int m_ExpectedResults[16];
		unsigned int m_bodies;
		unsigned int m_depth;
		unsigned int m_whichClass[CallTreeMaxDepth];

		TCOMPtr<ID3D11Buffer*> m_rConstBuffer;

		TCOMPtr<ID3D11VertexShader*>    m_rPassthroughVS1; // outputs POSITION
		TCOMPtr<ID3D11VertexShader*>    m_rPassthroughVS2; // outputs SV_POSITION
		TCOMPtr<ID3D11HullShader*>      m_rPassthroughHS;
		TCOMPtr<ID3D11DomainShader*>    m_rPassthroughDS;
		TCOMPtr<ID3D11GeometryShader*>  m_rPassthroughGS;

		TCOMPtr<ID3D10Blob*> m_rPassthroughVSByteCode;

		TCOMPtr<ID3D11VertexShader*>    m_rVertexShader;
		TCOMPtr<ID3D11HullShader*>      m_rHullShader;
		TCOMPtr<ID3D11DomainShader*>    m_rDomainShader;
		TCOMPtr<ID3D11GeometryShader*>  m_rGeometryShader;
		TCOMPtr<ID3D11PixelShader*>     m_rPixelShader;
		TCOMPtr<ID3D11ComputeShader*>   m_rComputeShader;

		TCOMPtr<ID3D11Buffer*>              m_rOutputBuffer;
		TCOMPtr<ID3D11Buffer*>              m_rOutputBufferCopy;
		TCOMPtr<ID3D11UnorderedAccessView*> m_rUAV;

		StructCallTree m_CallTree;

		D3D_SHADER_STAGE m_shaderStage;

		unsigned int m_Permutations;

		struct Vertex
		{
			float m_x;
			float m_y;
			float m_z;
		};
		static const Vertex s_pVertexData[4];

		unsigned int m_CalculatedResult[16];

		TCOMPtr<ID3D11RasterizerState*> m_rRastState;
		TCOMPtr<ID3D11DepthStencilState*> m_rDepthStencilState;
		TCOMPtr<ID3D11InputLayout*> m_rInputLayout;
		TCOMPtr<ID3D11Buffer*> m_rVertexBuffer;
};

!include  $(TSTPROJECT_ROOT)\common\etwConsumers\etwConsumers.mk

TARGETNAME         = NodeMetaData.Tests
TARGETTYPE         = DYNLINK
TARGET_DESTINATION = graphics\wddm\bin
DLLDEF             =


USE_MSVCRT  = 1
TEST_CODE   = 1
NT_SIGNCODE = 1

C_DEFINES = $(C_DEFINES) -DINLINE_TEST_METHOD_MARKUP /DUNICODE  /D_UNICODE

INCLUDES = $(INCLUDES); \
    $(SDKTOOLS_INC_PATH)\WexTest\Cue; \
    $(SDKTOOLS_INC_PATH)\WexTest\Tools; \
    $(PUBLIC_ROOT)\internal\windows\inc; \
    $(PUBLIC_ROOT)\internal\windows\inc\d3dpri; \
    $(BASE_VID_TST_ROOT)\common\monwmi; \
    $(WINDOWS_TST_ROOT)\common\etwConsumers\inc; \
    $(WINDOWS_TST_ROOT)\common\etwConsumers\etwlib\inc; \
    $(WINDOWS_TST_ROOT)\common\etw\inc; \
    $(WINDOWS_TST_OBJ_ROOT)\Common\etw\manifest\$O; \
    $(WINDOWS_TST_ROOT)\common\etwPostProcessing\XperfHelper\inc; \
    $(WINDOWS_TST_ROOT)\common\etwPostProcessing\DxEtwHelper\inc; \
    $(BASE_VID_TST_ROOT)\Tests\SchTaef\RenderLib\inc; \
    $(BASE_VID_TST_ROOT)\Tests\SchTaef\inc; \

#P