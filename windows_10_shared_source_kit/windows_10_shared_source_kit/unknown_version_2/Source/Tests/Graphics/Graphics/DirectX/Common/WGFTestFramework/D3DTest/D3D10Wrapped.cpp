//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D10Wrapped.cpp
//* 
//* Description: 
//*     Interface specific D3D wrapper classes.
//*
//*****************************************************************************

// Ignore Prefast warnings 26060 (Range Precondition violation) and 28930 (Unused assignment of pointer variable)
#pragma warning( disable: 26060 )
#pragma warning( disable: 28930 )

#include "pch.h"
#include "D3DWrappedTypes.h"
#include "D3D10Wrapped.h"
#include "d3d10_1.h"
#include "D3D10Convert.h"


#pragma prefast( disable: 387, "These are wrapper functions which will only pass 0 when given 0 for the wrapped parameter." )




////////////////////////////////////////////////////////////
//
// Wrapper classes
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C10DeviceChild
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C10DepthStencilState
//
////////////////////////////////////////////////////////////

C10DepthStencilState::C10DepthStencilState( ID3D10DepthStencilState *pDepthStencilState ) : 
    m_pDepthStencilState( pDepthStencilState )
{
}


C10DepthStencilState::~C10DepthStencilState()
{
    if( m_pDepthStencilState ) m_pDepthStencilState->Release();
}


////////////////////////////////////////////////////////////
// C10DepthStencilState::GetDesc()
////////////////////////////////////////////////////////////
void C10DepthStencilState::GetDesc(  D3D_DEPTH_STENCIL_DESC * pDesc )
{
    assert( m_pDepthStencilState );
    BYTE *pDescByteTemp = NULL;
    D3D10_DEPTH_STENCIL_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_DEPTH_STENCIL_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_DEPTH_STENCIL_DESC*)pDescByteTemp;
    }
    m_pDepthStencilState->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10DepthStencilState::GetDevice()
////////////////////////////////////////////////////////////
void C10DepthStencilState::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pDepthStencilState );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pDepthStencilState->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10DepthStencilState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10DepthStencilState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pDepthStencilState );
    NRESULT rval = S_OK;
    rval = m_pDepthStencilState->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10DepthStencilState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10DepthStencilState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pDepthStencilState );
    NRESULT rval = S_OK;
    rval = m_pDepthStencilState->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10DepthStencilState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10DepthStencilState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pDepthStencilState );
    NRESULT rval = S_OK;
    rval = m_pDepthStencilState->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10BlendState
//
////////////////////////////////////////////////////////////

C10BlendState::C10BlendState( ID3D10BlendState *pBlendState ) : 
    m_pBlendState( pBlendState )
{
}


C10BlendState::~C10BlendState()
{
    if( m_pBlendState ) m_pBlendState->Release();
}


////////////////////////////////////////////////////////////
// C10BlendState::GetDesc()
////////////////////////////////////////////////////////////
void C10BlendState::GetDesc(  D3D_BLEND_DESC * pDesc )
{
    assert( m_pBlendState );
    BYTE *pDescByteTemp = NULL;
    D3D10_BLEND_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_BLEND_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_BLEND_DESC*)pDescByteTemp;
    }
    m_pBlendState->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10BlendState::GetDevice()
////////////////////////////////////////////////////////////
void C10BlendState::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pBlendState );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pBlendState->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10BlendState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10BlendState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pBlendState );
    NRESULT rval = S_OK;
    rval = m_pBlendState->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10BlendState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10BlendState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pBlendState );
    NRESULT rval = S_OK;
    rval = m_pBlendState->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10BlendState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10BlendState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pBlendState );
    NRESULT rval = S_OK;
    rval = m_pBlendState->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10RasterizerState
//
////////////////////////////////////////////////////////////

C10RasterizerState::C10RasterizerState( ID3D10RasterizerState *pRasterizerState ) : 
    m_pRasterizerState( pRasterizerState )
{
}


C10RasterizerState::~C10RasterizerState()
{
    if( m_pRasterizerState ) m_pRasterizerState->Release();
}


////////////////////////////////////////////////////////////
// C10RasterizerState::GetDesc()
////////////////////////////////////////////////////////////
void C10RasterizerState::GetDesc(  D3D_RASTERIZER_DESC * pDesc )
{
    assert( m_pRasterizerState );
    BYTE *pDescByteTemp = NULL;
    D3D10_RASTERIZER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_RASTERIZER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_RASTERIZER_DESC*)pDescByteTemp;
    }
    m_pRasterizerState->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10RasterizerState::GetDevice()
////////////////////////////////////////////////////////////
void C10RasterizerState::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pRasterizerState );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pRasterizerState->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10RasterizerState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10RasterizerState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pRasterizerState );
    NRESULT rval = S_OK;
    rval = m_pRasterizerState->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10RasterizerState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10RasterizerState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pRasterizerState );
    NRESULT rval = S_OK;
    rval = m_pRasterizerState->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10RasterizerState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10RasterizerState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pRasterizerState );
    NRESULT rval = S_OK;
    rval = m_pRasterizerState->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10Resource
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C10Buffer
//
////////////////////////////////////////////////////////////

C10Buffer::C10Buffer( ID3D10Buffer *pBuffer ) : 
    m_pBuffer( pBuffer )
{
}


C10Buffer::~C10Buffer()
{
    if( m_pBuffer ) m_pBuffer->Release();
}


////////////////////////////////////////////////////////////
// C10Buffer::Map()
////////////////////////////////////////////////////////////
NRESULT C10Buffer::Map(  D3D_MAP MapType, UINT MapFlags, void ** ppData )
{
    assert( m_pBuffer );
    NRESULT rval = S_OK;
    D3D10_MAP temp_d3d10_map;
    D3DWrapperConvertEnum( &temp_d3d10_map, MapType );
    UINT temp_MapFlags;
    D3DFlagsConvertTo10_MAP_FLAG( &temp_MapFlags, MapFlags );
    rval = m_pBuffer->Map(  temp_d3d10_map,  temp_MapFlags,  ppData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Buffer::Unmap()
////////////////////////////////////////////////////////////
void C10Buffer::Unmap(  )
{
    assert( m_pBuffer );
    m_pBuffer->Unmap(  );
}


////////////////////////////////////////////////////////////
// C10Buffer::GetDesc()
////////////////////////////////////////////////////////////
void C10Buffer::GetDesc(  D3D_BUFFER_DESC * pDesc )
{
    assert( m_pBuffer );
    BYTE *pDescByteTemp = NULL;
    D3D10_BUFFER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_BUFFER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_BUFFER_DESC*)pDescByteTemp;
    }
    m_pBuffer->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10Buffer::GetType()
////////////////////////////////////////////////////////////
void C10Buffer::GetType(  D3D_RESOURCE_DIMENSION * rType )
{
    assert( m_pBuffer );
    D3D10_RESOURCE_DIMENSION temp_d3d10_resource_dimension;
    m_pBuffer->GetType(  &temp_d3d10_resource_dimension );
    D3DWrapperConvertEnum( rType, temp_d3d10_resource_dimension );
}


////////////////////////////////////////////////////////////
// C10Buffer::SetEvictionPriority()
////////////////////////////////////////////////////////////
void C10Buffer::SetEvictionPriority(  UINT EvictionPriority )
{
    assert( m_pBuffer );
    m_pBuffer->SetEvictionPriority(  EvictionPriority );
}


////////////////////////////////////////////////////////////
// C10Buffer::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT C10Buffer::GetEvictionPriority(  )
{
    assert( m_pBuffer );
    UINT rval = 0;
    rval = m_pBuffer->GetEvictionPriority(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Buffer::GetDevice()
////////////////////////////////////////////////////////////
void C10Buffer::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pBuffer );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pBuffer->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Buffer::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Buffer::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pBuffer );
    NRESULT rval = S_OK;
    rval = m_pBuffer->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Buffer::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Buffer::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pBuffer );
    NRESULT rval = S_OK;
    rval = m_pBuffer->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Buffer::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10Buffer::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pBuffer );
    NRESULT rval = S_OK;
    rval = m_pBuffer->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10Texture1D
//
////////////////////////////////////////////////////////////

C10Texture1D::C10Texture1D( ID3D10Texture1D *pTexture1D ) : 
    m_pTexture1D( pTexture1D )
{
}


C10Texture1D::~C10Texture1D()
{
    if( m_pTexture1D ) m_pTexture1D->Release();
}


////////////////////////////////////////////////////////////
// C10Texture1D::Map()
////////////////////////////////////////////////////////////
NRESULT C10Texture1D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, void ** ppData )
{
    assert( m_pTexture1D );
    NRESULT rval = S_OK;
    D3D10_MAP temp_d3d10_map;
    D3DWrapperConvertEnum( &temp_d3d10_map, MapType );
    UINT temp_MapFlags;
    D3DFlagsConvertTo10_MAP_FLAG( &temp_MapFlags, MapFlags );
    rval = m_pTexture1D->Map(  Subresource,  temp_d3d10_map,  temp_MapFlags,  ppData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture1D::Unmap()
////////////////////////////////////////////////////////////
void C10Texture1D::Unmap(  UINT Subresource )
{
    assert( m_pTexture1D );
    m_pTexture1D->Unmap(  Subresource );
}


////////////////////////////////////////////////////////////
// C10Texture1D::GetDesc()
////////////////////////////////////////////////////////////
void C10Texture1D::GetDesc(  D3D_TEXTURE1D_DESC * pDesc )
{
    assert( m_pTexture1D );
    BYTE *pDescByteTemp = NULL;
    D3D10_TEXTURE1D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_TEXTURE1D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_TEXTURE1D_DESC*)pDescByteTemp;
    }
    m_pTexture1D->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10Texture1D::GetType()
////////////////////////////////////////////////////////////
void C10Texture1D::GetType(  D3D_RESOURCE_DIMENSION * rType )
{
    assert( m_pTexture1D );
    D3D10_RESOURCE_DIMENSION temp_d3d10_resource_dimension;
    m_pTexture1D->GetType(  &temp_d3d10_resource_dimension );
    D3DWrapperConvertEnum( rType, temp_d3d10_resource_dimension );
}


////////////////////////////////////////////////////////////
// C10Texture1D::SetEvictionPriority()
////////////////////////////////////////////////////////////
void C10Texture1D::SetEvictionPriority(  UINT EvictionPriority )
{
    assert( m_pTexture1D );
    m_pTexture1D->SetEvictionPriority(  EvictionPriority );
}


////////////////////////////////////////////////////////////
// C10Texture1D::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT C10Texture1D::GetEvictionPriority(  )
{
    assert( m_pTexture1D );
    UINT rval = 0;
    rval = m_pTexture1D->GetEvictionPriority(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture1D::GetDevice()
////////////////////////////////////////////////////////////
void C10Texture1D::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pTexture1D );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pTexture1D->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Texture1D::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Texture1D::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pTexture1D );
    NRESULT rval = S_OK;
    rval = m_pTexture1D->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture1D::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Texture1D::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pTexture1D );
    NRESULT rval = S_OK;
    rval = m_pTexture1D->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture1D::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10Texture1D::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pTexture1D );
    NRESULT rval = S_OK;
    rval = m_pTexture1D->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10Texture2D
//
////////////////////////////////////////////////////////////

C10Texture2D::C10Texture2D( ID3D10Texture2D *pTexture2D ) : 
    m_pTexture2D( pTexture2D )
{
}


C10Texture2D::~C10Texture2D()
{
    if( m_pTexture2D ) m_pTexture2D->Release();
}


////////////////////////////////////////////////////////////
// C10Texture2D::Map()
////////////////////////////////////////////////////////////
NRESULT C10Texture2D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, D3D_MAPPED_TEXTURE2D * pMappedTex2D )
{
    assert( m_pTexture2D );
    NRESULT rval = S_OK;
    D3D10_MAP temp_d3d10_map;
    D3DWrapperConvertEnum( &temp_d3d10_map, MapType );
    UINT temp_MapFlags;
    D3DFlagsConvertTo10_MAP_FLAG( &temp_MapFlags, MapFlags );
    BYTE *pMappedTex2DByteTemp = NULL;
    D3D10_MAPPED_TEXTURE2D *pMappedTex2DTemp = NULL;
    SIZE_T pMappedTex2DSizeBuffer = sizeof(D3D10_MAPPED_TEXTURE2D);
    if( pMappedTex2D != NULL && pMappedTex2DSizeBuffer != 0 )
    {
        pMappedTex2DByteTemp = new BYTE[pMappedTex2DSizeBuffer];
        pMappedTex2DTemp = (D3D10_MAPPED_TEXTURE2D*)pMappedTex2DByteTemp;
    }
    rval = m_pTexture2D->Map(  Subresource,  temp_d3d10_map,  temp_MapFlags,  pMappedTex2DTemp );
    if( pMappedTex2D != NULL && pMappedTex2DSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pMappedTex2D, pMappedTex2DTemp );
        delete[] pMappedTex2DByteTemp;
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture2D::Unmap()
////////////////////////////////////////////////////////////
void C10Texture2D::Unmap(  UINT Subresource )
{
    assert( m_pTexture2D );
    m_pTexture2D->Unmap(  Subresource );
}


////////////////////////////////////////////////////////////
// C10Texture2D::GetDesc()
////////////////////////////////////////////////////////////
void C10Texture2D::GetDesc(  D3D_TEXTURE2D_DESC * pDesc )
{
    assert( m_pTexture2D );
    BYTE *pDescByteTemp = NULL;
    D3D10_TEXTURE2D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_TEXTURE2D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_TEXTURE2D_DESC*)pDescByteTemp;
    }
    m_pTexture2D->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10Texture2D::GetType()
////////////////////////////////////////////////////////////
void C10Texture2D::GetType(  D3D_RESOURCE_DIMENSION * rType )
{
    assert( m_pTexture2D );
    D3D10_RESOURCE_DIMENSION temp_d3d10_resource_dimension;
    m_pTexture2D->GetType(  &temp_d3d10_resource_dimension );
    D3DWrapperConvertEnum( rType, temp_d3d10_resource_dimension );
}


////////////////////////////////////////////////////////////
// C10Texture2D::SetEvictionPriority()
////////////////////////////////////////////////////////////
void C10Texture2D::SetEvictionPriority(  UINT EvictionPriority )
{
    assert( m_pTexture2D );
    m_pTexture2D->SetEvictionPriority(  EvictionPriority );
}


////////////////////////////////////////////////////////////
// C10Texture2D::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT C10Texture2D::GetEvictionPriority(  )
{
    assert( m_pTexture2D );
    UINT rval = 0;
    rval = m_pTexture2D->GetEvictionPriority(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture2D::GetDevice()
////////////////////////////////////////////////////////////
void C10Texture2D::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pTexture2D );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pTexture2D->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Texture2D::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Texture2D::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pTexture2D );
    NRESULT rval = S_OK;
    rval = m_pTexture2D->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture2D::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Texture2D::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pTexture2D );
    NRESULT rval = S_OK;
    rval = m_pTexture2D->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture2D::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10Texture2D::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pTexture2D );
    NRESULT rval = S_OK;
    rval = m_pTexture2D->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10Texture3D
//
////////////////////////////////////////////////////////////

C10Texture3D::C10Texture3D( ID3D10Texture3D *pTexture3D ) : 
    m_pTexture3D( pTexture3D )
{
}


C10Texture3D::~C10Texture3D()
{
    if( m_pTexture3D ) m_pTexture3D->Release();
}


////////////////////////////////////////////////////////////
// C10Texture3D::Map()
////////////////////////////////////////////////////////////
NRESULT C10Texture3D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, D3D_MAPPED_TEXTURE3D * pMappedTex3D )
{
    assert( m_pTexture3D );
    NRESULT rval = S_OK;
    D3D10_MAP temp_d3d10_map;
    D3DWrapperConvertEnum( &temp_d3d10_map, MapType );
    UINT temp_MapFlags;
    D3DFlagsConvertTo10_MAP_FLAG( &temp_MapFlags, MapFlags );
    BYTE *pMappedTex3DByteTemp = NULL;
    D3D10_MAPPED_TEXTURE3D *pMappedTex3DTemp = NULL;
    SIZE_T pMappedTex3DSizeBuffer = sizeof(D3D10_MAPPED_TEXTURE3D);
    if( pMappedTex3D != NULL && pMappedTex3DSizeBuffer != 0 )
    {
        pMappedTex3DByteTemp = new BYTE[pMappedTex3DSizeBuffer];
        pMappedTex3DTemp = (D3D10_MAPPED_TEXTURE3D*)pMappedTex3DByteTemp;
    }
    rval = m_pTexture3D->Map(  Subresource,  temp_d3d10_map,  temp_MapFlags,  pMappedTex3DTemp );
    if( pMappedTex3D != NULL && pMappedTex3DSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pMappedTex3D, pMappedTex3DTemp );
        delete[] pMappedTex3DByteTemp;
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture3D::Unmap()
////////////////////////////////////////////////////////////
void C10Texture3D::Unmap(  UINT Subresource )
{
    assert( m_pTexture3D );
    m_pTexture3D->Unmap(  Subresource );
}


////////////////////////////////////////////////////////////
// C10Texture3D::GetDesc()
////////////////////////////////////////////////////////////
void C10Texture3D::GetDesc(  D3D_TEXTURE3D_DESC * pDesc )
{
    assert( m_pTexture3D );
    BYTE *pDescByteTemp = NULL;
    D3D10_TEXTURE3D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_TEXTURE3D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_TEXTURE3D_DESC*)pDescByteTemp;
    }
    m_pTexture3D->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10Texture3D::GetType()
////////////////////////////////////////////////////////////
void C10Texture3D::GetType(  D3D_RESOURCE_DIMENSION * rType )
{
    assert( m_pTexture3D );
    D3D10_RESOURCE_DIMENSION temp_d3d10_resource_dimension;
    m_pTexture3D->GetType(  &temp_d3d10_resource_dimension );
    D3DWrapperConvertEnum( rType, temp_d3d10_resource_dimension );
}


////////////////////////////////////////////////////////////
// C10Texture3D::SetEvictionPriority()
////////////////////////////////////////////////////////////
void C10Texture3D::SetEvictionPriority(  UINT EvictionPriority )
{
    assert( m_pTexture3D );
    m_pTexture3D->SetEvictionPriority(  EvictionPriority );
}


////////////////////////////////////////////////////////////
// C10Texture3D::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT C10Texture3D::GetEvictionPriority(  )
{
    assert( m_pTexture3D );
    UINT rval = 0;
    rval = m_pTexture3D->GetEvictionPriority(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture3D::GetDevice()
////////////////////////////////////////////////////////////
void C10Texture3D::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pTexture3D );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pTexture3D->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Texture3D::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Texture3D::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pTexture3D );
    NRESULT rval = S_OK;
    rval = m_pTexture3D->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture3D::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Texture3D::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pTexture3D );
    NRESULT rval = S_OK;
    rval = m_pTexture3D->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Texture3D::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10Texture3D::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pTexture3D );
    NRESULT rval = S_OK;
    rval = m_pTexture3D->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10View
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C10ShaderResourceView
//
////////////////////////////////////////////////////////////

C10ShaderResourceView::C10ShaderResourceView( ID3D10ShaderResourceView *pShaderResourceView ) : 
    m_pShaderResourceView( pShaderResourceView )
{
}


C10ShaderResourceView::~C10ShaderResourceView()
{
    if( m_pShaderResourceView ) m_pShaderResourceView->Release();
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView::GetDesc()
////////////////////////////////////////////////////////////
void C10ShaderResourceView::GetDesc(  D3D_SHADER_RESOURCE_VIEW_DESC * pDesc )
{
    assert( m_pShaderResourceView );
    BYTE *pDescByteTemp = NULL;
    D3D10_SHADER_RESOURCE_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_SHADER_RESOURCE_VIEW_DESC*)pDescByteTemp;
    }
    m_pShaderResourceView->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView::GetResource()
////////////////////////////////////////////////////////////
void C10ShaderResourceView::GetResource(  CResource ** ppResource )
{
    assert( m_pShaderResourceView );
    ID3D10Resource *temp_pResource = NULL;
    ID3D10Resource **ppResourceTemp = ppResource == NULL ? NULL : &temp_pResource;
    m_pShaderResourceView->GetResource(  ppResourceTemp );
    if( ppResource != NULL )
    {
        D3DWrapperConvertInterface( ppResource, *ppResourceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView::GetDevice()
////////////////////////////////////////////////////////////
void C10ShaderResourceView::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pShaderResourceView );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pShaderResourceView->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10ShaderResourceView::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pShaderResourceView );
    NRESULT rval = S_OK;
    rval = m_pShaderResourceView->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10ShaderResourceView::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pShaderResourceView );
    NRESULT rval = S_OK;
    rval = m_pShaderResourceView->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10ShaderResourceView::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pShaderResourceView );
    NRESULT rval = S_OK;
    rval = m_pShaderResourceView->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10RenderTargetView
//
////////////////////////////////////////////////////////////

C10RenderTargetView::C10RenderTargetView( ID3D10RenderTargetView *pRenderTargetView ) : 
    m_pRenderTargetView( pRenderTargetView )
{
}


C10RenderTargetView::~C10RenderTargetView()
{
    if( m_pRenderTargetView ) m_pRenderTargetView->Release();
}


////////////////////////////////////////////////////////////
// C10RenderTargetView::GetDesc()
////////////////////////////////////////////////////////////
void C10RenderTargetView::GetDesc(  D3D_RENDER_TARGET_VIEW_DESC * pDesc )
{
    assert( m_pRenderTargetView );
    BYTE *pDescByteTemp = NULL;
    D3D10_RENDER_TARGET_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_RENDER_TARGET_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_RENDER_TARGET_VIEW_DESC*)pDescByteTemp;
    }
    m_pRenderTargetView->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10RenderTargetView::GetResource()
////////////////////////////////////////////////////////////
void C10RenderTargetView::GetResource(  CResource ** ppResource )
{
    assert( m_pRenderTargetView );
    ID3D10Resource *temp_pResource = NULL;
    ID3D10Resource **ppResourceTemp = ppResource == NULL ? NULL : &temp_pResource;
    m_pRenderTargetView->GetResource(  ppResourceTemp );
    if( ppResource != NULL )
    {
        D3DWrapperConvertInterface( ppResource, *ppResourceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10RenderTargetView::GetDevice()
////////////////////////////////////////////////////////////
void C10RenderTargetView::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pRenderTargetView );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pRenderTargetView->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10RenderTargetView::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10RenderTargetView::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pRenderTargetView );
    NRESULT rval = S_OK;
    rval = m_pRenderTargetView->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10RenderTargetView::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10RenderTargetView::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pRenderTargetView );
    NRESULT rval = S_OK;
    rval = m_pRenderTargetView->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10RenderTargetView::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10RenderTargetView::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pRenderTargetView );
    NRESULT rval = S_OK;
    rval = m_pRenderTargetView->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10DepthStencilView
//
////////////////////////////////////////////////////////////

C10DepthStencilView::C10DepthStencilView( ID3D10DepthStencilView *pDepthStencilView ) : 
    m_pDepthStencilView( pDepthStencilView )
{
}


C10DepthStencilView::~C10DepthStencilView()
{
    if( m_pDepthStencilView ) m_pDepthStencilView->Release();
}


////////////////////////////////////////////////////////////
// C10DepthStencilView::GetDesc()
////////////////////////////////////////////////////////////
void C10DepthStencilView::GetDesc(  D3D_DEPTH_STENCIL_VIEW_DESC * pDesc )
{
    assert( m_pDepthStencilView );
    BYTE *pDescByteTemp = NULL;
    D3D10_DEPTH_STENCIL_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_DEPTH_STENCIL_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_DEPTH_STENCIL_VIEW_DESC*)pDescByteTemp;
    }
    m_pDepthStencilView->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10DepthStencilView::GetResource()
////////////////////////////////////////////////////////////
void C10DepthStencilView::GetResource(  CResource ** ppResource )
{
    assert( m_pDepthStencilView );
    ID3D10Resource *temp_pResource = NULL;
    ID3D10Resource **ppResourceTemp = ppResource == NULL ? NULL : &temp_pResource;
    m_pDepthStencilView->GetResource(  ppResourceTemp );
    if( ppResource != NULL )
    {
        D3DWrapperConvertInterface( ppResource, *ppResourceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10DepthStencilView::GetDevice()
////////////////////////////////////////////////////////////
void C10DepthStencilView::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pDepthStencilView );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pDepthStencilView->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10DepthStencilView::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10DepthStencilView::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pDepthStencilView );
    NRESULT rval = S_OK;
    rval = m_pDepthStencilView->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10DepthStencilView::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10DepthStencilView::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pDepthStencilView );
    NRESULT rval = S_OK;
    rval = m_pDepthStencilView->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10DepthStencilView::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10DepthStencilView::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pDepthStencilView );
    NRESULT rval = S_OK;
    rval = m_pDepthStencilView->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10VertexShader
//
////////////////////////////////////////////////////////////

C10VertexShader::C10VertexShader( ID3D10VertexShader *pVertexShader ) : 
    m_pVertexShader( pVertexShader )
{
}


C10VertexShader::~C10VertexShader()
{
    if( m_pVertexShader ) m_pVertexShader->Release();
}


////////////////////////////////////////////////////////////
// C10VertexShader::GetDevice()
////////////////////////////////////////////////////////////
void C10VertexShader::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pVertexShader );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pVertexShader->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10VertexShader::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10VertexShader::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pVertexShader );
    NRESULT rval = S_OK;
    rval = m_pVertexShader->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10VertexShader::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10VertexShader::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pVertexShader );
    NRESULT rval = S_OK;
    rval = m_pVertexShader->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10VertexShader::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10VertexShader::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pVertexShader );
    NRESULT rval = S_OK;
    rval = m_pVertexShader->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10GeometryShader
//
////////////////////////////////////////////////////////////

C10GeometryShader::C10GeometryShader( ID3D10GeometryShader *pGeometryShader ) : 
    m_pGeometryShader( pGeometryShader )
{
}


C10GeometryShader::~C10GeometryShader()
{
    if( m_pGeometryShader ) m_pGeometryShader->Release();
}


////////////////////////////////////////////////////////////
// C10GeometryShader::GetDevice()
////////////////////////////////////////////////////////////
void C10GeometryShader::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pGeometryShader );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pGeometryShader->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10GeometryShader::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10GeometryShader::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pGeometryShader );
    NRESULT rval = S_OK;
    rval = m_pGeometryShader->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10GeometryShader::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10GeometryShader::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pGeometryShader );
    NRESULT rval = S_OK;
    rval = m_pGeometryShader->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10GeometryShader::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10GeometryShader::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pGeometryShader );
    NRESULT rval = S_OK;
    rval = m_pGeometryShader->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10PixelShader
//
////////////////////////////////////////////////////////////

C10PixelShader::C10PixelShader( ID3D10PixelShader *pPixelShader ) : 
    m_pPixelShader( pPixelShader )
{
}


C10PixelShader::~C10PixelShader()
{
    if( m_pPixelShader ) m_pPixelShader->Release();
}


////////////////////////////////////////////////////////////
// C10PixelShader::GetDevice()
////////////////////////////////////////////////////////////
void C10PixelShader::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pPixelShader );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pPixelShader->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10PixelShader::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10PixelShader::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pPixelShader );
    NRESULT rval = S_OK;
    rval = m_pPixelShader->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10PixelShader::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10PixelShader::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pPixelShader );
    NRESULT rval = S_OK;
    rval = m_pPixelShader->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10PixelShader::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10PixelShader::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pPixelShader );
    NRESULT rval = S_OK;
    rval = m_pPixelShader->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10InputLayout
//
////////////////////////////////////////////////////////////

C10InputLayout::C10InputLayout( ID3D10InputLayout *pInputLayout ) : 
    m_pInputLayout( pInputLayout )
{
}


C10InputLayout::~C10InputLayout()
{
    if( m_pInputLayout ) m_pInputLayout->Release();
}


////////////////////////////////////////////////////////////
// C10InputLayout::GetDevice()
////////////////////////////////////////////////////////////
void C10InputLayout::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pInputLayout );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pInputLayout->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10InputLayout::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10InputLayout::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pInputLayout );
    NRESULT rval = S_OK;
    rval = m_pInputLayout->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InputLayout::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10InputLayout::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pInputLayout );
    NRESULT rval = S_OK;
    rval = m_pInputLayout->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InputLayout::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10InputLayout::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pInputLayout );
    NRESULT rval = S_OK;
    rval = m_pInputLayout->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10SamplerState
//
////////////////////////////////////////////////////////////

C10SamplerState::C10SamplerState( ID3D10SamplerState *pSamplerState ) : 
    m_pSamplerState( pSamplerState )
{
}


C10SamplerState::~C10SamplerState()
{
    if( m_pSamplerState ) m_pSamplerState->Release();
}


////////////////////////////////////////////////////////////
// C10SamplerState::GetDesc()
////////////////////////////////////////////////////////////
void C10SamplerState::GetDesc(  D3D_SAMPLER_DESC * pDesc )
{
    assert( m_pSamplerState );
    BYTE *pDescByteTemp = NULL;
    D3D10_SAMPLER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_SAMPLER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_SAMPLER_DESC*)pDescByteTemp;
    }
    m_pSamplerState->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10SamplerState::GetDevice()
////////////////////////////////////////////////////////////
void C10SamplerState::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pSamplerState );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pSamplerState->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10SamplerState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10SamplerState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pSamplerState );
    NRESULT rval = S_OK;
    rval = m_pSamplerState->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10SamplerState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10SamplerState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pSamplerState );
    NRESULT rval = S_OK;
    rval = m_pSamplerState->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10SamplerState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10SamplerState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pSamplerState );
    NRESULT rval = S_OK;
    rval = m_pSamplerState->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10Asynchronous
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C10Query
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C10Predicate
//
////////////////////////////////////////////////////////////

C10Predicate::C10Predicate( ID3D10Predicate *pPredicate ) : 
    m_pPredicate( pPredicate )
{
}


C10Predicate::~C10Predicate()
{
    if( m_pPredicate ) m_pPredicate->Release();
}


////////////////////////////////////////////////////////////
// C10Predicate::GetDesc()
////////////////////////////////////////////////////////////
void C10Predicate::GetDesc(  D3D_QUERY_DESC * pDesc )
{
    assert( m_pPredicate );
    BYTE *pDescByteTemp = NULL;
    D3D10_QUERY_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_QUERY_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_QUERY_DESC*)pDescByteTemp;
    }
    m_pPredicate->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10Predicate::Begin()
////////////////////////////////////////////////////////////
void C10Predicate::Begin(  )
{
    assert( m_pPredicate );
    m_pPredicate->Begin(  );
}


////////////////////////////////////////////////////////////
// C10Predicate::End()
////////////////////////////////////////////////////////////
void C10Predicate::End(  )
{
    assert( m_pPredicate );
    m_pPredicate->End(  );
}


////////////////////////////////////////////////////////////
// C10Predicate::GetData()
////////////////////////////////////////////////////////////
NRESULT C10Predicate::GetData(  void * pData, UINT DataSize, UINT GetDataFlags )
{
    assert( m_pPredicate );
    NRESULT rval = S_OK;
    UINT temp_GetDataFlags;
    D3DFlagsConvertTo10_ASYNC_GETDATA_FLAG( &temp_GetDataFlags, GetDataFlags );
    rval = m_pPredicate->GetData(  pData,  DataSize,  temp_GetDataFlags );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Predicate::GetDataSize()
////////////////////////////////////////////////////////////
UINT C10Predicate::GetDataSize(  )
{
    assert( m_pPredicate );
    UINT rval = 0;
    rval = m_pPredicate->GetDataSize(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Predicate::GetDevice()
////////////////////////////////////////////////////////////
void C10Predicate::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pPredicate );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pPredicate->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Predicate::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Predicate::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pPredicate );
    NRESULT rval = S_OK;
    rval = m_pPredicate->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Predicate::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Predicate::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pPredicate );
    NRESULT rval = S_OK;
    rval = m_pPredicate->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Predicate::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10Predicate::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pPredicate );
    NRESULT rval = S_OK;
    rval = m_pPredicate->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10Counter
//
////////////////////////////////////////////////////////////

C10Counter::C10Counter( ID3D10Counter *pCounter ) : 
    m_pCounter( pCounter )
{
}


C10Counter::~C10Counter()
{
    if( m_pCounter ) m_pCounter->Release();
}


////////////////////////////////////////////////////////////
// C10Counter::GetDesc()
////////////////////////////////////////////////////////////
void C10Counter::GetDesc(  D3D_COUNTER_DESC * pDesc )
{
    assert( m_pCounter );
    BYTE *pDescByteTemp = NULL;
    D3D10_COUNTER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_COUNTER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_COUNTER_DESC*)pDescByteTemp;
    }
    m_pCounter->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10Counter::Begin()
////////////////////////////////////////////////////////////
void C10Counter::Begin(  )
{
    assert( m_pCounter );
    m_pCounter->Begin(  );
}


////////////////////////////////////////////////////////////
// C10Counter::End()
////////////////////////////////////////////////////////////
void C10Counter::End(  )
{
    assert( m_pCounter );
    m_pCounter->End(  );
}


////////////////////////////////////////////////////////////
// C10Counter::GetData()
////////////////////////////////////////////////////////////
NRESULT C10Counter::GetData(  void * pData, UINT DataSize, UINT GetDataFlags )
{
    assert( m_pCounter );
    NRESULT rval = S_OK;
    UINT temp_GetDataFlags;
    D3DFlagsConvertTo10_ASYNC_GETDATA_FLAG( &temp_GetDataFlags, GetDataFlags );
    rval = m_pCounter->GetData(  pData,  DataSize,  temp_GetDataFlags );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Counter::GetDataSize()
////////////////////////////////////////////////////////////
UINT C10Counter::GetDataSize(  )
{
    assert( m_pCounter );
    UINT rval = 0;
    rval = m_pCounter->GetDataSize(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Counter::GetDevice()
////////////////////////////////////////////////////////////
void C10Counter::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pCounter );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pCounter->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Counter::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Counter::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pCounter );
    NRESULT rval = S_OK;
    rval = m_pCounter->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Counter::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Counter::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pCounter );
    NRESULT rval = S_OK;
    rval = m_pCounter->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Counter::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10Counter::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pCounter );
    NRESULT rval = S_OK;
    rval = m_pCounter->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10Device
//
////////////////////////////////////////////////////////////

C10Device::C10Device( ID3D10Device *pDevice ) : 
    m_pDevice( pDevice )
{
}


C10Device::~C10Device()
{
    if( m_pDevice ) m_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C10Device::VSSetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device::VSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffers[j] )
            ppConstantBuffers[j]->GetInterfacePtr( &ppConstantBuffersConv[j] );
        else
            ppConstantBuffersConv[j] = NULL;
    }
    m_pDevice->VSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device::PSSetShaderResources()
////////////////////////////////////////////////////////////
void C10Device::PSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViews[j] )
            ppShaderResourceViews[j]->GetInterfacePtr( &ppShaderResourceViewsConv[j] );
        else
            ppShaderResourceViewsConv[j] = NULL;
    }
    m_pDevice->PSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device::PSSetShader()
////////////////////////////////////////////////////////////
void C10Device::PSSetShader(  CPixelShader * pPixelShader )
{
    assert( m_pDevice );
    ID3D10PixelShader *temp_pPixelShader = NULL;
    if( pPixelShader != NULL )
    {
        D3DWrapperConvertInterface( &temp_pPixelShader, pPixelShader );
    }
    m_pDevice->PSSetShader(  temp_pPixelShader );
}


////////////////////////////////////////////////////////////
// C10Device::PSSetSamplers()
////////////////////////////////////////////////////////////
void C10Device::PSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    assert( m_pDevice );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplers[j] )
            ppSamplers[j]->GetInterfacePtr( &ppSamplersConv[j] );
        else
            ppSamplersConv[j] = NULL;
    }
    m_pDevice->PSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device::VSSetShader()
////////////////////////////////////////////////////////////
void C10Device::VSSetShader(  CVertexShader * pVertexShader )
{
    assert( m_pDevice );
    ID3D10VertexShader *temp_pVertexShader = NULL;
    if( pVertexShader != NULL )
    {
        D3DWrapperConvertInterface( &temp_pVertexShader, pVertexShader );
    }
    m_pDevice->VSSetShader(  temp_pVertexShader );
}


////////////////////////////////////////////////////////////
// C10Device::DrawIndexed()
////////////////////////////////////////////////////////////
void C10Device::DrawIndexed(  UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation )
{
    assert( m_pDevice );
    m_pDevice->DrawIndexed(  IndexCount,  StartIndexLocation,  BaseVertexLocation );
}


////////////////////////////////////////////////////////////
// C10Device::Draw()
////////////////////////////////////////////////////////////
void C10Device::Draw(  UINT VertexCount, UINT StartVertexLocation )
{
    assert( m_pDevice );
    m_pDevice->Draw(  VertexCount,  StartVertexLocation );
}


////////////////////////////////////////////////////////////
// C10Device::PSSetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device::PSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffers[j] )
            ppConstantBuffers[j]->GetInterfacePtr( &ppConstantBuffersConv[j] );
        else
            ppConstantBuffersConv[j] = NULL;
    }
    m_pDevice->PSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device::IASetInputLayout()
////////////////////////////////////////////////////////////
void C10Device::IASetInputLayout(  CInputLayout * pInputLayout )
{
    assert( m_pDevice );
    ID3D10InputLayout *temp_pInputLayout = NULL;
    if( pInputLayout != NULL )
    {
        D3DWrapperConvertInterface( &temp_pInputLayout, pInputLayout );
    }
    m_pDevice->IASetInputLayout(  temp_pInputLayout );
}


////////////////////////////////////////////////////////////
// C10Device::IASetVertexBuffers()
////////////////////////////////////////////////////////////
void C10Device::IASetVertexBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppVertexBuffers, const UINT * pStrides, const UINT * pOffsets )
{
    assert( m_pDevice );
    ID3D10Buffer **ppVertexBuffersConv = ppVertexBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppVertexBuffers && !ppVertexBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppVertexBuffers[j] )
            ppVertexBuffers[j]->GetInterfacePtr( &ppVertexBuffersConv[j] );
        else
            ppVertexBuffersConv[j] = NULL;
    }
    m_pDevice->IASetVertexBuffers(  StartSlot,  NumBuffers,  ppVertexBuffersConv,  pStrides,  pOffsets );
    delete[]( ppVertexBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device::IASetIndexBuffer()
////////////////////////////////////////////////////////////
void C10Device::IASetIndexBuffer(  CBuffer * pIndexBuffer, DXGI_FORMAT Format, UINT Offset )
{
    assert( m_pDevice );
    ID3D10Buffer *temp_pIndexBuffer = NULL;
    if( pIndexBuffer != NULL )
    {
        D3DWrapperConvertInterface( &temp_pIndexBuffer, pIndexBuffer );
    }
    m_pDevice->IASetIndexBuffer(  temp_pIndexBuffer,  Format,  Offset );
}


////////////////////////////////////////////////////////////
// C10Device::DrawIndexedInstanced()
////////////////////////////////////////////////////////////
void C10Device::DrawIndexedInstanced(  UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation )
{
    assert( m_pDevice );
    m_pDevice->DrawIndexedInstanced(  IndexCountPerInstance,  InstanceCount,  StartIndexLocation,  BaseVertexLocation,  StartInstanceLocation );
}


////////////////////////////////////////////////////////////
// C10Device::DrawInstanced()
////////////////////////////////////////////////////////////
void C10Device::DrawInstanced(  UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation )
{
    assert( m_pDevice );
    m_pDevice->DrawInstanced(  VertexCountPerInstance,  InstanceCount,  StartVertexLocation,  StartInstanceLocation );
}


////////////////////////////////////////////////////////////
// C10Device::GSSetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device::GSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffers[j] )
            ppConstantBuffers[j]->GetInterfacePtr( &ppConstantBuffersConv[j] );
        else
            ppConstantBuffersConv[j] = NULL;
    }
    m_pDevice->GSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device::GSSetShader()
////////////////////////////////////////////////////////////
void C10Device::GSSetShader(  CGeometryShader * pShader )
{
    assert( m_pDevice );
    ID3D10GeometryShader *temp_pShader = NULL;
    if( pShader != NULL )
    {
        D3DWrapperConvertInterface( &temp_pShader, pShader );
    }
    m_pDevice->GSSetShader(  temp_pShader );
}


////////////////////////////////////////////////////////////
// C10Device::IASetPrimitiveTopology()
////////////////////////////////////////////////////////////
void C10Device::IASetPrimitiveTopology(  D3D_PRIMITIVE_TOPOLOGY Topology )
{
    assert( m_pDevice );
    D3D10_PRIMITIVE_TOPOLOGY temp_d3d10_primitive_topology;
    D3DWrapperConvertEnum( &temp_d3d10_primitive_topology, Topology );
    m_pDevice->IASetPrimitiveTopology(  temp_d3d10_primitive_topology );
}


////////////////////////////////////////////////////////////
// C10Device::VSSetShaderResources()
////////////////////////////////////////////////////////////
void C10Device::VSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViews[j] )
            ppShaderResourceViews[j]->GetInterfacePtr( &ppShaderResourceViewsConv[j] );
        else
            ppShaderResourceViewsConv[j] = NULL;
    }
    m_pDevice->VSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device::VSSetSamplers()
////////////////////////////////////////////////////////////
void C10Device::VSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    assert( m_pDevice );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplers[j] )
            ppSamplers[j]->GetInterfacePtr( &ppSamplersConv[j] );
        else
            ppSamplersConv[j] = NULL;
    }
    m_pDevice->VSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device::SetPredication()
////////////////////////////////////////////////////////////
void C10Device::SetPredication(  CPredicate * pPredicate, BOOL PredicateValue )
{
    assert( m_pDevice );
    ID3D10Predicate *temp_pPredicate = NULL;
    if( pPredicate != NULL )
    {
        D3DWrapperConvertInterface( &temp_pPredicate, pPredicate );
    }
    m_pDevice->SetPredication(  temp_pPredicate,  PredicateValue );
}


////////////////////////////////////////////////////////////
// C10Device::GSSetShaderResources()
////////////////////////////////////////////////////////////
void C10Device::GSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViews[j] )
            ppShaderResourceViews[j]->GetInterfacePtr( &ppShaderResourceViewsConv[j] );
        else
            ppShaderResourceViewsConv[j] = NULL;
    }
    m_pDevice->GSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device::GSSetSamplers()
////////////////////////////////////////////////////////////
void C10Device::GSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    assert( m_pDevice );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplers[j] )
            ppSamplers[j]->GetInterfacePtr( &ppSamplersConv[j] );
        else
            ppSamplersConv[j] = NULL;
    }
    m_pDevice->GSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device::OMSetRenderTargets()
////////////////////////////////////////////////////////////
void C10Device::OMSetRenderTargets(  UINT NumViews, CRenderTargetView *const * ppRenderTargetViews, CDepthStencilView * pDepthStencilView )
{
    assert( m_pDevice );
    ID3D10RenderTargetView **ppRenderTargetViewsConv = ppRenderTargetViews == NULL ? NULL : new ID3D10RenderTargetView*[NumViews];
    if( ppRenderTargetViews && !ppRenderTargetViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    ID3D10DepthStencilView *temp_pDepthStencilView = NULL;
    if( pDepthStencilView != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDepthStencilView, pDepthStencilView );
    }
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppRenderTargetViews[j] )
            ppRenderTargetViews[j]->GetInterfacePtr( &ppRenderTargetViewsConv[j] );
        else
            ppRenderTargetViewsConv[j] = NULL;
    }
    m_pDevice->OMSetRenderTargets(  NumViews,  ppRenderTargetViewsConv,  temp_pDepthStencilView );
    delete[]( ppRenderTargetViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device::OMSetBlendState()
////////////////////////////////////////////////////////////
void C10Device::OMSetBlendState(  CBlendState * pBlendState, const FLOAT BlendFactor[4], UINT SampleMask )
{
    assert( m_pDevice );
    ID3D10BlendState *temp_pBlendState = NULL;
    if( pBlendState != NULL )
    {
        D3DWrapperConvertInterface( &temp_pBlendState, pBlendState );
    }
    m_pDevice->OMSetBlendState(  temp_pBlendState,  BlendFactor,  SampleMask );
}


////////////////////////////////////////////////////////////
// C10Device::OMSetDepthStencilState()
////////////////////////////////////////////////////////////
void C10Device::OMSetDepthStencilState(  CDepthStencilState * pDepthStencilState, UINT StencilRef )
{
    assert( m_pDevice );
    ID3D10DepthStencilState *temp_pDepthStencilState = NULL;
    if( pDepthStencilState != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDepthStencilState, pDepthStencilState );
    }
    m_pDevice->OMSetDepthStencilState(  temp_pDepthStencilState,  StencilRef );
}


////////////////////////////////////////////////////////////
// C10Device::SOSetTargets()
////////////////////////////////////////////////////////////
void C10Device::SOSetTargets(  UINT NumBuffers, CBuffer *const * ppSOTargets, const UINT * pOffsets )
{
    assert( m_pDevice );
    ID3D10Buffer **ppSOTargetsConv = ppSOTargets == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppSOTargets && !ppSOTargetsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppSOTargets[j] )
            ppSOTargets[j]->GetInterfacePtr( &ppSOTargetsConv[j] );
        else
            ppSOTargetsConv[j] = NULL;
    }
    m_pDevice->SOSetTargets(  NumBuffers,  ppSOTargetsConv,  pOffsets );
    delete[]( ppSOTargetsConv );
}


////////////////////////////////////////////////////////////
// C10Device::DrawAuto()
////////////////////////////////////////////////////////////
void C10Device::DrawAuto(  )
{
    assert( m_pDevice );
    m_pDevice->DrawAuto(  );
}


////////////////////////////////////////////////////////////
// C10Device::RSSetState()
////////////////////////////////////////////////////////////
void C10Device::RSSetState(  CRasterizerState * pRasterizerState )
{
    assert( m_pDevice );
    ID3D10RasterizerState *temp_pRasterizerState = NULL;
    if( pRasterizerState != NULL )
    {
        D3DWrapperConvertInterface( &temp_pRasterizerState, pRasterizerState );
    }
    m_pDevice->RSSetState(  temp_pRasterizerState );
}


////////////////////////////////////////////////////////////
// C10Device::RSSetViewports()
////////////////////////////////////////////////////////////
void C10Device::RSSetViewports(  UINT NumViewports, const D3D_VIEWPORT * pViewports )
{
    assert( m_pDevice );
    D3D10_VIEWPORT *pViewportsConv = pViewports == NULL ? NULL : new D3D10_VIEWPORT[NumViewports];
    if( pViewports && !pViewportsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pViewports )
    {
        for( UINT i = NumViewports - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pViewportsConv[i], &pViewports[i] );
        }
    }
    m_pDevice->RSSetViewports(  NumViewports,  pViewportsConv );
    delete[]( pViewportsConv );
}


////////////////////////////////////////////////////////////
// C10Device::RSSetScissorRects()
////////////////////////////////////////////////////////////
void C10Device::RSSetScissorRects(  UINT NumRects, const D3D_RECT * pRects )
{
    assert( m_pDevice );
    m_pDevice->RSSetScissorRects(  NumRects,  pRects );
}


////////////////////////////////////////////////////////////
// C10Device::CopySubresourceRegion()
////////////////////////////////////////////////////////////
void C10Device::CopySubresourceRegion(  CResource * pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, CResource * pSrcResource, UINT SrcSubresource, const D3D_BOX * pSrcBox )
{
    assert( m_pDevice );
    ID3D10Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    ID3D10Resource *temp_pSrcResource = NULL;
    if( pSrcResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pSrcResource, pSrcResource );
    }
    BYTE *pSrcBoxByteTemp = NULL;
    D3D10_BOX *pSrcBoxTemp = NULL;
    SIZE_T pSrcBoxSizeBuffer = sizeof(D3D10_BOX);
    if( pSrcBox != NULL && pSrcBoxSizeBuffer != 0 )
    {
        pSrcBoxByteTemp = new BYTE[pSrcBoxSizeBuffer];
        pSrcBoxTemp = (D3D10_BOX*)pSrcBoxByteTemp;
        D3DWrapperConvertStruct( pSrcBoxTemp, pSrcBox );
    }
    m_pDevice->CopySubresourceRegion(  temp_pDstResource,  DstSubresource,  DstX,  DstY,  DstZ,  temp_pSrcResource,  SrcSubresource,  pSrcBoxTemp );
    delete[] pSrcBoxByteTemp;
}


////////////////////////////////////////////////////////////
// C10Device::CopyResource()
////////////////////////////////////////////////////////////
void C10Device::CopyResource(  CResource * pDstResource, CResource * pSrcResource )
{
    assert( m_pDevice );
    ID3D10Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    ID3D10Resource *temp_pSrcResource = NULL;
    if( pSrcResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pSrcResource, pSrcResource );
    }
    m_pDevice->CopyResource(  temp_pDstResource,  temp_pSrcResource );
}


////////////////////////////////////////////////////////////
// C10Device::UpdateSubresource()
////////////////////////////////////////////////////////////
void C10Device::UpdateSubresource(  CResource * pDstResource, UINT DstSubresource, const D3D_BOX * pDstBox, const void * pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch )
{
    assert( m_pDevice );
    ID3D10Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    BYTE *pDstBoxByteTemp = NULL;
    D3D10_BOX *pDstBoxTemp = NULL;
    SIZE_T pDstBoxSizeBuffer = sizeof(D3D10_BOX);
    if( pDstBox != NULL && pDstBoxSizeBuffer != 0 )
    {
        pDstBoxByteTemp = new BYTE[pDstBoxSizeBuffer];
        pDstBoxTemp = (D3D10_BOX*)pDstBoxByteTemp;
        D3DWrapperConvertStruct( pDstBoxTemp, pDstBox );
    }
    m_pDevice->UpdateSubresource(  temp_pDstResource,  DstSubresource,  pDstBoxTemp,  pSrcData,  SrcRowPitch,  SrcDepthPitch );
    delete[] pDstBoxByteTemp;
}


////////////////////////////////////////////////////////////
// C10Device::ClearRenderTargetView()
////////////////////////////////////////////////////////////
void C10Device::ClearRenderTargetView(  CRenderTargetView * pRenderTargetView, const FLOAT ColorRGBA[4] )
{
    assert( m_pDevice );
    ID3D10RenderTargetView *temp_pRenderTargetView = NULL;
    if( pRenderTargetView != NULL )
    {
        D3DWrapperConvertInterface( &temp_pRenderTargetView, pRenderTargetView );
    }
    m_pDevice->ClearRenderTargetView(  temp_pRenderTargetView,  ColorRGBA );
}


////////////////////////////////////////////////////////////
// C10Device::ClearDepthStencilView()
////////////////////////////////////////////////////////////
void C10Device::ClearDepthStencilView(  CDepthStencilView * pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil )
{
    assert( m_pDevice );
    ID3D10DepthStencilView *temp_pDepthStencilView = NULL;
    if( pDepthStencilView != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDepthStencilView, pDepthStencilView );
    }
    UINT temp_ClearFlags;
    D3DFlagsConvertTo10_CLEAR_FLAG( &temp_ClearFlags, ClearFlags );
    m_pDevice->ClearDepthStencilView(  temp_pDepthStencilView,  temp_ClearFlags,  Depth,  Stencil );
}


////////////////////////////////////////////////////////////
// C10Device::GenerateMips()
////////////////////////////////////////////////////////////
void C10Device::GenerateMips(  CShaderResourceView * pShaderResourceView )
{
    assert( m_pDevice );
    ID3D10ShaderResourceView *temp_pShaderResourceView = NULL;
    if( pShaderResourceView != NULL )
    {
        D3DWrapperConvertInterface( &temp_pShaderResourceView, pShaderResourceView );
    }
    m_pDevice->GenerateMips(  temp_pShaderResourceView );
}


////////////////////////////////////////////////////////////
// C10Device::ResolveSubresource()
////////////////////////////////////////////////////////////
void C10Device::ResolveSubresource(  CResource * pDstResource, UINT DstSubresource, CResource * pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format )
{
    assert( m_pDevice );
    ID3D10Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    ID3D10Resource *temp_pSrcResource = NULL;
    if( pSrcResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pSrcResource, pSrcResource );
    }
    m_pDevice->ResolveSubresource(  temp_pDstResource,  DstSubresource,  temp_pSrcResource,  SrcSubresource,  Format );
}


////////////////////////////////////////////////////////////
// C10Device::VSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device::VSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->VSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C10Buffer( ppConstantBuffersConv[j] );
            if( !ppConstantBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppConstantBuffers[j]->AddRef();
        }
        else
        {
            ppConstantBuffers[j] = NULL;
        }
    }
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device::PSGetShaderResources()
////////////////////////////////////////////////////////////
void C10Device::PSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->PSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C10ShaderResourceView( ppShaderResourceViewsConv[j] );
            if( !ppShaderResourceViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppShaderResourceViews[j]->AddRef();
        }
        else
        {
            ppShaderResourceViews[j] = NULL;
        }
    }
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device::PSGetShader()
////////////////////////////////////////////////////////////
void C10Device::PSGetShader(  CPixelShader ** ppPixelShader )
{
    assert( m_pDevice );
    ID3D10PixelShader *temp_pPixelShader = NULL;
    ID3D10PixelShader **ppPixelShaderTemp = ppPixelShader == NULL ? NULL : &temp_pPixelShader;
    m_pDevice->PSGetShader(  ppPixelShaderTemp );
    if( ppPixelShader != NULL )
    {
        D3DWrapperConvertInterface( ppPixelShader, *ppPixelShaderTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device::PSGetSamplers()
////////////////////////////////////////////////////////////
void C10Device::PSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->PSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C10SamplerState( ppSamplersConv[j] );
            if( !ppSamplers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppSamplers[j]->AddRef();
        }
        else
        {
            ppSamplers[j] = NULL;
        }
    }
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device::VSGetShader()
////////////////////////////////////////////////////////////
void C10Device::VSGetShader(  CVertexShader ** ppVertexShader )
{
    assert( m_pDevice );
    ID3D10VertexShader *temp_pVertexShader = NULL;
    ID3D10VertexShader **ppVertexShaderTemp = ppVertexShader == NULL ? NULL : &temp_pVertexShader;
    m_pDevice->VSGetShader(  ppVertexShaderTemp );
    if( ppVertexShader != NULL )
    {
        D3DWrapperConvertInterface( ppVertexShader, *ppVertexShaderTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device::PSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device::PSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->PSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C10Buffer( ppConstantBuffersConv[j] );
            if( !ppConstantBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppConstantBuffers[j]->AddRef();
        }
        else
        {
            ppConstantBuffers[j] = NULL;
        }
    }
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device::IAGetInputLayout()
////////////////////////////////////////////////////////////
void C10Device::IAGetInputLayout(  CInputLayout ** ppInputLayout )
{
    assert( m_pDevice );
    ID3D10InputLayout *temp_pInputLayout = NULL;
    ID3D10InputLayout **ppInputLayoutTemp = ppInputLayout == NULL ? NULL : &temp_pInputLayout;
    m_pDevice->IAGetInputLayout(  ppInputLayoutTemp );
    if( ppInputLayout != NULL )
    {
        D3DWrapperConvertInterface( ppInputLayout, *ppInputLayoutTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device::IAGetVertexBuffers()
////////////////////////////////////////////////////////////
void C10Device::IAGetVertexBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppVertexBuffers, UINT * pStrides, UINT * pOffsets )
{
    assert( m_pDevice );
    ID3D10Buffer **ppVertexBuffersConv = ppVertexBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppVertexBuffers && !ppVertexBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->IAGetVertexBuffers(  StartSlot,  NumBuffers,  ppVertexBuffersConv,  pStrides,  pOffsets );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppVertexBuffersConv[j] )
        {
            ppVertexBuffers[j] = new C10Buffer( ppVertexBuffersConv[j] );
            if( !ppVertexBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppVertexBuffers[j]->AddRef();
        }
        else
        {
            ppVertexBuffers[j] = NULL;
        }
    }
    delete[]( ppVertexBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device::IAGetIndexBuffer()
////////////////////////////////////////////////////////////
void C10Device::IAGetIndexBuffer(  CBuffer ** pIndexBuffer, DXGI_FORMAT * Format, UINT * Offset )
{
    assert( m_pDevice );
    ID3D10Buffer *temp_IndexBuffer = NULL;
    ID3D10Buffer **pIndexBufferTemp = pIndexBuffer == NULL ? NULL : &temp_IndexBuffer;
    m_pDevice->IAGetIndexBuffer(  pIndexBufferTemp,  Format,  Offset );
    if( pIndexBuffer != NULL )
    {
        D3DWrapperConvertInterface( pIndexBuffer, *pIndexBufferTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device::GSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device::GSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->GSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C10Buffer( ppConstantBuffersConv[j] );
            if( !ppConstantBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppConstantBuffers[j]->AddRef();
        }
        else
        {
            ppConstantBuffers[j] = NULL;
        }
    }
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device::GSGetShader()
////////////////////////////////////////////////////////////
void C10Device::GSGetShader(  CGeometryShader ** ppGeometryShader )
{
    assert( m_pDevice );
    ID3D10GeometryShader *temp_pGeometryShader = NULL;
    ID3D10GeometryShader **ppGeometryShaderTemp = ppGeometryShader == NULL ? NULL : &temp_pGeometryShader;
    m_pDevice->GSGetShader(  ppGeometryShaderTemp );
    if( ppGeometryShader != NULL )
    {
        D3DWrapperConvertInterface( ppGeometryShader, *ppGeometryShaderTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device::IAGetPrimitiveTopology()
////////////////////////////////////////////////////////////
void C10Device::IAGetPrimitiveTopology(  D3D_PRIMITIVE_TOPOLOGY * pTopology )
{
    assert( m_pDevice );
    D3D10_PRIMITIVE_TOPOLOGY temp_d3d10_primitive_topology;
    m_pDevice->IAGetPrimitiveTopology(  &temp_d3d10_primitive_topology );
    D3DWrapperConvertEnum( pTopology, temp_d3d10_primitive_topology );
}


////////////////////////////////////////////////////////////
// C10Device::VSGetShaderResources()
////////////////////////////////////////////////////////////
void C10Device::VSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->VSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C10ShaderResourceView( ppShaderResourceViewsConv[j] );
            if( !ppShaderResourceViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppShaderResourceViews[j]->AddRef();
        }
        else
        {
            ppShaderResourceViews[j] = NULL;
        }
    }
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device::VSGetSamplers()
////////////////////////////////////////////////////////////
void C10Device::VSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->VSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C10SamplerState( ppSamplersConv[j] );
            if( !ppSamplers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppSamplers[j]->AddRef();
        }
        else
        {
            ppSamplers[j] = NULL;
        }
    }
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device::GetPredication()
////////////////////////////////////////////////////////////
void C10Device::GetPredication(  CPredicate ** ppPredicate, BOOL * pPredicateValue )
{
    assert( m_pDevice );
    ID3D10Predicate *temp_pPredicate = NULL;
    ID3D10Predicate **ppPredicateTemp = ppPredicate == NULL ? NULL : &temp_pPredicate;
    m_pDevice->GetPredication(  ppPredicateTemp,  pPredicateValue );
    if( ppPredicate != NULL )
    {
        D3DWrapperConvertInterface( ppPredicate, *ppPredicateTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device::GSGetShaderResources()
////////////////////////////////////////////////////////////
void C10Device::GSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->GSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C10ShaderResourceView( ppShaderResourceViewsConv[j] );
            if( !ppShaderResourceViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppShaderResourceViews[j]->AddRef();
        }
        else
        {
            ppShaderResourceViews[j] = NULL;
        }
    }
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device::GSGetSamplers()
////////////////////////////////////////////////////////////
void C10Device::GSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->GSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C10SamplerState( ppSamplersConv[j] );
            if( !ppSamplers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppSamplers[j]->AddRef();
        }
        else
        {
            ppSamplers[j] = NULL;
        }
    }
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device::OMGetRenderTargets()
////////////////////////////////////////////////////////////
void C10Device::OMGetRenderTargets(  UINT NumViews, CRenderTargetView ** ppRenderTargetViews, CDepthStencilView ** ppDepthStencilView )
{
    assert( m_pDevice );
    ID3D10RenderTargetView **ppRenderTargetViewsConv = ppRenderTargetViews == NULL ? NULL : new ID3D10RenderTargetView*[NumViews];
    if( ppRenderTargetViews && !ppRenderTargetViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    ID3D10DepthStencilView *temp_pDepthStencilView = NULL;
    ID3D10DepthStencilView **ppDepthStencilViewTemp = ppDepthStencilView == NULL ? NULL : &temp_pDepthStencilView;
    m_pDevice->OMGetRenderTargets(  NumViews,  ppRenderTargetViewsConv,  ppDepthStencilViewTemp );
    if( ppDepthStencilView != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilView, *ppDepthStencilViewTemp );
    }
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppRenderTargetViewsConv[j] )
        {
            ppRenderTargetViews[j] = new C10RenderTargetView( ppRenderTargetViewsConv[j] );
            if( !ppRenderTargetViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppRenderTargetViews[j]->AddRef();
        }
        else
        {
            ppRenderTargetViews[j] = NULL;
        }
    }
    delete[]( ppRenderTargetViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device::OMGetBlendState()
////////////////////////////////////////////////////////////
void C10Device::OMGetBlendState(  CBlendState ** ppBlendState, FLOAT BlendFactor[4], UINT * pSampleMask )
{
    assert( m_pDevice );
    ID3D10BlendState *temp_pBlendState = NULL;
    ID3D10BlendState **ppBlendStateTemp = ppBlendState == NULL ? NULL : &temp_pBlendState;
    m_pDevice->OMGetBlendState(  ppBlendStateTemp,  BlendFactor,  pSampleMask );
    if( ppBlendState != NULL )
    {
        D3DWrapperConvertInterface( ppBlendState, *ppBlendStateTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device::OMGetDepthStencilState()
////////////////////////////////////////////////////////////
void C10Device::OMGetDepthStencilState(  CDepthStencilState ** ppDepthStencilState, UINT * pStencilRef )
{
    assert( m_pDevice );
    ID3D10DepthStencilState *temp_pDepthStencilState = NULL;
    ID3D10DepthStencilState **ppDepthStencilStateTemp = ppDepthStencilState == NULL ? NULL : &temp_pDepthStencilState;
    m_pDevice->OMGetDepthStencilState(  ppDepthStencilStateTemp,  pStencilRef );
    if( ppDepthStencilState != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilState, *ppDepthStencilStateTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device::SOGetTargets()
////////////////////////////////////////////////////////////
void C10Device::SOGetTargets(  UINT NumBuffers, CBuffer ** ppSOTargets, UINT * pOffsets )
{
    assert( m_pDevice );
    ID3D10Buffer **ppSOTargetsConv = ppSOTargets == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppSOTargets && !ppSOTargetsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->SOGetTargets(  NumBuffers,  ppSOTargetsConv,  pOffsets );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppSOTargetsConv[j] )
        {
            ppSOTargets[j] = new C10Buffer( ppSOTargetsConv[j] );
            if( !ppSOTargets[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppSOTargets[j]->AddRef();
        }
        else
        {
            ppSOTargets[j] = NULL;
        }
    }
    delete[]( ppSOTargetsConv );
}


////////////////////////////////////////////////////////////
// C10Device::RSGetState()
////////////////////////////////////////////////////////////
void C10Device::RSGetState(  CRasterizerState ** ppRasterizerState )
{
    assert( m_pDevice );
    ID3D10RasterizerState *temp_pRasterizerState = NULL;
    ID3D10RasterizerState **ppRasterizerStateTemp = ppRasterizerState == NULL ? NULL : &temp_pRasterizerState;
    m_pDevice->RSGetState(  ppRasterizerStateTemp );
    if( ppRasterizerState != NULL )
    {
        D3DWrapperConvertInterface( ppRasterizerState, *ppRasterizerStateTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device::RSGetViewports()
////////////////////////////////////////////////////////////
void C10Device::RSGetViewports(  UINT * NumViewports, D3D_VIEWPORT * pViewports )
{
    assert( m_pDevice );
    D3D10_VIEWPORT *pViewportsConv = pViewports == NULL ? NULL : new D3D10_VIEWPORT[*NumViewports];
    if( pViewports && !pViewportsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice->RSGetViewports(  NumViewports,  pViewportsConv );
    for( UINT i = *NumViewports - 1; i != (UINT) -1; --i )
        D3DWrapperConvertStruct( &pViewports[i], &pViewportsConv[i] );
}


////////////////////////////////////////////////////////////
// C10Device::RSGetScissorRects()
////////////////////////////////////////////////////////////
void C10Device::RSGetScissorRects(  UINT * NumRects, D3D_RECT * pRects )
{
    assert( m_pDevice );
    m_pDevice->RSGetScissorRects(  NumRects,  pRects );
}


////////////////////////////////////////////////////////////
// C10Device::GetDeviceRemovedReason()
////////////////////////////////////////////////////////////
NRESULT C10Device::GetDeviceRemovedReason(  )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    rval = m_pDevice->GetDeviceRemovedReason(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::SetExceptionMode()
////////////////////////////////////////////////////////////
NRESULT C10Device::SetExceptionMode(  UINT RaiseFlags )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    UINT temp_RaiseFlags;
    D3DFlagsConvertTo10_RAISE_FLAG( &temp_RaiseFlags, RaiseFlags );
    rval = m_pDevice->SetExceptionMode(  temp_RaiseFlags );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::GetExceptionMode()
////////////////////////////////////////////////////////////
UINT C10Device::GetExceptionMode(  )
{
    assert( m_pDevice );
    UINT rval = 0;
    rval = m_pDevice->GetExceptionMode(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Device::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    rval = m_pDevice->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Device::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    rval = m_pDevice->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10Device::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    rval = m_pDevice->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::ClearState()
////////////////////////////////////////////////////////////
void C10Device::ClearState(  )
{
    assert( m_pDevice );
    m_pDevice->ClearState(  );
}


////////////////////////////////////////////////////////////
// C10Device::Flush()
////////////////////////////////////////////////////////////
void C10Device::Flush(  )
{
    assert( m_pDevice );
    m_pDevice->Flush(  );
}


////////////////////////////////////////////////////////////
// C10Device::CreateBuffer()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateBuffer(  const D3D_BUFFER_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CBuffer ** ppBuffer )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D10_BUFFER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_BUFFER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_BUFFER_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    BYTE *pInitialDataByteTemp = NULL;
    D3D10_SUBRESOURCE_DATA *pInitialDataTemp = NULL;
    SIZE_T pInitialDataSizeBuffer = sizeof(D3D10_SUBRESOURCE_DATA);
    if( pInitialData != NULL && pInitialDataSizeBuffer != 0 )
    {
        pInitialDataByteTemp = new BYTE[pInitialDataSizeBuffer];
        pInitialDataTemp = (D3D10_SUBRESOURCE_DATA*)pInitialDataByteTemp;
        D3DWrapperConvertStruct( pInitialDataTemp, pInitialData );
    }
    ID3D10Buffer *temp_pBuffer = NULL;
    ID3D10Buffer **ppBufferTemp = ppBuffer == NULL ? NULL : &temp_pBuffer;
    rval = m_pDevice->CreateBuffer(  pDescTemp,  pInitialDataTemp,  ppBufferTemp );
    delete[] pDescByteTemp;
    delete[] pInitialDataByteTemp;
    if( ppBuffer != NULL )
    {
        D3DWrapperConvertInterface( ppBuffer, *ppBufferTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateTexture1D()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateTexture1D(  const D3D_TEXTURE1D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture1D ** ppTexture1D )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D10_TEXTURE1D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_TEXTURE1D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_TEXTURE1D_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    UINT mipCount = pDesc->MipLevels;
    if( mipCount == 0 )
    {
        UINT maxDimension = pDesc->Width;
        mipCount = IntLog2( maxDimension ) + 1;
    }
    D3D10_SUBRESOURCE_DATA *pInitialDataConv = pInitialData == NULL ? NULL : new D3D10_SUBRESOURCE_DATA[(pDesc->ArraySize * mipCount)];
    if( pInitialData && !pInitialDataConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInitialData )
    {
        for( UINT i = (pDesc->ArraySize * mipCount) - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInitialDataConv[i], &pInitialData[i] );
        }
    }
    ID3D10Texture1D *temp_pTexture1D = NULL;
    ID3D10Texture1D **ppTexture1DTemp = ppTexture1D == NULL ? NULL : &temp_pTexture1D;
    rval = m_pDevice->CreateTexture1D(  pDescTemp,  pInitialDataConv,  ppTexture1DTemp );
    delete[] pDescByteTemp;
    delete[]( pInitialDataConv );
    if( ppTexture1D != NULL )
    {
        D3DWrapperConvertInterface( ppTexture1D, *ppTexture1DTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateTexture2D()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateTexture2D(  const D3D_TEXTURE2D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture2D ** ppTexture2D )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D10_TEXTURE2D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_TEXTURE2D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_TEXTURE2D_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    UINT mipCount = pDesc->MipLevels;
    if( mipCount == 0 )
    {
        UINT maxDimension = max( pDesc->Width, pDesc->Height );
        mipCount = IntLog2( maxDimension ) + 1;
    }
    D3D10_SUBRESOURCE_DATA *pInitialDataConv = pInitialData == NULL ? NULL : new D3D10_SUBRESOURCE_DATA[(pDesc->ArraySize * mipCount)];
    if( pInitialData && !pInitialDataConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInitialData )
    {
        for( UINT i = (pDesc->ArraySize * mipCount) - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInitialDataConv[i], &pInitialData[i] );
        }
    }
    ID3D10Texture2D *temp_pTexture2D = NULL;
    ID3D10Texture2D **ppTexture2DTemp = ppTexture2D == NULL ? NULL : &temp_pTexture2D;
    rval = m_pDevice->CreateTexture2D(  pDescTemp,  pInitialDataConv,  ppTexture2DTemp );
    delete[] pDescByteTemp;
    delete[]( pInitialDataConv );
    if( ppTexture2D != NULL )
    {
        D3DWrapperConvertInterface( ppTexture2D, *ppTexture2DTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateTexture3D()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateTexture3D(  const D3D_TEXTURE3D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture3D ** ppTexture3D )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D10_TEXTURE3D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_TEXTURE3D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_TEXTURE3D_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    UINT mipCount = pDesc->MipLevels;
    if( mipCount == 0 )
    {
        UINT maxDimension = max( pDesc->Depth, max( pDesc->Width, pDesc->Height ) );
        mipCount = IntLog2( maxDimension ) + 1;
    }
    D3D10_SUBRESOURCE_DATA *pInitialDataConv = pInitialData == NULL ? NULL : new D3D10_SUBRESOURCE_DATA[(mipCount)];
    if( pInitialData && !pInitialDataConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInitialData )
    {
        for( UINT i = (mipCount) - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInitialDataConv[i], &pInitialData[i] );
        }
    }
    ID3D10Texture3D *temp_pTexture3D = NULL;
    ID3D10Texture3D **ppTexture3DTemp = ppTexture3D == NULL ? NULL : &temp_pTexture3D;
    rval = m_pDevice->CreateTexture3D(  pDescTemp,  pInitialDataConv,  ppTexture3DTemp );
    delete[] pDescByteTemp;
    delete[]( pInitialDataConv );
    if( ppTexture3D != NULL )
    {
        D3DWrapperConvertInterface( ppTexture3D, *ppTexture3DTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateShaderResourceView()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateShaderResourceView(  CResource * pResource, const D3D_SHADER_RESOURCE_VIEW_DESC * pDesc, CShaderResourceView ** ppSRView )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D10Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    BYTE *pDescByteTemp = NULL;
    D3D10_SHADER_RESOURCE_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_SHADER_RESOURCE_VIEW_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    ID3D10ShaderResourceView *temp_pSRView = NULL;
    ID3D10ShaderResourceView **ppSRViewTemp = ppSRView == NULL ? NULL : &temp_pSRView;
    rval = m_pDevice->CreateShaderResourceView(  temp_pResource,  pDescTemp,  ppSRViewTemp );
    delete[] pDescByteTemp;
    if( ppSRView != NULL )
    {
        D3DWrapperConvertInterface( ppSRView, *ppSRViewTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateRenderTargetView()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateRenderTargetView(  CResource * pResource, const D3D_RENDER_TARGET_VIEW_DESC * pDesc, CRenderTargetView ** ppRTView )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D10Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    BYTE *pDescByteTemp = NULL;
    D3D10_RENDER_TARGET_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_RENDER_TARGET_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_RENDER_TARGET_VIEW_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    ID3D10RenderTargetView *temp_pRTView = NULL;
    ID3D10RenderTargetView **ppRTViewTemp = ppRTView == NULL ? NULL : &temp_pRTView;
    rval = m_pDevice->CreateRenderTargetView(  temp_pResource,  pDescTemp,  ppRTViewTemp );
    delete[] pDescByteTemp;
    if( ppRTView != NULL )
    {
        D3DWrapperConvertInterface( ppRTView, *ppRTViewTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateDepthStencilView()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateDepthStencilView(  CResource * pResource, const D3D_DEPTH_STENCIL_VIEW_DESC * pDesc, CDepthStencilView ** ppDepthStencilView )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D10Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    BYTE *pDescByteTemp = NULL;
    D3D10_DEPTH_STENCIL_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_DEPTH_STENCIL_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_DEPTH_STENCIL_VIEW_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    ID3D10DepthStencilView *temp_pDepthStencilView = NULL;
    ID3D10DepthStencilView **ppDepthStencilViewTemp = ppDepthStencilView == NULL ? NULL : &temp_pDepthStencilView;
    rval = m_pDevice->CreateDepthStencilView(  temp_pResource,  pDescTemp,  ppDepthStencilViewTemp );
    delete[] pDescByteTemp;
    if( ppDepthStencilView != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilView, *ppDepthStencilViewTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateInputLayout()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateInputLayout(  const D3D_INPUT_ELEMENT_DESC * pInputElementDescs, UINT NumElements, const void * pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, CInputLayout ** ppInputLayout )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    D3D10_INPUT_ELEMENT_DESC *pInputElementDescsConv = pInputElementDescs == NULL ? NULL : new D3D10_INPUT_ELEMENT_DESC[NumElements];
    if( pInputElementDescs && !pInputElementDescsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInputElementDescs )
    {
        for( UINT i = NumElements - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInputElementDescsConv[i], &pInputElementDescs[i] );
        }
    }
    ID3D10InputLayout *temp_pInputLayout = NULL;
    ID3D10InputLayout **ppInputLayoutTemp = ppInputLayout == NULL ? NULL : &temp_pInputLayout;
    rval = m_pDevice->CreateInputLayout(  pInputElementDescsConv,  NumElements,  pShaderBytecodeWithInputSignature,  BytecodeLength,  ppInputLayoutTemp );
    delete[]( pInputElementDescsConv );
    if( ppInputLayout != NULL )
    {
        D3DWrapperConvertInterface( ppInputLayout, *ppInputLayoutTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateVertexShader()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateVertexShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CVertexShader ** ppVertexShader )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D10VertexShader *temp_pVertexShader = NULL;
    ID3D10VertexShader **ppVertexShaderTemp = ppVertexShader == NULL ? NULL : &temp_pVertexShader;
    rval = m_pDevice->CreateVertexShader(  pShaderBytecode,  BytecodeLength,  ppVertexShaderTemp );
    if( ppVertexShader != NULL )
    {
        D3DWrapperConvertInterface( ppVertexShader, *ppVertexShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateGeometryShader()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateGeometryShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CGeometryShader ** ppGeometryShader )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D10GeometryShader *temp_pGeometryShader = NULL;
    ID3D10GeometryShader **ppGeometryShaderTemp = ppGeometryShader == NULL ? NULL : &temp_pGeometryShader;
    rval = m_pDevice->CreateGeometryShader(  pShaderBytecode,  BytecodeLength,  ppGeometryShaderTemp );
    if( ppGeometryShader != NULL )
    {
        D3DWrapperConvertInterface( ppGeometryShader, *ppGeometryShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateGeometryShaderWithStreamOutput()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateGeometryShaderWithStreamOutput(  const void * pShaderBytecode, SIZE_T BytecodeLength, const D3D_SO_DECLARATION_ENTRY * pSODeclaration, UINT NumEntries, UINT OutputStreamStride, CGeometryShader ** ppGeometryShader )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    D3D10_SO_DECLARATION_ENTRY *pSODeclarationConv = pSODeclaration == NULL ? NULL : new D3D10_SO_DECLARATION_ENTRY[NumEntries];
    if( pSODeclaration && !pSODeclarationConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pSODeclaration )
    {
        for( UINT i = NumEntries - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pSODeclarationConv[i], &pSODeclaration[i] );
        }
    }
    ID3D10GeometryShader *temp_pGeometryShader = NULL;
    ID3D10GeometryShader **ppGeometryShaderTemp = ppGeometryShader == NULL ? NULL : &temp_pGeometryShader;
    rval = m_pDevice->CreateGeometryShaderWithStreamOutput(  pShaderBytecode,  BytecodeLength,  pSODeclarationConv,  NumEntries,  OutputStreamStride,  ppGeometryShaderTemp );
    delete[]( pSODeclarationConv );
    if( ppGeometryShader != NULL )
    {
        D3DWrapperConvertInterface( ppGeometryShader, *ppGeometryShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreatePixelShader()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreatePixelShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CPixelShader ** ppPixelShader )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D10PixelShader *temp_pPixelShader = NULL;
    ID3D10PixelShader **ppPixelShaderTemp = ppPixelShader == NULL ? NULL : &temp_pPixelShader;
    rval = m_pDevice->CreatePixelShader(  pShaderBytecode,  BytecodeLength,  ppPixelShaderTemp );
    if( ppPixelShader != NULL )
    {
        D3DWrapperConvertInterface( ppPixelShader, *ppPixelShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateBlendState()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateBlendState(  const D3D_BLEND_DESC * pBlendStateDesc, CBlendState ** ppBlendState )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pBlendStateDescByteTemp = NULL;
    D3D10_BLEND_DESC *pBlendStateDescTemp = NULL;
    SIZE_T pBlendStateDescSizeBuffer = sizeof(D3D10_BLEND_DESC);
    if( pBlendStateDesc != NULL && pBlendStateDescSizeBuffer != 0 )
    {
        pBlendStateDescByteTemp = new BYTE[pBlendStateDescSizeBuffer];
        pBlendStateDescTemp = (D3D10_BLEND_DESC*)pBlendStateDescByteTemp;
        D3DWrapperConvertStruct( pBlendStateDescTemp, pBlendStateDesc );
    }
    ID3D10BlendState *temp_pBlendState = NULL;
    ID3D10BlendState **ppBlendStateTemp = ppBlendState == NULL ? NULL : &temp_pBlendState;
    rval = m_pDevice->CreateBlendState(  pBlendStateDescTemp,  ppBlendStateTemp );
    delete[] pBlendStateDescByteTemp;
    if( ppBlendState != NULL )
    {
        D3DWrapperConvertInterface( ppBlendState, *ppBlendStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateDepthStencilState()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateDepthStencilState(  const D3D_DEPTH_STENCIL_DESC * pDepthStencilDesc, CDepthStencilState ** ppDepthStencilState )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDepthStencilDescByteTemp = NULL;
    D3D10_DEPTH_STENCIL_DESC *pDepthStencilDescTemp = NULL;
    SIZE_T pDepthStencilDescSizeBuffer = sizeof(D3D10_DEPTH_STENCIL_DESC);
    if( pDepthStencilDesc != NULL && pDepthStencilDescSizeBuffer != 0 )
    {
        pDepthStencilDescByteTemp = new BYTE[pDepthStencilDescSizeBuffer];
        pDepthStencilDescTemp = (D3D10_DEPTH_STENCIL_DESC*)pDepthStencilDescByteTemp;
        D3DWrapperConvertStruct( pDepthStencilDescTemp, pDepthStencilDesc );
    }
    ID3D10DepthStencilState *temp_pDepthStencilState = NULL;
    ID3D10DepthStencilState **ppDepthStencilStateTemp = ppDepthStencilState == NULL ? NULL : &temp_pDepthStencilState;
    rval = m_pDevice->CreateDepthStencilState(  pDepthStencilDescTemp,  ppDepthStencilStateTemp );
    delete[] pDepthStencilDescByteTemp;
    if( ppDepthStencilState != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilState, *ppDepthStencilStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateRasterizerState()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateRasterizerState(  const D3D_RASTERIZER_DESC * pRasterizerDesc, CRasterizerState ** ppRasterizerState )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pRasterizerDescByteTemp = NULL;
    D3D10_RASTERIZER_DESC *pRasterizerDescTemp = NULL;
    SIZE_T pRasterizerDescSizeBuffer = sizeof(D3D10_RASTERIZER_DESC);
    if( pRasterizerDesc != NULL && pRasterizerDescSizeBuffer != 0 )
    {
        pRasterizerDescByteTemp = new BYTE[pRasterizerDescSizeBuffer];
        pRasterizerDescTemp = (D3D10_RASTERIZER_DESC*)pRasterizerDescByteTemp;
        D3DWrapperConvertStruct( pRasterizerDescTemp, pRasterizerDesc );
    }
    ID3D10RasterizerState *temp_pRasterizerState = NULL;
    ID3D10RasterizerState **ppRasterizerStateTemp = ppRasterizerState == NULL ? NULL : &temp_pRasterizerState;
    rval = m_pDevice->CreateRasterizerState(  pRasterizerDescTemp,  ppRasterizerStateTemp );
    delete[] pRasterizerDescByteTemp;
    if( ppRasterizerState != NULL )
    {
        D3DWrapperConvertInterface( ppRasterizerState, *ppRasterizerStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateSamplerState()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateSamplerState(  const D3D_SAMPLER_DESC * pSamplerDesc, CSamplerState ** ppSamplerState )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pSamplerDescByteTemp = NULL;
    D3D10_SAMPLER_DESC *pSamplerDescTemp = NULL;
    SIZE_T pSamplerDescSizeBuffer = sizeof(D3D10_SAMPLER_DESC);
    if( pSamplerDesc != NULL && pSamplerDescSizeBuffer != 0 )
    {
        pSamplerDescByteTemp = new BYTE[pSamplerDescSizeBuffer];
        pSamplerDescTemp = (D3D10_SAMPLER_DESC*)pSamplerDescByteTemp;
        D3DWrapperConvertStruct( pSamplerDescTemp, pSamplerDesc );
    }
    ID3D10SamplerState *temp_pSamplerState = NULL;
    ID3D10SamplerState **ppSamplerStateTemp = ppSamplerState == NULL ? NULL : &temp_pSamplerState;
    rval = m_pDevice->CreateSamplerState(  pSamplerDescTemp,  ppSamplerStateTemp );
    delete[] pSamplerDescByteTemp;
    if( ppSamplerState != NULL )
    {
        D3DWrapperConvertInterface( ppSamplerState, *ppSamplerStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateQuery()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateQuery(  const D3D_QUERY_DESC * pQueryDesc, CQuery ** ppQuery )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pQueryDescByteTemp = NULL;
    D3D10_QUERY_DESC *pQueryDescTemp = NULL;
    SIZE_T pQueryDescSizeBuffer = sizeof(D3D10_QUERY_DESC);
    if( pQueryDesc != NULL && pQueryDescSizeBuffer != 0 )
    {
        pQueryDescByteTemp = new BYTE[pQueryDescSizeBuffer];
        pQueryDescTemp = (D3D10_QUERY_DESC*)pQueryDescByteTemp;
        D3DWrapperConvertStruct( pQueryDescTemp, pQueryDesc );
    }
    ID3D10Query *temp_pQuery = NULL;
    ID3D10Query **ppQueryTemp = ppQuery == NULL ? NULL : &temp_pQuery;
    rval = m_pDevice->CreateQuery(  pQueryDescTemp,  ppQueryTemp );
    delete[] pQueryDescByteTemp;
    if( ppQuery != NULL )
    {
        D3DWrapperConvertInterface( ppQuery, *ppQueryTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreatePredicate()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreatePredicate(  const D3D_QUERY_DESC * pPredicateDesc, CPredicate ** ppPredicate )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pPredicateDescByteTemp = NULL;
    D3D10_QUERY_DESC *pPredicateDescTemp = NULL;
    SIZE_T pPredicateDescSizeBuffer = sizeof(D3D10_QUERY_DESC);
    if( pPredicateDesc != NULL && pPredicateDescSizeBuffer != 0 )
    {
        pPredicateDescByteTemp = new BYTE[pPredicateDescSizeBuffer];
        pPredicateDescTemp = (D3D10_QUERY_DESC*)pPredicateDescByteTemp;
        D3DWrapperConvertStruct( pPredicateDescTemp, pPredicateDesc );
    }
    ID3D10Predicate *temp_pPredicate = NULL;
    ID3D10Predicate **ppPredicateTemp = ppPredicate == NULL ? NULL : &temp_pPredicate;
    rval = m_pDevice->CreatePredicate(  pPredicateDescTemp,  ppPredicateTemp );
    delete[] pPredicateDescByteTemp;
    if( ppPredicate != NULL )
    {
        D3DWrapperConvertInterface( ppPredicate, *ppPredicateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CreateCounter()
////////////////////////////////////////////////////////////
NRESULT C10Device::CreateCounter(  const D3D_COUNTER_DESC * pCounterDesc, CCounter ** ppCounter )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pCounterDescByteTemp = NULL;
    D3D10_COUNTER_DESC *pCounterDescTemp = NULL;
    SIZE_T pCounterDescSizeBuffer = sizeof(D3D10_COUNTER_DESC);
    if( pCounterDesc != NULL && pCounterDescSizeBuffer != 0 )
    {
        pCounterDescByteTemp = new BYTE[pCounterDescSizeBuffer];
        pCounterDescTemp = (D3D10_COUNTER_DESC*)pCounterDescByteTemp;
        D3DWrapperConvertStruct( pCounterDescTemp, pCounterDesc );
    }
    ID3D10Counter *temp_pCounter = NULL;
    ID3D10Counter **ppCounterTemp = ppCounter == NULL ? NULL : &temp_pCounter;
    rval = m_pDevice->CreateCounter(  pCounterDescTemp,  ppCounterTemp );
    delete[] pCounterDescByteTemp;
    if( ppCounter != NULL )
    {
        D3DWrapperConvertInterface( ppCounter, *ppCounterTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CheckFormatSupport()
////////////////////////////////////////////////////////////
NRESULT C10Device::CheckFormatSupport(  DXGI_FORMAT Format, UINT * pFormatSupport )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    UINT temp_pFormatSupport;
    rval = m_pDevice->CheckFormatSupport(  Format,  &temp_pFormatSupport );
    D3DFlagsConvertFrom10_FORMAT_SUPPORT( pFormatSupport, temp_pFormatSupport );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CheckMultisampleQualityLevels()
////////////////////////////////////////////////////////////
NRESULT C10Device::CheckMultisampleQualityLevels(  DXGI_FORMAT Format, UINT SampleCount, UINT * pNumQualityLevels )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    rval = m_pDevice->CheckMultisampleQualityLevels(  Format,  SampleCount,  pNumQualityLevels );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::CheckCounterInfo()
////////////////////////////////////////////////////////////
void C10Device::CheckCounterInfo(  D3D_COUNTER_INFO * pCounterInfo )
{
    assert( m_pDevice );
    BYTE *pCounterInfoByteTemp = NULL;
    D3D10_COUNTER_INFO *pCounterInfoTemp = NULL;
    SIZE_T pCounterInfoSizeBuffer = sizeof(D3D10_COUNTER_INFO);
    if( pCounterInfo != NULL && pCounterInfoSizeBuffer != 0 )
    {
        pCounterInfoByteTemp = new BYTE[pCounterInfoSizeBuffer];
        pCounterInfoTemp = (D3D10_COUNTER_INFO*)pCounterInfoByteTemp;
    }
    m_pDevice->CheckCounterInfo(  pCounterInfoTemp );
    if( pCounterInfo != NULL && pCounterInfoSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pCounterInfo, pCounterInfoTemp );
        delete[] pCounterInfoByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10Device::CheckCounter()
////////////////////////////////////////////////////////////
NRESULT C10Device::CheckCounter(  const D3D_COUNTER_DESC * pDesc, D3D_COUNTER_TYPE * pType, UINT * pActiveCounters, LPSTR szName, UINT * pNameLength, LPSTR szUnits, UINT * pUnitsLength, LPSTR szDescription, UINT * pDescriptionLength )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D10_COUNTER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_COUNTER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_COUNTER_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    D3D10_COUNTER_TYPE temp_d3d10_counter_type;
    rval = m_pDevice->CheckCounter(  pDescTemp,  &temp_d3d10_counter_type,  pActiveCounters,  szName,  pNameLength,  szUnits,  pUnitsLength,  szDescription,  pDescriptionLength );
    delete[] pDescByteTemp;
    D3DWrapperConvertEnum( pType, temp_d3d10_counter_type );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::GetCreationFlags()
////////////////////////////////////////////////////////////
UINT C10Device::GetCreationFlags(  )
{
    assert( m_pDevice );
    UINT rval = 0;
    rval = m_pDevice->GetCreationFlags(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::OpenSharedResource()
////////////////////////////////////////////////////////////
NRESULT C10Device::OpenSharedResource(  HANDLE hResource, REFIID ReturnedInterface, void ** ppResource )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    rval = m_pDevice->OpenSharedResource(  hResource,  ReturnedInterface,  ppResource );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device::SetTextFilterSize()
////////////////////////////////////////////////////////////
void C10Device::SetTextFilterSize(  UINT Width, UINT Height )
{
    assert( m_pDevice );
    m_pDevice->SetTextFilterSize(  Width,  Height );
}


////////////////////////////////////////////////////////////
// C10Device::GetTextFilterSize()
////////////////////////////////////////////////////////////
void C10Device::GetTextFilterSize(  UINT * pWidth, UINT * pHeight )
{
    assert( m_pDevice );
    m_pDevice->GetTextFilterSize(  pWidth,  pHeight );
}


////////////////////////////////////////////////////////////
//
// C10Multithread
//
////////////////////////////////////////////////////////////

C10Multithread::C10Multithread( ID3D10Multithread *pMultithread ) : 
    m_pMultithread( pMultithread )
{
}


C10Multithread::~C10Multithread()
{
    if( m_pMultithread ) m_pMultithread->Release();
}


////////////////////////////////////////////////////////////
// C10Multithread::Enter()
////////////////////////////////////////////////////////////
void C10Multithread::Enter(  )
{
    assert( m_pMultithread );
    m_pMultithread->Enter(  );
}


////////////////////////////////////////////////////////////
// C10Multithread::Leave()
////////////////////////////////////////////////////////////
void C10Multithread::Leave(  )
{
    assert( m_pMultithread );
    m_pMultithread->Leave(  );
}


////////////////////////////////////////////////////////////
// C10Multithread::SetMultithreadProtected()
////////////////////////////////////////////////////////////
BOOL C10Multithread::SetMultithreadProtected(  BOOL bMTProtect )
{
    assert( m_pMultithread );
    BOOL rval = 0;
    rval = m_pMultithread->SetMultithreadProtected(  bMTProtect );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Multithread::GetMultithreadProtected()
////////////////////////////////////////////////////////////
BOOL C10Multithread::GetMultithreadProtected(  )
{
    assert( m_pMultithread );
    BOOL rval = 0;
    rval = m_pMultithread->GetMultithreadProtected(  );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10Debug
//
////////////////////////////////////////////////////////////

C10Debug::C10Debug( ID3D10Debug *pDebug ) : 
    m_pDebug( pDebug )
{
}


C10Debug::~C10Debug()
{
    if( m_pDebug ) m_pDebug->Release();
}


////////////////////////////////////////////////////////////
// C10Debug::SetFeatureMask()
////////////////////////////////////////////////////////////
NRESULT C10Debug::SetFeatureMask(  UINT Mask )
{
    assert( m_pDebug );
    NRESULT rval = S_OK;
    rval = m_pDebug->SetFeatureMask(  Mask );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Debug::GetFeatureMask()
////////////////////////////////////////////////////////////
UINT C10Debug::GetFeatureMask(  )
{
    assert( m_pDebug );
    UINT rval = 0;
    rval = m_pDebug->GetFeatureMask(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Debug::SetPresentPerRenderOpDelay()
////////////////////////////////////////////////////////////
NRESULT C10Debug::SetPresentPerRenderOpDelay(  UINT Milliseconds )
{
    assert( m_pDebug );
    NRESULT rval = S_OK;
    rval = m_pDebug->SetPresentPerRenderOpDelay(  Milliseconds );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Debug::GetPresentPerRenderOpDelay()
////////////////////////////////////////////////////////////
UINT C10Debug::GetPresentPerRenderOpDelay(  )
{
    assert( m_pDebug );
    UINT rval = 0;
    rval = m_pDebug->GetPresentPerRenderOpDelay(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Debug::SetSwapChain()
////////////////////////////////////////////////////////////
NRESULT C10Debug::SetSwapChain(  IDXGISwapChain * pSwapChain )
{
    assert( m_pDebug );
    NRESULT rval = S_OK;
    rval = m_pDebug->SetSwapChain(  pSwapChain );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Debug::GetSwapChain()
////////////////////////////////////////////////////////////
NRESULT C10Debug::GetSwapChain(  IDXGISwapChain ** ppSwapChain )
{
    assert( m_pDebug );
    NRESULT rval = S_OK;
    rval = m_pDebug->GetSwapChain(  ppSwapChain );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Debug::Validate()
////////////////////////////////////////////////////////////
NRESULT C10Debug::Validate(  )
{
    assert( m_pDebug );
    NRESULT rval = S_OK;
    rval = m_pDebug->Validate(  );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10SwitchToRef
//
////////////////////////////////////////////////////////////

C10SwitchToRef::C10SwitchToRef( ID3D10SwitchToRef *pSwitchToRef ) : 
    m_pSwitchToRef( pSwitchToRef )
{
}


C10SwitchToRef::~C10SwitchToRef()
{
    if( m_pSwitchToRef ) m_pSwitchToRef->Release();
}


////////////////////////////////////////////////////////////
// C10SwitchToRef::SetUseRef()
////////////////////////////////////////////////////////////
BOOL C10SwitchToRef::SetUseRef(  BOOL UseRef )
{
    assert( m_pSwitchToRef );
    BOOL rval = 0;
    rval = m_pSwitchToRef->SetUseRef(  UseRef );
    return rval;
}


////////////////////////////////////////////////////////////
// C10SwitchToRef::GetUseRef()
////////////////////////////////////////////////////////////
BOOL C10SwitchToRef::GetUseRef(  )
{
    assert( m_pSwitchToRef );
    BOOL rval = 0;
    rval = m_pSwitchToRef->GetUseRef(  );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10InfoQueue
//
////////////////////////////////////////////////////////////

C10InfoQueue::C10InfoQueue( ID3D10InfoQueue *pInfoQueue ) : 
    m_pInfoQueue( pInfoQueue )
{
}


C10InfoQueue::~C10InfoQueue()
{
    if( m_pInfoQueue ) m_pInfoQueue->Release();
}


////////////////////////////////////////////////////////////
// C10InfoQueue::SetMessageCountLimit()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::SetMessageCountLimit(  UINT64 MessageCountLimit )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    rval = m_pInfoQueue->SetMessageCountLimit(  MessageCountLimit );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::ClearStoredMessages()
////////////////////////////////////////////////////////////
void C10InfoQueue::ClearStoredMessages(  )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->ClearStoredMessages(  );
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetMessage()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::GetMessage(  UINT64 MessageIndex, D3D_MESSAGE * pMessage, SIZE_T * pMessageByteLength )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pMessageByteTemp = NULL;
    D3D10_MESSAGE *pMessageTemp = NULL;
    SIZE_T pMessageSizeBuffer = *pMessageByteLength;
    if( pMessage != NULL && pMessageSizeBuffer != 0 )
    {
        pMessageByteTemp = new BYTE[pMessageSizeBuffer];
        pMessageTemp = (D3D10_MESSAGE*)pMessageByteTemp;
    }
    rval = m_pInfoQueue->GetMessage(  MessageIndex,  pMessageTemp,  pMessageByteLength );
    if( pMessage != NULL && pMessageSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pMessage, pMessageTemp );
        delete[] pMessageByteTemp;
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetNumMessagesAllowedByStorageFilter()
////////////////////////////////////////////////////////////
UINT64 C10InfoQueue::GetNumMessagesAllowedByStorageFilter(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetNumMessagesAllowedByStorageFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetNumMessagesDeniedByStorageFilter()
////////////////////////////////////////////////////////////
UINT64 C10InfoQueue::GetNumMessagesDeniedByStorageFilter(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetNumMessagesDeniedByStorageFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetNumStoredMessages()
////////////////////////////////////////////////////////////
UINT64 C10InfoQueue::GetNumStoredMessages(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetNumStoredMessages(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetNumStoredMessagesAllowedByRetrievalFilter()
////////////////////////////////////////////////////////////
UINT64 C10InfoQueue::GetNumStoredMessagesAllowedByRetrievalFilter(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetNumMessagesDiscardedByMessageCountLimit()
////////////////////////////////////////////////////////////
UINT64 C10InfoQueue::GetNumMessagesDiscardedByMessageCountLimit(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetNumMessagesDiscardedByMessageCountLimit(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetMessageCountLimit()
////////////////////////////////////////////////////////////
UINT64 C10InfoQueue::GetMessageCountLimit(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetMessageCountLimit(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::AddStorageFilterEntries()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::AddStorageFilterEntries(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D10_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = sizeof(D3D10_INFO_QUEUE_FILTER);
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D10_INFO_QUEUE_FILTER*)pFilterByteTemp;
        D3DWrapperConvertStruct( pFilterTemp, pFilter );
    }
    rval = m_pInfoQueue->AddStorageFilterEntries(  pFilterTemp );
    delete[] pFilterByteTemp;
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetStorageFilter()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::GetStorageFilter(  D3D_INFO_QUEUE_FILTER * pFilter, SIZE_T * pFilterByteLength )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D10_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = *pFilterByteLength;
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D10_INFO_QUEUE_FILTER*)pFilterByteTemp;
    }
    rval = m_pInfoQueue->GetStorageFilter(  pFilterTemp,  pFilterByteLength );
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pFilter, pFilterTemp );
        delete[] pFilterByteTemp;
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::ClearStorageFilter()
////////////////////////////////////////////////////////////
void C10InfoQueue::ClearStorageFilter(  )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->ClearStorageFilter(  );
}


////////////////////////////////////////////////////////////
// C10InfoQueue::PushEmptyStorageFilter()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::PushEmptyStorageFilter(  )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    rval = m_pInfoQueue->PushEmptyStorageFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::PushCopyOfStorageFilter()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::PushCopyOfStorageFilter(  )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    rval = m_pInfoQueue->PushCopyOfStorageFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::PushStorageFilter()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::PushStorageFilter(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D10_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = sizeof(D3D10_INFO_QUEUE_FILTER);
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D10_INFO_QUEUE_FILTER*)pFilterByteTemp;
        D3DWrapperConvertStruct( pFilterTemp, pFilter );
    }
    rval = m_pInfoQueue->PushStorageFilter(  pFilterTemp );
    delete[] pFilterByteTemp;
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::PopStorageFilter()
////////////////////////////////////////////////////////////
void C10InfoQueue::PopStorageFilter(  )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->PopStorageFilter(  );
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetStorageFilterStackSize()
////////////////////////////////////////////////////////////
UINT C10InfoQueue::GetStorageFilterStackSize(  )
{
    assert( m_pInfoQueue );
    UINT rval = 0;
    rval = m_pInfoQueue->GetStorageFilterStackSize(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::AddRetrievalFilterEntries()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::AddRetrievalFilterEntries(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D10_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = sizeof(D3D10_INFO_QUEUE_FILTER);
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D10_INFO_QUEUE_FILTER*)pFilterByteTemp;
        D3DWrapperConvertStruct( pFilterTemp, pFilter );
    }
    rval = m_pInfoQueue->AddRetrievalFilterEntries(  pFilterTemp );
    delete[] pFilterByteTemp;
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::GetRetrievalFilter(  D3D_INFO_QUEUE_FILTER * pFilter, SIZE_T * pFilterByteLength )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D10_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = *pFilterByteLength;
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D10_INFO_QUEUE_FILTER*)pFilterByteTemp;
    }
    rval = m_pInfoQueue->GetRetrievalFilter(  pFilterTemp,  pFilterByteLength );
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pFilter, pFilterTemp );
        delete[] pFilterByteTemp;
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::ClearRetrievalFilter()
////////////////////////////////////////////////////////////
void C10InfoQueue::ClearRetrievalFilter(  )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->ClearRetrievalFilter(  );
}


////////////////////////////////////////////////////////////
// C10InfoQueue::PushEmptyRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::PushEmptyRetrievalFilter(  )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    rval = m_pInfoQueue->PushEmptyRetrievalFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::PushCopyOfRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::PushCopyOfRetrievalFilter(  )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    rval = m_pInfoQueue->PushCopyOfRetrievalFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::PushRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::PushRetrievalFilter(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D10_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = sizeof(D3D10_INFO_QUEUE_FILTER);
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D10_INFO_QUEUE_FILTER*)pFilterByteTemp;
        D3DWrapperConvertStruct( pFilterTemp, pFilter );
    }
    rval = m_pInfoQueue->PushRetrievalFilter(  pFilterTemp );
    delete[] pFilterByteTemp;
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::PopRetrievalFilter()
////////////////////////////////////////////////////////////
void C10InfoQueue::PopRetrievalFilter(  )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->PopRetrievalFilter(  );
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetRetrievalFilterStackSize()
////////////////////////////////////////////////////////////
UINT C10InfoQueue::GetRetrievalFilterStackSize(  )
{
    assert( m_pInfoQueue );
    UINT rval = 0;
    rval = m_pInfoQueue->GetRetrievalFilterStackSize(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::AddMessage()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::AddMessage(  D3D_MESSAGE_CATEGORY Category, D3D_MESSAGE_SEVERITY Severity, D3D_MESSAGE_ID ID, LPCSTR pDescription )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    D3D10_MESSAGE_CATEGORY temp_d3d10_message_category;
    D3DWrapperConvertEnum( &temp_d3d10_message_category, Category );
    D3D10_MESSAGE_SEVERITY temp_d3d10_message_severity;
    D3DWrapperConvertEnum( &temp_d3d10_message_severity, Severity );
    D3D10_MESSAGE_ID temp_d3d10_message_id;
    D3DWrapperConvertEnum( &temp_d3d10_message_id, ID );
    rval = m_pInfoQueue->AddMessage(  temp_d3d10_message_category,  temp_d3d10_message_severity,  temp_d3d10_message_id,  pDescription );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::AddApplicationMessage()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::AddApplicationMessage(  D3D_MESSAGE_SEVERITY Severity, LPCSTR pDescription )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    D3D10_MESSAGE_SEVERITY temp_d3d10_message_severity;
    D3DWrapperConvertEnum( &temp_d3d10_message_severity, Severity );
    rval = m_pInfoQueue->AddApplicationMessage(  temp_d3d10_message_severity,  pDescription );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::SetBreakOnCategory()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::SetBreakOnCategory(  D3D_MESSAGE_CATEGORY Category, BOOL bEnable )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    D3D10_MESSAGE_CATEGORY temp_d3d10_message_category;
    D3DWrapperConvertEnum( &temp_d3d10_message_category, Category );
    rval = m_pInfoQueue->SetBreakOnCategory(  temp_d3d10_message_category,  bEnable );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::SetBreakOnSeverity()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::SetBreakOnSeverity(  D3D_MESSAGE_SEVERITY Severity, BOOL bEnable )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    D3D10_MESSAGE_SEVERITY temp_d3d10_message_severity;
    D3DWrapperConvertEnum( &temp_d3d10_message_severity, Severity );
    rval = m_pInfoQueue->SetBreakOnSeverity(  temp_d3d10_message_severity,  bEnable );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::SetBreakOnID()
////////////////////////////////////////////////////////////
NRESULT C10InfoQueue::SetBreakOnID(  D3D_MESSAGE_ID ID, BOOL bEnable )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    D3D10_MESSAGE_ID temp_d3d10_message_id;
    D3DWrapperConvertEnum( &temp_d3d10_message_id, ID );
    rval = m_pInfoQueue->SetBreakOnID(  temp_d3d10_message_id,  bEnable );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetBreakOnCategory()
////////////////////////////////////////////////////////////
BOOL C10InfoQueue::GetBreakOnCategory(  D3D_MESSAGE_CATEGORY Category )
{
    assert( m_pInfoQueue );
    BOOL rval = 0;
    D3D10_MESSAGE_CATEGORY temp_d3d10_message_category;
    D3DWrapperConvertEnum( &temp_d3d10_message_category, Category );
    rval = m_pInfoQueue->GetBreakOnCategory(  temp_d3d10_message_category );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetBreakOnSeverity()
////////////////////////////////////////////////////////////
BOOL C10InfoQueue::GetBreakOnSeverity(  D3D_MESSAGE_SEVERITY Severity )
{
    assert( m_pInfoQueue );
    BOOL rval = 0;
    D3D10_MESSAGE_SEVERITY temp_d3d10_message_severity;
    D3DWrapperConvertEnum( &temp_d3d10_message_severity, Severity );
    rval = m_pInfoQueue->GetBreakOnSeverity(  temp_d3d10_message_severity );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetBreakOnID()
////////////////////////////////////////////////////////////
BOOL C10InfoQueue::GetBreakOnID(  D3D_MESSAGE_ID ID )
{
    assert( m_pInfoQueue );
    BOOL rval = 0;
    D3D10_MESSAGE_ID temp_d3d10_message_id;
    D3DWrapperConvertEnum( &temp_d3d10_message_id, ID );
    rval = m_pInfoQueue->GetBreakOnID(  temp_d3d10_message_id );
    return rval;
}


////////////////////////////////////////////////////////////
// C10InfoQueue::SetMuteDebugOutput()
////////////////////////////////////////////////////////////
void C10InfoQueue::SetMuteDebugOutput(  BOOL bMute )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->SetMuteDebugOutput(  bMute );
}


////////////////////////////////////////////////////////////
// C10InfoQueue::GetMuteDebugOutput()
////////////////////////////////////////////////////////////
BOOL C10InfoQueue::GetMuteDebugOutput(  )
{
    assert( m_pInfoQueue );
    BOOL rval = 0;
    rval = m_pInfoQueue->GetMuteDebugOutput(  );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10Blob
//
////////////////////////////////////////////////////////////

C10Blob::C10Blob( ID3D10Blob *pBlob ) : 
    m_pBlob( pBlob )
{
}


C10Blob::~C10Blob()
{
    if( m_pBlob ) m_pBlob->Release();
}


////////////////////////////////////////////////////////////
// C10Blob::QueryInterface()
////////////////////////////////////////////////////////////
NRESULT C10Blob::QueryInterface(  REFIID iid, LPVOID * ppv )
{
    assert( m_pBlob );
    NRESULT rval = S_OK;
    rval = m_pBlob->QueryInterface(  iid,  ppv );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Blob::GetBufferPointer()
////////////////////////////////////////////////////////////
LPVOID C10Blob::GetBufferPointer(  )
{
    assert( m_pBlob );
    LPVOID rval = 0;
    rval = m_pBlob->GetBufferPointer(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Blob::GetBufferSize()
////////////////////////////////////////////////////////////
SIZE_T C10Blob::GetBufferSize(  )
{
    assert( m_pBlob );
    SIZE_T rval = 0;
    rval = m_pBlob->GetBufferSize(  );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10BlendState1
//
////////////////////////////////////////////////////////////

C10BlendState1::C10BlendState1( ID3D10BlendState1 *pBlendState1 ) : 
    C10BlendState( pBlendState1 ),
    m_pBlendState1( pBlendState1 )
{
}


C10BlendState1::~C10BlendState1()
{
}


////////////////////////////////////////////////////////////
// C10BlendState1::GetDesc1()
////////////////////////////////////////////////////////////
void C10BlendState1::GetDesc1(  D3D_BLEND_DESC1 * pDesc )
{
    assert( m_pBlendState1 );
    BYTE *pDescByteTemp = NULL;
    D3D10_BLEND_DESC1 *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_BLEND_DESC1);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_BLEND_DESC1*)pDescByteTemp;
    }
    m_pBlendState1->GetDesc1(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10BlendState1::GetDesc()
////////////////////////////////////////////////////////////
void C10BlendState1::GetDesc(  D3D_BLEND_DESC * pDesc )
{
    assert( m_pBlendState1 );
    BYTE *pDescByteTemp = NULL;
    D3D10_BLEND_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_BLEND_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_BLEND_DESC*)pDescByteTemp;
    }
    m_pBlendState1->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10BlendState1::GetDevice()
////////////////////////////////////////////////////////////
void C10BlendState1::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pBlendState1 );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pBlendState1->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10BlendState1::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10BlendState1::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pBlendState1 );
    NRESULT rval = S_OK;
    rval = m_pBlendState1->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10BlendState1::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10BlendState1::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pBlendState1 );
    NRESULT rval = S_OK;
    rval = m_pBlendState1->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10BlendState1::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10BlendState1::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pBlendState1 );
    NRESULT rval = S_OK;
    rval = m_pBlendState1->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10ShaderResourceView1
//
////////////////////////////////////////////////////////////

C10ShaderResourceView1::C10ShaderResourceView1( ID3D10ShaderResourceView1 *pShaderResourceView1 ) : 
    C10ShaderResourceView( pShaderResourceView1 ),
    m_pShaderResourceView1( pShaderResourceView1 )
{
}


C10ShaderResourceView1::~C10ShaderResourceView1()
{
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView1::GetDesc1()
////////////////////////////////////////////////////////////
void C10ShaderResourceView1::GetDesc1(  D3D_SHADER_RESOURCE_VIEW_DESC1 * pDesc )
{
    assert( m_pShaderResourceView1 );
    BYTE *pDescByteTemp = NULL;
    D3D10_SHADER_RESOURCE_VIEW_DESC1 *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC1);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_SHADER_RESOURCE_VIEW_DESC1*)pDescByteTemp;
    }
    m_pShaderResourceView1->GetDesc1(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView1::GetDesc()
////////////////////////////////////////////////////////////
void C10ShaderResourceView1::GetDesc(  D3D_SHADER_RESOURCE_VIEW_DESC * pDesc )
{
    assert( m_pShaderResourceView1 );
    BYTE *pDescByteTemp = NULL;
    D3D10_SHADER_RESOURCE_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_SHADER_RESOURCE_VIEW_DESC*)pDescByteTemp;
    }
    m_pShaderResourceView1->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView1::GetResource()
////////////////////////////////////////////////////////////
void C10ShaderResourceView1::GetResource(  CResource ** ppResource )
{
    assert( m_pShaderResourceView1 );
    ID3D10Resource *temp_pResource = NULL;
    ID3D10Resource **ppResourceTemp = ppResource == NULL ? NULL : &temp_pResource;
    m_pShaderResourceView1->GetResource(  ppResourceTemp );
    if( ppResource != NULL )
    {
        D3DWrapperConvertInterface( ppResource, *ppResourceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView1::GetDevice()
////////////////////////////////////////////////////////////
void C10ShaderResourceView1::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pShaderResourceView1 );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pShaderResourceView1->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView1::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10ShaderResourceView1::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pShaderResourceView1 );
    NRESULT rval = S_OK;
    rval = m_pShaderResourceView1->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView1::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10ShaderResourceView1::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pShaderResourceView1 );
    NRESULT rval = S_OK;
    rval = m_pShaderResourceView1->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10ShaderResourceView1::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10ShaderResourceView1::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pShaderResourceView1 );
    NRESULT rval = S_OK;
    rval = m_pShaderResourceView1->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C10Device1
//
////////////////////////////////////////////////////////////

C10Device1::C10Device1( ID3D10Device1 *pDevice1 ) : 
    C10Device( pDevice1 ),
    m_pDevice1( pDevice1 )
{
}


C10Device1::~C10Device1()
{
}


////////////////////////////////////////////////////////////
// C10Device1::CreateShaderResourceView1()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateShaderResourceView1(  CResource * pResource, const D3D_SHADER_RESOURCE_VIEW_DESC1 * pDesc, CShaderResourceView1 ** ppSRView )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    ID3D10Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    BYTE *pDescByteTemp = NULL;
    D3D10_SHADER_RESOURCE_VIEW_DESC1 *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC1);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_SHADER_RESOURCE_VIEW_DESC1*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    ID3D10ShaderResourceView1 *temp_pSRView = NULL;
    ID3D10ShaderResourceView1 **ppSRViewTemp = ppSRView == NULL ? NULL : &temp_pSRView;
    rval = m_pDevice1->CreateShaderResourceView1(  temp_pResource,  pDescTemp,  ppSRViewTemp );
    delete[] pDescByteTemp;
    if( ppSRView != NULL )
    {
        D3DWrapperConvertInterface( ppSRView, *ppSRViewTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateBlendState1()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateBlendState1(  const D3D_BLEND_DESC1 * pBlendStateDesc, CBlendState1 ** ppBlendState )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pBlendStateDescByteTemp = NULL;
    D3D10_BLEND_DESC1 *pBlendStateDescTemp = NULL;
    SIZE_T pBlendStateDescSizeBuffer = sizeof(D3D10_BLEND_DESC1);
    if( pBlendStateDesc != NULL && pBlendStateDescSizeBuffer != 0 )
    {
        pBlendStateDescByteTemp = new BYTE[pBlendStateDescSizeBuffer];
        pBlendStateDescTemp = (D3D10_BLEND_DESC1*)pBlendStateDescByteTemp;
        D3DWrapperConvertStruct( pBlendStateDescTemp, pBlendStateDesc );
    }
    ID3D10BlendState1 *temp_pBlendState = NULL;
    ID3D10BlendState1 **ppBlendStateTemp = ppBlendState == NULL ? NULL : &temp_pBlendState;
    rval = m_pDevice1->CreateBlendState1(  pBlendStateDescTemp,  ppBlendStateTemp );
    delete[] pBlendStateDescByteTemp;
    if( ppBlendState != NULL )
    {
        D3DWrapperConvertInterface( ppBlendState, *ppBlendStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::GetFeatureLevel()
////////////////////////////////////////////////////////////
D3D_FEATURE_LEVEL1 C10Device1::GetFeatureLevel(  )
{
    assert( m_pDevice1 );
    D3D_FEATURE_LEVEL1 rval = (D3D_FEATURE_LEVEL1)0;
    D3D10_FEATURE_LEVEL1 temp = m_pDevice1->GetFeatureLevel(  );
    D3DWrapperConvertEnum( &rval, temp );    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::VSSetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device1::VSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    assert( m_pDevice1 );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffers[j] )
            ppConstantBuffers[j]->GetInterfacePtr( &ppConstantBuffersConv[j] );
        else
            ppConstantBuffersConv[j] = NULL;
    }
    m_pDevice1->VSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::PSSetShaderResources()
////////////////////////////////////////////////////////////
void C10Device1::PSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    assert( m_pDevice1 );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViews[j] )
            ppShaderResourceViews[j]->GetInterfacePtr( &ppShaderResourceViewsConv[j] );
        else
            ppShaderResourceViewsConv[j] = NULL;
    }
    m_pDevice1->PSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device1::PSSetShader()
////////////////////////////////////////////////////////////
void C10Device1::PSSetShader(  CPixelShader * pPixelShader )
{
    assert( m_pDevice1 );
    ID3D10PixelShader *temp_pPixelShader = NULL;
    if( pPixelShader != NULL )
    {
        D3DWrapperConvertInterface( &temp_pPixelShader, pPixelShader );
    }
    m_pDevice1->PSSetShader(  temp_pPixelShader );
}


////////////////////////////////////////////////////////////
// C10Device1::PSSetSamplers()
////////////////////////////////////////////////////////////
void C10Device1::PSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    assert( m_pDevice1 );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplers[j] )
            ppSamplers[j]->GetInterfacePtr( &ppSamplersConv[j] );
        else
            ppSamplersConv[j] = NULL;
    }
    m_pDevice1->PSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::VSSetShader()
////////////////////////////////////////////////////////////
void C10Device1::VSSetShader(  CVertexShader * pVertexShader )
{
    assert( m_pDevice1 );
    ID3D10VertexShader *temp_pVertexShader = NULL;
    if( pVertexShader != NULL )
    {
        D3DWrapperConvertInterface( &temp_pVertexShader, pVertexShader );
    }
    m_pDevice1->VSSetShader(  temp_pVertexShader );
}


////////////////////////////////////////////////////////////
// C10Device1::DrawIndexed()
////////////////////////////////////////////////////////////
void C10Device1::DrawIndexed(  UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation )
{
    assert( m_pDevice1 );
    m_pDevice1->DrawIndexed(  IndexCount,  StartIndexLocation,  BaseVertexLocation );
}


////////////////////////////////////////////////////////////
// C10Device1::Draw()
////////////////////////////////////////////////////////////
void C10Device1::Draw(  UINT VertexCount, UINT StartVertexLocation )
{
    assert( m_pDevice1 );
    m_pDevice1->Draw(  VertexCount,  StartVertexLocation );
}


////////////////////////////////////////////////////////////
// C10Device1::PSSetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device1::PSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    assert( m_pDevice1 );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffers[j] )
            ppConstantBuffers[j]->GetInterfacePtr( &ppConstantBuffersConv[j] );
        else
            ppConstantBuffersConv[j] = NULL;
    }
    m_pDevice1->PSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::IASetInputLayout()
////////////////////////////////////////////////////////////
void C10Device1::IASetInputLayout(  CInputLayout * pInputLayout )
{
    assert( m_pDevice1 );
    ID3D10InputLayout *temp_pInputLayout = NULL;
    if( pInputLayout != NULL )
    {
        D3DWrapperConvertInterface( &temp_pInputLayout, pInputLayout );
    }
    m_pDevice1->IASetInputLayout(  temp_pInputLayout );
}


////////////////////////////////////////////////////////////
// C10Device1::IASetVertexBuffers()
////////////////////////////////////////////////////////////
void C10Device1::IASetVertexBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppVertexBuffers, const UINT * pStrides, const UINT * pOffsets )
{
    assert( m_pDevice1 );
    ID3D10Buffer **ppVertexBuffersConv = ppVertexBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppVertexBuffers && !ppVertexBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppVertexBuffers[j] )
            ppVertexBuffers[j]->GetInterfacePtr( &ppVertexBuffersConv[j] );
        else
            ppVertexBuffersConv[j] = NULL;
    }
    m_pDevice1->IASetVertexBuffers(  StartSlot,  NumBuffers,  ppVertexBuffersConv,  pStrides,  pOffsets );
    delete[]( ppVertexBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::IASetIndexBuffer()
////////////////////////////////////////////////////////////
void C10Device1::IASetIndexBuffer(  CBuffer * pIndexBuffer, DXGI_FORMAT Format, UINT Offset )
{
    assert( m_pDevice1 );
    ID3D10Buffer *temp_pIndexBuffer = NULL;
    if( pIndexBuffer != NULL )
    {
        D3DWrapperConvertInterface( &temp_pIndexBuffer, pIndexBuffer );
    }
    m_pDevice1->IASetIndexBuffer(  temp_pIndexBuffer,  Format,  Offset );
}


////////////////////////////////////////////////////////////
// C10Device1::DrawIndexedInstanced()
////////////////////////////////////////////////////////////
void C10Device1::DrawIndexedInstanced(  UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation )
{
    assert( m_pDevice1 );
    m_pDevice1->DrawIndexedInstanced(  IndexCountPerInstance,  InstanceCount,  StartIndexLocation,  BaseVertexLocation,  StartInstanceLocation );
}


////////////////////////////////////////////////////////////
// C10Device1::DrawInstanced()
////////////////////////////////////////////////////////////
void C10Device1::DrawInstanced(  UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation )
{
    assert( m_pDevice1 );
    m_pDevice1->DrawInstanced(  VertexCountPerInstance,  InstanceCount,  StartVertexLocation,  StartInstanceLocation );
}


////////////////////////////////////////////////////////////
// C10Device1::GSSetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device1::GSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    assert( m_pDevice1 );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffers[j] )
            ppConstantBuffers[j]->GetInterfacePtr( &ppConstantBuffersConv[j] );
        else
            ppConstantBuffersConv[j] = NULL;
    }
    m_pDevice1->GSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::GSSetShader()
////////////////////////////////////////////////////////////
void C10Device1::GSSetShader(  CGeometryShader * pShader )
{
    assert( m_pDevice1 );
    ID3D10GeometryShader *temp_pShader = NULL;
    if( pShader != NULL )
    {
        D3DWrapperConvertInterface( &temp_pShader, pShader );
    }
    m_pDevice1->GSSetShader(  temp_pShader );
}


////////////////////////////////////////////////////////////
// C10Device1::IASetPrimitiveTopology()
////////////////////////////////////////////////////////////
void C10Device1::IASetPrimitiveTopology(  D3D_PRIMITIVE_TOPOLOGY Topology )
{
    assert( m_pDevice1 );
    D3D10_PRIMITIVE_TOPOLOGY temp_d3d10_primitive_topology;
    D3DWrapperConvertEnum( &temp_d3d10_primitive_topology, Topology );
    m_pDevice1->IASetPrimitiveTopology(  temp_d3d10_primitive_topology );
}


////////////////////////////////////////////////////////////
// C10Device1::VSSetShaderResources()
////////////////////////////////////////////////////////////
void C10Device1::VSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    assert( m_pDevice1 );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViews[j] )
            ppShaderResourceViews[j]->GetInterfacePtr( &ppShaderResourceViewsConv[j] );
        else
            ppShaderResourceViewsConv[j] = NULL;
    }
    m_pDevice1->VSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device1::VSSetSamplers()
////////////////////////////////////////////////////////////
void C10Device1::VSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    assert( m_pDevice1 );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplers[j] )
            ppSamplers[j]->GetInterfacePtr( &ppSamplersConv[j] );
        else
            ppSamplersConv[j] = NULL;
    }
    m_pDevice1->VSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::SetPredication()
////////////////////////////////////////////////////////////
void C10Device1::SetPredication(  CPredicate * pPredicate, BOOL PredicateValue )
{
    assert( m_pDevice1 );
    ID3D10Predicate *temp_pPredicate = NULL;
    if( pPredicate != NULL )
    {
        D3DWrapperConvertInterface( &temp_pPredicate, pPredicate );
    }
    m_pDevice1->SetPredication(  temp_pPredicate,  PredicateValue );
}


////////////////////////////////////////////////////////////
// C10Device1::GSSetShaderResources()
////////////////////////////////////////////////////////////
void C10Device1::GSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    assert( m_pDevice1 );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViews[j] )
            ppShaderResourceViews[j]->GetInterfacePtr( &ppShaderResourceViewsConv[j] );
        else
            ppShaderResourceViewsConv[j] = NULL;
    }
    m_pDevice1->GSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device1::GSSetSamplers()
////////////////////////////////////////////////////////////
void C10Device1::GSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    assert( m_pDevice1 );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplers[j] )
            ppSamplers[j]->GetInterfacePtr( &ppSamplersConv[j] );
        else
            ppSamplersConv[j] = NULL;
    }
    m_pDevice1->GSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::OMSetRenderTargets()
////////////////////////////////////////////////////////////
void C10Device1::OMSetRenderTargets(  UINT NumViews, CRenderTargetView *const * ppRenderTargetViews, CDepthStencilView * pDepthStencilView )
{
    assert( m_pDevice1 );
    ID3D10RenderTargetView **ppRenderTargetViewsConv = ppRenderTargetViews == NULL ? NULL : new ID3D10RenderTargetView*[NumViews];
    if( ppRenderTargetViews && !ppRenderTargetViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    ID3D10DepthStencilView *temp_pDepthStencilView = NULL;
    if( pDepthStencilView != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDepthStencilView, pDepthStencilView );
    }
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppRenderTargetViews[j] )
            ppRenderTargetViews[j]->GetInterfacePtr( &ppRenderTargetViewsConv[j] );
        else
            ppRenderTargetViewsConv[j] = NULL;
    }
    m_pDevice1->OMSetRenderTargets(  NumViews,  ppRenderTargetViewsConv,  temp_pDepthStencilView );
    delete[]( ppRenderTargetViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device1::OMSetBlendState()
////////////////////////////////////////////////////////////
void C10Device1::OMSetBlendState(  CBlendState * pBlendState, const FLOAT BlendFactor[4], UINT SampleMask )
{
    assert( m_pDevice1 );
    ID3D10BlendState *temp_pBlendState = NULL;
    if( pBlendState != NULL )
    {
        D3DWrapperConvertInterface( &temp_pBlendState, pBlendState );
    }
    m_pDevice1->OMSetBlendState(  temp_pBlendState,  BlendFactor,  SampleMask );
}


////////////////////////////////////////////////////////////
// C10Device1::OMSetDepthStencilState()
////////////////////////////////////////////////////////////
void C10Device1::OMSetDepthStencilState(  CDepthStencilState * pDepthStencilState, UINT StencilRef )
{
    assert( m_pDevice1 );
    ID3D10DepthStencilState *temp_pDepthStencilState = NULL;
    if( pDepthStencilState != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDepthStencilState, pDepthStencilState );
    }
    m_pDevice1->OMSetDepthStencilState(  temp_pDepthStencilState,  StencilRef );
}


////////////////////////////////////////////////////////////
// C10Device1::SOSetTargets()
////////////////////////////////////////////////////////////
void C10Device1::SOSetTargets(  UINT NumBuffers, CBuffer *const * ppSOTargets, const UINT * pOffsets )
{
    assert( m_pDevice1 );
    ID3D10Buffer **ppSOTargetsConv = ppSOTargets == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppSOTargets && !ppSOTargetsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppSOTargets[j] )
            ppSOTargets[j]->GetInterfacePtr( &ppSOTargetsConv[j] );
        else
            ppSOTargetsConv[j] = NULL;
    }
    m_pDevice1->SOSetTargets(  NumBuffers,  ppSOTargetsConv,  pOffsets );
    delete[]( ppSOTargetsConv );
}


////////////////////////////////////////////////////////////
// C10Device1::DrawAuto()
////////////////////////////////////////////////////////////
void C10Device1::DrawAuto(  )
{
    assert( m_pDevice1 );
    m_pDevice1->DrawAuto(  );
}


////////////////////////////////////////////////////////////
// C10Device1::RSSetState()
////////////////////////////////////////////////////////////
void C10Device1::RSSetState(  CRasterizerState * pRasterizerState )
{
    assert( m_pDevice1 );
    ID3D10RasterizerState *temp_pRasterizerState = NULL;
    if( pRasterizerState != NULL )
    {
        D3DWrapperConvertInterface( &temp_pRasterizerState, pRasterizerState );
    }
    m_pDevice1->RSSetState(  temp_pRasterizerState );
}


////////////////////////////////////////////////////////////
// C10Device1::RSSetViewports()
////////////////////////////////////////////////////////////
void C10Device1::RSSetViewports(  UINT NumViewports, const D3D_VIEWPORT * pViewports )
{
    assert( m_pDevice1 );
    D3D10_VIEWPORT *pViewportsConv = pViewports == NULL ? NULL : new D3D10_VIEWPORT[NumViewports];
    if( pViewports && !pViewportsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pViewports )
    {
        for( UINT i = NumViewports - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pViewportsConv[i], &pViewports[i] );
        }
    }
    m_pDevice1->RSSetViewports(  NumViewports,  pViewportsConv );
    delete[]( pViewportsConv );
}


////////////////////////////////////////////////////////////
// C10Device1::RSSetScissorRects()
////////////////////////////////////////////////////////////
void C10Device1::RSSetScissorRects(  UINT NumRects, const D3D_RECT * pRects )
{
    assert( m_pDevice1 );
    m_pDevice1->RSSetScissorRects(  NumRects,  pRects );
}


////////////////////////////////////////////////////////////
// C10Device1::CopySubresourceRegion()
////////////////////////////////////////////////////////////
void C10Device1::CopySubresourceRegion(  CResource * pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, CResource * pSrcResource, UINT SrcSubresource, const D3D_BOX * pSrcBox )
{
    assert( m_pDevice1 );
    ID3D10Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    ID3D10Resource *temp_pSrcResource = NULL;
    if( pSrcResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pSrcResource, pSrcResource );
    }
    BYTE *pSrcBoxByteTemp = NULL;
    D3D10_BOX *pSrcBoxTemp = NULL;
    SIZE_T pSrcBoxSizeBuffer = sizeof(D3D10_BOX);
    if( pSrcBox != NULL && pSrcBoxSizeBuffer != 0 )
    {
        pSrcBoxByteTemp = new BYTE[pSrcBoxSizeBuffer];
        pSrcBoxTemp = (D3D10_BOX*)pSrcBoxByteTemp;
        D3DWrapperConvertStruct( pSrcBoxTemp, pSrcBox );
    }
    m_pDevice1->CopySubresourceRegion(  temp_pDstResource,  DstSubresource,  DstX,  DstY,  DstZ,  temp_pSrcResource,  SrcSubresource,  pSrcBoxTemp );
    delete[] pSrcBoxByteTemp;
}


////////////////////////////////////////////////////////////
// C10Device1::CopyResource()
////////////////////////////////////////////////////////////
void C10Device1::CopyResource(  CResource * pDstResource, CResource * pSrcResource )
{
    assert( m_pDevice1 );
    ID3D10Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    ID3D10Resource *temp_pSrcResource = NULL;
    if( pSrcResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pSrcResource, pSrcResource );
    }
    m_pDevice1->CopyResource(  temp_pDstResource,  temp_pSrcResource );
}


////////////////////////////////////////////////////////////
// C10Device1::UpdateSubresource()
////////////////////////////////////////////////////////////
void C10Device1::UpdateSubresource(  CResource * pDstResource, UINT DstSubresource, const D3D_BOX * pDstBox, const void * pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch )
{
    assert( m_pDevice1 );
    ID3D10Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    BYTE *pDstBoxByteTemp = NULL;
    D3D10_BOX *pDstBoxTemp = NULL;
    SIZE_T pDstBoxSizeBuffer = sizeof(D3D10_BOX);
    if( pDstBox != NULL && pDstBoxSizeBuffer != 0 )
    {
        pDstBoxByteTemp = new BYTE[pDstBoxSizeBuffer];
        pDstBoxTemp = (D3D10_BOX*)pDstBoxByteTemp;
        D3DWrapperConvertStruct( pDstBoxTemp, pDstBox );
    }
    m_pDevice1->UpdateSubresource(  temp_pDstResource,  DstSubresource,  pDstBoxTemp,  pSrcData,  SrcRowPitch,  SrcDepthPitch );
    delete[] pDstBoxByteTemp;
}


////////////////////////////////////////////////////////////
// C10Device1::ClearRenderTargetView()
////////////////////////////////////////////////////////////
void C10Device1::ClearRenderTargetView(  CRenderTargetView * pRenderTargetView, const FLOAT ColorRGBA[4] )
{
    assert( m_pDevice1 );
    ID3D10RenderTargetView *temp_pRenderTargetView = NULL;
    if( pRenderTargetView != NULL )
    {
        D3DWrapperConvertInterface( &temp_pRenderTargetView, pRenderTargetView );
    }
    m_pDevice1->ClearRenderTargetView(  temp_pRenderTargetView,  ColorRGBA );
}


////////////////////////////////////////////////////////////
// C10Device1::ClearDepthStencilView()
////////////////////////////////////////////////////////////
void C10Device1::ClearDepthStencilView(  CDepthStencilView * pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil )
{
    assert( m_pDevice1 );
    ID3D10DepthStencilView *temp_pDepthStencilView = NULL;
    if( pDepthStencilView != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDepthStencilView, pDepthStencilView );
    }
    UINT temp_ClearFlags;
    D3DFlagsConvertTo10_CLEAR_FLAG( &temp_ClearFlags, ClearFlags );
    m_pDevice1->ClearDepthStencilView(  temp_pDepthStencilView,  temp_ClearFlags,  Depth,  Stencil );
}


////////////////////////////////////////////////////////////
// C10Device1::GenerateMips()
////////////////////////////////////////////////////////////
void C10Device1::GenerateMips(  CShaderResourceView * pShaderResourceView )
{
    assert( m_pDevice1 );
    ID3D10ShaderResourceView *temp_pShaderResourceView = NULL;
    if( pShaderResourceView != NULL )
    {
        D3DWrapperConvertInterface( &temp_pShaderResourceView, pShaderResourceView );
    }
    m_pDevice1->GenerateMips(  temp_pShaderResourceView );
}


////////////////////////////////////////////////////////////
// C10Device1::ResolveSubresource()
////////////////////////////////////////////////////////////
void C10Device1::ResolveSubresource(  CResource * pDstResource, UINT DstSubresource, CResource * pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format )
{
    assert( m_pDevice1 );
    ID3D10Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    ID3D10Resource *temp_pSrcResource = NULL;
    if( pSrcResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pSrcResource, pSrcResource );
    }
    m_pDevice1->ResolveSubresource(  temp_pDstResource,  DstSubresource,  temp_pSrcResource,  SrcSubresource,  Format );
}


////////////////////////////////////////////////////////////
// C10Device1::VSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device1::VSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice1 );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->VSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C10Buffer( ppConstantBuffersConv[j] );
            if( !ppConstantBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppConstantBuffers[j]->AddRef();
        }
        else
        {
            ppConstantBuffers[j] = NULL;
        }
    }
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::PSGetShaderResources()
////////////////////////////////////////////////////////////
void C10Device1::PSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice1 );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->PSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C10ShaderResourceView( ppShaderResourceViewsConv[j] );
            if( !ppShaderResourceViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppShaderResourceViews[j]->AddRef();
        }
        else
        {
            ppShaderResourceViews[j] = NULL;
        }
    }
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device1::PSGetShader()
////////////////////////////////////////////////////////////
void C10Device1::PSGetShader(  CPixelShader ** ppPixelShader )
{
    assert( m_pDevice1 );
    ID3D10PixelShader *temp_pPixelShader = NULL;
    ID3D10PixelShader **ppPixelShaderTemp = ppPixelShader == NULL ? NULL : &temp_pPixelShader;
    m_pDevice1->PSGetShader(  ppPixelShaderTemp );
    if( ppPixelShader != NULL )
    {
        D3DWrapperConvertInterface( ppPixelShader, *ppPixelShaderTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device1::PSGetSamplers()
////////////////////////////////////////////////////////////
void C10Device1::PSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice1 );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->PSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C10SamplerState( ppSamplersConv[j] );
            if( !ppSamplers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppSamplers[j]->AddRef();
        }
        else
        {
            ppSamplers[j] = NULL;
        }
    }
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::VSGetShader()
////////////////////////////////////////////////////////////
void C10Device1::VSGetShader(  CVertexShader ** ppVertexShader )
{
    assert( m_pDevice1 );
    ID3D10VertexShader *temp_pVertexShader = NULL;
    ID3D10VertexShader **ppVertexShaderTemp = ppVertexShader == NULL ? NULL : &temp_pVertexShader;
    m_pDevice1->VSGetShader(  ppVertexShaderTemp );
    if( ppVertexShader != NULL )
    {
        D3DWrapperConvertInterface( ppVertexShader, *ppVertexShaderTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device1::PSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device1::PSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice1 );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->PSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C10Buffer( ppConstantBuffersConv[j] );
            if( !ppConstantBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppConstantBuffers[j]->AddRef();
        }
        else
        {
            ppConstantBuffers[j] = NULL;
        }
    }
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::IAGetInputLayout()
////////////////////////////////////////////////////////////
void C10Device1::IAGetInputLayout(  CInputLayout ** ppInputLayout )
{
    assert( m_pDevice1 );
    ID3D10InputLayout *temp_pInputLayout = NULL;
    ID3D10InputLayout **ppInputLayoutTemp = ppInputLayout == NULL ? NULL : &temp_pInputLayout;
    m_pDevice1->IAGetInputLayout(  ppInputLayoutTemp );
    if( ppInputLayout != NULL )
    {
        D3DWrapperConvertInterface( ppInputLayout, *ppInputLayoutTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device1::IAGetVertexBuffers()
////////////////////////////////////////////////////////////
void C10Device1::IAGetVertexBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppVertexBuffers, UINT * pStrides, UINT * pOffsets )
{
    assert( m_pDevice1 );
    ID3D10Buffer **ppVertexBuffersConv = ppVertexBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppVertexBuffers && !ppVertexBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->IAGetVertexBuffers(  StartSlot,  NumBuffers,  ppVertexBuffersConv,  pStrides,  pOffsets );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppVertexBuffersConv[j] )
        {
            ppVertexBuffers[j] = new C10Buffer( ppVertexBuffersConv[j] );
            if( !ppVertexBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppVertexBuffers[j]->AddRef();
        }
        else
        {
            ppVertexBuffers[j] = NULL;
        }
    }
    delete[]( ppVertexBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::IAGetIndexBuffer()
////////////////////////////////////////////////////////////
void C10Device1::IAGetIndexBuffer(  CBuffer ** pIndexBuffer, DXGI_FORMAT * Format, UINT * Offset )
{
    assert( m_pDevice1 );
    ID3D10Buffer *temp_IndexBuffer = NULL;
    ID3D10Buffer **pIndexBufferTemp = pIndexBuffer == NULL ? NULL : &temp_IndexBuffer;
    m_pDevice1->IAGetIndexBuffer(  pIndexBufferTemp,  Format,  Offset );
    if( pIndexBuffer != NULL )
    {
        D3DWrapperConvertInterface( pIndexBuffer, *pIndexBufferTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device1::GSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C10Device1::GSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice1 );
    ID3D10Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->GSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C10Buffer( ppConstantBuffersConv[j] );
            if( !ppConstantBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppConstantBuffers[j]->AddRef();
        }
        else
        {
            ppConstantBuffers[j] = NULL;
        }
    }
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::GSGetShader()
////////////////////////////////////////////////////////////
void C10Device1::GSGetShader(  CGeometryShader ** ppGeometryShader )
{
    assert( m_pDevice1 );
    ID3D10GeometryShader *temp_pGeometryShader = NULL;
    ID3D10GeometryShader **ppGeometryShaderTemp = ppGeometryShader == NULL ? NULL : &temp_pGeometryShader;
    m_pDevice1->GSGetShader(  ppGeometryShaderTemp );
    if( ppGeometryShader != NULL )
    {
        D3DWrapperConvertInterface( ppGeometryShader, *ppGeometryShaderTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device1::IAGetPrimitiveTopology()
////////////////////////////////////////////////////////////
void C10Device1::IAGetPrimitiveTopology(  D3D_PRIMITIVE_TOPOLOGY * pTopology )
{
    assert( m_pDevice1 );
    D3D10_PRIMITIVE_TOPOLOGY temp_d3d10_primitive_topology;
    m_pDevice1->IAGetPrimitiveTopology(  &temp_d3d10_primitive_topology );
    D3DWrapperConvertEnum( pTopology, temp_d3d10_primitive_topology );
}


////////////////////////////////////////////////////////////
// C10Device1::VSGetShaderResources()
////////////////////////////////////////////////////////////
void C10Device1::VSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice1 );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->VSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C10ShaderResourceView( ppShaderResourceViewsConv[j] );
            if( !ppShaderResourceViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppShaderResourceViews[j]->AddRef();
        }
        else
        {
            ppShaderResourceViews[j] = NULL;
        }
    }
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device1::VSGetSamplers()
////////////////////////////////////////////////////////////
void C10Device1::VSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice1 );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->VSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C10SamplerState( ppSamplersConv[j] );
            if( !ppSamplers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppSamplers[j]->AddRef();
        }
        else
        {
            ppSamplers[j] = NULL;
        }
    }
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::GetPredication()
////////////////////////////////////////////////////////////
void C10Device1::GetPredication(  CPredicate ** ppPredicate, BOOL * pPredicateValue )
{
    assert( m_pDevice1 );
    ID3D10Predicate *temp_pPredicate = NULL;
    ID3D10Predicate **ppPredicateTemp = ppPredicate == NULL ? NULL : &temp_pPredicate;
    m_pDevice1->GetPredication(  ppPredicateTemp,  pPredicateValue );
    if( ppPredicate != NULL )
    {
        D3DWrapperConvertInterface( ppPredicate, *ppPredicateTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device1::GSGetShaderResources()
////////////////////////////////////////////////////////////
void C10Device1::GSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice1 );
    ID3D10ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D10ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->GSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C10ShaderResourceView( ppShaderResourceViewsConv[j] );
            if( !ppShaderResourceViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppShaderResourceViews[j]->AddRef();
        }
        else
        {
            ppShaderResourceViews[j] = NULL;
        }
    }
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device1::GSGetSamplers()
////////////////////////////////////////////////////////////
void C10Device1::GSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice1 );
    ID3D10SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D10SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->GSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C10SamplerState( ppSamplersConv[j] );
            if( !ppSamplers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppSamplers[j]->AddRef();
        }
        else
        {
            ppSamplers[j] = NULL;
        }
    }
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C10Device1::OMGetRenderTargets()
////////////////////////////////////////////////////////////
void C10Device1::OMGetRenderTargets(  UINT NumViews, CRenderTargetView ** ppRenderTargetViews, CDepthStencilView ** ppDepthStencilView )
{
    assert( m_pDevice1 );
    ID3D10RenderTargetView **ppRenderTargetViewsConv = ppRenderTargetViews == NULL ? NULL : new ID3D10RenderTargetView*[NumViews];
    if( ppRenderTargetViews && !ppRenderTargetViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    ID3D10DepthStencilView *temp_pDepthStencilView = NULL;
    ID3D10DepthStencilView **ppDepthStencilViewTemp = ppDepthStencilView == NULL ? NULL : &temp_pDepthStencilView;
    m_pDevice1->OMGetRenderTargets(  NumViews,  ppRenderTargetViewsConv,  ppDepthStencilViewTemp );
    if( ppDepthStencilView != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilView, *ppDepthStencilViewTemp );
    }
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppRenderTargetViewsConv[j] )
        {
            ppRenderTargetViews[j] = new C10RenderTargetView( ppRenderTargetViewsConv[j] );
            if( !ppRenderTargetViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppRenderTargetViews[j]->AddRef();
        }
        else
        {
            ppRenderTargetViews[j] = NULL;
        }
    }
    delete[]( ppRenderTargetViewsConv );
}


////////////////////////////////////////////////////////////
// C10Device1::OMGetBlendState()
////////////////////////////////////////////////////////////
void C10Device1::OMGetBlendState(  CBlendState ** ppBlendState, FLOAT BlendFactor[4], UINT * pSampleMask )
{
    assert( m_pDevice1 );
    ID3D10BlendState *temp_pBlendState = NULL;
    ID3D10BlendState **ppBlendStateTemp = ppBlendState == NULL ? NULL : &temp_pBlendState;
    m_pDevice1->OMGetBlendState(  ppBlendStateTemp,  BlendFactor,  pSampleMask );
    if( ppBlendState != NULL )
    {
        D3DWrapperConvertInterface( ppBlendState, *ppBlendStateTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device1::OMGetDepthStencilState()
////////////////////////////////////////////////////////////
void C10Device1::OMGetDepthStencilState(  CDepthStencilState ** ppDepthStencilState, UINT * pStencilRef )
{
    assert( m_pDevice1 );
    ID3D10DepthStencilState *temp_pDepthStencilState = NULL;
    ID3D10DepthStencilState **ppDepthStencilStateTemp = ppDepthStencilState == NULL ? NULL : &temp_pDepthStencilState;
    m_pDevice1->OMGetDepthStencilState(  ppDepthStencilStateTemp,  pStencilRef );
    if( ppDepthStencilState != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilState, *ppDepthStencilStateTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device1::SOGetTargets()
////////////////////////////////////////////////////////////
void C10Device1::SOGetTargets(  UINT NumBuffers, CBuffer ** ppSOTargets, UINT * pOffsets )
{
    assert( m_pDevice1 );
    ID3D10Buffer **ppSOTargetsConv = ppSOTargets == NULL ? NULL : new ID3D10Buffer*[NumBuffers];
    if( ppSOTargets && !ppSOTargetsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->SOGetTargets(  NumBuffers,  ppSOTargetsConv,  pOffsets );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppSOTargetsConv[j] )
        {
            ppSOTargets[j] = new C10Buffer( ppSOTargetsConv[j] );
            if( !ppSOTargets[j] ) throw( WRAPPER_OUT_OF_MEMORY );
            ppSOTargets[j]->AddRef();
        }
        else
        {
            ppSOTargets[j] = NULL;
        }
    }
    delete[]( ppSOTargetsConv );
}


////////////////////////////////////////////////////////////
// C10Device1::RSGetState()
////////////////////////////////////////////////////////////
void C10Device1::RSGetState(  CRasterizerState ** ppRasterizerState )
{
    assert( m_pDevice1 );
    ID3D10RasterizerState *temp_pRasterizerState = NULL;
    ID3D10RasterizerState **ppRasterizerStateTemp = ppRasterizerState == NULL ? NULL : &temp_pRasterizerState;
    m_pDevice1->RSGetState(  ppRasterizerStateTemp );
    if( ppRasterizerState != NULL )
    {
        D3DWrapperConvertInterface( ppRasterizerState, *ppRasterizerStateTemp );
    }
}


////////////////////////////////////////////////////////////
// C10Device1::RSGetViewports()
////////////////////////////////////////////////////////////
void C10Device1::RSGetViewports(  UINT * NumViewports, D3D_VIEWPORT * pViewports )
{
    assert( m_pDevice1 );
    D3D10_VIEWPORT *pViewportsConv = pViewports == NULL ? NULL : new D3D10_VIEWPORT[*NumViewports];
    if( pViewports && !pViewportsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    m_pDevice1->RSGetViewports(  NumViewports,  pViewportsConv );
    for( UINT i = *NumViewports - 1; i != (UINT) -1; --i )
        D3DWrapperConvertStruct( &pViewports[i], &pViewportsConv[i] );
}


////////////////////////////////////////////////////////////
// C10Device1::RSGetScissorRects()
////////////////////////////////////////////////////////////
void C10Device1::RSGetScissorRects(  UINT * NumRects, D3D_RECT * pRects )
{
    assert( m_pDevice1 );
    m_pDevice1->RSGetScissorRects(  NumRects,  pRects );
}


////////////////////////////////////////////////////////////
// C10Device1::GetDeviceRemovedReason()
////////////////////////////////////////////////////////////
NRESULT C10Device1::GetDeviceRemovedReason(  )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    rval = m_pDevice1->GetDeviceRemovedReason(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::SetExceptionMode()
////////////////////////////////////////////////////////////
NRESULT C10Device1::SetExceptionMode(  UINT RaiseFlags )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    UINT temp_RaiseFlags;
    D3DFlagsConvertTo10_RAISE_FLAG( &temp_RaiseFlags, RaiseFlags );
    rval = m_pDevice1->SetExceptionMode(  temp_RaiseFlags );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::GetExceptionMode()
////////////////////////////////////////////////////////////
UINT C10Device1::GetExceptionMode(  )
{
    assert( m_pDevice1 );
    UINT rval = 0;
    rval = m_pDevice1->GetExceptionMode(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Device1::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    rval = m_pDevice1->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10Device1::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    rval = m_pDevice1->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10Device1::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    rval = m_pDevice1->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::ClearState()
////////////////////////////////////////////////////////////
void C10Device1::ClearState(  )
{
    assert( m_pDevice1 );
    m_pDevice1->ClearState(  );
}


////////////////////////////////////////////////////////////
// C10Device1::Flush()
////////////////////////////////////////////////////////////
void C10Device1::Flush(  )
{
    assert( m_pDevice1 );
    m_pDevice1->Flush(  );
}


////////////////////////////////////////////////////////////
// C10Device1::CreateBuffer()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateBuffer(  const D3D_BUFFER_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CBuffer ** ppBuffer )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D10_BUFFER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_BUFFER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_BUFFER_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    BYTE *pInitialDataByteTemp = NULL;
    D3D10_SUBRESOURCE_DATA *pInitialDataTemp = NULL;
    SIZE_T pInitialDataSizeBuffer = sizeof(D3D10_SUBRESOURCE_DATA);
    if( pInitialData != NULL && pInitialDataSizeBuffer != 0 )
    {
        pInitialDataByteTemp = new BYTE[pInitialDataSizeBuffer];
        pInitialDataTemp = (D3D10_SUBRESOURCE_DATA*)pInitialDataByteTemp;
        D3DWrapperConvertStruct( pInitialDataTemp, pInitialData );
    }
    ID3D10Buffer *temp_pBuffer = NULL;
    ID3D10Buffer **ppBufferTemp = ppBuffer == NULL ? NULL : &temp_pBuffer;
    rval = m_pDevice1->CreateBuffer(  pDescTemp,  pInitialDataTemp,  ppBufferTemp );
    delete[] pDescByteTemp;
    delete[] pInitialDataByteTemp;
    if( ppBuffer != NULL )
    {
        D3DWrapperConvertInterface( ppBuffer, *ppBufferTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateTexture1D()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateTexture1D(  const D3D_TEXTURE1D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture1D ** ppTexture1D )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D10_TEXTURE1D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_TEXTURE1D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_TEXTURE1D_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    UINT mipCount = pDesc->MipLevels;
    if( mipCount == 0 )
    {
        UINT maxDimension = pDesc->Width;
        mipCount = IntLog2( maxDimension ) + 1;
    }
    D3D10_SUBRESOURCE_DATA *pInitialDataConv = pInitialData == NULL ? NULL : new D3D10_SUBRESOURCE_DATA[(pDesc->ArraySize * mipCount)];
    if( pInitialData && !pInitialDataConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInitialData )
    {
        for( UINT i = (pDesc->ArraySize * mipCount) - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInitialDataConv[i], &pInitialData[i] );
        }
    }
    ID3D10Texture1D *temp_pTexture1D = NULL;
    ID3D10Texture1D **ppTexture1DTemp = ppTexture1D == NULL ? NULL : &temp_pTexture1D;
    rval = m_pDevice1->CreateTexture1D(  pDescTemp,  pInitialDataConv,  ppTexture1DTemp );
    delete[] pDescByteTemp;
    delete[]( pInitialDataConv );
    if( ppTexture1D != NULL )
    {
        D3DWrapperConvertInterface( ppTexture1D, *ppTexture1DTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateTexture2D()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateTexture2D(  const D3D_TEXTURE2D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture2D ** ppTexture2D )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D10_TEXTURE2D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_TEXTURE2D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_TEXTURE2D_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    UINT mipCount = pDesc->MipLevels;
    if( mipCount == 0 )
    {
        UINT maxDimension = max( pDesc->Width, pDesc->Height );
        mipCount = IntLog2( maxDimension ) + 1;
    }
    D3D10_SUBRESOURCE_DATA *pInitialDataConv = pInitialData == NULL ? NULL : new D3D10_SUBRESOURCE_DATA[(pDesc->ArraySize * mipCount)];
    if( pInitialData && !pInitialDataConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInitialData )
    {
        for( UINT i = (pDesc->ArraySize * mipCount) - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInitialDataConv[i], &pInitialData[i] );
        }
    }
    ID3D10Texture2D *temp_pTexture2D = NULL;
    ID3D10Texture2D **ppTexture2DTemp = ppTexture2D == NULL ? NULL : &temp_pTexture2D;
    rval = m_pDevice1->CreateTexture2D(  pDescTemp,  pInitialDataConv,  ppTexture2DTemp );
    delete[] pDescByteTemp;
    delete[]( pInitialDataConv );
    if( ppTexture2D != NULL )
    {
        D3DWrapperConvertInterface( ppTexture2D, *ppTexture2DTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateTexture3D()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateTexture3D(  const D3D_TEXTURE3D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture3D ** ppTexture3D )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D10_TEXTURE3D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_TEXTURE3D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_TEXTURE3D_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    UINT mipCount = pDesc->MipLevels;
    if( mipCount == 0 )
    {
        UINT maxDimension = max( pDesc->Depth, max( pDesc->Width, pDesc->Height ) );
        mipCount = IntLog2( maxDimension ) + 1;
    }
    D3D10_SUBRESOURCE_DATA *pInitialDataConv = pInitialData == NULL ? NULL : new D3D10_SUBRESOURCE_DATA[(mipCount)];
    if( pInitialData && !pInitialDataConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInitialData )
    {
        for( UINT i = (mipCount) - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInitialDataConv[i], &pInitialData[i] );
        }
    }
    ID3D10Texture3D *temp_pTexture3D = NULL;
    ID3D10Texture3D **ppTexture3DTemp = ppTexture3D == NULL ? NULL : &temp_pTexture3D;
    rval = m_pDevice1->CreateTexture3D(  pDescTemp,  pInitialDataConv,  ppTexture3DTemp );
    delete[] pDescByteTemp;
    delete[]( pInitialDataConv );
    if( ppTexture3D != NULL )
    {
        D3DWrapperConvertInterface( ppTexture3D, *ppTexture3DTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateShaderResourceView()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateShaderResourceView(  CResource * pResource, const D3D_SHADER_RESOURCE_VIEW_DESC * pDesc, CShaderResourceView ** ppSRView )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    ID3D10Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    BYTE *pDescByteTemp = NULL;
    D3D10_SHADER_RESOURCE_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_SHADER_RESOURCE_VIEW_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    ID3D10ShaderResourceView *temp_pSRView = NULL;
    ID3D10ShaderResourceView **ppSRViewTemp = ppSRView == NULL ? NULL : &temp_pSRView;
    rval = m_pDevice1->CreateShaderResourceView(  temp_pResource,  pDescTemp,  ppSRViewTemp );
    delete[] pDescByteTemp;
    if( ppSRView != NULL )
    {
        D3DWrapperConvertInterface( ppSRView, *ppSRViewTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateRenderTargetView()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateRenderTargetView(  CResource * pResource, const D3D_RENDER_TARGET_VIEW_DESC * pDesc, CRenderTargetView ** ppRTView )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    ID3D10Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    BYTE *pDescByteTemp = NULL;
    D3D10_RENDER_TARGET_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_RENDER_TARGET_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_RENDER_TARGET_VIEW_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    ID3D10RenderTargetView *temp_pRTView = NULL;
    ID3D10RenderTargetView **ppRTViewTemp = ppRTView == NULL ? NULL : &temp_pRTView;
    rval = m_pDevice1->CreateRenderTargetView(  temp_pResource,  pDescTemp,  ppRTViewTemp );
    delete[] pDescByteTemp;
    if( ppRTView != NULL )
    {
        D3DWrapperConvertInterface( ppRTView, *ppRTViewTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateDepthStencilView()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateDepthStencilView(  CResource * pResource, const D3D_DEPTH_STENCIL_VIEW_DESC * pDesc, CDepthStencilView ** ppDepthStencilView )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    ID3D10Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    BYTE *pDescByteTemp = NULL;
    D3D10_DEPTH_STENCIL_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_DEPTH_STENCIL_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_DEPTH_STENCIL_VIEW_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    ID3D10DepthStencilView *temp_pDepthStencilView = NULL;
    ID3D10DepthStencilView **ppDepthStencilViewTemp = ppDepthStencilView == NULL ? NULL : &temp_pDepthStencilView;
    rval = m_pDevice1->CreateDepthStencilView(  temp_pResource,  pDescTemp,  ppDepthStencilViewTemp );
    delete[] pDescByteTemp;
    if( ppDepthStencilView != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilView, *ppDepthStencilViewTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateInputLayout()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateInputLayout(  const D3D_INPUT_ELEMENT_DESC * pInputElementDescs, UINT NumElements, const void * pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, CInputLayout ** ppInputLayout )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    D3D10_INPUT_ELEMENT_DESC *pInputElementDescsConv = pInputElementDescs == NULL ? NULL : new D3D10_INPUT_ELEMENT_DESC[NumElements];
    if( pInputElementDescs && !pInputElementDescsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInputElementDescs )
    {
        for( UINT i = NumElements - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInputElementDescsConv[i], &pInputElementDescs[i] );
        }
    }
    ID3D10InputLayout *temp_pInputLayout = NULL;
    ID3D10InputLayout **ppInputLayoutTemp = ppInputLayout == NULL ? NULL : &temp_pInputLayout;
    rval = m_pDevice1->CreateInputLayout(  pInputElementDescsConv,  NumElements,  pShaderBytecodeWithInputSignature,  BytecodeLength,  ppInputLayoutTemp );
    delete[]( pInputElementDescsConv );
    if( ppInputLayout != NULL )
    {
        D3DWrapperConvertInterface( ppInputLayout, *ppInputLayoutTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateVertexShader()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateVertexShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CVertexShader ** ppVertexShader )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    ID3D10VertexShader *temp_pVertexShader = NULL;
    ID3D10VertexShader **ppVertexShaderTemp = ppVertexShader == NULL ? NULL : &temp_pVertexShader;
    rval = m_pDevice1->CreateVertexShader(  pShaderBytecode,  BytecodeLength,  ppVertexShaderTemp );
    if( ppVertexShader != NULL )
    {
        D3DWrapperConvertInterface( ppVertexShader, *ppVertexShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateGeometryShader()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateGeometryShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CGeometryShader ** ppGeometryShader )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    ID3D10GeometryShader *temp_pGeometryShader = NULL;
    ID3D10GeometryShader **ppGeometryShaderTemp = ppGeometryShader == NULL ? NULL : &temp_pGeometryShader;
    rval = m_pDevice1->CreateGeometryShader(  pShaderBytecode,  BytecodeLength,  ppGeometryShaderTemp );
    if( ppGeometryShader != NULL )
    {
        D3DWrapperConvertInterface( ppGeometryShader, *ppGeometryShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateGeometryShaderWithStreamOutput()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateGeometryShaderWithStreamOutput(  const void * pShaderBytecode, SIZE_T BytecodeLength, const D3D_SO_DECLARATION_ENTRY * pSODeclaration, UINT NumEntries, UINT OutputStreamStride, CGeometryShader ** ppGeometryShader )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    D3D10_SO_DECLARATION_ENTRY *pSODeclarationConv = pSODeclaration == NULL ? NULL : new D3D10_SO_DECLARATION_ENTRY[NumEntries];
    if( pSODeclaration && !pSODeclarationConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pSODeclaration )
    {
        for( UINT i = NumEntries - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pSODeclarationConv[i], &pSODeclaration[i] );
        }
    }
    ID3D10GeometryShader *temp_pGeometryShader = NULL;
    ID3D10GeometryShader **ppGeometryShaderTemp = ppGeometryShader == NULL ? NULL : &temp_pGeometryShader;
    rval = m_pDevice1->CreateGeometryShaderWithStreamOutput(  pShaderBytecode,  BytecodeLength,  pSODeclarationConv,  NumEntries,  OutputStreamStride,  ppGeometryShaderTemp );
    delete[]( pSODeclarationConv );
    if( ppGeometryShader != NULL )
    {
        D3DWrapperConvertInterface( ppGeometryShader, *ppGeometryShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreatePixelShader()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreatePixelShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CPixelShader ** ppPixelShader )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    ID3D10PixelShader *temp_pPixelShader = NULL;
    ID3D10PixelShader **ppPixelShaderTemp = ppPixelShader == NULL ? NULL : &temp_pPixelShader;
    rval = m_pDevice1->CreatePixelShader(  pShaderBytecode,  BytecodeLength,  ppPixelShaderTemp );
    if( ppPixelShader != NULL )
    {
        D3DWrapperConvertInterface( ppPixelShader, *ppPixelShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateBlendState()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateBlendState(  const D3D_BLEND_DESC * pBlendStateDesc, CBlendState ** ppBlendState )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pBlendStateDescByteTemp = NULL;
    D3D10_BLEND_DESC *pBlendStateDescTemp = NULL;
    SIZE_T pBlendStateDescSizeBuffer = sizeof(D3D10_BLEND_DESC);
    if( pBlendStateDesc != NULL && pBlendStateDescSizeBuffer != 0 )
    {
        pBlendStateDescByteTemp = new BYTE[pBlendStateDescSizeBuffer];
        pBlendStateDescTemp = (D3D10_BLEND_DESC*)pBlendStateDescByteTemp;
        D3DWrapperConvertStruct( pBlendStateDescTemp, pBlendStateDesc );
    }
    ID3D10BlendState *temp_pBlendState = NULL;
    ID3D10BlendState **ppBlendStateTemp = ppBlendState == NULL ? NULL : &temp_pBlendState;
    rval = m_pDevice1->CreateBlendState(  pBlendStateDescTemp,  ppBlendStateTemp );
    delete[] pBlendStateDescByteTemp;
    if( ppBlendState != NULL )
    {
        D3DWrapperConvertInterface( ppBlendState, *ppBlendStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateDepthStencilState()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateDepthStencilState(  const D3D_DEPTH_STENCIL_DESC * pDepthStencilDesc, CDepthStencilState ** ppDepthStencilState )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pDepthStencilDescByteTemp = NULL;
    D3D10_DEPTH_STENCIL_DESC *pDepthStencilDescTemp = NULL;
    SIZE_T pDepthStencilDescSizeBuffer = sizeof(D3D10_DEPTH_STENCIL_DESC);
    if( pDepthStencilDesc != NULL && pDepthStencilDescSizeBuffer != 0 )
    {
        pDepthStencilDescByteTemp = new BYTE[pDepthStencilDescSizeBuffer];
        pDepthStencilDescTemp = (D3D10_DEPTH_STENCIL_DESC*)pDepthStencilDescByteTemp;
        D3DWrapperConvertStruct( pDepthStencilDescTemp, pDepthStencilDesc );
    }
    ID3D10DepthStencilState *temp_pDepthStencilState = NULL;
    ID3D10DepthStencilState **ppDepthStencilStateTemp = ppDepthStencilState == NULL ? NULL : &temp_pDepthStencilState;
    rval = m_pDevice1->CreateDepthStencilState(  pDepthStencilDescTemp,  ppDepthStencilStateTemp );
    delete[] pDepthStencilDescByteTemp;
    if( ppDepthStencilState != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilState, *ppDepthStencilStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateRasterizerState()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateRasterizerState(  const D3D_RASTERIZER_DESC * pRasterizerDesc, CRasterizerState ** ppRasterizerState )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pRasterizerDescByteTemp = NULL;
    D3D10_RASTERIZER_DESC *pRasterizerDescTemp = NULL;
    SIZE_T pRasterizerDescSizeBuffer = sizeof(D3D10_RASTERIZER_DESC);
    if( pRasterizerDesc != NULL && pRasterizerDescSizeBuffer != 0 )
    {
        pRasterizerDescByteTemp = new BYTE[pRasterizerDescSizeBuffer];
        pRasterizerDescTemp = (D3D10_RASTERIZER_DESC*)pRasterizerDescByteTemp;
        D3DWrapperConvertStruct( pRasterizerDescTemp, pRasterizerDesc );
    }
    ID3D10RasterizerState *temp_pRasterizerState = NULL;
    ID3D10RasterizerState **ppRasterizerStateTemp = ppRasterizerState == NULL ? NULL : &temp_pRasterizerState;
    rval = m_pDevice1->CreateRasterizerState(  pRasterizerDescTemp,  ppRasterizerStateTemp );
    delete[] pRasterizerDescByteTemp;
    if( ppRasterizerState != NULL )
    {
        D3DWrapperConvertInterface( ppRasterizerState, *ppRasterizerStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateSamplerState()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateSamplerState(  const D3D_SAMPLER_DESC * pSamplerDesc, CSamplerState ** ppSamplerState )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pSamplerDescByteTemp = NULL;
    D3D10_SAMPLER_DESC *pSamplerDescTemp = NULL;
    SIZE_T pSamplerDescSizeBuffer = sizeof(D3D10_SAMPLER_DESC);
    if( pSamplerDesc != NULL && pSamplerDescSizeBuffer != 0 )
    {
        pSamplerDescByteTemp = new BYTE[pSamplerDescSizeBuffer];
        pSamplerDescTemp = (D3D10_SAMPLER_DESC*)pSamplerDescByteTemp;
        D3DWrapperConvertStruct( pSamplerDescTemp, pSamplerDesc );
    }
    ID3D10SamplerState *temp_pSamplerState = NULL;
    ID3D10SamplerState **ppSamplerStateTemp = ppSamplerState == NULL ? NULL : &temp_pSamplerState;
    rval = m_pDevice1->CreateSamplerState(  pSamplerDescTemp,  ppSamplerStateTemp );
    delete[] pSamplerDescByteTemp;
    if( ppSamplerState != NULL )
    {
        D3DWrapperConvertInterface( ppSamplerState, *ppSamplerStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateQuery()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateQuery(  const D3D_QUERY_DESC * pQueryDesc, CQuery ** ppQuery )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pQueryDescByteTemp = NULL;
    D3D10_QUERY_DESC *pQueryDescTemp = NULL;
    SIZE_T pQueryDescSizeBuffer = sizeof(D3D10_QUERY_DESC);
    if( pQueryDesc != NULL && pQueryDescSizeBuffer != 0 )
    {
        pQueryDescByteTemp = new BYTE[pQueryDescSizeBuffer];
        pQueryDescTemp = (D3D10_QUERY_DESC*)pQueryDescByteTemp;
        D3DWrapperConvertStruct( pQueryDescTemp, pQueryDesc );
    }
    ID3D10Query *temp_pQuery = NULL;
    ID3D10Query **ppQueryTemp = ppQuery == NULL ? NULL : &temp_pQuery;
    rval = m_pDevice1->CreateQuery(  pQueryDescTemp,  ppQueryTemp );
    delete[] pQueryDescByteTemp;
    if( ppQuery != NULL )
    {
        D3DWrapperConvertInterface( ppQuery, *ppQueryTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreatePredicate()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreatePredicate(  const D3D_QUERY_DESC * pPredicateDesc, CPredicate ** ppPredicate )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pPredicateDescByteTemp = NULL;
    D3D10_QUERY_DESC *pPredicateDescTemp = NULL;
    SIZE_T pPredicateDescSizeBuffer = sizeof(D3D10_QUERY_DESC);
    if( pPredicateDesc != NULL && pPredicateDescSizeBuffer != 0 )
    {
        pPredicateDescByteTemp = new BYTE[pPredicateDescSizeBuffer];
        pPredicateDescTemp = (D3D10_QUERY_DESC*)pPredicateDescByteTemp;
        D3DWrapperConvertStruct( pPredicateDescTemp, pPredicateDesc );
    }
    ID3D10Predicate *temp_pPredicate = NULL;
    ID3D10Predicate **ppPredicateTemp = ppPredicate == NULL ? NULL : &temp_pPredicate;
    rval = m_pDevice1->CreatePredicate(  pPredicateDescTemp,  ppPredicateTemp );
    delete[] pPredicateDescByteTemp;
    if( ppPredicate != NULL )
    {
        D3DWrapperConvertInterface( ppPredicate, *ppPredicateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CreateCounter()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CreateCounter(  const D3D_COUNTER_DESC * pCounterDesc, CCounter ** ppCounter )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pCounterDescByteTemp = NULL;
    D3D10_COUNTER_DESC *pCounterDescTemp = NULL;
    SIZE_T pCounterDescSizeBuffer = sizeof(D3D10_COUNTER_DESC);
    if( pCounterDesc != NULL && pCounterDescSizeBuffer != 0 )
    {
        pCounterDescByteTemp = new BYTE[pCounterDescSizeBuffer];
        pCounterDescTemp = (D3D10_COUNTER_DESC*)pCounterDescByteTemp;
        D3DWrapperConvertStruct( pCounterDescTemp, pCounterDesc );
    }
    ID3D10Counter *temp_pCounter = NULL;
    ID3D10Counter **ppCounterTemp = ppCounter == NULL ? NULL : &temp_pCounter;
    rval = m_pDevice1->CreateCounter(  pCounterDescTemp,  ppCounterTemp );
    delete[] pCounterDescByteTemp;
    if( ppCounter != NULL )
    {
        D3DWrapperConvertInterface( ppCounter, *ppCounterTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CheckFormatSupport()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CheckFormatSupport(  DXGI_FORMAT Format, UINT * pFormatSupport )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    UINT temp_pFormatSupport;
    rval = m_pDevice1->CheckFormatSupport(  Format,  &temp_pFormatSupport );
    D3DFlagsConvertFrom10_FORMAT_SUPPORT( pFormatSupport, temp_pFormatSupport );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CheckMultisampleQualityLevels()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CheckMultisampleQualityLevels(  DXGI_FORMAT Format, UINT SampleCount, UINT * pNumQualityLevels )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    rval = m_pDevice1->CheckMultisampleQualityLevels(  Format,  SampleCount,  pNumQualityLevels );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::CheckCounterInfo()
////////////////////////////////////////////////////////////
void C10Device1::CheckCounterInfo(  D3D_COUNTER_INFO * pCounterInfo )
{
    assert( m_pDevice1 );
    BYTE *pCounterInfoByteTemp = NULL;
    D3D10_COUNTER_INFO *pCounterInfoTemp = NULL;
    SIZE_T pCounterInfoSizeBuffer = sizeof(D3D10_COUNTER_INFO);
    if( pCounterInfo != NULL && pCounterInfoSizeBuffer != 0 )
    {
        pCounterInfoByteTemp = new BYTE[pCounterInfoSizeBuffer];
        pCounterInfoTemp = (D3D10_COUNTER_INFO*)pCounterInfoByteTemp;
    }
    m_pDevice1->CheckCounterInfo(  pCounterInfoTemp );
    if( pCounterInfo != NULL && pCounterInfoSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pCounterInfo, pCounterInfoTemp );
        delete[] pCounterInfoByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10Device1::CheckCounter()
////////////////////////////////////////////////////////////
NRESULT C10Device1::CheckCounter(  const D3D_COUNTER_DESC * pDesc, D3D_COUNTER_TYPE * pType, UINT * pActiveCounters, LPSTR szName, UINT * pNameLength, LPSTR szUnits, UINT * pUnitsLength, LPSTR szDescription, UINT * pDescriptionLength )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D10_COUNTER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_COUNTER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_COUNTER_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    D3D10_COUNTER_TYPE temp_d3d10_counter_type;
    rval = m_pDevice1->CheckCounter(  pDescTemp,  &temp_d3d10_counter_type,  pActiveCounters,  szName,  pNameLength,  szUnits,  pUnitsLength,  szDescription,  pDescriptionLength );
    delete[] pDescByteTemp;
    D3DWrapperConvertEnum( pType, temp_d3d10_counter_type );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::GetCreationFlags()
////////////////////////////////////////////////////////////
UINT C10Device1::GetCreationFlags(  )
{
    assert( m_pDevice1 );
    UINT rval = 0;
    rval = m_pDevice1->GetCreationFlags(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::OpenSharedResource()
////////////////////////////////////////////////////////////
NRESULT C10Device1::OpenSharedResource(  HANDLE hResource, REFIID ReturnedInterface, void ** ppResource )
{
    assert( m_pDevice1 );
    NRESULT rval = S_OK;
    rval = m_pDevice1->OpenSharedResource(  hResource,  ReturnedInterface,  ppResource );
    return rval;
}


////////////////////////////////////////////////////////////
// C10Device1::SetTextFilterSize()
////////////////////////////////////////////////////////////
void C10Device1::SetTextFilterSize(  UINT Width, UINT Height )
{
    assert( m_pDevice1 );
    m_pDevice1->SetTextFilterSize(  Width,  Height );
}


////////////////////////////////////////////////////////////
// C10Device1::GetTextFilterSize()
////////////////////////////////////////////////////////////
void C10Device1::GetTextFilterSize(  UINT * pWidth, UINT * pHeight )
{
    assert( m_pDevice1 );
    m_pDevice1->GetTextFilterSize(  pWidth,  pHeight );
}
