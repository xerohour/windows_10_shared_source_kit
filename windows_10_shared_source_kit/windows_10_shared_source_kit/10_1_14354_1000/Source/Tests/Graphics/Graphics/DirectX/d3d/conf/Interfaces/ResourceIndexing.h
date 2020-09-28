#pragma once

// Includes
#include "wgfinterfaces.h"
#include "d3dx11async.h" // Needed for D3DX11CompileFromResource

// Defines
const tstring g_successClassNamePrefix = "Batman";
const tstring g_failClassNamePrefix = "Superman"; 

enum D3D_RESOURCE_TYPE
{
	// Textures/Buffers
	D3D_RESOURCE_TYPE_BUFFER,
	D3D_RESOURCE_TYPE_TEXTURE1D,
	D3D_RESOURCE_TYPE_TEXTURE1D_ARRAY,
	D3D_RESOURCE_TYPE_TEXTURE2D,
	D3D_RESOURCE_TYPE_TEXTURE2D_ARRAY,
	D3D_RESOURCE_TYPE_TEXTURE2D_MS,
	D3D_RESOURCE_TYPE_TEXTURE2D_MS_ARRAY,
	D3D_RESOURCE_TYPE_TEXTURECUBE,
	D3D_RESOURCE_TYPE_TEXTURE3D,

	// Constant buffers
	D3D_RESOURCE_TYPE_CONSTANT_BUFFER,

	// Samplers
	D3D_RESOURCE_TYPE_SAMPLER,
};

DECLARE_NAMED_VALUES( D3D_RESOURCE_TYPE );

enum FAILING_INSTANCE
{
	FAILING_INSTANCE_BY_SLOT,
	FAILING_INSTANCE_BY_OFFSET
};

DECLARE_NAMED_VALUES( FAILING_INSTANCE );

//------------------------------------------------------------------------------------------------
// CResourceIndexing
//------------------------------------------------------------------------------------------------

class CResourceIndexing :
	public CInterfacesTest
{
public:
	struct VSIN
	{
		UINT data;
	};

protected:
	D3D_SHADER_STAGE	m_shaderStage;
	bool				m_resourceBound;
	UINT				m_successInstanceSlot;	// The index to bind the "success" interface to
	D3D_RESOURCE_TYPE	m_resourceType;				// Buffer, 1D, 2D, etc...
	bool                m_bConstantBufferForInstanceSlotInitialized;
	UINT                m_statisticsQueryTimeout = 0;
	
	ID3D11ClassInstance			*m_pSuccessInstance;
	ID3D11ClassInstance			*m_pFailInstance;
	ID3D11ClassInstance			*m_pInstances[MAX_INTERFACE_SLOTS];
	ID3D11VertexShader			*m_pVertexShader[2]; //[0] is the test shader, [1] is the passthrough shader
	ID3D11HullShader            *m_pHullShader[2];
	ID3D11DomainShader          *m_pDomainShader[2];
	ID3D11GeometryShader		*m_pGeometryShader[2];
	ID3D11PixelShader           *m_pPixelShader;
	ID3D11ComputeShader         *m_pComputeShader;

	ID3D11Buffer				*m_pConstantBufferForInstanceSlot;
	ID3D11Buffer				*m_pOutputStagingBuffer;
	ID3D11Buffer				*m_pOutputBuffer;
	ID3D11UnorderedAccessView   *m_pUAV;
	ID3D11RenderTargetView      *m_pRTV;

	ID3D11Query					*m_pSOStatsQuery;
	ID3D11Query					*m_pSOOverflowPredicate;
	ID3D11Query					*m_pPipelineStatsQuery;

public:
	CResourceIndexing(void)
		: m_resourceType(D3D_RESOURCE_TYPE_BUFFER)
		, m_successInstanceSlot(0)
		, m_resourceBound(true)
		, m_shaderStage(D3D_SHADER_STAGE_VERTEX)
		, m_pPixelShader(NULL)
		, m_pComputeShader(NULL)
		, m_pOutputBuffer(NULL)
		, m_pOutputStagingBuffer(NULL)
		, m_pUAV(NULL)
		, m_pRTV(NULL)
		, m_pConstantBufferForInstanceSlot(NULL)
		, m_pSuccessInstance(NULL)
		, m_pFailInstance(NULL)
		, m_pSOStatsQuery(NULL)
		, m_pSOOverflowPredicate(NULL)
		, m_pPipelineStatsQuery(NULL)
		, m_bConstantBufferForInstanceSlotInitialized(false)
	{
		ZeroMemory( m_pVertexShader,   sizeof(m_pVertexShader) );
		ZeroMemory( m_pHullShader,     sizeof(m_pHullShader) );
		ZeroMemory( m_pDomainShader,   sizeof(m_pDomainShader) );
		ZeroMemory( m_pGeometryShader, sizeof(m_pGeometryShader) );

		// Local variables
		m_numClassInstances	= ARRAY_SIZE(m_pInstances);

		for( UINT i = 0; i < m_numClassInstances; ++i )
		{
			m_pInstances[i] = NULL;
		}
	}

	virtual ~CResourceIndexing(void);

	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();
	virtual void Cleanup();


protected:
	// Pure virtual functions
	virtual tstring GetFXFileToCompile() const = 0;
	virtual UINT GetConstantBufferOffset( bool isSuccessClass = true ) const = 0;
	virtual UINT GetConstantVectorOffset( bool isSuccessClass = true ) const = 0;
	virtual UINT GetTextureOffset( bool isSuccessClass = true ) const = 0;
	virtual UINT GetSamplerOffset( bool isSuccessClass = true ) const = 0;
	virtual UINT GetShaderOutputSize() const = 0;
	virtual TEST_RESULT SetupResources() = 0;
	virtual TEST_RESULT VerifyResult( const D3D11_MAPPED_SUBRESOURCE &mappedResource ) = 0;

private:
	// Helper funtions
	TEST_RESULT SetupInterfaces();
	TEST_RESULT CreateShaders();
	TEST_RESULT CreateConstantBuffers();
	TEST_RESULT SetupOutputBuffers();
	TEST_RESULT SetupQueries();
};

//------------------------------------------------------------------------------------------------
// CTextureIndexing
//------------------------------------------------------------------------------------------------

class CTextureIndexing :
	public CResourceIndexing
{
protected:
	UINT				m_successTextureSlot;
	UINT				m_failTextureSlot;

	ID3D11ShaderResourceView	*m_pShaderResourceViews[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
	ID3D11Resource				*m_pResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

public:
	CTextureIndexing(void)
		: m_successTextureSlot(0)
		, m_failTextureSlot(1)
	{
		// Local variables
		const UINT numTextures	= ARRAY_SIZE(m_pResources);

		for( UINT i = 0; i < numTextures; ++i )
		{
			m_pResources[i] = NULL;
			m_pShaderResourceViews[i] = NULL;
		}
	}

	virtual ~CTextureIndexing(void);

	virtual tstring GetFXFileToCompile() const { return "TextureIndexing.fx"; };
	virtual UINT GetConstantBufferOffset( bool isSuccessClass = true ) const { return 0; };
	virtual UINT GetConstantVectorOffset( bool isSuccessClass = true ) const { return 0; };
	virtual UINT GetTextureOffset( bool isSuccessClass = true ) const { return isSuccessClass ? m_successTextureSlot : m_failTextureSlot; };
	virtual UINT GetSamplerOffset( bool isSuccessClass = true ) const { return 0; };
	virtual UINT GetShaderOutputSize() const { return sizeof(UINT); };	// Get one UINT value, the info read from the texture
	virtual TEST_RESULT VerifyResult( const D3D11_MAPPED_SUBRESOURCE &mappedResource );
	
	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual void CleanupTestCase();
	virtual TEST_RESULT SetupResources();

private:
	UINT GetAFailingTextureSlot() const;
	bool IsFailingTextureSlot( const UINT &index ) const { return (( index != m_successTextureSlot ) && ( index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT )); };
	UINT GetExpectedDataForResourceInSlot(const UINT &slot) const { return slot + 1; };
};

//------------------------------------------------------------------------------------------------
// CSamplerIndexing
//------------------------------------------------------------------------------------------------

class CSamplerIndexing :
	public CResourceIndexing
{
protected:
	UINT				m_successSamplerSlot;
	UINT				m_failSamplerSlot;

	ID3D11SamplerState			*m_pSamplerStates[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
	ID3D11Texture2D				*m_pTexture2D;
	ID3D11ShaderResourceView	*m_pShaderResourceView;


public:
	CSamplerIndexing(void)
		: m_successSamplerSlot(0)
		, m_failSamplerSlot(1)
		, m_pTexture2D(NULL)
		, m_pShaderResourceView(NULL)
	{
		// Local variables
		const UINT numSamplers	= ARRAY_SIZE(m_pSamplerStates);

		for( UINT i = 0; i < numSamplers; ++i )
		{
			m_pSamplerStates[i] = NULL;
		}
	}

	virtual ~CSamplerIndexing(void);

	virtual tstring GetFXFileToCompile() const { return "SamplerIndexing.fx"; };
	virtual UINT GetConstantBufferOffset( bool isSuccessClass = true ) const { return 0; };
	virtual UINT GetConstantVectorOffset( bool isSuccessClass = true ) const { return 0; };
	virtual UINT GetTextureOffset( bool isSuccessClass = true ) const { return 0; };
	virtual UINT GetSamplerOffset( bool isSuccessClass = true ) const { return isSuccessClass ? m_successSamplerSlot : m_failSamplerSlot; };
	virtual UINT GetShaderOutputSize() const { return sizeof(UINT); };	// Get one UINT value, the info read from the texture
	virtual TEST_RESULT VerifyResult( const D3D11_MAPPED_SUBRESOURCE &mappedResource );
	
	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual void CleanupTestCase();
	virtual TEST_RESULT SetupResources();

private:
	UINT GetAFailingSamplerSlot() const;
	bool IsFailingSamplerSlot( const UINT &index ) const { return (( index != m_successSamplerSlot ) && ( index < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT )); };
	UINT GetExpectedDataForResourceInSlot(const UINT &slot) const { return slot + 1; };
};

//------------------------------------------------------------------------------------------------
// CConstantBufferIndexing
//------------------------------------------------------------------------------------------------

class CConstantBufferIndexing :
	public CResourceIndexing
{
protected:
	UINT				m_successConstantBufferSlot;
	UINT				m_successConstantBufferOffset;
	UINT				m_failConstantBufferSlot;
	UINT				m_failConstantBufferOffset;
	UINT				m_numElementsInConstantBuffer;
	FAILING_INSTANCE	m_failBy;
	ID3D11Buffer		*m_ppConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];

public:
	CConstantBufferIndexing(void)
		: m_successConstantBufferSlot(0)
		, m_successConstantBufferOffset(0)
		, m_failConstantBufferSlot(1)
		, m_failConstantBufferOffset(1)
		, m_numElementsInConstantBuffer(5)
		, m_failBy(FAILING_INSTANCE_BY_SLOT)
	{
		// Local variables
		const UINT numConstantBuffers = ARRAY_SIZE(m_ppConstantBuffers);

		for( UINT i = 0; i < numConstantBuffers; ++i )
		{
			m_ppConstantBuffers[i] = NULL;
		}
	}

	virtual ~CConstantBufferIndexing(void);

	virtual tstring GetFXFileToCompile() const { return "ConstantBufferIndexing.fx"; };
	virtual UINT GetConstantBufferOffset( bool isSuccessClass = true ) const { return isSuccessClass ? m_successConstantBufferSlot : m_failConstantBufferSlot; };
	virtual UINT GetConstantVectorOffset( bool isSuccessClass = true ) const { return isSuccessClass ? m_successConstantBufferOffset : m_failConstantBufferOffset; };
	virtual UINT GetTextureOffset( bool isSuccessClass = true ) const { return 0; };
	virtual UINT GetSamplerOffset( bool isSuccessClass = true ) const { return 0; };
	virtual UINT GetShaderOutputSize() const { return sizeof(UINT); };	// Get one UINT value, the info read from the constant buffer
	virtual TEST_RESULT VerifyResult( const D3D11_MAPPED_SUBRESOURCE &mappedResource );
	
	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual void CleanupTestCase();
	virtual TEST_RESULT SetupResources();

private:
	UINT GetAFailingConstantBufferSlot() const;
	UINT GetAFailingConstantBufferOffset() const;
	bool IsFailingConstantBufferSlot( const UINT &index ) const { return (( index != m_successConstantBufferSlot ) && ( index < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT )); };
	bool IsFailingConstantBufferOffset( const UINT &index ) const { return (( index != m_successConstantBufferOffset ) && ( index < m_numElementsInConstantBuffer )); };
	int GetExpectedDataForResourceInSlot(const UINT &slot, const UINT &componentOffset) const;
};

