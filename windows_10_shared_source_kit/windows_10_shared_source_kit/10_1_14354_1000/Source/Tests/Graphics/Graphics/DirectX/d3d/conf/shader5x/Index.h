//////////////////////////////////////////////////////////////////////////
//  Index.h
//  created:	2005/05/11
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"

#define NUM_INDEXABLE_RESOURCES	4

class CShader5xTest_Index : public CShader5xTest
{
public:
	CShader5xTest_Index() : 
		CShader5xTest( DXGI_FORMAT_R32G32B32A32_UINT ), 
		m_pVSBlob( NULL ),
		m_pCB( NULL ), 
		m_pVB( NULL ), 
		m_pLayout( NULL ),
		m_pRTBuffer_2( NULL ),
		m_pRTBufferView_2( NULL ),
		m_pRTUABufferView_2( NULL )
	{
		for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
		{
			m_pResources[i] = NULL;
			m_pSamplers[i] = NULL;
			m_pSRVs[i] = NULL;
			m_pUAVs[i] = NULL;
		}
	}

	enum INDEXING_SCENARIO
	{
		INDEXING_SCENARIO_TEMP,
		INDEXING_SCENARIO_TEMP_NESTED,
		INDEXING_SCENARIO_CONSTANT_BUFFER,
		INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED,
		INDEXING_SCENARIO_INPUT_OUTPUT,
		INDEXING_SCENARIO_INPUT_OUTPUT_NESTED,
		INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM,
		INDEXING_SCENARIO_SAMPLER,
		INDEXING_SCENARIO_BUFFER,
		INDEXING_SCENARIO_TEXTURE2D,
		INDEXING_SCENARIO_TEXTURE3D
	};

	enum INDEXING_INSTRUCTION
	{
		INDEXING_INSTRUCTION_MOV,
		//INDEXING_INSTRUCTION_DMOV,

		// Resource instructions
		INDEXING_INSTRUCTION_LD,

		// Resource and Sampler instructions
		INDEXING_INSTRUCTION_SAMPLE,
		//INDEXING_INSTRUCTION_GATHER4,
	};

	enum INDEXING_COMPUTATION_METHOD
	{
		INDEXING_COMPUTATION_METHOD_TEMP_REG,				// #r
		INDEXING_COMPUTATION_METHOD_STATIC_INDEX_REG,		// #x
		INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST,		// e.g 4

		// Combinations
		INDEXING_COMPUTATION_METHOD_TEMP_PLUS_IMMEDIATE,	// e.g #r + 4
		INDEXING_COMPUTATION_METHOD_INDEX_PLUS_IMMEDIATE,	// e.g #x + 4
	};

	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();

protected:
	TEST_RESULT BuildShaders();

	ID3D11Buffer *m_pCB;
	ID3D11Buffer *m_pVB;
	ID3D11Buffer *m_pRTBuffer_2;
	ID3D11RenderTargetView *m_pRTBufferView_2;
	ID3D11UnorderedAccessView *m_pRTUABufferView_2;
	ID3D11InputLayout *m_pLayout;
	ID3D10Blob *m_pVSBlob;
	const char *m_szVSShaderCode;
	const char *m_szGSShaderCode;
	const char *m_szPSShaderCode;

	// Pure virtual functions
	virtual TEST_RESULT VerifyResult( const void *pResult ) = 0;

	// Parameter variables
	UINT m_resourceIndex;
	INDEXING_SCENARIO m_is;
	INDEXING_INSTRUCTION m_ii;
	INDEXING_COMPUTATION_METHOD m_icm;
	float m_fTexCoordX;
	float m_fTexCoordY;
	float m_fTexCoordZ;

	// indexable resources
	ID3D11Resource *m_pResources[NUM_INDEXABLE_RESOURCES];
	ID3D11SamplerState *m_pSamplers[NUM_INDEXABLE_RESOURCES];
	ID3D11ShaderResourceView *m_pSRVs[NUM_INDEXABLE_RESOURCES];
	ID3D11UnorderedAccessView *m_pUAVs[NUM_INDEXABLE_RESOURCES];
};


/////////////////////////////////////////////////
// CShader5xTest_IndexTemps
/////////////////////////////////////////////////

class CShader5xTest_IndexTemps : public CShader5xTest_Index
{
public:
	void InitTestParameters();
	virtual TEST_RESULT Setup();

protected:
	TEST_RESULT VerifyResult( const void *pResult );
};

/////////////////////////////////////////////////
// CShader5xTest_IndexConstantBuffers
/////////////////////////////////////////////////

class CShader5xTest_IndexConstantBuffers : public CShader5xTest_Index
{
public:
	void InitTestParameters();
	virtual TEST_RESULT Setup();

protected:
	TEST_RESULT VerifyResult( const void *pResult );
};

/////////////////////////////////////////////////
// CShader5xTest_IndexSamplers
/////////////////////////////////////////////////

class CShader5xTest_IndexSamplers : public CShader5xTest_Index
{
public:
	void InitTestParameters();
	virtual TEST_RESULT Setup();

protected:
	TEST_RESULT VerifyResult( const void *pResult );
};

/////////////////////////////////////////////////
// CShader5xTest_IndexTextures
/////////////////////////////////////////////////

class CShader5xTest_IndexTextures : public CShader5xTest_Index
{
public:
	void InitTestParameters();
	virtual TEST_RESULT Setup();

protected:
	TEST_RESULT VerifyResult( const void *pResult );
};

