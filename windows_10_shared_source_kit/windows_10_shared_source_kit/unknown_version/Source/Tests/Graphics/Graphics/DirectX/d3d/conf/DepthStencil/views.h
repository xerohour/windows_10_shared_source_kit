#pragma once

#include "D3D11Test.h"
#include "new_off.h"
#include "d3dx10.h"
#include "new_on.h"

#include <WGFTestCore.h>

using namespace std;
#include <map>

#define	DS_ARRAYSIZE	4
#define DS_MIPLEVELS	3
#define DS_DEPTH		0.5f
#define DS_STENCIL		9

#define MASK_8			0x000000ff
#define MASK_16			0x0000ffff
#define MASK_24			0x00ffffff

enum ComponentType
{
	COMP_UNORM,
	COMP_FLOAT,
	COMP_UINT
};

struct DSFormatInfo
{
	DXGI_FORMAT		Format;
	bool			HasStencil;
	UINT			DepthBits;
	UINT			StencilBits;
	UINT			TotalBits;
	ComponentType	DepthCompType;
	ComponentType	StencilCompType;
};

typedef map<DXGI_FORMAT,DSFormatInfo> DSFormatMap;

class CDepthStencilTest : public CD3D11Test
{
public:
	CDepthStencilTest( ) :
		m_uArraySize( DS_ARRAYSIZE ),
		m_pResourceDS( NULL ),
		m_pResourceDSCopy( NULL ),
		m_pDSV( NULL ),
		m_pSRV( NULL ),
		m_pRTV( NULL ),
		m_pDSState( NULL ),
		m_pVertexBuffer( NULL ),
		m_pVS( NULL ),
		m_pGS( NULL ),
		m_pPS( NULL ),
		m_pConstBuffer( NULL ),
		m_pLayout( NULL ),
		m_pSampler( NULL ),
		m_pRSEnabled( NULL ),
		m_uStencilRef( 0 ),
		m_bClearTest( false ),
		m_bRTTest( false ),
		m_uMipLevels( DS_MIPLEVELS ),
		m_fDepthClear( DS_DEPTH ),
		m_uStencilClear( DS_STENCIL )
	{ 
		CreateFormatMap( );
	}

	virtual void			InitTestParameters( );
	virtual TEST_RESULT		Setup( );
	virtual void			Cleanup( );

	virtual TEST_RESULT		SetupTestCase( );
	virtual TEST_RESULT		ExecuteTestCase( );
	virtual void			CleanupTestCase( );

	virtual TEST_RESULT		SetupGeometry( );
	virtual TEST_RESULT		SetupShaders( );
	virtual void			CreateFormatMap( );

	virtual bool			CompareResults( void *pResult, UINT uPitch, UINT uMipSlice );
	virtual bool			GetMipDimensions( UINT uMipSlice, UINT &uWidth, UINT &uHeight );
	virtual float			ToFloat32( UINT32 uData, UINT uBits, ComponentType CompType );
	virtual bool			WithinOneULP( float fReference, float fOutput );
	virtual bool			IsDenorm( float fValue );
	virtual bool			IsNaN( float fValue );

protected:
	D3D11_DEPTH_STENCIL_DESC		m_DSDesc;
	D3D11_DSV_DIMENSION			m_ResourceDimension;	
	DXGI_FORMAT					m_Format;
	UINT						m_uMipSlice;
	UINT						m_uArraySlice;
	UINT						m_uArraySize;
	UINT32						m_uWidth;
	UINT32						m_uHeight;
	float						m_fDepth;
	UINT						m_uStencilRef;
	D3DXMATRIX					m_mxTrans;
	D3D11_SAMPLER_DESC			m_SamplerDesc;

	bool						m_bClearTest;
	float						m_fDepthClear;
	UINT8						m_uStencilClear;
	bool						m_bRTTest;
	UINT						m_uMipLevels;
	DSFormatMap					m_FormatMap;

	ID3D11Resource					*m_pResourceDS;
	ID3D11Resource					*m_pResourceDSCopy;
	ID3D11DepthStencilView			*m_pDSV;
	ID3D11ShaderResourceView			*m_pSRV;
	ID3D11RenderTargetView			*m_pRTV;
	ID3D11DepthStencilState			*m_pDSState;
	ID3D11Buffer						*m_pVertexBuffer;
	ID3D11VertexShader				*m_pVS;
	ID3D11GeometryShader				*m_pGS;
	ID3D11PixelShader				*m_pPS;
	ID3D11Buffer						*m_pConstBuffer;
	ID3D11InputLayout				*m_pLayout;
	ID3D11SamplerState				*m_pSampler;
	ID3D11RasterizerState			*m_pRSEnabled;

};

class CDepthStencilClearTest : public CDepthStencilTest
{
public:
	CDepthStencilClearTest( )
	{ 
		m_bClearTest = true; 
		m_fDepth = 0.f; 
	}

	void		InitTestParameters();
	
protected:
	
};

struct TLVERTEX
{
	float	vPosition[2];
	float	vTexCoord[4];
};