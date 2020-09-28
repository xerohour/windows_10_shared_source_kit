//=======================================================================================
//
//  CDevice.cpp
//
//  Copyright (C) 1999 Microsoft Corporation
//
// ======================================================================================


#include "DevicePCH.h"

#include "CDevice.h"
#include "CSurface.h"

// --------------------------------------------------------------------------------------

CDevice::CDevice(CD3D *pD3D, UINT uAdapter, DWORD dwDevType, DWORD dwBehaviors, HWND hFocus)
{
	m_nDDILevel = 0;
	m_dwDevType = 0;
	m_fInterface = 0.f;
	m_dwBehaviors = 0;
	m_pD3D = NULL;
	m_pDevice9 = NULL;
	m_pDevice8 = NULL;
	m_pDevice7 = NULL;
	m_pDevice3 = NULL;
	m_pViewport3 = NULL;
	m_pMaterial3 = NULL;
	m_pD3DXFont = NULL;
	m_bHWVP = false;
	m_bPure = false;
	m_bFullscreen = false;
	m_nTextureFormat = 0;
	m_pTextureFormat = NULL;
	m_nCubeFormat = 0;
	m_pCubeFormat = NULL;
	m_nVolumeFormat = 0;
	m_pVolumeFormat = NULL;
	m_nZBufferFormat = 0;
	m_pZBufferFormat = NULL;
	m_nMultisampType = 0;
	m_pMultisampType = NULL;
	m_nBackBufferFormat = 0;
	m_pBackBufferFormat = NULL;
	m_pNext = NULL;
	m_pBackBuffers = NULL;
	m_nBackBuffers = 0;
	m_pPrimary = NULL;
	m_pZBuffer = NULL;
	m_dwWrapUV = 0;
	m_dwTexLoad = TEXLOAD_COPYRECTS;
	m_pLights3 = NULL;
	m_dwLights3 = 0;
	m_pPresParams = NULL;
	m_dwPSGPRegChange = PSGPREGCHANGE_NONE;
	m_dwVPEmuRegChange = VPEMUREGCHANGE_NONE;
	m_dwPPEmuRegChange = PPEMUREGCHANGE_NONE;

	pInitialize = NULL;
	pTextureFormatSupported = NULL;
	pZBufferFormatSupported = NULL;
	pCubeFormatSupported = NULL;
	pVolumeFormatSupported = NULL;
	pReleaseD3D = NULL;
	pHandleDeviceLost = NULL;
	pIsDeviceLost = NULL;
	pIsDeviceReadyForReset = NULL;

	m_bVPEmu = false;
	m_bPPEmu = false;

	memset(&m_Desc, 0, sizeof(DEVICEDESC));
	memset(&m_Guid, 0, sizeof(GUID));

	if(pD3D)
	{
		m_pD3D = pD3D;
		m_pD3D->AddRef();
		m_uAdapter = uAdapter;
		m_dwDevType = dwDevType;
		m_dwBehaviors = dwBehaviors;
		m_hFocusWindow = hFocus;
		m_fInterface = pD3D->GetVersion();

		// Create the default palettes
		if(m_fInterface < 8.f)
		{
			ZeroMemory(m_Palette4, sizeof(PALETTEENTRY) * 16);
			GenerateColorCube(m_Palette4, 4, m_fInterface);
		}
	
		ZeroMemory(m_Palette8, sizeof(PALETTEENTRY) * 256);
		GenerateColorCube(m_Palette8, 8, m_fInterface);
	}

	InitFunctionPtrs(m_fInterface);

	bSuppressGetDepthStencilDBGOutput = false;
	m_pCaps9Cached = NULL;
}

// --------------------------------------------------------------------------------------

CDevice::~CDevice()
{
	ReleaseAll();

	if ( m_pCaps9Cached )
		delete m_pCaps9Cached;
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CDevice::AddRef(void)
{
	if(m_pD3D)
		m_pD3D->AddRef();
	return CObject::AddRef();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CDevice::Release(void)
{	
	if(m_pD3D)
		m_pD3D->Release();
	return CObject::Release();
}

// --------------------------------------------------------------------------------------

ADAPTER* CDevice::GetAdapter()
{
	if(m_pD3D == NULL)
		return NULL;
	else
		return m_pD3D->GetAdapter(m_uAdapter);
}

// --------------------------------------------------------------------------------------

void CDevice::InitFunctionPtrs(float fVersion)
{
	DPF(4, _T("CDevice::InitFunctionPtrs()\n"));

	// Default 9.L extensions methods to be unsupported.
	pSetConvolutionMonoKernel = &CDevice::SetConvolutionMonoKernelUnsup;
	pComposeRects = &CDevice::ComposeRectsUnsup;
	pPresentEx = &CDevice::PresentExUnsup;
	pWaitForVBlank = &CDevice::WaitForVBlankUnsup;
	pCheckResourceResidency = &CDevice::CheckResourceResidencyUnsup;
	pSetMaximumFrameLatency = &CDevice::SetMaximumFrameLatencyUnsup;
	pGetMaximumFrameLatency = &CDevice::GetMaximumFrameLatencyUnsup;
	pCreateOffscreenPlainSurfaceEx = &CDevice::CreateOffscreenPlainSurfaceExUnsup;
	pCreateRenderTargetEx = &CDevice::CreateRenderTargetExUnsup;
	pCreateDepthStencilSurfaceEx = &CDevice::CreateDepthStencilSurfaceExUnsup;
	pCheckDeviceState = &CDevice::CheckDeviceStateUnsup;
	pResetEx = &CDevice::ResetExUnsup;

	// Resolve other function pointers.

	if (fVersion >= 9.1f && fVersion < 10.f)
	{
		pGetDeviceCaps = &CDevice::GetDeviceCaps9;
		pInitialize = &CDevice::Initialize9;
		pWrapDevice = &CDevice::WrapDevice9;
		pReleaseWrappers = &CDevice::ReleaseWrappers9;
		pIsDeviceEqual = &CDevice::IsDeviceEqual9;
		pTestCooperativeLevel = &CDevice::TestCooperativeLevel9;
		pGetAvailableTextureMem = &CDevice::GetAvailableTextureMem9;
		pResourceManagerDiscardBytes = &CDevice::ResourceManagerDiscardBytesUnsup;
		pEvictManagedResources = &CDevice::EvictManagedResources9;
		pGetDisplayMode = &CDevice::GetDisplayMode8To9;
		pGetDisplayMode2 = &CDevice::GetDisplayMode9;
		pGetCreationParameters = &CDevice::GetCreationParameters9;
		pSetCursorProperties = &CDevice::SetCursorProperties9;
		pSetCursorPosition = &CDevice::SetCursorPosition9;
		pShowCursor = &CDevice::ShowCursor9;
		pReset = &CDevice::Reset9;
		pGetBackBuffer = &CDevice::GetBackBuffer8To9;
		pGetBackBuffer2 = &CDevice::GetBackBuffer9;
		pTextureFormatSupported = &CDevice::TextureFormatSupported9;
		pZBufferFormatSupported = &CDevice::ZBufferFormatSupported9;
		pCubeFormatSupported = &CDevice::CubeFormatSupported9;
		pVolumeFormatSupported = &CDevice::VolumeFormatSupported9;
		pSetRenderTarget = &CDevice::SetRenderTarget8To9;
		pGetRenderTarget = &CDevice::GetRenderTarget8To9;
		pSetRenderTarget2 = &CDevice::SetRenderTarget9;
		pGetRenderTarget2 = &CDevice::GetRenderTarget9;
		pGetDepthStencilSurface = &CDevice::GetDepthStencilSurface9;
		pSetDepthStencilSurface = &CDevice::SetDepthStencilSurface9;
		pCreateRenderTarget = &CDevice::CreateRenderTarget8To9;
		pCreateRenderTarget2 = &CDevice::CreateRenderTarget9;
		pCreateDepthStencilSurface = &CDevice::CreateDepthStencilSurface8To9;
		pCreateDepthStencilSurface2 = &CDevice::CreateDepthStencilSurface9;
		pCreateImageSurface = &CDevice::CreateImageSurface9;
		pGetFrontBuffer = &CDevice::GetFrontBuffer8To9;
		pGetFrontBufferData = &CDevice::GetFrontBufferData9;
		pCopyRects = &CDevice::CopyRectsUnsup;
		pUpdateSurface = &CDevice::UpdateSurface9;
		pUpdateTexture = &CDevice::UpdateTexture9;
		pBeginScene = &CDevice::BeginScene9;
		pEndScene = &CDevice::EndScene9;
		pClear = &CDevice::Clear9;
		pSetTransform = &CDevice::SetTransform9;
		pGetTransform = &CDevice::GetTransform9;
		pMultiplyTransform = &CDevice::MultiplyTransform9;
		pSetViewport = &CDevice::SetViewport9;
		pGetViewport = &CDevice::GetViewport9;
		pSetMaterial = &CDevice::SetMaterial9;
		pGetMaterial = &CDevice::GetMaterial9;
		pSetLight = &CDevice::SetLight9;
		pGetLight = &CDevice::GetLight9;
		pLightEnable = &CDevice::LightEnable9;
		pGetLightEnable = &CDevice::GetLightEnable9;
		pSetClipPlane = &CDevice::SetClipPlane9;
		pGetClipPlane = &CDevice::GetClipPlane9;
		pSetLightState = &CDevice::SetLightStateUnsup;
		pGetLightState = &CDevice::GetLightStateUnsup;
		pSetRenderState = &CDevice::SetRenderState9;
		pGetRenderState = &CDevice::GetRenderState9;
		pBeginStateBlock = &CDevice::BeginStateBlock9;
		pEndStateBlock = &CDevice::EndStateBlock9;
		pApplyStateBlock = &CDevice::ApplyStateBlock9;
		pCaptureStateBlock = &CDevice::CaptureStateBlock9;
		pDeleteStateBlock = &CDevice::DeleteStateBlock9;
		pCreateStateBlock = &CDevice::CreateStateBlock9;
		pSetClipStatus = &CDevice::SetClipStatus9;
		pGetClipStatus = &CDevice::GetClipStatus9;
		pSetTexture = &CDevice::SetTexture9;
		pGetTexture = &CDevice::GetTexture9;
		pGetTextureStageState = &CDevice::GetTextureStageState9;
		pSetTextureStageState = &CDevice::SetTextureStageState9;
		pValidateDevice = &CDevice::ValidateDevice9;
		pGetInfo = &CDevice::GetInfo9;
		pGetVerticalBlankStatus = &CDevice::GetVerticalBlankStatusUnsup;
		pProcessVertices = &CDevice::ProcessVertices9;
		pCreateVertexShader = &CDevice::CreateVertexShader9;
		pCreateVertexShaderInt = &CDevice::CreateVertexShaderInt9;
		pDeleteVertexShader = &CDevice::DeleteVertexShader9;
		pSetVertexShader = &CDevice::SetFVF9;
		pSetVertexShaderInt = &CDevice::SetVertexShaderInt9;
		pSetFVF = &CDevice::SetFVF9;
		pGetVertexShader = &CDevice::GetVertexShaderUnsup;
		pGetVertexShaderInt = &CDevice::GetVertexShaderInt9;
		pSetVertexShaderConstant = &CDevice::SetVertexShaderConstantF9;
		pGetVertexShaderConstant = &CDevice::GetVertexShaderConstantF9;
		pSetVertexShaderConstantI = &CDevice::SetVertexShaderConstantI9;
		pGetVertexShaderConstantI = &CDevice::GetVertexShaderConstantI9;
		pSetVertexShaderConstantB = &CDevice::SetVertexShaderConstantB9;
		pGetVertexShaderConstantB = &CDevice::GetVertexShaderConstantB9;
		pSetStreamSource = &CDevice::SetStreamSource9;
		pGetStreamSource = &CDevice::GetStreamSource9;
		pSetIndices = &CDevice::SetIndices9;
		pGetIndices = &CDevice::GetIndices9;
		pCreatePixelShaderInt = &CDevice::CreatePixelShader9;
		pDeletePixelShaderInt = &CDevice::DeletePixelShader9;
		pSetPixelShaderInt = &CDevice::SetPixelShader9;
		pGetPixelShaderInt = &CDevice::GetPixelShader9;
		pCreatePixelShader = &CDevice::CreatePixelShaderUnsup;
		pDeletePixelShader = &CDevice::DeletePixelShaderUnsup;
		pSetPixelShader = &CDevice::SetPixelShaderUnsup;
		pGetPixelShader = &CDevice::GetPixelShaderUnsup;
		pSetPixelShaderConstant = &CDevice::SetPixelShaderConstantF9;
		pGetPixelShaderConstant = &CDevice::GetPixelShaderConstantF9;
		pSetPixelShaderConstantI = &CDevice::SetPixelShaderConstantI9;
		pGetPixelShaderConstantI = &CDevice::GetPixelShaderConstantI9;
		pSetPixelShaderConstantB = &CDevice::SetPixelShaderConstantB9;
		pGetPixelShaderConstantB = &CDevice::GetPixelShaderConstantB9;
		pDrawRectPatch = &CDevice::DrawRectPatch9;
		pDrawTriPatch = &CDevice::DrawTriPatch9;
		pDeletePatch = &CDevice::DeletePatch9;
		pSetNPatchMode = &CDevice::SetNPatchMode9;
		pGetNPatchMode = &CDevice::GetNPatchMode9;
		pDrawPrimitiveUserMemory = &CDevice::DrawPrimitiveUserMemory9;
		pDrawPrimitiveVertexBuffer = &CDevice::DrawPrimitiveVertexBuffer9;
		pDrawPrimitiveStridedData = &CDevice::DrawPrimitiveStridedData9;
		pDrawPrimitive = &CDevice::DrawPrimitive9;
		pDrawIndexedPrimitive = &CDevice::DrawIndexedPrimitive9;
		pDrawPrimitiveUP = &CDevice::DrawPrimitiveUP9;
		pDrawIndexedPrimitiveUP = &CDevice::DrawIndexedPrimitiveUP9;
		//Do we still need this? pBeginEnd
		pCreateVertexBuffer = &CDevice::CreateVertexBuffer8To9;
		pCreateIndexBuffer = &CDevice::CreateIndexBuffer8To9;
		pCreateVertexBuffer2 = &CDevice::CreateVertexBuffer9;
		pCreateIndexBuffer2 = &CDevice::CreateIndexBuffer9;
		pPresent = &CDevice::Present9;
		pReleaseD3D = &CDevice::ReleaseD3D9;
		pDraw2DText = &CDevice::Draw2DText9;
		pGetRasterStatus = &CDevice::GetRasterStatus8To9;
		pSetGammaRamp = &CDevice::SetGammaRamp8To9;
		pGetGammaRamp = &CDevice::GetGammaRamp8To9;
		pGetRasterStatus2 = &CDevice::GetRasterStatus9;
		pSetGammaRamp2 = &CDevice::SetGammaRamp9;
		pGetGammaRamp2 = &CDevice::GetGammaRamp9;
		pSetPaletteEntries = &CDevice::SetPaletteEntries9;
		pGetPaletteEntries = &CDevice::GetPaletteEntries9;
		pSetCurrentTexturePalette = &CDevice::SetCurrentTexturePalette9;
		pGetCurrentTexturePalette = &CDevice::GetCurrentTexturePalette9;
		pCreateTexture = &CDevice::CreateTexture8To9;
		pCreateVolumeTexture = &CDevice::CreateVolumeTexture8To9;
		pCreateCubeTexture = &CDevice::CreateCubeTexture8To9;
		pCreateTexture2 = &CDevice::CreateTexture9;
		pCreateVolumeTexture2 = &CDevice::CreateVolumeTexture9;
		pCreateCubeTexture2 = &CDevice::CreateCubeTexture9;
		pCreateAdditionalSwapChain = &CDevice::CreateAdditionalSwapChain9;
		pValidateMode = &CDevice::ValidateMode9;
		pHandleDeviceLost = &CDevice::HandleDeviceLost9;
		pResize = &CDevice::Resize9;
		pIsDeviceLost = &CDevice::IsDeviceLost9L;
		pIsDeviceReadyForReset = &CDevice::IsDeviceReadyForReset9L;
		pD3DXFontOnLostDevice = &CDevice::D3DXFontOnLostDevice9;
		pD3DXFontOnResetDevice = &CDevice::D3DXFontOnResetDevice9;
		pCreateVertexDeclaration = &CDevice::CreateVertexDeclaration9;
		pSetVertexDeclaration = &CDevice::SetVertexDeclaration9;
		pGetVertexDeclaration = &CDevice::GetVertexDeclaration9;
		pSetScissorRect = &CDevice::SetScissorRect9;
		pGetScissorRect = &CDevice::GetScissorRect9;
		pSetDialogBoxMode = &CDevice::SetDialogBoxMode9;
		pCreateOffscreenPlainSurface = &CDevice::CreateOffscreenPlainSurface9;
		pSetSoftwareVertexProcessing = &CDevice::SetSoftwareVertexProcessing9;
		pGetSoftwareVertexProcessing = &CDevice::GetSoftwareVertexProcessing9;
		pGetSwapChain = &CDevice::GetSwapChain9;
		pGetNumberOfSwapChains = &CDevice::GetNumberOfSwapChains9;
		pCreateQuery = &CDevice::CreateQuery9;
		pStretchRect = &CDevice::StretchRect9;
		pColorFill = &CDevice::ColorFill9;
		pGetFVF = &CDevice::GetFVF9;
		pSetStreamSourceFreq = &CDevice::SetStreamSourceFreq9;
		pGetStreamSourceFreq = &CDevice::GetStreamSourceFreq9;
		pGetRenderTargetData = &CDevice::GetRenderTargetData9;
		pSetSamplerState = &CDevice::SetSamplerState9;
		pGetSamplerState = &CDevice::GetSamplerState9;

		// D3D 9.L methods
		pSetConvolutionMonoKernel = &CDevice::SetConvolutionMonoKernel9;
		pComposeRects = &CDevice::ComposeRects9;
		pPresentEx = &CDevice::PresentEx9;
		pGetGPUThreadPriority = &CDevice::GetGPUThreadPriority9;
		pSetGPUThreadPriority = &CDevice::SetGPUThreadPriority9;
		pWaitForVBlank = &CDevice::WaitForVBlank9;
		pCheckResourceResidency = &CDevice::CheckResourceResidency9;
		pSetMaximumFrameLatency = &CDevice::SetMaximumFrameLatency9;
		pGetMaximumFrameLatency = &CDevice::GetMaximumFrameLatency9;
		pCheckDeviceState = &CDevice::CheckDeviceState9;
		pCreateRenderTargetEx = &CDevice::CreateRenderTargetEx9;
		pCreateOffscreenPlainSurfaceEx = &CDevice::CreateOffscreenPlainSurfaceEx9;
		pCreateDepthStencilSurfaceEx = &CDevice::CreateDepthStencilSurfaceEx9;
		pResetEx = &CDevice::ResetEx9;
		//pGetDisplayModeEx = &CDevice::GetDisplayModeEx9;

		// D3DX9 methods
		pD3DXInitializePSGP = &CDevice::D3DXInitializePSGP9;
		pD3DXCreateEffectFromFile = &CDevice::D3DXCreateEffectFromFile9;
		pCreateEffectFromFile = &CDevice::CreateEffectFromFile9;
		pD3DXCreateEffectFromResource = &CDevice::D3DXCreateEffectFromResource9;
		pCreateEffectFromResource = &CDevice::CreateEffectFromResource9;
		pD3DXCreateMesh = &CDevice::D3DXCreateMesh9;
		pD3DXCreateMeshFVF = &CDevice::D3DXCreateMeshFVF9;
		pD3DXCreateTexture = &CDevice::D3DXCreateTexture9;
		pD3DXCreateTextureFromFileA = &CDevice::D3DXCreateTextureFromFileA9;
		pD3DXCreateTextureFromFileExA = &CDevice::D3DXCreateTextureFromFileExA9;
		pD3DXCreateTextureFromFileInMemory = &CDevice::D3DXCreateTextureFromFileInMemory9;
		pD3DXCreateTextureFromFileInMemoryEx = &CDevice::D3DXCreateTextureFromFileInMemoryEx9;
		pD3DXCreateTextureFromResourceA = &CDevice::D3DXCreateTextureFromResourceA9;
		pD3DXCreateTextureFromResourceExA = &CDevice::D3DXCreateTextureFromResourceExA9;
		pD3DXFilterTexture = &CDevice::D3DXFilterTexture9;
		pD3DXLoadVolumeFromMemory = &CDevice::D3DXLoadVolumeFromMemory9;
		pD3DXLoadVolumeFromVolume = &CDevice::D3DXLoadVolumeFromVolume9;
		pD3DXEffect_CloneEffect = &CDevice::D3DXEffect_CloneEffect9;
		pD3DXLoadSurfaceFromSurface = &CDevice::D3DXLoadSurfaceFromSurface9;
		pD3DXLoadSurfaceFromFileInMemory = &CDevice::D3DXLoadSurfaceFromFileInMemory9;
		pD3DXLoadSurfaceFromMemory = &CDevice::D3DXLoadSurfaceFromMemory9;
		pD3DXLoadSurfaceFromFile = &CDevice::D3DXLoadSurfaceFromFile9;
		pD3DXSaveTextureToFileA = &CDevice::D3DXSaveTextureToFileA9;
		pD3DXSaveSurfaceToFileA = &CDevice::D3DXSaveSurfaceToFileA9;
		pD3DXFillTexture = &CDevice::D3DXFillTexture9;
		pD3DXTessellateRectPatch = &CDevice::D3DXTessellateRectPatch9;
		pD3DXTessellateTriPatch = &CDevice::D3DXTessellateTriPatch9;
	}
	else if(fVersion >= 9.f && fVersion < 9.1f)
	{
		pGetDeviceCaps = &CDevice::GetDeviceCaps9;
		pInitialize = &CDevice::Initialize9;
		pWrapDevice = &CDevice::WrapDevice9;
		pReleaseWrappers = &CDevice::ReleaseWrappers9;
		pIsDeviceEqual = &CDevice::IsDeviceEqual9;
		pTestCooperativeLevel = &CDevice::TestCooperativeLevel9;
		pGetAvailableTextureMem = &CDevice::GetAvailableTextureMem9;
		pResourceManagerDiscardBytes = &CDevice::ResourceManagerDiscardBytesUnsup;
		pEvictManagedResources = &CDevice::EvictManagedResources9;
		pGetDisplayMode = &CDevice::GetDisplayMode8To9;
		pGetDisplayMode2 = &CDevice::GetDisplayMode9;
		pGetCreationParameters = &CDevice::GetCreationParameters9;
		pSetCursorProperties = &CDevice::SetCursorProperties9;
		pSetCursorPosition = &CDevice::SetCursorPosition9;
		pShowCursor = &CDevice::ShowCursor9;
		pReset = &CDevice::Reset9;
		pGetBackBuffer = &CDevice::GetBackBuffer8To9;
		pGetBackBuffer2 = &CDevice::GetBackBuffer9;
		pTextureFormatSupported = &CDevice::TextureFormatSupported9;
		pZBufferFormatSupported = &CDevice::ZBufferFormatSupported9;
		pCubeFormatSupported = &CDevice::CubeFormatSupported9;
		pVolumeFormatSupported = &CDevice::VolumeFormatSupported9;
		pSetRenderTarget = &CDevice::SetRenderTarget8To9;
		pGetRenderTarget = &CDevice::GetRenderTarget8To9;
		pSetRenderTarget2 = &CDevice::SetRenderTarget9;
		pGetRenderTarget2 = &CDevice::GetRenderTarget9;
		pGetDepthStencilSurface = &CDevice::GetDepthStencilSurface9;
		pSetDepthStencilSurface = &CDevice::SetDepthStencilSurface9;
		pCreateRenderTarget = &CDevice::CreateRenderTarget8To9;
		pCreateRenderTarget2 = &CDevice::CreateRenderTarget9;
		pCreateDepthStencilSurface = &CDevice::CreateDepthStencilSurface8To9;
		pCreateDepthStencilSurface2 = &CDevice::CreateDepthStencilSurface9;
		pCreateImageSurface = &CDevice::CreateImageSurface9;
		pGetFrontBuffer = &CDevice::GetFrontBuffer8To9;
		pGetFrontBufferData = &CDevice::GetFrontBufferData9;
		pCopyRects = &CDevice::CopyRectsUnsup;
		pUpdateSurface = &CDevice::UpdateSurface9;
		pUpdateTexture = &CDevice::UpdateTexture9;
		pBeginScene = &CDevice::BeginScene9;
		pEndScene = &CDevice::EndScene9;
		pClear = &CDevice::Clear9;
		pSetTransform = &CDevice::SetTransform9;
		pGetTransform = &CDevice::GetTransform9;
		pMultiplyTransform = &CDevice::MultiplyTransform9;
		pSetViewport = &CDevice::SetViewport9;
		pGetViewport = &CDevice::GetViewport9;
		pSetMaterial = &CDevice::SetMaterial9;
		pGetMaterial = &CDevice::GetMaterial9;
		pSetLight = &CDevice::SetLight9;
		pGetLight = &CDevice::GetLight9;
		pLightEnable = &CDevice::LightEnable9;
		pGetLightEnable = &CDevice::GetLightEnable9;
		pSetClipPlane = &CDevice::SetClipPlane9;
		pGetClipPlane = &CDevice::GetClipPlane9;
		pSetLightState = &CDevice::SetLightStateUnsup;
		pGetLightState = &CDevice::GetLightStateUnsup;
		pSetRenderState = &CDevice::SetRenderState9;
		pGetRenderState = &CDevice::GetRenderState9;
		pBeginStateBlock = &CDevice::BeginStateBlock9;
		pEndStateBlock = &CDevice::EndStateBlock9;
		pApplyStateBlock = &CDevice::ApplyStateBlock9;
		pCaptureStateBlock = &CDevice::CaptureStateBlock9;
		pDeleteStateBlock = &CDevice::DeleteStateBlock9;
		pCreateStateBlock = &CDevice::CreateStateBlock9;
		pSetClipStatus = &CDevice::SetClipStatus9;
		pGetClipStatus = &CDevice::GetClipStatus9;
		pSetTexture = &CDevice::SetTexture9;
		pGetTexture = &CDevice::GetTexture9;
		pGetTextureStageState = &CDevice::GetTextureStageState9;
		pSetTextureStageState = &CDevice::SetTextureStageState9;
		pValidateDevice = &CDevice::ValidateDevice9;
		pGetInfo = &CDevice::GetInfo9;
		pGetVerticalBlankStatus = &CDevice::GetVerticalBlankStatusUnsup;
		pProcessVertices = &CDevice::ProcessVertices9;
		pCreateVertexShader = &CDevice::CreateVertexShader9;
		pCreateVertexShaderInt = &CDevice::CreateVertexShaderInt9;
		pDeleteVertexShader = &CDevice::DeleteVertexShader9;
		pSetVertexShader = &CDevice::SetFVF9;
		pSetVertexShaderInt = &CDevice::SetVertexShaderInt9;
		pSetFVF = &CDevice::SetFVF9;
		pGetVertexShader = &CDevice::GetVertexShaderUnsup;
		pGetVertexShaderInt = &CDevice::GetVertexShaderInt9;
		pSetVertexShaderConstant = &CDevice::SetVertexShaderConstantF9;
		pGetVertexShaderConstant = &CDevice::GetVertexShaderConstantF9;
		pSetVertexShaderConstantI = &CDevice::SetVertexShaderConstantI9;
		pGetVertexShaderConstantI = &CDevice::GetVertexShaderConstantI9;
		pSetVertexShaderConstantB = &CDevice::SetVertexShaderConstantB9;
		pGetVertexShaderConstantB = &CDevice::GetVertexShaderConstantB9;
		pSetStreamSource = &CDevice::SetStreamSource9;
		pGetStreamSource = &CDevice::GetStreamSource9;
		pSetIndices = &CDevice::SetIndices9;
		pGetIndices = &CDevice::GetIndices9;
		pCreatePixelShaderInt = &CDevice::CreatePixelShader9;
		pDeletePixelShaderInt = &CDevice::DeletePixelShader9;
		pSetPixelShaderInt = &CDevice::SetPixelShader9;
		pGetPixelShaderInt = &CDevice::GetPixelShader9;
		pCreatePixelShader = &CDevice::CreatePixelShaderUnsup;
		pDeletePixelShader = &CDevice::DeletePixelShaderUnsup;
		pSetPixelShader = &CDevice::SetPixelShaderUnsup;
		pGetPixelShader = &CDevice::GetPixelShaderUnsup;
		pSetPixelShaderConstant = &CDevice::SetPixelShaderConstantF9;
		pGetPixelShaderConstant = &CDevice::GetPixelShaderConstantF9;
		pSetPixelShaderConstantI = &CDevice::SetPixelShaderConstantI9;
		pGetPixelShaderConstantI = &CDevice::GetPixelShaderConstantI9;
		pSetPixelShaderConstantB = &CDevice::SetPixelShaderConstantB9;
		pGetPixelShaderConstantB = &CDevice::GetPixelShaderConstantB9;
		pDrawRectPatch = &CDevice::DrawRectPatch9;
		pDrawTriPatch = &CDevice::DrawTriPatch9;
		pDeletePatch = &CDevice::DeletePatch9;
		pSetNPatchMode = &CDevice::SetNPatchMode9;
		pGetNPatchMode = &CDevice::GetNPatchMode9;
		pDrawPrimitiveUserMemory = &CDevice::DrawPrimitiveUserMemory9;
		pDrawPrimitiveVertexBuffer = &CDevice::DrawPrimitiveVertexBuffer9;
		pDrawPrimitiveStridedData = &CDevice::DrawPrimitiveStridedData9;
		pDrawPrimitive = &CDevice::DrawPrimitive9;
		pDrawIndexedPrimitive = &CDevice::DrawIndexedPrimitive9;
		pDrawPrimitiveUP = &CDevice::DrawPrimitiveUP9;
		pDrawIndexedPrimitiveUP = &CDevice::DrawIndexedPrimitiveUP9;
		//Do we still need this? pBeginEnd
		pCreateVertexBuffer = &CDevice::CreateVertexBuffer8To9;
		pCreateIndexBuffer = &CDevice::CreateIndexBuffer8To9;
		pCreateVertexBuffer2 = &CDevice::CreateVertexBuffer9;
		pCreateIndexBuffer2 = &CDevice::CreateIndexBuffer9;
		pPresent = &CDevice::Present9;
		pReleaseD3D = &CDevice::ReleaseD3D9;
		pDraw2DText = &CDevice::Draw2DText9;
		pGetRasterStatus = &CDevice::GetRasterStatus8To9;
		pSetGammaRamp = &CDevice::SetGammaRamp8To9;
		pGetGammaRamp = &CDevice::GetGammaRamp8To9;
		pGetRasterStatus2 = &CDevice::GetRasterStatus9;
		pSetGammaRamp2 = &CDevice::SetGammaRamp9;
		pGetGammaRamp2 = &CDevice::GetGammaRamp9;
		pSetPaletteEntries = &CDevice::SetPaletteEntries9;
		pGetPaletteEntries = &CDevice::GetPaletteEntries9;
		pSetCurrentTexturePalette = &CDevice::SetCurrentTexturePalette9;
		pGetCurrentTexturePalette = &CDevice::GetCurrentTexturePalette9;
		pCreateTexture = &CDevice::CreateTexture8To9;
		pCreateVolumeTexture = &CDevice::CreateVolumeTexture8To9;
		pCreateCubeTexture = &CDevice::CreateCubeTexture8To9;
		pCreateTexture2 = &CDevice::CreateTexture9;
		pCreateVolumeTexture2 = &CDevice::CreateVolumeTexture9;
		pCreateCubeTexture2 = &CDevice::CreateCubeTexture9;
		pCreateAdditionalSwapChain = &CDevice::CreateAdditionalSwapChain9;
		pValidateMode = &CDevice::ValidateMode9;
		pHandleDeviceLost = &CDevice::HandleDeviceLost9;
		pResize = &CDevice::Resize9;
		pIsDeviceLost = &CDevice::IsDeviceLost9;
		pIsDeviceReadyForReset = &CDevice::IsDeviceReadyForReset9;
		pD3DXFontOnLostDevice = &CDevice::D3DXFontOnLostDevice9;
		pD3DXFontOnResetDevice = &CDevice::D3DXFontOnResetDevice9;
		pCreateVertexDeclaration = &CDevice::CreateVertexDeclaration9;
		pSetVertexDeclaration = &CDevice::SetVertexDeclaration9;
		pGetVertexDeclaration = &CDevice::GetVertexDeclaration9;
		pSetScissorRect = &CDevice::SetScissorRect9;
		pGetScissorRect = &CDevice::GetScissorRect9;
		pSetDialogBoxMode = &CDevice::SetDialogBoxMode9;
		pCreateOffscreenPlainSurface = &CDevice::CreateOffscreenPlainSurface9;
		pSetSoftwareVertexProcessing = &CDevice::SetSoftwareVertexProcessing9;
		pGetSoftwareVertexProcessing = &CDevice::GetSoftwareVertexProcessing9;
		pGetSwapChain = &CDevice::GetSwapChain9;
		pGetNumberOfSwapChains = &CDevice::GetNumberOfSwapChains9;
		pCreateQuery = &CDevice::CreateQuery9;
		pStretchRect = &CDevice::StretchRect9;
		pColorFill = &CDevice::ColorFill9;
		pGetFVF = &CDevice::GetFVF9;
		pSetStreamSourceFreq = &CDevice::SetStreamSourceFreq9;
		pGetStreamSourceFreq = &CDevice::GetStreamSourceFreq9;
		pGetRenderTargetData = &CDevice::GetRenderTargetData9;
		pSetSamplerState = &CDevice::SetSamplerState9;
		pGetSamplerState = &CDevice::GetSamplerState9;

		pD3DXInitializePSGP = &CDevice::D3DXInitializePSGP9;
		pD3DXCreateEffectFromFile = &CDevice::D3DXCreateEffectFromFile9;
		pCreateEffectFromFile = &CDevice::CreateEffectFromFile9;
		pD3DXCreateEffectFromResource = &CDevice::D3DXCreateEffectFromResource9;
		pCreateEffectFromResource = &CDevice::CreateEffectFromResource9;
		pD3DXCreateMesh = &CDevice::D3DXCreateMesh9;
		pD3DXCreateMeshFVF = &CDevice::D3DXCreateMeshFVF9;
		pD3DXCreateTexture = &CDevice::D3DXCreateTexture9;
		pD3DXCreateTextureFromFileA = &CDevice::D3DXCreateTextureFromFileA9;
		pD3DXCreateTextureFromFileExA = &CDevice::D3DXCreateTextureFromFileExA9;
		pD3DXCreateTextureFromFileInMemory = &CDevice::D3DXCreateTextureFromFileInMemory9;
		pD3DXCreateTextureFromFileInMemoryEx = &CDevice::D3DXCreateTextureFromFileInMemoryEx9;
		pD3DXCreateTextureFromResourceA = &CDevice::D3DXCreateTextureFromResourceA9;
		pD3DXCreateTextureFromResourceExA = &CDevice::D3DXCreateTextureFromResourceExA9;
		pD3DXFilterTexture = &CDevice::D3DXFilterTexture9;
		pD3DXLoadVolumeFromMemory = &CDevice::D3DXLoadVolumeFromMemory9;
		pD3DXLoadVolumeFromVolume = &CDevice::D3DXLoadVolumeFromVolume9;
		pD3DXEffect_CloneEffect = &CDevice::D3DXEffect_CloneEffect9;
		pD3DXLoadSurfaceFromSurface = &CDevice::D3DXLoadSurfaceFromSurface9;
		pD3DXLoadSurfaceFromFileInMemory = &CDevice::D3DXLoadSurfaceFromFileInMemory9;
		pD3DXLoadSurfaceFromMemory = &CDevice::D3DXLoadSurfaceFromMemory9;
		pD3DXLoadSurfaceFromFile = &CDevice::D3DXLoadSurfaceFromFile9;
		pD3DXSaveTextureToFileA = &CDevice::D3DXSaveTextureToFileA9;
		pD3DXSaveSurfaceToFileA = &CDevice::D3DXSaveSurfaceToFileA9;
		pD3DXFillTexture = &CDevice::D3DXFillTexture9;
		pD3DXTessellateRectPatch = &CDevice::D3DXTessellateRectPatch9;
		pD3DXTessellateTriPatch = &CDevice::D3DXTessellateTriPatch9;
		pCheckDeviceState = &CDevice::CheckDeviceStateUnsup;
	}
	else if(fVersion >= 8.f && fVersion < 9.f) 
	{
		pGetDeviceCaps = &CDevice::GetDeviceCaps8;
		pInitialize = &CDevice::Initialize8;
		pWrapDevice = &CDevice::WrapDevice8;
		pReleaseWrappers = &CDevice::ReleaseWrappers8;
		pIsDeviceEqual = &CDevice::IsDeviceEqual8;
		pTestCooperativeLevel = &CDevice::TestCooperativeLevel8;
		pGetAvailableTextureMem = &CDevice::GetAvailableTextureMem8;
		pEvictManagedResources = &CDevice::EvictManagedResources8;
		pResourceManagerDiscardBytes = &CDevice::ResourceManagerDiscardBytes8;
		pGetDisplayMode = &CDevice::GetDisplayMode8;
		pGetDisplayMode2 = &CDevice::GetDisplayMode2Unsup;
		pGetCreationParameters = &CDevice::GetCreationParameters8;
		pSetCursorProperties = &CDevice::SetCursorProperties8;
		pSetCursorPosition = &CDevice::SetCursorPosition8;
		pShowCursor = &CDevice::ShowCursor8;
		pReset = &CDevice::Reset8;
		pGetBackBuffer = &CDevice::GetBackBuffer8;
		pGetBackBuffer2 = &CDevice::GetBackBuffer2Unsup;
		pTextureFormatSupported = &CDevice::TextureFormatSupported8;
		pZBufferFormatSupported = &CDevice::ZBufferFormatSupported8;
		pCubeFormatSupported = &CDevice::CubeFormatSupported8;
		pVolumeFormatSupported = &CDevice::VolumeFormatSupported8;
		pSetRenderTarget = &CDevice::SetRenderTarget8;
		pGetRenderTarget = &CDevice::GetRenderTarget8;
		pSetRenderTarget2 = &CDevice::SetRenderTargetUnsup;
		pGetRenderTarget2 = &CDevice::GetRenderTargetUnsup;
		pGetDepthStencilSurface = &CDevice::GetDepthStencilSurface8;
		pSetDepthStencilSurface = &CDevice::SetDepthStencilSurfaceUnsup;
		pCreateRenderTarget = &CDevice::CreateRenderTarget8;
		pCreateRenderTarget2 = &CDevice::CreateRenderTarget2Unsup;
		pCreateDepthStencilSurface = &CDevice::CreateDepthStencilSurface8;
		pCreateDepthStencilSurface2 = &CDevice::CreateDepthStencilSurface2Unsup;
		pCreateImageSurface = &CDevice::CreateImageSurface8;
		pGetFrontBuffer = &CDevice::GetFrontBuffer8;
		pGetFrontBufferData = &CDevice::GetFrontBufferDataUnsup;
		pCopyRects = &CDevice::CopyRects8;
		pUpdateSurface = &CDevice::UpdateSurface9ToCopyRects8;
		pUpdateTexture = &CDevice::UpdateTexture8;
		pBeginScene = &CDevice::BeginScene8;
		pEndScene = &CDevice::EndScene8;
		pClear = &CDevice::Clear8;
		pSetTransform = &CDevice::SetTransform8;
		pGetTransform = &CDevice::GetTransform8;
		pMultiplyTransform = &CDevice::MultiplyTransform8;
		pSetViewport = &CDevice::SetViewport8;
		pGetViewport = &CDevice::GetViewport8;
		pSetMaterial = &CDevice::SetMaterial8;
		pGetMaterial = &CDevice::GetMaterial8;
		pSetLight = &CDevice::SetLight8;
		pGetLight = &CDevice::GetLight8;
		pLightEnable = &CDevice::LightEnable8;
		pGetLightEnable = &CDevice::GetLightEnable8;
		pSetClipPlane = &CDevice::SetClipPlane8;
		pGetClipPlane = &CDevice::GetClipPlane8;
		pSetLightState = &CDevice::SetLightStateUnsup;
		pGetLightState = &CDevice::GetLightStateUnsup;
		pSetRenderState = &CDevice::SetRenderState8;
		pGetRenderState = &CDevice::GetRenderState8;
		pBeginStateBlock = &CDevice::BeginStateBlock8;
		pEndStateBlock = &CDevice::EndStateBlock8;
		pApplyStateBlock = &CDevice::ApplyStateBlock8;
		pCaptureStateBlock = &CDevice::CaptureStateBlock8;
		pDeleteStateBlock = &CDevice::DeleteStateBlock8;
		pCreateStateBlock = &CDevice::CreateStateBlock8;
		pSetClipStatus = &CDevice::SetClipStatus8;
		pGetClipStatus = &CDevice::GetClipStatus8;
		pSetTexture = &CDevice::SetTexture8;
		pGetTexture = &CDevice::GetTexture8;
		pGetTextureStageState = &CDevice::GetTextureStageState8;
		pSetTextureStageState = &CDevice::SetTextureStageState8;
		pValidateDevice = &CDevice::ValidateDevice8;
		pGetInfo = &CDevice::GetInfo8;
		pGetVerticalBlankStatus = &CDevice::GetVerticalBlankStatusUnsup;
		pProcessVertices = &CDevice::ProcessVertices8;
		pCreateVertexShader = &CDevice::CreateVertexShader8;
		pCreateVertexShaderInt = &CDevice::CreateVertexShaderIntUnsup;
		pSetVertexShader = &CDevice::SetVertexShader8;
		pSetVertexShaderInt = &CDevice::SetVertexShaderInt8;
		pGetVertexShaderInt = &CDevice::GetVertexShaderIntUnsup;
		pSetFVF = &CDevice::SetFVF8;
		pGetVertexShader = &CDevice::GetVertexShader8;
		pDeleteVertexShader = &CDevice::DeleteVertexShader8;
		pSetVertexShaderConstant = &CDevice::SetVertexShaderConstant8;
		pGetVertexShaderConstant = &CDevice::GetVertexShaderConstant8;
		pSetVertexShaderConstantI = &CDevice::SetVertexShaderConstantIUnsup;
		pGetVertexShaderConstantI = &CDevice::GetVertexShaderConstantIUnsup;
		pSetVertexShaderConstantB = &CDevice::SetVertexShaderConstantBUnsup;
		pGetVertexShaderConstantB = &CDevice::GetVertexShaderConstantBUnsup;
		pGetVertexShaderDeclaration = &CDevice::GetVertexShaderDeclaration8;
		pGetVertexShaderFunction = &CDevice::GetVertexShaderFunction8;
		pSetStreamSource = &CDevice::SetStreamSource8;
		pGetStreamSource = &CDevice::GetStreamSource8;
		pSetIndices = &CDevice::SetIndices8;
		pGetIndices = &CDevice::GetIndices8;
		pCreatePixelShader = &CDevice::CreatePixelShader8;
		pSetPixelShader = &CDevice::SetPixelShader8;
		pGetPixelShader = &CDevice::GetPixelShader8;
		pDeletePixelShader = &CDevice::DeletePixelShader8;
		pCreatePixelShaderInt = &CDevice::CreatePixelShaderInt8;
		pSetPixelShaderInt = &CDevice::SetPixelShaderInt8;
		pGetPixelShaderInt = &CDevice::GetPixelShaderInt8;
		pDeletePixelShaderInt = &CDevice::DeletePixelShaderInt8;
		pSetPixelShaderConstant = &CDevice::SetPixelShaderConstant8;
		pGetPixelShaderConstant = &CDevice::GetPixelShaderConstant8;
		pSetPixelShaderConstantI = &CDevice::SetPixelShaderConstantIUnsup;
		pGetPixelShaderConstantI = &CDevice::GetPixelShaderConstantIUnsup;
		pSetPixelShaderConstantB = &CDevice::SetPixelShaderConstantBUnsup;
		pGetPixelShaderConstantB = &CDevice::GetPixelShaderConstantBUnsup;
		pGetPixelShaderFunction = &CDevice::GetPixelShaderFunction8;
		pDrawRectPatch = &CDevice::DrawRectPatch8;
		pDrawTriPatch = &CDevice::DrawTriPatch8;
		pDeletePatch = &CDevice::DeletePatch8;
		pSetNPatchMode = &CDevice::SetNPatchMode8;
		pGetNPatchMode = &CDevice::GetNPatchModeUnsup;
		pDrawPrimitiveUserMemory = &CDevice::DrawPrimitiveUserMemory8;
		pDrawPrimitiveVertexBuffer = &CDevice::DrawPrimitiveVertexBuffer8;
		pDrawPrimitiveStridedData = &CDevice::DrawPrimitiveStridedData8;
		pDrawPrimitive = &CDevice::DrawPrimitive8;
		pDrawIndexedPrimitive = &CDevice::DrawIndexedPrimitive8;
		pDrawPrimitiveUP = &CDevice::DrawPrimitiveUP8;
		pDrawIndexedPrimitiveUP = &CDevice::DrawIndexedPrimitiveUP8;
		//Do we still need this? pBeginEnd
		pCreateVertexBuffer = &CDevice::CreateVertexBuffer8;
		pCreateIndexBuffer = &CDevice::CreateIndexBuffer8;
		pCreateVertexBuffer2 = &CDevice::CreateVertexBuffer2Unsup;
		pCreateIndexBuffer2 = &CDevice::CreateIndexBuffer2Unsup;
		pPresent = &CDevice::Present8;
		pReleaseD3D = &CDevice::ReleaseD3D8;
		pDraw2DText = &CDevice::Draw2DText8;
		pGetRasterStatus = &CDevice::GetRasterStatus8;
		pSetGammaRamp = &CDevice::SetGammaRamp8;
		pGetGammaRamp = &CDevice::GetGammaRamp8;
		pGetRasterStatus2 = &CDevice::GetRasterStatus2Unsup;
		pSetGammaRamp2 = &CDevice::SetGammaRamp2Unsup;
		pGetGammaRamp2 = &CDevice::GetGammaRamp2Unsup;
		pSetPaletteEntries = &CDevice::SetPaletteEntries8;
		pGetPaletteEntries = &CDevice::GetPaletteEntries8;
		pSetCurrentTexturePalette = &CDevice::SetCurrentTexturePalette8;
		pGetCurrentTexturePalette = &CDevice::GetCurrentTexturePalette8;
		pCreateTexture = &CDevice::CreateTexture8;
		pCreateVolumeTexture = &CDevice::CreateVolumeTexture8;
		pCreateCubeTexture = &CDevice::CreateCubeTexture8;
		pCreateTexture2 = &CDevice::CreateTexture2Unsup;
		pCreateVolumeTexture2 = &CDevice::CreateVolumeTexture2Unsup;
		pCreateCubeTexture2 = &CDevice::CreateCubeTexture2Unsup;
		pCreateAdditionalSwapChain = &CDevice::CreateAdditionalSwapChain8;
		pValidateMode = &CDevice::ValidateMode8;
		pHandleDeviceLost = &CDevice::HandleDeviceLost8;
		pResize = &CDevice::Resize8;
		pIsDeviceLost = &CDevice::IsDeviceLost8;
		pIsDeviceReadyForReset = &CDevice::IsDeviceReadyForReset8;
		pD3DXFontOnLostDevice = &CDevice::D3DXFontOnLostDevice8;
		pD3DXFontOnResetDevice = &CDevice::D3DXFontOnResetDevice8;
		pCreateVertexDeclaration = &CDevice::CreateVertexDeclarationUnsup;
		pSetVertexDeclaration = &CDevice::SetVertexDeclarationUnsup;
		pGetVertexDeclaration = &CDevice::GetVertexDeclarationUnsup;
		pSetScissorRect = &CDevice::SetScissorRectUnsup;
		pGetScissorRect = &CDevice::GetScissorRectUnsup;
		pSetSamplerState = &CDevice::SetSamplerState8;
		pGetSamplerState = &CDevice::GetSamplerState8;
		pSetDialogBoxMode = &CDevice::SetDialogBoxModeUnsup;
		pCreateOffscreenPlainSurface = &CDevice::CreateOffscreenPlainSurfaceUnsup;
		pSetSoftwareVertexProcessing = &CDevice::SetSoftwareVertexProcessing8;
		pGetSoftwareVertexProcessing = &CDevice::GetSoftwareVertexProcessing8;
		pGetSwapChain = &CDevice::GetSwapChainUnsup;
		pGetNumberOfSwapChains = &CDevice::GetNumberOfSwapChainsUnsup;
		pCreateQuery = &CDevice::CreateQueryUnsup;
		pStretchRect = &CDevice::StretchRectUnsup;
		pColorFill = &CDevice::ColorFillUnsup;
		pGetFVF = &CDevice::GetVertexShader8;
		pSetStreamSourceFreq = &CDevice::SetStreamSourceFreqUnsup;
		pGetStreamSourceFreq = &CDevice::GetStreamSourceFreqUnsup;
		pGetRenderTargetData = &CDevice::GetRenderTargetDataUnsup;
		pCreateEffectFromFile = &CDevice::CreateEffectFromFileUnsup;
		pCreateEffectFromResource = &CDevice::CreateEffectFromResourceUnsup;

		pD3DXInitializePSGP = &CDevice::D3DXInitializePSGP8;
		pD3DXCreateMesh = &CDevice::D3DXCreateMesh8;
		pD3DXCreateMeshFVF = &CDevice::D3DXCreateMeshFVF8;
		pD3DXCreateTexture = &CDevice::D3DXCreateTexture8;
		pD3DXCreateTextureFromFileA = &CDevice::D3DXCreateTextureFromFileA8;
		pD3DXCreateTextureFromFileExA = &CDevice::D3DXCreateTextureFromFileExA8;
		pD3DXCreateTextureFromFileInMemory = &CDevice::D3DXCreateTextureFromFileInMemory8;
		pD3DXCreateTextureFromFileInMemoryEx = &CDevice::D3DXCreateTextureFromFileInMemoryEx8;
		pD3DXCreateTextureFromResourceA = &CDevice::D3DXCreateTextureFromResourceA8;
		pD3DXCreateTextureFromResourceExA = &CDevice::D3DXCreateTextureFromResourceExA8;
		pD3DXFilterTexture = &CDevice::D3DXFilterTexture8;
		pD3DXLoadVolumeFromMemory = &CDevice::D3DXLoadVolumeFromMemory8;
		pD3DXLoadVolumeFromVolume = &CDevice::D3DXLoadVolumeFromVolume8;
		pD3DXLoadSurfaceFromSurface = &CDevice::D3DXLoadSurfaceFromSurface8;
		pD3DXLoadSurfaceFromFileInMemory = &CDevice::D3DXLoadSurfaceFromFileInMemory8;
		pD3DXLoadSurfaceFromMemory = &CDevice::D3DXLoadSurfaceFromMemory8;
		pD3DXLoadSurfaceFromFile = &CDevice::D3DXLoadSurfaceFromFile8;
		pD3DXSaveTextureToFileA = &CDevice::D3DXSaveTextureToFileA8;
		pD3DXSaveSurfaceToFileA = &CDevice::D3DXSaveSurfaceToFileA8;
		pD3DXFillTexture = &CDevice::D3DXFillTexture8;
		pCheckDeviceState = &CDevice::CheckDeviceStateUnsup;
	}
	else if(fVersion == 7.f)
	{
		pGetDeviceCaps = &CDevice::GetDeviceCapsUnsup;
		pInitialize = &CDevice::Initialize7;
		pIsDeviceEqual = &CDevice::IsDeviceEqual7;
		pTestCooperativeLevel = &CDevice::TestCooperativeLevelUnsup;
		pGetAvailableTextureMem = &CDevice::GetAvailableTextureMem7;
		pGetDisplayMode = &CDevice::GetDisplayModeUnsup;
		pGetDisplayMode2 = &CDevice::GetDisplayMode2Unsup;
		pGetCreationParameters = &CDevice::GetCreationParametersUnsup;
		pSetCursorProperties = &CDevice::SetCursorPropertiesUnsup;
		pSetCursorPosition = &CDevice::SetCursorPositionUnsup;
		pShowCursor = &CDevice::ShowCursorUnsup;
		pReset = &CDevice::ResetUnsup;
		pGetBackBuffer = &CDevice::GetBackBufferCached;
		pGetBackBuffer2 = &CDevice::GetBackBuffer2Unsup;
		pEvictManagedResources = &CDevice::EvictManagedResources7;
		pResourceManagerDiscardBytes = &CDevice::ResourceManagerDiscardBytesUnsup;
		pTextureFormatSupported = &CDevice::TextureFormatSupported7;
		pZBufferFormatSupported = &CDevice::ZBufferFormatSupported7;
		pCubeFormatSupported = &CDevice::CubeFormatSupported7;
		pVolumeFormatSupported = &CDevice::VolumeFormatSupported7;
		pSetRenderTarget = &CDevice::SetRenderTarget7;
		pGetRenderTarget = &CDevice::GetRenderTarget7;
		pSetRenderTarget2 = &CDevice::SetRenderTargetUnsup;
		pGetRenderTarget2 = &CDevice::GetRenderTargetUnsup;
		pGetDepthStencilSurface = &CDevice::GetDepthStencilSurface7;
		pSetDepthStencilSurface = &CDevice::SetDepthStencilSurfaceUnsup;
		pCreateRenderTarget = &CDevice::CreateRenderTarget7;
		pCreateRenderTarget2 = &CDevice::CreateRenderTarget2Unsup;
		pCreateDepthStencilSurface = &CDevice::CreateDepthStencilSurface7;
		pCreateDepthStencilSurface2 = &CDevice::CreateDepthStencilSurface2Unsup;
		pCreateImageSurface = &CDevice::CreateImageSurface7;
		pGetFrontBuffer = &CDevice::GetFrontBufferUnsup;
		pGetFrontBufferData = &CDevice::GetFrontBufferDataUnsup;
		pCopyRects = &CDevice::CopyRects7;
		pUpdateSurface = &CDevice::UpdateSurfaceUnsup;
		pUpdateTexture = &CDevice::UpdateTexture7;
		pBeginScene = &CDevice::BeginScene7;
		pEndScene = &CDevice::EndScene7;
		pClear = &CDevice::Clear7;
		pSetTransform = &CDevice::SetTransform7;
		pGetTransform = &CDevice::GetTransform7;
		pMultiplyTransform = &CDevice::MultiplyTransform7;
		pSetViewport = &CDevice::SetViewport7;
		pGetViewport = &CDevice::GetViewport7;
		pSetMaterial = &CDevice::SetMaterial7;
		pGetMaterial = &CDevice::GetMaterial7;
		pSetLight = &CDevice::SetLight7;
		pGetLight = &CDevice::GetLight7;
		pLightEnable = &CDevice::LightEnable7;
		pGetLightEnable = &CDevice::GetLightEnable7;
		pSetClipPlane = &CDevice::SetClipPlane7;
		pGetClipPlane = &CDevice::GetClipPlane7;
		pSetLightState = &CDevice::SetLightStateUnsup;
		pGetLightState = &CDevice::GetLightStateUnsup;
		pSetRenderState = &CDevice::SetRenderState7;
		pGetRenderState = &CDevice::GetRenderState7;
		pBeginStateBlock = &CDevice::BeginStateBlock7;
		pEndStateBlock = &CDevice::EndStateBlock7;
		pApplyStateBlock = &CDevice::ApplyStateBlock7;
		pCaptureStateBlock = &CDevice::CaptureStateBlock7;
		pDeleteStateBlock = &CDevice::DeleteStateBlock7;
		pCreateStateBlock = &CDevice::CreateStateBlock7;
		pSetClipStatus = &CDevice::SetClipStatus7;
		pGetClipStatus = &CDevice::GetClipStatus7;
		pSetTexture = &CDevice::SetTexture7;
		pGetTexture = &CDevice::GetTexture7;
		pGetTextureStageState = &CDevice::GetTextureStageState7;
		pSetTextureStageState = &CDevice::SetTextureStageState7;
		pValidateDevice = &CDevice::ValidateDevice7;
		pGetInfo = &CDevice::GetInfo7;
		pGetVerticalBlankStatus = &CDevice::GetVerticalBlankStatus7;
		pProcessVertices = &CDevice::ProcessVerticesUnsup;
		pCreateVertexShader = &CDevice::CreateVertexShaderUnsup;
		pCreateVertexShaderInt = &CDevice::CreateVertexShaderIntUnsup;
		pSetVertexShader = &CDevice::SetVertexShaderUnsup;
		pSetVertexShaderInt = &CDevice::SetVertexShaderIntUnsup;
		pGetVertexShaderInt = &CDevice::GetVertexShaderIntUnsup;
		pSetFVF = &CDevice::SetFVFUnsup;
		pGetVertexShader = &CDevice::GetVertexShaderUnsup;
		pDeleteVertexShader = &CDevice::DeleteVertexShaderUnsup;
		pSetVertexShaderConstant = &CDevice::SetVertexShaderConstantUnsup;
		pGetVertexShaderConstant = &CDevice::GetVertexShaderConstantUnsup;
		pSetVertexShaderConstantI = &CDevice::SetVertexShaderConstantIUnsup;
		pGetVertexShaderConstantI = &CDevice::GetVertexShaderConstantIUnsup;
		pSetVertexShaderConstantB = &CDevice::SetVertexShaderConstantBUnsup;
		pGetVertexShaderConstantB = &CDevice::GetVertexShaderConstantBUnsup;
		pGetVertexShaderDeclaration = &CDevice::GetVertexShaderDeclarationUnsup;
		pGetVertexShaderFunction = &CDevice::GetVertexShaderFunctionUnsup;
		pSetStreamSource = &CDevice::SetStreamSourceUnsup;
		pGetStreamSource = &CDevice::GetStreamSourceUnsup;
		pSetIndices = &CDevice::SetIndicesUnsup;
		pGetIndices = &CDevice::GetIndicesUnsup;
		pCreatePixelShader = &CDevice::CreatePixelShaderUnsup;
		pSetPixelShader = &CDevice::SetPixelShaderUnsup;
		pGetPixelShader = &CDevice::GetPixelShaderUnsup;
		pDeletePixelShader = &CDevice::DeletePixelShaderUnsup;
		pSetPixelShaderConstant = &CDevice::SetPixelShaderConstantUnsup;
		pGetPixelShaderConstant = &CDevice::GetPixelShaderConstantUnsup;
		pSetPixelShaderConstantI = &CDevice::SetPixelShaderConstantIUnsup;
		pGetPixelShaderConstantI = &CDevice::GetPixelShaderConstantIUnsup;
		pSetPixelShaderConstantB = &CDevice::SetPixelShaderConstantBUnsup;
		pGetPixelShaderConstantB = &CDevice::GetPixelShaderConstantBUnsup;
		pGetPixelShaderFunction = &CDevice::GetPixelShaderFunctionUnsup;
		pDrawRectPatch = &CDevice::DrawRectPatchUnsup;
		pDrawTriPatch = &CDevice::DrawTriPatchUnsup;
		pDeletePatch = &CDevice::DeletePatchUnsup;
		pSetNPatchMode = &CDevice::SetNPatchModeUnsup;
		pGetNPatchMode = &CDevice::GetNPatchModeUnsup;
		pDrawPrimitiveUserMemory = &CDevice::DrawPrimitiveUserMemory7;
		pDrawPrimitiveVertexBuffer = &CDevice::DrawPrimitiveVertexBuffer7;
		pDrawPrimitiveStridedData = &CDevice::DrawPrimitiveStridedData7;
		pDrawPrimitive = &CDevice::DrawPrimitiveUnsup;
		pDrawIndexedPrimitive = &CDevice::DrawIndexedPrimitiveUnsup;
		pDrawPrimitiveUP = &CDevice::DrawPrimitiveUPUnsup;
		pDrawIndexedPrimitiveUP = &CDevice::DrawIndexedPrimitiveUPUnsup;
		//pBeginEnd
		pCreateVertexBuffer = &CDevice::CreateVertexBuffer7;
		pCreateIndexBuffer = &CDevice::CreateIndexBufferUnsup;
		pCreateVertexBuffer2 = &CDevice::CreateVertexBuffer2Unsup;
		pCreateIndexBuffer2 = &CDevice::CreateIndexBuffer2Unsup;
		//pCreatePalette
		//pLoad
		pPresent = &CDevice::Present7;
		pReleaseD3D = &CDevice::ReleaseD3D7;
		pDraw2DText = &CDevice::Draw2DText7;
		pGetRasterStatus = &CDevice::GetRasterStatusUnsup;
		pSetGammaRamp = &CDevice::SetGammaRampUnsup;
		pGetGammaRamp = &CDevice::GetGammaRampUnsup;
		pGetRasterStatus2 = &CDevice::GetRasterStatus2Unsup;
		pSetGammaRamp2 = &CDevice::SetGammaRamp2Unsup;
		pGetGammaRamp2 = &CDevice::GetGammaRamp2Unsup;
		pSetPaletteEntries = &CDevice::SetPaletteEntriesUnsup;
		pGetPaletteEntries = &CDevice::GetPaletteEntriesUnsup;
		pSetCurrentTexturePalette = &CDevice::SetCurrentTexturePaletteUnsup;
		pGetCurrentTexturePalette = &CDevice::GetCurrentTexturePaletteUnsup;
		pCreateTexture = &CDevice::CreateTexture7;
		pCreateVolumeTexture = &CDevice::CreateVolumeTextureUnsup;
		pCreateCubeTexture = &CDevice::CreateCubeTexture7;
		pCreateTexture2 = &CDevice::CreateTexture2Unsup;
		pCreateVolumeTexture2 = &CDevice::CreateVolumeTexture2Unsup;
		pCreateCubeTexture2 = &CDevice::CreateCubeTexture2Unsup;
		pCreateAdditionalSwapChain = &CDevice::CreateAdditionalSwapChain7;
		pValidateMode = &CDevice::ValidateMode7;
		pHandleDeviceLost = &CDevice::HandleDeviceLost7;
		pResize = &CDevice::Resize7;
		pIsDeviceLost = &CDevice::IsDeviceLost7;
		pIsDeviceReadyForReset = &CDevice::IsDeviceReadyForReset7;
		pD3DXFontOnLostDevice = &CDevice::D3DXFontOnLostDeviceUnsup;
		pD3DXFontOnResetDevice = &CDevice::D3DXFontOnResetDeviceUnsup;
		pCreateVertexDeclaration = &CDevice::CreateVertexDeclarationUnsup;
		pSetVertexDeclaration = &CDevice::SetVertexDeclarationUnsup;
		pGetVertexDeclaration = &CDevice::GetVertexDeclarationUnsup;
		pSetScissorRect = &CDevice::SetScissorRectUnsup;
		pGetScissorRect = &CDevice::GetScissorRectUnsup;
		pSetSamplerState = &CDevice::SetSamplerState7;
		pGetSamplerState = &CDevice::GetSamplerState7;
		pSetDialogBoxMode = &CDevice::SetDialogBoxModeUnsup;
		pCreateOffscreenPlainSurface = &CDevice::CreateOffscreenPlainSurfaceUnsup;
		pSetSoftwareVertexProcessing = &CDevice::SetSoftwareVertexProcessingUnsup;
		pGetSoftwareVertexProcessing = &CDevice::GetSoftwareVertexProcessingUnsup;
		pGetSwapChain = &CDevice::GetSwapChainUnsup;
		pGetNumberOfSwapChains = &CDevice::GetNumberOfSwapChainsUnsup;
		pCreateQuery = &CDevice::CreateQueryUnsup;
		pStretchRect = &CDevice::StretchRectUnsup;
		pColorFill = &CDevice::ColorFillUnsup;
		pGetFVF = &CDevice::GetFVFUnsup;
		pSetStreamSourceFreq = &CDevice::SetStreamSourceFreqUnsup;
		pGetStreamSourceFreq = &CDevice::GetStreamSourceFreqUnsup;
		pGetRenderTargetData = &CDevice::GetRenderTargetDataUnsup;
		pCreateEffectFromFile = &CDevice::CreateEffectFromFileUnsup;
		pCreateEffectFromResource = &CDevice::CreateEffectFromResourceUnsup;

		pD3DXInitializePSGP = &CDevice::D3DXInitializePSGP8;
		pCheckDeviceState = &CDevice::CheckDeviceStateUnsup;
	}
	else if(fVersion == 6.f)
	{
		pGetDeviceCaps = &CDevice::GetDeviceCapsUnsup;
		pInitialize = &CDevice::Initialize6;
		pIsDeviceEqual = &CDevice::IsDeviceEqual6;
		pTestCooperativeLevel = &CDevice::TestCooperativeLevelUnsup;
		pGetAvailableTextureMem = &CDevice::GetAvailableTextureMem6;
		pGetDisplayMode = &CDevice::GetDisplayModeUnsup;
		pGetDisplayMode2 = &CDevice::GetDisplayMode2Unsup;
		pGetCreationParameters = &CDevice::GetCreationParametersUnsup;
		pSetCursorProperties = &CDevice::SetCursorPropertiesUnsup;
		pSetCursorPosition = &CDevice::SetCursorPositionUnsup;
		pShowCursor = &CDevice::ShowCursorUnsup;
		pReset = &CDevice::ResetUnsup;
		pGetBackBuffer = &CDevice::GetBackBufferCached;
		pGetBackBuffer2 = &CDevice::GetBackBuffer2Unsup;
		pEvictManagedResources = &CDevice::EvictManagedResources6;
		pResourceManagerDiscardBytes = &CDevice::ResourceManagerDiscardBytesUnsup;
		pTextureFormatSupported = &CDevice::TextureFormatSupported7;
		pZBufferFormatSupported = &CDevice::ZBufferFormatSupported7;
		pCubeFormatSupported = &CDevice::CubeFormatSupported7;
		pVolumeFormatSupported = &CDevice::VolumeFormatSupported7;
		pSetRenderTarget = &CDevice::SetRenderTarget6;
		pGetRenderTarget = &CDevice::GetRenderTarget6;
		pSetRenderTarget2 = &CDevice::SetRenderTargetUnsup;
		pGetRenderTarget2 = &CDevice::GetRenderTargetUnsup;
		pGetDepthStencilSurface = &CDevice::GetDepthStencilSurface6;
		pSetDepthStencilSurface = &CDevice::SetDepthStencilSurfaceUnsup;
		pCreateRenderTarget = &CDevice::CreateRenderTarget6;
		pCreateRenderTarget2 = &CDevice::CreateRenderTarget2Unsup;
		pCreateDepthStencilSurface = &CDevice::CreateDepthStencilSurface6;
		pCreateDepthStencilSurface2 = &CDevice::CreateDepthStencilSurface2Unsup;
		pCreateImageSurface = &CDevice::CreateImageSurface6;
		pGetFrontBuffer = &CDevice::GetFrontBufferUnsup;
		pGetFrontBufferData = &CDevice::GetFrontBufferDataUnsup;
		pCopyRects = &CDevice::CopyRects6;
		pUpdateSurface = &CDevice::UpdateSurfaceUnsup;
		pUpdateTexture = &CDevice::UpdateTexture6;
		pBeginScene = &CDevice::BeginScene6;
		pEndScene = &CDevice::EndScene6;
		pClear = &CDevice::Clear6;
		pSetTransform = &CDevice::SetTransform6;
		pGetTransform = &CDevice::GetTransform6;
		pMultiplyTransform = &CDevice::MultiplyTransform6;
		pSetViewport = &CDevice::SetViewport6;
		pGetViewport = &CDevice::GetViewport6;
		pSetMaterial = &CDevice::SetMaterial6;
		pGetMaterial = &CDevice::GetMaterial6;
		pSetLight = &CDevice::SetLight6;
		pGetLight = &CDevice::GetLight6;
		pLightEnable = &CDevice::LightEnable6;
		pGetLightEnable = &CDevice::GetLightEnableUnsup;
		pSetClipPlane = &CDevice::SetClipPlaneUnsup;
		pGetClipPlane = &CDevice::GetClipPlaneUnsup;
		pSetLightState = &CDevice::SetLightState6;
		pGetLightState = &CDevice::GetLightState6;
		pSetRenderState = &CDevice::SetRenderState6;
		pGetRenderState = &CDevice::GetRenderState6;
		pBeginStateBlock = &CDevice::BeginStateBlockUnsup;
		pEndStateBlock = &CDevice::EndStateBlockUnsup;
		pApplyStateBlock = &CDevice::ApplyStateBlockUnsup;
		pCaptureStateBlock = &CDevice::CaptureStateBlockUnsup;
		pDeleteStateBlock = &CDevice::DeleteStateBlockUnsup;
		pCreateStateBlock = &CDevice::CreateStateBlockUnsup;
		pSetClipStatus = &CDevice::SetClipStatus6;
		pGetClipStatus = &CDevice::GetClipStatus6;
		pSetTexture = &CDevice::SetTexture6;
		pGetTexture = &CDevice::GetTexture6;
		pGetTextureStageState = &CDevice::GetTextureStageState6;
		pSetTextureStageState = &CDevice::SetTextureStageState6;
		pValidateDevice = &CDevice::ValidateDevice6;
		pGetInfo = &CDevice::GetInfoUnsup;
		pGetVerticalBlankStatus = &CDevice::GetVerticalBlankStatus6;
		pProcessVertices = &CDevice::ProcessVerticesUnsup;
		pCreateVertexShader = &CDevice::CreateVertexShaderUnsup;
		pCreateVertexShaderInt = &CDevice::CreateVertexShaderIntUnsup;
		pSetVertexShader = &CDevice::SetVertexShaderUnsup;
		pSetVertexShaderInt = &CDevice::SetVertexShaderIntUnsup;
		pGetVertexShaderInt = &CDevice::GetVertexShaderIntUnsup;
		pSetFVF = &CDevice::SetFVFUnsup;
		pGetVertexShader = &CDevice::GetVertexShaderUnsup;
		pDeleteVertexShader = &CDevice::DeleteVertexShaderUnsup;
		pSetVertexShaderConstant = &CDevice::SetVertexShaderConstantUnsup;
		pGetVertexShaderConstant = &CDevice::GetVertexShaderConstantUnsup;
		pSetVertexShaderConstantI = &CDevice::SetVertexShaderConstantIUnsup;
		pGetVertexShaderConstantI = &CDevice::GetVertexShaderConstantIUnsup;
		pSetVertexShaderConstantB = &CDevice::SetVertexShaderConstantBUnsup;
		pGetVertexShaderConstantB = &CDevice::GetVertexShaderConstantBUnsup;
		pGetVertexShaderDeclaration = &CDevice::GetVertexShaderDeclarationUnsup;
		pGetVertexShaderFunction = &CDevice::GetVertexShaderFunctionUnsup;
		pSetStreamSource = &CDevice::SetStreamSourceUnsup;
		pGetStreamSource = &CDevice::GetStreamSourceUnsup;
		pSetIndices = &CDevice::SetIndicesUnsup;
		pGetIndices = &CDevice::GetIndicesUnsup;
		pCreatePixelShader = &CDevice::CreatePixelShaderUnsup;
		pSetPixelShader = &CDevice::SetPixelShaderUnsup;
		pGetPixelShader = &CDevice::GetPixelShaderUnsup;
		pDeletePixelShader = &CDevice::DeletePixelShaderUnsup;
		pSetPixelShaderConstant = &CDevice::SetPixelShaderConstantUnsup;
		pGetPixelShaderConstant = &CDevice::GetPixelShaderConstantUnsup;
		pSetPixelShaderConstantI = &CDevice::SetPixelShaderConstantIUnsup;
		pGetPixelShaderConstantI = &CDevice::GetPixelShaderConstantIUnsup;
		pSetPixelShaderConstantB = &CDevice::SetPixelShaderConstantBUnsup;
		pGetPixelShaderConstantB = &CDevice::GetPixelShaderConstantBUnsup;
		pGetPixelShaderFunction = &CDevice::GetPixelShaderFunctionUnsup;
		pDrawRectPatch = &CDevice::DrawRectPatchUnsup;
		pDrawTriPatch = &CDevice::DrawTriPatchUnsup;
		pDeletePatch = &CDevice::DeletePatchUnsup;
		pSetNPatchMode = &CDevice::SetNPatchModeUnsup;
		pGetNPatchMode = &CDevice::GetNPatchModeUnsup;
		pDrawPrimitiveUserMemory = &CDevice::DrawPrimitiveUserMemory6;
		pDrawPrimitiveVertexBuffer = &CDevice::DrawPrimitiveVertexBuffer6;
		pDrawPrimitiveStridedData = &CDevice::DrawPrimitiveStridedData6;
		pDrawPrimitive = &CDevice::DrawPrimitiveUnsup;
		pDrawIndexedPrimitive = &CDevice::DrawIndexedPrimitiveUnsup;
		pDrawPrimitiveUP = &CDevice::DrawPrimitiveUPUnsup;
		pDrawIndexedPrimitiveUP = &CDevice::DrawIndexedPrimitiveUPUnsup;
		//pBeginEnd
		pCreateVertexBuffer = &CDevice::CreateVertexBuffer6;
		pCreateIndexBuffer = &CDevice::CreateIndexBufferUnsup;
		pCreateVertexBuffer2 = &CDevice::CreateVertexBuffer2Unsup;
		pCreateIndexBuffer2 = &CDevice::CreateIndexBuffer2Unsup;
		//pCreatePalette
		//pLoad
		pPresent = &CDevice::Present6;
		pReleaseD3D = &CDevice::ReleaseD3D6;
		pDraw2DText = &CDevice::Draw2DText6;
		pGetRasterStatus = &CDevice::GetRasterStatusUnsup;
		pSetGammaRamp = &CDevice::SetGammaRampUnsup;
		pGetGammaRamp = &CDevice::GetGammaRampUnsup;
		pGetRasterStatus2 = &CDevice::GetRasterStatus2Unsup;
		pSetGammaRamp2 = &CDevice::SetGammaRamp2Unsup;
		pGetGammaRamp2 = &CDevice::GetGammaRamp2Unsup;
		pSetPaletteEntries = &CDevice::SetPaletteEntriesUnsup;
		pGetPaletteEntries = &CDevice::GetPaletteEntriesUnsup;
		pSetCurrentTexturePalette = &CDevice::SetCurrentTexturePaletteUnsup;
		pGetCurrentTexturePalette = &CDevice::GetCurrentTexturePaletteUnsup;
		pCreateTexture = &CDevice::CreateTexture6;
		pCreateVolumeTexture = &CDevice::CreateVolumeTextureUnsup;
		pCreateCubeTexture = &CDevice::CreateCubeTextureUnsup;
		pCreateTexture2 = &CDevice::CreateTexture2Unsup;
		pCreateVolumeTexture2 = &CDevice::CreateVolumeTexture2Unsup;
		pCreateCubeTexture2 = &CDevice::CreateCubeTexture2Unsup;
		pCreateAdditionalSwapChain = &CDevice::CreateAdditionalSwapChain6;
		pValidateMode = &CDevice::ValidateMode6;
		pHandleDeviceLost = &CDevice::HandleDeviceLost6;
		pResize = &CDevice::Resize7;
		pIsDeviceLost = &CDevice::IsDeviceLost6;
		pIsDeviceReadyForReset = &CDevice::IsDeviceReadyForReset6;
		pD3DXFontOnLostDevice = &CDevice::D3DXFontOnLostDeviceUnsup;
		pD3DXFontOnResetDevice = &CDevice::D3DXFontOnResetDeviceUnsup;
		pCreateVertexDeclaration = &CDevice::CreateVertexDeclarationUnsup;
		pSetVertexDeclaration = &CDevice::SetVertexDeclarationUnsup;
		pGetVertexDeclaration = &CDevice::GetVertexDeclarationUnsup;
		pSetScissorRect = &CDevice::SetScissorRectUnsup;
		pGetScissorRect = &CDevice::GetScissorRectUnsup;
		pSetSamplerState = &CDevice::SetSamplerState6;
		pGetSamplerState = &CDevice::GetSamplerState6;
		pSetDialogBoxMode = &CDevice::SetDialogBoxModeUnsup;
		pCreateOffscreenPlainSurface = &CDevice::CreateOffscreenPlainSurfaceUnsup;
		pSetSoftwareVertexProcessing = &CDevice::SetSoftwareVertexProcessingUnsup;
		pGetSoftwareVertexProcessing = &CDevice::GetSoftwareVertexProcessingUnsup;
		pGetSwapChain = &CDevice::GetSwapChainUnsup;
		pGetNumberOfSwapChains = &CDevice::GetNumberOfSwapChainsUnsup;
		pCreateQuery = &CDevice::CreateQueryUnsup;
		pStretchRect = &CDevice::StretchRectUnsup;
		pColorFill = &CDevice::ColorFillUnsup;
		pGetFVF = &CDevice::GetFVFUnsup;
		pSetStreamSourceFreq = &CDevice::SetStreamSourceFreqUnsup;
		pGetStreamSourceFreq = &CDevice::GetStreamSourceFreqUnsup;
		pGetRenderTargetData = &CDevice::GetRenderTargetDataUnsup;
		pD3DXInitializePSGP = &CDevice::D3DXInitializePSGP8;
		pCreateEffectFromFile = &CDevice::CreateEffectFromFileUnsup;
		pCreateEffectFromResource = &CDevice::CreateEffectFromResourceUnsup;
		pCheckDeviceState = &CDevice::CheckDeviceStateUnsup;
	}

	// Initialize API cycling
	InitializeAPICycling();
}

// --------------------------------------------------------------------------------------

void CDevice::ReleaseFormatLists()
{
	FORMAT *pFmt = NULL, *pFmtTemp = NULL;
	
	pFmt = m_pTextureFormat;
	while(pFmt)
	{
		pFmtTemp = pFmt->pNext;
		SAFEDELETE(pFmt);
		pFmt = pFmtTemp;
	}

	pFmt = m_pCubeFormat;
	while(pFmt)
	{
		pFmtTemp = pFmt->pNext;
		SAFEDELETE(pFmt);
		pFmt = pFmtTemp;
	}
	
	pFmt = m_pVolumeFormat;
	while(pFmt)
	{
		pFmtTemp = pFmt->pNext;
		SAFEDELETE(pFmt);
		pFmt = pFmtTemp;
	}
	
	pFmt = m_pZBufferFormat;
	while(pFmt)
	{
		pFmtTemp = pFmt->pNext;
		SAFEDELETE(pFmt);
		pFmt = pFmtTemp;
	}

	pFmt = m_pBackBufferFormat;
	while(pFmt)
	{
		pFmtTemp = pFmt->pNext;
		SAFEDELETE(pFmt);
		pFmt = pFmtTemp;
	}

	SAFEDELETEA(m_pMultisampType);
}

// --------------------------------------------------------------------------------------

void CDevice::ReleaseAll()
{
	ReleaseFormatLists();

	ReleaseAPICycling();
	
	// Do the version specific D3D releases
	if(pReleaseD3D)
		ReleaseD3D();
}

// --------------------------------------------------------------------------------------

void CDevice::CreateDescription(TCHAR *pcsString)
{
	TCHAR tcsSWRast[256];
	TCHAR* pcsDev = _T("<Unknown>");
	TCHAR* pcsVP = _T("<Unknown>");
	TCHAR* pcsBeh = _T("");
	TCHAR* pcsManaged = _T("");

	// Generate the device description
	if(m_dwBehaviors & DEVICECREATE_MANAGED)
		pcsManaged = _T("Managed ");

	if(m_dwBehaviors & DEVICECREATE_HWVP)
		pcsVP = _T("HW");
	else
		pcsVP = _T("SW");
	
	if(m_dwBehaviors & DEVICECREATE_PURE)
		pcsBeh = _T("Pure ");
	else if(m_dwBehaviors & DEVICECREATE_MIXED)
		pcsBeh = _T("Mixed ");

	if(m_dwDevType == DEVICETYPE_HAL)
		pcsDev = _T("HAL");
	else if(m_dwDevType == DEVICETYPE_REF)
		pcsDev = _T("REF");
	else if(m_dwDevType == DEVICETYPE_SW)
	{
		_stprintf(tcsSWRast, _T("%s(%s)"), _T("SW"), GetD3D()->GetSWRast());
		pcsDev = tcsSWRast;
	}
	else if(m_dwDevType == DEVICETYPE_NULLREF)
		pcsDev = _T("NULLREF");

	_stprintf(pcsString, _T("DX%.1f %s%s%s, %s VP"), 
			  GetInterface(), pcsManaged, pcsBeh, pcsDev, pcsVP);
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::UpdateDevice(PRESENT_PARAMETERS *pParams)
{
	HRESULT 	hr;
	bool		bRestorePSGP = false, bRestoreVPEmu = false, bRestorePPEmu = false;
	DWORD		dwPSGP, dwVPEmu, dwPPEmu;

	if(!m_pD3D || !pParams)
	{
		DPF(1, _T("CDevice::Instantiate() - Invalid ptrs.\n"));
		return E_FAIL;
	}

	if(floor(m_fInterface) < m_pD3D->GetMinVersion() || floor(m_fInterface) > m_pD3D->GetMaxVersion())
	{
		DPF(1, ("CDevice::Instantiate() - version %.0f unsupported.\n"), m_fInterface);
		return E_FAIL;
	}

	// Release any existing D3D objects
	if(pReleaseD3D)
	   ReleaseD3D();
	
	// Reset PSGP 
	if (m_dwPSGPRegChange != PSGPREGCHANGE_NONE) 
	{
		dwPSGP = SetPSGPRegistry(m_dwPSGPRegChange);
		bRestorePSGP = true;
	}

	if (m_dwVPEmuRegChange != VPEMUREGCHANGE_NONE) 
	{
		dwVPEmu = SetVPEmuRegistry(m_dwVPEmuRegChange);
		bRestoreVPEmu = true;
	}
	if (m_dwPPEmuRegChange != PPEMUREGCHANGE_NONE) 
	{
		dwPPEmu = SetPPEmuRegistry(m_dwPPEmuRegChange);
		bRestorePPEmu = true;
	}	
	
	// Create the D3D device
	hr = Initialize(pParams);
	if(hr != S_OK)
	{
		DPF((hr == E_UNSUPPORTED) || (hr == S_FALSE) ? 2 : 1, _T("CDevice::UpdateDevice() - Initialize failed. hr=%s(%x)\n"),
			m_pD3D->HResultToString(hr), hr);
		return hr;
	}

	// Make a call to D3DX to initialize its PSGP settings
	D3DXInitializePSGP();

	if (bRestorePSGP)
		SetPSGPRegistry(dwPSGP);
	if (bRestoreVPEmu)
		SetVPEmuRegistry(dwVPEmu);
	if (bRestorePPEmu)
		SetPPEmuRegistry(dwPPEmu);

	// Clear the primary
	hr = Clear(0, NULL, FMWK_DEFAULT, 0x00000000, 1.f, 0x00000000);
	if(FAILED(hr))
	{
		DPF(1, _T("CDevice::UpdateDevice() - Clear() failed.hr=%s(%x)\n"), 
			m_pD3D->HResultToString(hr), hr);
		return hr;
	}

	if(!pParams->bWindowed)
		m_bFullscreen = true;

	return S_OK;
}

// --------------------------------------------------------------------------------------

bool CDevice::CopyFormatList(FORMAT *pSrc, FORMAT **pDest)
{
	if(pDest == NULL)
	{
		DPF(1, _T("CDevice::CopyFormatList() - invalid params.\n"));
		return false;
	}

	FORMAT *pSrcTemp = pSrc;
	FORMAT **pDestTemp = pDest;

	while(pSrcTemp)
	{
		*pDestTemp = new FORMAT;
		if(*pDestTemp == NULL)
		{
			DPF(1, _T("CDevice::CopyFormatList() - Out of memory.\n"));
			return false;
		}

		memcpy(*pDestTemp, pSrcTemp, sizeof(FORMAT));

		pSrcTemp = pSrcTemp->pNext;
		pDestTemp = &((*pDestTemp)->pNext);
	}

	return true;
}

// --------------------------------------------------------------------------------------

void CDevice::GenerateColorCube(PALETTEENTRY* rgPal, DWORD dwNumEntries, float fVersion)
{
	int i;
	BYTE bRedSize, bGreenSize, bBlueSize;

	switch(dwNumEntries)
	{
		case	1:
				// 1 bpp - 2 entries
				bRedSize = 1;
				bGreenSize = 2;
				bBlueSize = 1;
				break;
		case	2:
				// 2 bpp - 4 entries
				bRedSize = 2;
				bGreenSize = 2;
				bBlueSize = 1;
				break;
		case	4:
				// 4 bpp - 8 entries
				bRedSize = 2;
				bGreenSize = 2;
				bBlueSize = 2;
				
				// Dx8 treats Flag field as alpha, so initialize it to 255
				for (i=0; i < 16; i++)
					rgPal[i].peFlags = (fVersion <= 7.0f) ? PC_NOCOLLAPSE : 255;

				break;
		case	8:
				// 8 bpp - 216 entries
				bRedSize = 6;
				bGreenSize = 6;
				bBlueSize = 6;

				// Dx8 treats Flag field as alpha, so initialize it to 255
				for (i=0; i < 256; i++)
					rgPal[i].peFlags = (fVersion <= 7.0f) ? PC_NOCOLLAPSE : 255;

				break;
		default:
				// No palette flags set, so don't make a cube
				return;
	}

	for(BYTE bRed=0; bRed < bRedSize; bRed++)
	{
		for(BYTE bGreen=0; bGreen < bGreenSize; bGreen++)
		{
			for(BYTE bBlue=0; bBlue < bBlueSize; bBlue++)
			{
				LPPALETTEENTRY lpCurrent;
			   
				lpCurrent = rgPal + bRed + ((UINT_PTR)(bGreen * bRedSize)) + ((UINT_PTR)(bBlue * (bGreenSize * bRedSize)));
				lpCurrent->peRed = (bRedSize == 1 ? 0 : (BYTE)(255.0f * ((float)bRed / (float)(bRedSize - 1))));
				lpCurrent->peGreen = (BYTE)(255.0f * ((float)bGreen / (float)(bGreenSize - 1)));
				lpCurrent->peBlue = (bBlueSize == 1 ? 0 : (BYTE)(255.0f * ((float)bBlue / (float)(bBlueSize - 1))));
			}
		}
	}
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::SetLastError(HRESULT hr)
{ 
	return m_pD3D->SetLastError(hr); 
}

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetLastError()
{ 
	return m_pD3D->GetLastError();
}

// --------------------------------------------------------------------------------------

void CDevice::NextTexLoad(SURFACEDESC *pDesc)
{	
	while(1)
	{
		m_dwTexLoad = (m_dwTexLoad % 4) + 1;

		if(m_pD3D->GetVersion() >= 9.f)
		{
			// UpdateSurface only supported for POOL_DEFAULT(DX9)
			if(pDesc->Pool != POOL_DEFAULT && m_dwTexLoad == TEXLOAD_UPDATESURFACE)
				continue;

			// CopyRects is not supported on DX9
			if(m_dwTexLoad == TEXLOAD_COPYRECTS)
				continue;
		}

		if(m_pD3D->GetVersion() >= 8.f)
		{
			// LOCKCOPY only supported for POOL_DEFAULT if DYNAMIC is also specified for Usage(DX8)
			if((!(pDesc->Usage & USAGE_DYNAMIC) && pDesc->Pool == POOL_DEFAULT) && m_dwTexLoad == TEXLOAD_LOCKCOPY)
				continue;

			// UpdateTexture only supported for POOL_DEFAULT(DX8)
			if(pDesc->Pool != POOL_DEFAULT && m_dwTexLoad == TEXLOAD_UPDATETEXTURE)
				continue;
		}

		if(m_pD3D->GetVersion() < 9.f)
		{
			// UpdateSurface is only supported on >= DX9
			if(m_dwTexLoad == TEXLOAD_UPDATESURFACE)
				continue;
		}
		
		break;
	}
}

// --------------------------------------------------------------------------------------

void CDevice::NextTexLoad(VOLUME_DESC *pDesc)
{	
	while(1)
	{
		m_dwTexLoad = (m_dwTexLoad % 4) + 1;

		// LOCKCOPY only supported for POOL_DEFAULT if DYNAMIC is also specified for Usage(DX8)
		if((!(pDesc->Usage & USAGE_DYNAMIC) && pDesc->Pool == POOL_DEFAULT) && m_dwTexLoad == TEXLOAD_LOCKCOPY)
			continue;

		// UpdateTexture only supported for POOL_DEFAULT(DX8)
		if(pDesc->Pool != POOL_DEFAULT && m_dwTexLoad == TEXLOAD_UPDATETEXTURE)
			continue;
		
		// COPYRECTS and UPDATESURFACE not supported for Volumes
		if(m_dwTexLoad == TEXLOAD_COPYRECTS || m_dwTexLoad == TEXLOAD_UPDATESURFACE) 
			continue;

		break;
	}
}

// -------------------------------------------------------------------------------------

void * CDevice::GetInternalPointer(void)
{
	if (m_pDevice9)
		return m_pDevice9;
	if (m_pDevice8)
		return m_pDevice8;
	else if (m_pDevice7)
		return m_pDevice7;
	else if (m_pDevice3)
		return m_pDevice3;

	//No internal pointer set
	return NULL;
}

// --------------------------------------------------------------------------------------

DWORD CDevice::SetPSGPRegistry(DWORD dwPSGP)
{
	HKEY	hD3DReg;
	DWORD	dwOriginal = 0;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\MICROSOFT\\Direct3D",0,KEY_ALL_ACCESS,&hD3DReg))
	{
		DWORD	dwSize = sizeof(DWORD);
		DWORD	dwType;

		if (ERROR_SUCCESS != RegQueryValueEx(hD3DReg,"DisablePSGP",NULL,&dwType,(PBYTE)&dwOriginal,&dwSize))
			dwOriginal = PSGPREGCHANGE_NONE;
		else
			dwOriginal = (dwOriginal == 0) ? PSGPREGCHANGE_ENABLE : PSGPREGCHANGE_DISABLE;

		if (dwPSGP == PSGPREGCHANGE_NONE)
		{
			RegDeleteValue(hD3DReg, "DisablePSGP");
			RegCloseKey(hD3DReg);
		}
		else
		{
			DWORD	dwValue = (dwPSGP == PSGPREGCHANGE_ENABLE) ? 0 : 1;

			RegSetValueEx(hD3DReg,"DisablePSGP",0,REG_DWORD,(PBYTE)&dwValue,sizeof(DWORD));
			RegCloseKey(hD3DReg);
		}
	}
	return dwOriginal;
}
bool CDevice::IsPSGPEnabled() 
{ 
	return (m_dwPSGPRegChange == PSGPREGCHANGE_NONE ? m_pD3D->GetPSGP() : m_dwPSGPRegChange == PSGPREGCHANGE_ENABLE); 
}
	
void CDevice::SetVPEmuRegChange(DWORD dwVPEmuRegChange)
{
	m_dwVPEmuRegChange = dwVPEmuRegChange;

	if (dwVPEmuRegChange == VPEMUREGCHANGE_NONE)
	{
		if ( DEVICETYPE_REF == m_dwDevType )
		{
			m_bVPEmu = false;
		}
		else if ( DRIVERMODEL_LDDM == m_pD3D->m_DriverModel )
		{
			const CAPS *caps = GetCachedCaps9();
			
			if ( caps && (0xFFFF & (2<<8)) <= (0xFFFF &  caps->VertexShaderVersion) )
			{
				m_bVPEmu = true;
			}
			else
			{
				m_bVPEmu = false;
			}
		}
		else
		{
			m_bVPEmu = false;
		}
	}
	else
	{
		m_bVPEmu = (dwVPEmuRegChange == VPEMUREGCHANGE_ENABLE) ? true : false;
	}
}
void CDevice::SetPPEmuRegChange(DWORD dwPPEmuRegChange)
{
	m_dwPPEmuRegChange = dwPPEmuRegChange;

	if ( dwPPEmuRegChange == PPEMUREGCHANGE_NONE )
	{
		if ( DEVICETYPE_REF == m_dwDevType )
		{
			m_bPPEmu = false;
		}
		else if ( DRIVERMODEL_LDDM == m_pD3D->m_DriverModel )
		{
			const CAPS *caps = GetCachedCaps9();
			
			if ( caps && (0xFFFF & (2<<8)) <= (0xFFFF & caps->PixelShaderVersion) )
			{
				m_bPPEmu = true;
			}
			else
			{
				m_bPPEmu = false;
			}
		}
		else
		{
			m_bPPEmu = false;
		}
	}
	else
	{
		m_bPPEmu = (dwPPEmuRegChange == PPEMUREGCHANGE_ENABLE) ? true : false;
	}
}
DWORD CDevice::SetVPEmuRegistry(DWORD dwVPEmu)
{
	HKEY	hD3DReg;
	DWORD	dwOriginal = 0;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\MICROSOFT\\Direct3D",0,KEY_ALL_ACCESS,&hD3DReg))
	{
		DWORD	dwSize = sizeof(DWORD);
		DWORD	dwType;

		if (ERROR_SUCCESS != RegQueryValueEx(hD3DReg,"UseVSConverter",NULL,&dwType,(PBYTE)&dwOriginal,&dwSize))
			dwOriginal = VPEMUREGCHANGE_NONE;
		else
			dwOriginal = (dwOriginal == 0) ? VPEMUREGCHANGE_ENABLE : VPEMUREGCHANGE_DISABLE;

		if (dwVPEmu == VPEMUREGCHANGE_NONE)
		{
			RegDeleteValue(hD3DReg, "UseVSConverter");
			RegDeleteValue(hD3DReg, "UseVSConverterWithRef");
			RegDeleteValue(hD3DReg, "UseVSConverterSW");
			RegCloseKey(hD3DReg);	
		}
		else
		{
			DWORD	dwValue = (dwVPEmu == VPEMUREGCHANGE_ENABLE) ? 1 : 0;

			RegSetValueEx(hD3DReg,"UseVSConverterSW",0,REG_DWORD,(PBYTE)&dwValue,sizeof(DWORD));
			if ( DEVICETYPE_REF == m_dwDevType )
				RegSetValueEx(hD3DReg,"UseVSConverterWithRef",0,REG_DWORD,(PBYTE)&dwValue,sizeof(DWORD));
			else
				RegSetValueEx(hD3DReg,"UseVSConverter",0,REG_DWORD,(PBYTE)&dwValue,sizeof(DWORD));

			RegCloseKey(hD3DReg);
		}
	}
	return dwOriginal;
}
DWORD CDevice::SetPPEmuRegistry(DWORD dwPPEmu)
{
	HKEY	hD3DReg;
	DWORD	dwOriginal = 0;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\MICROSOFT\\Direct3D",0,KEY_ALL_ACCESS,&hD3DReg))
	{
		DWORD	dwSize = sizeof(DWORD);
		DWORD	dwType;

		if (ERROR_SUCCESS != RegQueryValueEx(hD3DReg,"UsePSConverter",NULL,&dwType,(PBYTE)&dwOriginal,&dwSize))
			dwOriginal = PPEMUREGCHANGE_NONE;
		else
			dwOriginal = (dwOriginal == 0) ? PPEMUREGCHANGE_ENABLE : PPEMUREGCHANGE_DISABLE;

		if (dwPPEmu == PPEMUREGCHANGE_NONE)
		{
			RegDeleteValue(hD3DReg, "UsePSConverter");
			RegDeleteValue(hD3DReg, "UsePSConverterWithRef");
			RegCloseKey(hD3DReg);
		}
		else
		{
			DWORD	dwValue = (dwPPEmu == PPEMUREGCHANGE_ENABLE) ? 1 : 0;
			
			if ( DEVICETYPE_REF == m_dwDevType )
				RegSetValueEx(hD3DReg,"UsePSConverterWithRef",0,REG_DWORD,(PBYTE)&dwValue,sizeof(DWORD));
			else
				RegSetValueEx(hD3DReg,"UsePSConverter",0,REG_DWORD,(PBYTE)&dwValue,sizeof(DWORD));

			RegCloseKey(hD3DReg);
		}
	}
	return dwOriginal;
}

