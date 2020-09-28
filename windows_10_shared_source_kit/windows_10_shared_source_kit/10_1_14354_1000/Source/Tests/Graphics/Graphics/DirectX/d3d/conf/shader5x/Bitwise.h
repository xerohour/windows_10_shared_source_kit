//////////////////////////////////////////////////////////////////////////
//  Bitwise.h
//  created:	2005/03/10
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"

struct BitwiseShaderParams
{
	UINT src0[4];
	UINT src1[4];
};

struct BitfieldShaderParams
{
	UINT srcVals[4][4];
};

class CShader5xTest_Bitwise : public CShader5xTest
{
protected:
	CShader5xTest_Bitwise() : CShader5xTest(DXGI_FORMAT_R32G32B32A32_UINT)
		{}
	
	virtual const char *GetOpCode() const = 0;
	virtual int GetNumDestRegisters() const { return 1; }
	virtual int GetNumSrcRegisters() const { return 2; }

	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();

	virtual UINT CalcResult(UINT src0, UINT src1) = 0;

	TEST_RESULT SetPipeline(const char *szDest, const char *szSrc[2]);

protected:

	// Parameters	
	
	UINT m_Src[2];
};

class CShader5xTest_ShiftBitwise : public CShader5xTest_Bitwise
{
public:

	virtual void InitTestParameters();
	TEST_RESULT SetupTestCase();

};

class CShader5xTest_NonShiftBitwise : public CShader5xTest_Bitwise
{
public:

	virtual void InitTestParameters();
	TEST_RESULT SetupTestCase();

};

class CShader5xTest_not : public CShader5xTest_Bitwise
{
public:
	virtual const char *GetOpCode() const { return "not"; }
	UINT CalcResult(UINT src0, UINT src1);

	virtual void InitTestParameters();
	TEST_RESULT SetupTestCase();

};

class CShader5xTest_and : public CShader5xTest_NonShiftBitwise
{
public:
	virtual const char *GetOpCode() const { return "and"; }
	UINT CalcResult(UINT src0, UINT src1);
};


class CShader5xTest_ishl : public CShader5xTest_ShiftBitwise
{
public:
	virtual const char *GetOpCode() const { return "ishl"; }
	UINT CalcResult(UINT src0, UINT src1);
};


class CShader5xTest_ishr : public CShader5xTest_ShiftBitwise
{
public:
	virtual const char *GetOpCode() const { return "ishr"; }
	UINT CalcResult(UINT src0, UINT src1);
};


class CShader5xTest_or : public CShader5xTest_NonShiftBitwise
{
public:
	virtual const char *GetOpCode() const { return "or"; }
	UINT CalcResult(UINT src0, UINT src1);
};


class CShader5xTest_ushr : public CShader5xTest_ShiftBitwise
{
public:
	virtual const char *GetOpCode() const { return "ushr"; }
	UINT CalcResult(UINT src0, UINT src1);
};


class CShader5xTest_xor : public CShader5xTest_NonShiftBitwise
{
public:
	virtual const char *GetOpCode() const { return "xor"; }
	UINT CalcResult(UINT src0, UINT src1);
};

//
// Bitfield classes
//

class CShader5xTest_Bitfield : public CShader5xTest
{
protected:
	CShader5xTest_Bitfield() :	CShader5xTest(DXGI_FORMAT_R32G32B32A32_UINT)
								,m_destMask(AssemblyHelper::xyzw)
								,m_src0Swizzle(AssemblyHelper::swizXYZW)
								,m_src1Swizzle(AssemblyHelper::swizXYZW)
								,m_src2Swizzle(AssemblyHelper::swizXYZW)
								,m_src3Swizzle(AssemblyHelper::swizXYZW)
		 {}
	
	virtual const char *GetOpCode() const = 0;
	virtual int GetNumDestRegisters() const { return 1; }
	virtual int GetNumSrcRegisters() const = 0;

	virtual void InitTestParameters();
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();

	UINT ComputeComponentIndex(AssemblyHelper::Swizzle swizz, const UINT &index);
	virtual UINT CalcResult(UINT src0, UINT src1, UINT src2, UINT src3) = 0;

	TEST_RESULT SetPipeline(const char *szDest, const char *szSrc[3]);

protected:

	// Parameters	
	
	UINT m_Src[4];	// Note, not all of these may be used

	AssemblyHelper::MaskFlag m_destMask;
	AssemblyHelper::Swizzle m_src0Swizzle;
	AssemblyHelper::Swizzle m_src1Swizzle;
	AssemblyHelper::Swizzle m_src2Swizzle;
	AssemblyHelper::Swizzle m_src3Swizzle;
};

/////

class CShader5xTest_ubfe : public CShader5xTest_Bitfield
{
public:
	int GetNumSrcRegisters() const { return 3; }
	virtual const char *GetOpCode() const { return "ubfe"; }
	UINT CalcResult(UINT src0, UINT src1, UINT src2, UINT src3);
};

/////

class CShader5xTest_ibfe : public CShader5xTest_Bitfield
{
public:
	int GetNumSrcRegisters() const { return 3; }
	virtual const char *GetOpCode() const { return "ibfe"; }
	UINT CalcResult(UINT src0, UINT src1, UINT src2, UINT src3);
};

/////

class CShader5xTest_bfi : public CShader5xTest_Bitfield
{
public:
	int GetNumSrcRegisters() const { return 4; }
	virtual const char *GetOpCode() const { return "bfi"; }
	UINT CalcResult(UINT src0, UINT src1, UINT src2, UINT src3);
};

/////

class CShader5xTest_bfrev : public CShader5xTest_Bitfield
{
public:
	int GetNumSrcRegisters() const { return 1; }
	virtual const char *GetOpCode() const { return "bfrev"; }
	UINT CalcResult(UINT src0, UINT src1, UINT src2, UINT src3);
};

/////

class CShader5xTest_countbits : public CShader5xTest_Bitfield
{
public:
	int GetNumSrcRegisters() const { return 1; }
	virtual const char *GetOpCode() const { return "countbits"; }
	UINT CalcResult(UINT src0, UINT src1, UINT src2, UINT src3);
};

/////

class CShader5xTest_firstbit_lo : public CShader5xTest_Bitfield
{
public:
	int GetNumSrcRegisters() const { return 1; }
	virtual const char *GetOpCode() const { return "firstbit_lo"; }
	UINT CalcResult(UINT src0, UINT src1, UINT src2, UINT src3);
};

/////

class CShader5xTest_firstbit_hi : public CShader5xTest_Bitfield
{
public:
	int GetNumSrcRegisters() const { return 1; }
	virtual const char *GetOpCode() const { return "firstbit_hi"; }
	UINT CalcResult(UINT src0, UINT src1, UINT src2, UINT src3);
};

/////

class CShader5xTest_firstbit_shi : public CShader5xTest_Bitfield
{
public:
	int GetNumSrcRegisters() const { return 1; }
	virtual const char *GetOpCode() const { return "firstbit_shi"; }
	UINT CalcResult(UINT src0, UINT src1, UINT src2, UINT src3);
};





