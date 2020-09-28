#pragma once

#include "WGFCompute.h"
#include <StringUtils.h>

static const int MAX_MEMORY_SOURCE_RESOURCES = 2;

enum MemoryBarrierType
{
	MemoryBarrier_NoBarrier = 0,
	MemoryBarrier_GroupMem = 1,
	MemoryBarrier_DeviceMem = 2,
	MemoryBarrier_AllMem = 3,
	// the followings are private intrinsics for _ugroup sync instructions which are only valid on debug compiler
	MemoryBarrier_AllMemGroup = 4,
	MemoryBarrier_AllMemGroupSync = 5,
	MemoryBarrier_DeviceMemGroup = 6,
	MemoryBarrier_DeviceMemGroupSync = 7,
};

DECLARE_NAMED_VALUES(MemoryBarrierType);

enum UAV_RESOURCE_TYPE
{
	RAW_BUFFER = 1,
	TYPED_BUFFER,
	STRUCTURED_BUFFER,
	TEXTURE1D,
	TEXTURE1DARRAY,
	TEXTURE2D,
	TEXTURE2DARRAY,
	TEXTURE3D,
};  

DECLARE_NAMED_VALUES(UAV_RESOURCE_TYPE);

class CComputeMemoryTest : public CComputeTest
{
protected:
	CComputeMemoryTest()
		: m_UAVSourceType(RAW_BUFFER)
		, m_BarrierType(MemoryBarrier_NoBarrier)
		, m_NumMemorySourceResources(MAX_MEMORY_SOURCE_RESOURCES)
		, m_SourceUAVBufferFlag( 0 )
	{
		for (int i = 0; i < MAX_MEMORY_SOURCE_RESOURCES; ++i)
		{
			m_pSourceUAVs[i] = NULL;
			m_pSourceResources[i] = NULL;
		}
	}

	virtual TEST_RESULT SetupTestCase();
	virtual void CleanupTestCase();

protected:
	MemoryBarrierType			m_BarrierType;
	UAV_RESOURCE_TYPE			m_UAVSourceType;
	UINT						m_NumThreads[3];
	ID3D11Resource				*m_pSourceResources[MAX_MEMORY_SOURCE_RESOURCES];
	ID3D11UnorderedAccessView	*m_pSourceUAVs[MAX_MEMORY_SOURCE_RESOURCES];
	UINT						m_NumMemorySourceResources;
	UINT						m_SourceUAVBufferFlag;
};

class CMemorySyncTest : public CComputeMemoryTest
{
public:
	CMemorySyncTest()
	{
	};

	virtual void InitTestParameters();

	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();

protected:
	virtual tstring GetShaderCode();
	TEST_RESULT CreateSourceUAVs();

private:
};

class CMemoryBarrierTest : public CComputeMemoryTest
{
	public:
		CMemoryBarrierTest()
			: m_pConstBuffer(NULL)
		{
			//m_UAVDimension = D3D11_UAV_DIMENSION_BUFFER;
			m_SourceUAVBufferFlag = 0;
			m_NumMemorySourceResources = 0;
			m_ResultOffset = 0;
		}

		virtual void InitTestParameters();
		virtual TEST_RESULT Setup();
		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();
		virtual void CleanupTestCase();
	
	private:
		
		bool LoadStringFileFromResource(const TCHAR* pFileName, tstring& s );

		ID3D11Buffer* m_pConstBuffer;
		//tstring m_ShaderCode;

		// Parameters
		unsigned int m_NumGroups[3];
		unsigned int m_NumGroupThreads[3];
		unsigned int m_CheckWindowSize;
		unsigned int m_NumSharedMemUpdates;
		unsigned int m_NumSharedMemChecks;
		unsigned int m_TargetUAVStructureStrideMin;
		unsigned int m_ResultOffset;
		MemoryBarrierType m_MemoryBarrierType;
		bool m_UseGroupSharedMemoryFlag;
		bool m_UseCrossGroupReads;
};

///////////////////////////////////////////////////////////////////////////////////
class CMemoryBarrierPSTest : public CMemoryBarrierTest
{
public:
	CMemoryBarrierPSTest():
		m_ViewWidth(0),
		m_ViewHeight(0),
		m_pPixelShader(NULL),
		m_pResultUAVBuffer(NULL),
		m_pResultCopyBuffer(NULL)
		{
			 m_ppUAVs[0] =  m_ppUAVs[1] =  m_ppUAVs[2] = NULL;
			 m_ppUAVBuf[0] = m_ppUAVBuf[1] = NULL;
			 m_ppUAVTex1D[0] = m_ppUAVTex1D[1] = NULL;
			 m_ppUAVTex2D[0] = m_ppUAVTex2D[1] = NULL;
			 m_ppUAVTex3D[0] = m_ppUAVTex3D[1] = NULL;
		};

		virtual void InitTestParameters();
		virtual TEST_RESULT Setup();
		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();
		virtual void CleanupTestCase();
		virtual void Cleanup();
private:
		UINT m_ViewWidth;
		UINT m_ViewHeight;
		UAV_RESOURCE_TYPE m_TypeUAV[2];
		tstring m_DataTypeUAV[2];
	
		ID3D11PixelShader *m_pPixelShader;
		
		// 3 UAVs; the first 2 are read/write UAVs; the last one is the result UAV
		ID3D11UnorderedAccessView * m_ppUAVs[3];
		ID3D11Buffer * m_ppUAVBuf[2];
		ID3D11Texture1D * m_ppUAVTex1D[2];
		ID3D11Texture2D * m_ppUAVTex2D[2];
		ID3D11Texture3D * m_ppUAVTex3D[2];

		ID3D11Buffer * m_pResultUAVBuffer;
		ID3D11Buffer * m_pResultCopyBuffer;

		TEST_RESULT CreateUAVs();
		TEST_RESULT SetupPixelShader();
		
};
