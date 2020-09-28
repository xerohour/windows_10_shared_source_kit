#include "pch.h"
#include "D3D11Wrapped.h"
#include "D3D11Convert.h"

HRESULT C11Device::ExecDeferredContext()
{
	HRESULT hr;
	ID3D11CommandList* pCommandList = NULL;
	hr = m_pDeviceDeferredContext->FinishCommandList(TRUE, &pCommandList);
	if (SUCCEEDED(hr))
	{
		m_pDeviceImmediateContext->ExecuteCommandList(pCommandList, TRUE);
		SAFE_RELEASE(pCommandList);
	}
	return hr;
}

HRESULT C11Device::SwitchDeferredContext(bool bForceDisable)
{
	HRESULT hr = S_OK;
	bool enableDeferred = m_bUseDeferredContext;
	if (bForceDisable)
		enableDeferred = false;
	else
	{
		int randval = rand() % 100;
		if (randval < 10)
			enableDeferred = !enableDeferred;
	}
	if (enableDeferred != m_bUseDeferredContext)
	{
		if (enableDeferred)
		{
			//nothing special to do here
			m_bUseDeferredContext = true;
		}
		else
		{
			// flush & execute our existing deferred context state
			hr = ExecDeferredContext();
			m_bUseDeferredContext = false;
		}
	}
	return hr;
}

HRESULT C11Device::CreateTexture2DFromSwapChain( IDXGISwapChain **ppSwapChain, CTexture2D ** ppTexture2D )
{
	assert( m_pDevice != NULL );
	assert( ppSwapChain != NULL );

	HRESULT hr = S_OK;

	if ( ppTexture2D )
	{
		ID3D11Texture2D * pTexture2D10 = NULL;
		hr = (*ppSwapChain)->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void **) &pTexture2D10 );
		*ppTexture2D = new C11Texture2D( pTexture2D10 );
		(*ppTexture2D)->AddRef();
	}

	return hr;
}

HRESULT C11Device::CreateDebug( CDebug ** ppDebug )
{
	assert( m_pDevice != NULL );
	assert( ppDebug != NULL );

	HRESULT hr = S_OK;

	if ( ppDebug )
	{
		ID3D11Debug * ppDebug10 = NULL;
		hr = m_pDevice->QueryInterface(__uuidof( ID3D11Debug ), (void **) &ppDebug10 );
		*ppDebug = new C11Debug( ppDebug10 );
		(*ppDebug)->AddRef();
	}

	return hr;
}

HRESULT C11Device::CreateInfoQueue( CInfoQueue ** ppInfoQueue )
{
	assert( m_pDevice != NULL );
	assert( ppInfoQueue != NULL );

	HRESULT hr = S_OK;

	if ( ppInfoQueue )
	{
		ID3D11InfoQueue * ppInfoQueue10 = NULL;
		hr = m_pDevice->QueryInterface(__uuidof( ID3D11InfoQueue ), (void **) &ppInfoQueue10 );
		*ppInfoQueue = new C11InfoQueue( ppInfoQueue10 );
		(*ppInfoQueue)->AddRef();
	}

	return hr;
}

HRESULT C11Device::CreateMultithread( CMultithread ** ppMultithread )
{
	assert( m_pDevice != NULL );
	assert( ppMultithread != NULL );

	HRESULT hr = S_OK;

	if ( ppMultithread )
	{
		/*ID3D11Multithread * ppMultithread10 = NULL;
		hr = m_pDevice->QueryInterface(__uuidof( ID3D11Multithread ), (void **) &ppMultithread10 );
		*ppMultithread = new C11Multithread( ppMultithread10 );
		(*ppMultithread)->AddRef();*/
		*ppMultithread = NULL;
		hr = S_OK;
	}
	return hr;
}

HRESULT C11Device::CreateSwitchToRef( CSwitchToRef ** ppSwitchToRef )
{
	assert( m_pDevice != NULL );
	assert( ppSwitchToRef != NULL );

	HRESULT hr = S_OK;

	if ( ppSwitchToRef )
	{
		ID3D11SwitchToRef * ppSwitchToRef10 = NULL;
		hr = m_pDevice->QueryInterface(__uuidof( ID3D11SwitchToRef ), (void **) &ppSwitchToRef10 );
		*ppSwitchToRef = new C11SwitchToRef( ppSwitchToRef10 );
		(*ppSwitchToRef)->AddRef();
	}
	return hr;
}

void D3DWrapperConvertStruct( D3D_BLEND_DESC *d3d_blend_desc, D3D11_BLEND_DESC const *d3d_blend_desc_10 )
{
	d3d_blend_desc->AlphaToCoverageEnable = d3d_blend_desc_10->AlphaToCoverageEnable;
	for (int n = 0; n < 8; ++n)
	{
		d3d_blend_desc->BlendEnable[n] = d3d_blend_desc_10->RenderTarget[n].BlendEnable;
		d3d_blend_desc->RenderTargetWriteMask[n] = d3d_blend_desc_10->RenderTarget[n].RenderTargetWriteMask;
	}
	D3DWrapperConvertEnum(&d3d_blend_desc->SrcBlend, d3d_blend_desc_10->RenderTarget[0].SrcBlend);
	D3DWrapperConvertEnum(&d3d_blend_desc->DestBlend, d3d_blend_desc_10->RenderTarget[0].DestBlend);
	D3DWrapperConvertEnum(&d3d_blend_desc->BlendOp, d3d_blend_desc_10->RenderTarget[0].BlendOp);
	D3DWrapperConvertEnum(&d3d_blend_desc->SrcBlendAlpha, d3d_blend_desc_10->RenderTarget[0].SrcBlendAlpha);
	D3DWrapperConvertEnum(&d3d_blend_desc->DestBlendAlpha, d3d_blend_desc_10->RenderTarget[0].DestBlendAlpha);
	D3DWrapperConvertEnum(&d3d_blend_desc->BlendOpAlpha, d3d_blend_desc_10->RenderTarget[0].BlendOpAlpha);
}

void D3DWrapperConvertStruct( D3D11_BLEND_DESC *d3d_blend_desc_10, D3D_BLEND_DESC const *d3d_blend_desc )
{
	d3d_blend_desc_10->AlphaToCoverageEnable = d3d_blend_desc->AlphaToCoverageEnable;
	d3d_blend_desc_10->IndependentBlendEnable = TRUE;
	for (int n = 0; n < 8; ++n)
	{
		d3d_blend_desc_10->RenderTarget[n].BlendEnable = d3d_blend_desc->BlendEnable[n];
		d3d_blend_desc_10->RenderTarget[n].RenderTargetWriteMask = d3d_blend_desc->RenderTargetWriteMask[n];
		D3DWrapperConvertEnum(&d3d_blend_desc_10->RenderTarget[n].SrcBlend, d3d_blend_desc->SrcBlend);
		D3DWrapperConvertEnum(&d3d_blend_desc_10->RenderTarget[n].DestBlend, d3d_blend_desc->DestBlend);
		D3DWrapperConvertEnum(&d3d_blend_desc_10->RenderTarget[n].BlendOp, d3d_blend_desc->BlendOp);
		D3DWrapperConvertEnum(&d3d_blend_desc_10->RenderTarget[n].SrcBlendAlpha, d3d_blend_desc->SrcBlendAlpha);
		D3DWrapperConvertEnum(&d3d_blend_desc_10->RenderTarget[n].DestBlendAlpha, d3d_blend_desc->DestBlendAlpha);
		D3DWrapperConvertEnum(&d3d_blend_desc_10->RenderTarget[n].BlendOpAlpha, d3d_blend_desc->BlendOpAlpha);
	}
}

void D3DWrapperConvertStruct( D3D_VIEWPORT *d3d_viewport, D3D11_VIEWPORT const *d3d_viewport_10 )
{
    d3d_viewport->TopLeftX = (INT)d3d_viewport_10->TopLeftX;
    d3d_viewport->TopLeftY = (INT)d3d_viewport_10->TopLeftY;
    d3d_viewport->Width = (INT)d3d_viewport_10->Width;
    d3d_viewport->Height = (INT)d3d_viewport_10->Height;
    d3d_viewport->MinDepth = d3d_viewport_10->MinDepth;
    d3d_viewport->MaxDepth = d3d_viewport_10->MaxDepth;
}

void D3DWrapperConvertStruct( D3D11_VIEWPORT *d3d_viewport_10, D3D_VIEWPORT const *d3d_viewport )
{
    d3d_viewport_10->TopLeftX = (FLOAT)d3d_viewport->TopLeftX;
    d3d_viewport_10->TopLeftY = (FLOAT)d3d_viewport->TopLeftY;
    d3d_viewport_10->Width = (FLOAT)d3d_viewport->Width;
    d3d_viewport_10->Height = (FLOAT)d3d_viewport->Height;
    d3d_viewport_10->MinDepth = d3d_viewport->MinDepth;
    d3d_viewport_10->MaxDepth = d3d_viewport->MaxDepth;
}
