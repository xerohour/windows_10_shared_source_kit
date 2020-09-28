//////////////////////////////////////////////////////////////////////////
//  D3D10Test.h
//  created:	2005/06/08
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>

//Need to include d3d10_1.h before anything that includes d3d10.h until the header is fixed
#include "D3D10_1.h"

#include "new_off.h"
#include "D3DX10.h"
#include "new_on.h"

#include <WGFTestCore.h>
#include "d3dtest.h"
#include "D3D10ValueSets.h"
#include "DXGIValueSets.h"

class CD3D10TestFramework : public CD3DTestFramework
{
public:
	CD3D10TestFramework();

	virtual void InitOptionVariables();

	ID3D10Device *GetD3DDevice()					{ ID3D10Device *pDevice = NULL; if( CD3DTestFramework::GetDevice() ) CD3DTestFramework::GetDevice()->GetInterfacePtr( &pDevice ); return pDevice; }
	ID3D10Debug *GetD3DDebug()						{ ID3D10Debug *pDebug = NULL; if( CD3DTestFramework::GetDebug() ) CD3DTestFramework::GetDebug()->GetInterfacePtr( &pDebug ); return pDebug; }
	ID3D10InfoQueue *GetD3DInfoQueue()				{ ID3D10InfoQueue *pInfoQueue = NULL; if( CD3DTestFramework::GetInfoQueue() ) CD3DTestFramework::GetInfoQueue()->GetInterfacePtr( &pInfoQueue ); return pInfoQueue; }
	ID3D10Multithread *GetD3DMultithread()			{ ID3D10Multithread *pMultithread = NULL; if( CD3DTestFramework::GetMultithread() ) CD3DTestFramework::GetMultithread()->GetInterfacePtr( &pMultithread ); return pMultithread; }
	
	ID3D10Texture2D *GetRTTexture(UINT uRT=0)		{ ID3D10Texture2D *pTexture = NULL; if( CD3DTestFramework::GetRTTexture( uRT ) ) CD3DTestFramework::GetRTTexture( uRT )->GetInterfacePtr( &pTexture ); return pTexture; }
	ID3D10RenderTargetView *GetRTView(UINT uRT=0)	{ ID3D10RenderTargetView *pView = NULL; if( CD3DTestFramework::GetRTView( uRT ) ) CD3DTestFramework::GetRTView( uRT )->GetInterfacePtr( &pView ); return pView; }
	ID3D10Texture2D *GetDSTexture()					{ ID3D10Texture2D *pTexture = NULL; if( CD3DTestFramework::GetDSTexture() ) CD3DTestFramework::GetDSTexture()->GetInterfacePtr( &pTexture ); return pTexture; }
	ID3D10DepthStencilView *GetDSView()				{ ID3D10DepthStencilView *pView = NULL; if( CD3DTestFramework::GetDSView() ) CD3DTestFramework::GetDSView()->GetInterfacePtr( &pView ); return pView; }

	bool IsSDKTest() const { return m_bSDKTest; }

private:	
	bool m_bSDKTest;
};

class CD3D10Test : public CD3DTest
{
public:
	CD3D10TestFramework *GetFramework() { return dynamic_cast< CD3D10TestFramework *>( ::GetFramework() ); }
	
	IDXGIFactory *GetDXGIFactory()					{ return GetFramework()->GetDXGIFactory( 0 );		}
	IDXGIAdapter *GetDXGIAdapter()					{ return GetFramework()->GetDXGIAdapter( 0 );		}
	IDXGISwapChain *GetDXGISwapChain(int uRT)		{ return GetFramework()->GetDXGISwapChains( uRT )[0]; }
	
	ID3D10Device *GetD3DDevice()					{ return GetFramework()->GetD3DDevice();		}
	ID3D10Debug *GetD3DDebug()						{ return GetFramework()->GetD3DDebug();			}
	ID3D10InfoQueue *GetD3DInfoQueue()				{ return GetFramework()->GetD3DInfoQueue();			}
	ID3D10Multithread *GetD3DMultithread()			{ return GetFramework()->GetD3DMultithread();	}
	
	ID3D10Texture2D *GetRTTexture(int uRT=0)		{ return GetFramework()->GetRTTexture( uRT );	}
	ID3D10RenderTargetView *GetRTView(int uRT=0)	{ return GetFramework()->GetRTView( uRT );	}
	ID3D10Texture2D *GetDSTexture()					{ return GetFramework()->GetDSTexture();		}
	ID3D10DepthStencilView *GetDSView()				{ return GetFramework()->GetDSView();			}
};


