//////////////////////////////////////////////////////////////////////////
//  WGFCompute.h
//  created:	2009/03/23
//
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

// Includes
#include "D3D11Test.h"
#include <D3DSampleLocation.h> // for TRException
#include <strsafe.h>

#define ZeroStructMem(x) ZeroMemory(&(x), sizeof(x))
const size_t MAX_DEFINES = 100;

enum SHARED_MEMORY_TYPE
{
	SHARED_MEMORY_TGSM,
	SHARED_MEMORY_UAV,
};
DECLARE_NAMED_VALUES(SHARED_MEMORY_TYPE);

//------------------------------------------------------------------------------------------------
class CComputeTest : public CD3D11Test
{
protected:
	ID3D11Device				*m_pDevice;
	ID3D11DeviceContext			*m_pEffectiveContext;
	ID3D11Resource				*m_pTargetResource;
	ID3D11Resource				*m_pTargetResourceCopy;
	ID3D11UnorderedAccessView	*m_pTargetUAV;
	UINT						m_TargetUAVSize[3];
	DXGI_FORMAT					m_TargetUAVFormat;
	D3D11_UAV_DIMENSION			m_TargetUAVDimension;
	// optional
	D3D11_RESOURCE_MISC_FLAG    m_TargetUAVMiscFlags;
	D3D11_BUFFER_UAV_FLAG		m_TargetUAVBufferFlag;
	UINT						m_TargetUAVArraySize;
	UINT						m_TargetUAVElementByteStride;
	ID3D11ComputeShader			*m_pCS;
	// for test setup of pixel shader stage
	ID3D11InputLayout			*m_pInputLayout;
	ID3D11VertexShader			*m_pVertexShader;
	ID3D11Buffer				*m_pVertexBuffer;
	ID3D11Texture2D *m_pRTTex2D;
	ID3D11RenderTargetView *m_pRTView;

	CComputeTest() 
		: m_pDevice(NULL)
		, m_pEffectiveContext(NULL)
		, m_pTargetResource(NULL)
		, m_pTargetResourceCopy(NULL)
		, m_pTargetUAV(NULL)
		, m_TargetUAVFormat( DXGI_FORMAT_R32_UINT )
		, m_TargetUAVDimension( D3D11_UAV_DIMENSION_TEXTURE2DARRAY )
		, m_TargetUAVMiscFlags( D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )													  
		, m_TargetUAVBufferFlag( static_cast<D3D11_BUFFER_UAV_FLAG>(0) )																																  
		, m_TargetUAVArraySize( 0u )
		, m_TargetUAVElementByteStride( sizeof(UINT) )		 
		, m_pCS(NULL)
		, m_pInputLayout(NULL)
		, m_pVertexShader(NULL)
		, m_pVertexBuffer(NULL)
		, m_pRTTex2D(NULL)
		, m_pRTView(NULL)
	{ 
		m_TargetUAVSize[0] = 1;
		m_TargetUAVSize[1] = 1;
		m_TargetUAVSize[2] = 1;
	}

	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	virtual void CleanupTestCase();
	virtual void Cleanup();

	TEST_RESULT SetupVSStage();
	TEST_RESULT SetupRTV(UINT rtWidth, UINT rtHeight);

	// Helper functions
	HRESULT CompileAndCreateComputeShaderFromMemory(LPCSTR shaderStr, size_t shaderLen, LPCSTR entryPt, D3D_FEATURE_LEVEL minFL, ID3D11ComputeShader** pCS, CONST D3D10_SHADER_MACRO *pDefines = NULL );
	HRESULT CompileAndCreateComputeShaderFromResource(LPCSTR resourceStr, LPCSTR entryPt, D3D_FEATURE_LEVEL minFL, ID3D11ComputeShader** pCS, CONST D3D10_SHADER_MACRO *pDefines = NULL );
	HRESULT CompileAndCreatePixelShaderFromResource(LPCSTR resourceStr, LPCSTR entryPt, D3D_FEATURE_LEVEL minFL, 
													ID3D11PixelShader** ppPS, CONST D3D10_SHADER_MACRO *pDefines );
	bool SkipSlowOnRefWithMessage( bool isSlow ); // Test cases will be skipped on REF if they run too slow 

	template<typename T>
	HRESULT MapAndRead(ID3D11DeviceContext* pContext, ID3D11Resource* pResource, T & outVal)
	{
		HRESULT hr = E_UNEXPECTED;
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if( FAILED( hr = pContext->Map( pResource, 0, D3D11_MAP_READ, 0, &mappedResource ) ) )
			return hr;

		memcpy(&outVal, mappedResource.pData, sizeof(outVal));

		pContext->Unmap( pResource, 0 );

		return hr;
	}

	template<typename T> 
	HRESULT MapAndWrite(ID3D11DeviceContext* pContext, ID3D11Resource* pResource, const T & inVal)
	{
		HRESULT hr = E_UNEXPECTED;
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if( FAILED( hr = pContext->Map( pResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			return hr;

		memcpy(mappedResource.pData, &inVal, sizeof(inVal));

		pContext->Unmap( pResource, 0 );

		return hr;
	}
};

#include "Interaction.h"

//------------------------------------------------------------------------------------------------
class CComputeTestApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	virtual bool InitTestGroups();
	virtual void InitOptionVariables();
	UINT GetDefaultQueryTimeout() { return 30000; }

	bool m_bRunSlowOnRef;
	bool m_bSkipSlowOnHardware;
	bool m_bLogShaderText;
};

extern CComputeTestApp  g_TestApp;
