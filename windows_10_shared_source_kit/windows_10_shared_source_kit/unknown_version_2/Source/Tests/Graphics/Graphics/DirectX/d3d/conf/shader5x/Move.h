//////////////////////////////////////////////////////////////////////////
//  Move.h
//  created:	2005/03/10
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"
#include "DoublesArith.h"

enum REGS
{
	REGS_DIFFERENT,
	REGS_SAME,
	REGS_MIXED,
};

struct MoveSwapShaderParams
{
	UINT srcVals[3][4];
};

struct MoveSwapResults
{
	UINT destVals[2][4];
};

class CShader5xTest_moveswap : public CShader5xTest
{
protected:
	CShader5xTest_moveswap() :	CShader5xTest(DXGI_FORMAT_R32G32B32A32_UINT)
							,m_dest0Mask(AssemblyHelper::xyzw)
							,m_dest1Mask(AssemblyHelper::xyzw)
							,m_src0Swizzle(AssemblyHelper::swizXYZW)
							,m_src1Swizzle(AssemblyHelper::swizXYZW)
							,m_src2Swizzle(AssemblyHelper::swizXYZW)
							,m_pRTTexture2D2(NULL)
							,m_pRTView2(NULL)
							,m_pRTUAView2(NULL)
							,m_srcValues(NULL)
	 {}

	virtual ~CShader5xTest_moveswap()
	{
		if( m_srcValues )
			delete [] m_srcValues;
	}

	virtual const char *GetOpCode() const = 0;
	virtual int GetNumDestRegisters() const = 0;
	virtual int GetNumSrcRegisters() const = 0;
	virtual void CalcResult(UINT &dest0, UINT &dest1, UINT src0, UINT src1, UINT src2) = 0;

	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();

	UINT ComputeComponentIndex(AssemblyHelper::Swizzle swizz, const UINT &index);
	TEST_RESULT SetPipeline(const char *szDest[], const char *szSrc[]);

protected:

	// Because we have multiple outputs, we need multiple rendertargets
	ID3D11Texture2D *m_pRTTexture2D2;
	ID3D11RenderTargetView *m_pRTView2;
	ID3D11UnorderedAccessView *m_pRTUAView2;

	// Parameters
	
	UINT *m_srcValues;
	REGS m_srcRegs;

	bool m_bDest0OverlapWithSrc;
	bool m_bDest1OverlapWithSrc;

	AssemblyHelper::MaskFlag m_dest0Mask;
	AssemblyHelper::MaskFlag m_dest1Mask;
	AssemblyHelper::Swizzle m_src0Swizzle;
	AssemblyHelper::Swizzle m_src1Swizzle;
	AssemblyHelper::Swizzle m_src2Swizzle;
};

class CShader5xTest_mov : public CShader5xTest_moveswap
{
};


class CShader5xTest_movc : public CShader5xTest_moveswap
{
};

////

class CShader5xTest_dmov : public CShader5xTest_DoubleArith
{
protected:
	CShader5xTest_DoubleArith::Binary64Vec2 m_Src;
	tstring m_Mask;
	tstring m_Swizzle;
	SRC_MOD_TYPE m_SrcModifier;
	OPERAND_TYPE m_OperandType;

public:
	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	CShader5xTest_DoubleArith::Binary64Vec2 CalcSrcSwizzle(CShader5xTest_DoubleArith::Binary64Vec2 const&, tstring const&);
	CShader5xTest_DoubleArith::Binary64Vec2 CalcResultMask(CShader5xTest_DoubleArith::Binary64Vec2 const&, tstring const&);
	virtual ~CShader5xTest_dmov();
};

////

class CShader5xTest_dmovc : public CShader5xTest_dmov
{
	protected:
		CShader5xTest_DoubleArith::Binary64Vec2 m_Src0;
		CShader5xTest_DoubleArith::Binary64Vec2 m_Src1;
		CShader5xTest_DoubleArith::Binary64Vec2 m_Src2;
		tstring m_Mask;
		tstring m_Swizzle0;
		tstring m_Swizzle1;
		tstring m_Swizzle2;
		OPERAND_TYPE m_OperandType[3];
		SRC_MOD_TYPE m_SrcModifier;
	public:
		virtual void InitTestParameters();
		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();
		
		virtual ~CShader5xTest_dmovc();
};

////
class CShader5xTest_swapc : public CShader5xTest_moveswap
{
	virtual TEST_RESULT Setup();
	int GetNumDestRegisters() const { return 2; }
	int GetNumSrcRegisters() const { return 3; }
	virtual const char *GetOpCode() const { return "swapc"; }
	void CalcResult(UINT &dest0, UINT &dest1, UINT src0, UINT src1, UINT src2);
};

////
class CShader5xTest_cyclecounter : public CShader5xTest
{
public:
	CShader5xTest_cyclecounter() : CShader5xTest(DXGI_FORMAT_R32G32B32A32_UINT)
		{}

	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();

protected:
	// Parameters
	AssemblyHelper::Swizzle  m_SrcSwizzle;
	AssemblyHelper::MaskFlag m_DclMask;
	AssemblyHelper::MaskFlag m_DstMask;
};