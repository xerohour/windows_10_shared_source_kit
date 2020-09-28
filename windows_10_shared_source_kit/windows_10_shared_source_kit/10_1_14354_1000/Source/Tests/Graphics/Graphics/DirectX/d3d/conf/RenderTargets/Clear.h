#pragma once
#include "RenderTargets.h"

#include "D3D11Test.h"
#include "d3d10_1.h"
#include "new_off.h"
#include "d3dx10.h"
#include "new_on.h"

#define INF_P		0x7f800000
#define INF_N		0xff800000
#define NAN			0x7f800001
#define	DENORM		0x00000001

#define CRTV_TEXDIM			32
#define CRTV_MAXELEMENTSIZE	16

#include "D3D11Test.h"
#include "FormatHelper.h"

class CClearRenderTargetViewTest : public CD3D11Test
{
public:
	CClearRenderTargetViewTest( ) :
		m_pRTVSubset( NULL ),
		m_pTexture( NULL ),
		m_pTextureCopy( NULL ),
		m_pAdapterDescriptor( NULL )
	{ }

	void		InitTestParameters( );
	TEST_RESULT Setup( );
	TEST_RESULT	SetupTestCase( );
	void		CleanupTestCase( );
	TEST_RESULT	ExecuteTestCase( );

	bool		CompareResults( void *pData, 
								UINT x, UINT y,
								UINT uWidth, UINT uHeight,
								UINT uRowPitch,
								UINT uSubresource, UINT uWSlice,
								FLOAT_COLOR* pClearValues );

	bool        CompareResultsBuffer( UINT uSubsetX, UINT uSubsetWidth, UINT uFullWidth, 
									  FLOAT_COLOR* pExpectedInBounds, FLOAT_COLOR* pExpectedOutOfBounds );

	bool        CompareResultsTexture( UINT uArraySize, UINT uMipLevels,
									   UINT uWidth, UINT uHeight, UINT uDepth,
									   UINT uRTVMipSlice, UINT uRTVFirstArraySlice, UINT uRTVArraySize,
									   UINT uRTVFirstWSlice, UINT uRTVWSize,
									   FLOAT_COLOR* pExpectedInBounds, FLOAT_COLOR* pExpectedOutOfBounds );

	bool		IsValidFL9TestCase( ) const;

protected:
	D3D_RESOURCE_SCENARIO	m_ResourceType;
	DXGI_FORMAT				m_Format;
	float					m_fClearVal;
	tstring					m_szSlots;
	UINT                    m_uClearOffset[3];
	
	ID3D11RenderTargetView		*m_pRTVSubset;
	ID3D11Resource				*m_pTexture;
	ID3D11Resource				*m_pTextureCopy;

	bool							m_bFL9X;
	DevX::GRFX::AdapterDescriptorCore   *m_pAdapterDescriptor;
    D3DKMT_DRIVERVERSION            m_wddmDriverVersion;
};