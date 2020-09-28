#include <pch.h>
#include "D3DWrapped.h"

#pragma prefast( disable: 1, "All functions in this module are stubs which throw exceptions" )

HRESULT CDevice::CreateTexture2DFromSwapChain( IDXGISwapChain **ppSwapChain, CTexture2D ** ppTexture2D )
{
	throw( WRAPPER_METHOD_UNSUPPORTED );
	HRESULT hr;
	return hr;
}

HRESULT CDevice::CreateDebug( CDebug ** ppDebug )
{
	throw( WRAPPER_METHOD_UNSUPPORTED );
	HRESULT hr;
	return hr;
}

HRESULT CDevice::CreateInfoQueue( CInfoQueue ** ppInfoQueue )
{
	throw( WRAPPER_METHOD_UNSUPPORTED );
	HRESULT hr;
	return hr;
}

HRESULT CDevice::CreateMultithread( CMultithread ** ppMultithread )
{
	throw( WRAPPER_METHOD_UNSUPPORTED );
	HRESULT hr;
	return hr;
}

HRESULT CDevice::CreateSwitchToRef( CSwitchToRef **ppSwitchToRef )
{
	throw( WRAPPER_METHOD_UNSUPPORTED );
	HRESULT hr;
	return hr;
}
