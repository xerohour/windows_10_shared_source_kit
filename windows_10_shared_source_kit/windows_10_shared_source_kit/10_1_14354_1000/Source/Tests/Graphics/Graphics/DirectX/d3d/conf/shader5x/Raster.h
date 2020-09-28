//////////////////////////////////////////////////////////////////////////
//  Raster.h
//  created:	2005/03/10
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"

#include "new_off.h"
#include "d3dx10math.h"
#include "new_on.h"

#define	PRIMITIVE_SIZE		32
#define	TEXTURE_FORMAT		DXGI_FORMAT_R32G32B32A32_FLOAT
#define RANDOM_MAX			1024
#define MAX_STR				4000

struct S_Color
{
	float a, b, g, r;

	S_Color operator=( const S_Color &rhs )
	{
		a = rhs.a;
		b = rhs.b;
		g = rhs.g;
		r = rhs.r;

		return rhs;
	}

	S_Color operator*( const S_Color &rhs )
	{
		S_Color res;

		res.a = a * rhs.a;
		res.b = b * rhs.b;
		res.b = g * rhs.g;
		res.r = r * rhs.r;

		return res;
	}

	S_Color operator-( const S_Color &rhs )
	{
		S_Color res;

		res.a = a - rhs.a;
		res.b = b - rhs.b;
		res.b = g - rhs.g;
		res.r = r - rhs.r;

		return res;
	}
};

class CShader5xTest_discard : public CShader5xTest
{

};

class CShader5xTest_deriv : public CShader5xTest
{
public:
	CShader5xTest_deriv( );

	virtual void		InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual void		Cleanup();
	virtual TEST_RESULT	SetupTestCase();
	virtual void		CleanupTestCase();
	virtual TEST_RESULT ExecuteTestCase();

	virtual bool		SupportsFeatureLevel( D3D_FEATURE_LEVEL featureLevel ) = 0;
	virtual TCHAR *		GetInstName( D3D_FEATURE_LEVEL featureLevel ) = 0;
	virtual bool		CreateGeometry( );
	virtual TEST_RESULT	SetupTextures( );
	virtual bool		CalculateResult( );
	virtual bool		CompareResults( void *pOutput, const SIZE_T &uPitch );
	virtual bool		IsNaN( float fValue );


protected:
    DXGI_FORMAT				m_Format;
    DXGI_SAMPLE_DESC		m_SampleDesc;
	SRC_MOD_TYPE			m_SrcMod;
	INST_MOD_TYPE			m_InstMod;
	float					m_fDerivative;
	UINT32					m_nDerivIndex;
	int						m_nPrimitiveSize;

	ID3D11Texture2D				*m_pTexture;
	ID3D11Texture2D				*m_pTextureRT;
	ID3D11Texture2D				*m_pTextureRTCopy;
	ID3D11Buffer				*m_pBufferTL;
	ID3D11InputLayout			*m_pLayoutTL;
	ID3D11SamplerState			*m_pSampler;
	ID3D11ShaderResourceView	*m_pSRV;
	ID3D11RenderTargetView		*m_pRTV;
	ID3D11VertexShader			*m_pVShader;
	ID3D11GeometryShader		*m_pGShader;
	ID3D11PixelShader			*m_pPShader;

	float					*m_pExpectedResult;
	BYTE					*m_pResourceData;
	D3DXMATRIX				m_MatWorld;

	struct TLVERTEX
    {
        float  position[2];
        float texcoord[2];
    };

	static char				m_szVSCodeBegin [ ];
	static char				m_szVSCodeEnd [ ];
	static char				m_szPSCodeBegin [ ];
	static char				m_szPSCodeEnd [ ];

};

class CShader5xTest_deriv_rtx_coarse : public CShader5xTest_deriv
{
	virtual bool		SupportsFeatureLevel( D3D_FEATURE_LEVEL featureLevel )
	{
		return (featureLevel >= D3D_FEATURE_LEVEL_10_0);
	}
	virtual TCHAR *		GetInstName( D3D_FEATURE_LEVEL featureLevel )
	{ 
		if (featureLevel >= D3D_FEATURE_LEVEL_11_0)
			return _T( "deriv_rtx_coarse" );
		else
			return _T( "deriv_rtx" ); 
	}
};

class CShader5xTest_deriv_rty_coarse : public CShader5xTest_deriv
{
	virtual bool		SupportsFeatureLevel( D3D_FEATURE_LEVEL featureLevel )
	{
		return (featureLevel >= D3D_FEATURE_LEVEL_10_0);
	}
	virtual TCHAR *		GetInstName( D3D_FEATURE_LEVEL featureLevel ) 
	{ 
		if (featureLevel >= D3D_FEATURE_LEVEL_11_0)
			return _T( "deriv_rty_coarse" );
		else
			return _T( "deriv_rty" );
	}
};

class CShader5xTest_deriv_rtx_fine : public CShader5xTest_deriv
{
	virtual bool		SupportsFeatureLevel( D3D_FEATURE_LEVEL featureLevel )
	{
		return (featureLevel >= D3D_FEATURE_LEVEL_11_0);
	}
	virtual TCHAR *		GetInstName( D3D_FEATURE_LEVEL featureLevel ) { return _T( "deriv_rtx_fine" ); }
};

class CShader5xTest_deriv_rty_fine : public CShader5xTest_deriv
{
	virtual bool		SupportsFeatureLevel( D3D_FEATURE_LEVEL featureLevel )
	{
		return (featureLevel >= D3D_FEATURE_LEVEL_11_0);
	}
	virtual TCHAR *		GetInstName( D3D_FEATURE_LEVEL featureLevel ) { return _T( "deriv_rty_fine" ); }
};
