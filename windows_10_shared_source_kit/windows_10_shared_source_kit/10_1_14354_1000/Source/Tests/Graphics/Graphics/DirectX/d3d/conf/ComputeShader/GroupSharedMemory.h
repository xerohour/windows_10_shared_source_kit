//////////////////////////////////////////////////////////////////////////
//  WGFCompute.h
//  created:	2009/03/23
//
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "d3dx11.h"
#include "WGFCompute.h"

using namespace std;

//------------------------------------------------------------------------------------------------
class CTGSMTest : public CComputeTest
{
public:
	CTGSMTest() 
		: m_pUABuffer(NULL)
		, m_pCopyUABuffer(NULL)
		, m_pOutUAV(NULL)
		, m_pCBufferData(NULL)
		, m_pCS(NULL)
	{ 
	}

	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();

protected:

	ID3D11Buffer *m_pCBufferData;
	ID3D11UnorderedAccessView *m_pOutUAV;
	ID3D11ComputeShader *m_pCS;
	UINT m_dispatchX;
	UINT m_dispatchY;
	UINT m_dispatchZ;
	UINT m_numThreadsX;
	UINT m_numThreadsY;
	UINT m_numThreadsZ;

	tstring m_dataType; // uint, float 

	ID3D11Buffer *m_pUABuffer;
	ID3D11Buffer *m_pCopyUABuffer;
	

	TEST_RESULT SetupOutputUAV();
	TEST_RESULT GenerateCBdata( );
	virtual TEST_RESULT SetupShader() = 0;
	virtual bool GetExpectedResult( UINT* pUintResult, float* pFloatResult, double* pDoubleResult = NULL) = 0;

	
};


//------------------------------------------------------------------------------------------------
class CTGSM10Test : public CTGSMTest
{
public:

	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	void InitTestParameters();

protected:
	UINT m_TGSMSize;	// writable TGSM size for each thread

	TEST_RESULT SetupShader();
	bool GetExpectedResult( UINT* pUintResult, float* pFloatResult, double* pDoubleResult = NULL);

};


//------------------------------------------------------------------------------------------------
class CTGSM11Test : public CTGSMTest
{
public:
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	void InitTestParameters();

protected:
	UINT m_TGSMUnitSize;  // number of data in data struct 
	UINT m_numTGSMDcls;
	UINT m_TGSMArraySize;  // only used for dcl_tgsm_structured
	
	tstring m_TGSMDclScenarios;

	TEST_RESULT SetupShader();
	bool GetExpectedResult( UINT* pUintResult, float* pFloatResult, double* pDoubleResult);

};


