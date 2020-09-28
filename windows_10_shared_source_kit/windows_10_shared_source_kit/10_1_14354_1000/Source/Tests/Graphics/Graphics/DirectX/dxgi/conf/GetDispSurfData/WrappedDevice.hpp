#pragma once

#include <d3d11_1.h>
#include <d3d11.h>
#include <d3d10_1.h>
#include <d3d10.h>
#include <D3dx11.h>
#include <D3dx11Effect.h>
#include <windows.h>
#include <dxgi.h>
#include <d3dx10.h>
#include <StrSafe.h>
#include "Common.hpp"
#include "fwtypes.h"

#include "d3dconvert.h"

#include <map>

typedef std::map< ID3D10Effect**, ID3DX11Effect* > EffectMapType;
typedef std::map< ID3D10EffectTechnique**, ID3DX11EffectTechnique*  > EffectTechniqueMapType;

class D3DWrappedDevice
{
public:
	D3DWrappedDevice( ID3D10Device* pD3D10Device )
		: m_pD3D11Device1( NULL )
		, m_pD3D11Device( NULL )
		, m_pD3D10Device( NULL )
		, m_emulatedInterface( __uuidof(ID3D10Device))
		, m_iCurrentDCSIndex( 0 )
	{
		if( pD3D10Device )
			m_pD3D10Device = pD3D10Device;

		memset( &m_pDeviceContextState, 0 , sizeof(m_pDeviceContextState) ); 
		m_effectMap.clear();
		m_effectTechniqueMap.clear();
	}

	~D3DWrappedDevice()
	{
		SAFE_RELEASE( m_pD3D11Device1 );
		SAFE_RELEASE( m_pD3D11Device );
		for( int i =0 ; i< sizeof(m_pDeviceContextState)/sizeof(m_pDeviceContextState[0]); ++i )
			SAFE_RELEASE( m_pDeviceContextState[i] );
		for( EffectMapType::iterator it = m_effectMap.begin(); it != m_effectMap.end(); it++ )
			SAFE_RELEASE( it->second );
	}

	HRESULT Initialize( REFIID EmulatedInterface )
	{
		m_emulatedInterface = EmulatedInterface;
		HRESULT hr  = E_FAIL;

		SAFE_RELEASE( m_pD3D11Device1 );
		SAFE_RELEASE( m_pD3D11Device );
		for( int i =0 ; i< sizeof(m_pDeviceContextState)/sizeof(m_pDeviceContextState[0]); ++i )
			SAFE_RELEASE( m_pDeviceContextState[i] );
	
		char szMsg[c_nMSG_LEN] = {0};    

		if( m_pD3D10Device )
		{
			hr = m_pD3D10Device->QueryInterface( __uuidof(ID3D11Device), (void **)&m_pD3D11Device);

			if( FAILED( hr ) )
			{
				StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWappedDevice::Initialize: QI for ID3D11Device failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
				CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
				return hr;
			}
			else if(!m_pD3D11Device )
			{
				StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWappedDevice::Initialize: QI for ID3D11Device returned NULL\n", __FUNCTION__, __FILE__, __LINE__ );
				CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
				return hr;
			}

			if( FAILED( m_pD3D11Device->QueryInterface( __uuidof(ID3D11Device1), (void **) &m_pD3D11Device1 )) )
			{
				StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWappedDevice::Initialize: QI for ID3D11Device1 failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
				CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
				return hr;
			}

			D3D_FEATURE_LEVEL featureLevels[1];
			featureLevels[0] = m_pD3D11Device1->GetFeatureLevel();
			UINT Flags = m_pD3D11Device1->GetCreationFlags();

			hr = m_pD3D11Device1->CreateDeviceContextState( Flags & ~(D3D11_CREATE_DEVICE_DEBUG), featureLevels, 1, D3D11_SDK_VERSION, EmulatedInterface, &m_FeatureLevelCreated, &(m_pDeviceContextState[1]) );
			if( FAILED( hr ) )
			{
				StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWappedDevice::Initialize: CreateDeviceContextState failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
				CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
			}
			else
			{
				ID3D11DeviceContext1* immediateContext1;
				m_pD3D11Device1->GetImmediateContext1( &immediateContext1 );
				immediateContext1->SwapDeviceContextState( (m_pDeviceContextState[1]), &(m_pDeviceContextState[0]) );
				SAFE_RELEASE( immediateContext1 );
				m_iCurrentDCSIndex = 1;
			}

			// Create an D3D11 DCS for swapping variataions 10->11 & 11->11
			hr = m_pD3D11Device1->CreateDeviceContextState( Flags & ~(D3D11_CREATE_DEVICE_DEBUG), featureLevels, 1, D3D11_SDK_VERSION, __uuidof( ID3D11Device ), &m_FeatureLevelCreated, &(m_pDeviceContextState[2]) );
			if( FAILED( hr ) )
			{
				StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWappedDevice::Initialize: CreateDeviceContextState failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
				CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
			}
		}

		return hr;
	}

	void Uninitialize()
	{
		if( m_pDeviceContextState[0] )
		{
			ID3D11DeviceContext1* immediateContext1;
			m_pD3D11Device1->GetImmediateContext1( &immediateContext1 );
			immediateContext1->SwapDeviceContextState( (m_pDeviceContextState[0]), NULL );
			for( int i = 1 ; i< sizeof(m_pDeviceContextState)/sizeof(m_pDeviceContextState[0]); ++i )
				SAFE_RELEASE( m_pDeviceContextState[i] );
			SAFE_RELEASE( immediateContext1 );
		}
	}

	ID3D10Device* GetD3D10DevicePtr() { return m_pD3D10Device; }

	HRESULT SwapToOriginalDeviceContextState()
	{
		if( m_iCurrentDCSIndex != 0 && m_pDeviceContextState[0] )
		{
			ID3D11DeviceContext1* immediateContext1;
			m_pD3D11Device1->GetImmediateContext1( &immediateContext1 );
			SAFE_RELEASE( m_pDeviceContextState[m_iCurrentDCSIndex] );
			immediateContext1->SwapDeviceContextState( (m_pDeviceContextState[0]), &(m_pDeviceContextState[m_iCurrentDCSIndex]) );			
			SAFE_RELEASE( immediateContext1 );
			m_iCurrentDCSIndex = 0;
			return S_OK;
		}
		return E_FAIL;
	}

	HRESULT SwapToEmulatedDeviceContextState()
	{
		if( m_iCurrentDCSIndex != 1 && m_pDeviceContextState[1] )
		{
			ID3D11DeviceContext1* immediateContext1;
			m_pD3D11Device1->GetImmediateContext1( &immediateContext1 );
			SAFE_RELEASE( m_pDeviceContextState[m_iCurrentDCSIndex] );
			immediateContext1->SwapDeviceContextState( (m_pDeviceContextState[1]), &(m_pDeviceContextState[m_iCurrentDCSIndex]) );			
			SAFE_RELEASE( immediateContext1 );
			m_iCurrentDCSIndex = 1;
			return S_OK;
		}
		return E_FAIL;
	}

	HRESULT SwapToDeviceContextState_OtherMode()
	{
		int dstDcsIndex = -1;
		if( m_iCurrentDCSIndex == 1 )
		{
			if( m_emulatedInterface == __uuidof( ID3D11Device ))
				dstDcsIndex = 0;
			else if ( m_emulatedInterface == __uuidof( ID3D10Device ))
				dstDcsIndex = 2;	
		}

		if( dstDcsIndex >=0 && dstDcsIndex < sizeof(m_pDeviceContextState)/sizeof(m_pDeviceContextState[0]) )
		{
			if( m_pDeviceContextState[dstDcsIndex] )
			{
				ID3D11DeviceContext1* immediateContext1;
				m_pD3D11Device1->GetImmediateContext1( &immediateContext1 );
				SAFE_RELEASE( m_pDeviceContextState[m_iCurrentDCSIndex] );
				immediateContext1->SwapDeviceContextState( (m_pDeviceContextState[dstDcsIndex]), &(m_pDeviceContextState[m_iCurrentDCSIndex]) );			
				SAFE_RELEASE( immediateContext1 );
				m_iCurrentDCSIndex = dstDcsIndex;
				return S_OK;
			}
		}
		return E_FAIL;
	}

	void OMSetRenderTargets(
	  __in  UINT NumViews,
	  __in  ID3D10RenderTargetView *const *ppRenderTargetViews,
	  __in  ID3D10DepthStencilView *pDepthStencilView
	  )
	{
		char szMsg[c_nMSG_LEN] = {0};    

		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
			return m_pD3D10Device->OMSetRenderTargets( NumViews, ppRenderTargetViews, pDepthStencilView );
		else
		{
			ID3D11RenderTargetView* pD3D11RenderTargetViews[ D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT ] = {0};
			ID3D11DepthStencilView* pD3D11DepthStencilView = NULL;
			HRESULT hr = S_OK;
			for (UINT i=0; i< NumViews; ++i)
			{
				assert( (ppRenderTargetViews[i]) != NULL ) ;				
				hr = (ppRenderTargetViews[i])->QueryInterface( __uuidof(ID3D11RenderTargetView), (void **)&(pD3D11RenderTargetViews[i]));
				if( pD3D11RenderTargetViews[i] && SUCCEEDED(hr) )
					(ppRenderTargetViews[i])->Release();
				else
				{
					StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3D11Scene::Initialize: QI for ID3D11RenderTargetView failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
					CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
					return;
				}
			}
			if( pDepthStencilView )
			{
				hr = pDepthStencilView->QueryInterface( __uuidof(ID3D11DepthStencilView), (void **)&pD3D11DepthStencilView );
				if( pD3D11DepthStencilView && SUCCEEDED(hr) )
					pDepthStencilView->Release();
				else
				{
					StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: QI for ID3D11DepthStencilView failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
					CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
					return;
				}
			}
			ID3D11DeviceContext* immediateContext;
			m_pD3D11Device->GetImmediateContext( &immediateContext );
			immediateContext->OMSetRenderTargets( NumViews, pD3D11RenderTargetViews, pD3D11DepthStencilView );
			SAFE_RELEASE( immediateContext );
		}
	}

	void ClearRenderTargetView( 
		__in  ID3D10RenderTargetView *pRenderTargetView,
		__in  const FLOAT ColorRGBA[4]
		)
	{
		char szMsg[c_nMSG_LEN] = {0}; 
		HRESULT hr = S_OK;

		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
			return m_pD3D10Device->ClearRenderTargetView( pRenderTargetView, ColorRGBA );
		else 
		{	
			ID3D11RenderTargetView* pD3D11RenderTargetView = NULL;
			if ( pRenderTargetView )
			{
				if ( 
					SUCCEEDED( 	hr = pRenderTargetView->QueryInterface( __uuidof(ID3D11RenderTargetView), (void **)&pD3D11RenderTargetView ) ) 
					&& pD3D11RenderTargetView 
					)
					pRenderTargetView->Release();
				else
				{
					StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3D11Scene::Initialize: QI for ID3D11RenderTargetView failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
					CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
					return;
				}
			}
			ID3D11DeviceContext* immediateContext;
			m_pD3D11Device->GetImmediateContext( &immediateContext );
			immediateContext->ClearRenderTargetView( pD3D11RenderTargetView, ColorRGBA );
			SAFE_RELEASE( immediateContext );
		}
	}

	void RSSetViewports(
	  __in  UINT NumViews,
	  __in  const D3D10_VIEWPORT *pViewports
	  )
	{
		char szMsg[c_nMSG_LEN] = {0};    

		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
			return m_pD3D10Device->RSSetViewports( NumViews, pViewports );
		else
		{
			D3D11_VIEWPORT D3D11ViewPorts[ D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE ] = {0};
			HRESULT hr = S_OK;
			for (UINT i=0; i< NumViews; ++i)
			{
				hr = D3DConvert::Convert( D3D11ViewPorts[i], pViewports[i]);
				if( FAILED(hr) )
				{
					StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3D11Scene::Initialize: Convert from D3D10_VIEWPORT failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
					CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
					return;
				}
			}
			ID3D11DeviceContext* immediateContext;
			m_pD3D11Device->GetImmediateContext( &immediateContext );
			immediateContext->RSSetViewports( NumViews, D3D11ViewPorts );
			SAFE_RELEASE( immediateContext );
		}
	}

	void IASetInputLayout(
	  __in  ID3D10InputLayout *pInputLayout
	  )
	{
		char szMsg[c_nMSG_LEN] = {0}; 
		HRESULT hr = S_OK;

		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
			return m_pD3D10Device->IASetInputLayout( pInputLayout );
		else
		{
			ID3D11InputLayout* pD3D11InputLayout = NULL;
			if( pInputLayout )
			{
				if ( 
					SUCCEEDED( 	hr = pInputLayout->QueryInterface( __uuidof(ID3D11InputLayout), (void **)&pD3D11InputLayout ) ) 
					&& pD3D11InputLayout 
					)
					pInputLayout->Release();
				else
				{
					StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3D11Scene::Initialize: QI for ID3D11InputLayout failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
					CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
					return;
				}
			}
			ID3D11DeviceContext* immediateContext;
			m_pD3D11Device->GetImmediateContext( &immediateContext );
			immediateContext->IASetInputLayout( pD3D11InputLayout );
			SAFE_RELEASE( immediateContext );
		}
	}

	void IASetVertexBuffers(
		__in  UINT StartSlot,
		__in  UINT NumBuffers,
		__in  ID3D10Buffer *const *ppVertexBuffers,
		__in  const UINT *pStrides,
		__in  const UINT *pOffsets
		)
	{
		char szMsg[c_nMSG_LEN] = {0};    

		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
			return m_pD3D10Device->IASetVertexBuffers( StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets );
		else
		{
			ID3D11Buffer* pD3D11VertexBuffers[ D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT ] = {0};
			HRESULT hr = S_OK;
			for (UINT i=0; i< NumBuffers; ++i)
			{
				assert( (ppVertexBuffers[i]) != NULL );
				hr = (ppVertexBuffers[i])->QueryInterface( __uuidof(ID3D11Buffer), (void **)&(pD3D11VertexBuffers[i]));
				if( pD3D11VertexBuffers[i] && SUCCEEDED(hr) )
					(ppVertexBuffers[i])->Release();
				else
				{
					StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3D11Scene::Initialize: QI for ID3D11Buffer failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
					CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
					return;
				}
			}

			ID3D11DeviceContext* immediateContext;
			m_pD3D11Device->GetImmediateContext( &immediateContext );
			immediateContext->IASetVertexBuffers( StartSlot, NumBuffers, pD3D11VertexBuffers, pStrides, pOffsets );
			SAFE_RELEASE( immediateContext );
		}
	}

	void IASetPrimitiveTopology(
	  __in  D3D10_PRIMITIVE_TOPOLOGY Topology
	  )
	{
		char szMsg[c_nMSG_LEN] = {0};    

		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
			return m_pD3D10Device->IASetPrimitiveTopology( Topology );
		else
		{
			ID3D11DeviceContext* immediateContext;
			m_pD3D11Device->GetImmediateContext( &immediateContext );
			immediateContext->IASetPrimitiveTopology( (D3D_PRIMITIVE_TOPOLOGY )Topology );
			SAFE_RELEASE( immediateContext );
		}
	}

	void Draw(
	  __in  UINT VertexCount,
	  __in  UINT StartVertexLocation
	  )
	{
		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
			return m_pD3D10Device->Draw( VertexCount, StartVertexLocation );
		else
		{
			ID3D11DeviceContext* immediateContext;
			m_pD3D11Device->GetImmediateContext( &immediateContext );
			immediateContext->Draw( VertexCount, StartVertexLocation );
			SAFE_RELEASE( immediateContext );
		}
	}

    HRESULT CopyTextureToTexture(
        __in  ID3D10Texture2D **ppDstTex,
        __in  ID3D10Texture2D *pSrcTex,
        __in  D3DX10_TEXTURE_LOAD_INFO& tlinfo
        )
    {
        HRESULT hr = S_OK;
        char szMsg[c_nMSG_LEN] = {0};    

        if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
            return D3DX10LoadTextureFromTexture( pSrcTex, &tlinfo, *ppDstTex );
        else
        {
            ID3D11Resource *pDstTex11;
            ID3D11Resource *pSrcTex11;

            D3DX11_TEXTURE_LOAD_INFO tlinfo11;
            tlinfo11.DstFirstElement = tlinfo.DstFirstElement;
            tlinfo11.DstFirstMip = tlinfo.DstFirstMip;
            tlinfo11.Filter = tlinfo.Filter;
            tlinfo11.MipFilter = tlinfo.MipFilter;
            tlinfo11.NumElements = tlinfo.NumElements;
            tlinfo11.NumMips = tlinfo.NumMips;
            tlinfo11.pDstBox = reinterpret_cast<D3D11_BOX*>(tlinfo.pDstBox);
            tlinfo11.pSrcBox = reinterpret_cast<D3D11_BOX*>(tlinfo.pSrcBox);
            tlinfo11.SrcFirstElement = tlinfo.SrcFirstElement;
            tlinfo11.SrcFirstMip = tlinfo.SrcFirstMip;

            if( FAILED( (*ppDstTex)->QueryInterface( __uuidof( ID3D11Texture2D ), (void**)&pDstTex11 ) ) )							
            {
                StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWrappedDevice::CopyTextureToTexture: QI for ID3D11Texture2D failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
                CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                return E_FAIL;
            }
            if( FAILED( pSrcTex->QueryInterface( __uuidof( ID3D11Texture2D ), (void**)&pSrcTex11 ) ) )
            {
                SAFE_RELEASE( pDstTex11 );
                StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWrappedDevice::CopyTextureToTexture: QI for ID3D11Texture2D failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
                CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                return E_FAIL;
            }
            ID3D11DeviceContext* immediateContext;
            m_pD3D11Device->GetImmediateContext( &immediateContext );
            D3DX11LoadTextureFromTexture( immediateContext, pSrcTex11, &tlinfo11, pDstTex11 );
            SAFE_RELEASE( (*ppDstTex) );
            if( FAILED( pDstTex11->QueryInterface( __uuidof( ID3D10Texture2D ), (void**)ppDstTex ) )) 
            {
                StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWrappedDevice::CopyTextureToTexture: QI for ID3D10Texture2D failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
                CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
                hr = E_FAIL;
            }
            SAFE_RELEASE( pSrcTex11 );
            SAFE_RELEASE( pDstTex11 );
            SAFE_RELEASE( immediateContext );			
        }
        return S_OK;
    }

	HRESULT CopyResource(
  __in  ID3D10Resource **ppDstResource,
  __in  ID3D10Resource *pSrcResource
)
	{
		HRESULT hr = S_OK;
		char szMsg[c_nMSG_LEN] = {0};    

		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
			m_pD3D10Device->CopyResource( *ppDstResource, pSrcResource );
		else
		{
			ID3D11Resource *pDstResource11;
			ID3D11Resource *pSrcResource11;
			if( FAILED( (*ppDstResource)->QueryInterface( __uuidof( ID3D11Resource ), (void**)&pDstResource11 ) ) )							
			{
				StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWrappedDevice::CopyResource: QI for ID3D11Resource failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
				CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
				return E_FAIL;
			}
			if( FAILED( pSrcResource->QueryInterface( __uuidof( ID3D11Resource ), (void**)&pSrcResource11 ) ) )
			{
				SAFE_RELEASE( pDstResource11 );
				StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWrappedDevice::CopyResource: QI for ID3D11Resource failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
				CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
				return E_FAIL;
			}
			ID3D11DeviceContext* immediateContext;
			m_pD3D11Device->GetImmediateContext( &immediateContext );
			immediateContext->CopyResource( pDstResource11, pSrcResource11 );
			// first release the ID3D10Resource object obtained from QI @ Tests.cpp line 1563
			SAFE_RELEASE( (*ppDstResource) );
			if( FAILED( pDstResource11->QueryInterface( __uuidof( ID3D10Resource ), (void**)ppDstResource ) )) 
			{
				StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWrappedDevice::CopyResource: QI for ID3D10Resource failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
				CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
				hr = E_FAIL;
			}
			SAFE_RELEASE( pSrcResource11 );
			SAFE_RELEASE( pDstResource11 );
			SAFE_RELEASE( immediateContext );			
		}
			return S_OK;
	}

	D3D_FEATURE_LEVEL GetFeatureLevel() { return m_FeatureLevelCreated; }

	HRESULT CreateEffectFromFile(  __in   LPCTSTR pFileName,
		__in   const D3D10_SHADER_MACRO *pDefines,
		__in   ID3D10Include *pInclude,
		__in   LPCSTR pProfile,
		__in   UINT HLSLFlags,
		__in   UINT FXFlags,
		__in   ID3D10Device *pDevice,
		__in   ID3D10EffectPool *pEffectPool,
		__in   ID3DX10ThreadPump *pPump,
		__out  ID3D10Effect **ppEffect,
		__out  ID3D10Blob **ppErrors,
		__out  HRESULT *pHResult
		)
	{
		HRESULT hr;
		char szMsg[c_nMSG_LEN] = {0}; 

		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
		{
			hr = D3DX10CreateEffectFromFile( pFileName,
				pDefines,
				pInclude,
				pProfile,
				HLSLFlags,
				FXFlags,
				pDevice,
				pEffectPool,
				pPump,
				ppEffect,
				ppErrors,
				pHResult
				);	
		}
		else
		{
			ID3D10Blob *pShaderByteCode = NULL;
			ID3D10Blob *pErrorBlob = NULL;
			hr = D3DX11CompileFromFile( pFileName,
				pDefines,
				pInclude,
				NULL,
				"fx_5_0",
				HLSLFlags,
				FXFlags,
				NULL,
				&pShaderByteCode,
				&pErrorBlob,
				pHResult
				);	

			if( FAILED( hr ) )
			{
				if( pErrorBlob )
				{
					StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d. Compile failed with errors. D3DX11CompileFromFile Error Buffer: %s", __FUNCTION__, __FILE__, __LINE__, (LPCSTR) pErrorBlob->GetBufferPointer() );
					CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
				}

				StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DX11CompileFromFile failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
				CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
			}
			else
			{
				if( pErrorBlob )
				{
					StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d. Compile succeeded with warnings. D3DX11CompileFromFile Error Buffer: %s", __FUNCTION__, __FILE__, __LINE__, (LPCSTR) pErrorBlob->GetBufferPointer() );
					CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
				}

				ID3DX11Effect *pEffect11;
				hr = D3DX11CreateEffectFromMemory( pShaderByteCode->GetBufferPointer(),
					pShaderByteCode->GetBufferSize(),
					FXFlags,
					m_pD3D11Device,
					&pEffect11
					);
				if( FAILED( hr ) )
				{
					StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DX11CreateEffectFromMemory failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
					CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
				}
				else
				{
					EffectMapType::iterator it = m_effectMap.find( ppEffect );
					if( it != m_effectMap.end() )
					{
						SAFE_RELEASE( it->second );
						it->second= pEffect11;
					}
					else
					{
						m_effectMap.insert( EffectMapType::value_type(ppEffect, pEffect11) );
					}
				}
			}
			SAFE_RELEASE( pErrorBlob );
			SAFE_RELEASE( pShaderByteCode );
		}
		return hr;
	}

	void GetTechniqueByName(
		__in  LPCSTR Name, 
		__in  ID3D10Effect **ppEffect10,
		__out ID3D10EffectTechnique** ppTechnique10
		)
	{
		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
		{
			*ppTechnique10 = (*ppEffect10)->GetTechniqueByName( Name );			
		}
		else
		{
			EffectMapType::iterator it_effect = m_effectMap.find( ppEffect10 );
			EffectTechniqueMapType::iterator it_technique = m_effectTechniqueMap.find( ppTechnique10 );
			if( it_effect != m_effectMap.end() && it_technique == m_effectTechniqueMap.end() )
			{
				if( it_effect->second )
				{
					ID3DX11EffectTechnique *pTemp = (it_effect->second)->GetTechniqueByName( Name );
					m_effectTechniqueMap.insert( EffectTechniqueMapType::value_type( ppTechnique10, pTemp ) );
				}
			}
		}
	}

	HRESULT Technique_GetDesc(
		__in  ID3D10EffectTechnique **ppTechnique10,
		__out D3D10_TECHNIQUE_DESC* pEffectDesc10
		)
	{
		HRESULT hr = E_FAIL;
		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
		{
			hr = (*ppTechnique10)->GetDesc( pEffectDesc10 );	
		}
		else
		{
			EffectTechniqueMapType::iterator it_technique = m_effectTechniqueMap.find( ppTechnique10 );
			if( it_technique != m_effectTechniqueMap.end() && it_technique->second != NULL )
			{				
				D3DX11_TECHNIQUE_DESC techniqueDesc11;
				hr = it_technique->second->GetDesc( &techniqueDesc11 );
				if( SUCCEEDED( hr ) )
				{
					pEffectDesc10->Annotations = techniqueDesc11.Annotations;
					pEffectDesc10->Name = techniqueDesc11.Name;
					pEffectDesc10->Passes = techniqueDesc11.Passes;
				}
			}
		}
		return hr;
	}

	HRESULT Technique_GetPassDescByIndex(
		__in  ID3D10EffectTechnique **ppTechnique10,
		__in  UINT index,
		__out D3D10_PASS_DESC* pPassDesc10
		)
	{
		HRESULT hr = E_FAIL;
		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
		{
			hr = (*ppTechnique10)->GetPassByIndex( index )->GetDesc( pPassDesc10 );	
		}
		else
		{
			EffectTechniqueMapType::iterator it_technique = m_effectTechniqueMap.find( ppTechnique10 );
			if( it_technique != m_effectTechniqueMap.end() && it_technique->second != NULL )
			{				
				D3DX11_PASS_DESC passDesc11;
				hr = it_technique->second->GetPassByIndex( index )->GetDesc( &passDesc11 );	
				if( SUCCEEDED( hr ) )
				{
					// Pass back the PASS_DESC args as D3D10_PASS_SEC
					pPassDesc10->Name = passDesc11.Name;
					pPassDesc10->Annotations = passDesc11.Annotations;
					pPassDesc10->pIAInputSignature = passDesc11.pIAInputSignature;
					pPassDesc10->IAInputSignatureSize = passDesc11.IAInputSignatureSize;
					pPassDesc10->BlendFactor[0] = passDesc11.BlendFactor[0];
					pPassDesc10->BlendFactor[1] = passDesc11.BlendFactor[1];
					pPassDesc10->BlendFactor[2] = passDesc11.BlendFactor[2];
					pPassDesc10->BlendFactor[3] = passDesc11.BlendFactor[3];
					pPassDesc10->SampleMask = passDesc11.SampleMask;
					pPassDesc10->StencilRef = passDesc11.StencilRef;
				}
			}
		}
		return hr;
	}

	HRESULT Technique_ApplyPassByIndex(
		__in  ID3D10EffectTechnique **ppTechnique10,
		__in UINT index
		)
	{
		HRESULT hr = E_FAIL;
		if( IsEqualGUID( m_emulatedInterface, __uuidof(ID3D10Device) ))
		{
			hr = (*ppTechnique10)->GetPassByIndex(index)->Apply( 0 );	
		}
		else
		{
			EffectTechniqueMapType::iterator it_technique = m_effectTechniqueMap.find( ppTechnique10 );
			if( it_technique != m_effectTechniqueMap.end() && it_technique->second != NULL )
			{				
				ID3D11DeviceContext *pImmctx;
				m_pD3D11Device->GetImmediateContext( &pImmctx );
				hr = it_technique->second->GetPassByIndex(index)->Apply( 0, pImmctx );	
				pImmctx->Release();
			}
		}
		return hr;
	}

private:
	ID3D11Device1                   *m_pD3D11Device1;
	ID3D11Device                    *m_pD3D11Device;
	ID3D10Device                    *m_pD3D10Device;

	// save the original state
	ID3DDeviceContextState			*m_pDeviceContextState[3];
	GUID m_emulatedInterface;

	// preserve the feature level used during dxgi test framework's device creation
	D3D_FEATURE_LEVEL m_FeatureLevelCreated;

	// D3DX11 Effect
	EffectMapType m_effectMap;
	EffectTechniqueMapType m_effectTechniqueMap;

	UINT m_iCurrentDCSIndex;

};
