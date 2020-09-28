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

struct RECORD
{
	UINT incrementID;
	UINT decrementID;
	UINT data;
};

enum UAV_READ_TYPE
{
	READ_ALL_UAVS_APPEND, // use a big append UAV to read data from all write UAVs
	READ_ALL_UAVS_COUNTER, // use a big counter UAV to read data from all write UAVs
	MATCHING_READ_WRITE_UAVS // multi read UAVs with types matching the corresponding write UAVs
};
DECLARE_NAMED_VALUES(UAV_READ_TYPE);

class CCounterUAVTest : public CComputeTest
{
public:
	CCounterUAVTest() 
		: m_pOutputBuf(NULL)
		, m_pCopyOutBuffer(NULL)
		, m_pOutputUAV(NULL)
		, m_pGlobalCounterBuf(NULL)
		, m_pGlobalCounterUAV(NULL)
		, m_pCopySctCountBuf(NULL)
		, m_pSourceBuf(NULL)
		, m_ppWriteUAVs(NULL)
		, m_ppReadUAVs(NULL)
		, m_pCSWriteToCounter(NULL)
		, m_pCSWriteToAppend(NULL)
		, m_pCSReadFromCounter(NULL)
		, m_pCSReadFromCounterSingle(NULL)
		, m_pCSReadFromAppend(NULL)
	{ 
		m_UAVSize = 0;
		m_UAVGap = 0; 
		m_bufferSize = 0;
		m_sizeScale = 0;
		m_dispatchX = 0;
		m_dispatchY = 0;
		m_dispatchZ = 0;
		m_numThreadsX = 0;
		m_numThreadsY = 0;
		m_numThreadsZ = 0;
		m_numUAVsWrite = 0;
		m_numUAVsRead = 0;
		m_bMixedAppend = 0;
		m_bOutBoundRead = 0;
		m_bOutBoundWrite = 0;
		m_bFullUAV = 0;	
		m_bContinuousUAV = 0;
	}

	void InitTestParameters();
	testfactor::RFactor InitCommonTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
//	void Cleanup();


protected:

	ID3D11Buffer *m_pOutputBuf;
	ID3D11Buffer *m_pCopyOutBuffer;
	ID3D11UnorderedAccessView *m_pOutputUAV;
	ID3D11Buffer *m_pGlobalCounterBuf;
	ID3D11UnorderedAccessView *m_pGlobalCounterUAV;
	ID3D11Buffer *m_pSourceBuf;
	ID3D11Buffer *m_pCopySctCountBuf;
	ID3D11UnorderedAccessView **m_ppWriteUAVs;
	ID3D11UnorderedAccessView **m_ppReadUAVs;
	ID3D11ComputeShader *m_pCSWriteToCounter;
	ID3D11ComputeShader *m_pCSWriteToAppend;
	ID3D11ComputeShader *m_pCSReadFromCounter;
	ID3D11ComputeShader *m_pCSReadFromCounterSingle;
	ID3D11ComputeShader *m_pCSReadFromAppend;
	UINT m_bufferSize;
	UINT m_UAVSize;
	UINT m_UAVGap; // the gap in bytes between the UAVs if they are not continuous
	UINT m_sizeScale; // For the scenario that the UAV should not be full; multiply the filled size by m_sizeScale

	// test parameters
	UINT m_dispatchX;
	UINT m_dispatchY;
	UINT m_dispatchZ;
	UINT m_numThreadsX;
	UINT m_numThreadsY;
	UINT m_numThreadsZ;
	UINT m_numUAVsWrite;
	UINT m_numUAVsRead;
	bool m_bMixedAppend;
	bool m_bOutBoundRead;
	bool m_bOutBoundWrite;
	bool m_bFullUAV;	// if the UAV is full after write
	bool m_bContinuousUAV;	// if two UAVs are continuous in memory	
	UAV_READ_TYPE m_readUAVType;

	TEST_RESULT SetupUAVs();
	TEST_RESULT SetupShader();
	TEST_RESULT VerifyStructureCount(ID3D11UnorderedAccessView* pUAVWithCount, UINT expectVal);
	TEST_RESULT VerifyResultData(ID3D11UnorderedAccessView* pUAVDataWindow);
	
};



class CCounterUAVPSTest : public CCounterUAVTest
{
public:
	CCounterUAVPSTest()
		: m_pPSWriteToCounter(NULL)
		, m_pPSWriteToAppend(NULL)
		, m_pPSReadFromCounter(NULL)
		, m_pPSReadFromAppend(NULL)
	{
		m_ViewWidth = 0;
	}

	void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	TEST_RESULT Setup();
	void Cleanup();

protected:
	TEST_RESULT SetupUAVs();
	TEST_RESULT SetupShader();

	UINT m_ViewWidth;
	// UINT m_ViewHeight;
	ID3D11PixelShader *m_pPSWriteToCounter;
	ID3D11PixelShader *m_pPSWriteToAppend;
	ID3D11PixelShader *m_pPSReadFromCounter;
	ID3D11PixelShader *m_pPSReadFromAppend;
	
	// pixel shader with UAV only, or with both UAV and RTVs
	bool		m_bUAVOnly;

};
