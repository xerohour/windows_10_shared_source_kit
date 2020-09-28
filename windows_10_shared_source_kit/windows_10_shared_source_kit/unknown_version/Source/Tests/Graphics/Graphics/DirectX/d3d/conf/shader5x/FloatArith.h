//////////////////////////////////////////////////////////////////////////
//  FloatArith.h
//  created:	2005/03/10
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"

#include "new_off.h"
#include "d3dx10math.h"
#include "new_on.h"


struct FloatShaderParams
{
	D3DXVECTOR4 src0;
	D3DXVECTOR4 src1;
	D3DXVECTOR4 src2;
	D3DXVECTOR4 expected;
};

class CShader5xTest_FloatArith : public CShader5xTest
{
protected:
	typedef float ResultType[4];

	CShader5xTest_FloatArith();
	
	virtual TEST_RESULT Setup();
	virtual void Cleanup();
	
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	
	TEST_RESULT BuildShaders();
	
	static FLOAT_INPUT_CLASS FloatResultToFloatInput( FLOAT_RESULT_CLASS frc, float fSrc0, float fSrc1, float fSrc2 );
	static FLOAT_RESULT_CLASS FloatInputToFloatResult( FLOAT_INPUT_CLASS fic );
	static FLOAT_RESULT_CLASS FloatResultClassNegate( FLOAT_RESULT_CLASS frc );
	static FLOAT_INPUT_CLASS FloatInputClassNegate( FLOAT_INPUT_CLASS fic );
	
	bool CShader5xTest_FloatArith::CompareResults( int nResultIndex, ResultType &Result );

	virtual const char *GetOpCode() const = 0;
	virtual int GetNumDestRegisters() const { return 1; }
	virtual bool IsDestNull( int nIndex ) const { return false; }
	virtual int GetNumSrcRegisters() const { return 2; }
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const {}
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const {}
	virtual void CalcExpectedResult2( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const {}
	virtual void GetResultClass2( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const {}
	
	virtual bool IsReal( const float &f ) const;
	virtual bool CompareINFn( const float &f, int i ) const;
	virtual bool CompareINFp( const float &f, int i ) const;
	virtual bool CompareReal( const float &f, const float &fref, int i ) const;
	virtual bool CompareZEROn( const float &f, int i ) const;
	virtual bool CompareZEROp( const float &f, int i ) const;
	virtual bool CompareZERO( const float &f, int i ) const;
	virtual bool CompareONEn( const float &f, int i ) const;
	virtual bool CompareONEp( const float &f, int i ) const;
	virtual bool CompareNaN( const float &f, int i ) const;
	virtual bool CompareExact( const float &f, const float &fref, int i ) const;
	virtual bool CompareDenZp( const float &f, int i ) const;
	virtual bool CompareDenZn( const float &f, int i ) const;
	virtual bool CompareDenZ( const float &f, int i ) const;
	virtual bool CompareUnknown( const float &f, int i );

protected:

	// Parameters	
	float m_Src[3][4];
	SRC_MOD_TYPE m_SrcMod[3];
	INST_MOD_TYPE m_InstMod;	
	
	ID3D11Buffer *m_pRTBuffer2;
	ID3D11RenderTargetView *m_pRTBufferView2;
	ID3D11UnorderedAccessView *m_pRTUABufferView2;

	// Calculated values
	float m_ExpectedResult[2][4];
	float m_OptionalResult[4];
	FLOAT_RESULT_CLASS m_frc[2][4];
	int m_nMaxErrorULP;
};


class CShader5xTest_add : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "add"; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
};


class CShader5xTest_div : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "div"; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
	virtual bool CompareUnknown( const float &f, int i );
	virtual bool CompareReal( const float &f, const float &fref, int i ) const;
};


class CShader5xTest_dpn : public CShader5xTest_FloatArith
{
public:
	CShader5xTest_dpn( int nVectorSize ) : m_nVectorSize( nVectorSize ) {}
	
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;

protected:
	int m_nVectorSize;
};


class CShader5xTest_dp2 : public CShader5xTest_dpn
{
public:
	CShader5xTest_dp2() : CShader5xTest_dpn( 2 ) {}
	virtual const char *GetOpCode() const { return "dp2"; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual bool CompareReal( const float &f, const float &fref, int i ) const;
};


class CShader5xTest_dp3 : public CShader5xTest_dpn
{
public:
	CShader5xTest_dp3() : CShader5xTest_dpn( 3 ) {}
	virtual const char *GetOpCode() const { return "dp3"; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual bool CompareReal( const float &f, const float &fref, int i ) const;
};


class CShader5xTest_dp4 : public CShader5xTest_dpn
{
public:
	CShader5xTest_dp4() : CShader5xTest_dpn( 4 ) {}
	virtual const char *GetOpCode() const { return "dp4"; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual bool CompareReal( const float &f, const float &fref, int i ) const;
};


class CShader5xTest_exp : public CShader5xTest_FloatArith
{
public:
	CShader5xTest_exp() { m_nMaxErrorULP = 3; }
	
	virtual int GetNumSrcRegisters() const { return 1; }
	virtual const char *GetOpCode() const { return "exp"; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
	virtual bool CompareReal( const float &f, const float &fref, int i ) const;
};


class CShader5xTest_frc : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "frc"; }
	virtual int GetNumSrcRegisters() const { return 1; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
};


class CShader5xTest_log : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "log"; }
	virtual int GetNumSrcRegisters() const { return 1; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
	virtual bool CompareReal( const float &f, const float &fref, int i ) const;
};


class CShader5xTest_mad : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "mad"; }
	virtual int GetNumSrcRegisters() const { return 3; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
	virtual bool CompareReal( const float &f, const float &fref, int i ) const;
};


class CShader5xTest_max : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "max"; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
	virtual bool CompareExact( const float &f, const float &fref, int i ) const;
};


class CShader5xTest_min : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "min"; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
	virtual bool CompareExact( const float &f, const float &fref, int i ) const;
};


class CShader5xTest_mul : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "mul"; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
};


class CShader5xTest_round : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "round_ne"; }
	virtual int GetNumSrcRegisters() const { return 1; }
	virtual void InitTestParameters();
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
};


class CShader5xTest_round_ne : public CShader5xTest_round
{
public:
	virtual const char *GetOpCode() const { return "round_ne"; }
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
};


class CShader5xTest_round_ni : public CShader5xTest_round
{
public:
	virtual const char *GetOpCode() const { return "round_ni"; }
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
};


class CShader5xTest_round_pi : public CShader5xTest_round
{
public:
	virtual const char *GetOpCode() const { return "round_pi"; }
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
};


class CShader5xTest_round_z : public CShader5xTest_round
{
public:
	virtual const char *GetOpCode() const { return "round_z"; }
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
};


class CShader5xTest_rsq : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "rsq"; }
	virtual int GetNumSrcRegisters() const { return 1; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
	virtual bool CompareReal( const float &f, const float &fref, int i ) const;
};

class CShader5xTest_rcp : public CShader5xTest_rsq
{
	TEST_RESULT Setup();
	virtual const char *GetOpCode() const { return "rcp"; }
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
};

class CShader5xTest_sincos : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "sincos"; }
	virtual TEST_RESULT SetupRenderTarget();
	virtual int GetNumSrcRegisters() const { return 1; }
	virtual int GetNumDestRegisters() const { return 2; }
	virtual bool IsDestNull( int nIndex ) const;
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
	virtual void CalcExpectedResult2( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass2( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
	virtual bool CompareReal( const float &f, const float &fref, int i ) const;

	enum SINCOSOUTPUT
	{
		BOTH = 0,
		SINONLY,
		COSONLY,
	};
	
	SINCOSOUTPUT m_SinCosOutput;
};


class CShader5xTest_sqrt : public CShader5xTest_FloatArith
{
public:
	virtual const char *GetOpCode() const { return "sqrt"; }
	virtual int GetNumSrcRegisters() const { return 1; }
	virtual void InitTestParameters();
	virtual void CalcExpectedResult( float fResult[4], const float *src0, const float *src1 = NULL, const float *src2 = NULL ) const;
	virtual void GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 = NULL, const float *fSrc2 = NULL ) const;
};
