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

#define NUM_COLORS			4
#define NUM_COLOR_SETS		3
#define NUM_RESOURCES		3
#define NUM_ARRAY_SLICES	18

#define MAX_HEIGHT			256
#define MAX_WIDTH			128

#define MAX_RESOURCE_SIZE	MAX_WIDTH * MAX_HEIGHT * 16

#if !defined(PASTER2)
#define PASTER2(x,y) x ## y
#endif

#if !defined(PASTER)
#define PASTER(x,y) PASTER2(x,y)
#endif

#if !defined(UNICODIFY)
#define UNICODIFY( x ) PASTER(L,x)
#endif

#if !defined(__FILEW__)
#define __FILEW__ UNICODIFY(__FILE__)
#endif

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
	D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY,
	D3D_RESOURCE_SCENARIO_TEXTURE3D
};

enum IMAGE_PATTERN	
{
	IP_STRIPES,
	IP_GRADIENT,
	IP_TILES
};

#define	D3D_RS_FLAGS_BUFFER			1
#define	D3D_RS_FLAGS_TEXTURE1D		2
#define	D3D_RS_FLAGS_TEXTURE2D		4
#define	D3D_RS_FLAGS_TEXTURE3D		8
#define	D3D_RS_FLAGS_TEXTURECUBE	16
#define	D3D_RS_FLAGS_ARRAY			32
#define D3D_RS_FLAGS_MS				64

extern bool GetScenarioFlags( D3D_RESOURCE_SCENARIO scenario, UINT *pFlags );
extern bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_SRV_DIMENSION *pSRVDim );
extern bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_RTV_DIMENSION *pRTVDim );
extern bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_DSV_DIMENSION *pDSVDim );

extern tstring __stdcall ToString( const D3D_RESOURCE_SCENARIO &flags );
extern bool __stdcall FromString( D3D_RESOURCE_SCENARIO *pFlags, const tstring &str );
extern tstring __stdcall ToString( const IMAGE_PATTERN &pattern );
extern bool __stdcall FromString( IMAGE_PATTERN *pPattern, const tstring &str );

extern bool	WithinOneULP( float fReference, float fOutput );

struct TLVERTEX
{
	float	vPosition[2];
	float	vTexCoord[4];
};

class CRenderTargetsTest : public CD3D11Test
{
public:
	CRenderTargetsTest( ) :
		 m_uNumSlices( 3 ),
		 m_pRTCurrent( NULL ),
		 m_pConstBuffer( NULL ),
		 m_pRSEnabled( NULL ),
		 m_pRSDisabled( NULL ),
		 m_pVertexBuffer( NULL ),
		 m_pLayout( NULL ),
		 m_pSampler( NULL ),
		 m_pRTView( NULL ),
		 m_pShaderResource( NULL ),
		 m_pSRView( NULL ),
		 m_pVS( NULL ),
		 m_pGS( NULL ),
		 m_pPS( NULL ),
		 m_pPS_SR_Readback( NULL ),
		 m_bLastFaces( false ),
		 m_bSwapChain( false ),
		 m_bFL9X( false ),
		 m_pAdapterDescriptor( nullptr ),
		 m_wddmDriverVersion( KMT_DRIVERVERSION_WDDM_1_0 )
	{
	}

	virtual void			InitTestParameters( );
	virtual TEST_RESULT		Setup( );
	virtual TEST_RESULT		SetupTestCase( );
	virtual void			Cleanup( );
	virtual void			CleanupTestCase( );
	virtual TEST_RESULT		ExecuteTestCase( );

	virtual bool			CompareResults( void *pResultData, UINT32 nSubresource, UINT uRowPitch, UINT uDepthPitch );

	virtual bool			CopyResource( ID3D11Resource *pResSource, ID3D11Resource **pResDest, D3D_RESOURCE_SCENARIO resType, bool bRead );
	virtual bool			LockResource( ID3D11Resource *pResource, UINT32 nSubresource, D3D_RESOURCE_SCENARIO resType, void **ppData, UINT &uRowPitch, UINT &uDepthPitch );
	virtual bool			UnLockResource( ID3D11Resource *pResource, UINT32 nSubresource, D3D_RESOURCE_SCENARIO resType );

	virtual bool			CreateShaderResources( );
	virtual bool			CreateRenderTargets( );
	virtual bool			SetupShaders(bool bForceFloatInput = false );
	virtual bool			IsValidFL9TestCase() const;

	static DXGI_FORMAT		GetRTViewFormat( const DXGI_FORMAT &resourceFormat );
	static DXGI_FORMAT		GetSRViewFormat( const DXGI_FORMAT &resourceFormat );
	static bool				GetMipDimensions( UINT uMipSlice, UINT &uWidth, UINT &uHeight, UINT &uDepth );
	static bool				WithinOneULP( float fReference, float fOutput );
	static bool				IsDenorm( float fValue );
	static bool				IsNaN( float fValue );
	
protected:

	DXGI_FORMAT					m_Format;
	DXGI_FORMAT					m_FormatRT;
	D3D11_SAMPLER_DESC			m_SamplerDesc;

	ID3D11VertexShader				*m_pVS;
	ID3D11GeometryShader				*m_pGS;
	ID3D11PixelShader				*m_pPS;
	ID3D11PixelShader				*m_pPS_SR_Readback;

	ID3D11Resource					*m_pRTCurrent;
	ID3D11Texture2D					*m_pShaderResource;
	ID3D11Buffer						*m_pConstBuffer;
	ID3D11RenderTargetView			*m_pRTView;
	ID3D11RasterizerState			*m_pRSEnabled;
	ID3D11RasterizerState			*m_pRSDisabled;
	ID3D11DepthStencilState			*m_pDepthStencilState;
	ID3D11Buffer						*m_pVertexBuffer;

	ID3D11InputLayout				*m_pLayout;
	ID3D11SamplerState				*m_pSampler;
	ID3D11ShaderResourceView			*m_pSRView;

	BYTE						m_pResourceData[ NUM_RESOURCES ][ MAX_RESOURCE_SIZE ];
	BYTE						m_pResourceDataCopy[ NUM_RESOURCES ][ MAX_RESOURCE_SIZE ];
	TLVERTEX					m_pVertexData[ 4 ];
	D3DXMATRIX					m_mxTrans;
	bool						m_bLastFaces;
	bool						m_bSwapChain;

	bool						m_bFL9X;
	DevX::GRFX::AdapterDescriptorCore   *m_pAdapterDescriptor;
    D3DKMT_DRIVERVERSION            m_wddmDriverVersion;

	// Test parameters
	IMAGE_PATTERN				m_ImagePattern;
	UINT32						m_uNumStripes;
	bool						m_bVertStripes;
	UINT32						m_uTileSize;
	UINT32						m_uWidth;
	UINT32						m_uHeight;
	UINT32						m_uDepth;
	D3D_RESOURCE_SCENARIO		m_ResourceType;
	UINT32						m_uNumSlices;
	UINT32						m_uMipLevel;
	UINT32						m_uRTIndex;
	D3D11_BIND_FLAG				m_BindFlag;
	bool						m_bCubeArray;

	static FLOAT_COLOR			m_ColorSets [ NUM_COLOR_SETS ] [ NUM_COLORS ];

	static char					m_szVSCode [ ];
	static char					m_szPSCode [ ];
};

class CRenderTargetsTestApp : public CD3D11TestFramework
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
	D3D_DRIVER_TYPE		GetSrcDriverType( ) const { return m_D3DOptions.DriverType; }
};

