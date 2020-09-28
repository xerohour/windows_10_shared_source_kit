// --------------------------------------------------------------------------------------
// Generic Device Wrappers
// --------------------------------------------------------------------------------------

#include "DevicePCH.h"

#include "CDevice.h"
#include "CSurface.h"
#include "CTextures.h"

// --------------------------------------------------------------------------------------

HRESULT CDevice::GetBackBufferCached(UINT BackBuffer, DWORD Type, CSurface** ppBackBuffer)
{
	if (m_pBackBuffers == NULL || BackBuffer >= m_nBackBuffers)
	{
		DPF(1, _T("CDevice::GetBackBuffer() - Invalid params.\n"));
		return SetLastError(E_FAIL);
	}

	m_pBackBuffers[BackBuffer]->AddRef();
	*ppBackBuffer = m_pBackBuffers[BackBuffer];

	return S_OK;
}

// --------------------------------------------------------------------------------------
// Functions for interfaces which do not support particular features
// --------------------------------------------------------------------------------------

HRESULT CDevice::GetDeviceCapsUnsup(CAPS* pCaps)
{
	DPF(1, "CDevice: GetDeviceCaps() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetVerticalBlankStatusUnsup(LPBOOL lpbIsInVB)
{
	DPF(1, "CDevice: GetVerticalBlankStatus() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreatePixelShaderUnsup(DWORD* pFunction,DWORD *pHandle)
{
	DPF(1, "CDevice: CreatePixelShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetPixelShaderUnsup(DWORD Handle)
{
	DPF(1, "CDevice: SetPixelShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetPixelShaderUnsup(DWORD* pHandle)
{
	DPF(1, "CDevice: GetPixelShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::DeletePixelShaderUnsup(DWORD Handle)
{
	DPF(1, "CDevice: DeletePixelShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetPixelShaderConstantUnsup(UINT Register,const float* pConstantData,UINT ConstantCount)
{
	DPF(1, "CDevice: SetPixelShaderConstant() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetPixelShaderConstantUnsup(UINT Register,float* pConstantData,UINT ConstantCount)
{
	DPF(1, "CDevice: GetPixelShaderConstant() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetPixelShaderConstantIUnsup(UINT StartRegister,const int* pConstantData,UINT Vector4iCount)
{
	DPF(1, "CDevice: SetPixelShaderConstantI() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetPixelShaderConstantIUnsup(UINT StartRegister,int* pConstantData,UINT Vector4iCount)
{
	DPF(1, "CDevice: GetPixelShaderConstantI() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetPixelShaderConstantBUnsup(UINT StartRegister,const BOOL* pConstantData,UINT BoolCount)
{
	DPF(1, "CDevice: SetPixelShaderConstantB() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetPixelShaderConstantBUnsup(UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
{
	DPF(1, "CDevice: GetPixelShaderConstantB() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetPixelShaderFunctionUnsup(DWORD Handle,void* pData,DWORD* pSizeOfData)
{
	DPF(1, "CDevice: GetPixelShaderFunction() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::CreateIndexBufferUnsup(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB)
{
	DPF(1, "CDevice: CreateIndexBuffer() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::CreateVertexDeclarationUnsup(VERTEXELEMENT *pVE, CVertexDeclaration **pDecl)
{
	DPF(1, "CDevice: CreateVertexDeclaration() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::CreateVertexShaderUnsup(DWORD* pDeclaration,DWORD* pFunction,CVertexShader** pHandle,DWORD Usage)
{
	DPF(1, "CDevice: CreateVertexShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::CreateVertexShaderIntUnsup(DWORD* pFunction, CVertexShader **pVShader)
{
	DPF(1, "CDevice: CreateVertexShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetVertexShaderUnsup(DWORD Handle)
{
	DPF(1, "CDevice: SetVertexShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetVertexShaderIntUnsup(CVertexShader* Handle)
{
	DPF(1, "CDevice: SetVertexShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetVertexShaderIntUnsup(CVertexShader** Handle)
{
	DPF(1, "CDevice: GetVertexShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetFVFUnsup(DWORD dwFVF)
{
	DPF(1, "CDevice: SetFVF() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetVertexShaderUnsup(DWORD* pHandle)
{
	DPF(1, "CDevice: GetVertexShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::DeleteVertexShaderUnsup(CVertexShader* Handle)
{
	DPF(1, "CDevice: DeleteVertexShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetVertexShaderConstantUnsup(UINT Register, CONST float* pConstantData,UINT ConstantCount)
{
	DPF(1, "CDevice: SetVertexShaderConstant() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetVertexShaderConstantUnsup(UINT Register,float* pConstantData,UINT ConstantCount)
{
	DPF(1, "CDevice: GetVertexShaderConstant() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetVertexShaderDeclarationUnsup(DWORD Handle,void* pData,DWORD* pSizeOfData)
{
	DPF(1, "CDevice: GetVertexShaderDeclaration() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetVertexShaderFunctionUnsup(DWORD Handle,void* pData,DWORD* pSizeOfData)
{
	DPF(1, "CDevice: GetVertexShaderFunction() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetStreamSourceUnsup(UINT streamNumber, CVertexBuffer *pStreamData, UINT stride, UINT offset)
{
	DPF(1, "CDevice: SetStreamSource() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetStreamSourceUnsup(UINT streamNumber, CVertexBuffer **ppStreamData, UINT *pStride)
{
	DPF(1, "CDevice: GetStreamSource() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetIndicesUnsup(CIndexBuffer *pIndexData, UINT baseVertexIndex)
{
	DPF(1, "CDevice: SetIndices() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetIndicesUnsup(CIndexBuffer **ppIndexData,  UINT *pBaseVertexIndex)
{
	DPF(1, "CDevice: GetIndices() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::DrawRectPatchUnsup(UINT uHandle, float *pSegments, RECTPATCH_INFO *pInfo)
{
	DPF(1, "CDevice: DrawRectPatch() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::DrawTriPatchUnsup(UINT uHandle, float *pSegments, TRIPATCH_INFO *pInfo)
{
	DPF(1, "CDevice: DrawTriPatch() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::DeletePatchUnsup(UINT uHandle)
{
	DPF(1, "CDevice: DeletePatch() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetNPatchModeUnsup(float nSegments)
{
	DPF(1, "CDevice: SetNPatchMode() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
float CDevice::GetNPatchModeUnsup()
{
	DPF(1, "CDevice: GetNPatchMode() unsupported on version %4.2f interfaces.\n", m_fInterface);
	SetLastError(E_NOTIMPL);
	return 0.f;
}
HRESULT CDevice::TestCooperativeLevelUnsup(void)
{
	DPF(1, "CDevice: TestCooperativeLevel() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
UINT CDevice::GetAvailableTextureMemUnsup(void)
{
	DPF(1, "CDevice: GetAvailableTextureMem() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return 0;
}
HRESULT CDevice::GetBackBufferUnsup(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer)
{
	DPF(1, "CDevice: GetBackBuffer() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetBackBuffer2Unsup(UINT iSwapChain,UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer)
{
	DPF(1, "CDevice: GetBackBuffer() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::ResourceManagerDiscardBytesUnsup(DWORD Bytes)
{
	DPF(1, "CDevice: ResourceManagerDiscardBytes() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetRasterStatusUnsup(RASTER_STATUS* pRasterStatus)
{
	DPF(1, "CDevice: GetRasterStatus() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetRasterStatus2Unsup(UINT uSwapChain, RASTER_STATUS* pRasterStatus)
{
	DPF(1, "CDevice: GetRasterStatus(UINT, RASTER_STATUS*) unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
void  CDevice::SetGammaRampUnsup(DWORD Flags,GAMMARAMP* pRamp)
{
	DPF(1, "CDevice: SetGammaRamp() unsupported on version %4.2f interfaces.\n", m_fInterface);
}
void  CDevice::SetGammaRamp2Unsup(UINT uSwapChain, DWORD Flags,GAMMARAMP* pRamp)
{
	DPF(1, "CDevice: SetGammaRamp(UINT, DWORD, GAMMARAMP) unsupported on version %4.2f interfaces.\n", m_fInterface);
}
void  CDevice::GetGammaRampUnsup(GAMMARAMP* pRamp)
{
	DPF(1, "CDevice: GetGammaRamp() unsupported on version %4.2f interfaces.\n", m_fInterface);
}
void  CDevice::GetGammaRamp2Unsup(UINT uSwapChain, GAMMARAMP* pRamp)
{
	DPF(1, "CDevice: GetGammaRamp(UINT, GAMMARAMP*) unsupported on version %4.2f interfaces.\n", m_fInterface);
}
HRESULT CDevice::SetPaletteEntriesUnsup(UINT PaletteNumber,PALETTEENTRY* pEntries)
{
	DPF(1, "CDevice: SetPaletteEntries() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetPaletteEntriesUnsup(UINT PaletteNumber,PALETTEENTRY* pEntries)
{
	DPF(1, "CDevice: GetPaletteEntries() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetCurrentTexturePaletteUnsup(UINT PaletteNumber)
{
	DPF(1, "CDevice: SetCurrentTexturePalette() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetCurrentTexturePaletteUnsup(UINT *PaletteNumber)
{
	DPF(1, "CDevice: GetCurrentTexturePalette() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::CreateTextureUnsup(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format, DWORD Pool,CTexture** ppTexture)
{
	DPF(1, "CDevice: CreateTexture() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::CreateVolumeTextureUnsup(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture** ppVolumeTexture)
{
	DPF(1, "CDevice: CreateVolumeTexture() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::CreateCubeTextureUnsup(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture** ppCubeTexture)
{
	DPF(1, "CDevice: CreateCubeTexture() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetFrontBufferUnsup(CSurface* pDestSurface)
{
	DPF(1, "CDevice: GetFrontBuffer() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetRenderTargetDataUnsup(CSurface* pSrcRenderTarget, CSurface* pDestSurface)
{
	DPF(1, "CDevice: GetRenderTargetData() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetFrontBufferDataUnsup(UINT iSwapChain, CSurface* pDestSurface)
{
	DPF(1, "CDevice: GetFrontBufferData() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::UpdateTextureUnsup(CBaseTexture* pSourceTexture,CBaseTexture* pDestinationTexture)
{
	DPF(1, "CDevice: UpdateTexture() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::CopyRectsUnsup(CSurface* pSourceSurface,RECT* pSourceRectsArray,UINT cRects,CSurface* pDestinationSurface,POINT* pDestPointsArray)
{
	DPF(1, "CDevice: CopyRects() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetDisplayModeUnsup(DISPLAYMODE* pMode)
{
	DPF(1, "CDevice: GetDisplayMode(DISPLAYMODE) unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetDisplayMode2Unsup(UINT uSwapChain, DISPLAYMODE* pMode)
{
	DPF(1, "CDevice: GetDisplayMode(UINT, DISPLAYMODE) unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::GetCreationParametersUnsup(DEVICE_CREATION_PARAMETERS *pParameters)
{
	DPF(1, "CDevice: GetCreationParameters() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::ResetUnsup(PRESENT_PARAMETERS* pPresentationParameters)
{
	DPF(1, "CDevice: Reset() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetCursorPropertiesUnsup(UINT XHotSpot,UINT YHotSpot,CSurface* pCursorBitmap)
{
	DPF(1, "CDevice: SetCursorProperties() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
void CDevice::SetCursorPositionUnsup(UINT XScreenSpace,UINT YScreenSpace,DWORD Flags)
{
	DPF(1, "CDevice: SetCursorPosition() unsupported on version %4.2f interfaces.\n", m_fInterface);
}
BOOL CDevice::ShowCursorUnsup(BOOL bShow)
{
	DPF(1, "CDevice: ShowCursor() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return bShow;
}

HRESULT CDevice::ProcessVerticesUnsup(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,CVertexBuffer* pDestBuffer, CVertexDeclaration *pDecl, DWORD Flags)
{
	DPF(1, "CDevice: ProcessVertices() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::DrawPrimitiveUnsup(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	DPF(1, "CDevice: DrawPrimitive() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::DrawIndexedPrimitiveUnsup(D3DPRIMITIVETYPE,UINT minIndex,UINT NumVertices,UINT startIndex,UINT primCount,INT BaseVertexIndex)
{
	DPF(1, "CDevice: DrawIndexedPrimitive() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::DrawPrimitiveUPUnsup(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	DPF(1, "CDevice: DrawPrimitiveUP() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::DrawIndexedPrimitiveUPUnsup(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,void* pIndexData,FORMAT IndexDataFormat,void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	DPF(1, "CDevice: DrawIndexedPrimitiveUP() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetLightUnsup(DWORD dwLightIndex,LPLIGHT lpLight)
{
	DPF(1, "CDevice: SetLight() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetLightUnsup(DWORD dwLightIndex,LPLIGHT lpLight)
{
	DPF(1, "CDevice: GetLight() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::LightEnableUnsup(DWORD dwLightIndex,BOOL bEnable)
{
	DPF(1, "CDevice: LightEnable() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetLightEnableUnsup(DWORD dwLightIndex,BOOL *bEnable)
{
	DPF(1, "CDevice: GetLightEnable() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetClipPlaneUnsup(DWORD dwIndex,D3DVALUE* pPlaneEquation)
{
	DPF(1, "CDevice: SetClipPlane() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetClipPlaneUnsup(DWORD dwIndex,D3DVALUE* pPlaneEquation)
{
	DPF(1, "CDevice: GetClipPlane() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::BeginStateBlockUnsup()
{
	DPF(1, "CDevice: BeginStateBlock() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::EndStateBlockUnsup(UINT_PTR *dwHandle)
{
	DPF(1, "CDevice: EndStateBlock() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::ApplyStateBlockUnsup(UINT_PTR dwHandle)
{
	DPF(1, "CDevice: ApplyStateBlock() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CaptureStateBlockUnsup(UINT_PTR dwHandle)
{
	DPF(1, "CDevice: CaptureStateBlock() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::DeleteStateBlockUnsup(UINT_PTR dwHandle)
{
	DPF(1, "CDevice: DeleteStateBlock() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateStateBlockUnsup(D3DSTATEBLOCKTYPE sbt,  UINT_PTR *dwHandle)
{
	DPF(1, "CDevice: CreateStateBlock() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetClipStatusUnsup(LPCLIPSTATUS pClipStatus)
{
	DPF(1, "CDevice: SetClipStatus() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetClipStatusUnsup(LPCLIPSTATUS pClipStatus)
{
	DPF(1, "CDevice: GetClipStatus() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetInfoUnsup(DWORD dwDevInfoID,LPVOID pDevInfoStruct,DWORD dwSize)
{
	DPF(1, "CDevice: GetInfo() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetLightStateUnsup(D3DLIGHTSTATETYPE dwState, DWORD dwValue)
{
	DPF(1, "CDevice: SetLightState() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetLightStateUnsup(D3DLIGHTSTATETYPE dwState, DWORD *pdwValue)
{
	DPF(1, "CDevice: GetLightState() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateImageSurfaceUnsup(UINT Width,UINT Height,FORMAT Format, CSurface** ppSurface)
{
	DPF(1, "CDevice: CreateImageSurface() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::StretchRectUnsup(CSurface* pSourceSurface,CONST RECT* pSourceRect,CSurface* pDestSurface,CONST RECT* pDestRect, DWORD dwFilter)
{
	DPF(1, "CDevice: StretchRect() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::ColorFillUnsup(CSurface* pSurface,CONST RECT* pRect,DWORD color)
{
	DPF(1, "CDevice: ColorFill() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateOffscreenPlainSurfaceUnsup(UINT Width,UINT Height,FORMAT Format,DWORD dwPool, CSurface** ppSurface, HANDLE *pSharedHandle)
{
	DPF(1, "CDevice: CreateOffscreenPlainSurface() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
HRESULT CDevice::SetScissorRectUnsup(CONST RECT* pRect)
{
	DPF(1, "CDevice: SetScissorRect() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetScissorRectUnsup(RECT* pRect)
{
	DPF(1, "CDevice: GetScissorRect() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateVertexDeclarationUnsup(CONST VERTEXELEMENT* pVertexElements,CVertexDeclaration** ppDecl)
{
	DPF(1, "CDevice: CreateVertexDeclaration() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetVertexDeclarationUnsup(CVertexDeclaration* pDecl)
{
	DPF(1, "CDevice: SetVertexDeclaration() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetVertexDeclarationUnsup(CVertexDeclaration** ppDecl)
{
	DPF(1, "CDevice: GetVertexDeclaration() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetFVFUnsup(DWORD* pFVF)
{
	DPF(1, "CDevice: GetFVF() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetVertexShaderConstantFUnsup(UINT Register,CONST float* pConstantData,UINT ConstantCount)
{
	DPF(1, "CDevice: SetVertexShaderConstantF() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetVertexShaderConstantFUnsup(UINT Register,float* pConstantData,UINT ConstantCount)
{
	DPF(1, "CDevice: GetVertexShaderConstantF() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetVertexShaderConstantIUnsup(UINT Register,CONST int* pConstantData,UINT ConstantCount)
{
	DPF(1, "CDevice: SetVertexShaderConstantI() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetVertexShaderConstantIUnsup(UINT Register,int* pConstantData,UINT ConstantCount)
{
	DPF(1, "CDevice: GetVertexShaderConstantI() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetVertexShaderConstantBUnsup(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount)
{
	DPF(1, "CDevice: SetVertexShaderConstantB() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetVertexShaderConstantBUnsup(UINT StartRegister,BOOL* pConstantData, UINT BoolCount)
{
	DPF(1, "CDevice: GetVertexShaderConstantB() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreatePixelShaderIntUnsup(DWORD *pFunc, CPShader** pShader)
{
	DPF(1, "CDevice: CreatePixelShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetPixelShaderIntUnsup(CPShader* pShader)
{
	DPF(1, "CDevice: SetPixelShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetPixelShaderIntUnsup(CPShader** ppShader)
{
	DPF(1, "CDevice: GetPixelShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::DeletePixelShaderIntUnsup(CPShader* pShader)
{
	DPF(1, "CDevice: DeletePixelShader() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetSamplerStateUnsup(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value)
{
	DPF(1, "CDevice: SetSamplerState() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetSamplerStateUnsup(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value)
{
	DPF(1, "CDevice: GetSamplerState() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetDialogBoxModeUnsup(BOOL bEnableDialogs)
{
	DPF(1, "CDevice::SetDialogBoxModeUnsup(BOOL bEnableDialogs) unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetSoftwareVertexProcessingUnsup(BOOL bSoftware)
{
	DPF(1, "CDevice::SetSoftwareVertexProcessing unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

BOOL CDevice::GetSoftwareVertexProcessingUnsup()
{
	DPF(1, "CDevice::SetSoftwareVertexProcessing unsupported on version %4.2f interfaces.\n", m_fInterface);
	return FALSE;
}

HRESULT CDevice::GetSwapChainUnsup(UINT iSwapChain,CSwapChain** pSwapChain)
{
	DPF(1, "CDevice::GetSwapChain unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

UINT CDevice::GetNumberOfSwapChainsUnsup()
{
	DPF(1, "CDevice::GetNumberOfSwapChains unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateRenderTarget2Unsup(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,CSurface **ppSurface, HANDLE *pSharedHandle)
{
	DPF(1, "CDevice: CreateRenderTarget(UINT, UINT, FORMAT, MULTISAMPLE_TYPE, DWORD, BOOL, CSurface **, HANDLE *) unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateDepthStencilSurface2Unsup(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality, BOOL Discard, CSurface **ppSurface, HANDLE *pSharedHandle)
{
	DPF(1, "CDevice: CreateDepthStencilSurface(UINT, UINT, FORMAT, MULTISAMPLE_TYPE, DWORD, CSurface **, HANDLE *) unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateTexture2Unsup(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture, HANDLE *pSharedHandle)
{
	DPF(1, "CDevice:CreateTexture2(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture, HANDLE *pSharedHandle)  unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateVolumeTexture2Unsup(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture **ppVolumeTexture, HANDLE *pSharedHandle)
{
	DPF(1, "CDevice:CreateVolumeTexture2(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture **ppVolumeTexture, HANDLE *pSharedHandle)  unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateCubeTexture2Unsup(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture **ppCubeTexture, HANDLE *pSharedHandle)
{
	DPF(1, "CDevice:CreateCubeTexture2(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture **ppCubeTexture, HANDLE *pSharedHandle)  unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateVertexBuffer2Unsup(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB, HANDLE *pSharedHandle)
{
	DPF(1, "CDevice:CreateVertexBuffer2(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB, HANDLE *pSharedHandle)  unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateIndexBuffer2Unsup(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB, HANDLE *pSharedHandle)
{
	DPF(1, "CDevice:CreateIndexBuffer2(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB, HANDLE *pSharedHandle)  unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateQueryUnsup(QUERYTYPE Type, CQuery** pQuery)
{
	DPF(1, "CDevice::CreateQuery unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetRenderTargetUnsup(DWORD RenderTargetIndex, CSurface *pNew)
{
	DPF(1, "CDevice::SetRenderTarget(DWORD, CSurface*, CSurface*) unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetDepthStencilSurfaceUnsup(CSurface *pNewDepth)
{
	DPF(1, "CDevice::SetDepthStencilSurface unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetRenderTargetUnsup(DWORD RenderTargetIndex, CSurface **pNew)
{
	DPF(1, "CDevice::GetRenderTarget(DWORD, CSurface**) unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::UpdateSurfaceUnsup(CSurface* pSourceSurface,RECT* pSourceRects,CSurface* pDestinationSurface,POINT* pDestPoint)
{
	DPF(1, "CDevice: CopyRects() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetStreamSourceFreqUnsup(UINT nStreamNumber, UINT nDivider)
{
	DPF(1, "CDevice::SetStreamSourceFreq() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetStreamSourceFreqUnsup(UINT nStreamNumber, UINT* pDividerOut)
{
	DPF(1, "CDevice::GetStreamSourceFreq() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::D3DXFontOnLostDeviceUnsup()
{
	DPF(1, "CDevice::D3DXFontOnLostDevice() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::D3DXFontOnResetDeviceUnsup()
{
	DPF(1, "CDevice::D3DXFontOnResetDevice() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateEffectFromFileUnsup(LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
	DPF(1, "CDevice::CreateEffectFromFile() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateEffectFromResourceUnsup(HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
	DPF(1, "CDevice::CreateEffectFromResource() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CheckDeviceStateUnsup(HWND)
{
	DPF(1, "CDevice::CheckDeviceState() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetConvolutionMonoKernelUnsup(UINT,UINT,float *, float *)
{
	DPF(1, "CDevice::SetConvolutionMonoKernel() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::ComposeRectsUnsup( LPSURFACE ,LPSURFACE, LPVERTEXBUFFER, UINT, LPVERTEXBUFFER, UINT, int, int )
{
	DPF(1, "CDevice::ComposeRects() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::PresentExUnsup( CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*, DWORD )
{
	DPF(1, "CDevice::PresentEx() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetGPUThreadPriorityUnsup( INT * )
{
	DPF(1, "CDevice::GetGPUThreadPriority() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetGPUThreadPriorityUnsup( INT )
{
	DPF(1, "CDevice::SetGPUThreadPriority() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::WaitForVBlankUnsup( UINT SwapChainIndex )
{
	DPF(1, "CDevice::WaitForVBlank() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CheckResourceResidencyUnsup(CResource**, UINT)
{
	DPF(1, "CDevice::CheckResourceResidency() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::SetMaximumFrameLatencyUnsup(UINT)
{
	DPF(1, "CDevice::SetMaximumFrameLatency() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::GetMaximumFrameLatencyUnsup(UINT*)
{
	DPF(1, "CDevice::GetMaximumFrameLatency() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateOffscreenPlainSurfaceExUnsup(UINT, UINT, FORMAT, DWORD, CSurface**, HANDLE*, DWORD)
{
	DPF(1, "CDevice::CreateOffscreenPlainSurfaceEx() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateRenderTargetExUnsup(UINT, UINT, FORMAT, MULTISAMPLE_TYPE, DWORD, BOOL, CSurface**, HANDLE*, DWORD)
{
	DPF(1, "CDevice::CreateRenderTargetEx() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::CreateDepthStencilSurfaceExUnsup(UINT, UINT, FORMAT, MULTISAMPLE_TYPE, DWORD, BOOL, CSurface**, HANDLE*, DWORD)
{
	DPF(1, "CDevice::CreateDepthStencilSurfaceEx() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}

HRESULT CDevice::ResetExUnsup( PRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX *pFullscreenDisplayMode )
{
	DPF(1, "CDevice::ResetEx() unsupported on version %4.2f interfaces.\n", m_fInterface);
	return SetLastError(E_NOTIMPL);
}
