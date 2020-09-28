#include <pch.h>
#include "D3DMultiWrapped.h"

HRESULT CMultiDevice::CreateTexture2DFromSwapChain( IDXGISwapChain *ppSwapChain[], CTexture2D ** ppTexture2D )
{
	HRESULT rval = S_OK;
	CMultiTexture2D *pTempTexture2D = new CMultiTexture2D( GetSubObjectCount() );
	for( int i = GetSubObjectCount() - 1; i >= 0; --i )
	{
		CTexture2D *pTexture2D;
		rval = GetSubDevice( i )->CreateTexture2DFromSwapChain( &ppSwapChain[i], &pTexture2D );
		if( FAILED( rval ) )
			break;
		pTempTexture2D->SetSubTexture2D( i, pTexture2D );	
		pTexture2D->Release();	
	}

	if( FAILED( rval ) )
	{
		delete( pTempTexture2D );
		return rval;
	}
	pTempTexture2D->AddRef();
	*ppTexture2D = pTempTexture2D;
	return rval;
}

HRESULT CMultiDevice::CreateDebug( CDebug *ppDebug[] )
{
	HRESULT rval = S_OK;
	CMultiDebug *pTempDebug = new CMultiDebug( GetSubObjectCount() );
	for( int i = GetSubObjectCount() - 1; i >= 0; --i )
	{
		CDebug *pDebug;
		rval = GetSubDevice( i )->CreateDebug( &pDebug );
		if( FAILED( rval ) )
			break;
		pTempDebug->SetSubDebug( i, pDebug );	
		pDebug->Release();	
	}

	if( FAILED( rval ) )
	{
		delete( pTempDebug );
		return rval;
	}
	pTempDebug->AddRef();
	*ppDebug = pTempDebug;
	return rval;
}

HRESULT CMultiDevice::CreateInfoQueue( CInfoQueue *ppInfoQueue[] )
{
	HRESULT rval = S_OK;
	CMultiInfoQueue *pTempInfoQueue = new CMultiInfoQueue( GetSubObjectCount() );
	for( int i = GetSubObjectCount() - 1; i >= 0; --i )
	{
		CInfoQueue *pInfoQueue;
		rval = GetSubDevice( i )->CreateInfoQueue( &pInfoQueue );
		if( FAILED( rval ) )
			break;
		pTempInfoQueue->SetSubInfoQueue( i, pInfoQueue );		
		pInfoQueue->Release();
	}

	if( FAILED( rval ) )
	{
		delete( pTempInfoQueue );
		return rval;
	}
	pTempInfoQueue->AddRef();
	*ppInfoQueue = pTempInfoQueue;
	return rval;
}

HRESULT CMultiDevice::CreateMultithread( CMultithread *ppMultithread[] )
{
	HRESULT rval = S_OK;
	CMultiMultithread *pTempMultithread = new CMultiMultithread( GetSubObjectCount() );
	for( int i = GetSubObjectCount() - 1; i >= 0; --i )
	{
		CMultithread *pMultithread;
		rval = GetSubDevice( i )->CreateMultithread( &pMultithread );
		if( FAILED( rval ) )
			break;
		pTempMultithread->SetSubMultithread( i, pMultithread );	
		pMultithread->Release();	
	}

	if( FAILED( rval ) )
	{
		delete( pTempMultithread );
		return rval;
	}
	pTempMultithread->AddRef();
	*ppMultithread = pTempMultithread;
	return rval;
}

HRESULT CMultiDevice::CreateSwitchToRef( CSwitchToRef *ppSwitchToRef[] )
{
	HRESULT rval = S_OK;
	CMultiSwitchToRef *pTempSwitchToRef = new CMultiSwitchToRef( GetSubObjectCount() );
	for( int i = GetSubObjectCount() - 1; i >= 0; --i )
	{
		CSwitchToRef *pSwitchToRef;
		rval = GetSubDevice( i )->CreateSwitchToRef( &pSwitchToRef );
		if( FAILED( rval ) )
			break;
		pTempSwitchToRef->SetSubSwitchToRef( i, pSwitchToRef );	
		pSwitchToRef->Release();	
	}

	if( FAILED( rval ) )
	{
		delete( pTempSwitchToRef );
		return rval;
	}
	pTempSwitchToRef->AddRef();
	*ppSwitchToRef = pTempSwitchToRef;
	return rval;
}
