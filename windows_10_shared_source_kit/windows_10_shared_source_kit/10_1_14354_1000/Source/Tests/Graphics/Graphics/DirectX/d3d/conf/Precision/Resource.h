#pragma once

#include "Arithmetic.h"

enum RES_OP
{
	RO_SAMPLE,
	RO_GATHER,
	RO_LD,
	RO_LD2DMS,
	RO_BUFINFO,
	RO_RESINFO,
	RO_RESINFO_FLOAT,
	RO_LDUAVTYPED,
};

DECLARE_NAMED_VALUES( RES_OP );

enum RES_SOURCE
{
	RS_SRV,
	RS_UAV,
};

DECLARE_NAMED_VALUES( RES_SOURCE );

static const UINT MAX_DEPTH = 4;

//---------------------------------------------------------------

class CResourcePrecisionTest : public CArithmeticPrecisionTest
{
public:
	CResourcePrecisionTest() : m_ArraySizeOrDepth(1) { m_InitData[0].pSysMem = m_InitData[2].pSysMem = NULL; }

	virtual TEST_RESULT		Setup();
	virtual void			InitTestParameters();
	virtual DXGI_FORMAT		GetRenderTargetFormat();
	virtual bool			CreateResources();
	virtual bool			CreateShaders();

	virtual TEST_RESULT		SetupTestCase();
	virtual TEST_RESULT		ExecuteTestCase();
	virtual void			CleanupTestCase();
	virtual TEST_RESULT		ValidateResult();

	string TextureDeclaration();
	CArithmeticPrecisionTest::EValueType TextureCoordType();
	template< class TDest, class TCoord >
	TEST_RESULT ValidateResultWithTemplates();
	template< class TDest,class TCoord >
	bool SampleTexture( CMinPrecValue<TCoord>& x, CMinPrecValue<TCoord>& y, CMinPrecValue<TCoord>& z, CMinPrecValue<TDest> ReadBack[4] );
	template< class TDest, class TCoord >
	bool GatherTexture( CMinPrecValue<TCoord>& x, CMinPrecValue<TCoord>& y, CMinPrecValue<TCoord>& z, CMinPrecValue<TDest> ReadBack[4] );
	template< class TDest, class TCoord >
	bool PerformOperation( CMinPrecValue<TCoord>& x, CMinPrecValue<TCoord>& y, CMinPrecValue<TCoord>& z, CMinPrecValue<TDest> Result[4] );
	string GetResinfoCoordinates();
	string GetSRVLoadCoordinates();
	string GetUAVLoadCoordinates();
	string OperationToString();
	char* GetTypeName( CArithmeticPrecisionTest::EValueType type );
	char* GetResourceTypeName( CArithmeticPrecisionTest::EValueType type );
	bool IsResinfoUintOperation();
	bool IsResinfoFloatOperation();
	bool IsLoadOperation();
	virtual void* GetSrcData() ;
	UINT GetSrcDataSize();

	bool InitBuffer();
	bool InitTexture1D();
	bool InitTexture2D();
	bool InitTexture3D();
	bool InitInitialData();

	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor() { testfactor::RFactor rEmpty; return rEmpty; }

	EValueType GetTextureType();
	EValueType GetInterShaderValueType();
	EValueType GetShaderOperationDestValueType();

protected:
	ATL::CComPtr<ID3D11Buffer>				m_pBuf;
	ATL::CComPtr<ID3D11Buffer>				m_pBufStaging;
	ATL::CComPtr<ID3D11Texture1D>			m_pTex1D;
	ATL::CComPtr<ID3D11Texture1D>			m_pTex1DStaging;
	ATL::CComPtr<ID3D11Texture2D>			m_pTex2D;
	ATL::CComPtr<ID3D11Texture2D>			m_pTex2DStaging;
	ATL::CComPtr<ID3D11Texture3D>			m_pTex3D;
	ATL::CComPtr<ID3D11Texture3D>			m_pTex3DStaging;
	ATL::CComPtr<ID3D11ShaderResourceView>	m_pSRV;
	ATL::CComPtr<ID3D11UnorderedAccessView>	m_pUAV;
	ATL::CComPtr<ID3D11SamplerState>		m_pSampler;

	UINT						m_Width;
	UINT						m_Height;
	UINT						m_ArraySizeOrDepth;
	D3D11_SUBRESOURCE_DATA		m_InitData[MAX_DEPTH];


	RES_OP						m_ResOp;
	D3D_MIN_PRECISION			m_DestMinPrecision;
	RES_SOURCE					m_ViewType;
	D3D_SRV_DIMENSION			m_Dimension;
	Vec4<float>					m_Texcoord;
	Vec4<UINT>					m_Ldcoord;
	DXGI_FORMAT					m_Format;
};

