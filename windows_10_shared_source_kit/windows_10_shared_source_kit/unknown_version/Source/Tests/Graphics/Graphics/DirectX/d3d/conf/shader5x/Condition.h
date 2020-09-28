//////////////////////////////////////////////////////////////////////////
//  Condition.h
//  created:	2005/03/10
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"
#include "DoublesArith.h"

#include "new_off.h"
#include "d3dx10math.h"
#include "new_on.h"

enum FLOAT_CONDITION_RESULT_CLASS
{
	FCRC_FALSE  = 0x00000000,	// always FALSE
	FCRC_CALC	= 0x0000ca1c,	// depends on the modifier and/or values for real
	FCRC_TRUE	= 0xFFFFFFFF,	// always TRUE
};

enum DOUBLE_CONDITION_RESULT_CLASS
{
	DCRC_FALSE = 0x00000000, // always FALSE
	DCRC_CALC  = 0x0000ca1c, // result is TRUE or FALSE CalcResult determines which
	DCRC_SCLC  = 0xca1cca1c, // result is either src0 or src1, CalcResult determines which
	DCRC_SRC0  = 0x512C0000, // result is always src0
	DCRC_SRC1  = 0x512C0001, // result is always src1
	DCRC_SRC01 = 0x512C0002, // result can be either src0 or src1 (the spec allows for either)
	DCRC_TRUE  = 0xFFFFFFFF, // always TRUE
};

struct FloatConditionShaderParams
{
	D3DXVECTOR4 src0;
	D3DXVECTOR4 src1;
};

struct IntConditionShaderParams
{
	int4 src0;
	int4 src1;
};

struct UIntConditionShaderParams
{
	uint4 src0;
	uint4 src1;
};

struct DoubleConditionShaderParams
{
	CShader5xTest_DoubleArith::Binary64Vec2 src0;
	CShader5xTest_DoubleArith::Binary64Vec2 src1;
};
///<sumary>
///Base class for all the Condition instructions tests
///</sumary>
class CShader5xTest_Condition : public CShader5xTest
{
protected:
	CShader5xTest_Condition() : CShader5xTest(DXGI_FORMAT_R32G32B32A32_UINT)
		{}
	
	virtual const char *GetOpCode() const = 0;
	virtual int GetNumDestRegisters() const { return 1; }
	virtual int GetNumSrcRegisters() const { return 2; }

	///<sumary>
	///Sets the shader for the current pipeline
	///</sumary>
	TEST_RESULT SetPipeline(const char *szDest, const char *szSrc[2], bool doubleEnable = false);

	void CleanupTestCase();

protected:

	// Parameters	
	
};

class CShader5xTest_FloatCondition : public CShader5xTest_Condition
{
public:

	virtual FLOAT_CONDITION_RESULT_CLASS GetResultClass( FLOAT_INPUT_CLASS ficSrc0, FLOAT_INPUT_CLASS ficSrc1 = FIC_NONE ) const { return FCRC_FALSE; }

	virtual void InitTestParameters();

	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();

	virtual bool CalcResult(float srcModified0, float srcModified1) = 0;

protected:

	// Parameters	
	D3DXVECTOR4 m_Src[2];
	SRC_MOD_TYPE m_SrcMod[2];
};

class CShader5xTest_DoubleCondition : public CShader5xTest_Condition
{
public:

	virtual DOUBLE_CONDITION_RESULT_CLASS GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS ficSrc1 = DIC_NONE ) const { return DCRC_FALSE; }
	virtual void InitTestParameters();

	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();

	virtual bool CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1) = 0;
	
	static DOUBLE_INPUT_CLASS GetInputClass( CShader5xTest_DoubleArith::Binary64 d );
	static CShader5xTest_DoubleArith::Binary64 ResolveSrc( CShader5xTest_DoubleArith::Binary64 d, SRC_MOD_TYPE mod );
	static CShader5xTest_DoubleArith::Binary64 ResolveDest( CShader5xTest_DoubleArith::Binary64 d, INST_MOD_TYPE mod );

protected:
	CShader5xTest_DoubleArith::Binary64Vec2 m_Src[2];
	SRC_MOD_TYPE  m_SrcMod[2];
	OPERAND_TYPE m_OperandType[2];
};

class CShader5xTest_IntCondition : public CShader5xTest_Condition
{
public:

	virtual void InitTestParameters();

	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();

	virtual bool CalcResult(int src0, int src1) = 0;

protected:

	// Parameters	
	int4 m_Src[2];
};

class CShader5xTest_UIntCondition : public CShader5xTest_Condition
{
public:

	virtual void InitTestParameters();

	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();

	virtual bool CalcResult(UINT src0, UINT src1) = 0;

protected:

	// Parameters	
	uint4 m_Src[2];
};


class CShader5xTest_eq : public CShader5xTest_FloatCondition
{
public:
	virtual const char *GetOpCode() const { return "eq"; }
	virtual FLOAT_CONDITION_RESULT_CLASS GetResultClass( FLOAT_INPUT_CLASS ficSrc0, FLOAT_INPUT_CLASS ficSrc1 = FIC_NONE ) const;
	bool CalcResult(float srcModified0, float srcModified1);
};


class CShader5xTest_ge : public CShader5xTest_FloatCondition
{
public:
	virtual const char *GetOpCode() const { return "ge"; }
	virtual FLOAT_CONDITION_RESULT_CLASS GetResultClass( FLOAT_INPUT_CLASS ficSrc0, FLOAT_INPUT_CLASS ficSrc1 = FIC_NONE ) const;
	bool CalcResult(float srcModified0, float srcModified1);
};


class CShader5xTest_lt : public CShader5xTest_FloatCondition
{
public:
	virtual const char *GetOpCode() const { return "lt"; }
	virtual FLOAT_CONDITION_RESULT_CLASS GetResultClass( FLOAT_INPUT_CLASS ficSrc0, FLOAT_INPUT_CLASS ficSrc1 = FIC_NONE ) const;
	bool CalcResult(float srcModified0, float srcModified1);
};


class CShader5xTest_ne : public CShader5xTest_FloatCondition
{
public:
	virtual const char *GetOpCode() const { return "ne"; }
	virtual FLOAT_CONDITION_RESULT_CLASS GetResultClass( FLOAT_INPUT_CLASS ficSrc0, FLOAT_INPUT_CLASS ficSrc1 = FIC_NONE ) const;
	bool CalcResult(float srcModified0, float srcModified1);
};


class CShader5xTest_deq : public CShader5xTest_DoubleCondition
{
public:
	virtual const char* GetOpCode() const {return "deq";}
	virtual DOUBLE_CONDITION_RESULT_CLASS GetResultClass(DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dic_Src1 = DIC_NONE) const;
	bool CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1);
};


class CShader5xTest_dge : public CShader5xTest_DoubleCondition
{
public:
	virtual const char *GetOpCode() const { return "dge"; }
	virtual DOUBLE_CONDITION_RESULT_CLASS GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dicSrc1 = DIC_NONE ) const;
	bool CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1);
};


class CShader5xTest_dlt : public CShader5xTest_DoubleCondition
{
public:
	virtual const char *GetOpCode() const { return "dlt"; }
	virtual DOUBLE_CONDITION_RESULT_CLASS GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dicSrc1 = DIC_NONE ) const;
	bool CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1);
};


class CShader5xTest_dne : public CShader5xTest_DoubleCondition
{
public:
	virtual const char *GetOpCode() const { return "dne"; }
	virtual DOUBLE_CONDITION_RESULT_CLASS GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dicSrc1 = DIC_NONE ) const;
	bool CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1);
};


class CShader5xTest_dmin : public CShader5xTest_DoubleCondition
{
public:
	TEST_RESULT SetupTestCase();
	virtual const char *GetOpCode() const { return "dmin"; }
	virtual DOUBLE_CONDITION_RESULT_CLASS GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dicSrc1 = DIC_NONE ) const;
	bool CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1);
};


class CShader5xTest_dmax : public CShader5xTest_DoubleCondition
{
public:
	TEST_RESULT SetupTestCase();
	virtual const char *GetOpCode() const { return "dmax"; }
	virtual DOUBLE_CONDITION_RESULT_CLASS GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dicSrc1 = DIC_NONE ) const;
	bool CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1);
};


class CShader5xTest_ige : public CShader5xTest_IntCondition
{
public:
	virtual const char *GetOpCode() const { return "ige"; }
	bool CalcResult(int src0, int src1);
};


class CShader5xTest_ieq : public CShader5xTest_IntCondition
{
public:
	virtual const char *GetOpCode() const { return "ieq"; }
	bool CalcResult(int src0, int src1);
};


class CShader5xTest_ilt : public CShader5xTest_IntCondition
{
public:
	virtual const char *GetOpCode() const { return "ilt"; }
	bool CalcResult(int src0, int src1);
};


class CShader5xTest_ine : public CShader5xTest_IntCondition
{
public:
	virtual const char *GetOpCode() const { return "ine"; }
	bool CalcResult(int src0, int src1);
};


class CShader5xTest_uge : public CShader5xTest_UIntCondition
{
public:
	virtual const char *GetOpCode() const { return "uge"; }
	bool CalcResult(UINT src0, UINT src1);
};


class CShader5xTest_ult : public CShader5xTest_UIntCondition
{
public:
	virtual const char *GetOpCode() const { return "ult"; }
	bool CalcResult(UINT src0, UINT src1);
};


