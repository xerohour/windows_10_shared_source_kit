#pragma once


//
// Win32 headers
//

#include <windows.h>

#include <dxgi.h>

#include <d3d11.h>

#include <d3d10_1.h>
#include <d3dx10.h>


//
// STL headers
//

#include <map>


//
// Framework headers
//

#include <WGFTestCore.h>
#include <DXGITestFramework.h>
#include "HResultHelp.h"

//
// Project headers
//

#include "D3D10SwapChain.h"

#include "WGFHelpers.h"


class CDeviceWrapper10
{
	public:
		CDeviceWrapper10
		(
			CTest * pInitLoggingGroup
		);

		virtual ~CDeviceWrapper10();


	private:
		friend class CSwapChainWrapper10;
		friend class DXGI_RESULT;

		bool m_bAlternateRender;
		UINT m_nxDivisions;
		UINT m_nyDivisions;
		UINT m_nNumVertices;
		UINT m_nNumIndixes;
		UINT m_nStride;

		bool bReadyForTexRender;
		ID3D10Buffer *m_pVB;
		ID3D10Buffer *m_pIB;
		ID3D10Buffer *m_pVSBuffer;
		ID3D10InputLayout *m_pEL;
		ID3D10PixelShader *m_pPixelShader;
        ID3D10PixelShader *m_pPixelShaderRemoveXR_BIAS;
		ID3D10VertexShader *m_pVertexShader;
		ID3D10SamplerState *m_pSampler;
		ID3D10RasterizerState *m_pRastState;
		ID3D10BlendState *m_pBlendState;
		ID3D10DepthStencilState *m_pDepthState;


	protected:
		CTest * const pLoggingGroup;

		ID3D10Device * pD3DDevice;

		DXGI_MODE_ROTATION m_RotationMode;

		HRESULT m_hrLastError;

	public:
		HRESULT WrapDevice
		(
			ID3D10Device * pDevice
		);


		bool IsReadyForTexRender() {return bReadyForTexRender;}

		HRESULT LoadShader
		(
			LPCSTR ShaderCode,
			__out  void **ppShader,
			LPCSTR pFuncName,
			LPCSTR pProfile,
			__out_opt ID3D10Blob **ppRetainCompiledShader = NULL
		);

		bool CreateTexRenderResources
		(
			LPCSTR VertexShaderModel,
			LPCSTR PixelShaderModel
		);

		void ReleaseTexRenderResources();

		bool RenderTexture
		(
			__in     ID3D10RenderTargetView *pRTView,
			__in     ID3D10ShaderResourceView *pTexSRV,
			__in     const D3D10_VIEWPORT *pD3D10ViewPort,
            __in     BOOL renderXR_BIAS
		);

		bool GetFactory
		(
			__inout     IDXGIFactory **pFactoryPointer
		);

		BOOL GetDevicePtr
		(
			__inout     ID3D10Device **pDevicePointer
		);

		void ReleaseAllObjects();

		inline void SetAlternateRenderPath( bool bAlternateRenderPath )
		{
			m_bAlternateRender = bAlternateRenderPath;
		};

		inline void SetRotationMode( DXGI_MODE_ROTATION rotationMode )
		{
			m_RotationMode = rotationMode;
		};

		HRESULT GetLastError() { return m_hrLastError; }
};


class CSwapChainWrapper10
{
	private:
		UINT nDisplayDeviceCount;


	protected:
		CTest * const pLoggingGroup;
		CDeviceWrapper10 * const pParentD3DDevice;
		IDXGISwapChain *pSwapChain;
		HRESULT hrLastError;

	public:

		CSwapChainWrapper10
		(
			CTest * pInitLoggingGroup,
			CDeviceWrapper10 * pInitParentD3D10Device
		)
		:
			pLoggingGroup( pInitLoggingGroup ),
			pParentD3DDevice( pInitParentD3D10Device ),
			pSwapChain( NULL ),
			hrLastError( S_OK )
		{
			clsGDIData  GDIData;

			nDisplayDeviceCount = GDIData.GetAttachedDisplayDeviceCount();
		};

		virtual ~CSwapChainWrapper10();


	public:
		BOOL  InitializeSwapChain( DXGI_SWAP_CHAIN_DESC *pSwapChainDesc );
		BOOL  InitializeFullscreenSwapChain( DXGI_SWAP_CHAIN_DESC* pSwapChainDesc );

		void  ReleaseAllObjects();

		bool  InitBBFrom2DTexture
		(
			UINT uiBBIndex,
			__in ID3D10ShaderResourceView *pTexSRV,
			LPCSTR VertexShaderModel,
			LPCSTR PixelShaderModel,
			const HelperType::Vector2D<unsigned>* = nullptr
		);

		bool  InitBBViaClear( UINT uiBBIndex );

		BOOL  GetSwapChainPtr( IDXGISwapChain **pSwapChainPointer );
        void  WrapSwapchain(__in IDXGISwapChain *pD3DSwapChain);

		HRESULT GetLastError() { return hrLastError; }
};

class DXGI_RESULT
{
public:
	DXGI_RESULT( CDeviceWrapper10* pdevice ) : m_pDevice ( pdevice ), m_sc ( NULL ), m_hr( S_OK ) {}
	DXGI_RESULT( CDeviceWrapper10* pdevice, HRESULT hr ) : m_pDevice ( pdevice ), m_sc ( NULL ) , m_hr( hr )
	{
		*this = hr;
	}
	DXGI_RESULT( IDXGISwapChain* &sc ) : m_pDevice ( NULL ), m_sc ( sc ), m_hr( S_OK )
	{
		if(m_sc) m_sc->AddRef();
	}
	DXGI_RESULT( IDXGISwapChain* &sc, HRESULT hr ) :m_pDevice ( NULL ), m_sc( sc ) , m_hr( hr )
	{
		if(m_sc) m_sc->AddRef();
		*this = hr;
	}
	DXGI_RESULT( IDXGISwapChain1* &sc1 ) : m_pDevice ( NULL ), m_sc ( NULL ), m_hr( S_OK )
	{
		if(sc1) sc1->QueryInterface( __uuidof( m_sc ), (void**)&m_sc);
	}
	DXGI_RESULT( IDXGISwapChain1* &sc1, HRESULT hr ) :m_pDevice ( NULL ), m_sc( NULL ) , m_hr( hr )
	{
		if(sc1) sc1->QueryInterface( __uuidof( m_sc ), (void**)&m_sc);
	}
	DXGI_RESULT( const DXGI_RESULT &dxgihr ) : m_pDevice( dxgihr.m_pDevice ), m_sc( dxgihr.m_sc )
	{
		if(m_sc) m_sc->AddRef();
		*this = dxgihr.m_hr;
	}
	~DXGI_RESULT()
	{
		SAFE_RELEASE( m_sc );
	}

    bool operator==( HRESULT hr ) const { return m_hr == hr; }
    bool operator==( const DXGI_RESULT &tr ) const { return m_hr == tr.m_hr; }

    DXGI_RESULT &operator=( const HRESULT hr )
    {
        m_hr = hr;
        switch( hr )
        {
        case S_OK:
            break;
		case DXGI_ERROR_DEVICE_REMOVED:
			{
				ID3D10Device* pDevice10 = NULL;
				if( m_pDevice && m_pDevice->GetDevicePtr( &pDevice10 ) )
				{
					if( pDevice10 )
					{
						HRESULT hrDR = pDevice10->GetDeviceRemovedReason();
						if( m_pDevice->pLoggingGroup )
							m_pDevice->pLoggingGroup->WriteToLog( _T( "DeviceRemoved: Reason: %s." ), D3DHResultToString(m_hr).c_str() );
					}
				}
				else if( m_sc && SUCCEEDED( m_sc->GetDevice( __uuidof( ID3D10Device ), (void**)&pDevice10 ) ) )
				{
					if( pDevice10 )
					{
						HRESULT hrDR = pDevice10->GetDeviceRemovedReason();
						CWGFTestFramework* pFramework = ::GetFramework();
						if( pFramework ) pFramework->WriteToLogF( _T( "DeviceRemoved: Reason: %s." ), D3DHResultToString(m_hr).c_str() );
					}
				}
				SAFE_RELEASE( pDevice10 );
				// proceed to print out original result string
			}
        default:
            if( m_pDevice && m_pDevice->pLoggingGroup )
                m_pDevice->pLoggingGroup->WriteToLog( _T( "HRESULT: %s." ), D3DHResultToString(m_hr).c_str() );
            else if( m_sc )
			{
				CWGFTestFramework* pFramework = ::GetFramework();
                if( pFramework ) pFramework->WriteToLogF( _T( "HRESULT: %s." ), D3DHResultToString(m_hr).c_str() );
			}
            break;
        }

        return *this;
    }

	operator HRESULT() { return m_hr; }

private:
    HRESULT m_hr;
	CDeviceWrapper10* m_pDevice;
	IDXGISwapChain* m_sc;
};