//////////////////////////////////////////////////////////////////////////
//  SumAbsDiff.h
//  created:	5/10/2011
//
//  purpose:    Test Masked Sum of Absolute Differences (msad) and any 
//              future similar instructions.
//              These are typically used for video compression and
//              pattern matching. 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"
	
class CShader5xTest_SumAbsDiff : public CShader5xTest
{
public:
	CShader5xTest_SumAbsDiff() : 
		CShader5xTest(DXGI_FORMAT_R32G32B32A32_UINT),
		m_pCurrentUAView(NULL)
	{
	}

protected:
	void InitTestParameters();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();

	virtual tstring GetInstruction() = 0;
	virtual void CalcResult() = 0;
	virtual TEST_RESULT CheckPixel( BYTE Pixel[4][4] ) = 0;

	UINT32			m_uiSrc0[4];
	UINT32			m_uiSrc1[4];
	UINT32			m_uiSrc2[4];
	UINT32			m_uiResult[4];
	ID3D11UnorderedAccessView* m_pCurrentUAView;
	tstring			m_InstructionCode;
};
	
class CShader5xTest_msad : public CShader5xTest_SumAbsDiff
{
protected:
	tstring GetInstruction();
	void CalcResult();
	TEST_RESULT CheckPixel( BYTE Pixel[4][4] );
};
