//////////////////////////////////////////////////////////////////////////
//  ControlFlow.h
//  created:	2005/03/10
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"

#define EXPECT_ZERO 0x1000
#define EXPECT_NOTZERO 0x2000

enum ControlFlowConditionalInstruction
{
	CFC_if_z,
	CFC_if_nz,
	CFC_continuec_z,
	CFC_continuec_nz,
	CFC_breakc_z,
	CFC_breakc_nz,
	CFC_retc_z,
	CFC_retc_nz,
	CFC_callc_z,
	CFC_callc_nz,
	CFC_discard_z,
	CFC_discard_nz,
	nControlFlowConditionalInstructions,
};

enum ControlFlowInstruction
{
	CF_if_z			= 1 | EXPECT_ZERO,
	CF_if_nz		= 2 | EXPECT_NOTZERO,
	CF_continue		= 3,
	CF_continuec_z	= 4 | EXPECT_ZERO,
	CF_continuec_nz = 5 | EXPECT_NOTZERO,
	CF_break		= 6,
	CF_breakc_z		= 7 | EXPECT_ZERO,
	CF_breakc_nz	= 8 | EXPECT_NOTZERO,
	CF_else			= 9,
	CF_ret			= 10,
	CF_retc_z		= 11 | EXPECT_ZERO,
	CF_retc_nz		= 12 | EXPECT_NOTZERO,
	CF_call			= 13,
	CF_callc_z		= 14 | EXPECT_ZERO,
	CF_callc_nz		= 15 | EXPECT_NOTZERO,
	CF_discard_z	= 16 | EXPECT_ZERO,
	CF_discard_nz	= 17 | EXPECT_NOTZERO,
	CF_switch		= 18,
	CF_loop			= 19,
	nControlFlowInstructions = 19,
};

enum ControlFlowConditionalInstructionNC
{
	CFNC_if_z_R,
	CFNC_if_z_RG,
	CFNC_if_z_RGB,
	CFNC_if_z_RGBA,
	CFNC_if_z_RB,
	CFNC_if_z_GA,
	CFNC_if_z_G,
	CFNC_if_z_B,
	CFNC_if_z_A,
	CFNC_loop_R

};

enum SwitchCaseBreak
{
	Switch_Case_None,
	Switch_Case_Break,
	Switch_Case_BreakC,
	Switch_Case_Ret,
	Switch_Case_RetC,
	Switch_Case_BreakC_Break,
	Switch_Case_BreakC_BreakC,
	Switch_Case_BreakC_Ret,
	Switch_Case_BreakC_RetC,
	Switch_Case_RetC_Break,
	Switch_Case_RetC_BreakC,
	Switch_Case_RetC_Ret,
	Switch_Case_RetC_RetC,
};

enum SwitchCaseDefault
{
	Switch_Case_No_Default,
	Switch_Case_Default_Begin,
	Switch_Case_Default_Middle,
	Switch_Case_Default_End,
};

extern tstring __stdcall ToString( ControlFlowInstruction type );
extern bool __stdcall FromString( ControlFlowInstruction *pType, const tstring &str );
extern tstring __stdcall ToString( ControlFlowConditionalInstruction type );
extern bool __stdcall FromString( ControlFlowConditionalInstruction *pType, const tstring &str );
extern tstring __stdcall ToString( ControlFlowConditionalInstructionNC type );
extern bool __stdcall FromString( ControlFlowConditionalInstructionNC *pType, const tstring &str );

//-------------------------------------------------------------------------------
class CShader5xTest_ControlFlow : public CShader5xTest
{
protected:
	CShader5xTest_ControlFlow():
		 CShader5xTest(DXGI_FORMAT_R32G32B32A32_TYPELESS),
			 m_pLayout(NULL),
			 m_pVertexBuffer(NULL),
			 m_pRTBufferViewFloat(NULL)
		 {
			m_bDrawTriangle=false;
			m_bUseFourVertices=false;
		 }
	virtual TEST_RESULT SetupRenderTarget();
	virtual const char *GetOpCode() const = 0;
	virtual int GetNumDestRegisters() const { return 1; }
	virtual int GetNumSrcRegisters() const { return 2; }
	virtual void CleanupTestCase();
	virtual void Cleanup();
	virtual TEST_RESULT ExecuteTestCase();
	virtual UINT CalcResult() = 0;
	virtual TEST_RESULT SetPipeline( AssemblyHelper& asmHelper, const char *szInstruction, bool bAppendFinishShader = true);

protected:
	//Support for drawing a triangle
	bool m_bDrawTriangle;
	//Support for sending 4 vertices to vertex shader stage
	bool m_bUseFourVertices;
	ID3D11InputLayout* m_pLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11RenderTargetView* m_pRTBufferViewFloat;
};

//-------------------------------------------------------------------------------
typedef struct _PIXEL32
{
	DWORD red;
	DWORD green;
	DWORD blue;
	DWORD alpha;
} PIXEL32;

//-------------------------------------------------------------------------------
//This test ensures non-coherent branching works.
class CShader5xTest_ControlFlowNC : public CShader5xTest_ControlFlow
{
public:

	CShader5xTest_ControlFlowNC():
	  m_pLayout(NULL),
	  m_pVertexBuffer(NULL)
	  {
		  m_viewPort.Width=16;
		  m_viewPort.Height=16;

		  m_bDrawTriangle=true;
	  }

	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	
	virtual UINT CalcResult();
	PIXEL32 CalcResult(UINT x, UINT y);

	virtual const char *GetOpCode() const { return "TrueValues"; }

	ID3D11InputLayout* m_pLayout;
	ID3D11Buffer* m_pVertexBuffer;

protected:

	// Parameters	
	ControlFlowConditionalInstructionNC m_Instruction;
	UINT32 m_Src;
};

//-------------------------------------------------------------------------------
//This test is to make sure that all instructions testing for zero\not zero
//treat any non zero value correctly.
class CShader5xTest_ControlFlowTrueValues : public CShader5xTest_ControlFlow
{
public:
	virtual void InitTestParameters();
	TEST_RESULT SetupTestCase();
	virtual UINT CalcResult();
	virtual const char *GetOpCode() const { return "TrueValues"; }

protected:
	// Parameters	
	ControlFlowConditionalInstruction m_Instruction;
	UINT32 m_Src;
};

//-------------------------------------------------------------------------------
class CShader5xTest_ControlFlowTrueValuesNC : public CShader5xTest_ControlFlowTrueValues
{
public:
	CShader5xTest_ControlFlowTrueValuesNC()
	{
		m_viewPort.Width=16;
		m_viewPort.Height=16;
		m_bDrawTriangle=true;
		m_bUseFourVertices=true;
	}

	TEST_RESULT ExecuteTestCase();
};

//-------------------------------------------------------------------------------
//This test is to make sure that all instructions testing for zero\not zero
//treat any non zero value correctly.
class CShader5xTest_NestedControlFlow : public CShader5xTest_ControlFlow
{
public:
	virtual void InitTestParameters();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	virtual UINT CalcResult();
	virtual const char *GetOpCode() const { return "Nested"; }

protected:
	// Parameters	
	ControlFlowInstruction m_OddInstruction;
	ControlFlowInstruction m_EvenInstruction;
	UINT m_NumNest;
	UINT m_NumSubRoutine;
};

//-------------------------------------------------------------------------------
class CShader5xTest_if : public CShader5xTest_ControlFlow
{
public:
	virtual const char *GetOpCode() const { return "if"; }
	virtual void InitTestParameters();
	TEST_RESULT SetupTestCase();
	virtual UINT CalcResult();

protected:
	// Parameters	
	bool m_bZeroCondition;
	bool m_bElse;
	UINT m_Src;
};

//-------------------------------------------------------------------------------
class CShader5xTest_loop : public CShader5xTest_ControlFlow
{
public:
	virtual const char *GetOpCode() const { return "loop"; }
	virtual void InitTestParameters();
	TEST_RESULT SetupTestCase();
	virtual UINT CalcResult();

protected:
	UINT m_ExpectedResult;
	// Parameters	
	ControlFlowInstruction m_LoopInstruction;
	UINT m_Src;
};

//-------------------------------------------------------------------------------
class CShader5xTest_call : public CShader5xTest_ControlFlow
{
public:
	virtual const char *GetOpCode() const { return "call"; }
	virtual void InitTestParameters();
	TEST_RESULT SetupTestCase();
	virtual UINT CalcResult();

protected:
	UINT m_ExpectedResult;
	// Parameters	
	ControlFlowInstruction m_Instruction;
	UINT m_NumRecursion;
	UINT m_NumLabel;
};

//-------------------------------------------------------------------------------
class CShader5xTest_callc : public CShader5xTest
{
};

//-------------------------------------------------------------------------------
class CShader5xTest_switch : public CShader5xTest_ControlFlow
{
public:
	virtual const char *GetOpCode() const { return "switch"; }
	virtual void InitTestParameters();
	TEST_RESULT SetupTestCase();
	virtual UINT CalcResult();

protected:
	tstring GenerateSwitchCase(AssemblyHelper& asmHelper, unsigned int caseValue);
	UINT m_ExpectedResult;
	// Parameters	
	SwitchCaseBreak m_Break[3];
	SwitchCaseDefault m_Default;
	UINT m_Src[3];
};

//-------------------------------------------------------------------------------
class CShader5xTest_switchNC : public CShader5xTest_switch
{
public:
	CShader5xTest_switchNC()
	{
		m_viewPort.Width=16;
		m_viewPort.Height=16;
		m_bDrawTriangle=true;
		m_bUseFourVertices=true;
	}
	virtual void InitTestParameters();
	TEST_RESULT SetupTestCase();
	UINT CalcResult(UINT index);
	TEST_RESULT ExecuteTestCase();
protected:
	UINT m_Src[4][3];
};


