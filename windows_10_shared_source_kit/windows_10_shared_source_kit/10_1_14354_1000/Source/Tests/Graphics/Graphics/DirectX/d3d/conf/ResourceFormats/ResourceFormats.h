#pragma once

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <dxmini.h>
#include <d3dkmddi.h>
#include <d3dkmthk.h>
#include "AdapterUtil.h"

#include "d3d10_1.h"
#include "new_off.h"
#include "d3dx10.h"
#include "new_on.h"

#include "D3D11Test.h"
#include "FormatHelper.h"
#include "FormatDesc.hpp"

#include <WGFTestCore.h>


#define	FORMAT_TEXTURE	DXGI_FORMAT_R32G32B32A32_FLOAT
#define	FORMAT_RT_FLOAT	DXGI_FORMAT_R32G32B32A32_FLOAT
#define	FORMAT_RT_INT	DXGI_FORMAT_R32G32B32A32_SINT
#define PRIMITIVE_SIZE	32
#define	TEXTURE_SIZE	32

#define NUM_COLOR_SETS	3
#define NUM_COLORS		4

#define FORMATS_ARRAY_SIZE		6
#define FORMATS_MIP_LEVELS		6
#define	FORMATS_DATA_ELEMENTS	6

#define MAX_WIDTH		128
#define MAX_HEIGHT		256
#define MAX_RESOURCE_SIZE	MAX_WIDTH * MAX_HEIGHT * 16

enum D3D_RESOURCE_SCENARIO
{
	D3D_RESOURCE_SCENARIO_BUFFER,
	D3D_RESOURCE_SCENARIO_TEXTURE1D,
	D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY,
	D3D_RESOURCE_SCENARIO_TEXTURE2D,
	D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY,
	D3D_RESOURCE_SCENARIO_TEXTURE2D_MS,
	D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY,
	D3D_RESOURCE_SCENARIO_TEXTURECUBE,
	D3D_RESOURCE_SCENARIO_TEXTURE3D
};

#define	D3D_RS_FLAGS_BUFFER			1
#define	D3D_RS_FLAGS_TEXTURE1D		2
#define	D3D_RS_FLAGS_TEXTURE2D		4
#define	D3D_RS_FLAGS_TEXTURE3D		8
#define	D3D_RS_FLAGS_TEXTURECUBE	16
#define	D3D_RS_FLAGS_ARRAY			32
#define D3D_RS_FLAGS_MS				64

enum IMAGE_PATTERN	
{
	IP_STRIPES,
	IP_GRADIENT,
	IP_TILES
};

class CResourceFormatsTest : public CD3D11Test
{
public:
	CResourceFormatsTest( ) :
		m_pVS( NULL ),
		m_pGS( NULL ),
		m_pPS( NULL ),
		m_pTextureRT( NULL ),
		m_pTextureRTCopy( NULL ),
		m_pConstBuffer( NULL ),
		m_pRTView( NULL ),
		m_pRSEnabled( NULL ),
		m_pRSDisabled( NULL ),
		m_pVertexBuffer( NULL ),
		m_pLayout( NULL ),
		m_pSampler( NULL ),
		m_pSRV( NULL ),
		m_nPrimitiveSize( PRIMITIVE_SIZE ),
		m_Format( FORMAT_TEXTURE ),
		m_FormatRT( FORMAT_RT_FLOAT )
	{
	}

	virtual void			InitTestParameters( );
	virtual TEST_RESULT		Setup( );
	virtual void			Cleanup( );

	virtual TEST_RESULT		SetupTestCase( );
	virtual TEST_RESULT		ExecuteTestCase( );
	virtual void			CleanupTestCase( );
	virtual bool			CompareResults( UINT uSlice, void *pOutput, UINT uPitch );
	virtual bool			GetMipDimensions( UINT uMipSlice, UINT &uWidth, UINT &uHeight, UINT &uDepth );
	virtual float			FloatToSRGB( float fSRGBVal, int nBits );

	virtual bool			WithinOneULP( float fReference, float fOutput );
	virtual bool			IsDenorm( UINT32 uValue, UINT32 uBits );
	virtual bool			IsDenorm( float fValue );
	virtual bool			IsNaN( float fValue );


	virtual TEST_RESULT		SetupGeometry( );
	virtual TEST_RESULT		SetupShaders( );

	virtual TEST_RESULT		CreateShaderResources( );
	virtual bool			CreateRenderTargets( );
	virtual DXGI_FORMAT		GetTypelessFormat( DXGI_FORMAT format );
	virtual DXGI_FORMAT		GetRTViewFormat( const DXGI_FORMAT &resourceFormat );
	virtual DXGI_FORMAT		GetSRViewFormat( const DXGI_FORMAT &resourceFormat );

	virtual bool			EncodeFormat( FLOAT_COLOR *pOriginal, void *pEncoded, const UINT &uWidth, const UINT &uHeight ) { return true; }
	virtual bool			DecodeFormat( void *pEncoded, FLOAT_COLOR *pDecoded, const UINT &uWidth, const UINT &uHeight ) { return true; }
	virtual void			GetFormatsFactor( testfactor::RFactor *pRFactor );

protected:
	int							m_nPrimitiveSize;	
	DXGI_FORMAT					m_Format;
	DXGI_FORMAT					m_FormatRT;
	D3D11_SAMPLER_DESC			m_SamplerDesc;

	ID3D11VertexShader			*m_pVS;
	ID3D11GeometryShader		*m_pGS;
	ID3D11PixelShader			*m_pPS;

	ID3D11Resource				*m_pTextureSR;
	ID3D11Texture2D				*m_pTextureRT;
	ID3D11Texture2D				*m_pTextureRTCopy;
	ID3D11Buffer				*m_pConstBuffer;
	ID3D11RenderTargetView		*m_pRTView;
	ID3D11RasterizerState		*m_pRSEnabled;
	ID3D11RasterizerState		*m_pRSDisabled;
	ID3D11Buffer				*m_pVertexBuffer;
	D3DXMATRIX					m_mxTrans;

	ID3D11InputLayout			*m_pLayout;
	ID3D11SamplerState			*m_pSampler;
	ID3D11ShaderResourceView	*m_pSRV;

	// Test parameters
	IMAGE_PATTERN				m_ImagePattern;
	UINT32						m_uNumStripes;
	bool						m_bVertStripes;
	UINT32						m_uTileSize;
	UINT32						m_uWidth;
	UINT32						m_uHeight;
	UINT32						m_uDepth;
	D3D_RESOURCE_SCENARIO		m_ResourceScenario;
	UINT32						m_uMipSlice;
	UINT32						m_uMipLevels;
	UINT32						m_uArraySlice;
	bool						m_bUseTypeless;
	bool						m_bOneMip;

	BYTE						m_pResourceData[ FORMATS_DATA_ELEMENTS ][ MAX_RESOURCE_SIZE ];
	BYTE						m_pColorsTemp[ FORMATS_DATA_ELEMENTS ][ MAX_RESOURCE_SIZE ];

	static FLOAT_COLOR			m_ColorSets [ NUM_COLOR_SETS ] [ NUM_COLORS ];

	struct TLVERTEX
    {
        float	vPosition[2];
        float	vTexCoord[4];
    };
};

class CDepthReadbackTest : public CResourceFormatsTest
{
public:
	void					InitTestParameters();
	TEST_RESULT				SetupTestCase();
	void					CleanupTestCase();
	TEST_RESULT				ExecuteTestCase();

	DXGI_FORMAT				GetRGBFormat( DXGI_FORMAT depthFormat );

private:
	ID3D11DepthStencilView		*m_pDSV;
	D3D11_VIEWPORT			m_Viewport;
};

class CCheckFormatSupportTest : public CD3D11Test
{
public:
	CCheckFormatSupportTest( ) :
		m_Format( FORMAT_TEXTURE ),
		m_pAdapterDescriptor( nullptr ),
		m_wddmDriverVersion( KMT_DRIVERVERSION_WDDM_1_0 )
	{
	}

	virtual void			InitTestParameters( );
	virtual TEST_RESULT		Setup( );
	virtual void			Cleanup( );

	virtual TEST_RESULT		ExecuteTestCase( );

protected:
	DXGI_FORMAT					m_Format;
	DevX::GRFX::AdapterDescriptorCore   *m_pAdapterDescriptor;
    D3DKMT_DRIVERVERSION            m_wddmDriverVersion;

};

class CResourceFormatsTestApp : public CD3D11TestFramework
{
private:
	TESTTYPE	GetTestType( ) const { return TESTTYPE_CONF; }
	void		InitOptionVariables( );
	bool		InitTestGroups( );

protected:
	bool		m_bLogShaders;

public:
	bool				IsDebugRuntime( ) const { return m_D3DOptions.Debug; }
	bool				IsLoggingShaders( ) const { return m_bLogShaders; }
	D3D_DRIVER_TYPE	GetSrcDriverType( ) const { return m_D3DOptions.DriverType; }
};

extern CResourceFormatsTestApp g_TestApp;
