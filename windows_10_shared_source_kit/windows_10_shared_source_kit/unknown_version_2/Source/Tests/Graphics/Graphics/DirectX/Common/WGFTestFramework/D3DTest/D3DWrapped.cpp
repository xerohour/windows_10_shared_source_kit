//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3DWrapped.cpp
//* 
//* Description: 
//*     Base D3D wrapper class definitions.
//*
//*****************************************************************************

// Ignore Prefast warnings 26060 (Range Precondition violation) and 28930 (Unused assignment of pointer variable)
#pragma warning( disable: 26060 )
#pragma warning( disable: 28930 )

#include "pch.h"
#include "D3DWrappedTypes.h"
#include "D3DWrapped.h"


#pragma prefast( disable: 1, "All functions in this module are stubs which throw exceptions." )


////////////////////////////////////////////////////////////
//
// Default wrapper class methods
//
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// CDeviceChild::GetDevice
////////////////////////////////////////////////////////////
void CDeviceChild::GetDevice(  CDevice ** ppDevice )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDeviceChild::GetPrivateData
////////////////////////////////////////////////////////////
NRESULT CDeviceChild::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDeviceChild::SetPrivateData
////////////////////////////////////////////////////////////
NRESULT CDeviceChild::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDeviceChild::SetPrivateDataInterface
////////////////////////////////////////////////////////////
NRESULT CDeviceChild::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDepthStencilState::GetDesc
////////////////////////////////////////////////////////////
void CDepthStencilState::GetDesc(  D3D_DEPTH_STENCIL_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CBlendState::GetDesc
////////////////////////////////////////////////////////////
void CBlendState::GetDesc(  D3D_BLEND_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CRasterizerState::GetDesc
////////////////////////////////////////////////////////////
void CRasterizerState::GetDesc(  D3D_RASTERIZER_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CResource::GetType
////////////////////////////////////////////////////////////
void CResource::GetType(  D3D_RESOURCE_DIMENSION * rType )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CResource::SetEvictionPriority
////////////////////////////////////////////////////////////
void CResource::SetEvictionPriority(  UINT EvictionPriority )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CResource::GetEvictionPriority
////////////////////////////////////////////////////////////
UINT CResource::GetEvictionPriority(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CBuffer::Map
////////////////////////////////////////////////////////////
NRESULT CBuffer::Map(  D3D_MAP MapType, UINT MapFlags, void ** ppData )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CBuffer::Unmap
////////////////////////////////////////////////////////////
void CBuffer::Unmap(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CBuffer::GetDesc
////////////////////////////////////////////////////////////
void CBuffer::GetDesc(  D3D_BUFFER_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CTexture1D::Map
////////////////////////////////////////////////////////////
NRESULT CTexture1D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, void ** ppData )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CTexture1D::Unmap
////////////////////////////////////////////////////////////
void CTexture1D::Unmap(  UINT Subresource )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CTexture1D::GetDesc
////////////////////////////////////////////////////////////
void CTexture1D::GetDesc(  D3D_TEXTURE1D_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CTexture2D::Map
////////////////////////////////////////////////////////////
NRESULT CTexture2D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, D3D_MAPPED_TEXTURE2D * pMappedTex2D )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CTexture2D::Unmap
////////////////////////////////////////////////////////////
void CTexture2D::Unmap(  UINT Subresource )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CTexture2D::GetDesc
////////////////////////////////////////////////////////////
void CTexture2D::GetDesc(  D3D_TEXTURE2D_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CTexture3D::Map
////////////////////////////////////////////////////////////
NRESULT CTexture3D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, D3D_MAPPED_TEXTURE3D * pMappedTex3D )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CTexture3D::Unmap
////////////////////////////////////////////////////////////
void CTexture3D::Unmap(  UINT Subresource )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CTexture3D::GetDesc
////////////////////////////////////////////////////////////
void CTexture3D::GetDesc(  D3D_TEXTURE3D_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CView::GetResource
////////////////////////////////////////////////////////////
void CView::GetResource(  CResource ** ppResource )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CShaderResourceView::GetDesc
////////////////////////////////////////////////////////////
void CShaderResourceView::GetDesc(  D3D_SHADER_RESOURCE_VIEW_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CRenderTargetView::GetDesc
////////////////////////////////////////////////////////////
void CRenderTargetView::GetDesc(  D3D_RENDER_TARGET_VIEW_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDepthStencilView::GetDesc
////////////////////////////////////////////////////////////
void CDepthStencilView::GetDesc(  D3D_DEPTH_STENCIL_VIEW_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CSamplerState::GetDesc
////////////////////////////////////////////////////////////
void CSamplerState::GetDesc(  D3D_SAMPLER_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CAsynchronous::Begin
////////////////////////////////////////////////////////////
void CAsynchronous::Begin(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CAsynchronous::End
////////////////////////////////////////////////////////////
void CAsynchronous::End(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CAsynchronous::GetData
////////////////////////////////////////////////////////////
NRESULT CAsynchronous::GetData(  void * pData, UINT DataSize, UINT GetDataFlags )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CAsynchronous::GetDataSize
////////////////////////////////////////////////////////////
UINT CAsynchronous::GetDataSize(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CQuery::GetDesc
////////////////////////////////////////////////////////////
void CQuery::GetDesc(  D3D_QUERY_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CCounter::GetDesc
////////////////////////////////////////////////////////////
void CCounter::GetDesc(  D3D_COUNTER_DESC * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::VSSetConstantBuffers
////////////////////////////////////////////////////////////
void CDevice::VSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

void CDevice::VSSetConstantBuffers1( UINT StartSlot, UINT NumBuffers, CBuffer *const* ppConstantBuffers, UINT32 const* pOffsets, UINT32 const* ppNumConstants )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::PSSetShaderResources
////////////////////////////////////////////////////////////
void CDevice::PSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::PSSetShader
////////////////////////////////////////////////////////////
void CDevice::PSSetShader(  CPixelShader * pPixelShader )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::PSSetSamplers
////////////////////////////////////////////////////////////
void CDevice::PSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::VSSetShader
////////////////////////////////////////////////////////////
void CDevice::VSSetShader(  CVertexShader * pVertexShader )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::DrawIndexed
////////////////////////////////////////////////////////////
void CDevice::DrawIndexed(  UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::Draw
////////////////////////////////////////////////////////////
void CDevice::Draw(  UINT VertexCount, UINT StartVertexLocation )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::PSSetConstantBuffers
////////////////////////////////////////////////////////////
void CDevice::PSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

void CDevice::PSSetConstantBuffers1( UINT StartSlot, UINT NumBuffers, CBuffer *const* ppConstantBuffers, UINT32 const* pOffsets, UINT32 const* ppNumConstants )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::IASetInputLayout
////////////////////////////////////////////////////////////
void CDevice::IASetInputLayout(  CInputLayout * pInputLayout )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::IASetVertexBuffers
////////////////////////////////////////////////////////////
void CDevice::IASetVertexBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppVertexBuffers, const UINT * pStrides, const UINT * pOffsets )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::IASetIndexBuffer
////////////////////////////////////////////////////////////
void CDevice::IASetIndexBuffer(  CBuffer * pIndexBuffer, DXGI_FORMAT Format, UINT Offset )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::DrawIndexedInstanced
////////////////////////////////////////////////////////////
void CDevice::DrawIndexedInstanced(  UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::DrawInstanced
////////////////////////////////////////////////////////////
void CDevice::DrawInstanced(  UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GSSetConstantBuffers
////////////////////////////////////////////////////////////
void CDevice::GSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GSSetShader
////////////////////////////////////////////////////////////
void CDevice::GSSetShader(  CGeometryShader * pShader )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::IASetPrimitiveTopology
////////////////////////////////////////////////////////////
void CDevice::IASetPrimitiveTopology(  D3D_PRIMITIVE_TOPOLOGY Topology )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::VSSetShaderResources
////////////////////////////////////////////////////////////
void CDevice::VSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::VSSetSamplers
////////////////////////////////////////////////////////////
void CDevice::VSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::SetPredication
////////////////////////////////////////////////////////////
void CDevice::SetPredication(  CPredicate * pPredicate, BOOL PredicateValue )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GSSetShaderResources
////////////////////////////////////////////////////////////
void CDevice::GSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GSSetSamplers
////////////////////////////////////////////////////////////
void CDevice::GSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::OMSetRenderTargets
////////////////////////////////////////////////////////////
void CDevice::OMSetRenderTargets(  UINT NumViews, CRenderTargetView *const * ppRenderTargetViews, CDepthStencilView * pDepthStencilView )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::OMSetBlendState
////////////////////////////////////////////////////////////
void CDevice::OMSetBlendState(  CBlendState * pBlendState, const FLOAT BlendFactor[4], UINT SampleMask )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::OMSetDepthStencilState
////////////////////////////////////////////////////////////
void CDevice::OMSetDepthStencilState(  CDepthStencilState * pDepthStencilState, UINT StencilRef )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::SOSetTargets
////////////////////////////////////////////////////////////
void CDevice::SOSetTargets(  UINT NumBuffers, CBuffer *const * ppSOTargets, const UINT * pOffsets )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::DrawAuto
////////////////////////////////////////////////////////////
void CDevice::DrawAuto(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::RSSetState
////////////////////////////////////////////////////////////
void CDevice::RSSetState(  CRasterizerState * pRasterizerState )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::RSSetViewports
////////////////////////////////////////////////////////////
void CDevice::RSSetViewports(  UINT NumViewports, const D3D_VIEWPORT * pViewports )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::RSSetScissorRects
////////////////////////////////////////////////////////////
void CDevice::RSSetScissorRects(  UINT NumRects, const D3D_RECT * pRects )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::CopySubresourceRegion
////////////////////////////////////////////////////////////
void CDevice::CopySubresourceRegion(  CResource * pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, CResource * pSrcResource, UINT SrcSubresource, const D3D_BOX * pSrcBox )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::CopyResource
////////////////////////////////////////////////////////////
void CDevice::CopyResource(  CResource * pDstResource, CResource * pSrcResource )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::UpdateSubresource
////////////////////////////////////////////////////////////
void CDevice::UpdateSubresource(  CResource * pDstResource, UINT DstSubresource, const D3D_BOX * pDstBox, const void * pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::ClearRenderTargetView
////////////////////////////////////////////////////////////
void CDevice::ClearRenderTargetView(  CRenderTargetView * pRenderTargetView, const FLOAT ColorRGBA[4] )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::ClearDepthStencilView
////////////////////////////////////////////////////////////
void CDevice::ClearDepthStencilView(  CDepthStencilView * pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GenerateMips
////////////////////////////////////////////////////////////
void CDevice::GenerateMips(  CShaderResourceView * pShaderResourceView )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::ResolveSubresource
////////////////////////////////////////////////////////////
void CDevice::ResolveSubresource(  CResource * pDstResource, UINT DstSubresource, CResource * pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::VSGetConstantBuffers
////////////////////////////////////////////////////////////
void CDevice::VSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::PSGetShaderResources
////////////////////////////////////////////////////////////
void CDevice::PSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::PSGetShader
////////////////////////////////////////////////////////////
void CDevice::PSGetShader(  CPixelShader ** ppPixelShader )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::PSGetSamplers
////////////////////////////////////////////////////////////
void CDevice::PSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::VSGetShader
////////////////////////////////////////////////////////////
void CDevice::VSGetShader(  CVertexShader ** ppVertexShader )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::PSGetConstantBuffers
////////////////////////////////////////////////////////////
void CDevice::PSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::IAGetInputLayout
////////////////////////////////////////////////////////////
void CDevice::IAGetInputLayout(  CInputLayout ** ppInputLayout )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::IAGetVertexBuffers
////////////////////////////////////////////////////////////
void CDevice::IAGetVertexBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppVertexBuffers, UINT * pStrides, UINT * pOffsets )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::IAGetIndexBuffer
////////////////////////////////////////////////////////////
void CDevice::IAGetIndexBuffer(  CBuffer ** pIndexBuffer, DXGI_FORMAT * Format, UINT * Offset )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GSGetConstantBuffers
////////////////////////////////////////////////////////////
void CDevice::GSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GSGetShader
////////////////////////////////////////////////////////////
void CDevice::GSGetShader(  CGeometryShader ** ppGeometryShader )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::IAGetPrimitiveTopology
////////////////////////////////////////////////////////////
void CDevice::IAGetPrimitiveTopology(  D3D_PRIMITIVE_TOPOLOGY * pTopology )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::VSGetShaderResources
////////////////////////////////////////////////////////////
void CDevice::VSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::VSGetSamplers
////////////////////////////////////////////////////////////
void CDevice::VSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GetPredication
////////////////////////////////////////////////////////////
void CDevice::GetPredication(  CPredicate ** ppPredicate, BOOL * pPredicateValue )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GSGetShaderResources
////////////////////////////////////////////////////////////
void CDevice::GSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GSGetSamplers
////////////////////////////////////////////////////////////
void CDevice::GSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::OMGetRenderTargets
////////////////////////////////////////////////////////////
void CDevice::OMGetRenderTargets(  UINT NumViews, CRenderTargetView ** ppRenderTargetViews, CDepthStencilView ** ppDepthStencilView )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::OMGetBlendState
////////////////////////////////////////////////////////////
void CDevice::OMGetBlendState(  CBlendState ** ppBlendState, FLOAT BlendFactor[4], UINT * pSampleMask )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::OMGetDepthStencilState
////////////////////////////////////////////////////////////
void CDevice::OMGetDepthStencilState(  CDepthStencilState ** ppDepthStencilState, UINT * pStencilRef )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::SOGetTargets
////////////////////////////////////////////////////////////
void CDevice::SOGetTargets(  UINT NumBuffers, CBuffer ** ppSOTargets, UINT * pOffsets )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::RSGetState
////////////////////////////////////////////////////////////
void CDevice::RSGetState(  CRasterizerState ** ppRasterizerState )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::RSGetViewports
////////////////////////////////////////////////////////////
void CDevice::RSGetViewports(  UINT * NumViewports, D3D_VIEWPORT * pViewports )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::RSGetScissorRects
////////////////////////////////////////////////////////////
void CDevice::RSGetScissorRects(  UINT * NumRects, D3D_RECT * pRects )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GetDeviceRemovedReason
////////////////////////////////////////////////////////////
NRESULT CDevice::GetDeviceRemovedReason(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::SetExceptionMode
////////////////////////////////////////////////////////////
NRESULT CDevice::SetExceptionMode(  UINT RaiseFlags )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::GetExceptionMode
////////////////////////////////////////////////////////////
UINT CDevice::GetExceptionMode(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::GetPrivateData
////////////////////////////////////////////////////////////
NRESULT CDevice::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::SetPrivateData
////////////////////////////////////////////////////////////
NRESULT CDevice::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::SetPrivateDataInterface
////////////////////////////////////////////////////////////
NRESULT CDevice::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::ClearState
////////////////////////////////////////////////////////////
void CDevice::ClearState(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::Flush
////////////////////////////////////////////////////////////
void CDevice::Flush(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::CreateBuffer
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateBuffer(  const D3D_BUFFER_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CBuffer ** ppBuffer )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateTexture1D
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateTexture1D(  const D3D_TEXTURE1D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture1D ** ppTexture1D )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateTexture2D
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateTexture2D(  const D3D_TEXTURE2D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture2D ** ppTexture2D )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateTexture3D
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateTexture3D(  const D3D_TEXTURE3D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture3D ** ppTexture3D )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateShaderResourceView
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateShaderResourceView(  CResource * pResource, const D3D_SHADER_RESOURCE_VIEW_DESC * pDesc, CShaderResourceView ** ppSRView )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateRenderTargetView
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateRenderTargetView(  CResource * pResource, const D3D_RENDER_TARGET_VIEW_DESC * pDesc, CRenderTargetView ** ppRTView )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateDepthStencilView
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateDepthStencilView(  CResource * pResource, const D3D_DEPTH_STENCIL_VIEW_DESC * pDesc, CDepthStencilView ** ppDepthStencilView )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateInputLayout
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateInputLayout(  const D3D_INPUT_ELEMENT_DESC * pInputElementDescs, UINT NumElements, const void * pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, CInputLayout ** ppInputLayout )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateVertexShader
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateVertexShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CVertexShader ** ppVertexShader )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateGeometryShader
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateGeometryShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CGeometryShader ** ppGeometryShader )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateGeometryShaderWithStreamOutput
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateGeometryShaderWithStreamOutput(  const void * pShaderBytecode, SIZE_T BytecodeLength, const D3D_SO_DECLARATION_ENTRY * pSODeclaration, UINT NumEntries, UINT OutputStreamStride, CGeometryShader ** ppGeometryShader )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreatePixelShader
////////////////////////////////////////////////////////////
NRESULT CDevice::CreatePixelShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CPixelShader ** ppPixelShader )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateBlendState
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateBlendState(  const D3D_BLEND_DESC * pBlendStateDesc, CBlendState ** ppBlendState )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateDepthStencilState
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateDepthStencilState(  const D3D_DEPTH_STENCIL_DESC * pDepthStencilDesc, CDepthStencilState ** ppDepthStencilState )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateRasterizerState
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateRasterizerState(  const D3D_RASTERIZER_DESC * pRasterizerDesc, CRasterizerState ** ppRasterizerState )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateSamplerState
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateSamplerState(  const D3D_SAMPLER_DESC * pSamplerDesc, CSamplerState ** ppSamplerState )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateQuery
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateQuery(  const D3D_QUERY_DESC * pQueryDesc, CQuery ** ppQuery )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreatePredicate
////////////////////////////////////////////////////////////
NRESULT CDevice::CreatePredicate(  const D3D_QUERY_DESC * pPredicateDesc, CPredicate ** ppPredicate )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateCounter
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateCounter(  const D3D_COUNTER_DESC * pCounterDesc, CCounter ** ppCounter )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CheckFormatSupport
////////////////////////////////////////////////////////////
NRESULT CDevice::CheckFormatSupport(  DXGI_FORMAT Format, UINT * pFormatSupport )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CheckMultisampleQualityLevels
////////////////////////////////////////////////////////////
NRESULT CDevice::CheckMultisampleQualityLevels(  DXGI_FORMAT Format, UINT SampleCount, UINT * pNumQualityLevels )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CheckCounterInfo
////////////////////////////////////////////////////////////
void CDevice::CheckCounterInfo(  D3D_COUNTER_INFO * pCounterInfo )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::CheckCounter
////////////////////////////////////////////////////////////
NRESULT CDevice::CheckCounter(  const D3D_COUNTER_DESC * pDesc, D3D_COUNTER_TYPE * pType, UINT * pActiveCounters, LPSTR szName, UINT * pNameLength, LPSTR szUnits, UINT * pUnitsLength, LPSTR szDescription, UINT * pDescriptionLength )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::GetCreationFlags
////////////////////////////////////////////////////////////
UINT CDevice::GetCreationFlags(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::OpenSharedResource
////////////////////////////////////////////////////////////
NRESULT CDevice::OpenSharedResource(  HANDLE hResource, REFIID ReturnedInterface, void ** ppResource )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::SetTextFilterSize
////////////////////////////////////////////////////////////
void CDevice::SetTextFilterSize(  UINT Width, UINT Height )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::GetTextFilterSize
////////////////////////////////////////////////////////////
void CDevice::GetTextFilterSize(  UINT * pWidth, UINT * pHeight )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CDevice::CreateShaderResourceView1
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateShaderResourceView1(  CResource * pResource, const D3D_SHADER_RESOURCE_VIEW_DESC1 * pDesc, CShaderResourceView1 ** ppSRView )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::CreateBlendState1
////////////////////////////////////////////////////////////
NRESULT CDevice::CreateBlendState1(  const D3D_BLEND_DESC1 * pBlendStateDesc, CBlendState1 ** ppBlendState )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDevice::GetFeatureLevel
////////////////////////////////////////////////////////////
D3D_FEATURE_LEVEL1 CDevice::GetFeatureLevel(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    D3D_FEATURE_LEVEL1 rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CMultithread::Enter
////////////////////////////////////////////////////////////
void CMultithread::Enter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CMultithread::Leave
////////////////////////////////////////////////////////////
void CMultithread::Leave(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CMultithread::SetMultithreadProtected
////////////////////////////////////////////////////////////
BOOL CMultithread::SetMultithreadProtected(  BOOL bMTProtect )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    BOOL rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CMultithread::GetMultithreadProtected
////////////////////////////////////////////////////////////
BOOL CMultithread::GetMultithreadProtected(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    BOOL rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDebug::SetFeatureMask
////////////////////////////////////////////////////////////
NRESULT CDebug::SetFeatureMask(  UINT Mask )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDebug::GetFeatureMask
////////////////////////////////////////////////////////////
UINT CDebug::GetFeatureMask(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDebug::SetPresentPerRenderOpDelay
////////////////////////////////////////////////////////////
NRESULT CDebug::SetPresentPerRenderOpDelay(  UINT Milliseconds )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDebug::GetPresentPerRenderOpDelay
////////////////////////////////////////////////////////////
UINT CDebug::GetPresentPerRenderOpDelay(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDebug::SetSwapChain
////////////////////////////////////////////////////////////
NRESULT CDebug::SetSwapChain(  IDXGISwapChain * pSwapChain )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDebug::GetSwapChain
////////////////////////////////////////////////////////////
NRESULT CDebug::GetSwapChain(  IDXGISwapChain ** ppSwapChain )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CDebug::Validate
////////////////////////////////////////////////////////////
NRESULT CDebug::Validate(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CSwitchToRef::SetUseRef
////////////////////////////////////////////////////////////
BOOL CSwitchToRef::SetUseRef(  BOOL UseRef )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    BOOL rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CSwitchToRef::GetUseRef
////////////////////////////////////////////////////////////
BOOL CSwitchToRef::GetUseRef(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    BOOL rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::SetMessageCountLimit
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::SetMessageCountLimit(  UINT64 MessageCountLimit )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::ClearStoredMessages
////////////////////////////////////////////////////////////
void CInfoQueue::ClearStoredMessages(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetMessage
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::GetMessage(  UINT64 MessageIndex, D3D_MESSAGE * pMessage, SIZE_T * pMessageByteLength )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetNumMessagesAllowedByStorageFilter
////////////////////////////////////////////////////////////
UINT64 CInfoQueue::GetNumMessagesAllowedByStorageFilter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT64 rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetNumMessagesDeniedByStorageFilter
////////////////////////////////////////////////////////////
UINT64 CInfoQueue::GetNumMessagesDeniedByStorageFilter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT64 rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetNumStoredMessages
////////////////////////////////////////////////////////////
UINT64 CInfoQueue::GetNumStoredMessages(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT64 rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetNumStoredMessagesAllowedByRetrievalFilter
////////////////////////////////////////////////////////////
UINT64 CInfoQueue::GetNumStoredMessagesAllowedByRetrievalFilter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT64 rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetNumMessagesDiscardedByMessageCountLimit
////////////////////////////////////////////////////////////
UINT64 CInfoQueue::GetNumMessagesDiscardedByMessageCountLimit(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT64 rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetMessageCountLimit
////////////////////////////////////////////////////////////
UINT64 CInfoQueue::GetMessageCountLimit(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT64 rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::AddStorageFilterEntries
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::AddStorageFilterEntries(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetStorageFilter
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::GetStorageFilter(  D3D_INFO_QUEUE_FILTER * pFilter, SIZE_T * pFilterByteLength )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::ClearStorageFilter
////////////////////////////////////////////////////////////
void CInfoQueue::ClearStorageFilter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CInfoQueue::PushEmptyStorageFilter
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::PushEmptyStorageFilter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::PushCopyOfStorageFilter
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::PushCopyOfStorageFilter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::PushStorageFilter
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::PushStorageFilter(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::PopStorageFilter
////////////////////////////////////////////////////////////
void CInfoQueue::PopStorageFilter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetStorageFilterStackSize
////////////////////////////////////////////////////////////
UINT CInfoQueue::GetStorageFilterStackSize(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::AddRetrievalFilterEntries
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::AddRetrievalFilterEntries(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetRetrievalFilter
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::GetRetrievalFilter(  D3D_INFO_QUEUE_FILTER * pFilter, SIZE_T * pFilterByteLength )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::ClearRetrievalFilter
////////////////////////////////////////////////////////////
void CInfoQueue::ClearRetrievalFilter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CInfoQueue::PushEmptyRetrievalFilter
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::PushEmptyRetrievalFilter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::PushCopyOfRetrievalFilter
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::PushCopyOfRetrievalFilter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::PushRetrievalFilter
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::PushRetrievalFilter(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::PopRetrievalFilter
////////////////////////////////////////////////////////////
void CInfoQueue::PopRetrievalFilter(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetRetrievalFilterStackSize
////////////////////////////////////////////////////////////
UINT CInfoQueue::GetRetrievalFilterStackSize(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    UINT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::AddMessage
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::AddMessage(  D3D_MESSAGE_CATEGORY Category, D3D_MESSAGE_SEVERITY Severity, D3D_MESSAGE_ID ID, LPCSTR pDescription )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::AddApplicationMessage
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::AddApplicationMessage(  D3D_MESSAGE_SEVERITY Severity, LPCSTR pDescription )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::SetBreakOnCategory
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::SetBreakOnCategory(  D3D_MESSAGE_CATEGORY Category, BOOL bEnable )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::SetBreakOnSeverity
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::SetBreakOnSeverity(  D3D_MESSAGE_SEVERITY Severity, BOOL bEnable )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::SetBreakOnID
////////////////////////////////////////////////////////////
NRESULT CInfoQueue::SetBreakOnID(  D3D_MESSAGE_ID ID, BOOL bEnable )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetBreakOnCategory
////////////////////////////////////////////////////////////
BOOL CInfoQueue::GetBreakOnCategory(  D3D_MESSAGE_CATEGORY Category )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    BOOL rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetBreakOnSeverity
////////////////////////////////////////////////////////////
BOOL CInfoQueue::GetBreakOnSeverity(  D3D_MESSAGE_SEVERITY Severity )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    BOOL rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetBreakOnID
////////////////////////////////////////////////////////////
BOOL CInfoQueue::GetBreakOnID(  D3D_MESSAGE_ID ID )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    BOOL rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CInfoQueue::SetMuteDebugOutput
////////////////////////////////////////////////////////////
void CInfoQueue::SetMuteDebugOutput(  BOOL bMute )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CInfoQueue::GetMuteDebugOutput
////////////////////////////////////////////////////////////
BOOL CInfoQueue::GetMuteDebugOutput(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    BOOL rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CBlob::QueryInterface
////////////////////////////////////////////////////////////
NRESULT CBlob::QueryInterface(  REFIID iid, LPVOID * ppv )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    NRESULT rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CBlob::GetBufferPointer
////////////////////////////////////////////////////////////
LPVOID CBlob::GetBufferPointer(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    LPVOID rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CBlob::GetBufferSize
////////////////////////////////////////////////////////////
SIZE_T CBlob::GetBufferSize(  )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
    SIZE_T rval;
    return rval;
}

////////////////////////////////////////////////////////////
// CBlendState1::GetDesc1
////////////////////////////////////////////////////////////
void CBlendState1::GetDesc1(  D3D_BLEND_DESC1 * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}

////////////////////////////////////////////////////////////
// CShaderResourceView1::GetDesc1
////////////////////////////////////////////////////////////
void CShaderResourceView1::GetDesc1(  D3D_SHADER_RESOURCE_VIEW_DESC1 * pDesc )
{
    // By default this method is unsupported
    throw( WRAPPER_METHOD_UNSUPPORTED );
}
