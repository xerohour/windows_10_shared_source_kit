#pragma once

#include "Arithmetic.h"

enum CONV_OP
{
	CO_F32TOF16,
	CO_F16TOF32,
	CO_FTOI,
	CO_FTOU,
	CO_ITOF,
	CO_UTOF,
	CO_DTOF,
	CO_FTOD,
};

DECLARE_NAMED_VALUES( CONV_OP );

//---------------------------------------------------------------

template< class TDest, class TSrc >
CMinPrecValue<TDest> Convert( const CMinPrecValue<TSrc> & A );
CMinPrecValue<UINT> f32tof16( const CMinPrecValue<float> & A );
CMinPrecValue<float> f16tof32( const CMinPrecValue<UINT> & A );

//---------------------------------------------------------------

class CConvertPrecisionTest : public CArithmeticPrecisionTest
{
public:
	CConvertPrecisionTest() {}

	virtual TEST_RESULT		Setup();
	virtual void			InitTestParameters();
	virtual DXGI_FORMAT		GetRenderTargetFormat();
	virtual bool			CreateShaders();

	virtual TEST_RESULT		ValidateResult();

	template< class TDest, class TSrc >
	TEST_RESULT ValidateResultWithTemplates();
	string OperationToString( string pSrc0 );
	char* GetTypeName( CArithmeticPrecisionTest::EValueType type );
	virtual void* GetSrcData() ;

	template< class TDest, class TSrc >
	CMinPrecValue<TDest> PerformOperation( CONV_OP op, CMinPrecValue<TSrc>& a );
	template< class T >
	void ApplySourceModifier( SRC_MOD_TYPE ModType, CMinPrecValue<T>& a );
	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor() { testfactor::RFactor rEmpty; return rEmpty; }

	EValueType GetInterShaderValueType();
	EValueType GetShaderOperationDestValueType();
	EValueType GetShaderOperationSrcValueType();

protected:
	Vec4<double>				m_SrcDouble[3];

	CONV_OP						m_ConvOp;
	D3D_MIN_PRECISION			m_DestMinPrecision;
};

