#pragma once

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <dxmini.h>
#include <d3dkmddi.h>
#include <d3dkmthk.h>
#include <atlbase.h>
#include "AdapterUtil.h"

#include "D3D11Test.h"
#include "new_off.h"
#include "d3dx10.h"
#include "new_on.h"
#include "d3d11internaltest.h"

#include <WGFTestCore.h>

#include "FormatHelper.h"
#include "Direct3DFailureCategoryStrings.h"
using namespace WindowsTest::Graphics;

using namespace std;
#include <map>

#include "strsafe.h"

#define RA_ARRAYSIZE		1
#define RA_MIPLEVELS		1
#define RA_NUMSUBRES		RA_ARRAYSIZE * RA_MIPLEVELS
#define	RA_DATA_ELEMENTS	6
#define RA_NUMVERTICES		1000

#define NUM_COLOR_SETS		3
#define NUM_COLORS			4

#define MAX_WIDTH			128
#define MAX_HEIGHT			256
#define MAX_DEPTH			10
#define MAX_ARRAYSIZE		512
#define MAX_RESOURCE_SIZE	MAX_WIDTH * MAX_WIDTH * 16

#define DONOTWAIT_ATTEMPTS	50	
#define DONOTWAIT_DELAY		50
#define NUM_LARGE_COPIES	30

#define INFP_FLOAT32		0x7f800000
#define INFN_FLOAT32		0xff800000
#define INFP_FLOAT16		0x00007c00
#define INFN_FLOAT16		0x0000fc00
#define INFP_FLOAT11		0x000007c0
#define INFN_FLOAT11		0x00000000
#define INFP_FLOAT10		0x000003e0
#define INFN_FLOAT10		0x00000000

#define DENORM_FLOAT32		0x00000001
#define DENORM_FLOAT16		0x00000001
#define DENORM_FLOAT11		0x00000001
#define DENORM_FLOAT10		0x00000002

#define NAN_FLOAT32			0x7f800001
#define NAN_FLOAT16			0x00007c01
#define NAN_FLOAT11			0x000007c1
#define NAN_FLOAT10			0x000007c2

#define MAX_FLOAT16			99999999.f
#define MIN_FLOAT16			-99999999.f
#define MAX_FLOAT11			99999999.f
#define MIN_FLOAT11			0.f
#define MAX_FLOAT10			99999999.f
#define MIN_FLOAT10			0.f

#define DESC_TEST_COPYCOMPRESSED	"Normally, the source and destination of a copy operation need to have texture formats in " \
	"the same typeless family.  D3D10.1 introduced the ability to copy between compressed and structured formats in order to " \
	"facilitate algorithms involving compressed textures.  This test verifies this behavior and should only run on " \
	"feature level 10.1 or higher."

#define DESC_TEST_DISCARD	"D3D11.1 has features to improve performance accross a wide range of devices. One new feature is Discard. " \
	"D3D11.1 provides two new API's: DiscardResource() and DiscardView(), this is to make it easy for developers to indicate they no longer need " \
	"a resource or set of subresources. This is essentially a hint to the driver so it can optimize how the resource is used."

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
	D3D_RESOURCE_SCENARIO_TEXTURE3D,
};

#define BYTE_ALIGNED_16				16

#define	D3D_RS_FLAGS_BUFFER			1
#define	D3D_RS_FLAGS_TEXTURE1D		2
#define	D3D_RS_FLAGS_TEXTURE2D		4
#define	D3D_RS_FLAGS_TEXTURE3D		8
#define	D3D_RS_FLAGS_TEXTURECUBE	16
#define	D3D_RS_FLAGS_ARRAY			32
#define D3D_RS_FLAGS_MS				64

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

enum ACCESS_METHOD
{
	AM_COPY,
	AM_COPYREGION,
	AM_UPDATE,
	AM_MAP
};

enum IMAGE_PATTERN	
{
	IP_STRIPES,
	IP_GRADIENT,
	IP_TILES
};

enum TEST_COPY_FLAGS
{
	D3D11_COPY_NONE	 = 0x0,
	D3D11_COPY_NO_OVERWRITE_TEST = 0x1,
	D3D11_COPY_DISCARD_TEST = 0x2,
	D3D11_COPY_TILEABLE = 0x4,
	D3D11_COPY_FORCE_DWORD = 0xFFFFFFFF
};

struct Point
{
	UINT x, y, z;

	bool operator==( const Point &rhs ) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	bool operator !=( const Point &rhs ) const { return !(*this == rhs); }
};

struct D3D_TEST_BOX : public D3D11_BOX
{
	D3D_TEST_BOX( ) { }

	D3D_TEST_BOX( UINT xS, UINT xE, UINT yS, UINT yE, UINT zS, UINT zE )	
	{
		left = xS; right = xE;
		top = yS; bottom = yE;
		front = zS; back = zE;
	}

	bool operator ==( const D3D11_BOX &rhs ) const
	{
		return
			left == rhs.left && right == rhs.right &&
			top == rhs.top && bottom == rhs.bottom &&
			front == rhs.front && back == rhs.back;
	}

	bool operator !=( const D3D11_BOX &rhs ) const { return !(*this == rhs); }

	UINT Width() const { return right - left; }
	UINT Height() const { return bottom - top; }
};

struct TexDataInfo
{
	DXGI_FORMAT Format;
	UINT		Width;
	UINT		Height;
	UINT		ColorSet;
};

extern bool operator <(const TexDataInfo &left, const TexDataInfo &right);

extern tstring __stdcall ToString( const D3D_RESOURCE_SCENARIO &scenario );
extern bool __stdcall FromString( D3D_RESOURCE_SCENARIO *pScenario, const tstring &str );

extern bool GetScenarioFlags( D3D_RESOURCE_SCENARIO scenario, UINT *pFlags );
extern bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_DSV_DIMENSION *pDSVDim );
extern bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_RTV_DIMENSION *pRTVDim );
extern bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_SRV_DIMENSION *pSRVDim );
extern bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_UAV_DIMENSION *pUAVDim );

class CResourceAccessTest : public CD3D11Test
{
public:
	CResourceAccessTest( ) :
		m_uArraySize( RA_ARRAYSIZE ),
		m_pAdapterDescriptor( NULL ),
		m_pSrcResource( NULL ),
		m_pDestResource( NULL ),
		m_pSrcPreCopyResource( NULL ),
		m_pQuery( NULL ),
		m_SrcFormat( DXGI_FORMAT_UNKNOWN ),
		m_CopyFlags( 0 )
	{ 
	}

	struct TilingVertex
	{
		float pos[2];
		float tex[2];
	};

	virtual void			InitTestParameters( );
	virtual TEST_RESULT		Setup( );
	virtual void			Cleanup( );

	virtual TEST_RESULT		SetupTestCase( );
	virtual TEST_RESULT		ExecuteTestCase( );
	virtual void			CleanupTestCase( );

	virtual TEST_RESULT		CreateResources( );
	virtual TEST_RESULT		ValidateResult( );
	virtual bool			TestMap( );

	virtual bool			CompareSubresources( void *pSrcData, UINT uSrcRowPitch, UINT uSrcDepthPitch, void *pDestData, UINT uDestRowPitch, UINT uDestDepthPitch, void *pSrcPreCopyData, UINT uSrcPreCopyRowPitch, UINT uSrcPreCopyDepthPitch, UINT uSubres, UINT iDestSubres ) const;
	virtual bool			GetMipDimensions( UINT uMipSlice, UINT &uWidth, UINT &uHeight, UINT &uDepth ) const;
	virtual bool			WithinOneULP( float fReference, float fOutput );
	virtual bool			IsDenorm( float fValue );
	virtual bool			IsNaN( float fValue );
	virtual bool			WithinBox( const D3D_TEST_BOX &box, UINT x, UINT y, UINT z ) const;

	virtual DXGI_FORMAT		GetTypelessFormat( DXGI_FORMAT format );
	bool					CheckMapIsByteAligned( D3D11_MAPPED_SUBRESOURCE* pMapData );
	bool					CheckNByteAlignedPointer(UINT n, void * ptr);
	bool					IsSupportedBufferScenario( const D3D11_BUFFER_DESC* bufferDesc );
	bool					TestTileableCopy() const;
	bool					TestOverlappingCopy() const;
	bool					TestSwapbuffer() const;
	bool					EightBitUNORMFormat() const;

	HRESULT UpdateSubresource_Workaround(ID3D11Resource *pDstResource,
										 UINT dstSubresource,
										 const D3D11_BOX *pDstBox,
										 const void *pSrcData,
										 UINT srcBytesPerElement,
										 UINT srcRowPitch,
										 UINT srcDepthPitch,
										 bool* pDidWorkAround);

	HRESULT RunLargeCommandList();

protected:
	DXGI_FORMAT					m_SrcFormat;
	DXGI_FORMAT					m_DestFormat;
	DXGI_FORMAT					m_SrcFormatSame;
	DXGI_FORMAT					m_DestFormatSame;
	UINT						m_uSrcSubres;
	UINT						m_uDestSubres;
	UINT						m_uArraySize;
	UINT32						m_uWidth;
	UINT32						m_uHeight;
	UINT32						m_uDepth;
	ACCESS_METHOD				m_AccessMethod;
	D3D_RESOURCE_SCENARIO		m_ResourceScenario;
	D3D_TEST_BOX				m_SrcBox;
	Point						m_DestPoint;
	D3D11_MAP					m_MapType;
	D3D11_USAGE					m_MapUsage;
	bool						m_bSameResource;
	bool						m_bDoNotWait;
	bool						m_bNULLSrcBox;
	bool						m_bDepthResource;
	D3D11_BIND_FLAG				m_BindFlags;
	UINT						m_CopyFlags;
	bool						m_bTextureReadback;
	D3D11_CPU_ACCESS_FLAG		m_CPUAccessFlags;

	bool						m_bBC;
	
	ATL::CComPtr<ID3D11DeviceTest>      m_pDevTest;
	DevX::GRFX::AdapterDescriptorCore   *m_pAdapterDescriptor;
	D3DKMT_DRIVERVERSION                m_wddmDriverVersion;

	ID3D11Resource					*m_pSrcResource;
	ID3D11Resource					*m_pDestResource;
	ID3D11Resource					*m_pSrcPreCopyResource;	// CopySubresourceRegion can support an overlapping copy
	ID3D11Query						*m_pQuery;
	ATL::CComPtr<ID3D11ShaderResourceView>	m_pTilingSRV;
	ATL::CComPtr<ID3D11RenderTargetView>	m_pTilingRTV;
	ATL::CComPtr<ID3D11RasterizerState>		m_pTilingRS;
	ATL::CComPtr<ID3D11Buffer>				m_pTilingVB;
	ATL::CComPtr<ID3D11SamplerState>		m_pTilingSampler;
	ATL::CComPtr<ID3D11PixelShader>			m_pTilingPS;
	ATL::CComPtr<ID3D11PixelShader>			m_pTilingPS_Array;
	ATL::CComPtr<ID3D11PixelShader>			m_pTilingPS_Invert;
	ATL::CComPtr<ID3D11PixelShader>			m_pTilingPS_InvertArray;
	ATL::CComPtr<ID3D11InputLayout>			m_pTilingIL;
	ATL::CComPtr<ID3D11VertexShader>		m_pTilingVS;


	static Point				m_pPoints[ ];
	static Point				m_pPointsWithDepth[ ];
	static D3D_TEST_BOX			m_pBoxes[ ];
	static D3D_TEST_BOX			m_pBoxesWithDepth[ ];
	static FLOAT_COLOR			m_ColorSets [ NUM_COLOR_SETS ] [ NUM_COLORS ];

	BYTE						m_pResourceData[ RA_DATA_ELEMENTS ][ MAX_RESOURCE_SIZE ];
	BYTE						m_pTagData[ MAX_RESOURCE_SIZE ];
	map<TexDataInfo,BYTE*>		m_TexCache;
};

struct MSAAQualities
{
	UINT    Count;
	UINT    Qualities;
};
struct MSAACapabilities
{
	UINT    FormatSupportBits;
	std::vector< MSAAQualities > Qualities;
};
typedef std::map< DXGI_FORMAT, MSAACapabilities > TMSAACaps;

extern const LONG Edge0;
extern const LONG Edge1;

class CResourceAccessTestApp : public CD3D11TestFramework
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
	TMSAACaps                   m_MSAACaps;
};

struct TLVERTEX
{
	float	vPosition[2];
	float	vTexCoord[4];
};

#include "ClearView.h"
#include "ClearView.inl"
