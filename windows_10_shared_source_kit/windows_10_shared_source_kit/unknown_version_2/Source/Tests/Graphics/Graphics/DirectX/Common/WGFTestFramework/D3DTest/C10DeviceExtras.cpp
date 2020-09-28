#include "pch.h"
#include "D3D10Wrapped.h"

HRESULT C10Device::CreateTexture2DFromSwapChain( IDXGISwapChain **ppSwapChain, CTexture2D ** ppTexture2D )
{
	assert( m_pDevice != NULL );
	assert( ppSwapChain != NULL );

	HRESULT hr = S_OK;

	if ( ppTexture2D )
	{
		ID3D10Texture2D * pTexture2D10 = NULL;
		hr = (*ppSwapChain)->GetBuffer( 0, __uuidof( ID3D10Texture2D ), (void **) &pTexture2D10 );
		*ppTexture2D = new C10Texture2D( pTexture2D10 );
		(*ppTexture2D)->AddRef();
	}

	return hr;
}

HRESULT C10Device::CreateDebug( CDebug ** ppDebug )
{
	assert( m_pDevice != NULL );
	assert( ppDebug != NULL );

	HRESULT hr = S_OK;

	if ( ppDebug )
	{
		ID3D10Debug * ppDebug10 = NULL;
		hr = m_pDevice->QueryInterface(__uuidof( ID3D10Debug ), (void **) &ppDebug10 );
		*ppDebug = new C10Debug( ppDebug10 );
		(*ppDebug)->AddRef();
	}

	return hr;
}

HRESULT C10Device::CreateInfoQueue( CInfoQueue ** ppInfoQueue )
{
	assert( m_pDevice != NULL );
	assert( ppInfoQueue != NULL );

	HRESULT hr = S_OK;

	if ( ppInfoQueue )
	{
		ID3D10InfoQueue * ppInfoQueue10 = NULL;
		hr = m_pDevice->QueryInterface(__uuidof( ID3D10InfoQueue ), (void **) &ppInfoQueue10 );
		*ppInfoQueue = new C10InfoQueue( ppInfoQueue10 );
		(*ppInfoQueue)->AddRef();
	}

	return hr;
}

HRESULT C10Device::CreateMultithread( CMultithread ** ppMultithread )
{
	assert( m_pDevice != NULL );
	assert( ppMultithread != NULL );

	HRESULT hr = S_OK;

	if ( ppMultithread )
	{
		ID3D10Multithread * ppMultithread10 = NULL;
		hr = m_pDevice->QueryInterface(__uuidof( ID3D10Multithread ), (void **) &ppMultithread10 );
		*ppMultithread = new C10Multithread( ppMultithread10 );
		(*ppMultithread)->AddRef();
	}
	return hr;
}

HRESULT C10Device::CreateSwitchToRef( CSwitchToRef ** ppSwitchToRef )
{
	assert( m_pDevice != NULL );
	assert( ppSwitchToRef != NULL );

	HRESULT hr = S_OK;

	if ( ppSwitchToRef )
	{
		ID3D10SwitchToRef * ppSwitchToRef10 = NULL;
		hr = m_pDevice->QueryInterface(__uuidof( ID3D10SwitchToRef ), (void **) &ppSwitchToRef10 );
		*ppSwitchToRef = new C10SwitchToRef( ppSwitchToRef10 );
		(*ppSwitchToRef)->AddRef();
	}
	return hr;
}
