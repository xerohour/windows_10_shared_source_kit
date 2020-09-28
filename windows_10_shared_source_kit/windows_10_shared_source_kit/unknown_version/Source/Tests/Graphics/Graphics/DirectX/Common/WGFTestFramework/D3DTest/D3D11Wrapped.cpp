//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D11Wrapped.cpp
//* 
//* Description: 
//*     Interface specific D3D wrapper classes.
//*
//*****************************************************************************

#include "pch.h"
#include "D3DWrappedTypes.h"
#include "D3D11Wrapped.h"
#include "d3d11_1.h"
#include "D3D11Convert.h"


#pragma prefast( disable: 387, "These are wrapper functions which will only pass 0 when given 0 for the wrapped parameter." )




////////////////////////////////////////////////////////////
//
// Wrapper classes
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C11DeviceChild
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C11DepthStencilState
//
////////////////////////////////////////////////////////////

C11DepthStencilState::C11DepthStencilState( ID3D11DepthStencilState *pDepthStencilState ) : 
    m_pDepthStencilState( pDepthStencilState )
{
}


C11DepthStencilState::~C11DepthStencilState()
{
    if( m_pDepthStencilState ) m_pDepthStencilState->Release();
}


////////////////////////////////////////////////////////////
// C11DepthStencilState::GetDesc()
////////////////////////////////////////////////////////////
void C11DepthStencilState::GetDesc(  D3D_DEPTH_STENCIL_DESC * pDesc )
{
    assert( m_pDepthStencilState );
    BYTE *pDescByteTemp = NULL;
    D3D11_DEPTH_STENCIL_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_DEPTH_STENCIL_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_DEPTH_STENCIL_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pDepthStencilState->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11DepthStencilState::GetDevice()
////////////////////////////////////////////////////////////
void C11DepthStencilState::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pDepthStencilState );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pDepthStencilState->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11DepthStencilState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11DepthStencilState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pDepthStencilState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pDepthStencilState->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11DepthStencilState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11DepthStencilState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pDepthStencilState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pDepthStencilState->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11DepthStencilState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11DepthStencilState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pDepthStencilState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pDepthStencilState->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11BlendState
//
////////////////////////////////////////////////////////////

C11BlendState::C11BlendState( ID3D11BlendState *pBlendState ) : 
    m_pBlendState( pBlendState )
{
}


C11BlendState::~C11BlendState()
{
    if( m_pBlendState ) m_pBlendState->Release();
}


////////////////////////////////////////////////////////////
// C11BlendState::GetDesc()
////////////////////////////////////////////////////////////
void C11BlendState::GetDesc(  D3D_BLEND_DESC * pDesc )
{
    assert( m_pBlendState );
    BYTE *pDescByteTemp = NULL;
    D3D11_BLEND_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_BLEND_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_BLEND_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pBlendState->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11BlendState::GetDevice()
////////////////////////////////////////////////////////////
void C11BlendState::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pBlendState );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pBlendState->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11BlendState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11BlendState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pBlendState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pBlendState->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11BlendState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11BlendState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pBlendState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pBlendState->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11BlendState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11BlendState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pBlendState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pBlendState->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11RasterizerState
//
////////////////////////////////////////////////////////////

C11RasterizerState::C11RasterizerState( ID3D11RasterizerState *pRasterizerState ) : 
    m_pRasterizerState( pRasterizerState )
{
}


C11RasterizerState::~C11RasterizerState()
{
    if( m_pRasterizerState ) m_pRasterizerState->Release();
}


////////////////////////////////////////////////////////////
// C11RasterizerState::GetDesc()
////////////////////////////////////////////////////////////
void C11RasterizerState::GetDesc(  D3D_RASTERIZER_DESC * pDesc )
{
    assert( m_pRasterizerState );
    BYTE *pDescByteTemp = NULL;
    D3D11_RASTERIZER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_RASTERIZER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_RASTERIZER_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pRasterizerState->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11RasterizerState::GetDevice()
////////////////////////////////////////////////////////////
void C11RasterizerState::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pRasterizerState );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pRasterizerState->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11RasterizerState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11RasterizerState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pRasterizerState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pRasterizerState->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11RasterizerState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11RasterizerState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pRasterizerState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pRasterizerState->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11RasterizerState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11RasterizerState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pRasterizerState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pRasterizerState->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11Resource
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C11Buffer
//
////////////////////////////////////////////////////////////

C11Buffer::C11Buffer( ID3D11Buffer *pBuffer ) : 
    m_pBuffer( pBuffer )
{
}


C11Buffer::~C11Buffer()
{
    if( m_pBuffer ) m_pBuffer->Release();
}


////////////////////////////////////////////////////////////
// C11Buffer::GetDesc()
////////////////////////////////////////////////////////////
void C11Buffer::GetDesc(  D3D_BUFFER_DESC * pDesc )
{
    assert( m_pBuffer );
    BYTE *pDescByteTemp = NULL;
    D3D11_BUFFER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_BUFFER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_BUFFER_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pBuffer->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Buffer::Map()
////////////////////////////////////////////////////////////
NRESULT C11Buffer::Map(  D3D_MAP MapType, UINT MapFlags, void** ppData )
{
    assert( m_pBuffer );
    NRESULT rval = S_OK;
    D3D11_MAP temp_d3d11_map;
    D3DWrapperConvertEnum( &temp_d3d11_map, MapType );
    UINT temp_MapFlags;
    D3DFlagsConvertTo11_MAP_FLAG( &temp_MapFlags, MapFlags );
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = pTempDevice11->ExecDeferredContext( );
    if (FAILED(rval))
    {
        return rval;
    }
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = pImmContext->Map(  m_pBuffer,  0,  temp_d3d11_map,  temp_MapFlags,  &mappedSubresource );
    *ppData = mappedSubresource.pData;
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Buffer::Unmap()
////////////////////////////////////////////////////////////
void C11Buffer::Unmap(  )
{
    assert( m_pBuffer );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Unmap(  m_pBuffer,  0 );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Buffer::GetType()
////////////////////////////////////////////////////////////
void C11Buffer::GetType(  D3D_RESOURCE_DIMENSION * pResourceDimension )
{
    assert( m_pBuffer );
    D3D11_RESOURCE_DIMENSION temp_d3d11_resource_dimension;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pBuffer->GetType(  &temp_d3d11_resource_dimension );
    D3DWrapperConvertEnum( pResourceDimension, temp_d3d11_resource_dimension );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Buffer::SetEvictionPriority()
////////////////////////////////////////////////////////////
void C11Buffer::SetEvictionPriority(  UINT EvictionPriority )
{
    assert( m_pBuffer );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pBuffer->SetEvictionPriority(  EvictionPriority );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Buffer::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT C11Buffer::GetEvictionPriority(  )
{
    assert( m_pBuffer );
    UINT rval = 0;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pBuffer->GetEvictionPriority(  );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Buffer::GetDevice()
////////////////////////////////////////////////////////////
void C11Buffer::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pBuffer );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pBuffer->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11Buffer::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Buffer::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pBuffer );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pBuffer->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Buffer::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Buffer::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pBuffer );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pBuffer->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Buffer::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11Buffer::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pBuffer );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pBuffer->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11Texture1D
//
////////////////////////////////////////////////////////////

C11Texture1D::C11Texture1D( ID3D11Texture1D *pTexture1D ) : 
    m_pTexture1D( pTexture1D )
{
}


C11Texture1D::~C11Texture1D()
{
    if( m_pTexture1D ) m_pTexture1D->Release();
}


////////////////////////////////////////////////////////////
// C11Texture1D::GetDesc()
////////////////////////////////////////////////////////////
void C11Texture1D::GetDesc(  D3D_TEXTURE1D_DESC * pDesc )
{
    assert( m_pTexture1D );
    BYTE *pDescByteTemp = NULL;
    D3D11_TEXTURE1D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_TEXTURE1D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_TEXTURE1D_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pTexture1D->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture1D::Map()
////////////////////////////////////////////////////////////
NRESULT C11Texture1D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, void** ppData )
{
    assert( m_pTexture1D );
    NRESULT rval = S_OK;
    D3D11_MAP temp_d3d11_map;
    D3DWrapperConvertEnum( &temp_d3d11_map, MapType );
    UINT temp_MapFlags;
    D3DFlagsConvertTo11_MAP_FLAG( &temp_MapFlags, MapFlags );
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = pTempDevice11->ExecDeferredContext( );
    if (FAILED(rval))
    {
        return rval;
    }
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = pImmContext->Map(  m_pTexture1D,  Subresource,  temp_d3d11_map,  temp_MapFlags,  &mappedSubresource );
    *ppData = mappedSubresource.pData;
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture1D::Unmap()
////////////////////////////////////////////////////////////
void C11Texture1D::Unmap(  UINT Subresource )
{
    assert( m_pTexture1D );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Unmap(  m_pTexture1D,  Subresource );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture1D::GetType()
////////////////////////////////////////////////////////////
void C11Texture1D::GetType(  D3D_RESOURCE_DIMENSION * pResourceDimension )
{
    assert( m_pTexture1D );
    D3D11_RESOURCE_DIMENSION temp_d3d11_resource_dimension;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pTexture1D->GetType(  &temp_d3d11_resource_dimension );
    D3DWrapperConvertEnum( pResourceDimension, temp_d3d11_resource_dimension );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture1D::SetEvictionPriority()
////////////////////////////////////////////////////////////
void C11Texture1D::SetEvictionPriority(  UINT EvictionPriority )
{
    assert( m_pTexture1D );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pTexture1D->SetEvictionPriority(  EvictionPriority );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture1D::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT C11Texture1D::GetEvictionPriority(  )
{
    assert( m_pTexture1D );
    UINT rval = 0;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture1D->GetEvictionPriority(  );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture1D::GetDevice()
////////////////////////////////////////////////////////////
void C11Texture1D::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pTexture1D );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pTexture1D->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11Texture1D::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Texture1D::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pTexture1D );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture1D->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture1D::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Texture1D::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pTexture1D );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture1D->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture1D::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11Texture1D::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pTexture1D );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture1D->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11Texture2D
//
////////////////////////////////////////////////////////////

C11Texture2D::C11Texture2D( ID3D11Texture2D *pTexture2D ) : 
    m_pTexture2D( pTexture2D )
{
}


C11Texture2D::~C11Texture2D()
{
    if( m_pTexture2D ) m_pTexture2D->Release();
}


////////////////////////////////////////////////////////////
// C11Texture2D::GetDesc()
////////////////////////////////////////////////////////////
void C11Texture2D::GetDesc(  D3D_TEXTURE2D_DESC * pDesc )
{
    assert( m_pTexture2D );
    BYTE *pDescByteTemp = NULL;
    D3D11_TEXTURE2D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_TEXTURE2D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_TEXTURE2D_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pTexture2D->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture2D::Map()
////////////////////////////////////////////////////////////
NRESULT C11Texture2D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, D3D_MAPPED_TEXTURE2D* pMappedTex2D )
{
    assert( m_pTexture2D );
    NRESULT rval = S_OK;
    D3D11_MAP temp_d3d11_map;
    D3DWrapperConvertEnum( &temp_d3d11_map, MapType );
    UINT temp_MapFlags;
    D3DFlagsConvertTo11_MAP_FLAG( &temp_MapFlags, MapFlags );
    BYTE *pMappedTex2DByteTemp = NULL;
    D3D11_MAPPED_TEXTURE2D *pMappedTex2DTemp = NULL;
    SIZE_T pMappedTex2DSizeBuffer = sizeof(D3D11_MAPPED_TEXTURE2D);
    if( pMappedTex2D != NULL && pMappedTex2DSizeBuffer != 0 )
    {
        pMappedTex2DByteTemp = new BYTE[pMappedTex2DSizeBuffer];
        pMappedTex2DTemp = (D3D11_MAPPED_TEXTURE2D*)pMappedTex2DByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = pTempDevice11->ExecDeferredContext( );
    if (FAILED(rval))
    {
        return rval;
    }
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = pImmContext->Map(  m_pTexture2D,  Subresource,  temp_d3d11_map,  temp_MapFlags,  pMappedTex2DTemp );
    if( pMappedTex2D != NULL && pMappedTex2DSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pMappedTex2D, pMappedTex2DTemp );
        delete[] pMappedTex2DByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture2D::Unmap()
////////////////////////////////////////////////////////////
void C11Texture2D::Unmap(  UINT Subresource )
{
    assert( m_pTexture2D );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Unmap(  m_pTexture2D,  Subresource );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture2D::GetType()
////////////////////////////////////////////////////////////
void C11Texture2D::GetType(  D3D_RESOURCE_DIMENSION * pResourceDimension )
{
    assert( m_pTexture2D );
    D3D11_RESOURCE_DIMENSION temp_d3d11_resource_dimension;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pTexture2D->GetType(  &temp_d3d11_resource_dimension );
    D3DWrapperConvertEnum( pResourceDimension, temp_d3d11_resource_dimension );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture2D::SetEvictionPriority()
////////////////////////////////////////////////////////////
void C11Texture2D::SetEvictionPriority(  UINT EvictionPriority )
{
    assert( m_pTexture2D );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pTexture2D->SetEvictionPriority(  EvictionPriority );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture2D::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT C11Texture2D::GetEvictionPriority(  )
{
    assert( m_pTexture2D );
    UINT rval = 0;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture2D->GetEvictionPriority(  );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture2D::GetDevice()
////////////////////////////////////////////////////////////
void C11Texture2D::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pTexture2D );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pTexture2D->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11Texture2D::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Texture2D::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pTexture2D );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture2D->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture2D::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Texture2D::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pTexture2D );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture2D->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture2D::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11Texture2D::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pTexture2D );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture2D->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11Texture3D
//
////////////////////////////////////////////////////////////

C11Texture3D::C11Texture3D( ID3D11Texture3D *pTexture3D ) : 
    m_pTexture3D( pTexture3D )
{
}


C11Texture3D::~C11Texture3D()
{
    if( m_pTexture3D ) m_pTexture3D->Release();
}


////////////////////////////////////////////////////////////
// C11Texture3D::GetDesc()
////////////////////////////////////////////////////////////
void C11Texture3D::GetDesc(  D3D_TEXTURE3D_DESC * pDesc )
{
    assert( m_pTexture3D );
    BYTE *pDescByteTemp = NULL;
    D3D11_TEXTURE3D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_TEXTURE3D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_TEXTURE3D_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pTexture3D->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture3D::Map()
////////////////////////////////////////////////////////////
NRESULT C11Texture3D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, D3D_MAPPED_TEXTURE3D* pMappedTex3D )
{
    assert( m_pTexture3D );
    NRESULT rval = S_OK;
    D3D11_MAP temp_d3d11_map;
    D3DWrapperConvertEnum( &temp_d3d11_map, MapType );
    UINT temp_MapFlags;
    D3DFlagsConvertTo11_MAP_FLAG( &temp_MapFlags, MapFlags );
    BYTE *pMappedTex3DByteTemp = NULL;
    D3D11_MAPPED_TEXTURE3D *pMappedTex3DTemp = NULL;
    SIZE_T pMappedTex3DSizeBuffer = sizeof(D3D11_MAPPED_TEXTURE3D);
    if( pMappedTex3D != NULL && pMappedTex3DSizeBuffer != 0 )
    {
        pMappedTex3DByteTemp = new BYTE[pMappedTex3DSizeBuffer];
        pMappedTex3DTemp = (D3D11_MAPPED_TEXTURE3D*)pMappedTex3DByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = pTempDevice11->ExecDeferredContext( );
    if (FAILED(rval))
    {
        return rval;
    }
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = pImmContext->Map(  m_pTexture3D,  Subresource,  temp_d3d11_map,  temp_MapFlags,  pMappedTex3DTemp );
    if( pMappedTex3D != NULL && pMappedTex3DSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pMappedTex3D, pMappedTex3DTemp );
        delete[] pMappedTex3DByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture3D::Unmap()
////////////////////////////////////////////////////////////
void C11Texture3D::Unmap(  UINT Subresource )
{
    assert( m_pTexture3D );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Unmap(  m_pTexture3D,  Subresource );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture3D::GetType()
////////////////////////////////////////////////////////////
void C11Texture3D::GetType(  D3D_RESOURCE_DIMENSION * pResourceDimension )
{
    assert( m_pTexture3D );
    D3D11_RESOURCE_DIMENSION temp_d3d11_resource_dimension;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pTexture3D->GetType(  &temp_d3d11_resource_dimension );
    D3DWrapperConvertEnum( pResourceDimension, temp_d3d11_resource_dimension );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture3D::SetEvictionPriority()
////////////////////////////////////////////////////////////
void C11Texture3D::SetEvictionPriority(  UINT EvictionPriority )
{
    assert( m_pTexture3D );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pTexture3D->SetEvictionPriority(  EvictionPriority );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Texture3D::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT C11Texture3D::GetEvictionPriority(  )
{
    assert( m_pTexture3D );
    UINT rval = 0;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture3D->GetEvictionPriority(  );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture3D::GetDevice()
////////////////////////////////////////////////////////////
void C11Texture3D::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pTexture3D );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pTexture3D->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11Texture3D::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Texture3D::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pTexture3D );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture3D->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture3D::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Texture3D::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pTexture3D );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture3D->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Texture3D::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11Texture3D::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pTexture3D );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pTexture3D->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11View
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C11ShaderResourceView
//
////////////////////////////////////////////////////////////

C11ShaderResourceView::C11ShaderResourceView( ID3D11ShaderResourceView *pShaderResourceView ) : 
    m_pShaderResourceView( pShaderResourceView )
{
}


C11ShaderResourceView::~C11ShaderResourceView()
{
    if( m_pShaderResourceView ) m_pShaderResourceView->Release();
}


////////////////////////////////////////////////////////////
// C11ShaderResourceView::GetDesc()
////////////////////////////////////////////////////////////
void C11ShaderResourceView::GetDesc(  D3D_SHADER_RESOURCE_VIEW_DESC * pDesc )
{
    assert( m_pShaderResourceView );
    BYTE *pDescByteTemp = NULL;
    D3D11_SHADER_RESOURCE_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_SHADER_RESOURCE_VIEW_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pShaderResourceView->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11ShaderResourceView::GetResource()
////////////////////////////////////////////////////////////
void C11ShaderResourceView::GetResource(  CResource ** ppResource )
{
    assert( m_pShaderResourceView );
    ID3D11Resource *temp_pResource = NULL;
    ID3D11Resource **ppResourceTemp = ppResource == NULL ? NULL : &temp_pResource;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pShaderResourceView->GetResource(  ppResourceTemp );
    if( ppResource != NULL )
    {
        D3DWrapperConvertInterface( ppResource, *ppResourceTemp );
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11ShaderResourceView::GetDevice()
////////////////////////////////////////////////////////////
void C11ShaderResourceView::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pShaderResourceView );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pShaderResourceView->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11ShaderResourceView::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11ShaderResourceView::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pShaderResourceView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pShaderResourceView->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11ShaderResourceView::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11ShaderResourceView::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pShaderResourceView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pShaderResourceView->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11ShaderResourceView::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11ShaderResourceView::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pShaderResourceView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pShaderResourceView->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11RenderTargetView
//
////////////////////////////////////////////////////////////

C11RenderTargetView::C11RenderTargetView( ID3D11RenderTargetView *pRenderTargetView ) : 
    m_pRenderTargetView( pRenderTargetView )
{
}


C11RenderTargetView::~C11RenderTargetView()
{
    if( m_pRenderTargetView ) m_pRenderTargetView->Release();
}


////////////////////////////////////////////////////////////
// C11RenderTargetView::GetDesc()
////////////////////////////////////////////////////////////
void C11RenderTargetView::GetDesc(  D3D_RENDER_TARGET_VIEW_DESC * pDesc )
{
    assert( m_pRenderTargetView );
    BYTE *pDescByteTemp = NULL;
    D3D11_RENDER_TARGET_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_RENDER_TARGET_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_RENDER_TARGET_VIEW_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pRenderTargetView->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11RenderTargetView::GetResource()
////////////////////////////////////////////////////////////
void C11RenderTargetView::GetResource(  CResource ** ppResource )
{
    assert( m_pRenderTargetView );
    ID3D11Resource *temp_pResource = NULL;
    ID3D11Resource **ppResourceTemp = ppResource == NULL ? NULL : &temp_pResource;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pRenderTargetView->GetResource(  ppResourceTemp );
    if( ppResource != NULL )
    {
        D3DWrapperConvertInterface( ppResource, *ppResourceTemp );
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11RenderTargetView::GetDevice()
////////////////////////////////////////////////////////////
void C11RenderTargetView::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pRenderTargetView );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pRenderTargetView->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11RenderTargetView::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11RenderTargetView::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pRenderTargetView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pRenderTargetView->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11RenderTargetView::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11RenderTargetView::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pRenderTargetView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pRenderTargetView->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11RenderTargetView::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11RenderTargetView::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pRenderTargetView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pRenderTargetView->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11DepthStencilView
//
////////////////////////////////////////////////////////////

C11DepthStencilView::C11DepthStencilView( ID3D11DepthStencilView *pDepthStencilView ) : 
    m_pDepthStencilView( pDepthStencilView )
{
}


C11DepthStencilView::~C11DepthStencilView()
{
    if( m_pDepthStencilView ) m_pDepthStencilView->Release();
}


////////////////////////////////////////////////////////////
// C11DepthStencilView::GetDesc()
////////////////////////////////////////////////////////////
void C11DepthStencilView::GetDesc(  D3D_DEPTH_STENCIL_VIEW_DESC * pDesc )
{
    assert( m_pDepthStencilView );
    BYTE *pDescByteTemp = NULL;
    D3D11_DEPTH_STENCIL_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_DEPTH_STENCIL_VIEW_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pDepthStencilView->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11DepthStencilView::GetResource()
////////////////////////////////////////////////////////////
void C11DepthStencilView::GetResource(  CResource ** ppResource )
{
    assert( m_pDepthStencilView );
    ID3D11Resource *temp_pResource = NULL;
    ID3D11Resource **ppResourceTemp = ppResource == NULL ? NULL : &temp_pResource;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pDepthStencilView->GetResource(  ppResourceTemp );
    if( ppResource != NULL )
    {
        D3DWrapperConvertInterface( ppResource, *ppResourceTemp );
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11DepthStencilView::GetDevice()
////////////////////////////////////////////////////////////
void C11DepthStencilView::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pDepthStencilView );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pDepthStencilView->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11DepthStencilView::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11DepthStencilView::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pDepthStencilView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pDepthStencilView->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11DepthStencilView::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11DepthStencilView::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pDepthStencilView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pDepthStencilView->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11DepthStencilView::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11DepthStencilView::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pDepthStencilView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pDepthStencilView->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11VertexShader
//
////////////////////////////////////////////////////////////

C11VertexShader::C11VertexShader( ID3D11VertexShader *pVertexShader ) : 
    m_pVertexShader( pVertexShader )
{
}


C11VertexShader::~C11VertexShader()
{
    if( m_pVertexShader ) m_pVertexShader->Release();
}


////////////////////////////////////////////////////////////
// C11VertexShader::GetDevice()
////////////////////////////////////////////////////////////
void C11VertexShader::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pVertexShader );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pVertexShader->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11VertexShader::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11VertexShader::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pVertexShader );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pVertexShader->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11VertexShader::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11VertexShader::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pVertexShader );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pVertexShader->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11VertexShader::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11VertexShader::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pVertexShader );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pVertexShader->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11GeometryShader
//
////////////////////////////////////////////////////////////

C11GeometryShader::C11GeometryShader( ID3D11GeometryShader *pGeometryShader ) : 
    m_pGeometryShader( pGeometryShader )
{
}


C11GeometryShader::~C11GeometryShader()
{
    if( m_pGeometryShader ) m_pGeometryShader->Release();
}


////////////////////////////////////////////////////////////
// C11GeometryShader::GetDevice()
////////////////////////////////////////////////////////////
void C11GeometryShader::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pGeometryShader );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pGeometryShader->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11GeometryShader::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11GeometryShader::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pGeometryShader );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pGeometryShader->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11GeometryShader::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11GeometryShader::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pGeometryShader );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pGeometryShader->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11GeometryShader::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11GeometryShader::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pGeometryShader );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pGeometryShader->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11PixelShader
//
////////////////////////////////////////////////////////////

C11PixelShader::C11PixelShader( ID3D11PixelShader *pPixelShader ) : 
    m_pPixelShader( pPixelShader )
{
}


C11PixelShader::~C11PixelShader()
{
    if( m_pPixelShader ) m_pPixelShader->Release();
}


////////////////////////////////////////////////////////////
// C11PixelShader::GetDevice()
////////////////////////////////////////////////////////////
void C11PixelShader::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pPixelShader );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pPixelShader->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11PixelShader::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11PixelShader::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pPixelShader );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pPixelShader->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11PixelShader::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11PixelShader::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pPixelShader );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pPixelShader->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11PixelShader::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11PixelShader::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pPixelShader );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pPixelShader->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11InputLayout
//
////////////////////////////////////////////////////////////

C11InputLayout::C11InputLayout( ID3D11InputLayout *pInputLayout ) : 
    m_pInputLayout( pInputLayout )
{
}


C11InputLayout::~C11InputLayout()
{
    if( m_pInputLayout ) m_pInputLayout->Release();
}


////////////////////////////////////////////////////////////
// C11InputLayout::GetDevice()
////////////////////////////////////////////////////////////
void C11InputLayout::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pInputLayout );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pInputLayout->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11InputLayout::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11InputLayout::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pInputLayout );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pInputLayout->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11InputLayout::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11InputLayout::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pInputLayout );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pInputLayout->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11InputLayout::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11InputLayout::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pInputLayout );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pInputLayout->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11SamplerState
//
////////////////////////////////////////////////////////////

C11SamplerState::C11SamplerState( ID3D11SamplerState *pSamplerState ) : 
    m_pSamplerState( pSamplerState )
{
}


C11SamplerState::~C11SamplerState()
{
    if( m_pSamplerState ) m_pSamplerState->Release();
}


////////////////////////////////////////////////////////////
// C11SamplerState::GetDesc()
////////////////////////////////////////////////////////////
void C11SamplerState::GetDesc(  D3D_SAMPLER_DESC * pDesc )
{
    assert( m_pSamplerState );
    BYTE *pDescByteTemp = NULL;
    D3D11_SAMPLER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_SAMPLER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_SAMPLER_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pSamplerState->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11SamplerState::GetDevice()
////////////////////////////////////////////////////////////
void C11SamplerState::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pSamplerState );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pSamplerState->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11SamplerState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11SamplerState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pSamplerState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pSamplerState->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11SamplerState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11SamplerState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pSamplerState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pSamplerState->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11SamplerState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11SamplerState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pSamplerState );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pSamplerState->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11Asynchronous
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C11Query
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// C11Predicate
//
////////////////////////////////////////////////////////////

C11Predicate::C11Predicate( ID3D11Predicate *pPredicate ) : 
    m_pPredicate( pPredicate )
{
}


C11Predicate::~C11Predicate()
{
    if( m_pPredicate ) m_pPredicate->Release();
}


////////////////////////////////////////////////////////////
// C11Predicate::GetDesc()
////////////////////////////////////////////////////////////
void C11Predicate::GetDesc(  D3D_QUERY_DESC * pDesc )
{
    assert( m_pPredicate );
    BYTE *pDescByteTemp = NULL;
    D3D11_QUERY_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_QUERY_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_QUERY_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pPredicate->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Predicate::GetDataSize()
////////////////////////////////////////////////////////////
UINT C11Predicate::GetDataSize(  )
{
    assert( m_pPredicate );
    UINT rval = 0;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pPredicate->GetDataSize(  );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Predicate::Begin()
////////////////////////////////////////////////////////////
void C11Predicate::Begin(  )
{
    assert( m_pPredicate );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->Begin(  m_pPredicate );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Predicate::End()
////////////////////////////////////////////////////////////
void C11Predicate::End(  )
{
    assert( m_pPredicate );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->End(  m_pPredicate );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Predicate::GetData()
////////////////////////////////////////////////////////////
NRESULT C11Predicate::GetData(  void* pData, UINT DataSize, UINT GetDataFlags )
{
    assert( m_pPredicate );
    NRESULT rval = S_OK;
    UINT temp_GetDataFlags;
    D3DFlagsConvertTo11_ASYNC_GETDATA_FLAG( &temp_GetDataFlags, GetDataFlags );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = pTempDevice11->ExecDeferredContext( );
    if (FAILED(rval))
    {
        return rval;
    }
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = pImmContext->GetData(  m_pPredicate,  pData,  DataSize,  temp_GetDataFlags );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Predicate::GetDevice()
////////////////////////////////////////////////////////////
void C11Predicate::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pPredicate );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pPredicate->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11Predicate::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Predicate::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pPredicate );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pPredicate->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Predicate::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Predicate::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pPredicate );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pPredicate->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Predicate::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11Predicate::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pPredicate );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pPredicate->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11Counter
//
////////////////////////////////////////////////////////////

C11Counter::C11Counter( ID3D11Counter *pCounter ) : 
    m_pCounter( pCounter )
{
}


C11Counter::~C11Counter()
{
    if( m_pCounter ) m_pCounter->Release();
}


////////////////////////////////////////////////////////////
// C11Counter::GetDesc()
////////////////////////////////////////////////////////////
void C11Counter::GetDesc(  D3D_COUNTER_DESC * pDesc )
{
    assert( m_pCounter );
    BYTE *pDescByteTemp = NULL;
    D3D11_COUNTER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_COUNTER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_COUNTER_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pCounter->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Counter::GetDataSize()
////////////////////////////////////////////////////////////
UINT C11Counter::GetDataSize(  )
{
    assert( m_pCounter );
    UINT rval = 0;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pCounter->GetDataSize(  );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Counter::Begin()
////////////////////////////////////////////////////////////
void C11Counter::Begin(  )
{
    assert( m_pCounter );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->Begin(  m_pCounter );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Counter::End()
////////////////////////////////////////////////////////////
void C11Counter::End(  )
{
    assert( m_pCounter );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->End(  m_pCounter );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11Counter::GetData()
////////////////////////////////////////////////////////////
NRESULT C11Counter::GetData(  void* pData, UINT DataSize, UINT GetDataFlags )
{
    assert( m_pCounter );
    NRESULT rval = S_OK;
    UINT temp_GetDataFlags;
    D3DFlagsConvertTo11_ASYNC_GETDATA_FLAG( &temp_GetDataFlags, GetDataFlags );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = pTempDevice11->ExecDeferredContext( );
    if (FAILED(rval))
    {
        return rval;
    }
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = pImmContext->GetData(  m_pCounter,  pData,  DataSize,  temp_GetDataFlags );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Counter::GetDevice()
////////////////////////////////////////////////////////////
void C11Counter::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pCounter );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pCounter->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11Counter::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Counter::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pCounter );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pCounter->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Counter::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Counter::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pCounter );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pCounter->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11Counter::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11Counter::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pCounter );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pCounter->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11Device
//
////////////////////////////////////////////////////////////

C11Device::C11Device( ID3D11Device *pDevice ) : 
    m_pDevice( pDevice )
{
    HRESULT hr = m_pDevice->CreateDeferredContext(0, &m_pDeviceDeferredContext);
    assert(SUCCEEDED(hr));
    m_pDevice->GetImmediateContext(&m_pDeviceImmediateContext);
    m_bUseDeferredContext = true;
    C11Device *pTempC11Device = this;
    m_pDevice->SetPrivateData(C11DeviceGuid, sizeof(m_pDevice), (void*)(&pTempC11Device));
}


C11Device::~C11Device()
{
    if( m_pDeviceDeferredContext ) m_pDeviceDeferredContext->Release();
    if( m_pDeviceImmediateContext ) m_pDeviceImmediateContext->Release();
    if( m_pDevice ) m_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11Device::CreateBuffer()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateBuffer(  const D3D_BUFFER_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CBuffer ** ppBuffer )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D11_BUFFER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_BUFFER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_BUFFER_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    BYTE *pInitialDataByteTemp = NULL;
    D3D11_SUBRESOURCE_DATA *pInitialDataTemp = NULL;
    SIZE_T pInitialDataSizeBuffer = sizeof(D3D11_SUBRESOURCE_DATA);
    if( pInitialData != NULL && pInitialDataSizeBuffer != 0 )
    {
        pInitialDataByteTemp = new BYTE[pInitialDataSizeBuffer];
        pInitialDataTemp = (D3D11_SUBRESOURCE_DATA*)pInitialDataByteTemp;
        D3DWrapperConvertStruct( pInitialDataTemp, pInitialData );
    }
    ID3D11Buffer *temp_pBuffer = NULL;
    ID3D11Buffer **ppBufferTemp = ppBuffer == NULL ? NULL : &temp_pBuffer;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
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
// C11Device::CreateTexture1D()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateTexture1D(  const D3D_TEXTURE1D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture1D ** ppTexture1D )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D11_TEXTURE1D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_TEXTURE1D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_TEXTURE1D_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    UINT mipCount = pDesc->MipLevels;
    if( mipCount == 0 )
    {
        UINT maxDimension = pDesc->Width;
        mipCount = IntLog2( maxDimension ) + 1;
    }
    D3D11_SUBRESOURCE_DATA *pInitialDataConv = pInitialData == NULL ? NULL : new D3D11_SUBRESOURCE_DATA[(pDesc->ArraySize * mipCount)];
    if( pInitialData && !pInitialDataConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInitialData )
    {
        for( UINT i = (pDesc->ArraySize * mipCount) - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInitialDataConv[i], &pInitialData[i] );
        }
    }
    ID3D11Texture1D *temp_pTexture1D = NULL;
    ID3D11Texture1D **ppTexture1DTemp = ppTexture1D == NULL ? NULL : &temp_pTexture1D;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
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
// C11Device::CreateTexture2D()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateTexture2D(  const D3D_TEXTURE2D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture2D ** ppTexture2D )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D11_TEXTURE2D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_TEXTURE2D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_TEXTURE2D_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    UINT mipCount = pDesc->MipLevels;
    if( mipCount == 0 )
    {
        UINT maxDimension = max( pDesc->Width, pDesc->Height );
        mipCount = IntLog2( maxDimension ) + 1;
    }
    D3D11_SUBRESOURCE_DATA *pInitialDataConv = pInitialData == NULL ? NULL : new D3D11_SUBRESOURCE_DATA[(pDesc->ArraySize * mipCount)];
    if( pInitialData && !pInitialDataConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInitialData )
    {
        for( UINT i = (pDesc->ArraySize * mipCount) - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInitialDataConv[i], &pInitialData[i] );
        }
    }
    ID3D11Texture2D *temp_pTexture2D = NULL;
    ID3D11Texture2D **ppTexture2DTemp = ppTexture2D == NULL ? NULL : &temp_pTexture2D;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
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
// C11Device::CreateTexture3D()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateTexture3D(  const D3D_TEXTURE3D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture3D ** ppTexture3D )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D11_TEXTURE3D_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_TEXTURE3D_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_TEXTURE3D_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    UINT mipCount = pDesc->MipLevels;
    if( mipCount == 0 )
    {
        UINT maxDimension = max( pDesc->Depth, max( pDesc->Width, pDesc->Height ) );
        mipCount = IntLog2( maxDimension ) + 1;
    }
    D3D11_SUBRESOURCE_DATA *pInitialDataConv = pInitialData == NULL ? NULL : new D3D11_SUBRESOURCE_DATA[(mipCount)];
    if( pInitialData && !pInitialDataConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInitialData )
    {
        for( UINT i = (mipCount) - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInitialDataConv[i], &pInitialData[i] );
        }
    }
    ID3D11Texture3D *temp_pTexture3D = NULL;
    ID3D11Texture3D **ppTexture3DTemp = ppTexture3D == NULL ? NULL : &temp_pTexture3D;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
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
// C11Device::CreateShaderResourceView()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateShaderResourceView(  CResource * pResource, const D3D_SHADER_RESOURCE_VIEW_DESC * pDesc, CShaderResourceView ** ppSRView )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D11Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    BYTE *pDescByteTemp = NULL;
    D3D11_SHADER_RESOURCE_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_SHADER_RESOURCE_VIEW_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    ID3D11ShaderResourceView *temp_pSRView = NULL;
    ID3D11ShaderResourceView **ppSRViewTemp = ppSRView == NULL ? NULL : &temp_pSRView;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateShaderResourceView(  temp_pResource,  pDescTemp,  ppSRViewTemp );
    delete[] pDescByteTemp;
    if( ppSRView != NULL )
    {
        D3DWrapperConvertInterface( ppSRView, *ppSRViewTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateRenderTargetView()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateRenderTargetView(  CResource * pResource, const D3D_RENDER_TARGET_VIEW_DESC * pDesc, CRenderTargetView ** ppRTView )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D11Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    BYTE *pDescByteTemp = NULL;
    D3D11_RENDER_TARGET_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_RENDER_TARGET_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_RENDER_TARGET_VIEW_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    ID3D11RenderTargetView *temp_pRTView = NULL;
    ID3D11RenderTargetView **ppRTViewTemp = ppRTView == NULL ? NULL : &temp_pRTView;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateRenderTargetView(  temp_pResource,  pDescTemp,  ppRTViewTemp );
    delete[] pDescByteTemp;
    if( ppRTView != NULL )
    {
        D3DWrapperConvertInterface( ppRTView, *ppRTViewTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateDepthStencilView()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateDepthStencilView(  CResource * pResource, const D3D_DEPTH_STENCIL_VIEW_DESC * pDesc, CDepthStencilView ** ppDepthStencilView )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D11Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    BYTE *pDescByteTemp = NULL;
    D3D11_DEPTH_STENCIL_VIEW_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_DEPTH_STENCIL_VIEW_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    ID3D11DepthStencilView *temp_pDepthStencilView = NULL;
    ID3D11DepthStencilView **ppDepthStencilViewTemp = ppDepthStencilView == NULL ? NULL : &temp_pDepthStencilView;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateDepthStencilView(  temp_pResource,  pDescTemp,  ppDepthStencilViewTemp );
    delete[] pDescByteTemp;
    if( ppDepthStencilView != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilView, *ppDepthStencilViewTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateInputLayout()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateInputLayout(  const D3D_INPUT_ELEMENT_DESC * pInputElementDescs, UINT NumElements, const void * pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, CInputLayout ** ppInputLayout )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    D3D11_INPUT_ELEMENT_DESC *pInputElementDescsConv = pInputElementDescs == NULL ? NULL : new D3D11_INPUT_ELEMENT_DESC[NumElements];
    if( pInputElementDescs && !pInputElementDescsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pInputElementDescs )
    {
        for( UINT i = NumElements - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pInputElementDescsConv[i], &pInputElementDescs[i] );
        }
    }
    ID3D11InputLayout *temp_pInputLayout = NULL;
    ID3D11InputLayout **ppInputLayoutTemp = ppInputLayout == NULL ? NULL : &temp_pInputLayout;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateInputLayout(  pInputElementDescsConv,  NumElements,  pShaderBytecodeWithInputSignature,  BytecodeLength,  ppInputLayoutTemp );
    delete[]( pInputElementDescsConv );
    if( ppInputLayout != NULL )
    {
        D3DWrapperConvertInterface( ppInputLayout, *ppInputLayoutTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateVertexShader()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateVertexShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CVertexShader ** ppVertexShader )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D11VertexShader *temp_pVertexShader = NULL;
    ID3D11VertexShader **ppVertexShaderTemp = ppVertexShader == NULL ? NULL : &temp_pVertexShader;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateVertexShader(  pShaderBytecode,  BytecodeLength,  NULL,  ppVertexShaderTemp );
    if( ppVertexShader != NULL )
    {
        D3DWrapperConvertInterface( ppVertexShader, *ppVertexShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateGeometryShader()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateGeometryShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CGeometryShader ** ppGeometryShader )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D11GeometryShader *temp_pGeometryShader = NULL;
    ID3D11GeometryShader **ppGeometryShaderTemp = ppGeometryShader == NULL ? NULL : &temp_pGeometryShader;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateGeometryShader(  pShaderBytecode,  BytecodeLength,  NULL,  ppGeometryShaderTemp );
    if( ppGeometryShader != NULL )
    {
        D3DWrapperConvertInterface( ppGeometryShader, *ppGeometryShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateGeometryShaderWithStreamOutput()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateGeometryShaderWithStreamOutput(  const void * pShaderBytecode, SIZE_T BytecodeLength, const D3D_SO_DECLARATION_ENTRY * pSODeclaration, UINT NumEntries, UINT OutputStreamStride, CGeometryShader ** ppGeometryShader )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    D3D11_SO_DECLARATION_ENTRY *pSODeclarationConv = pSODeclaration == NULL ? NULL : new D3D11_SO_DECLARATION_ENTRY[NumEntries];
    if( pSODeclaration && !pSODeclarationConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pSODeclaration )
    {
        for( UINT i = NumEntries - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pSODeclarationConv[i], &pSODeclaration[i] );
        }
    }
    ID3D11GeometryShader *temp_pGeometryShader = NULL;
    ID3D11GeometryShader **ppGeometryShaderTemp = ppGeometryShader == NULL ? NULL : &temp_pGeometryShader;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateGeometryShaderWithStreamOutput(  pShaderBytecode,  BytecodeLength,  pSODeclarationConv,  NumEntries,  &OutputStreamStride,  1,  0,  NULL,  ppGeometryShaderTemp );
    delete[]( pSODeclarationConv );
    if( ppGeometryShader != NULL )
    {
        D3DWrapperConvertInterface( ppGeometryShader, *ppGeometryShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreatePixelShader()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreatePixelShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CPixelShader ** ppPixelShader )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    ID3D11PixelShader *temp_pPixelShader = NULL;
    ID3D11PixelShader **ppPixelShaderTemp = ppPixelShader == NULL ? NULL : &temp_pPixelShader;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreatePixelShader(  pShaderBytecode,  BytecodeLength,  NULL,  ppPixelShaderTemp );
    if( ppPixelShader != NULL )
    {
        D3DWrapperConvertInterface( ppPixelShader, *ppPixelShaderTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateBlendState()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateBlendState(  const D3D_BLEND_DESC * pBlendStateDesc, CBlendState ** ppBlendState )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pBlendStateDescByteTemp = NULL;
    D3D11_BLEND_DESC *pBlendStateDescTemp = NULL;
    SIZE_T pBlendStateDescSizeBuffer = sizeof(D3D11_BLEND_DESC);
    if( pBlendStateDesc != NULL && pBlendStateDescSizeBuffer != 0 )
    {
        pBlendStateDescByteTemp = new BYTE[pBlendStateDescSizeBuffer];
        pBlendStateDescTemp = (D3D11_BLEND_DESC*)pBlendStateDescByteTemp;
        D3DWrapperConvertStruct( pBlendStateDescTemp, pBlendStateDesc );
    }
    ID3D11BlendState *temp_pBlendState = NULL;
    ID3D11BlendState **ppBlendStateTemp = ppBlendState == NULL ? NULL : &temp_pBlendState;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateBlendState(  pBlendStateDescTemp,  ppBlendStateTemp );
    delete[] pBlendStateDescByteTemp;
    if( ppBlendState != NULL )
    {
        D3DWrapperConvertInterface( ppBlendState, *ppBlendStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateDepthStencilState()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateDepthStencilState(  const D3D_DEPTH_STENCIL_DESC * pDepthStencilDesc, CDepthStencilState ** ppDepthStencilState )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDepthStencilDescByteTemp = NULL;
    D3D11_DEPTH_STENCIL_DESC *pDepthStencilDescTemp = NULL;
    SIZE_T pDepthStencilDescSizeBuffer = sizeof(D3D11_DEPTH_STENCIL_DESC);
    if( pDepthStencilDesc != NULL && pDepthStencilDescSizeBuffer != 0 )
    {
        pDepthStencilDescByteTemp = new BYTE[pDepthStencilDescSizeBuffer];
        pDepthStencilDescTemp = (D3D11_DEPTH_STENCIL_DESC*)pDepthStencilDescByteTemp;
        D3DWrapperConvertStruct( pDepthStencilDescTemp, pDepthStencilDesc );
    }
    ID3D11DepthStencilState *temp_pDepthStencilState = NULL;
    ID3D11DepthStencilState **ppDepthStencilStateTemp = ppDepthStencilState == NULL ? NULL : &temp_pDepthStencilState;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateDepthStencilState(  pDepthStencilDescTemp,  ppDepthStencilStateTemp );
    delete[] pDepthStencilDescByteTemp;
    if( ppDepthStencilState != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilState, *ppDepthStencilStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateRasterizerState()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateRasterizerState(  const D3D_RASTERIZER_DESC * pRasterizerDesc, CRasterizerState ** ppRasterizerState )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pRasterizerDescByteTemp = NULL;
    D3D11_RASTERIZER_DESC *pRasterizerDescTemp = NULL;
    SIZE_T pRasterizerDescSizeBuffer = sizeof(D3D11_RASTERIZER_DESC);
    if( pRasterizerDesc != NULL && pRasterizerDescSizeBuffer != 0 )
    {
        pRasterizerDescByteTemp = new BYTE[pRasterizerDescSizeBuffer];
        pRasterizerDescTemp = (D3D11_RASTERIZER_DESC*)pRasterizerDescByteTemp;
        D3DWrapperConvertStruct( pRasterizerDescTemp, pRasterizerDesc );
    }
    ID3D11RasterizerState *temp_pRasterizerState = NULL;
    ID3D11RasterizerState **ppRasterizerStateTemp = ppRasterizerState == NULL ? NULL : &temp_pRasterizerState;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateRasterizerState(  pRasterizerDescTemp,  ppRasterizerStateTemp );
    delete[] pRasterizerDescByteTemp;
    if( ppRasterizerState != NULL )
    {
        D3DWrapperConvertInterface( ppRasterizerState, *ppRasterizerStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateSamplerState()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateSamplerState(  const D3D_SAMPLER_DESC * pSamplerDesc, CSamplerState ** ppSamplerState )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pSamplerDescByteTemp = NULL;
    D3D11_SAMPLER_DESC *pSamplerDescTemp = NULL;
    SIZE_T pSamplerDescSizeBuffer = sizeof(D3D11_SAMPLER_DESC);
    if( pSamplerDesc != NULL && pSamplerDescSizeBuffer != 0 )
    {
        pSamplerDescByteTemp = new BYTE[pSamplerDescSizeBuffer];
        pSamplerDescTemp = (D3D11_SAMPLER_DESC*)pSamplerDescByteTemp;
        D3DWrapperConvertStruct( pSamplerDescTemp, pSamplerDesc );
    }
    ID3D11SamplerState *temp_pSamplerState = NULL;
    ID3D11SamplerState **ppSamplerStateTemp = ppSamplerState == NULL ? NULL : &temp_pSamplerState;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateSamplerState(  pSamplerDescTemp,  ppSamplerStateTemp );
    delete[] pSamplerDescByteTemp;
    if( ppSamplerState != NULL )
    {
        D3DWrapperConvertInterface( ppSamplerState, *ppSamplerStateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateQuery()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateQuery(  const D3D_QUERY_DESC * pQueryDesc, CQuery ** ppQuery )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pQueryDescByteTemp = NULL;
    D3D11_QUERY_DESC *pQueryDescTemp = NULL;
    SIZE_T pQueryDescSizeBuffer = sizeof(D3D11_QUERY_DESC);
    if( pQueryDesc != NULL && pQueryDescSizeBuffer != 0 )
    {
        pQueryDescByteTemp = new BYTE[pQueryDescSizeBuffer];
        pQueryDescTemp = (D3D11_QUERY_DESC*)pQueryDescByteTemp;
        D3DWrapperConvertStruct( pQueryDescTemp, pQueryDesc );
    }
    ID3D11Query *temp_pQuery = NULL;
    ID3D11Query **ppQueryTemp = ppQuery == NULL ? NULL : &temp_pQuery;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateQuery(  pQueryDescTemp,  ppQueryTemp );
    delete[] pQueryDescByteTemp;
    if( ppQuery != NULL )
    {
        D3DWrapperConvertInterface( ppQuery, *ppQueryTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreatePredicate()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreatePredicate(  const D3D_QUERY_DESC * pPredicateDesc, CPredicate ** ppPredicate )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pPredicateDescByteTemp = NULL;
    D3D11_QUERY_DESC *pPredicateDescTemp = NULL;
    SIZE_T pPredicateDescSizeBuffer = sizeof(D3D11_QUERY_DESC);
    if( pPredicateDesc != NULL && pPredicateDescSizeBuffer != 0 )
    {
        pPredicateDescByteTemp = new BYTE[pPredicateDescSizeBuffer];
        pPredicateDescTemp = (D3D11_QUERY_DESC*)pPredicateDescByteTemp;
        D3DWrapperConvertStruct( pPredicateDescTemp, pPredicateDesc );
    }
    ID3D11Predicate *temp_pPredicate = NULL;
    ID3D11Predicate **ppPredicateTemp = ppPredicate == NULL ? NULL : &temp_pPredicate;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreatePredicate(  pPredicateDescTemp,  ppPredicateTemp );
    delete[] pPredicateDescByteTemp;
    if( ppPredicate != NULL )
    {
        D3DWrapperConvertInterface( ppPredicate, *ppPredicateTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CreateCounter()
////////////////////////////////////////////////////////////
NRESULT C11Device::CreateCounter(  const D3D_COUNTER_DESC * pCounterDesc, CCounter ** ppCounter )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pCounterDescByteTemp = NULL;
    D3D11_COUNTER_DESC *pCounterDescTemp = NULL;
    SIZE_T pCounterDescSizeBuffer = sizeof(D3D11_COUNTER_DESC);
    if( pCounterDesc != NULL && pCounterDescSizeBuffer != 0 )
    {
        pCounterDescByteTemp = new BYTE[pCounterDescSizeBuffer];
        pCounterDescTemp = (D3D11_COUNTER_DESC*)pCounterDescByteTemp;
        D3DWrapperConvertStruct( pCounterDescTemp, pCounterDesc );
    }
    ID3D11Counter *temp_pCounter = NULL;
    ID3D11Counter **ppCounterTemp = ppCounter == NULL ? NULL : &temp_pCounter;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    pImmContext->Flush( );
    rval = m_pDevice->CreateCounter(  pCounterDescTemp,  ppCounterTemp );
    delete[] pCounterDescByteTemp;
    if( ppCounter != NULL )
    {
        D3DWrapperConvertInterface( ppCounter, *ppCounterTemp );
    }
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::OpenSharedResource()
////////////////////////////////////////////////////////////
NRESULT C11Device::OpenSharedResource(  HANDLE hResource, REFIID ReturnedInterface, void ** ppResource )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = m_pDevice->OpenSharedResource(  hResource,  ReturnedInterface,  ppResource );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CheckFormatSupport()
////////////////////////////////////////////////////////////
NRESULT C11Device::CheckFormatSupport(  DXGI_FORMAT Format, UINT * pFormatSupport )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    UINT temp_pFormatSupport;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = m_pDevice->CheckFormatSupport(  Format,  &temp_pFormatSupport );
    D3DFlagsConvertFrom11_FORMAT_SUPPORT( pFormatSupport, temp_pFormatSupport );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CheckMultisampleQualityLevels()
////////////////////////////////////////////////////////////
NRESULT C11Device::CheckMultisampleQualityLevels(  DXGI_FORMAT Format, UINT SampleCount, UINT * pNumQualityLevels )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = m_pDevice->CheckMultisampleQualityLevels(  Format,  SampleCount,  pNumQualityLevels );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::CheckCounterInfo()
////////////////////////////////////////////////////////////
void C11Device::CheckCounterInfo(  D3D_COUNTER_INFO * pCounterInfo )
{
    assert( m_pDevice );
    BYTE *pCounterInfoByteTemp = NULL;
    D3D11_COUNTER_INFO *pCounterInfoTemp = NULL;
    SIZE_T pCounterInfoSizeBuffer = sizeof(D3D11_COUNTER_INFO);
    if( pCounterInfo != NULL && pCounterInfoSizeBuffer != 0 )
    {
        pCounterInfoByteTemp = new BYTE[pCounterInfoSizeBuffer];
        pCounterInfoTemp = (D3D11_COUNTER_INFO*)pCounterInfoByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    m_pDevice->CheckCounterInfo(  pCounterInfoTemp );
    if( pCounterInfo != NULL && pCounterInfoSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pCounterInfo, pCounterInfoTemp );
        delete[] pCounterInfoByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C11Device::CheckCounter()
////////////////////////////////////////////////////////////
NRESULT C11Device::CheckCounter(  const D3D_COUNTER_DESC * pDesc, D3D_COUNTER_TYPE * pType, UINT * pActiveCounters, LPSTR szName, UINT * pNameLength, LPSTR szUnits, UINT * pUnitsLength, LPSTR szDescription, UINT * pDescriptionLength )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    BYTE *pDescByteTemp = NULL;
    D3D11_COUNTER_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_COUNTER_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_COUNTER_DESC*)pDescByteTemp;
        D3DWrapperConvertStruct( pDescTemp, pDesc );
    }
    D3D11_COUNTER_TYPE temp_d3d11_counter_type;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = m_pDevice->CheckCounter(  pDescTemp,  &temp_d3d11_counter_type,  pActiveCounters,  szName,  pNameLength,  szUnits,  pUnitsLength,  szDescription,  pDescriptionLength );
    delete[] pDescByteTemp;
    D3DWrapperConvertEnum( pType, temp_d3d11_counter_type );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Device::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = m_pDevice->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11Device::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = m_pDevice->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11Device::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = m_pDevice->SetPrivateDataInterface(  guid,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::GetFeatureLevel()
////////////////////////////////////////////////////////////
D3D_FEATURE_LEVEL1 C11Device::GetFeatureLevel(  )
{
    assert( m_pDevice );
    D3D_FEATURE_LEVEL1 rval = (D3D_FEATURE_LEVEL1)0;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    D3D_FEATURE_LEVEL temp = m_pDevice->GetFeatureLevel(  );
    D3DWrapperConvertEnum( &rval, temp );    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::GetCreationFlags()
////////////////////////////////////////////////////////////
UINT C11Device::GetCreationFlags(  )
{
    assert( m_pDevice );
    UINT rval = 0;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = m_pDevice->GetCreationFlags(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::GetDeviceRemovedReason()
////////////////////////////////////////////////////////////
NRESULT C11Device::GetDeviceRemovedReason(  )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = m_pDevice->GetDeviceRemovedReason(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::SetExceptionMode()
////////////////////////////////////////////////////////////
NRESULT C11Device::SetExceptionMode(  UINT RaiseFlags )
{
    assert( m_pDevice );
    NRESULT rval = S_OK;
    UINT temp_RaiseFlags;
    D3DFlagsConvertTo11_RAISE_FLAG( &temp_RaiseFlags, RaiseFlags );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = m_pDevice->SetExceptionMode(  temp_RaiseFlags );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::GetExceptionMode()
////////////////////////////////////////////////////////////
UINT C11Device::GetExceptionMode(  )
{
    assert( m_pDevice );
    UINT rval = 0;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = m_pDevice->GetExceptionMode(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::VSSetConstantBuffers()
////////////////////////////////////////////////////////////
void C11Device::VSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D11Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffers[j] )
            ppConstantBuffers[j]->GetInterfacePtr( &ppConstantBuffersConv[j] );
        else
            ppConstantBuffersConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->VSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    delete[]( ppConstantBuffersConv );
}

void C11Device::VSSetConstantBuffers1( UINT StartSlot, UINT NumBuffers, CBuffer *const* ppConstantBuffers, UINT32 const* pOffsets, UINT32 const* ppNumConstants )
{
    assert( m_pDevice );
    ID3D11Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffers[j] )
            ppConstantBuffers[j]->GetInterfacePtr( &ppConstantBuffersConv[j] );
        else
            ppConstantBuffersConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();

    ID3D11DeviceContext1* pContext1 = nullptr;
    HRESULT hr;
    if( FAILED(hr = pDefContext->QueryInterface( __uuidof(ID3D11DeviceContext1), (void**)&pContext1 )) )
    {
        throw( WRAPPER_METHOD_UNSUPPORTED );
    }

    pContext1->VSSetConstantBuffers1(  StartSlot,  NumBuffers,  ppConstantBuffersConv, pOffsets, ppNumConstants );
	SAFE_RELEASE(pContext1);
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C11Device::PSSetShaderResources()
////////////////////////////////////////////////////////////
void C11Device::PSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D11ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D11ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViews[j] )
            ppShaderResourceViews[j]->GetInterfacePtr( &ppShaderResourceViewsConv[j] );
        else
            ppShaderResourceViewsConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->PSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C11Device::PSSetShader()
////////////////////////////////////////////////////////////
void C11Device::PSSetShader(  CPixelShader * pPixelShader )
{
    assert( m_pDevice );
    ID3D11PixelShader *temp_pPixelShader = NULL;
    if( pPixelShader != NULL )
    {
        D3DWrapperConvertInterface( &temp_pPixelShader, pPixelShader );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->PSSetShader(  temp_pPixelShader,  NULL,  NULL );
}


////////////////////////////////////////////////////////////
// C11Device::PSSetSamplers()
////////////////////////////////////////////////////////////
void C11Device::PSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    assert( m_pDevice );
    ID3D11SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D11SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplers[j] )
            ppSamplers[j]->GetInterfacePtr( &ppSamplersConv[j] );
        else
            ppSamplersConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->PSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C11Device::VSSetShader()
////////////////////////////////////////////////////////////
void C11Device::VSSetShader(  CVertexShader * pVertexShader )
{
    assert( m_pDevice );
    ID3D11VertexShader *temp_pVertexShader = NULL;
    if( pVertexShader != NULL )
    {
        D3DWrapperConvertInterface( &temp_pVertexShader, pVertexShader );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->VSSetShader(  temp_pVertexShader,  NULL,  NULL );
}


////////////////////////////////////////////////////////////
// C11Device::DrawIndexed()
////////////////////////////////////////////////////////////
void C11Device::DrawIndexed(  UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation )
{
    assert( m_pDevice );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->DrawIndexed(  IndexCount,  StartIndexLocation,  BaseVertexLocation );
}


////////////////////////////////////////////////////////////
// C11Device::Draw()
////////////////////////////////////////////////////////////
void C11Device::Draw(  UINT VertexCount, UINT StartVertexLocation )
{
    assert( m_pDevice );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->Draw(  VertexCount,  StartVertexLocation );
}


////////////////////////////////////////////////////////////
// C11Device::PSSetConstantBuffers()
////////////////////////////////////////////////////////////
void C11Device::PSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D11Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffers[j] )
            ppConstantBuffers[j]->GetInterfacePtr( &ppConstantBuffersConv[j] );
        else
            ppConstantBuffersConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->PSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    delete[]( ppConstantBuffersConv );
}

void C11Device::PSSetConstantBuffers1( UINT StartSlot, UINT NumBuffers, CBuffer *const* ppConstantBuffers, UINT32 const* pOffsets, UINT32 const* ppNumConstants )
{
    assert( m_pDevice );
    ID3D11Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffers[j] )
            ppConstantBuffers[j]->GetInterfacePtr( &ppConstantBuffersConv[j] );
        else
            ppConstantBuffersConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();

    ID3D11DeviceContext1* pContext1 = nullptr;
    HRESULT hr;
    if( FAILED(hr = pDefContext->QueryInterface( __uuidof(ID3D11DeviceContext1), (void**)&pContext1 )) )
    {
        throw( WRAPPER_METHOD_UNSUPPORTED );
    }

    pContext1->PSSetConstantBuffers1(  StartSlot,  NumBuffers,  ppConstantBuffersConv, pOffsets, ppNumConstants );
    SAFE_RELEASE(pContext1);
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C11Device::IASetInputLayout()
////////////////////////////////////////////////////////////
void C11Device::IASetInputLayout(  CInputLayout * pInputLayout )
{
    assert( m_pDevice );
    ID3D11InputLayout *temp_pInputLayout = NULL;
    if( pInputLayout != NULL )
    {
        D3DWrapperConvertInterface( &temp_pInputLayout, pInputLayout );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->IASetInputLayout(  temp_pInputLayout );
}


////////////////////////////////////////////////////////////
// C11Device::IASetVertexBuffers()
////////////////////////////////////////////////////////////
void C11Device::IASetVertexBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppVertexBuffers, const UINT * pStrides, const UINT * pOffsets )
{
    assert( m_pDevice );
    ID3D11Buffer **ppVertexBuffersConv = ppVertexBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppVertexBuffers && !ppVertexBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppVertexBuffers[j] )
            ppVertexBuffers[j]->GetInterfacePtr( &ppVertexBuffersConv[j] );
        else
            ppVertexBuffersConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->IASetVertexBuffers(  StartSlot,  NumBuffers,  ppVertexBuffersConv,  pStrides,  pOffsets );
    delete[]( ppVertexBuffersConv );
}


////////////////////////////////////////////////////////////
// C11Device::IASetIndexBuffer()
////////////////////////////////////////////////////////////
void C11Device::IASetIndexBuffer(  CBuffer * pIndexBuffer, DXGI_FORMAT Format, UINT Offset )
{
    assert( m_pDevice );
    ID3D11Buffer *temp_pIndexBuffer = NULL;
    if( pIndexBuffer != NULL )
    {
        D3DWrapperConvertInterface( &temp_pIndexBuffer, pIndexBuffer );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->IASetIndexBuffer(  temp_pIndexBuffer,  Format,  Offset );
}


////////////////////////////////////////////////////////////
// C11Device::DrawIndexedInstanced()
////////////////////////////////////////////////////////////
void C11Device::DrawIndexedInstanced(  UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation )
{
    assert( m_pDevice );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->DrawIndexedInstanced(  IndexCountPerInstance,  InstanceCount,  StartIndexLocation,  BaseVertexLocation,  StartInstanceLocation );
}


////////////////////////////////////////////////////////////
// C11Device::DrawInstanced()
////////////////////////////////////////////////////////////
void C11Device::DrawInstanced(  UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation )
{
    assert( m_pDevice );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->DrawInstanced(  VertexCountPerInstance,  InstanceCount,  StartVertexLocation,  StartInstanceLocation );
}


////////////////////////////////////////////////////////////
// C11Device::GSSetConstantBuffers()
////////////////////////////////////////////////////////////
void C11Device::GSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D11Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffers[j] )
            ppConstantBuffers[j]->GetInterfacePtr( &ppConstantBuffersConv[j] );
        else
            ppConstantBuffersConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->GSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// C11Device::GSSetShader()
////////////////////////////////////////////////////////////
void C11Device::GSSetShader(  CGeometryShader * pShader )
{
    assert( m_pDevice );
    ID3D11GeometryShader *temp_pShader = NULL;
    if( pShader != NULL )
    {
        D3DWrapperConvertInterface( &temp_pShader, pShader );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->GSSetShader(  temp_pShader,  NULL,  NULL );
}


////////////////////////////////////////////////////////////
// C11Device::IASetPrimitiveTopology()
////////////////////////////////////////////////////////////
void C11Device::IASetPrimitiveTopology(  D3D_PRIMITIVE_TOPOLOGY Topology )
{
    assert( m_pDevice );
    D3D11_PRIMITIVE_TOPOLOGY temp_d3d11_primitive_topology;
    D3DWrapperConvertEnum( &temp_d3d11_primitive_topology, Topology );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->IASetPrimitiveTopology(  temp_d3d11_primitive_topology );
}


////////////////////////////////////////////////////////////
// C11Device::VSSetShaderResources()
////////////////////////////////////////////////////////////
void C11Device::VSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D11ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D11ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViews[j] )
            ppShaderResourceViews[j]->GetInterfacePtr( &ppShaderResourceViewsConv[j] );
        else
            ppShaderResourceViewsConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->VSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C11Device::VSSetSamplers()
////////////////////////////////////////////////////////////
void C11Device::VSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    assert( m_pDevice );
    ID3D11SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D11SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplers[j] )
            ppSamplers[j]->GetInterfacePtr( &ppSamplersConv[j] );
        else
            ppSamplersConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->VSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C11Device::SetPredication()
////////////////////////////////////////////////////////////
void C11Device::SetPredication(  CPredicate * pPredicate, BOOL PredicateValue )
{
    assert( m_pDevice );
    ID3D11Predicate *temp_pPredicate = NULL;
    if( pPredicate != NULL )
    {
        D3DWrapperConvertInterface( &temp_pPredicate, pPredicate );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->SetPredication(  temp_pPredicate,  PredicateValue );
}


////////////////////////////////////////////////////////////
// C11Device::GSSetShaderResources()
////////////////////////////////////////////////////////////
void C11Device::GSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D11ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D11ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViews[j] )
            ppShaderResourceViews[j]->GetInterfacePtr( &ppShaderResourceViewsConv[j] );
        else
            ppShaderResourceViewsConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->GSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// C11Device::GSSetSamplers()
////////////////////////////////////////////////////////////
void C11Device::GSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    assert( m_pDevice );
    ID3D11SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D11SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplers[j] )
            ppSamplers[j]->GetInterfacePtr( &ppSamplersConv[j] );
        else
            ppSamplersConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->GSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// C11Device::OMSetRenderTargets()
////////////////////////////////////////////////////////////
void C11Device::OMSetRenderTargets(  UINT NumViews, CRenderTargetView *const * ppRenderTargetViews, CDepthStencilView * pDepthStencilView )
{
    assert( m_pDevice );
    ID3D11RenderTargetView **ppRenderTargetViewsConv = ppRenderTargetViews == NULL ? NULL : new ID3D11RenderTargetView*[NumViews];
    if( ppRenderTargetViews && !ppRenderTargetViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    ID3D11DepthStencilView *temp_pDepthStencilView = NULL;
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
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->OMSetRenderTargets(  NumViews,  ppRenderTargetViewsConv,  temp_pDepthStencilView );
    delete[]( ppRenderTargetViewsConv );
}


////////////////////////////////////////////////////////////
// C11Device::OMSetBlendState()
////////////////////////////////////////////////////////////
void C11Device::OMSetBlendState(  CBlendState * pBlendState, const FLOAT BlendFactor[4], UINT SampleMask )
{
    assert( m_pDevice );
    ID3D11BlendState *temp_pBlendState = NULL;
    if( pBlendState != NULL )
    {
        D3DWrapperConvertInterface( &temp_pBlendState, pBlendState );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->OMSetBlendState(  temp_pBlendState,  BlendFactor,  SampleMask );
}


////////////////////////////////////////////////////////////
// C11Device::OMSetDepthStencilState()
////////////////////////////////////////////////////////////
void C11Device::OMSetDepthStencilState(  CDepthStencilState * pDepthStencilState, UINT StencilRef )
{
    assert( m_pDevice );
    ID3D11DepthStencilState *temp_pDepthStencilState = NULL;
    if( pDepthStencilState != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDepthStencilState, pDepthStencilState );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->OMSetDepthStencilState(  temp_pDepthStencilState,  StencilRef );
}


////////////////////////////////////////////////////////////
// C11Device::SOSetTargets()
////////////////////////////////////////////////////////////
void C11Device::SOSetTargets(  UINT NumBuffers, CBuffer *const * ppSOTargets, const UINT * pOffsets )
{
    assert( m_pDevice );
    ID3D11Buffer **ppSOTargetsConv = ppSOTargets == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppSOTargets && !ppSOTargetsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppSOTargets[j] )
            ppSOTargets[j]->GetInterfacePtr( &ppSOTargetsConv[j] );
        else
            ppSOTargetsConv[j] = NULL;
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->SOSetTargets(  NumBuffers,  ppSOTargetsConv,  pOffsets );
    delete[]( ppSOTargetsConv );
}


////////////////////////////////////////////////////////////
// C11Device::DrawAuto()
////////////////////////////////////////////////////////////
void C11Device::DrawAuto(  )
{
    assert( m_pDevice );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->DrawAuto(  );
}


////////////////////////////////////////////////////////////
// C11Device::DrawIndexedInstancedIndirect()
////////////////////////////////////////////////////////////
void C11Device::DrawIndexedInstancedIndirect(  CBuffer * pBufferForArgs, UINT AlignedByteOffsetForArgs )
{
    assert( m_pDevice );
    ID3D11Buffer *temp_pBufferForArgs = NULL;
    if( pBufferForArgs != NULL )
    {
        D3DWrapperConvertInterface( &temp_pBufferForArgs, pBufferForArgs );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->DrawIndexedInstancedIndirect(  temp_pBufferForArgs,  AlignedByteOffsetForArgs );
}


////////////////////////////////////////////////////////////
// C11Device::DrawInstancedIndirect()
////////////////////////////////////////////////////////////
void C11Device::DrawInstancedIndirect(  CBuffer * pBufferForArgs, UINT AlignedByteOffsetForArgs )
{
    assert( m_pDevice );
    ID3D11Buffer *temp_pBufferForArgs = NULL;
    if( pBufferForArgs != NULL )
    {
        D3DWrapperConvertInterface( &temp_pBufferForArgs, pBufferForArgs );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->DrawInstancedIndirect(  temp_pBufferForArgs,  AlignedByteOffsetForArgs );
}


////////////////////////////////////////////////////////////
// C11Device::RSSetState()
////////////////////////////////////////////////////////////
void C11Device::RSSetState(  CRasterizerState * pRasterizerState )
{
    assert( m_pDevice );
    ID3D11RasterizerState *temp_pRasterizerState = NULL;
    if( pRasterizerState != NULL )
    {
        D3DWrapperConvertInterface( &temp_pRasterizerState, pRasterizerState );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->RSSetState(  temp_pRasterizerState );
}


////////////////////////////////////////////////////////////
// C11Device::RSSetViewports()
////////////////////////////////////////////////////////////
void C11Device::RSSetViewports(  UINT NumViewports, const D3D_VIEWPORT * pViewports )
{
    assert( m_pDevice );
    D3D11_VIEWPORT *pViewportsConv = pViewports == NULL ? NULL : new D3D11_VIEWPORT[NumViewports];
    if( pViewports && !pViewportsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    if( pViewports )
    {
        for( UINT i = NumViewports - 1; i != (UINT) -1; --i )
        {
            D3DWrapperConvertStruct( &pViewportsConv[i], &pViewports[i] );
        }
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->RSSetViewports(  NumViewports,  pViewportsConv );
    delete[]( pViewportsConv );
}


////////////////////////////////////////////////////////////
// C11Device::RSSetScissorRects()
////////////////////////////////////////////////////////////
void C11Device::RSSetScissorRects(  UINT NumRects, const D3D_RECT * pRects )
{
    assert( m_pDevice );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->RSSetScissorRects(  NumRects,  pRects );
}


////////////////////////////////////////////////////////////
// C11Device::CopySubresourceRegion()
////////////////////////////////////////////////////////////
void C11Device::CopySubresourceRegion(  CResource * pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, CResource * pSrcResource, UINT SrcSubresource, const D3D_BOX * pSrcBox )
{
    assert( m_pDevice );
    ID3D11Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    ID3D11Resource *temp_pSrcResource = NULL;
    if( pSrcResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pSrcResource, pSrcResource );
    }
    BYTE *pSrcBoxByteTemp = NULL;
    D3D11_BOX *pSrcBoxTemp = NULL;
    SIZE_T pSrcBoxSizeBuffer = sizeof(D3D11_BOX);
    if( pSrcBox != NULL && pSrcBoxSizeBuffer != 0 )
    {
        pSrcBoxByteTemp = new BYTE[pSrcBoxSizeBuffer];
        pSrcBoxTemp = (D3D11_BOX*)pSrcBoxByteTemp;
        D3DWrapperConvertStruct( pSrcBoxTemp, pSrcBox );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->CopySubresourceRegion(  temp_pDstResource,  DstSubresource,  DstX,  DstY,  DstZ,  temp_pSrcResource,  SrcSubresource,  pSrcBoxTemp );
    delete[] pSrcBoxByteTemp;
}


////////////////////////////////////////////////////////////
// C11Device::CopyResource()
////////////////////////////////////////////////////////////
void C11Device::CopyResource(  CResource * pDstResource, CResource * pSrcResource )
{
    assert( m_pDevice );
    ID3D11Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    ID3D11Resource *temp_pSrcResource = NULL;
    if( pSrcResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pSrcResource, pSrcResource );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->CopyResource(  temp_pDstResource,  temp_pSrcResource );
}


////////////////////////////////////////////////////////////
// C11Device::UpdateSubresource()
////////////////////////////////////////////////////////////
void C11Device::UpdateSubresource(  CResource * pDstResource, UINT DstSubresource, const D3D_BOX * pDstBox, const void * pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch )
{
    assert( m_pDevice );
    ID3D11Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    BYTE *pDstBoxByteTemp = NULL;
    D3D11_BOX *pDstBoxTemp = NULL;
    SIZE_T pDstBoxSizeBuffer = sizeof(D3D11_BOX);
    if( pDstBox != NULL && pDstBoxSizeBuffer != 0 )
    {
        pDstBoxByteTemp = new BYTE[pDstBoxSizeBuffer];
        pDstBoxTemp = (D3D11_BOX*)pDstBoxByteTemp;
        D3DWrapperConvertStruct( pDstBoxTemp, pDstBox );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->UpdateSubresource(  temp_pDstResource,  DstSubresource,  pDstBoxTemp,  pSrcData,  SrcRowPitch,  SrcDepthPitch );
    delete[] pDstBoxByteTemp;
}


////////////////////////////////////////////////////////////
// C11Device::ClearRenderTargetView()
////////////////////////////////////////////////////////////
void C11Device::ClearRenderTargetView(  CRenderTargetView * pRenderTargetView, const FLOAT ColorRGBA[4] )
{
    assert( m_pDevice );
    ID3D11RenderTargetView *temp_pRenderTargetView = NULL;
    if( pRenderTargetView != NULL )
    {
        D3DWrapperConvertInterface( &temp_pRenderTargetView, pRenderTargetView );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->ClearRenderTargetView(  temp_pRenderTargetView,  ColorRGBA );
}


////////////////////////////////////////////////////////////
// C11Device::ClearDepthStencilView()
////////////////////////////////////////////////////////////
void C11Device::ClearDepthStencilView(  CDepthStencilView * pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil )
{
    assert( m_pDevice );
    ID3D11DepthStencilView *temp_pDepthStencilView = NULL;
    if( pDepthStencilView != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDepthStencilView, pDepthStencilView );
    }
    UINT temp_ClearFlags;
    D3DFlagsConvertTo11_CLEAR_FLAG( &temp_ClearFlags, ClearFlags );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->ClearDepthStencilView(  temp_pDepthStencilView,  temp_ClearFlags,  Depth,  Stencil );
}


////////////////////////////////////////////////////////////
// C11Device::GenerateMips()
////////////////////////////////////////////////////////////
void C11Device::GenerateMips(  CShaderResourceView * pShaderResourceView )
{
    assert( m_pDevice );
    ID3D11ShaderResourceView *temp_pShaderResourceView = NULL;
    if( pShaderResourceView != NULL )
    {
        D3DWrapperConvertInterface( &temp_pShaderResourceView, pShaderResourceView );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->GenerateMips(  temp_pShaderResourceView );
}


////////////////////////////////////////////////////////////
// C11Device::SetResourceMinLOD()
////////////////////////////////////////////////////////////
void C11Device::SetResourceMinLOD(  CResource * pResource, FLOAT MinLOD )
{
    assert( m_pDevice );
    ID3D11Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->SetResourceMinLOD(  temp_pResource,  MinLOD );
}


////////////////////////////////////////////////////////////
// C11Device::GetResourceMinLOD()
////////////////////////////////////////////////////////////
FLOAT C11Device::GetResourceMinLOD(  CResource * pResource )
{
    assert( m_pDevice );
    FLOAT rval = 0;
    ID3D11Resource *temp_pResource = NULL;
    if( pResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pResource, pResource );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    rval = pDefContext->GetResourceMinLOD(  temp_pResource );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Device::ResolveSubresource()
////////////////////////////////////////////////////////////
void C11Device::ResolveSubresource(  CResource * pDstResource, UINT DstSubresource, CResource * pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format )
{
    assert( m_pDevice );
    ID3D11Resource *temp_pDstResource = NULL;
    if( pDstResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pDstResource, pDstResource );
    }
    ID3D11Resource *temp_pSrcResource = NULL;
    if( pSrcResource != NULL )
    {
        D3DWrapperConvertInterface( &temp_pSrcResource, pSrcResource );
    }
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->ResolveSubresource(  temp_pDstResource,  DstSubresource,  temp_pSrcResource,  SrcSubresource,  Format );
}


////////////////////////////////////////////////////////////
// C11Device::VSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C11Device::VSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D11Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->VSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C11Buffer( ppConstantBuffersConv[j] );
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
// C11Device::PSGetShaderResources()
////////////////////////////////////////////////////////////
void C11Device::PSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D11ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D11ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->PSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C11ShaderResourceView( ppShaderResourceViewsConv[j] );
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
// C11Device::PSGetShader()
////////////////////////////////////////////////////////////
void C11Device::PSGetShader(  CPixelShader ** ppPixelShader )
{
    assert( m_pDevice );
    ID3D11PixelShader *temp_pPixelShader = NULL;
    ID3D11PixelShader **ppPixelShaderTemp = ppPixelShader == NULL ? NULL : &temp_pPixelShader;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->PSGetShader(  ppPixelShaderTemp,  NULL,  NULL );
    if( ppPixelShader != NULL )
    {
        D3DWrapperConvertInterface( ppPixelShader, *ppPixelShaderTemp );
    }
}


////////////////////////////////////////////////////////////
// C11Device::PSGetSamplers()
////////////////////////////////////////////////////////////
void C11Device::PSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice );
    ID3D11SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D11SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->PSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C11SamplerState( ppSamplersConv[j] );
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
// C11Device::VSGetShader()
////////////////////////////////////////////////////////////
void C11Device::VSGetShader(  CVertexShader ** ppVertexShader )
{
    assert( m_pDevice );
    ID3D11VertexShader *temp_pVertexShader = NULL;
    ID3D11VertexShader **ppVertexShaderTemp = ppVertexShader == NULL ? NULL : &temp_pVertexShader;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->VSGetShader(  ppVertexShaderTemp,  NULL,  NULL );
    if( ppVertexShader != NULL )
    {
        D3DWrapperConvertInterface( ppVertexShader, *ppVertexShaderTemp );
    }
}


////////////////////////////////////////////////////////////
// C11Device::PSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C11Device::PSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D11Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->PSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C11Buffer( ppConstantBuffersConv[j] );
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
// C11Device::IAGetInputLayout()
////////////////////////////////////////////////////////////
void C11Device::IAGetInputLayout(  CInputLayout ** ppInputLayout )
{
    assert( m_pDevice );
    ID3D11InputLayout *temp_pInputLayout = NULL;
    ID3D11InputLayout **ppInputLayoutTemp = ppInputLayout == NULL ? NULL : &temp_pInputLayout;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->IAGetInputLayout(  ppInputLayoutTemp );
    if( ppInputLayout != NULL )
    {
        D3DWrapperConvertInterface( ppInputLayout, *ppInputLayoutTemp );
    }
}


////////////////////////////////////////////////////////////
// C11Device::IAGetVertexBuffers()
////////////////////////////////////////////////////////////
void C11Device::IAGetVertexBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppVertexBuffers, UINT * pStrides, UINT * pOffsets )
{
    assert( m_pDevice );
    ID3D11Buffer **ppVertexBuffersConv = ppVertexBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppVertexBuffers && !ppVertexBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->IAGetVertexBuffers(  StartSlot,  NumBuffers,  ppVertexBuffersConv,  pStrides,  pOffsets );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppVertexBuffersConv[j] )
        {
            ppVertexBuffers[j] = new C11Buffer( ppVertexBuffersConv[j] );
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
// C11Device::IAGetIndexBuffer()
////////////////////////////////////////////////////////////
void C11Device::IAGetIndexBuffer(  CBuffer ** pIndexBuffer, DXGI_FORMAT * Format, UINT * Offset )
{
    assert( m_pDevice );
    ID3D11Buffer *temp_IndexBuffer = NULL;
    ID3D11Buffer **pIndexBufferTemp = pIndexBuffer == NULL ? NULL : &temp_IndexBuffer;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->IAGetIndexBuffer(  pIndexBufferTemp,  Format,  Offset );
    if( pIndexBuffer != NULL )
    {
        D3DWrapperConvertInterface( pIndexBuffer, *pIndexBufferTemp );
    }
}


////////////////////////////////////////////////////////////
// C11Device::GSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C11Device::GSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D11Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->GSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C11Buffer( ppConstantBuffersConv[j] );
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
// C11Device::GSGetShader()
////////////////////////////////////////////////////////////
void C11Device::GSGetShader(  CGeometryShader ** ppGeometryShader )
{
    assert( m_pDevice );
    ID3D11GeometryShader *temp_pGeometryShader = NULL;
    ID3D11GeometryShader **ppGeometryShaderTemp = ppGeometryShader == NULL ? NULL : &temp_pGeometryShader;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->GSGetShader(  ppGeometryShaderTemp,  NULL,  NULL );
    if( ppGeometryShader != NULL )
    {
        D3DWrapperConvertInterface( ppGeometryShader, *ppGeometryShaderTemp );
    }
}


////////////////////////////////////////////////////////////
// C11Device::IAGetPrimitiveTopology()
////////////////////////////////////////////////////////////
void C11Device::IAGetPrimitiveTopology(  D3D_PRIMITIVE_TOPOLOGY * pTopology )
{
    assert( m_pDevice );
    D3D11_PRIMITIVE_TOPOLOGY temp_d3d11_primitive_topology;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->IAGetPrimitiveTopology(  &temp_d3d11_primitive_topology );
    D3DWrapperConvertEnum( pTopology, temp_d3d11_primitive_topology );
}


////////////////////////////////////////////////////////////
// C11Device::VSGetShaderResources()
////////////////////////////////////////////////////////////
void C11Device::VSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D11ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D11ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->VSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C11ShaderResourceView( ppShaderResourceViewsConv[j] );
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
// C11Device::VSGetSamplers()
////////////////////////////////////////////////////////////
void C11Device::VSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice );
    ID3D11SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D11SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->VSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C11SamplerState( ppSamplersConv[j] );
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
// C11Device::GetPredication()
////////////////////////////////////////////////////////////
void C11Device::GetPredication(  CPredicate ** ppPredicate, BOOL * pPredicateValue )
{
    assert( m_pDevice );
    ID3D11Predicate *temp_pPredicate = NULL;
    ID3D11Predicate **ppPredicateTemp = ppPredicate == NULL ? NULL : &temp_pPredicate;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->GetPredication(  ppPredicateTemp,  pPredicateValue );
    if( ppPredicate != NULL )
    {
        D3DWrapperConvertInterface( ppPredicate, *ppPredicateTemp );
    }
}


////////////////////////////////////////////////////////////
// C11Device::GSGetShaderResources()
////////////////////////////////////////////////////////////
void C11Device::GSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D11ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D11ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->GSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C11ShaderResourceView( ppShaderResourceViewsConv[j] );
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
// C11Device::GSGetSamplers()
////////////////////////////////////////////////////////////
void C11Device::GSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice );
    ID3D11SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D11SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->GSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C11SamplerState( ppSamplersConv[j] );
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
// C11Device::OMGetRenderTargets()
////////////////////////////////////////////////////////////
void C11Device::OMGetRenderTargets(  UINT NumViews, CRenderTargetView ** ppRenderTargetViews, CDepthStencilView ** ppDepthStencilView )
{
    assert( m_pDevice );
    ID3D11RenderTargetView **ppRenderTargetViewsConv = ppRenderTargetViews == NULL ? NULL : new ID3D11RenderTargetView*[NumViews];
    if( ppRenderTargetViews && !ppRenderTargetViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    ID3D11DepthStencilView *temp_pDepthStencilView = NULL;
    ID3D11DepthStencilView **ppDepthStencilViewTemp = ppDepthStencilView == NULL ? NULL : &temp_pDepthStencilView;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->OMGetRenderTargets(  NumViews,  ppRenderTargetViewsConv,  ppDepthStencilViewTemp );
    if( ppDepthStencilView != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilView, *ppDepthStencilViewTemp );
    }
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppRenderTargetViewsConv[j] )
        {
            ppRenderTargetViews[j] = new C11RenderTargetView( ppRenderTargetViewsConv[j] );
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
// C11Device::OMGetBlendState()
////////////////////////////////////////////////////////////
void C11Device::OMGetBlendState(  CBlendState ** ppBlendState, FLOAT BlendFactor[4], UINT * pSampleMask )
{
    assert( m_pDevice );
    ID3D11BlendState *temp_pBlendState = NULL;
    ID3D11BlendState **ppBlendStateTemp = ppBlendState == NULL ? NULL : &temp_pBlendState;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->OMGetBlendState(  ppBlendStateTemp,  BlendFactor,  pSampleMask );
    if( ppBlendState != NULL )
    {
        D3DWrapperConvertInterface( ppBlendState, *ppBlendStateTemp );
    }
}


////////////////////////////////////////////////////////////
// C11Device::OMGetDepthStencilState()
////////////////////////////////////////////////////////////
void C11Device::OMGetDepthStencilState(  CDepthStencilState ** ppDepthStencilState, UINT * pStencilRef )
{
    assert( m_pDevice );
    ID3D11DepthStencilState *temp_pDepthStencilState = NULL;
    ID3D11DepthStencilState **ppDepthStencilStateTemp = ppDepthStencilState == NULL ? NULL : &temp_pDepthStencilState;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->OMGetDepthStencilState(  ppDepthStencilStateTemp,  pStencilRef );
    if( ppDepthStencilState != NULL )
    {
        D3DWrapperConvertInterface( ppDepthStencilState, *ppDepthStencilStateTemp );
    }
}


////////////////////////////////////////////////////////////
// C11Device::SOGetTargets()
////////////////////////////////////////////////////////////
void C11Device::SOGetTargets(  UINT NumBuffers, CBuffer ** ppSOTargets )
{
    assert( m_pDevice );
    ID3D11Buffer **ppSOTargetsConv = ppSOTargets == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppSOTargets && !ppSOTargetsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->SOGetTargets(  NumBuffers,  ppSOTargetsConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppSOTargetsConv[j] )
        {
            ppSOTargets[j] = new C11Buffer( ppSOTargetsConv[j] );
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
// C11Device::RSGetState()
////////////////////////////////////////////////////////////
void C11Device::RSGetState(  CRasterizerState ** ppRasterizerState )
{
    assert( m_pDevice );
    ID3D11RasterizerState *temp_pRasterizerState = NULL;
    ID3D11RasterizerState **ppRasterizerStateTemp = ppRasterizerState == NULL ? NULL : &temp_pRasterizerState;
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->RSGetState(  ppRasterizerStateTemp );
    if( ppRasterizerState != NULL )
    {
        D3DWrapperConvertInterface( ppRasterizerState, *ppRasterizerStateTemp );
    }
}


////////////////////////////////////////////////////////////
// C11Device::RSGetViewports()
////////////////////////////////////////////////////////////
void C11Device::RSGetViewports(  UINT * pNumViewports, D3D_VIEWPORT * pViewports )
{
    assert( m_pDevice );
    D3D11_VIEWPORT *pViewportsConv = pViewports == NULL ? NULL : new D3D11_VIEWPORT[*pNumViewports];
    if( pViewports && !pViewportsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->RSGetViewports(  pNumViewports,  pViewportsConv );
    for( UINT i = *pNumViewports - 1; i != (UINT) -1; --i )
        D3DWrapperConvertStruct( &pViewports[i], &pViewportsConv[i] );
}


////////////////////////////////////////////////////////////
// C11Device::RSGetScissorRects()
////////////////////////////////////////////////////////////
void C11Device::RSGetScissorRects(  UINT * pNumRects, D3D_RECT * pRects )
{
    assert( m_pDevice );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->RSGetScissorRects(  pNumRects,  pRects );
}


////////////////////////////////////////////////////////////
// C11Device::HSGetShaderResources()
////////////////////////////////////////////////////////////
void C11Device::HSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D11ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D11ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->HSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C11ShaderResourceView( ppShaderResourceViewsConv[j] );
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
// C11Device::HSGetSamplers()
////////////////////////////////////////////////////////////
void C11Device::HSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice );
    ID3D11SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D11SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->HSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C11SamplerState( ppSamplersConv[j] );
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
// C11Device::HSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C11Device::HSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D11Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->HSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C11Buffer( ppConstantBuffersConv[j] );
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
// C11Device::DSGetShaderResources()
////////////////////////////////////////////////////////////
void C11Device::DSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D11ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D11ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->DSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C11ShaderResourceView( ppShaderResourceViewsConv[j] );
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
// C11Device::DSGetSamplers()
////////////////////////////////////////////////////////////
void C11Device::DSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice );
    ID3D11SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D11SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->DSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C11SamplerState( ppSamplersConv[j] );
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
// C11Device::DSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C11Device::DSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D11Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->DSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C11Buffer( ppConstantBuffersConv[j] );
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
// C11Device::CSGetShaderResources()
////////////////////////////////////////////////////////////
void C11Device::CSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    assert( m_pDevice );
    ID3D11ShaderResourceView **ppShaderResourceViewsConv = ppShaderResourceViews == NULL ? NULL : new ID3D11ShaderResourceView*[NumViews];
    if( ppShaderResourceViews && !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->CSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    for( int j = NumViews - 1; j != -1; --j )
    {
        if( ppShaderResourceViewsConv[j] )
        {
            ppShaderResourceViews[j] = new C11ShaderResourceView( ppShaderResourceViewsConv[j] );
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
// C11Device::CSGetSamplers()
////////////////////////////////////////////////////////////
void C11Device::CSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    assert( m_pDevice );
    ID3D11SamplerState **ppSamplersConv = ppSamplers == NULL ? NULL : new ID3D11SamplerState*[NumSamplers];
    if( ppSamplers && !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->CSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    for( int j = NumSamplers - 1; j != -1; --j )
    {
        if( ppSamplersConv[j] )
        {
            ppSamplers[j] = new C11SamplerState( ppSamplersConv[j] );
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
// C11Device::CSGetConstantBuffers()
////////////////////////////////////////////////////////////
void C11Device::CSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    assert( m_pDevice );
    ID3D11Buffer **ppConstantBuffersConv = ppConstantBuffers == NULL ? NULL : new ID3D11Buffer*[NumBuffers];
    if( ppConstantBuffers && !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->CSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    for( int j = NumBuffers - 1; j != -1; --j )
    {
        if( ppConstantBuffersConv[j] )
        {
            ppConstantBuffers[j] = new C11Buffer( ppConstantBuffersConv[j] );
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
// C11Device::ClearState()
////////////////////////////////////////////////////////////
void C11Device::ClearState(  )
{
    assert( m_pDevice );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->ClearState(  );
}


////////////////////////////////////////////////////////////
// C11Device::Flush()
////////////////////////////////////////////////////////////
void C11Device::Flush(  )
{
    assert( m_pDevice );
    C11Device* pTempDevice11 = NULL;
    pTempDevice11 = this;
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->Flush(  );
}


////////////////////////////////////////////////////////////
//
// C11Debug
//
////////////////////////////////////////////////////////////

C11Debug::C11Debug( ID3D11Debug *pDebug ) : 
    m_pDebug( pDebug )
{
}


C11Debug::~C11Debug()
{
    if( m_pDebug ) m_pDebug->Release();
}


////////////////////////////////////////////////////////////
// C11Debug::SetFeatureMask()
////////////////////////////////////////////////////////////
NRESULT C11Debug::SetFeatureMask(  UINT Mask )
{
    assert( m_pDebug );
    NRESULT rval = S_OK;
    rval = m_pDebug->SetFeatureMask(  Mask );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Debug::GetFeatureMask()
////////////////////////////////////////////////////////////
UINT C11Debug::GetFeatureMask(  )
{
    assert( m_pDebug );
    UINT rval = 0;
    rval = m_pDebug->GetFeatureMask(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Debug::SetPresentPerRenderOpDelay()
////////////////////////////////////////////////////////////
NRESULT C11Debug::SetPresentPerRenderOpDelay(  UINT Milliseconds )
{
    assert( m_pDebug );
    NRESULT rval = S_OK;
    rval = m_pDebug->SetPresentPerRenderOpDelay(  Milliseconds );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Debug::GetPresentPerRenderOpDelay()
////////////////////////////////////////////////////////////
UINT C11Debug::GetPresentPerRenderOpDelay(  )
{
    assert( m_pDebug );
    UINT rval = 0;
    rval = m_pDebug->GetPresentPerRenderOpDelay(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Debug::SetSwapChain()
////////////////////////////////////////////////////////////
NRESULT C11Debug::SetSwapChain(  IDXGISwapChain * pSwapChain )
{
    assert( m_pDebug );
    NRESULT rval = S_OK;
    rval = m_pDebug->SetSwapChain(  pSwapChain );
    return rval;
}


////////////////////////////////////////////////////////////
// C11Debug::GetSwapChain()
////////////////////////////////////////////////////////////
NRESULT C11Debug::GetSwapChain(  IDXGISwapChain ** ppSwapChain )
{
    assert( m_pDebug );
    NRESULT rval = S_OK;
    rval = m_pDebug->GetSwapChain(  ppSwapChain );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11SwitchToRef
//
////////////////////////////////////////////////////////////

C11SwitchToRef::C11SwitchToRef( ID3D11SwitchToRef *pSwitchToRef ) : 
    m_pSwitchToRef( pSwitchToRef )
{
}


C11SwitchToRef::~C11SwitchToRef()
{
    if( m_pSwitchToRef ) m_pSwitchToRef->Release();
}


////////////////////////////////////////////////////////////
// C11SwitchToRef::SetUseRef()
////////////////////////////////////////////////////////////
BOOL C11SwitchToRef::SetUseRef(  BOOL UseRef )
{
    assert( m_pSwitchToRef );
    BOOL rval = 0;
    rval = m_pSwitchToRef->SetUseRef(  UseRef );
    return rval;
}


////////////////////////////////////////////////////////////
// C11SwitchToRef::GetUseRef()
////////////////////////////////////////////////////////////
BOOL C11SwitchToRef::GetUseRef(  )
{
    assert( m_pSwitchToRef );
    BOOL rval = 0;
    rval = m_pSwitchToRef->GetUseRef(  );
    return rval;
}


////////////////////////////////////////////////////////////
//
// C11InfoQueue
//
////////////////////////////////////////////////////////////

C11InfoQueue::C11InfoQueue( ID3D11InfoQueue *pInfoQueue ) : 
    m_pInfoQueue( pInfoQueue )
{
}


C11InfoQueue::~C11InfoQueue()
{
    if( m_pInfoQueue ) m_pInfoQueue->Release();
}


////////////////////////////////////////////////////////////
// C11InfoQueue::SetMessageCountLimit()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::SetMessageCountLimit(  UINT64 MessageCountLimit )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    rval = m_pInfoQueue->SetMessageCountLimit(  MessageCountLimit );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::ClearStoredMessages()
////////////////////////////////////////////////////////////
void C11InfoQueue::ClearStoredMessages(  )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->ClearStoredMessages(  );
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetMessage()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::GetMessage(  UINT64 MessageIndex, D3D_MESSAGE * pMessage, SIZE_T * pMessageByteLength )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pMessageByteTemp = NULL;
    D3D11_MESSAGE *pMessageTemp = NULL;
    SIZE_T pMessageSizeBuffer = *pMessageByteLength;
    if( pMessage != NULL && pMessageSizeBuffer != 0 )
    {
        pMessageByteTemp = new BYTE[pMessageSizeBuffer];
        pMessageTemp = (D3D11_MESSAGE*)pMessageByteTemp;
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
// C11InfoQueue::GetNumMessagesAllowedByStorageFilter()
////////////////////////////////////////////////////////////
UINT64 C11InfoQueue::GetNumMessagesAllowedByStorageFilter(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetNumMessagesAllowedByStorageFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetNumMessagesDeniedByStorageFilter()
////////////////////////////////////////////////////////////
UINT64 C11InfoQueue::GetNumMessagesDeniedByStorageFilter(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetNumMessagesDeniedByStorageFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetNumStoredMessages()
////////////////////////////////////////////////////////////
UINT64 C11InfoQueue::GetNumStoredMessages(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetNumStoredMessages(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetNumStoredMessagesAllowedByRetrievalFilter()
////////////////////////////////////////////////////////////
UINT64 C11InfoQueue::GetNumStoredMessagesAllowedByRetrievalFilter(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetNumMessagesDiscardedByMessageCountLimit()
////////////////////////////////////////////////////////////
UINT64 C11InfoQueue::GetNumMessagesDiscardedByMessageCountLimit(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetNumMessagesDiscardedByMessageCountLimit(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetMessageCountLimit()
////////////////////////////////////////////////////////////
UINT64 C11InfoQueue::GetMessageCountLimit(  )
{
    assert( m_pInfoQueue );
    UINT64 rval = 0;
    rval = m_pInfoQueue->GetMessageCountLimit(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::AddStorageFilterEntries()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::AddStorageFilterEntries(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D11_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = sizeof(D3D11_INFO_QUEUE_FILTER);
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D11_INFO_QUEUE_FILTER*)pFilterByteTemp;
        D3DWrapperConvertStruct( pFilterTemp, pFilter );
    }
    rval = m_pInfoQueue->AddStorageFilterEntries(  pFilterTemp );
    delete[] pFilterByteTemp;
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetStorageFilter()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::GetStorageFilter(  D3D_INFO_QUEUE_FILTER * pFilter, SIZE_T * pFilterByteLength )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D11_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = *pFilterByteLength;
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D11_INFO_QUEUE_FILTER*)pFilterByteTemp;
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
// C11InfoQueue::ClearStorageFilter()
////////////////////////////////////////////////////////////
void C11InfoQueue::ClearStorageFilter(  )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->ClearStorageFilter(  );
}


////////////////////////////////////////////////////////////
// C11InfoQueue::PushEmptyStorageFilter()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::PushEmptyStorageFilter(  )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    rval = m_pInfoQueue->PushEmptyStorageFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::PushCopyOfStorageFilter()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::PushCopyOfStorageFilter(  )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    rval = m_pInfoQueue->PushCopyOfStorageFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::PushStorageFilter()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::PushStorageFilter(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D11_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = sizeof(D3D11_INFO_QUEUE_FILTER);
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D11_INFO_QUEUE_FILTER*)pFilterByteTemp;
        D3DWrapperConvertStruct( pFilterTemp, pFilter );
    }
    rval = m_pInfoQueue->PushStorageFilter(  pFilterTemp );
    delete[] pFilterByteTemp;
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::PopStorageFilter()
////////////////////////////////////////////////////////////
void C11InfoQueue::PopStorageFilter(  )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->PopStorageFilter(  );
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetStorageFilterStackSize()
////////////////////////////////////////////////////////////
UINT C11InfoQueue::GetStorageFilterStackSize(  )
{
    assert( m_pInfoQueue );
    UINT rval = 0;
    rval = m_pInfoQueue->GetStorageFilterStackSize(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::AddRetrievalFilterEntries()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::AddRetrievalFilterEntries(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D11_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = sizeof(D3D11_INFO_QUEUE_FILTER);
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D11_INFO_QUEUE_FILTER*)pFilterByteTemp;
        D3DWrapperConvertStruct( pFilterTemp, pFilter );
    }
    rval = m_pInfoQueue->AddRetrievalFilterEntries(  pFilterTemp );
    delete[] pFilterByteTemp;
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::GetRetrievalFilter(  D3D_INFO_QUEUE_FILTER * pFilter, SIZE_T * pFilterByteLength )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D11_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = *pFilterByteLength;
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D11_INFO_QUEUE_FILTER*)pFilterByteTemp;
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
// C11InfoQueue::ClearRetrievalFilter()
////////////////////////////////////////////////////////////
void C11InfoQueue::ClearRetrievalFilter(  )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->ClearRetrievalFilter(  );
}


////////////////////////////////////////////////////////////
// C11InfoQueue::PushEmptyRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::PushEmptyRetrievalFilter(  )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    rval = m_pInfoQueue->PushEmptyRetrievalFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::PushCopyOfRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::PushCopyOfRetrievalFilter(  )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    rval = m_pInfoQueue->PushCopyOfRetrievalFilter(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::PushRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::PushRetrievalFilter(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    BYTE *pFilterByteTemp = NULL;
    D3D11_INFO_QUEUE_FILTER *pFilterTemp = NULL;
    SIZE_T pFilterSizeBuffer = sizeof(D3D11_INFO_QUEUE_FILTER);
    if( pFilter != NULL && pFilterSizeBuffer != 0 )
    {
        pFilterByteTemp = new BYTE[pFilterSizeBuffer];
        pFilterTemp = (D3D11_INFO_QUEUE_FILTER*)pFilterByteTemp;
        D3DWrapperConvertStruct( pFilterTemp, pFilter );
    }
    rval = m_pInfoQueue->PushRetrievalFilter(  pFilterTemp );
    delete[] pFilterByteTemp;
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::PopRetrievalFilter()
////////////////////////////////////////////////////////////
void C11InfoQueue::PopRetrievalFilter(  )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->PopRetrievalFilter(  );
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetRetrievalFilterStackSize()
////////////////////////////////////////////////////////////
UINT C11InfoQueue::GetRetrievalFilterStackSize(  )
{
    assert( m_pInfoQueue );
    UINT rval = 0;
    rval = m_pInfoQueue->GetRetrievalFilterStackSize(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::AddMessage()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::AddMessage(  D3D_MESSAGE_CATEGORY Category, D3D_MESSAGE_SEVERITY Severity, D3D_MESSAGE_ID ID, LPCSTR pDescription )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    D3D11_MESSAGE_CATEGORY temp_d3d11_message_category;
    D3DWrapperConvertEnum( &temp_d3d11_message_category, Category );
    D3D11_MESSAGE_SEVERITY temp_d3d11_message_severity;
    D3DWrapperConvertEnum( &temp_d3d11_message_severity, Severity );
    D3D11_MESSAGE_ID temp_d3d11_message_id;
    D3DWrapperConvertEnum( &temp_d3d11_message_id, ID );
    rval = m_pInfoQueue->AddMessage(  temp_d3d11_message_category,  temp_d3d11_message_severity,  temp_d3d11_message_id,  pDescription );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::AddApplicationMessage()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::AddApplicationMessage(  D3D_MESSAGE_SEVERITY Severity, LPCSTR pDescription )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    D3D11_MESSAGE_SEVERITY temp_d3d11_message_severity;
    D3DWrapperConvertEnum( &temp_d3d11_message_severity, Severity );
    rval = m_pInfoQueue->AddApplicationMessage(  temp_d3d11_message_severity,  pDescription );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::SetBreakOnCategory()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::SetBreakOnCategory(  D3D_MESSAGE_CATEGORY Category, BOOL bEnable )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    D3D11_MESSAGE_CATEGORY temp_d3d11_message_category;
    D3DWrapperConvertEnum( &temp_d3d11_message_category, Category );
    rval = m_pInfoQueue->SetBreakOnCategory(  temp_d3d11_message_category,  bEnable );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::SetBreakOnSeverity()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::SetBreakOnSeverity(  D3D_MESSAGE_SEVERITY Severity, BOOL bEnable )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    D3D11_MESSAGE_SEVERITY temp_d3d11_message_severity;
    D3DWrapperConvertEnum( &temp_d3d11_message_severity, Severity );
    rval = m_pInfoQueue->SetBreakOnSeverity(  temp_d3d11_message_severity,  bEnable );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::SetBreakOnID()
////////////////////////////////////////////////////////////
NRESULT C11InfoQueue::SetBreakOnID(  D3D_MESSAGE_ID ID, BOOL bEnable )
{
    assert( m_pInfoQueue );
    NRESULT rval = S_OK;
    D3D11_MESSAGE_ID temp_d3d11_message_id;
    D3DWrapperConvertEnum( &temp_d3d11_message_id, ID );
    rval = m_pInfoQueue->SetBreakOnID(  temp_d3d11_message_id,  bEnable );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetBreakOnCategory()
////////////////////////////////////////////////////////////
BOOL C11InfoQueue::GetBreakOnCategory(  D3D_MESSAGE_CATEGORY Category )
{
    assert( m_pInfoQueue );
    BOOL rval = 0;
    D3D11_MESSAGE_CATEGORY temp_d3d11_message_category;
    D3DWrapperConvertEnum( &temp_d3d11_message_category, Category );
    rval = m_pInfoQueue->GetBreakOnCategory(  temp_d3d11_message_category );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetBreakOnSeverity()
////////////////////////////////////////////////////////////
BOOL C11InfoQueue::GetBreakOnSeverity(  D3D_MESSAGE_SEVERITY Severity )
{
    assert( m_pInfoQueue );
    BOOL rval = 0;
    D3D11_MESSAGE_SEVERITY temp_d3d11_message_severity;
    D3DWrapperConvertEnum( &temp_d3d11_message_severity, Severity );
    rval = m_pInfoQueue->GetBreakOnSeverity(  temp_d3d11_message_severity );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetBreakOnID()
////////////////////////////////////////////////////////////
BOOL C11InfoQueue::GetBreakOnID(  D3D_MESSAGE_ID ID )
{
    assert( m_pInfoQueue );
    BOOL rval = 0;
    D3D11_MESSAGE_ID temp_d3d11_message_id;
    D3DWrapperConvertEnum( &temp_d3d11_message_id, ID );
    rval = m_pInfoQueue->GetBreakOnID(  temp_d3d11_message_id );
    return rval;
}


////////////////////////////////////////////////////////////
// C11InfoQueue::SetMuteDebugOutput()
////////////////////////////////////////////////////////////
void C11InfoQueue::SetMuteDebugOutput(  BOOL bMute )
{
    assert( m_pInfoQueue );
    m_pInfoQueue->SetMuteDebugOutput(  bMute );
}


////////////////////////////////////////////////////////////
// C11InfoQueue::GetMuteDebugOutput()
////////////////////////////////////////////////////////////
BOOL C11InfoQueue::GetMuteDebugOutput(  )
{
    assert( m_pInfoQueue );
    BOOL rval = 0;
    rval = m_pInfoQueue->GetMuteDebugOutput(  );
    return rval;
}
