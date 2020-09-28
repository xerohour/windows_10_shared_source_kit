//////////////////////////////////////////////////////////////////////////
//  Limits.h
//  created:	2006/09/22
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"

class CShader5xTest_Registers : public CShader5xTest
{
protected:
	CShader5xTest_Registers() : CShader5xTest(DXGI_FORMAT_R32G32B32A32_UINT)
		{}
	
	//virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();
	virtual TEST_RESULT Setup();
	
	TEST_RESULT CreateVertexShader(tstring szShaderBody);
	TEST_RESULT CreateGeometryShader(tstring szShaderBody, bool bCreateWithStreamOutput);
	TEST_RESULT CreatePixelShader(tstring szShaderBody);
	TEST_RESULT SetPipeline(tstring szShaderBody);

protected:

	// Parameters
	UINT m_Src[2];
};

class CShader5xTest_Registers_LimitsTemp : public CShader5xTest_Registers
{
public:

	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

	UINT GetNumElementsForIndexableTemp(UINT indexableTemp);

protected:

	// Parameters	
	UINT m_TempCount;
	UINT m_IndexableTempCount;
	UINT m_NumIndexableRegisters;
};

class CShader5xTest_Registers_LimitsInputOutput : public CShader5xTest_Registers
{
public:

	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:

	// Parameters	
	UINT				m_NumInput;
	UINT				m_NumOutput;
	SHADER_STAGE_TYPE	m_InputStage;
	SHADER_STAGE_TYPE	m_OutputStage;
	bool				m_SequentialRegisters;
	tstring				m_OutputMask;
	tstring				m_InputMask;
};

class CShader5xTest_Registers_IncompleteWrite : public CShader5xTest_Registers
{
public:

	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:

	// Parameters	
	UINT				m_NumInput;
	UINT				m_NumOutput;
	SHADER_STAGE_TYPE	m_InputStage;
	SHADER_STAGE_TYPE	m_OutputStage;
	bool				m_SequentialRegisters;
	tstring				m_OutputMask;
	tstring				m_InputMask;
};


