//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3DMultiWrapped.cpp
//* 
//* Description: 
//*     Multi-Interface D3D wrapper container class definitions.
//*
//*****************************************************************************

// Ignore Prefast warnings 26060 (Range Precondition violation) and 28930 (Unused assignment of pointer variable)
#pragma warning( disable: 26060 )
#pragma warning( disable: 28930 )

#include "pch.h"
#include "D3DWrappedTypes.h"
#include "D3DMultiWrapped.h"


////////////////////////////////////////////////////////////
//
// Wrapper classes
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// CMultiDeviceChild
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// CMultiDeviceChildGeneric
//
////////////////////////////////////////////////////////////

class CMultiDeviceChildGeneric : public CMultiDeviceChild
{
    CDeviceChild **m_pDeviceChildPtrArray;
    int m_nDeviceChildCount;

public:
    CMultiDeviceChildGeneric( int nCount );
    CMultiDeviceChildGeneric( CDeviceChild **pDeviceChildPtrArray, int nCount );
    ~CMultiDeviceChildGeneric();

    ////////////////////////////////////////////////////////////
    // Sub object accessors
    ////////////////////////////////////////////////////////////
    int GetSubObjectCount() const { return m_nDeviceChildCount; }
    void SetSubDeviceChild( int index, CDeviceChild *pDeviceChild );
    virtual CDeviceChild *Src() { return GetSubDeviceChild( 0 ); }
    virtual CDeviceChild *Ref() { return GetSubDeviceChild( 1 ); }

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    void GetInterfacePtr( ID3D10DeviceChild **pInterface ) { m_pDeviceChildPtrArray[0]->GetInterfacePtr( (ID3D10DeviceChild **) pInterface ); }
    void GetInterfacePtr( ID3D11DeviceChild **pInterface ) { m_pDeviceChildPtrArray[0]->GetInterfacePtr( (ID3D11DeviceChild **) pInterface ); }
    CDeviceChild *GetSubDeviceChild( int index ) { return m_pDeviceChildPtrArray[index]; }

    ////////////////////////////////////////////////////////////
    // GetDevice
    ////////////////////////////////////////////////////////////
    void GetDevice(  CDevice ** ppDevice );

    ////////////////////////////////////////////////////////////
    // GetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateDataInterface
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////
};


////////////////////////////////////////////////////////////
//
// CMultiDeviceChildGeneric
//
////////////////////////////////////////////////////////////

CMultiDeviceChildGeneric::CMultiDeviceChildGeneric( int nCount ) : 
    m_nDeviceChildCount( nCount )
{
    if( nCount > 0 )
    {
        m_pDeviceChildPtrArray = new CDeviceChild*[nCount];
        if( !m_pDeviceChildPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pDeviceChildPtrArray[i] = NULL;
    }
}


CMultiDeviceChildGeneric::CMultiDeviceChildGeneric( CDeviceChild **pDeviceChildPtrArray, int nCount ) : 
    m_nDeviceChildCount( nCount )
{
    if( nCount > 0 )
    {
        m_pDeviceChildPtrArray = new CDeviceChild*[nCount];
        if( !m_pDeviceChildPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pDeviceChildPtrArray[i] = pDeviceChildPtrArray[i];
            m_pDeviceChildPtrArray[i]->AddRef();
        }
    }
}


CMultiDeviceChildGeneric::~CMultiDeviceChildGeneric()
{
    if( m_pDeviceChildPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pDeviceChildPtrArray[i] ) m_pDeviceChildPtrArray[i]->CDeviceChild::Release();
        delete[]( m_pDeviceChildPtrArray );
    }
}


void CMultiDeviceChildGeneric::SetSubDeviceChild( int index, CDeviceChild *pDeviceChild )
{
    if( pDeviceChild ) pDeviceChild->AddRef();
    if( m_pDeviceChildPtrArray[index] )
        m_pDeviceChildPtrArray[index]->Release();
    m_pDeviceChildPtrArray[index] = pDeviceChild;
}




////////////////////////////////////////////////////////////
// CMultiDeviceChildGeneric::GetDevice()
////////////////////////////////////////////////////////////
void CMultiDeviceChildGeneric::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDeviceChildGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiDeviceChildGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDeviceChildGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiDeviceChildGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDeviceChildGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiDeviceChildGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiDepthStencilState
//
////////////////////////////////////////////////////////////

CMultiDepthStencilState::CMultiDepthStencilState( int nCount ) : 
    m_nDepthStencilStateCount( nCount )
{
    if( nCount > 0 )
    {
        m_pDepthStencilStatePtrArray = new CDepthStencilState*[nCount];
        if( !m_pDepthStencilStatePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pDepthStencilStatePtrArray[i] = NULL;
    }
}


CMultiDepthStencilState::CMultiDepthStencilState( CDepthStencilState **pDepthStencilStatePtrArray, int nCount ) : 
    m_nDepthStencilStateCount( nCount )
{
    if( nCount > 0 )
    {
        m_pDepthStencilStatePtrArray = new CDepthStencilState*[nCount];
        if( !m_pDepthStencilStatePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pDepthStencilStatePtrArray[i] = pDepthStencilStatePtrArray[i];
            m_pDepthStencilStatePtrArray[i]->AddRef();
        }
    }
}


CMultiDepthStencilState::~CMultiDepthStencilState()
{
    if( m_pDepthStencilStatePtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pDepthStencilStatePtrArray[i] ) m_pDepthStencilStatePtrArray[i]->CDepthStencilState::Release();
        delete[]( m_pDepthStencilStatePtrArray );
    }
}


void CMultiDepthStencilState::SetSubDepthStencilState( int index, CDepthStencilState *pDepthStencilState )
{
    if( pDepthStencilState ) pDepthStencilState->AddRef();
    if( m_pDepthStencilStatePtrArray[index] )
        m_pDepthStencilStatePtrArray[index]->Release();
    m_pDepthStencilStatePtrArray[index] = pDepthStencilState;
}




////////////////////////////////////////////////////////////
// CMultiDepthStencilState::GetDesc()
////////////////////////////////////////////////////////////
void CMultiDepthStencilState::GetDesc(  D3D_DEPTH_STENCIL_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDepthStencilState( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiDepthStencilState::GetDevice()
////////////////////////////////////////////////////////////
void CMultiDepthStencilState::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDepthStencilState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiDepthStencilState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDepthStencilState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiDepthStencilState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDepthStencilState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiDepthStencilState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiBlendState
//
////////////////////////////////////////////////////////////

CMultiBlendState::CMultiBlendState( int nCount ) : 
    m_nBlendStateCount( nCount )
{
    if( nCount > 0 )
    {
        m_pBlendStatePtrArray = new CBlendState*[nCount];
        if( !m_pBlendStatePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pBlendStatePtrArray[i] = NULL;
    }
}


CMultiBlendState::CMultiBlendState( CBlendState **pBlendStatePtrArray, int nCount ) : 
    m_nBlendStateCount( nCount )
{
    if( nCount > 0 )
    {
        m_pBlendStatePtrArray = new CBlendState*[nCount];
        if( !m_pBlendStatePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pBlendStatePtrArray[i] = pBlendStatePtrArray[i];
            m_pBlendStatePtrArray[i]->AddRef();
        }
    }
}


CMultiBlendState::~CMultiBlendState()
{
    if( m_pBlendStatePtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pBlendStatePtrArray[i] ) m_pBlendStatePtrArray[i]->CBlendState::Release();
        delete[]( m_pBlendStatePtrArray );
    }
}


void CMultiBlendState::SetSubBlendState( int index, CBlendState *pBlendState )
{
    if( pBlendState ) pBlendState->AddRef();
    if( m_pBlendStatePtrArray[index] )
        m_pBlendStatePtrArray[index]->Release();
    m_pBlendStatePtrArray[index] = pBlendState;
}




////////////////////////////////////////////////////////////
// CMultiBlendState::GetDesc()
////////////////////////////////////////////////////////////
void CMultiBlendState::GetDesc(  D3D_BLEND_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubBlendState( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiBlendState::GetDevice()
////////////////////////////////////////////////////////////
void CMultiBlendState::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiBlendState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiBlendState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiBlendState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiBlendState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiBlendState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiBlendState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiRasterizerState
//
////////////////////////////////////////////////////////////

CMultiRasterizerState::CMultiRasterizerState( int nCount ) : 
    m_nRasterizerStateCount( nCount )
{
    if( nCount > 0 )
    {
        m_pRasterizerStatePtrArray = new CRasterizerState*[nCount];
        if( !m_pRasterizerStatePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pRasterizerStatePtrArray[i] = NULL;
    }
}


CMultiRasterizerState::CMultiRasterizerState( CRasterizerState **pRasterizerStatePtrArray, int nCount ) : 
    m_nRasterizerStateCount( nCount )
{
    if( nCount > 0 )
    {
        m_pRasterizerStatePtrArray = new CRasterizerState*[nCount];
        if( !m_pRasterizerStatePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pRasterizerStatePtrArray[i] = pRasterizerStatePtrArray[i];
            m_pRasterizerStatePtrArray[i]->AddRef();
        }
    }
}


CMultiRasterizerState::~CMultiRasterizerState()
{
    if( m_pRasterizerStatePtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pRasterizerStatePtrArray[i] ) m_pRasterizerStatePtrArray[i]->CRasterizerState::Release();
        delete[]( m_pRasterizerStatePtrArray );
    }
}


void CMultiRasterizerState::SetSubRasterizerState( int index, CRasterizerState *pRasterizerState )
{
    if( pRasterizerState ) pRasterizerState->AddRef();
    if( m_pRasterizerStatePtrArray[index] )
        m_pRasterizerStatePtrArray[index]->Release();
    m_pRasterizerStatePtrArray[index] = pRasterizerState;
}




////////////////////////////////////////////////////////////
// CMultiRasterizerState::GetDesc()
////////////////////////////////////////////////////////////
void CMultiRasterizerState::GetDesc(  D3D_RASTERIZER_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubRasterizerState( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiRasterizerState::GetDevice()
////////////////////////////////////////////////////////////
void CMultiRasterizerState::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiRasterizerState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiRasterizerState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiRasterizerState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiRasterizerState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiRasterizerState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiRasterizerState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiResource
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// CMultiResourceGeneric
//
////////////////////////////////////////////////////////////

class CMultiResourceGeneric : public CMultiResource
{
    CResource **m_pResourcePtrArray;
    int m_nResourceCount;

public:
    CMultiResourceGeneric( int nCount );
    CMultiResourceGeneric( CResource **pResourcePtrArray, int nCount );
    ~CMultiResourceGeneric();

    ////////////////////////////////////////////////////////////
    // Sub object accessors
    ////////////////////////////////////////////////////////////
    int GetSubObjectCount() const { return m_nResourceCount; }
    void SetSubResource( int index, CResource *pResource );
    virtual CResource *Src() { return GetSubResource( 0 ); }
    virtual CResource *Ref() { return GetSubResource( 1 ); }

    //
    // CResource Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    void GetInterfacePtr( ID3D10Resource **pInterface ) { m_pResourcePtrArray[0]->GetInterfacePtr( (ID3D10Resource **) pInterface ); }
    void GetInterfacePtr( ID3D11Resource **pInterface ) { m_pResourcePtrArray[0]->GetInterfacePtr( (ID3D11Resource **) pInterface ); }
    CResource *GetSubResource( int index ) { return m_pResourcePtrArray[index]; }

    ////////////////////////////////////////////////////////////
    // GetType
    ////////////////////////////////////////////////////////////
    void GetType(  D3D_RESOURCE_DIMENSION * rType );

    ////////////////////////////////////////////////////////////
    // SetEvictionPriority
    ////////////////////////////////////////////////////////////
    void SetEvictionPriority(  UINT EvictionPriority );

    ////////////////////////////////////////////////////////////
    // GetEvictionPriority
    ////////////////////////////////////////////////////////////
    UINT GetEvictionPriority(  );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    void GetInterfacePtr( ID3D10DeviceChild **pInterface ) { m_pResourcePtrArray[0]->GetInterfacePtr( (ID3D10Resource **) pInterface ); }
    void GetInterfacePtr( ID3D11DeviceChild **pInterface ) { m_pResourcePtrArray[0]->GetInterfacePtr( (ID3D11Resource **) pInterface ); }
    CDeviceChild *GetSubDeviceChild( int index ) { return m_pResourcePtrArray[index]; }

    ////////////////////////////////////////////////////////////
    // GetDevice
    ////////////////////////////////////////////////////////////
    void GetDevice(  CDevice ** ppDevice );

    ////////////////////////////////////////////////////////////
    // GetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateDataInterface
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////
};


////////////////////////////////////////////////////////////
//
// CMultiResourceGeneric
//
////////////////////////////////////////////////////////////

CMultiResourceGeneric::CMultiResourceGeneric( int nCount ) : 
    m_nResourceCount( nCount )
{
    if( nCount > 0 )
    {
        m_pResourcePtrArray = new CResource*[nCount];
        if( !m_pResourcePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pResourcePtrArray[i] = NULL;
    }
}


CMultiResourceGeneric::CMultiResourceGeneric( CResource **pResourcePtrArray, int nCount ) : 
    m_nResourceCount( nCount )
{
    if( nCount > 0 )
    {
        m_pResourcePtrArray = new CResource*[nCount];
        if( !m_pResourcePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pResourcePtrArray[i] = pResourcePtrArray[i];
            m_pResourcePtrArray[i]->AddRef();
        }
    }
}


CMultiResourceGeneric::~CMultiResourceGeneric()
{
    if( m_pResourcePtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pResourcePtrArray[i] ) m_pResourcePtrArray[i]->CResource::Release();
        delete[]( m_pResourcePtrArray );
    }
}


void CMultiResourceGeneric::SetSubResource( int index, CResource *pResource )
{
    if( pResource ) pResource->AddRef();
    if( m_pResourcePtrArray[index] )
        m_pResourcePtrArray[index]->Release();
    m_pResourcePtrArray[index] = pResource;
}




////////////////////////////////////////////////////////////
// CMultiResourceGeneric::GetType()
////////////////////////////////////////////////////////////
void CMultiResourceGeneric::GetType(  D3D_RESOURCE_DIMENSION * rType )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubResource( i )->GetType(  rType );
    }

}


////////////////////////////////////////////////////////////
// CMultiResourceGeneric::SetEvictionPriority()
////////////////////////////////////////////////////////////
void CMultiResourceGeneric::SetEvictionPriority(  UINT EvictionPriority )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubResource( i )->SetEvictionPriority(  EvictionPriority );
    }

}


////////////////////////////////////////////////////////////
// CMultiResourceGeneric::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT CMultiResourceGeneric::GetEvictionPriority(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubResource( i )->GetEvictionPriority(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiResourceGeneric::GetDevice()
////////////////////////////////////////////////////////////
void CMultiResourceGeneric::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiResourceGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiResourceGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiResourceGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiResourceGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiResourceGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiResourceGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiBuffer
//
////////////////////////////////////////////////////////////

CMultiBuffer::CMultiBuffer( int nCount ) : 
    m_nBufferCount( nCount )
{
    if( nCount > 0 )
    {
        m_pBufferPtrArray = new CBuffer*[nCount];
        if( !m_pBufferPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pBufferPtrArray[i] = NULL;
    }
}


CMultiBuffer::CMultiBuffer( CBuffer **pBufferPtrArray, int nCount ) : 
    m_nBufferCount( nCount )
{
    if( nCount > 0 )
    {
        m_pBufferPtrArray = new CBuffer*[nCount];
        if( !m_pBufferPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pBufferPtrArray[i] = pBufferPtrArray[i];
            m_pBufferPtrArray[i]->AddRef();
        }
    }
}


CMultiBuffer::~CMultiBuffer()
{
    if( m_pBufferPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pBufferPtrArray[i] ) m_pBufferPtrArray[i]->CBuffer::Release();
        delete[]( m_pBufferPtrArray );
    }
}


void CMultiBuffer::SetSubBuffer( int index, CBuffer *pBuffer )
{
    if( pBuffer ) pBuffer->AddRef();
    if( m_pBufferPtrArray[index] )
        m_pBufferPtrArray[index]->Release();
    m_pBufferPtrArray[index] = pBuffer;
}




////////////////////////////////////////////////////////////
// CMultiBuffer::Map()
////////////////////////////////////////////////////////////
NRESULT CMultiBuffer::Map(  D3D_MAP MapType, UINT MapFlags, void ** ppData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubBuffer( i )->Map(  MapType,  MapFlags,  ppData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiBuffer::Unmap()
////////////////////////////////////////////////////////////
void CMultiBuffer::Unmap(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubBuffer( i )->Unmap(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiBuffer::GetDesc()
////////////////////////////////////////////////////////////
void CMultiBuffer::GetDesc(  D3D_BUFFER_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubBuffer( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiBuffer::GetType()
////////////////////////////////////////////////////////////
void CMultiBuffer::GetType(  D3D_RESOURCE_DIMENSION * rType )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubResource( i )->GetType(  rType );
    }

}


////////////////////////////////////////////////////////////
// CMultiBuffer::SetEvictionPriority()
////////////////////////////////////////////////////////////
void CMultiBuffer::SetEvictionPriority(  UINT EvictionPriority )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubResource( i )->SetEvictionPriority(  EvictionPriority );
    }

}


////////////////////////////////////////////////////////////
// CMultiBuffer::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT CMultiBuffer::GetEvictionPriority(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubResource( i )->GetEvictionPriority(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiBuffer::GetDevice()
////////////////////////////////////////////////////////////
void CMultiBuffer::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiBuffer::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiBuffer::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiBuffer::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiBuffer::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiBuffer::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiBuffer::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiTexture1D
//
////////////////////////////////////////////////////////////

CMultiTexture1D::CMultiTexture1D( int nCount ) : 
    m_nTexture1DCount( nCount )
{
    if( nCount > 0 )
    {
        m_pTexture1DPtrArray = new CTexture1D*[nCount];
        if( !m_pTexture1DPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pTexture1DPtrArray[i] = NULL;
    }
}


CMultiTexture1D::CMultiTexture1D( CTexture1D **pTexture1DPtrArray, int nCount ) : 
    m_nTexture1DCount( nCount )
{
    if( nCount > 0 )
    {
        m_pTexture1DPtrArray = new CTexture1D*[nCount];
        if( !m_pTexture1DPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pTexture1DPtrArray[i] = pTexture1DPtrArray[i];
            m_pTexture1DPtrArray[i]->AddRef();
        }
    }
}


CMultiTexture1D::~CMultiTexture1D()
{
    if( m_pTexture1DPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pTexture1DPtrArray[i] ) m_pTexture1DPtrArray[i]->CTexture1D::Release();
        delete[]( m_pTexture1DPtrArray );
    }
}


void CMultiTexture1D::SetSubTexture1D( int index, CTexture1D *pTexture1D )
{
    if( pTexture1D ) pTexture1D->AddRef();
    if( m_pTexture1DPtrArray[index] )
        m_pTexture1DPtrArray[index]->Release();
    m_pTexture1DPtrArray[index] = pTexture1D;
}




////////////////////////////////////////////////////////////
// CMultiTexture1D::Map()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture1D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, void ** ppData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubTexture1D( i )->Map(  Subresource,  MapType,  MapFlags,  ppData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture1D::Unmap()
////////////////////////////////////////////////////////////
void CMultiTexture1D::Unmap(  UINT Subresource )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubTexture1D( i )->Unmap(  Subresource );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture1D::GetDesc()
////////////////////////////////////////////////////////////
void CMultiTexture1D::GetDesc(  D3D_TEXTURE1D_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubTexture1D( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture1D::GetType()
////////////////////////////////////////////////////////////
void CMultiTexture1D::GetType(  D3D_RESOURCE_DIMENSION * rType )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubResource( i )->GetType(  rType );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture1D::SetEvictionPriority()
////////////////////////////////////////////////////////////
void CMultiTexture1D::SetEvictionPriority(  UINT EvictionPriority )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubResource( i )->SetEvictionPriority(  EvictionPriority );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture1D::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT CMultiTexture1D::GetEvictionPriority(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubResource( i )->GetEvictionPriority(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture1D::GetDevice()
////////////////////////////////////////////////////////////
void CMultiTexture1D::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiTexture1D::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture1D::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture1D::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture1D::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture1D::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture1D::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiTexture2D
//
////////////////////////////////////////////////////////////

CMultiTexture2D::CMultiTexture2D( int nCount ) : 
    m_nTexture2DCount( nCount )
{
    if( nCount > 0 )
    {
        m_pTexture2DPtrArray = new CTexture2D*[nCount];
        if( !m_pTexture2DPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pTexture2DPtrArray[i] = NULL;
    }
}


CMultiTexture2D::CMultiTexture2D( CTexture2D **pTexture2DPtrArray, int nCount ) : 
    m_nTexture2DCount( nCount )
{
    if( nCount > 0 )
    {
        m_pTexture2DPtrArray = new CTexture2D*[nCount];
        if( !m_pTexture2DPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pTexture2DPtrArray[i] = pTexture2DPtrArray[i];
            m_pTexture2DPtrArray[i]->AddRef();
        }
    }
}


CMultiTexture2D::~CMultiTexture2D()
{
    if( m_pTexture2DPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pTexture2DPtrArray[i] ) m_pTexture2DPtrArray[i]->CTexture2D::Release();
        delete[]( m_pTexture2DPtrArray );
    }
}


void CMultiTexture2D::SetSubTexture2D( int index, CTexture2D *pTexture2D )
{
    if( pTexture2D ) pTexture2D->AddRef();
    if( m_pTexture2DPtrArray[index] )
        m_pTexture2DPtrArray[index]->Release();
    m_pTexture2DPtrArray[index] = pTexture2D;
}




////////////////////////////////////////////////////////////
// CMultiTexture2D::Map()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture2D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, D3D_MAPPED_TEXTURE2D * pMappedTex2D )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubTexture2D( i )->Map(  Subresource,  MapType,  MapFlags,  pMappedTex2D );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture2D::Unmap()
////////////////////////////////////////////////////////////
void CMultiTexture2D::Unmap(  UINT Subresource )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubTexture2D( i )->Unmap(  Subresource );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture2D::GetDesc()
////////////////////////////////////////////////////////////
void CMultiTexture2D::GetDesc(  D3D_TEXTURE2D_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubTexture2D( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture2D::GetType()
////////////////////////////////////////////////////////////
void CMultiTexture2D::GetType(  D3D_RESOURCE_DIMENSION * rType )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubResource( i )->GetType(  rType );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture2D::SetEvictionPriority()
////////////////////////////////////////////////////////////
void CMultiTexture2D::SetEvictionPriority(  UINT EvictionPriority )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubResource( i )->SetEvictionPriority(  EvictionPriority );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture2D::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT CMultiTexture2D::GetEvictionPriority(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubResource( i )->GetEvictionPriority(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture2D::GetDevice()
////////////////////////////////////////////////////////////
void CMultiTexture2D::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiTexture2D::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture2D::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture2D::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture2D::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture2D::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture2D::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiTexture3D
//
////////////////////////////////////////////////////////////

CMultiTexture3D::CMultiTexture3D( int nCount ) : 
    m_nTexture3DCount( nCount )
{
    if( nCount > 0 )
    {
        m_pTexture3DPtrArray = new CTexture3D*[nCount];
        if( !m_pTexture3DPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pTexture3DPtrArray[i] = NULL;
    }
}


CMultiTexture3D::CMultiTexture3D( CTexture3D **pTexture3DPtrArray, int nCount ) : 
    m_nTexture3DCount( nCount )
{
    if( nCount > 0 )
    {
        m_pTexture3DPtrArray = new CTexture3D*[nCount];
        if( !m_pTexture3DPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pTexture3DPtrArray[i] = pTexture3DPtrArray[i];
            m_pTexture3DPtrArray[i]->AddRef();
        }
    }
}


CMultiTexture3D::~CMultiTexture3D()
{
    if( m_pTexture3DPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pTexture3DPtrArray[i] ) m_pTexture3DPtrArray[i]->CTexture3D::Release();
        delete[]( m_pTexture3DPtrArray );
    }
}


void CMultiTexture3D::SetSubTexture3D( int index, CTexture3D *pTexture3D )
{
    if( pTexture3D ) pTexture3D->AddRef();
    if( m_pTexture3DPtrArray[index] )
        m_pTexture3DPtrArray[index]->Release();
    m_pTexture3DPtrArray[index] = pTexture3D;
}




////////////////////////////////////////////////////////////
// CMultiTexture3D::Map()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture3D::Map(  UINT Subresource, D3D_MAP MapType, UINT MapFlags, D3D_MAPPED_TEXTURE3D * pMappedTex3D )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubTexture3D( i )->Map(  Subresource,  MapType,  MapFlags,  pMappedTex3D );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture3D::Unmap()
////////////////////////////////////////////////////////////
void CMultiTexture3D::Unmap(  UINT Subresource )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubTexture3D( i )->Unmap(  Subresource );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture3D::GetDesc()
////////////////////////////////////////////////////////////
void CMultiTexture3D::GetDesc(  D3D_TEXTURE3D_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubTexture3D( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture3D::GetType()
////////////////////////////////////////////////////////////
void CMultiTexture3D::GetType(  D3D_RESOURCE_DIMENSION * rType )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubResource( i )->GetType(  rType );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture3D::SetEvictionPriority()
////////////////////////////////////////////////////////////
void CMultiTexture3D::SetEvictionPriority(  UINT EvictionPriority )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubResource( i )->SetEvictionPriority(  EvictionPriority );
    }

}


////////////////////////////////////////////////////////////
// CMultiTexture3D::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT CMultiTexture3D::GetEvictionPriority(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubResource( i )->GetEvictionPriority(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture3D::GetDevice()
////////////////////////////////////////////////////////////
void CMultiTexture3D::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiTexture3D::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture3D::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture3D::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture3D::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiTexture3D::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiTexture3D::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiView
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// CMultiViewGeneric
//
////////////////////////////////////////////////////////////

class CMultiViewGeneric : public CMultiView
{
    CView **m_pViewPtrArray;
    int m_nViewCount;

public:
    CMultiViewGeneric( int nCount );
    CMultiViewGeneric( CView **pViewPtrArray, int nCount );
    ~CMultiViewGeneric();

    ////////////////////////////////////////////////////////////
    // Sub object accessors
    ////////////////////////////////////////////////////////////
    int GetSubObjectCount() const { return m_nViewCount; }
    void SetSubView( int index, CView *pView );
    virtual CView *Src() { return GetSubView( 0 ); }
    virtual CView *Ref() { return GetSubView( 1 ); }

    //
    // CView Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    void GetInterfacePtr( ID3D10View **pInterface ) { m_pViewPtrArray[0]->GetInterfacePtr( (ID3D10View **) pInterface ); }
    void GetInterfacePtr( ID3D11View **pInterface ) { m_pViewPtrArray[0]->GetInterfacePtr( (ID3D11View **) pInterface ); }
    CView *GetSubView( int index ) { return m_pViewPtrArray[index]; }

    ////////////////////////////////////////////////////////////
    // GetResource
    ////////////////////////////////////////////////////////////
    void GetResource(  CResource ** ppResource );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    void GetInterfacePtr( ID3D10DeviceChild **pInterface ) { m_pViewPtrArray[0]->GetInterfacePtr( (ID3D10View **) pInterface ); }
    void GetInterfacePtr( ID3D11DeviceChild **pInterface ) { m_pViewPtrArray[0]->GetInterfacePtr( (ID3D11View **) pInterface ); }
    CDeviceChild *GetSubDeviceChild( int index ) { return m_pViewPtrArray[index]; }

    ////////////////////////////////////////////////////////////
    // GetDevice
    ////////////////////////////////////////////////////////////
    void GetDevice(  CDevice ** ppDevice );

    ////////////////////////////////////////////////////////////
    // GetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateDataInterface
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////
};


////////////////////////////////////////////////////////////
//
// CMultiViewGeneric
//
////////////////////////////////////////////////////////////

CMultiViewGeneric::CMultiViewGeneric( int nCount ) : 
    m_nViewCount( nCount )
{
    if( nCount > 0 )
    {
        m_pViewPtrArray = new CView*[nCount];
        if( !m_pViewPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pViewPtrArray[i] = NULL;
    }
}


CMultiViewGeneric::CMultiViewGeneric( CView **pViewPtrArray, int nCount ) : 
    m_nViewCount( nCount )
{
    if( nCount > 0 )
    {
        m_pViewPtrArray = new CView*[nCount];
        if( !m_pViewPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pViewPtrArray[i] = pViewPtrArray[i];
            m_pViewPtrArray[i]->AddRef();
        }
    }
}


CMultiViewGeneric::~CMultiViewGeneric()
{
    if( m_pViewPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pViewPtrArray[i] ) m_pViewPtrArray[i]->CView::Release();
        delete[]( m_pViewPtrArray );
    }
}


void CMultiViewGeneric::SetSubView( int index, CView *pView )
{
    if( pView ) pView->AddRef();
    if( m_pViewPtrArray[index] )
        m_pViewPtrArray[index]->Release();
    m_pViewPtrArray[index] = pView;
}




////////////////////////////////////////////////////////////
// CMultiViewGeneric::GetResource()
////////////////////////////////////////////////////////////
void CMultiViewGeneric::GetResource(  CResource ** ppResource )
{
    CMultiResourceGeneric *pMultiResource = new CMultiResourceGeneric( GetSubObjectCount() );
    if( !pMultiResource ) throw( WRAPPER_OUT_OF_MEMORY );
    CResource *pTempResource;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubView( i )->GetResource(  &pTempResource );
        if( pTempResource )
        {
            pMultiResource->SetSubResource( i, pTempResource );
            pTempResource->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiResource );
            pMultiResource = NULL;
            break;
        }
    }

    *ppResource = pMultiResource;
    if( *ppResource ) (*ppResource)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiViewGeneric::GetDevice()
////////////////////////////////////////////////////////////
void CMultiViewGeneric::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiViewGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiViewGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiViewGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiViewGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiViewGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiViewGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiShaderResourceView
//
////////////////////////////////////////////////////////////

CMultiShaderResourceView::CMultiShaderResourceView( int nCount ) : 
    m_nShaderResourceViewCount( nCount )
{
    if( nCount > 0 )
    {
        m_pShaderResourceViewPtrArray = new CShaderResourceView*[nCount];
        if( !m_pShaderResourceViewPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pShaderResourceViewPtrArray[i] = NULL;
    }
}


CMultiShaderResourceView::CMultiShaderResourceView( CShaderResourceView **pShaderResourceViewPtrArray, int nCount ) : 
    m_nShaderResourceViewCount( nCount )
{
    if( nCount > 0 )
    {
        m_pShaderResourceViewPtrArray = new CShaderResourceView*[nCount];
        if( !m_pShaderResourceViewPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pShaderResourceViewPtrArray[i] = pShaderResourceViewPtrArray[i];
            m_pShaderResourceViewPtrArray[i]->AddRef();
        }
    }
}


CMultiShaderResourceView::~CMultiShaderResourceView()
{
    if( m_pShaderResourceViewPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pShaderResourceViewPtrArray[i] ) m_pShaderResourceViewPtrArray[i]->CShaderResourceView::Release();
        delete[]( m_pShaderResourceViewPtrArray );
    }
}


void CMultiShaderResourceView::SetSubShaderResourceView( int index, CShaderResourceView *pShaderResourceView )
{
    if( pShaderResourceView ) pShaderResourceView->AddRef();
    if( m_pShaderResourceViewPtrArray[index] )
        m_pShaderResourceViewPtrArray[index]->Release();
    m_pShaderResourceViewPtrArray[index] = pShaderResourceView;
}




////////////////////////////////////////////////////////////
// CMultiShaderResourceView::GetDesc()
////////////////////////////////////////////////////////////
void CMultiShaderResourceView::GetDesc(  D3D_SHADER_RESOURCE_VIEW_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubShaderResourceView( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiShaderResourceView::GetResource()
////////////////////////////////////////////////////////////
void CMultiShaderResourceView::GetResource(  CResource ** ppResource )
{
    CMultiResourceGeneric *pMultiResource = new CMultiResourceGeneric( GetSubObjectCount() );
    if( !pMultiResource ) throw( WRAPPER_OUT_OF_MEMORY );
    CResource *pTempResource;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubView( i )->GetResource(  &pTempResource );
        if( pTempResource )
        {
            pMultiResource->SetSubResource( i, pTempResource );
            pTempResource->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiResource );
            pMultiResource = NULL;
            break;
        }
    }

    *ppResource = pMultiResource;
    if( *ppResource ) (*ppResource)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiShaderResourceView::GetDevice()
////////////////////////////////////////////////////////////
void CMultiShaderResourceView::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiShaderResourceView::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiShaderResourceView::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiShaderResourceView::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiShaderResourceView::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiShaderResourceView::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiShaderResourceView::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiRenderTargetView
//
////////////////////////////////////////////////////////////

CMultiRenderTargetView::CMultiRenderTargetView( int nCount ) : 
    m_nRenderTargetViewCount( nCount )
{
    if( nCount > 0 )
    {
        m_pRenderTargetViewPtrArray = new CRenderTargetView*[nCount];
        if( !m_pRenderTargetViewPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pRenderTargetViewPtrArray[i] = NULL;
    }
}


CMultiRenderTargetView::CMultiRenderTargetView( CRenderTargetView **pRenderTargetViewPtrArray, int nCount ) : 
    m_nRenderTargetViewCount( nCount )
{
    if( nCount > 0 )
    {
        m_pRenderTargetViewPtrArray = new CRenderTargetView*[nCount];
        if( !m_pRenderTargetViewPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pRenderTargetViewPtrArray[i] = pRenderTargetViewPtrArray[i];
            m_pRenderTargetViewPtrArray[i]->AddRef();
        }
    }
}


CMultiRenderTargetView::~CMultiRenderTargetView()
{
    if( m_pRenderTargetViewPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pRenderTargetViewPtrArray[i] ) m_pRenderTargetViewPtrArray[i]->CRenderTargetView::Release();
        delete[]( m_pRenderTargetViewPtrArray );
    }
}


void CMultiRenderTargetView::SetSubRenderTargetView( int index, CRenderTargetView *pRenderTargetView )
{
    if( pRenderTargetView ) pRenderTargetView->AddRef();
    if( m_pRenderTargetViewPtrArray[index] )
        m_pRenderTargetViewPtrArray[index]->Release();
    m_pRenderTargetViewPtrArray[index] = pRenderTargetView;
}




////////////////////////////////////////////////////////////
// CMultiRenderTargetView::GetDesc()
////////////////////////////////////////////////////////////
void CMultiRenderTargetView::GetDesc(  D3D_RENDER_TARGET_VIEW_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubRenderTargetView( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiRenderTargetView::GetResource()
////////////////////////////////////////////////////////////
void CMultiRenderTargetView::GetResource(  CResource ** ppResource )
{
    CMultiResourceGeneric *pMultiResource = new CMultiResourceGeneric( GetSubObjectCount() );
    if( !pMultiResource ) throw( WRAPPER_OUT_OF_MEMORY );
    CResource *pTempResource;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubView( i )->GetResource(  &pTempResource );
        if( pTempResource )
        {
            pMultiResource->SetSubResource( i, pTempResource );
            pTempResource->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiResource );
            pMultiResource = NULL;
            break;
        }
    }

    *ppResource = pMultiResource;
    if( *ppResource ) (*ppResource)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiRenderTargetView::GetDevice()
////////////////////////////////////////////////////////////
void CMultiRenderTargetView::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiRenderTargetView::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiRenderTargetView::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiRenderTargetView::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiRenderTargetView::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiRenderTargetView::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiRenderTargetView::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiDepthStencilView
//
////////////////////////////////////////////////////////////

CMultiDepthStencilView::CMultiDepthStencilView( int nCount ) : 
    m_nDepthStencilViewCount( nCount )
{
    if( nCount > 0 )
    {
        m_pDepthStencilViewPtrArray = new CDepthStencilView*[nCount];
        if( !m_pDepthStencilViewPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pDepthStencilViewPtrArray[i] = NULL;
    }
}


CMultiDepthStencilView::CMultiDepthStencilView( CDepthStencilView **pDepthStencilViewPtrArray, int nCount ) : 
    m_nDepthStencilViewCount( nCount )
{
    if( nCount > 0 )
    {
        m_pDepthStencilViewPtrArray = new CDepthStencilView*[nCount];
        if( !m_pDepthStencilViewPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pDepthStencilViewPtrArray[i] = pDepthStencilViewPtrArray[i];
            m_pDepthStencilViewPtrArray[i]->AddRef();
        }
    }
}


CMultiDepthStencilView::~CMultiDepthStencilView()
{
    if( m_pDepthStencilViewPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pDepthStencilViewPtrArray[i] ) m_pDepthStencilViewPtrArray[i]->CDepthStencilView::Release();
        delete[]( m_pDepthStencilViewPtrArray );
    }
}


void CMultiDepthStencilView::SetSubDepthStencilView( int index, CDepthStencilView *pDepthStencilView )
{
    if( pDepthStencilView ) pDepthStencilView->AddRef();
    if( m_pDepthStencilViewPtrArray[index] )
        m_pDepthStencilViewPtrArray[index]->Release();
    m_pDepthStencilViewPtrArray[index] = pDepthStencilView;
}




////////////////////////////////////////////////////////////
// CMultiDepthStencilView::GetDesc()
////////////////////////////////////////////////////////////
void CMultiDepthStencilView::GetDesc(  D3D_DEPTH_STENCIL_VIEW_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDepthStencilView( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiDepthStencilView::GetResource()
////////////////////////////////////////////////////////////
void CMultiDepthStencilView::GetResource(  CResource ** ppResource )
{
    CMultiResourceGeneric *pMultiResource = new CMultiResourceGeneric( GetSubObjectCount() );
    if( !pMultiResource ) throw( WRAPPER_OUT_OF_MEMORY );
    CResource *pTempResource;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubView( i )->GetResource(  &pTempResource );
        if( pTempResource )
        {
            pMultiResource->SetSubResource( i, pTempResource );
            pTempResource->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiResource );
            pMultiResource = NULL;
            break;
        }
    }

    *ppResource = pMultiResource;
    if( *ppResource ) (*ppResource)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDepthStencilView::GetDevice()
////////////////////////////////////////////////////////////
void CMultiDepthStencilView::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDepthStencilView::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiDepthStencilView::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDepthStencilView::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiDepthStencilView::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDepthStencilView::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiDepthStencilView::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiVertexShader
//
////////////////////////////////////////////////////////////

CMultiVertexShader::CMultiVertexShader( int nCount ) : 
    m_nVertexShaderCount( nCount )
{
    if( nCount > 0 )
    {
        m_pVertexShaderPtrArray = new CVertexShader*[nCount];
        if( !m_pVertexShaderPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pVertexShaderPtrArray[i] = NULL;
    }
}


CMultiVertexShader::CMultiVertexShader( CVertexShader **pVertexShaderPtrArray, int nCount ) : 
    m_nVertexShaderCount( nCount )
{
    if( nCount > 0 )
    {
        m_pVertexShaderPtrArray = new CVertexShader*[nCount];
        if( !m_pVertexShaderPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pVertexShaderPtrArray[i] = pVertexShaderPtrArray[i];
            m_pVertexShaderPtrArray[i]->AddRef();
        }
    }
}


CMultiVertexShader::~CMultiVertexShader()
{
    if( m_pVertexShaderPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pVertexShaderPtrArray[i] ) m_pVertexShaderPtrArray[i]->CVertexShader::Release();
        delete[]( m_pVertexShaderPtrArray );
    }
}


void CMultiVertexShader::SetSubVertexShader( int index, CVertexShader *pVertexShader )
{
    if( pVertexShader ) pVertexShader->AddRef();
    if( m_pVertexShaderPtrArray[index] )
        m_pVertexShaderPtrArray[index]->Release();
    m_pVertexShaderPtrArray[index] = pVertexShader;
}




////////////////////////////////////////////////////////////
// CMultiVertexShader::GetDevice()
////////////////////////////////////////////////////////////
void CMultiVertexShader::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiVertexShader::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiVertexShader::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiVertexShader::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiVertexShader::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiVertexShader::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiVertexShader::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiGeometryShader
//
////////////////////////////////////////////////////////////

CMultiGeometryShader::CMultiGeometryShader( int nCount ) : 
    m_nGeometryShaderCount( nCount )
{
    if( nCount > 0 )
    {
        m_pGeometryShaderPtrArray = new CGeometryShader*[nCount];
        if( !m_pGeometryShaderPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pGeometryShaderPtrArray[i] = NULL;
    }
}


CMultiGeometryShader::CMultiGeometryShader( CGeometryShader **pGeometryShaderPtrArray, int nCount ) : 
    m_nGeometryShaderCount( nCount )
{
    if( nCount > 0 )
    {
        m_pGeometryShaderPtrArray = new CGeometryShader*[nCount];
        if( !m_pGeometryShaderPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pGeometryShaderPtrArray[i] = pGeometryShaderPtrArray[i];
            m_pGeometryShaderPtrArray[i]->AddRef();
        }
    }
}


CMultiGeometryShader::~CMultiGeometryShader()
{
    if( m_pGeometryShaderPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pGeometryShaderPtrArray[i] ) m_pGeometryShaderPtrArray[i]->CGeometryShader::Release();
        delete[]( m_pGeometryShaderPtrArray );
    }
}


void CMultiGeometryShader::SetSubGeometryShader( int index, CGeometryShader *pGeometryShader )
{
    if( pGeometryShader ) pGeometryShader->AddRef();
    if( m_pGeometryShaderPtrArray[index] )
        m_pGeometryShaderPtrArray[index]->Release();
    m_pGeometryShaderPtrArray[index] = pGeometryShader;
}




////////////////////////////////////////////////////////////
// CMultiGeometryShader::GetDevice()
////////////////////////////////////////////////////////////
void CMultiGeometryShader::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiGeometryShader::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiGeometryShader::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiGeometryShader::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiGeometryShader::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiGeometryShader::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiGeometryShader::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiPixelShader
//
////////////////////////////////////////////////////////////

CMultiPixelShader::CMultiPixelShader( int nCount ) : 
    m_nPixelShaderCount( nCount )
{
    if( nCount > 0 )
    {
        m_pPixelShaderPtrArray = new CPixelShader*[nCount];
        if( !m_pPixelShaderPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pPixelShaderPtrArray[i] = NULL;
    }
}


CMultiPixelShader::CMultiPixelShader( CPixelShader **pPixelShaderPtrArray, int nCount ) : 
    m_nPixelShaderCount( nCount )
{
    if( nCount > 0 )
    {
        m_pPixelShaderPtrArray = new CPixelShader*[nCount];
        if( !m_pPixelShaderPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pPixelShaderPtrArray[i] = pPixelShaderPtrArray[i];
            m_pPixelShaderPtrArray[i]->AddRef();
        }
    }
}


CMultiPixelShader::~CMultiPixelShader()
{
    if( m_pPixelShaderPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pPixelShaderPtrArray[i] ) m_pPixelShaderPtrArray[i]->CPixelShader::Release();
        delete[]( m_pPixelShaderPtrArray );
    }
}


void CMultiPixelShader::SetSubPixelShader( int index, CPixelShader *pPixelShader )
{
    if( pPixelShader ) pPixelShader->AddRef();
    if( m_pPixelShaderPtrArray[index] )
        m_pPixelShaderPtrArray[index]->Release();
    m_pPixelShaderPtrArray[index] = pPixelShader;
}




////////////////////////////////////////////////////////////
// CMultiPixelShader::GetDevice()
////////////////////////////////////////////////////////////
void CMultiPixelShader::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiPixelShader::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiPixelShader::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiPixelShader::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiPixelShader::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiPixelShader::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiPixelShader::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiInputLayout
//
////////////////////////////////////////////////////////////

CMultiInputLayout::CMultiInputLayout( int nCount ) : 
    m_nInputLayoutCount( nCount )
{
    if( nCount > 0 )
    {
        m_pInputLayoutPtrArray = new CInputLayout*[nCount];
        if( !m_pInputLayoutPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pInputLayoutPtrArray[i] = NULL;
    }
}


CMultiInputLayout::CMultiInputLayout( CInputLayout **pInputLayoutPtrArray, int nCount ) : 
    m_nInputLayoutCount( nCount )
{
    if( nCount > 0 )
    {
        m_pInputLayoutPtrArray = new CInputLayout*[nCount];
        if( !m_pInputLayoutPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pInputLayoutPtrArray[i] = pInputLayoutPtrArray[i];
            m_pInputLayoutPtrArray[i]->AddRef();
        }
    }
}


CMultiInputLayout::~CMultiInputLayout()
{
    if( m_pInputLayoutPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pInputLayoutPtrArray[i] ) m_pInputLayoutPtrArray[i]->CInputLayout::Release();
        delete[]( m_pInputLayoutPtrArray );
    }
}


void CMultiInputLayout::SetSubInputLayout( int index, CInputLayout *pInputLayout )
{
    if( pInputLayout ) pInputLayout->AddRef();
    if( m_pInputLayoutPtrArray[index] )
        m_pInputLayoutPtrArray[index]->Release();
    m_pInputLayoutPtrArray[index] = pInputLayout;
}




////////////////////////////////////////////////////////////
// CMultiInputLayout::GetDevice()
////////////////////////////////////////////////////////////
void CMultiInputLayout::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiInputLayout::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiInputLayout::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInputLayout::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiInputLayout::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInputLayout::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiInputLayout::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiSamplerState
//
////////////////////////////////////////////////////////////

CMultiSamplerState::CMultiSamplerState( int nCount ) : 
    m_nSamplerStateCount( nCount )
{
    if( nCount > 0 )
    {
        m_pSamplerStatePtrArray = new CSamplerState*[nCount];
        if( !m_pSamplerStatePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pSamplerStatePtrArray[i] = NULL;
    }
}


CMultiSamplerState::CMultiSamplerState( CSamplerState **pSamplerStatePtrArray, int nCount ) : 
    m_nSamplerStateCount( nCount )
{
    if( nCount > 0 )
    {
        m_pSamplerStatePtrArray = new CSamplerState*[nCount];
        if( !m_pSamplerStatePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pSamplerStatePtrArray[i] = pSamplerStatePtrArray[i];
            m_pSamplerStatePtrArray[i]->AddRef();
        }
    }
}


CMultiSamplerState::~CMultiSamplerState()
{
    if( m_pSamplerStatePtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pSamplerStatePtrArray[i] ) m_pSamplerStatePtrArray[i]->CSamplerState::Release();
        delete[]( m_pSamplerStatePtrArray );
    }
}


void CMultiSamplerState::SetSubSamplerState( int index, CSamplerState *pSamplerState )
{
    if( pSamplerState ) pSamplerState->AddRef();
    if( m_pSamplerStatePtrArray[index] )
        m_pSamplerStatePtrArray[index]->Release();
    m_pSamplerStatePtrArray[index] = pSamplerState;
}




////////////////////////////////////////////////////////////
// CMultiSamplerState::GetDesc()
////////////////////////////////////////////////////////////
void CMultiSamplerState::GetDesc(  D3D_SAMPLER_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubSamplerState( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiSamplerState::GetDevice()
////////////////////////////////////////////////////////////
void CMultiSamplerState::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiSamplerState::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiSamplerState::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiSamplerState::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiSamplerState::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiSamplerState::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiSamplerState::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiAsynchronous
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// CMultiAsynchronousGeneric
//
////////////////////////////////////////////////////////////

class CMultiAsynchronousGeneric : public CMultiAsynchronous
{
    CAsynchronous **m_pAsynchronousPtrArray;
    int m_nAsynchronousCount;

public:
    CMultiAsynchronousGeneric( int nCount );
    CMultiAsynchronousGeneric( CAsynchronous **pAsynchronousPtrArray, int nCount );
    ~CMultiAsynchronousGeneric();

    ////////////////////////////////////////////////////////////
    // Sub object accessors
    ////////////////////////////////////////////////////////////
    int GetSubObjectCount() const { return m_nAsynchronousCount; }
    void SetSubAsynchronous( int index, CAsynchronous *pAsynchronous );
    virtual CAsynchronous *Src() { return GetSubAsynchronous( 0 ); }
    virtual CAsynchronous *Ref() { return GetSubAsynchronous( 1 ); }

    //
    // CAsynchronous Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    void GetInterfacePtr( ID3D10Asynchronous **pInterface ) { m_pAsynchronousPtrArray[0]->GetInterfacePtr( (ID3D10Asynchronous **) pInterface ); }
    void GetInterfacePtr( ID3D11Asynchronous **pInterface ) { m_pAsynchronousPtrArray[0]->GetInterfacePtr( (ID3D11Asynchronous **) pInterface ); }
    CAsynchronous *GetSubAsynchronous( int index ) { return m_pAsynchronousPtrArray[index]; }

    ////////////////////////////////////////////////////////////
    // Begin
    ////////////////////////////////////////////////////////////
    void Begin(  );

    ////////////////////////////////////////////////////////////
    // End
    ////////////////////////////////////////////////////////////
    void End(  );

    ////////////////////////////////////////////////////////////
    // GetData
    ////////////////////////////////////////////////////////////
    NRESULT GetData(  void * pData, UINT DataSize, UINT GetDataFlags );

    ////////////////////////////////////////////////////////////
    // GetDataSize
    ////////////////////////////////////////////////////////////
    UINT GetDataSize(  );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    void GetInterfacePtr( ID3D10DeviceChild **pInterface ) { m_pAsynchronousPtrArray[0]->GetInterfacePtr( (ID3D10Asynchronous **) pInterface ); }
    void GetInterfacePtr( ID3D11DeviceChild **pInterface ) { m_pAsynchronousPtrArray[0]->GetInterfacePtr( (ID3D11Asynchronous **) pInterface ); }
    CDeviceChild *GetSubDeviceChild( int index ) { return m_pAsynchronousPtrArray[index]; }

    ////////////////////////////////////////////////////////////
    // GetDevice
    ////////////////////////////////////////////////////////////
    void GetDevice(  CDevice ** ppDevice );

    ////////////////////////////////////////////////////////////
    // GetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateDataInterface
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////
};


////////////////////////////////////////////////////////////
//
// CMultiAsynchronousGeneric
//
////////////////////////////////////////////////////////////

CMultiAsynchronousGeneric::CMultiAsynchronousGeneric( int nCount ) : 
    m_nAsynchronousCount( nCount )
{
    if( nCount > 0 )
    {
        m_pAsynchronousPtrArray = new CAsynchronous*[nCount];
        if( !m_pAsynchronousPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pAsynchronousPtrArray[i] = NULL;
    }
}


CMultiAsynchronousGeneric::CMultiAsynchronousGeneric( CAsynchronous **pAsynchronousPtrArray, int nCount ) : 
    m_nAsynchronousCount( nCount )
{
    if( nCount > 0 )
    {
        m_pAsynchronousPtrArray = new CAsynchronous*[nCount];
        if( !m_pAsynchronousPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pAsynchronousPtrArray[i] = pAsynchronousPtrArray[i];
            m_pAsynchronousPtrArray[i]->AddRef();
        }
    }
}


CMultiAsynchronousGeneric::~CMultiAsynchronousGeneric()
{
    if( m_pAsynchronousPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pAsynchronousPtrArray[i] ) m_pAsynchronousPtrArray[i]->CAsynchronous::Release();
        delete[]( m_pAsynchronousPtrArray );
    }
}


void CMultiAsynchronousGeneric::SetSubAsynchronous( int index, CAsynchronous *pAsynchronous )
{
    if( pAsynchronous ) pAsynchronous->AddRef();
    if( m_pAsynchronousPtrArray[index] )
        m_pAsynchronousPtrArray[index]->Release();
    m_pAsynchronousPtrArray[index] = pAsynchronous;
}




////////////////////////////////////////////////////////////
// CMultiAsynchronousGeneric::Begin()
////////////////////////////////////////////////////////////
void CMultiAsynchronousGeneric::Begin(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubAsynchronous( i )->Begin(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiAsynchronousGeneric::End()
////////////////////////////////////////////////////////////
void CMultiAsynchronousGeneric::End(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubAsynchronous( i )->End(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiAsynchronousGeneric::GetData()
////////////////////////////////////////////////////////////
NRESULT CMultiAsynchronousGeneric::GetData(  void * pData, UINT DataSize, UINT GetDataFlags )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubAsynchronous( i )->GetData(  pData,  DataSize,  GetDataFlags );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiAsynchronousGeneric::GetDataSize()
////////////////////////////////////////////////////////////
UINT CMultiAsynchronousGeneric::GetDataSize(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubAsynchronous( i )->GetDataSize(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiAsynchronousGeneric::GetDevice()
////////////////////////////////////////////////////////////
void CMultiAsynchronousGeneric::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiAsynchronousGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiAsynchronousGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiAsynchronousGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiAsynchronousGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiAsynchronousGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiAsynchronousGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiQuery
//
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//
// CMultiQueryGeneric
//
////////////////////////////////////////////////////////////

class CMultiQueryGeneric : public CMultiQuery
{
    CQuery **m_pQueryPtrArray;
    int m_nQueryCount;

public:
    CMultiQueryGeneric( int nCount );
    CMultiQueryGeneric( CQuery **pQueryPtrArray, int nCount );
    ~CMultiQueryGeneric();

    ////////////////////////////////////////////////////////////
    // Sub object accessors
    ////////////////////////////////////////////////////////////
    int GetSubObjectCount() const { return m_nQueryCount; }
    void SetSubQuery( int index, CQuery *pQuery );
    virtual CQuery *Src() { return GetSubQuery( 0 ); }
    virtual CQuery *Ref() { return GetSubQuery( 1 ); }

    //
    // CQuery Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    void GetInterfacePtr( ID3D10Query **pInterface ) { m_pQueryPtrArray[0]->GetInterfacePtr( (ID3D10Query **) pInterface ); }
    void GetInterfacePtr( ID3D11Query **pInterface ) { m_pQueryPtrArray[0]->GetInterfacePtr( (ID3D11Query **) pInterface ); }
    CQuery *GetSubQuery( int index ) { return m_pQueryPtrArray[index]; }

    ////////////////////////////////////////////////////////////
    // GetDesc
    ////////////////////////////////////////////////////////////
    void GetDesc(  D3D_QUERY_DESC * pDesc );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CAsynchronous Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    void GetInterfacePtr( ID3D10Asynchronous **pInterface ) { m_pQueryPtrArray[0]->GetInterfacePtr( (ID3D10Query **) pInterface ); }
    void GetInterfacePtr( ID3D11Asynchronous **pInterface ) { m_pQueryPtrArray[0]->GetInterfacePtr( (ID3D11Query **) pInterface ); }
    CAsynchronous *GetSubAsynchronous( int index ) { return m_pQueryPtrArray[index]; }

    ////////////////////////////////////////////////////////////
    // Begin
    ////////////////////////////////////////////////////////////
    void Begin(  );

    ////////////////////////////////////////////////////////////
    // End
    ////////////////////////////////////////////////////////////
    void End(  );

    ////////////////////////////////////////////////////////////
    // GetData
    ////////////////////////////////////////////////////////////
    NRESULT GetData(  void * pData, UINT DataSize, UINT GetDataFlags );

    ////////////////////////////////////////////////////////////
    // GetDataSize
    ////////////////////////////////////////////////////////////
    UINT GetDataSize(  );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    void GetInterfacePtr( ID3D10DeviceChild **pInterface ) { m_pQueryPtrArray[0]->GetInterfacePtr( (ID3D10Query **) pInterface ); }
    void GetInterfacePtr( ID3D11DeviceChild **pInterface ) { m_pQueryPtrArray[0]->GetInterfacePtr( (ID3D11Query **) pInterface ); }
    CDeviceChild *GetSubDeviceChild( int index ) { return m_pQueryPtrArray[index]; }

    ////////////////////////////////////////////////////////////
    // GetDevice
    ////////////////////////////////////////////////////////////
    void GetDevice(  CDevice ** ppDevice );

    ////////////////////////////////////////////////////////////
    // GetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateDataInterface
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////
};


////////////////////////////////////////////////////////////
//
// CMultiQueryGeneric
//
////////////////////////////////////////////////////////////

CMultiQueryGeneric::CMultiQueryGeneric( int nCount ) : 
    m_nQueryCount( nCount )
{
    if( nCount > 0 )
    {
        m_pQueryPtrArray = new CQuery*[nCount];
        if( !m_pQueryPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pQueryPtrArray[i] = NULL;
    }
}


CMultiQueryGeneric::CMultiQueryGeneric( CQuery **pQueryPtrArray, int nCount ) : 
    m_nQueryCount( nCount )
{
    if( nCount > 0 )
    {
        m_pQueryPtrArray = new CQuery*[nCount];
        if( !m_pQueryPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pQueryPtrArray[i] = pQueryPtrArray[i];
            m_pQueryPtrArray[i]->AddRef();
        }
    }
}


CMultiQueryGeneric::~CMultiQueryGeneric()
{
    if( m_pQueryPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pQueryPtrArray[i] ) m_pQueryPtrArray[i]->CQuery::Release();
        delete[]( m_pQueryPtrArray );
    }
}


void CMultiQueryGeneric::SetSubQuery( int index, CQuery *pQuery )
{
    if( pQuery ) pQuery->AddRef();
    if( m_pQueryPtrArray[index] )
        m_pQueryPtrArray[index]->Release();
    m_pQueryPtrArray[index] = pQuery;
}




////////////////////////////////////////////////////////////
// CMultiQueryGeneric::GetDesc()
////////////////////////////////////////////////////////////
void CMultiQueryGeneric::GetDesc(  D3D_QUERY_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubQuery( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiQueryGeneric::Begin()
////////////////////////////////////////////////////////////
void CMultiQueryGeneric::Begin(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubAsynchronous( i )->Begin(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiQueryGeneric::End()
////////////////////////////////////////////////////////////
void CMultiQueryGeneric::End(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubAsynchronous( i )->End(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiQueryGeneric::GetData()
////////////////////////////////////////////////////////////
NRESULT CMultiQueryGeneric::GetData(  void * pData, UINT DataSize, UINT GetDataFlags )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubAsynchronous( i )->GetData(  pData,  DataSize,  GetDataFlags );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiQueryGeneric::GetDataSize()
////////////////////////////////////////////////////////////
UINT CMultiQueryGeneric::GetDataSize(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubAsynchronous( i )->GetDataSize(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiQueryGeneric::GetDevice()
////////////////////////////////////////////////////////////
void CMultiQueryGeneric::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiQueryGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiQueryGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiQueryGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiQueryGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiQueryGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiQueryGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiPredicate
//
////////////////////////////////////////////////////////////

CMultiPredicate::CMultiPredicate( int nCount ) : 
    m_nPredicateCount( nCount )
{
    if( nCount > 0 )
    {
        m_pPredicatePtrArray = new CPredicate*[nCount];
        if( !m_pPredicatePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pPredicatePtrArray[i] = NULL;
    }
}


CMultiPredicate::CMultiPredicate( CPredicate **pPredicatePtrArray, int nCount ) : 
    m_nPredicateCount( nCount )
{
    if( nCount > 0 )
    {
        m_pPredicatePtrArray = new CPredicate*[nCount];
        if( !m_pPredicatePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pPredicatePtrArray[i] = pPredicatePtrArray[i];
            m_pPredicatePtrArray[i]->AddRef();
        }
    }
}


CMultiPredicate::~CMultiPredicate()
{
    if( m_pPredicatePtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pPredicatePtrArray[i] ) m_pPredicatePtrArray[i]->CPredicate::Release();
        delete[]( m_pPredicatePtrArray );
    }
}


void CMultiPredicate::SetSubPredicate( int index, CPredicate *pPredicate )
{
    if( pPredicate ) pPredicate->AddRef();
    if( m_pPredicatePtrArray[index] )
        m_pPredicatePtrArray[index]->Release();
    m_pPredicatePtrArray[index] = pPredicate;
}




////////////////////////////////////////////////////////////
// CMultiPredicate::GetDesc()
////////////////////////////////////////////////////////////
void CMultiPredicate::GetDesc(  D3D_QUERY_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubQuery( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiPredicate::Begin()
////////////////////////////////////////////////////////////
void CMultiPredicate::Begin(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubAsynchronous( i )->Begin(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiPredicate::End()
////////////////////////////////////////////////////////////
void CMultiPredicate::End(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubAsynchronous( i )->End(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiPredicate::GetData()
////////////////////////////////////////////////////////////
NRESULT CMultiPredicate::GetData(  void * pData, UINT DataSize, UINT GetDataFlags )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubAsynchronous( i )->GetData(  pData,  DataSize,  GetDataFlags );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiPredicate::GetDataSize()
////////////////////////////////////////////////////////////
UINT CMultiPredicate::GetDataSize(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubAsynchronous( i )->GetDataSize(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiPredicate::GetDevice()
////////////////////////////////////////////////////////////
void CMultiPredicate::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiPredicate::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiPredicate::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiPredicate::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiPredicate::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiPredicate::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiPredicate::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiCounter
//
////////////////////////////////////////////////////////////

CMultiCounter::CMultiCounter( int nCount ) : 
    m_nCounterCount( nCount )
{
    if( nCount > 0 )
    {
        m_pCounterPtrArray = new CCounter*[nCount];
        if( !m_pCounterPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pCounterPtrArray[i] = NULL;
    }
}


CMultiCounter::CMultiCounter( CCounter **pCounterPtrArray, int nCount ) : 
    m_nCounterCount( nCount )
{
    if( nCount > 0 )
    {
        m_pCounterPtrArray = new CCounter*[nCount];
        if( !m_pCounterPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pCounterPtrArray[i] = pCounterPtrArray[i];
            m_pCounterPtrArray[i]->AddRef();
        }
    }
}


CMultiCounter::~CMultiCounter()
{
    if( m_pCounterPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pCounterPtrArray[i] ) m_pCounterPtrArray[i]->CCounter::Release();
        delete[]( m_pCounterPtrArray );
    }
}


void CMultiCounter::SetSubCounter( int index, CCounter *pCounter )
{
    if( pCounter ) pCounter->AddRef();
    if( m_pCounterPtrArray[index] )
        m_pCounterPtrArray[index]->Release();
    m_pCounterPtrArray[index] = pCounter;
}




////////////////////////////////////////////////////////////
// CMultiCounter::GetDesc()
////////////////////////////////////////////////////////////
void CMultiCounter::GetDesc(  D3D_COUNTER_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubCounter( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiCounter::Begin()
////////////////////////////////////////////////////////////
void CMultiCounter::Begin(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubAsynchronous( i )->Begin(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiCounter::End()
////////////////////////////////////////////////////////////
void CMultiCounter::End(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubAsynchronous( i )->End(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiCounter::GetData()
////////////////////////////////////////////////////////////
NRESULT CMultiCounter::GetData(  void * pData, UINT DataSize, UINT GetDataFlags )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubAsynchronous( i )->GetData(  pData,  DataSize,  GetDataFlags );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiCounter::GetDataSize()
////////////////////////////////////////////////////////////
UINT CMultiCounter::GetDataSize(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubAsynchronous( i )->GetDataSize(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiCounter::GetDevice()
////////////////////////////////////////////////////////////
void CMultiCounter::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiCounter::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiCounter::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiCounter::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiCounter::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiCounter::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiCounter::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiDevice
//
////////////////////////////////////////////////////////////

CMultiDevice::CMultiDevice( int nCount ) : 
    m_nDeviceCount( nCount )
{
    if( nCount > 0 )
    {
        m_pDevicePtrArray = new CDevice*[nCount];
        if( !m_pDevicePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pDevicePtrArray[i] = NULL;
    }
}


CMultiDevice::CMultiDevice( CDevice **pDevicePtrArray, int nCount ) : 
    m_nDeviceCount( nCount )
{
    if( nCount > 0 )
    {
        m_pDevicePtrArray = new CDevice*[nCount];
        if( !m_pDevicePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pDevicePtrArray[i] = pDevicePtrArray[i];
            m_pDevicePtrArray[i]->AddRef();
        }
    }
}


CMultiDevice::~CMultiDevice()
{
    if( m_pDevicePtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pDevicePtrArray[i] ) m_pDevicePtrArray[i]->CDevice::Release();
        delete[]( m_pDevicePtrArray );
    }
}


void CMultiDevice::SetSubDevice( int index, CDevice *pDevice )
{
    if( pDevice ) pDevice->AddRef();
    if( m_pDevicePtrArray[index] )
        m_pDevicePtrArray[index]->Release();
    m_pDevicePtrArray[index] = pDevice;
}




////////////////////////////////////////////////////////////
// CMultiDevice::VSSetConstantBuffers()
////////////////////////////////////////////////////////////
void CMultiDevice::VSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    CBuffer **ppConstantBuffersConv = new CBuffer*[NumBuffers];
    if( !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            ppConstantBuffersConv[j] = ppConstantBuffers[j] ? ppConstantBuffers[j]->GetSubBuffer( i ) : NULL;
        }
        GetSubDevice( i )->VSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    }

    delete[]( ppConstantBuffersConv );
}

void CMultiDevice::VSSetConstantBuffers1( UINT StartSlot, UINT NumBuffers, CBuffer *const* ppConstantBuffers, UINT32 const* pOffsets, UINT32 const* ppNumConstants )
{
    CBuffer **ppConstantBuffersConv = new CBuffer*[NumBuffers];
    if( !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            ppConstantBuffersConv[j] = ppConstantBuffers[j] ? ppConstantBuffers[j]->GetSubBuffer( i ) : NULL;
        }
        GetSubDevice( i )->VSSetConstantBuffers1(  StartSlot,  NumBuffers,  ppConstantBuffersConv, pOffsets, ppNumConstants );
    }

    delete[]( ppConstantBuffersConv );
}

////////////////////////////////////////////////////////////
// CMultiDevice::PSSetShaderResources()
////////////////////////////////////////////////////////////
void CMultiDevice::PSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    CShaderResourceView **ppShaderResourceViewsConv = new CShaderResourceView*[NumViews];
    if( !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumViews - 1; j != -1; --j )
        {
            ppShaderResourceViewsConv[j] = ppShaderResourceViews[j] ? ppShaderResourceViews[j]->GetSubShaderResourceView( i ) : NULL;
        }
        GetSubDevice( i )->PSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    }

    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::PSSetShader()
////////////////////////////////////////////////////////////
void CMultiDevice::PSSetShader(  CPixelShader * pPixelShader )
{
    assert( pPixelShader == NULL || pPixelShader->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->PSSetShader(  pPixelShader ? pPixelShader->GetSubPixelShader( i ) : NULL );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::PSSetSamplers()
////////////////////////////////////////////////////////////
void CMultiDevice::PSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    CSamplerState **ppSamplersConv = new CSamplerState*[NumSamplers];
    if( !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumSamplers - 1; j != -1; --j )
        {
            ppSamplersConv[j] = ppSamplers[j] ? ppSamplers[j]->GetSubSamplerState( i ) : NULL;
        }
        GetSubDevice( i )->PSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    }

    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::VSSetShader()
////////////////////////////////////////////////////////////
void CMultiDevice::VSSetShader(  CVertexShader * pVertexShader )
{
    assert( pVertexShader == NULL || pVertexShader->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->VSSetShader(  pVertexShader ? pVertexShader->GetSubVertexShader( i ) : NULL );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::DrawIndexed()
////////////////////////////////////////////////////////////
void CMultiDevice::DrawIndexed(  UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->DrawIndexed(  IndexCount,  StartIndexLocation,  BaseVertexLocation );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::Draw()
////////////////////////////////////////////////////////////
void CMultiDevice::Draw(  UINT VertexCount, UINT StartVertexLocation )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->Draw(  VertexCount,  StartVertexLocation );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::PSSetConstantBuffers()
////////////////////////////////////////////////////////////
void CMultiDevice::PSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    CBuffer **ppConstantBuffersConv = new CBuffer*[NumBuffers];
    if( !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            ppConstantBuffersConv[j] = ppConstantBuffers[j] ? ppConstantBuffers[j]->GetSubBuffer( i ) : NULL;
        }
        GetSubDevice( i )->PSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    }

    delete[]( ppConstantBuffersConv );
}

void CMultiDevice::PSSetConstantBuffers1( UINT StartSlot, UINT NumBuffers, CBuffer *const* ppConstantBuffers, UINT32 const* pOffsets, UINT32 const* ppNumConstants )
{
    CBuffer **ppConstantBuffersConv = new CBuffer*[NumBuffers];
    if( !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            ppConstantBuffersConv[j] = ppConstantBuffers[j] ? ppConstantBuffers[j]->GetSubBuffer( i ) : NULL;
        }
        GetSubDevice( i )->PSSetConstantBuffers1(  StartSlot,  NumBuffers,  ppConstantBuffersConv, pOffsets, ppNumConstants );
    }

    delete[]( ppConstantBuffersConv );
}

////////////////////////////////////////////////////////////
// CMultiDevice::IASetInputLayout()
////////////////////////////////////////////////////////////
void CMultiDevice::IASetInputLayout(  CInputLayout * pInputLayout )
{
    assert( pInputLayout == NULL || pInputLayout->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->IASetInputLayout(  pInputLayout ? pInputLayout->GetSubInputLayout( i ) : NULL );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::IASetVertexBuffers()
////////////////////////////////////////////////////////////
void CMultiDevice::IASetVertexBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppVertexBuffers, const UINT * pStrides, const UINT * pOffsets )
{
    CBuffer **ppVertexBuffersConv = new CBuffer*[NumBuffers];
    if( !ppVertexBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            ppVertexBuffersConv[j] = ppVertexBuffers[j] ? ppVertexBuffers[j]->GetSubBuffer( i ) : NULL;
        }
        GetSubDevice( i )->IASetVertexBuffers(  StartSlot,  NumBuffers,  ppVertexBuffersConv,  pStrides,  pOffsets );
    }

    delete[]( ppVertexBuffersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::IASetIndexBuffer()
////////////////////////////////////////////////////////////
void CMultiDevice::IASetIndexBuffer(  CBuffer * pIndexBuffer, DXGI_FORMAT Format, UINT Offset )
{
    assert( pIndexBuffer == NULL || pIndexBuffer->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->IASetIndexBuffer(  pIndexBuffer ? pIndexBuffer->GetSubBuffer( i ) : NULL,  Format,  Offset );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::DrawIndexedInstanced()
////////////////////////////////////////////////////////////
void CMultiDevice::DrawIndexedInstanced(  UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->DrawIndexedInstanced(  IndexCountPerInstance,  InstanceCount,  StartIndexLocation,  BaseVertexLocation,  StartInstanceLocation );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::DrawInstanced()
////////////////////////////////////////////////////////////
void CMultiDevice::DrawInstanced(  UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->DrawInstanced(  VertexCountPerInstance,  InstanceCount,  StartVertexLocation,  StartInstanceLocation );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::GSSetConstantBuffers()
////////////////////////////////////////////////////////////
void CMultiDevice::GSSetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer *const * ppConstantBuffers )
{
    CBuffer **ppConstantBuffersConv = new CBuffer*[NumBuffers];
    if( !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            ppConstantBuffersConv[j] = ppConstantBuffers[j] ? ppConstantBuffers[j]->GetSubBuffer( i ) : NULL;
        }
        GetSubDevice( i )->GSSetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
    }

    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::GSSetShader()
////////////////////////////////////////////////////////////
void CMultiDevice::GSSetShader(  CGeometryShader * pShader )
{
    assert( pShader == NULL || pShader->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->GSSetShader(  pShader ? pShader->GetSubGeometryShader( i ) : NULL );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::IASetPrimitiveTopology()
////////////////////////////////////////////////////////////
void CMultiDevice::IASetPrimitiveTopology(  D3D_PRIMITIVE_TOPOLOGY Topology )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->IASetPrimitiveTopology(  Topology );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::VSSetShaderResources()
////////////////////////////////////////////////////////////
void CMultiDevice::VSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    CShaderResourceView **ppShaderResourceViewsConv = new CShaderResourceView*[NumViews];
    if( !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumViews - 1; j != -1; --j )
        {
            ppShaderResourceViewsConv[j] = ppShaderResourceViews[j] ? ppShaderResourceViews[j]->GetSubShaderResourceView( i ) : NULL;
        }
        GetSubDevice( i )->VSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    }

    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::VSSetSamplers()
////////////////////////////////////////////////////////////
void CMultiDevice::VSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    CSamplerState **ppSamplersConv = new CSamplerState*[NumSamplers];
    if( !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumSamplers - 1; j != -1; --j )
        {
            ppSamplersConv[j] = ppSamplers[j] ? ppSamplers[j]->GetSubSamplerState( i ) : NULL;
        }
        GetSubDevice( i )->VSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    }

    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::SetPredication()
////////////////////////////////////////////////////////////
void CMultiDevice::SetPredication(  CPredicate * pPredicate, BOOL PredicateValue )
{
    assert( pPredicate == NULL || pPredicate->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->SetPredication(  pPredicate ? pPredicate->GetSubPredicate( i ) : NULL,  PredicateValue );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::GSSetShaderResources()
////////////////////////////////////////////////////////////
void CMultiDevice::GSSetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView *const * ppShaderResourceViews )
{
    CShaderResourceView **ppShaderResourceViewsConv = new CShaderResourceView*[NumViews];
    if( !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumViews - 1; j != -1; --j )
        {
            ppShaderResourceViewsConv[j] = ppShaderResourceViews[j] ? ppShaderResourceViews[j]->GetSubShaderResourceView( i ) : NULL;
        }
        GetSubDevice( i )->GSSetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
    }

    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::GSSetSamplers()
////////////////////////////////////////////////////////////
void CMultiDevice::GSSetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState *const * ppSamplers )
{
    CSamplerState **ppSamplersConv = new CSamplerState*[NumSamplers];
    if( !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumSamplers - 1; j != -1; --j )
        {
            ppSamplersConv[j] = ppSamplers[j] ? ppSamplers[j]->GetSubSamplerState( i ) : NULL;
        }
        GetSubDevice( i )->GSSetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
    }

    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::OMSetRenderTargets()
////////////////////////////////////////////////////////////
void CMultiDevice::OMSetRenderTargets(  UINT NumViews, CRenderTargetView *const * ppRenderTargetViews, CDepthStencilView * pDepthStencilView )
{
    CRenderTargetView **ppRenderTargetViewsConv = new CRenderTargetView*[NumViews];
    if( !ppRenderTargetViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    assert( pDepthStencilView == NULL || pDepthStencilView->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumViews - 1; j != -1; --j )
        {
            ppRenderTargetViewsConv[j] = ppRenderTargetViews[j] ? ppRenderTargetViews[j]->GetSubRenderTargetView( i ) : NULL;
        }
        GetSubDevice( i )->OMSetRenderTargets(  NumViews,  ppRenderTargetViewsConv,  pDepthStencilView ? pDepthStencilView->GetSubDepthStencilView( i ) : NULL );
    }

    delete[]( ppRenderTargetViewsConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::OMSetBlendState()
////////////////////////////////////////////////////////////
void CMultiDevice::OMSetBlendState(  CBlendState * pBlendState, const FLOAT BlendFactor[4], UINT SampleMask )
{
    assert( pBlendState == NULL || pBlendState->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->OMSetBlendState(  pBlendState ? pBlendState->GetSubBlendState( i ) : NULL,  BlendFactor,  SampleMask );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::OMSetDepthStencilState()
////////////////////////////////////////////////////////////
void CMultiDevice::OMSetDepthStencilState(  CDepthStencilState * pDepthStencilState, UINT StencilRef )
{
    assert( pDepthStencilState == NULL || pDepthStencilState->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->OMSetDepthStencilState(  pDepthStencilState ? pDepthStencilState->GetSubDepthStencilState( i ) : NULL,  StencilRef );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::SOSetTargets()
////////////////////////////////////////////////////////////
void CMultiDevice::SOSetTargets(  UINT NumBuffers, CBuffer *const * ppSOTargets, const UINT * pOffsets )
{
    CBuffer **ppSOTargetsConv = new CBuffer*[NumBuffers];
    if( !ppSOTargetsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            ppSOTargetsConv[j] = ppSOTargets[j] ? ppSOTargets[j]->GetSubBuffer( i ) : NULL;
        }
        GetSubDevice( i )->SOSetTargets(  NumBuffers,  ppSOTargetsConv,  pOffsets );
    }

    delete[]( ppSOTargetsConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::DrawAuto()
////////////////////////////////////////////////////////////
void CMultiDevice::DrawAuto(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->DrawAuto(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::RSSetState()
////////////////////////////////////////////////////////////
void CMultiDevice::RSSetState(  CRasterizerState * pRasterizerState )
{
    assert( pRasterizerState == NULL || pRasterizerState->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->RSSetState(  pRasterizerState ? pRasterizerState->GetSubRasterizerState( i ) : NULL );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::RSSetViewports()
////////////////////////////////////////////////////////////
void CMultiDevice::RSSetViewports(  UINT NumViewports, const D3D_VIEWPORT * pViewports )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->RSSetViewports(  NumViewports,  pViewports );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::RSSetScissorRects()
////////////////////////////////////////////////////////////
void CMultiDevice::RSSetScissorRects(  UINT NumRects, const D3D_RECT * pRects )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->RSSetScissorRects(  NumRects,  pRects );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::CopySubresourceRegion()
////////////////////////////////////////////////////////////
void CMultiDevice::CopySubresourceRegion(  CResource * pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, CResource * pSrcResource, UINT SrcSubresource, const D3D_BOX * pSrcBox )
{
    assert( pDstResource == NULL || pDstResource->GetSubObjectCount() == GetSubObjectCount() );
    assert( pSrcResource == NULL || pSrcResource->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->CopySubresourceRegion(  pDstResource ? pDstResource->GetSubResource( i ) : NULL,  DstSubresource,  DstX,  DstY,  DstZ,  pSrcResource ? pSrcResource->GetSubResource( i ) : NULL,  SrcSubresource,  pSrcBox );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::CopyResource()
////////////////////////////////////////////////////////////
void CMultiDevice::CopyResource(  CResource * pDstResource, CResource * pSrcResource )
{
    assert( pDstResource == NULL || pDstResource->GetSubObjectCount() == GetSubObjectCount() );
    assert( pSrcResource == NULL || pSrcResource->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->CopyResource(  pDstResource ? pDstResource->GetSubResource( i ) : NULL,  pSrcResource ? pSrcResource->GetSubResource( i ) : NULL );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::UpdateSubresource()
////////////////////////////////////////////////////////////
void CMultiDevice::UpdateSubresource(  CResource * pDstResource, UINT DstSubresource, const D3D_BOX * pDstBox, const void * pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch )
{
    assert( pDstResource == NULL || pDstResource->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->UpdateSubresource(  pDstResource ? pDstResource->GetSubResource( i ) : NULL,  DstSubresource,  pDstBox,  pSrcData,  SrcRowPitch,  SrcDepthPitch );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::ClearRenderTargetView()
////////////////////////////////////////////////////////////
void CMultiDevice::ClearRenderTargetView(  CRenderTargetView * pRenderTargetView, const FLOAT ColorRGBA[4] )
{
    assert( pRenderTargetView == NULL || pRenderTargetView->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->ClearRenderTargetView(  pRenderTargetView ? pRenderTargetView->GetSubRenderTargetView( i ) : NULL,  ColorRGBA );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::ClearDepthStencilView()
////////////////////////////////////////////////////////////
void CMultiDevice::ClearDepthStencilView(  CDepthStencilView * pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil )
{
    assert( pDepthStencilView == NULL || pDepthStencilView->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->ClearDepthStencilView(  pDepthStencilView ? pDepthStencilView->GetSubDepthStencilView( i ) : NULL,  ClearFlags,  Depth,  Stencil );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::GenerateMips()
////////////////////////////////////////////////////////////
void CMultiDevice::GenerateMips(  CShaderResourceView * pShaderResourceView )
{
    assert( pShaderResourceView == NULL || pShaderResourceView->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->GenerateMips(  pShaderResourceView ? pShaderResourceView->GetSubShaderResourceView( i ) : NULL );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::ResolveSubresource()
////////////////////////////////////////////////////////////
void CMultiDevice::ResolveSubresource(  CResource * pDstResource, UINT DstSubresource, CResource * pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format )
{
    assert( pDstResource == NULL || pDstResource->GetSubObjectCount() == GetSubObjectCount() );
    assert( pSrcResource == NULL || pSrcResource->GetSubObjectCount() == GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->ResolveSubresource(  pDstResource ? pDstResource->GetSubResource( i ) : NULL,  DstSubresource,  pSrcResource ? pSrcResource->GetSubResource( i ) : NULL,  SrcSubresource,  Format );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::VSGetConstantBuffers()
////////////////////////////////////////////////////////////
void CMultiDevice::VSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    CBuffer **ppConstantBuffersConv = new CBuffer*[NumBuffers];
    if( !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumBuffers - 1; j >= 0; --j )
        ppConstantBuffers[j] = NULL;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->VSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            if( ppConstantBuffersConv[j] )
            {
                if( !ppConstantBuffers[j] )
                {
                    // Allocate new multi wrapper object
                    ppConstantBuffers[j] = new CMultiBuffer( GetSubObjectCount() );
                    if( !ppConstantBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppConstantBuffers[j]->AddRef();
                }
                ppConstantBuffers[j]->SetSubBuffer( i, ppConstantBuffersConv[j] );
                ppConstantBuffersConv[j]->Release();
            }
        }
    }

    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::PSGetShaderResources()
////////////////////////////////////////////////////////////
void CMultiDevice::PSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    CShaderResourceView **ppShaderResourceViewsConv = new CShaderResourceView*[NumViews];
    if( !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumViews - 1; j >= 0; --j )
        ppShaderResourceViews[j] = NULL;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->PSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
        for( int j = NumViews - 1; j != -1; --j )
        {
            if( ppShaderResourceViewsConv[j] )
            {
                if( !ppShaderResourceViews[j] )
                {
                    // Allocate new multi wrapper object
                    ppShaderResourceViews[j] = new CMultiShaderResourceView( GetSubObjectCount() );
                    if( !ppShaderResourceViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppShaderResourceViews[j]->AddRef();
                }
                ppShaderResourceViews[j]->SetSubShaderResourceView( i, ppShaderResourceViewsConv[j] );
                ppShaderResourceViewsConv[j]->Release();
            }
        }
    }

    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::PSGetShader()
////////////////////////////////////////////////////////////
void CMultiDevice::PSGetShader(  CPixelShader ** ppPixelShader )
{
    CMultiPixelShader *pMultiPixelShader = new CMultiPixelShader( GetSubObjectCount() );
    if( !pMultiPixelShader ) throw( WRAPPER_OUT_OF_MEMORY );
    CPixelShader *pTempPixelShader;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->PSGetShader(  &pTempPixelShader );
        if( pTempPixelShader )
        {
            pMultiPixelShader->SetSubPixelShader( i, pTempPixelShader );
            pTempPixelShader->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiPixelShader );
            pMultiPixelShader = NULL;
            break;
        }
    }

    *ppPixelShader = pMultiPixelShader;
    if( *ppPixelShader ) (*ppPixelShader)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDevice::PSGetSamplers()
////////////////////////////////////////////////////////////
void CMultiDevice::PSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    CSamplerState **ppSamplersConv = new CSamplerState*[NumSamplers];
    if( !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumSamplers - 1; j >= 0; --j )
        ppSamplers[j] = NULL;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->PSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
        for( int j = NumSamplers - 1; j != -1; --j )
        {
            if( ppSamplersConv[j] )
            {
                if( !ppSamplers[j] )
                {
                    // Allocate new multi wrapper object
                    ppSamplers[j] = new CMultiSamplerState( GetSubObjectCount() );
                    if( !ppSamplers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppSamplers[j]->AddRef();
                }
                ppSamplers[j]->SetSubSamplerState( i, ppSamplersConv[j] );
                ppSamplersConv[j]->Release();
            }
        }
    }

    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::VSGetShader()
////////////////////////////////////////////////////////////
void CMultiDevice::VSGetShader(  CVertexShader ** ppVertexShader )
{
    CMultiVertexShader *pMultiVertexShader = new CMultiVertexShader( GetSubObjectCount() );
    if( !pMultiVertexShader ) throw( WRAPPER_OUT_OF_MEMORY );
    CVertexShader *pTempVertexShader;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->VSGetShader(  &pTempVertexShader );
        if( pTempVertexShader )
        {
            pMultiVertexShader->SetSubVertexShader( i, pTempVertexShader );
            pTempVertexShader->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiVertexShader );
            pMultiVertexShader = NULL;
            break;
        }
    }

    *ppVertexShader = pMultiVertexShader;
    if( *ppVertexShader ) (*ppVertexShader)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDevice::PSGetConstantBuffers()
////////////////////////////////////////////////////////////
void CMultiDevice::PSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    CBuffer **ppConstantBuffersConv = new CBuffer*[NumBuffers];
    if( !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumBuffers - 1; j >= 0; --j )
        ppConstantBuffers[j] = NULL;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->PSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            if( ppConstantBuffersConv[j] )
            {
                if( !ppConstantBuffers[j] )
                {
                    // Allocate new multi wrapper object
                    ppConstantBuffers[j] = new CMultiBuffer( GetSubObjectCount() );
                    if( !ppConstantBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppConstantBuffers[j]->AddRef();
                }
                ppConstantBuffers[j]->SetSubBuffer( i, ppConstantBuffersConv[j] );
                ppConstantBuffersConv[j]->Release();
            }
        }
    }

    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::IAGetInputLayout()
////////////////////////////////////////////////////////////
void CMultiDevice::IAGetInputLayout(  CInputLayout ** ppInputLayout )
{
    CMultiInputLayout *pMultiInputLayout = new CMultiInputLayout( GetSubObjectCount() );
    if( !pMultiInputLayout ) throw( WRAPPER_OUT_OF_MEMORY );
    CInputLayout *pTempInputLayout;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->IAGetInputLayout(  &pTempInputLayout );
        if( pTempInputLayout )
        {
            pMultiInputLayout->SetSubInputLayout( i, pTempInputLayout );
            pTempInputLayout->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiInputLayout );
            pMultiInputLayout = NULL;
            break;
        }
    }

    *ppInputLayout = pMultiInputLayout;
    if( *ppInputLayout ) (*ppInputLayout)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDevice::IAGetVertexBuffers()
////////////////////////////////////////////////////////////
void CMultiDevice::IAGetVertexBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppVertexBuffers, UINT * pStrides, UINT * pOffsets )
{
    CBuffer **ppVertexBuffersConv = new CBuffer*[NumBuffers];
    if( !ppVertexBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumBuffers - 1; j >= 0; --j )
        ppVertexBuffers[j] = NULL;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->IAGetVertexBuffers(  StartSlot,  NumBuffers,  ppVertexBuffersConv,  pStrides,  pOffsets );
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            if( ppVertexBuffersConv[j] )
            {
                if( !ppVertexBuffers[j] )
                {
                    // Allocate new multi wrapper object
                    ppVertexBuffers[j] = new CMultiBuffer( GetSubObjectCount() );
                    if( !ppVertexBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppVertexBuffers[j]->AddRef();
                }
                ppVertexBuffers[j]->SetSubBuffer( i, ppVertexBuffersConv[j] );
                ppVertexBuffersConv[j]->Release();
            }
        }
    }

    delete[]( ppVertexBuffersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::IAGetIndexBuffer()
////////////////////////////////////////////////////////////
void CMultiDevice::IAGetIndexBuffer(  CBuffer ** pIndexBuffer, DXGI_FORMAT * Format, UINT * Offset )
{
    CMultiBuffer *pMultiBuffer = new CMultiBuffer( GetSubObjectCount() );
    if( !pMultiBuffer ) throw( WRAPPER_OUT_OF_MEMORY );
    CBuffer *pTempBuffer;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->IAGetIndexBuffer(  &pTempBuffer,  Format,  Offset );
        if( pTempBuffer )
        {
            pMultiBuffer->SetSubBuffer( i, pTempBuffer );
            pTempBuffer->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiBuffer );
            pMultiBuffer = NULL;
            break;
        }
    }

    *pIndexBuffer = pMultiBuffer;
    if( *pIndexBuffer ) (*pIndexBuffer)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDevice::GSGetConstantBuffers()
////////////////////////////////////////////////////////////
void CMultiDevice::GSGetConstantBuffers(  UINT StartSlot, UINT NumBuffers, CBuffer ** ppConstantBuffers )
{
    CBuffer **ppConstantBuffersConv = new CBuffer*[NumBuffers];
    if( !ppConstantBuffersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumBuffers - 1; j >= 0; --j )
        ppConstantBuffers[j] = NULL;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->GSGetConstantBuffers(  StartSlot,  NumBuffers,  ppConstantBuffersConv );
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            if( ppConstantBuffersConv[j] )
            {
                if( !ppConstantBuffers[j] )
                {
                    // Allocate new multi wrapper object
                    ppConstantBuffers[j] = new CMultiBuffer( GetSubObjectCount() );
                    if( !ppConstantBuffers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppConstantBuffers[j]->AddRef();
                }
                ppConstantBuffers[j]->SetSubBuffer( i, ppConstantBuffersConv[j] );
                ppConstantBuffersConv[j]->Release();
            }
        }
    }

    delete[]( ppConstantBuffersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::GSGetShader()
////////////////////////////////////////////////////////////
void CMultiDevice::GSGetShader(  CGeometryShader ** ppGeometryShader )
{
    CMultiGeometryShader *pMultiGeometryShader = new CMultiGeometryShader( GetSubObjectCount() );
    if( !pMultiGeometryShader ) throw( WRAPPER_OUT_OF_MEMORY );
    CGeometryShader *pTempGeometryShader;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->GSGetShader(  &pTempGeometryShader );
        if( pTempGeometryShader )
        {
            pMultiGeometryShader->SetSubGeometryShader( i, pTempGeometryShader );
            pTempGeometryShader->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiGeometryShader );
            pMultiGeometryShader = NULL;
            break;
        }
    }

    *ppGeometryShader = pMultiGeometryShader;
    if( *ppGeometryShader ) (*ppGeometryShader)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDevice::IAGetPrimitiveTopology()
////////////////////////////////////////////////////////////
void CMultiDevice::IAGetPrimitiveTopology(  D3D_PRIMITIVE_TOPOLOGY * pTopology )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->IAGetPrimitiveTopology(  pTopology );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::VSGetShaderResources()
////////////////////////////////////////////////////////////
void CMultiDevice::VSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    CShaderResourceView **ppShaderResourceViewsConv = new CShaderResourceView*[NumViews];
    if( !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumViews - 1; j >= 0; --j )
        ppShaderResourceViews[j] = NULL;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->VSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
        for( int j = NumViews - 1; j != -1; --j )
        {
            if( ppShaderResourceViewsConv[j] )
            {
                if( !ppShaderResourceViews[j] )
                {
                    // Allocate new multi wrapper object
                    ppShaderResourceViews[j] = new CMultiShaderResourceView( GetSubObjectCount() );
                    if( !ppShaderResourceViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppShaderResourceViews[j]->AddRef();
                }
                ppShaderResourceViews[j]->SetSubShaderResourceView( i, ppShaderResourceViewsConv[j] );
                ppShaderResourceViewsConv[j]->Release();
            }
        }
    }

    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::VSGetSamplers()
////////////////////////////////////////////////////////////
void CMultiDevice::VSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    CSamplerState **ppSamplersConv = new CSamplerState*[NumSamplers];
    if( !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumSamplers - 1; j >= 0; --j )
        ppSamplers[j] = NULL;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->VSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
        for( int j = NumSamplers - 1; j != -1; --j )
        {
            if( ppSamplersConv[j] )
            {
                if( !ppSamplers[j] )
                {
                    // Allocate new multi wrapper object
                    ppSamplers[j] = new CMultiSamplerState( GetSubObjectCount() );
                    if( !ppSamplers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppSamplers[j]->AddRef();
                }
                ppSamplers[j]->SetSubSamplerState( i, ppSamplersConv[j] );
                ppSamplersConv[j]->Release();
            }
        }
    }

    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::GetPredication()
////////////////////////////////////////////////////////////
void CMultiDevice::GetPredication(  CPredicate ** ppPredicate, BOOL * pPredicateValue )
{
    CMultiPredicate *pMultiPredicate = new CMultiPredicate( GetSubObjectCount() );
    if( !pMultiPredicate ) throw( WRAPPER_OUT_OF_MEMORY );
    CPredicate *pTempPredicate;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->GetPredication(  &pTempPredicate,  pPredicateValue );
        if( pTempPredicate )
        {
            pMultiPredicate->SetSubPredicate( i, pTempPredicate );
            pTempPredicate->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiPredicate );
            pMultiPredicate = NULL;
            break;
        }
    }

    *ppPredicate = pMultiPredicate;
    if( *ppPredicate ) (*ppPredicate)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDevice::GSGetShaderResources()
////////////////////////////////////////////////////////////
void CMultiDevice::GSGetShaderResources(  UINT StartSlot, UINT NumViews, CShaderResourceView ** ppShaderResourceViews )
{
    CShaderResourceView **ppShaderResourceViewsConv = new CShaderResourceView*[NumViews];
    if( !ppShaderResourceViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumViews - 1; j >= 0; --j )
        ppShaderResourceViews[j] = NULL;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->GSGetShaderResources(  StartSlot,  NumViews,  ppShaderResourceViewsConv );
        for( int j = NumViews - 1; j != -1; --j )
        {
            if( ppShaderResourceViewsConv[j] )
            {
                if( !ppShaderResourceViews[j] )
                {
                    // Allocate new multi wrapper object
                    ppShaderResourceViews[j] = new CMultiShaderResourceView( GetSubObjectCount() );
                    if( !ppShaderResourceViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppShaderResourceViews[j]->AddRef();
                }
                ppShaderResourceViews[j]->SetSubShaderResourceView( i, ppShaderResourceViewsConv[j] );
                ppShaderResourceViewsConv[j]->Release();
            }
        }
    }

    delete[]( ppShaderResourceViewsConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::GSGetSamplers()
////////////////////////////////////////////////////////////
void CMultiDevice::GSGetSamplers(  UINT StartSlot, UINT NumSamplers, CSamplerState ** ppSamplers )
{
    CSamplerState **ppSamplersConv = new CSamplerState*[NumSamplers];
    if( !ppSamplersConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumSamplers - 1; j >= 0; --j )
        ppSamplers[j] = NULL;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->GSGetSamplers(  StartSlot,  NumSamplers,  ppSamplersConv );
        for( int j = NumSamplers - 1; j != -1; --j )
        {
            if( ppSamplersConv[j] )
            {
                if( !ppSamplers[j] )
                {
                    // Allocate new multi wrapper object
                    ppSamplers[j] = new CMultiSamplerState( GetSubObjectCount() );
                    if( !ppSamplers[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppSamplers[j]->AddRef();
                }
                ppSamplers[j]->SetSubSamplerState( i, ppSamplersConv[j] );
                ppSamplersConv[j]->Release();
            }
        }
    }

    delete[]( ppSamplersConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::OMGetRenderTargets()
////////////////////////////////////////////////////////////
void CMultiDevice::OMGetRenderTargets(  UINT NumViews, CRenderTargetView ** ppRenderTargetViews, CDepthStencilView ** ppDepthStencilView )
{
    CRenderTargetView **ppRenderTargetViewsConv = new CRenderTargetView*[NumViews];
    if( !ppRenderTargetViewsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumViews - 1; j >= 0; --j )
        ppRenderTargetViews[j] = NULL;
    CMultiDepthStencilView *pMultiDepthStencilView = new CMultiDepthStencilView( GetSubObjectCount() );
    if( !pMultiDepthStencilView ) throw( WRAPPER_OUT_OF_MEMORY );
    CDepthStencilView *pTempDepthStencilView;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->OMGetRenderTargets(  NumViews,  ppRenderTargetViewsConv,  &pTempDepthStencilView );
        for( int j = NumViews - 1; j != -1; --j )
        {
            if( ppRenderTargetViewsConv[j] )
            {
                if( !ppRenderTargetViews[j] )
                {
                    // Allocate new multi wrapper object
                    ppRenderTargetViews[j] = new CMultiRenderTargetView( GetSubObjectCount() );
                    if( !ppRenderTargetViews[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppRenderTargetViews[j]->AddRef();
                }
                ppRenderTargetViews[j]->SetSubRenderTargetView( i, ppRenderTargetViewsConv[j] );
                ppRenderTargetViewsConv[j]->Release();
            }
        }
        if( pTempDepthStencilView )
        {
            pMultiDepthStencilView->SetSubDepthStencilView( i, pTempDepthStencilView );
            pTempDepthStencilView->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDepthStencilView );
            pMultiDepthStencilView = NULL;
            break;
        }
    }

    delete[]( ppRenderTargetViewsConv );
    *ppDepthStencilView = pMultiDepthStencilView;
    if( *ppDepthStencilView ) (*ppDepthStencilView)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDevice::OMGetBlendState()
////////////////////////////////////////////////////////////
void CMultiDevice::OMGetBlendState(  CBlendState ** ppBlendState, FLOAT BlendFactor[4], UINT * pSampleMask )
{
    CMultiBlendState *pMultiBlendState = new CMultiBlendState( GetSubObjectCount() );
    if( !pMultiBlendState ) throw( WRAPPER_OUT_OF_MEMORY );
    CBlendState *pTempBlendState;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->OMGetBlendState(  &pTempBlendState,  BlendFactor,  pSampleMask );
        if( pTempBlendState )
        {
            pMultiBlendState->SetSubBlendState( i, pTempBlendState );
            pTempBlendState->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiBlendState );
            pMultiBlendState = NULL;
            break;
        }
    }

    *ppBlendState = pMultiBlendState;
    if( *ppBlendState ) (*ppBlendState)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDevice::OMGetDepthStencilState()
////////////////////////////////////////////////////////////
void CMultiDevice::OMGetDepthStencilState(  CDepthStencilState ** ppDepthStencilState, UINT * pStencilRef )
{
    CMultiDepthStencilState *pMultiDepthStencilState = new CMultiDepthStencilState( GetSubObjectCount() );
    if( !pMultiDepthStencilState ) throw( WRAPPER_OUT_OF_MEMORY );
    CDepthStencilState *pTempDepthStencilState;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->OMGetDepthStencilState(  &pTempDepthStencilState,  pStencilRef );
        if( pTempDepthStencilState )
        {
            pMultiDepthStencilState->SetSubDepthStencilState( i, pTempDepthStencilState );
            pTempDepthStencilState->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDepthStencilState );
            pMultiDepthStencilState = NULL;
            break;
        }
    }

    *ppDepthStencilState = pMultiDepthStencilState;
    if( *ppDepthStencilState ) (*ppDepthStencilState)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDevice::SOGetTargets()
////////////////////////////////////////////////////////////
void CMultiDevice::SOGetTargets(  UINT NumBuffers, CBuffer ** ppSOTargets, UINT * pOffsets )
{
    CBuffer **ppSOTargetsConv = new CBuffer*[NumBuffers];
    if( !ppSOTargetsConv ) throw( WRAPPER_OUT_OF_MEMORY );
    // Fill output array with NULL
    for( int j = NumBuffers - 1; j >= 0; --j )
        ppSOTargets[j] = NULL;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->SOGetTargets(  NumBuffers,  ppSOTargetsConv,  pOffsets );
        for( int j = NumBuffers - 1; j != -1; --j )
        {
            if( ppSOTargetsConv[j] )
            {
                if( !ppSOTargets[j] )
                {
                    // Allocate new multi wrapper object
                    ppSOTargets[j] = new CMultiBuffer( GetSubObjectCount() );
                    if( !ppSOTargets[j] ) throw( WRAPPER_OUT_OF_MEMORY );
                    ppSOTargets[j]->AddRef();
                }
                ppSOTargets[j]->SetSubBuffer( i, ppSOTargetsConv[j] );
                ppSOTargetsConv[j]->Release();
            }
        }
    }

    delete[]( ppSOTargetsConv );
}


////////////////////////////////////////////////////////////
// CMultiDevice::RSGetState()
////////////////////////////////////////////////////////////
void CMultiDevice::RSGetState(  CRasterizerState ** ppRasterizerState )
{
    CMultiRasterizerState *pMultiRasterizerState = new CMultiRasterizerState( GetSubObjectCount() );
    if( !pMultiRasterizerState ) throw( WRAPPER_OUT_OF_MEMORY );
    CRasterizerState *pTempRasterizerState;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->RSGetState(  &pTempRasterizerState );
        if( pTempRasterizerState )
        {
            pMultiRasterizerState->SetSubRasterizerState( i, pTempRasterizerState );
            pTempRasterizerState->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiRasterizerState );
            pMultiRasterizerState = NULL;
            break;
        }
    }

    *ppRasterizerState = pMultiRasterizerState;
    if( *ppRasterizerState ) (*ppRasterizerState)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiDevice::RSGetViewports()
////////////////////////////////////////////////////////////
void CMultiDevice::RSGetViewports(  UINT * NumViewports, D3D_VIEWPORT * pViewports )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->RSGetViewports(  NumViewports,  pViewports );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::RSGetScissorRects()
////////////////////////////////////////////////////////////
void CMultiDevice::RSGetScissorRects(  UINT * NumRects, D3D_RECT * pRects )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->RSGetScissorRects(  NumRects,  pRects );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::GetDeviceRemovedReason()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::GetDeviceRemovedReason(  )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->GetDeviceRemovedReason(  );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::SetExceptionMode()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::SetExceptionMode(  UINT RaiseFlags )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->SetExceptionMode(  RaiseFlags );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::GetExceptionMode()
////////////////////////////////////////////////////////////
UINT CMultiDevice::GetExceptionMode(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubDevice( i )->GetExceptionMode(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::ClearState()
////////////////////////////////////////////////////////////
void CMultiDevice::ClearState(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->ClearState(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::Flush()
////////////////////////////////////////////////////////////
void CMultiDevice::Flush(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->Flush(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateBuffer()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateBuffer(  const D3D_BUFFER_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CBuffer ** ppBuffer )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiBuffer *pMultiBuffer = new CMultiBuffer( GetSubObjectCount() );
    if( !pMultiBuffer ) throw( WRAPPER_OUT_OF_MEMORY );
    CBuffer *pTempBuffer;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateBuffer(  pDesc,  pInitialData,  &pTempBuffer );
        if( pTempBuffer )
        {
            pMultiBuffer->SetSubBuffer( i, pTempBuffer );
            pTempBuffer->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiBuffer );
            pMultiBuffer = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiBuffer );
        return rval;
    }

    *ppBuffer = pMultiBuffer;
    if( *ppBuffer ) (*ppBuffer)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateTexture1D()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateTexture1D(  const D3D_TEXTURE1D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture1D ** ppTexture1D )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiTexture1D *pMultiTexture1D = new CMultiTexture1D( GetSubObjectCount() );
    if( !pMultiTexture1D ) throw( WRAPPER_OUT_OF_MEMORY );
    CTexture1D *pTempTexture1D;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateTexture1D(  pDesc,  pInitialData,  &pTempTexture1D );
        if( pTempTexture1D )
        {
            pMultiTexture1D->SetSubTexture1D( i, pTempTexture1D );
            pTempTexture1D->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiTexture1D );
            pMultiTexture1D = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiTexture1D );
        return rval;
    }

    *ppTexture1D = pMultiTexture1D;
    if( *ppTexture1D ) (*ppTexture1D)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateTexture2D()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateTexture2D(  const D3D_TEXTURE2D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture2D ** ppTexture2D )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiTexture2D *pMultiTexture2D = new CMultiTexture2D( GetSubObjectCount() );
    if( !pMultiTexture2D ) throw( WRAPPER_OUT_OF_MEMORY );
    CTexture2D *pTempTexture2D;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateTexture2D(  pDesc,  pInitialData,  &pTempTexture2D );
        if( pTempTexture2D )
        {
            pMultiTexture2D->SetSubTexture2D( i, pTempTexture2D );
            pTempTexture2D->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiTexture2D );
            pMultiTexture2D = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiTexture2D );
        return rval;
    }

    *ppTexture2D = pMultiTexture2D;
    if( *ppTexture2D ) (*ppTexture2D)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateTexture3D()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateTexture3D(  const D3D_TEXTURE3D_DESC * pDesc, const D3D_SUBRESOURCE_DATA * pInitialData, CTexture3D ** ppTexture3D )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiTexture3D *pMultiTexture3D = new CMultiTexture3D( GetSubObjectCount() );
    if( !pMultiTexture3D ) throw( WRAPPER_OUT_OF_MEMORY );
    CTexture3D *pTempTexture3D;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateTexture3D(  pDesc,  pInitialData,  &pTempTexture3D );
        if( pTempTexture3D )
        {
            pMultiTexture3D->SetSubTexture3D( i, pTempTexture3D );
            pTempTexture3D->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiTexture3D );
            pMultiTexture3D = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiTexture3D );
        return rval;
    }

    *ppTexture3D = pMultiTexture3D;
    if( *ppTexture3D ) (*ppTexture3D)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateShaderResourceView()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateShaderResourceView(  CResource * pResource, const D3D_SHADER_RESOURCE_VIEW_DESC * pDesc, CShaderResourceView ** ppSRView )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    assert( pResource == NULL || pResource->GetSubObjectCount() == GetSubObjectCount() );
    CMultiShaderResourceView *pMultiShaderResourceView = new CMultiShaderResourceView( GetSubObjectCount() );
    if( !pMultiShaderResourceView ) throw( WRAPPER_OUT_OF_MEMORY );
    CShaderResourceView *pTempShaderResourceView;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateShaderResourceView(  pResource ? pResource->GetSubResource( i ) : NULL,  pDesc,  &pTempShaderResourceView );
        if( pTempShaderResourceView )
        {
            pMultiShaderResourceView->SetSubShaderResourceView( i, pTempShaderResourceView );
            pTempShaderResourceView->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiShaderResourceView );
            pMultiShaderResourceView = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiShaderResourceView );
        return rval;
    }

    *ppSRView = pMultiShaderResourceView;
    if( *ppSRView ) (*ppSRView)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateRenderTargetView()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateRenderTargetView(  CResource * pResource, const D3D_RENDER_TARGET_VIEW_DESC * pDesc, CRenderTargetView ** ppRTView )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    assert( pResource == NULL || pResource->GetSubObjectCount() == GetSubObjectCount() );
    CMultiRenderTargetView *pMultiRenderTargetView = new CMultiRenderTargetView( GetSubObjectCount() );
    if( !pMultiRenderTargetView ) throw( WRAPPER_OUT_OF_MEMORY );
    CRenderTargetView *pTempRenderTargetView;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateRenderTargetView(  pResource ? pResource->GetSubResource( i ) : NULL,  pDesc,  &pTempRenderTargetView );
        if( pTempRenderTargetView )
        {
            pMultiRenderTargetView->SetSubRenderTargetView( i, pTempRenderTargetView );
            pTempRenderTargetView->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiRenderTargetView );
            pMultiRenderTargetView = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiRenderTargetView );
        return rval;
    }

    *ppRTView = pMultiRenderTargetView;
    if( *ppRTView ) (*ppRTView)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateDepthStencilView()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateDepthStencilView(  CResource * pResource, const D3D_DEPTH_STENCIL_VIEW_DESC * pDesc, CDepthStencilView ** ppDepthStencilView )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    assert( pResource == NULL || pResource->GetSubObjectCount() == GetSubObjectCount() );
    CMultiDepthStencilView *pMultiDepthStencilView = new CMultiDepthStencilView( GetSubObjectCount() );
    if( !pMultiDepthStencilView ) throw( WRAPPER_OUT_OF_MEMORY );
    CDepthStencilView *pTempDepthStencilView;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateDepthStencilView(  pResource ? pResource->GetSubResource( i ) : NULL,  pDesc,  &pTempDepthStencilView );
        if( pTempDepthStencilView )
        {
            pMultiDepthStencilView->SetSubDepthStencilView( i, pTempDepthStencilView );
            pTempDepthStencilView->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDepthStencilView );
            pMultiDepthStencilView = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiDepthStencilView );
        return rval;
    }

    *ppDepthStencilView = pMultiDepthStencilView;
    if( *ppDepthStencilView ) (*ppDepthStencilView)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateInputLayout()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateInputLayout(  const D3D_INPUT_ELEMENT_DESC * pInputElementDescs, UINT NumElements, const void * pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, CInputLayout ** ppInputLayout )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiInputLayout *pMultiInputLayout = new CMultiInputLayout( GetSubObjectCount() );
    if( !pMultiInputLayout ) throw( WRAPPER_OUT_OF_MEMORY );
    CInputLayout *pTempInputLayout;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateInputLayout(  pInputElementDescs,  NumElements,  pShaderBytecodeWithInputSignature,  BytecodeLength,  &pTempInputLayout );
        if( pTempInputLayout )
        {
            pMultiInputLayout->SetSubInputLayout( i, pTempInputLayout );
            pTempInputLayout->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiInputLayout );
            pMultiInputLayout = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiInputLayout );
        return rval;
    }

    *ppInputLayout = pMultiInputLayout;
    if( *ppInputLayout ) (*ppInputLayout)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateVertexShader()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateVertexShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CVertexShader ** ppVertexShader )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiVertexShader *pMultiVertexShader = new CMultiVertexShader( GetSubObjectCount() );
    if( !pMultiVertexShader ) throw( WRAPPER_OUT_OF_MEMORY );
    CVertexShader *pTempVertexShader;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateVertexShader(  pShaderBytecode,  BytecodeLength,  &pTempVertexShader );
        if( pTempVertexShader )
        {
            pMultiVertexShader->SetSubVertexShader( i, pTempVertexShader );
            pTempVertexShader->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiVertexShader );
            pMultiVertexShader = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiVertexShader );
        return rval;
    }

    *ppVertexShader = pMultiVertexShader;
    if( *ppVertexShader ) (*ppVertexShader)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateGeometryShader()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateGeometryShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CGeometryShader ** ppGeometryShader )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiGeometryShader *pMultiGeometryShader = new CMultiGeometryShader( GetSubObjectCount() );
    if( !pMultiGeometryShader ) throw( WRAPPER_OUT_OF_MEMORY );
    CGeometryShader *pTempGeometryShader;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateGeometryShader(  pShaderBytecode,  BytecodeLength,  &pTempGeometryShader );
        if( pTempGeometryShader )
        {
            pMultiGeometryShader->SetSubGeometryShader( i, pTempGeometryShader );
            pTempGeometryShader->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiGeometryShader );
            pMultiGeometryShader = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiGeometryShader );
        return rval;
    }

    *ppGeometryShader = pMultiGeometryShader;
    if( *ppGeometryShader ) (*ppGeometryShader)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateGeometryShaderWithStreamOutput()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateGeometryShaderWithStreamOutput(  const void * pShaderBytecode, SIZE_T BytecodeLength, const D3D_SO_DECLARATION_ENTRY * pSODeclaration, UINT NumEntries, UINT OutputStreamStride, CGeometryShader ** ppGeometryShader )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiGeometryShader *pMultiGeometryShader = new CMultiGeometryShader( GetSubObjectCount() );
    if( !pMultiGeometryShader ) throw( WRAPPER_OUT_OF_MEMORY );
    CGeometryShader *pTempGeometryShader;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateGeometryShaderWithStreamOutput(  pShaderBytecode,  BytecodeLength,  pSODeclaration,  NumEntries,  OutputStreamStride,  &pTempGeometryShader );
        if( pTempGeometryShader )
        {
            pMultiGeometryShader->SetSubGeometryShader( i, pTempGeometryShader );
            pTempGeometryShader->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiGeometryShader );
            pMultiGeometryShader = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiGeometryShader );
        return rval;
    }

    *ppGeometryShader = pMultiGeometryShader;
    if( *ppGeometryShader ) (*ppGeometryShader)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreatePixelShader()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreatePixelShader(  const void * pShaderBytecode, SIZE_T BytecodeLength, CPixelShader ** ppPixelShader )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiPixelShader *pMultiPixelShader = new CMultiPixelShader( GetSubObjectCount() );
    if( !pMultiPixelShader ) throw( WRAPPER_OUT_OF_MEMORY );
    CPixelShader *pTempPixelShader;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreatePixelShader(  pShaderBytecode,  BytecodeLength,  &pTempPixelShader );
        if( pTempPixelShader )
        {
            pMultiPixelShader->SetSubPixelShader( i, pTempPixelShader );
            pTempPixelShader->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiPixelShader );
            pMultiPixelShader = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiPixelShader );
        return rval;
    }

    *ppPixelShader = pMultiPixelShader;
    if( *ppPixelShader ) (*ppPixelShader)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateBlendState()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateBlendState(  const D3D_BLEND_DESC * pBlendStateDesc, CBlendState ** ppBlendState )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiBlendState *pMultiBlendState = new CMultiBlendState( GetSubObjectCount() );
    if( !pMultiBlendState ) throw( WRAPPER_OUT_OF_MEMORY );
    CBlendState *pTempBlendState;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateBlendState(  pBlendStateDesc,  &pTempBlendState );
        if( pTempBlendState )
        {
            pMultiBlendState->SetSubBlendState( i, pTempBlendState );
            pTempBlendState->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiBlendState );
            pMultiBlendState = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiBlendState );
        return rval;
    }

    *ppBlendState = pMultiBlendState;
    if( *ppBlendState ) (*ppBlendState)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateDepthStencilState()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateDepthStencilState(  const D3D_DEPTH_STENCIL_DESC * pDepthStencilDesc, CDepthStencilState ** ppDepthStencilState )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiDepthStencilState *pMultiDepthStencilState = new CMultiDepthStencilState( GetSubObjectCount() );
    if( !pMultiDepthStencilState ) throw( WRAPPER_OUT_OF_MEMORY );
    CDepthStencilState *pTempDepthStencilState;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateDepthStencilState(  pDepthStencilDesc,  &pTempDepthStencilState );
        if( pTempDepthStencilState )
        {
            pMultiDepthStencilState->SetSubDepthStencilState( i, pTempDepthStencilState );
            pTempDepthStencilState->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDepthStencilState );
            pMultiDepthStencilState = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiDepthStencilState );
        return rval;
    }

    *ppDepthStencilState = pMultiDepthStencilState;
    if( *ppDepthStencilState ) (*ppDepthStencilState)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateRasterizerState()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateRasterizerState(  const D3D_RASTERIZER_DESC * pRasterizerDesc, CRasterizerState ** ppRasterizerState )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiRasterizerState *pMultiRasterizerState = new CMultiRasterizerState( GetSubObjectCount() );
    if( !pMultiRasterizerState ) throw( WRAPPER_OUT_OF_MEMORY );
    CRasterizerState *pTempRasterizerState;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateRasterizerState(  pRasterizerDesc,  &pTempRasterizerState );
        if( pTempRasterizerState )
        {
            pMultiRasterizerState->SetSubRasterizerState( i, pTempRasterizerState );
            pTempRasterizerState->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiRasterizerState );
            pMultiRasterizerState = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiRasterizerState );
        return rval;
    }

    *ppRasterizerState = pMultiRasterizerState;
    if( *ppRasterizerState ) (*ppRasterizerState)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateSamplerState()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateSamplerState(  const D3D_SAMPLER_DESC * pSamplerDesc, CSamplerState ** ppSamplerState )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiSamplerState *pMultiSamplerState = new CMultiSamplerState( GetSubObjectCount() );
    if( !pMultiSamplerState ) throw( WRAPPER_OUT_OF_MEMORY );
    CSamplerState *pTempSamplerState;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateSamplerState(  pSamplerDesc,  &pTempSamplerState );
        if( pTempSamplerState )
        {
            pMultiSamplerState->SetSubSamplerState( i, pTempSamplerState );
            pTempSamplerState->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiSamplerState );
            pMultiSamplerState = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiSamplerState );
        return rval;
    }

    *ppSamplerState = pMultiSamplerState;
    if( *ppSamplerState ) (*ppSamplerState)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateQuery()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateQuery(  const D3D_QUERY_DESC * pQueryDesc, CQuery ** ppQuery )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiQueryGeneric *pMultiQuery = new CMultiQueryGeneric( GetSubObjectCount() );
    if( !pMultiQuery ) throw( WRAPPER_OUT_OF_MEMORY );
    CQuery *pTempQuery;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateQuery(  pQueryDesc,  &pTempQuery );
        if( pTempQuery )
        {
            pMultiQuery->SetSubQuery( i, pTempQuery );
            pTempQuery->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiQuery );
            pMultiQuery = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiQuery );
        return rval;
    }

    *ppQuery = pMultiQuery;
    if( *ppQuery ) (*ppQuery)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreatePredicate()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreatePredicate(  const D3D_QUERY_DESC * pPredicateDesc, CPredicate ** ppPredicate )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiPredicate *pMultiPredicate = new CMultiPredicate( GetSubObjectCount() );
    if( !pMultiPredicate ) throw( WRAPPER_OUT_OF_MEMORY );
    CPredicate *pTempPredicate;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreatePredicate(  pPredicateDesc,  &pTempPredicate );
        if( pTempPredicate )
        {
            pMultiPredicate->SetSubPredicate( i, pTempPredicate );
            pTempPredicate->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiPredicate );
            pMultiPredicate = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiPredicate );
        return rval;
    }

    *ppPredicate = pMultiPredicate;
    if( *ppPredicate ) (*ppPredicate)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateCounter()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateCounter(  const D3D_COUNTER_DESC * pCounterDesc, CCounter ** ppCounter )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiCounter *pMultiCounter = new CMultiCounter( GetSubObjectCount() );
    if( !pMultiCounter ) throw( WRAPPER_OUT_OF_MEMORY );
    CCounter *pTempCounter;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateCounter(  pCounterDesc,  &pTempCounter );
        if( pTempCounter )
        {
            pMultiCounter->SetSubCounter( i, pTempCounter );
            pTempCounter->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiCounter );
            pMultiCounter = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiCounter );
        return rval;
    }

    *ppCounter = pMultiCounter;
    if( *ppCounter ) (*ppCounter)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CheckFormatSupport()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CheckFormatSupport(  DXGI_FORMAT Format, UINT * pFormatSupport )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CheckFormatSupport(  Format,  pFormatSupport );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CheckMultisampleQualityLevels()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CheckMultisampleQualityLevels(  DXGI_FORMAT Format, UINT SampleCount, UINT * pNumQualityLevels )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CheckMultisampleQualityLevels(  Format,  SampleCount,  pNumQualityLevels );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CheckCounterInfo()
////////////////////////////////////////////////////////////
void CMultiDevice::CheckCounterInfo(  D3D_COUNTER_INFO * pCounterInfo )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->CheckCounterInfo(  pCounterInfo );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::CheckCounter()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CheckCounter(  const D3D_COUNTER_DESC * pDesc, D3D_COUNTER_TYPE * pType, UINT * pActiveCounters, LPSTR szName, UINT * pNameLength, LPSTR szUnits, UINT * pUnitsLength, LPSTR szDescription, UINT * pDescriptionLength )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CheckCounter(  pDesc,  pType,  pActiveCounters,  szName,  pNameLength,  szUnits,  pUnitsLength,  szDescription,  pDescriptionLength );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::GetCreationFlags()
////////////////////////////////////////////////////////////
UINT CMultiDevice::GetCreationFlags(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubDevice( i )->GetCreationFlags(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::OpenSharedResource()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::OpenSharedResource(  HANDLE hResource, REFIID ReturnedInterface, void ** ppResource )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->OpenSharedResource(  hResource,  ReturnedInterface,  ppResource );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::SetTextFilterSize()
////////////////////////////////////////////////////////////
void CMultiDevice::SetTextFilterSize(  UINT Width, UINT Height )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->SetTextFilterSize(  Width,  Height );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::GetTextFilterSize()
////////////////////////////////////////////////////////////
void CMultiDevice::GetTextFilterSize(  UINT * pWidth, UINT * pHeight )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDevice( i )->GetTextFilterSize(  pWidth,  pHeight );
    }

}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateShaderResourceView1()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateShaderResourceView1(  CResource * pResource, const D3D_SHADER_RESOURCE_VIEW_DESC1 * pDesc, CShaderResourceView1 ** ppSRView )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    assert( pResource == NULL || pResource->GetSubObjectCount() == GetSubObjectCount() );
    CMultiShaderResourceView1 *pMultiShaderResourceView1 = new CMultiShaderResourceView1( GetSubObjectCount() );
    if( !pMultiShaderResourceView1 ) throw( WRAPPER_OUT_OF_MEMORY );
    CShaderResourceView1 *pTempShaderResourceView1;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateShaderResourceView1(  pResource ? pResource->GetSubResource( i ) : NULL,  pDesc,  &pTempShaderResourceView1 );
        if( pTempShaderResourceView1 )
        {
            pMultiShaderResourceView1->SetSubShaderResourceView1( i, pTempShaderResourceView1 );
            pTempShaderResourceView1->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiShaderResourceView1 );
            pMultiShaderResourceView1 = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiShaderResourceView1 );
        return rval;
    }

    *ppSRView = pMultiShaderResourceView1;
    if( *ppSRView ) (*ppSRView)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::CreateBlendState1()
////////////////////////////////////////////////////////////
NRESULT CMultiDevice::CreateBlendState1(  const D3D_BLEND_DESC1 * pBlendStateDesc, CBlendState1 ** ppBlendState )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    CMultiBlendState1 *pMultiBlendState1 = new CMultiBlendState1( GetSubObjectCount() );
    if( !pMultiBlendState1 ) throw( WRAPPER_OUT_OF_MEMORY );
    CBlendState1 *pTempBlendState1;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDevice( i )->CreateBlendState1(  pBlendStateDesc,  &pTempBlendState1 );
        if( pTempBlendState1 )
        {
            pMultiBlendState1->SetSubBlendState1( i, pTempBlendState1 );
            pTempBlendState1->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiBlendState1 );
            pMultiBlendState1 = NULL;
            break;
        }
    }
    if( FAILED( rval ) )
    {
        delete( pMultiBlendState1 );
        return rval;
    }

    *ppBlendState = pMultiBlendState1;
    if( *ppBlendState ) (*ppBlendState)->AddRef();
    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDevice::GetFeatureLevel()
////////////////////////////////////////////////////////////
D3D_FEATURE_LEVEL1 CMultiDevice::GetFeatureLevel(  )
{
    D3D_FEATURE_LEVEL1 rval = (D3D_FEATURE_LEVEL1)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubDevice( i )->GetFeatureLevel(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiMultithread
//
////////////////////////////////////////////////////////////

CMultiMultithread::CMultiMultithread( int nCount ) : 
    m_nMultithreadCount( nCount )
{
    if( nCount > 0 )
    {
        m_pMultithreadPtrArray = new CMultithread*[nCount];
        if( !m_pMultithreadPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pMultithreadPtrArray[i] = NULL;
    }
}


CMultiMultithread::CMultiMultithread( CMultithread **pMultithreadPtrArray, int nCount ) : 
    m_nMultithreadCount( nCount )
{
    if( nCount > 0 )
    {
        m_pMultithreadPtrArray = new CMultithread*[nCount];
        if( !m_pMultithreadPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pMultithreadPtrArray[i] = pMultithreadPtrArray[i];
            m_pMultithreadPtrArray[i]->AddRef();
        }
    }
}


CMultiMultithread::~CMultiMultithread()
{
    if( m_pMultithreadPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pMultithreadPtrArray[i] ) m_pMultithreadPtrArray[i]->CMultithread::Release();
        delete[]( m_pMultithreadPtrArray );
    }
}


void CMultiMultithread::SetSubMultithread( int index, CMultithread *pMultithread )
{
    if( pMultithread ) pMultithread->AddRef();
    if( m_pMultithreadPtrArray[index] )
        m_pMultithreadPtrArray[index]->Release();
    m_pMultithreadPtrArray[index] = pMultithread;
}




////////////////////////////////////////////////////////////
// CMultiMultithread::Enter()
////////////////////////////////////////////////////////////
void CMultiMultithread::Enter(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubMultithread( i )->Enter(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiMultithread::Leave()
////////////////////////////////////////////////////////////
void CMultiMultithread::Leave(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubMultithread( i )->Leave(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiMultithread::SetMultithreadProtected()
////////////////////////////////////////////////////////////
BOOL CMultiMultithread::SetMultithreadProtected(  BOOL bMTProtect )
{
    BOOL rval = (BOOL)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubMultithread( i )->SetMultithreadProtected(  bMTProtect );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiMultithread::GetMultithreadProtected()
////////////////////////////////////////////////////////////
BOOL CMultiMultithread::GetMultithreadProtected(  )
{
    BOOL rval = (BOOL)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubMultithread( i )->GetMultithreadProtected(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiDebug
//
////////////////////////////////////////////////////////////

CMultiDebug::CMultiDebug( int nCount ) : 
    m_nDebugCount( nCount )
{
    if( nCount > 0 )
    {
        m_pDebugPtrArray = new CDebug*[nCount];
        if( !m_pDebugPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pDebugPtrArray[i] = NULL;
    }
}


CMultiDebug::CMultiDebug( CDebug **pDebugPtrArray, int nCount ) : 
    m_nDebugCount( nCount )
{
    if( nCount > 0 )
    {
        m_pDebugPtrArray = new CDebug*[nCount];
        if( !m_pDebugPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pDebugPtrArray[i] = pDebugPtrArray[i];
            m_pDebugPtrArray[i]->AddRef();
        }
    }
}


CMultiDebug::~CMultiDebug()
{
    if( m_pDebugPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pDebugPtrArray[i] ) m_pDebugPtrArray[i]->CDebug::Release();
        delete[]( m_pDebugPtrArray );
    }
}


void CMultiDebug::SetSubDebug( int index, CDebug *pDebug )
{
    if( pDebug ) pDebug->AddRef();
    if( m_pDebugPtrArray[index] )
        m_pDebugPtrArray[index]->Release();
    m_pDebugPtrArray[index] = pDebug;
}




////////////////////////////////////////////////////////////
// CMultiDebug::SetFeatureMask()
////////////////////////////////////////////////////////////
NRESULT CMultiDebug::SetFeatureMask(  UINT Mask )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDebug( i )->SetFeatureMask(  Mask );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDebug::GetFeatureMask()
////////////////////////////////////////////////////////////
UINT CMultiDebug::GetFeatureMask(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubDebug( i )->GetFeatureMask(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDebug::SetPresentPerRenderOpDelay()
////////////////////////////////////////////////////////////
NRESULT CMultiDebug::SetPresentPerRenderOpDelay(  UINT Milliseconds )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDebug( i )->SetPresentPerRenderOpDelay(  Milliseconds );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDebug::GetPresentPerRenderOpDelay()
////////////////////////////////////////////////////////////
UINT CMultiDebug::GetPresentPerRenderOpDelay(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubDebug( i )->GetPresentPerRenderOpDelay(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDebug::SetSwapChain()
////////////////////////////////////////////////////////////
NRESULT CMultiDebug::SetSwapChain(  IDXGISwapChain * pSwapChain )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDebug( i )->SetSwapChain(  pSwapChain );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDebug::GetSwapChain()
////////////////////////////////////////////////////////////
NRESULT CMultiDebug::GetSwapChain(  IDXGISwapChain ** ppSwapChain )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDebug( i )->GetSwapChain(  ppSwapChain );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiDebug::Validate()
////////////////////////////////////////////////////////////
NRESULT CMultiDebug::Validate(  )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDebug( i )->Validate(  );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiSwitchToRef
//
////////////////////////////////////////////////////////////

CMultiSwitchToRef::CMultiSwitchToRef( int nCount ) : 
    m_nSwitchToRefCount( nCount )
{
    if( nCount > 0 )
    {
        m_pSwitchToRefPtrArray = new CSwitchToRef*[nCount];
        if( !m_pSwitchToRefPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pSwitchToRefPtrArray[i] = NULL;
    }
}


CMultiSwitchToRef::CMultiSwitchToRef( CSwitchToRef **pSwitchToRefPtrArray, int nCount ) : 
    m_nSwitchToRefCount( nCount )
{
    if( nCount > 0 )
    {
        m_pSwitchToRefPtrArray = new CSwitchToRef*[nCount];
        if( !m_pSwitchToRefPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pSwitchToRefPtrArray[i] = pSwitchToRefPtrArray[i];
            m_pSwitchToRefPtrArray[i]->AddRef();
        }
    }
}


CMultiSwitchToRef::~CMultiSwitchToRef()
{
    if( m_pSwitchToRefPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pSwitchToRefPtrArray[i] ) m_pSwitchToRefPtrArray[i]->CSwitchToRef::Release();
        delete[]( m_pSwitchToRefPtrArray );
    }
}


void CMultiSwitchToRef::SetSubSwitchToRef( int index, CSwitchToRef *pSwitchToRef )
{
    if( pSwitchToRef ) pSwitchToRef->AddRef();
    if( m_pSwitchToRefPtrArray[index] )
        m_pSwitchToRefPtrArray[index]->Release();
    m_pSwitchToRefPtrArray[index] = pSwitchToRef;
}




////////////////////////////////////////////////////////////
// CMultiSwitchToRef::SetUseRef()
////////////////////////////////////////////////////////////
BOOL CMultiSwitchToRef::SetUseRef(  BOOL UseRef )
{
    BOOL rval = (BOOL)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubSwitchToRef( i )->SetUseRef(  UseRef );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiSwitchToRef::GetUseRef()
////////////////////////////////////////////////////////////
BOOL CMultiSwitchToRef::GetUseRef(  )
{
    BOOL rval = (BOOL)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubSwitchToRef( i )->GetUseRef(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiInfoQueue
//
////////////////////////////////////////////////////////////

CMultiInfoQueue::CMultiInfoQueue( int nCount ) : 
    m_nInfoQueueCount( nCount )
{
    if( nCount > 0 )
    {
        m_pInfoQueuePtrArray = new CInfoQueue*[nCount];
        if( !m_pInfoQueuePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pInfoQueuePtrArray[i] = NULL;
    }
}


CMultiInfoQueue::CMultiInfoQueue( CInfoQueue **pInfoQueuePtrArray, int nCount ) : 
    m_nInfoQueueCount( nCount )
{
    if( nCount > 0 )
    {
        m_pInfoQueuePtrArray = new CInfoQueue*[nCount];
        if( !m_pInfoQueuePtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pInfoQueuePtrArray[i] = pInfoQueuePtrArray[i];
            m_pInfoQueuePtrArray[i]->AddRef();
        }
    }
}


CMultiInfoQueue::~CMultiInfoQueue()
{
    if( m_pInfoQueuePtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pInfoQueuePtrArray[i] ) m_pInfoQueuePtrArray[i]->CInfoQueue::Release();
        delete[]( m_pInfoQueuePtrArray );
    }
}


void CMultiInfoQueue::SetSubInfoQueue( int index, CInfoQueue *pInfoQueue )
{
    if( pInfoQueue ) pInfoQueue->AddRef();
    if( m_pInfoQueuePtrArray[index] )
        m_pInfoQueuePtrArray[index]->Release();
    m_pInfoQueuePtrArray[index] = pInfoQueue;
}




////////////////////////////////////////////////////////////
// CMultiInfoQueue::SetMessageCountLimit()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::SetMessageCountLimit(  UINT64 MessageCountLimit )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->SetMessageCountLimit(  MessageCountLimit );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::ClearStoredMessages()
////////////////////////////////////////////////////////////
void CMultiInfoQueue::ClearStoredMessages(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubInfoQueue( i )->ClearStoredMessages(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetMessage()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::GetMessage(  UINT64 MessageIndex, D3D_MESSAGE * pMessage, SIZE_T * pMessageByteLength )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->GetMessage(  MessageIndex,  pMessage,  pMessageByteLength );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetNumMessagesAllowedByStorageFilter()
////////////////////////////////////////////////////////////
UINT64 CMultiInfoQueue::GetNumMessagesAllowedByStorageFilter(  )
{
    UINT64 rval = (UINT64)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetNumMessagesAllowedByStorageFilter(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetNumMessagesDeniedByStorageFilter()
////////////////////////////////////////////////////////////
UINT64 CMultiInfoQueue::GetNumMessagesDeniedByStorageFilter(  )
{
    UINT64 rval = (UINT64)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetNumMessagesDeniedByStorageFilter(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetNumStoredMessages()
////////////////////////////////////////////////////////////
UINT64 CMultiInfoQueue::GetNumStoredMessages(  )
{
    UINT64 rval = (UINT64)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetNumStoredMessages(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetNumStoredMessagesAllowedByRetrievalFilter()
////////////////////////////////////////////////////////////
UINT64 CMultiInfoQueue::GetNumStoredMessagesAllowedByRetrievalFilter(  )
{
    UINT64 rval = (UINT64)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetNumStoredMessagesAllowedByRetrievalFilter(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetNumMessagesDiscardedByMessageCountLimit()
////////////////////////////////////////////////////////////
UINT64 CMultiInfoQueue::GetNumMessagesDiscardedByMessageCountLimit(  )
{
    UINT64 rval = (UINT64)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetNumMessagesDiscardedByMessageCountLimit(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetMessageCountLimit()
////////////////////////////////////////////////////////////
UINT64 CMultiInfoQueue::GetMessageCountLimit(  )
{
    UINT64 rval = (UINT64)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetMessageCountLimit(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::AddStorageFilterEntries()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::AddStorageFilterEntries(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->AddStorageFilterEntries(  pFilter );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetStorageFilter()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::GetStorageFilter(  D3D_INFO_QUEUE_FILTER * pFilter, SIZE_T * pFilterByteLength )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->GetStorageFilter(  pFilter,  pFilterByteLength );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::ClearStorageFilter()
////////////////////////////////////////////////////////////
void CMultiInfoQueue::ClearStorageFilter(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubInfoQueue( i )->ClearStorageFilter(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::PushEmptyStorageFilter()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::PushEmptyStorageFilter(  )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->PushEmptyStorageFilter(  );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::PushCopyOfStorageFilter()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::PushCopyOfStorageFilter(  )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->PushCopyOfStorageFilter(  );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::PushStorageFilter()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::PushStorageFilter(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->PushStorageFilter(  pFilter );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::PopStorageFilter()
////////////////////////////////////////////////////////////
void CMultiInfoQueue::PopStorageFilter(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubInfoQueue( i )->PopStorageFilter(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetStorageFilterStackSize()
////////////////////////////////////////////////////////////
UINT CMultiInfoQueue::GetStorageFilterStackSize(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetStorageFilterStackSize(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::AddRetrievalFilterEntries()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::AddRetrievalFilterEntries(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->AddRetrievalFilterEntries(  pFilter );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::GetRetrievalFilter(  D3D_INFO_QUEUE_FILTER * pFilter, SIZE_T * pFilterByteLength )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->GetRetrievalFilter(  pFilter,  pFilterByteLength );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::ClearRetrievalFilter()
////////////////////////////////////////////////////////////
void CMultiInfoQueue::ClearRetrievalFilter(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubInfoQueue( i )->ClearRetrievalFilter(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::PushEmptyRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::PushEmptyRetrievalFilter(  )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->PushEmptyRetrievalFilter(  );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::PushCopyOfRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::PushCopyOfRetrievalFilter(  )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->PushCopyOfRetrievalFilter(  );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::PushRetrievalFilter()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::PushRetrievalFilter(  D3D_INFO_QUEUE_FILTER * pFilter )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->PushRetrievalFilter(  pFilter );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::PopRetrievalFilter()
////////////////////////////////////////////////////////////
void CMultiInfoQueue::PopRetrievalFilter(  )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubInfoQueue( i )->PopRetrievalFilter(  );
    }

}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetRetrievalFilterStackSize()
////////////////////////////////////////////////////////////
UINT CMultiInfoQueue::GetRetrievalFilterStackSize(  )
{
    UINT rval = (UINT)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetRetrievalFilterStackSize(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::AddMessage()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::AddMessage(  D3D_MESSAGE_CATEGORY Category, D3D_MESSAGE_SEVERITY Severity, D3D_MESSAGE_ID ID, LPCSTR pDescription )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->AddMessage(  Category,  Severity,  ID,  pDescription );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::AddApplicationMessage()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::AddApplicationMessage(  D3D_MESSAGE_SEVERITY Severity, LPCSTR pDescription )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->AddApplicationMessage(  Severity,  pDescription );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::SetBreakOnCategory()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::SetBreakOnCategory(  D3D_MESSAGE_CATEGORY Category, BOOL bEnable )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->SetBreakOnCategory(  Category,  bEnable );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::SetBreakOnSeverity()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::SetBreakOnSeverity(  D3D_MESSAGE_SEVERITY Severity, BOOL bEnable )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->SetBreakOnSeverity(  Severity,  bEnable );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::SetBreakOnID()
////////////////////////////////////////////////////////////
NRESULT CMultiInfoQueue::SetBreakOnID(  D3D_MESSAGE_ID ID, BOOL bEnable )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubInfoQueue( i )->SetBreakOnID(  ID,  bEnable );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetBreakOnCategory()
////////////////////////////////////////////////////////////
BOOL CMultiInfoQueue::GetBreakOnCategory(  D3D_MESSAGE_CATEGORY Category )
{
    BOOL rval = (BOOL)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetBreakOnCategory(  Category );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetBreakOnSeverity()
////////////////////////////////////////////////////////////
BOOL CMultiInfoQueue::GetBreakOnSeverity(  D3D_MESSAGE_SEVERITY Severity )
{
    BOOL rval = (BOOL)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetBreakOnSeverity(  Severity );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetBreakOnID()
////////////////////////////////////////////////////////////
BOOL CMultiInfoQueue::GetBreakOnID(  D3D_MESSAGE_ID ID )
{
    BOOL rval = (BOOL)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetBreakOnID(  ID );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::SetMuteDebugOutput()
////////////////////////////////////////////////////////////
void CMultiInfoQueue::SetMuteDebugOutput(  BOOL bMute )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubInfoQueue( i )->SetMuteDebugOutput(  bMute );
    }

}


////////////////////////////////////////////////////////////
// CMultiInfoQueue::GetMuteDebugOutput()
////////////////////////////////////////////////////////////
BOOL CMultiInfoQueue::GetMuteDebugOutput(  )
{
    BOOL rval = (BOOL)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubInfoQueue( i )->GetMuteDebugOutput(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiBlob
//
////////////////////////////////////////////////////////////

CMultiBlob::CMultiBlob( int nCount ) : 
    m_nBlobCount( nCount )
{
    if( nCount > 0 )
    {
        m_pBlobPtrArray = new CBlob*[nCount];
        if( !m_pBlobPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pBlobPtrArray[i] = NULL;
    }
}


CMultiBlob::CMultiBlob( CBlob **pBlobPtrArray, int nCount ) : 
    m_nBlobCount( nCount )
{
    if( nCount > 0 )
    {
        m_pBlobPtrArray = new CBlob*[nCount];
        if( !m_pBlobPtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pBlobPtrArray[i] = pBlobPtrArray[i];
            m_pBlobPtrArray[i]->AddRef();
        }
    }
}


CMultiBlob::~CMultiBlob()
{
    if( m_pBlobPtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pBlobPtrArray[i] ) m_pBlobPtrArray[i]->CBlob::Release();
        delete[]( m_pBlobPtrArray );
    }
}


void CMultiBlob::SetSubBlob( int index, CBlob *pBlob )
{
    if( pBlob ) pBlob->AddRef();
    if( m_pBlobPtrArray[index] )
        m_pBlobPtrArray[index]->Release();
    m_pBlobPtrArray[index] = pBlob;
}




////////////////////////////////////////////////////////////
// CMultiBlob::QueryInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiBlob::QueryInterface(  REFIID iid, LPVOID * ppv )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubBlob( i )->QueryInterface(  iid,  ppv );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiBlob::GetBufferPointer()
////////////////////////////////////////////////////////////
LPVOID CMultiBlob::GetBufferPointer(  )
{
    LPVOID rval = (LPVOID)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubBlob( i )->GetBufferPointer(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiBlob::GetBufferSize()
////////////////////////////////////////////////////////////
SIZE_T CMultiBlob::GetBufferSize(  )
{
    SIZE_T rval = (SIZE_T)0;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval = GetSubBlob( i )->GetBufferSize(  );
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiBlendState1
//
////////////////////////////////////////////////////////////

CMultiBlendState1::CMultiBlendState1( int nCount ) : 
    CMultiBlendState( nCount ),
    m_nBlendState1Count( nCount )
{
    if( nCount > 0 )
    {
        m_pBlendState1PtrArray = new CBlendState1*[nCount];
        if( !m_pBlendState1PtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pBlendState1PtrArray[i] = NULL;
    }
}


CMultiBlendState1::CMultiBlendState1( CBlendState1 **pBlendState1PtrArray, int nCount ) : 
    CMultiBlendState( nCount ),
    m_nBlendState1Count( nCount )
{
    if( nCount > 0 )
    {
        m_pBlendState1PtrArray = new CBlendState1*[nCount];
        if( !m_pBlendState1PtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pBlendState1PtrArray[i] = pBlendState1PtrArray[i];
            m_pBlendState1PtrArray[i]->AddRef();
        }
    }
}


CMultiBlendState1::~CMultiBlendState1()
{
    if( m_pBlendState1PtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pBlendState1PtrArray[i] ) m_pBlendState1PtrArray[i]->CBlendState1::Release();
        delete[]( m_pBlendState1PtrArray );
    }
}


void CMultiBlendState1::SetSubBlendState1( int index, CBlendState1 *pBlendState1 )
{
    if( pBlendState1 ) pBlendState1->AddRef();
    if( m_pBlendState1PtrArray[index] )
        m_pBlendState1PtrArray[index]->Release();
    m_pBlendState1PtrArray[index] = pBlendState1;
}




////////////////////////////////////////////////////////////
// CMultiBlendState1::GetDesc1()
////////////////////////////////////////////////////////////
void CMultiBlendState1::GetDesc1(  D3D_BLEND_DESC1 * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubBlendState1( i )->GetDesc1(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiBlendState1::GetDesc()
////////////////////////////////////////////////////////////
void CMultiBlendState1::GetDesc(  D3D_BLEND_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubBlendState( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiBlendState1::GetDevice()
////////////////////////////////////////////////////////////
void CMultiBlendState1::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiBlendState1::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiBlendState1::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiBlendState1::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiBlendState1::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiBlendState1::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiBlendState1::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
//
// CMultiShaderResourceView1
//
////////////////////////////////////////////////////////////

CMultiShaderResourceView1::CMultiShaderResourceView1( int nCount ) : 
    CMultiShaderResourceView( nCount ),
    m_nShaderResourceView1Count( nCount )
{
    if( nCount > 0 )
    {
        m_pShaderResourceView1PtrArray = new CShaderResourceView1*[nCount];
        if( !m_pShaderResourceView1PtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
            m_pShaderResourceView1PtrArray[i] = NULL;
    }
}


CMultiShaderResourceView1::CMultiShaderResourceView1( CShaderResourceView1 **pShaderResourceView1PtrArray, int nCount ) : 
    CMultiShaderResourceView( nCount ),
    m_nShaderResourceView1Count( nCount )
{
    if( nCount > 0 )
    {
        m_pShaderResourceView1PtrArray = new CShaderResourceView1*[nCount];
        if( !m_pShaderResourceView1PtrArray ) throw( WRAPPER_OUT_OF_MEMORY );
        for( int i = nCount - 1; i != -1; --i )
        {
            m_pShaderResourceView1PtrArray[i] = pShaderResourceView1PtrArray[i];
            m_pShaderResourceView1PtrArray[i]->AddRef();
        }
    }
}


CMultiShaderResourceView1::~CMultiShaderResourceView1()
{
    if( m_pShaderResourceView1PtrArray )
    {
        for( int i = GetSubObjectCount() - 1; i != -1; --i )
            if( m_pShaderResourceView1PtrArray[i] ) m_pShaderResourceView1PtrArray[i]->CShaderResourceView1::Release();
        delete[]( m_pShaderResourceView1PtrArray );
    }
}


void CMultiShaderResourceView1::SetSubShaderResourceView1( int index, CShaderResourceView1 *pShaderResourceView1 )
{
    if( pShaderResourceView1 ) pShaderResourceView1->AddRef();
    if( m_pShaderResourceView1PtrArray[index] )
        m_pShaderResourceView1PtrArray[index]->Release();
    m_pShaderResourceView1PtrArray[index] = pShaderResourceView1;
}




////////////////////////////////////////////////////////////
// CMultiShaderResourceView1::GetDesc1()
////////////////////////////////////////////////////////////
void CMultiShaderResourceView1::GetDesc1(  D3D_SHADER_RESOURCE_VIEW_DESC1 * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubShaderResourceView1( i )->GetDesc1(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiShaderResourceView1::GetDesc()
////////////////////////////////////////////////////////////
void CMultiShaderResourceView1::GetDesc(  D3D_SHADER_RESOURCE_VIEW_DESC * pDesc )
{
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubShaderResourceView( i )->GetDesc(  pDesc );
    }

}


////////////////////////////////////////////////////////////
// CMultiShaderResourceView1::GetResource()
////////////////////////////////////////////////////////////
void CMultiShaderResourceView1::GetResource(  CResource ** ppResource )
{
    CMultiResourceGeneric *pMultiResource = new CMultiResourceGeneric( GetSubObjectCount() );
    if( !pMultiResource ) throw( WRAPPER_OUT_OF_MEMORY );
    CResource *pTempResource;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubView( i )->GetResource(  &pTempResource );
        if( pTempResource )
        {
            pMultiResource->SetSubResource( i, pTempResource );
            pTempResource->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiResource );
            pMultiResource = NULL;
            break;
        }
    }

    *ppResource = pMultiResource;
    if( *ppResource ) (*ppResource)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiShaderResourceView1::GetDevice()
////////////////////////////////////////////////////////////
void CMultiShaderResourceView1::GetDevice(  CDevice ** ppDevice )
{
    CMultiDevice *pMultiDevice = new CMultiDevice( GetSubObjectCount() );
    if( !pMultiDevice ) throw( WRAPPER_OUT_OF_MEMORY );
    CDevice *pTempDevice;
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        GetSubDeviceChild( i )->GetDevice(  &pTempDevice );
        if( pTempDevice )
        {
            pMultiDevice->SetSubDevice( i, pTempDevice );
            pTempDevice->Release();
        }
        else
        {
            // Assume all interfaces will return NULL
            delete( pMultiDevice );
            pMultiDevice = NULL;
            break;
        }
    }

    *ppDevice = pMultiDevice;
    if( *ppDevice ) (*ppDevice)->AddRef();
}


////////////////////////////////////////////////////////////
// CMultiShaderResourceView1::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiShaderResourceView1::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->GetPrivateData(  guid,  pDataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiShaderResourceView1::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT CMultiShaderResourceView1::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateData(  guid,  DataSize,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}


////////////////////////////////////////////////////////////
// CMultiShaderResourceView1::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT CMultiShaderResourceView1::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    NRESULT rval( S_OK, GetSubObjectCount() );
    for( int i = GetSubObjectCount() - 1; i >= 0; --i )
    {
        rval[i] = GetSubDeviceChild( i )->SetPrivateDataInterface(  guid,  pData );
    }
    if( FAILED( rval ) )
    {
        return rval;
    }

    return rval;
}
