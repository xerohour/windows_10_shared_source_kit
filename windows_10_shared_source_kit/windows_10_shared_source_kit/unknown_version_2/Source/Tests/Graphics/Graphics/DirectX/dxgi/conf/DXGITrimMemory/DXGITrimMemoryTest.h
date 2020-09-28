////////////////////////////////////////
// DXGITrimMemoryTest.h

#pragma once

#include "DXGITrimMemory.h"
#include <Mmsystem.h>
#include <d3d10_1.h>
#include <d3d10.h>

#include "d3d11internaltest.h" // Internal interfaces for ID3D11DeviceTest

/////////////////////////////////////////
// CTrimMemoryTest
////////////////////////////////////////

#define TRIM_TEXTURE_COUNT 100
#define TRIM_TEXTURE_DIMENSION 512
#define TRIM_MAPS_PER_FRAME 100
#define RESOURCE_COUNT 5000

struct VertexPositionColor
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Color;
};

struct VertexPositionNormalTexture
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 TexCoord;
};

class CTrimMemoryTest : public CTrimTest
{
public:
	// Constructor/Destructor
	CTrimMemoryTest(void)
		: m_pD3DDevice(nullptr)
		, m_pSwapChain(nullptr)
		, m_pFactory(nullptr)
		, m_pAdapter(nullptr)
		, m_pDepthStencil(nullptr)
		, m_pDepthStencilView(nullptr)
		, m_pRenderTargetView(nullptr)
		, m_rasterizerState(nullptr)
		, m_pTrimData(nullptr)
		, m_pTextures(nullptr)
		, m_pTextureTrimMap(nullptr)
		, m_TextureCount(0)
		, m_TextureCountTrimMap(0)
		, m_pTexture2D(nullptr)
		, m_pTexture2D_Copy(nullptr)
		, m_pEffect(nullptr)
		, m_pEffect2(nullptr)
		, m_pTechnique(nullptr)
		, m_pWorldViewProjVariable(nullptr)
		, m_pVertexLayout(nullptr)
		, m_pBuffers(nullptr)
		, m_pCBuffers(nullptr)
		, m_pBuffers2(nullptr)
		, m_pCBuffers2(nullptr)
		, m_pTrimBufferData(nullptr)
		, m_pTrimCBufferData(nullptr)
		, m_pQuad(nullptr)
		, m_pTextureVariable(nullptr)
		, m_hWnd(nullptr)
	{
	}

	~CTrimMemoryTest(void)
	{
	}

protected:
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	void InitTestParameters();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	void Cleanup();
	HRESULT CreateSwapChain(__in HWND hWnd);
	HRESULT InitD3D( HWND hWnd );
	HRESULT InitGeom();
	HRESULT CreateQuad(LPCSTR path);
	HRESULT createStandardEffect(LPCSTR path);
	VOID Render();
	VOID SetupMatrices();
	void PrepareRect();
	void UpdateTrimResource(UINT i);
	void DrawRect(ID3D10ShaderResourceView * shaderResourceView);
	bool ValidateResources();
	bool CompareTexture2D(D3D10_TEXTURE2D_DESC tex1, D3D10_TEXTURE2D_DESC tex2);
	bool CompareShaderRV(D3D10_SHADER_RESOURCE_VIEW_DESC srv1, D3D10_SHADER_RESOURCE_VIEW_DESC srv2);
	bool CompareBuffers(D3D10_BUFFER_DESC buf1, D3D10_BUFFER_DESC buf2);

protected:
	UINT					m_Scenario;
	ID3D10Device1*			m_pD3DDevice;
	IDXGISwapChain*			m_pSwapChain;
	IDXGIFactory*			m_pFactory;
	IDXGIAdapter*			m_pAdapter;
	ID3D10Texture2D*		m_pDepthStencil;
	ID3D10DepthStencilView* m_pDepthStencilView;
	ID3D10RenderTargetView* m_pRenderTargetView;
	ID3D10RasterizerState*	m_rasterizerState;
	BYTE*					m_pTrimData;
	ID3D10ShaderResourceView**	m_pTextures;
	ID3D10ShaderResourceView*	m_pTextureTrimMap;
	UINT					m_TextureCount;
	ID3D10Texture2D*		m_pTrimTextures[TRIM_TEXTURE_COUNT];
	UINT					m_TextureCountTrimMap;
	ID3D10Texture2D*        m_pTexture2D;
	ID3D10Texture2D*        m_pTexture2D_Copy;
	ID3D10Effect*           m_pEffect;
	ID3D10Effect*           m_pEffect2;
	ID3D10EffectTechnique*	m_pTechnique;
	ID3D10EffectMatrixVariable*	m_pWorldViewProjVariable;
	ID3D10InputLayout*		m_pVertexLayout;
	ID3D10Buffer**			m_pBuffers;
	ID3D10Buffer**			m_pCBuffers;
	ID3D10Buffer**			m_pBuffers2;
	ID3D10Buffer**			m_pCBuffers2;
	BYTE*					m_pTrimBufferData;
	BYTE*					m_pTrimCBufferData;
	ID3D10Buffer*			m_pQuad;
	ID3D10EffectShaderResourceVariable*	m_pTextureVariable;
	HWND					m_hWnd;
	D3D10_TEXTURE2D_DESC	m_DescTexture;
	D3D10_SHADER_RESOURCE_VIEW_DESC m_DescSRV;
	D3D10_TEXTURE2D_DESC	m_DescTexture2;
	D3D10_SHADER_RESOURCE_VIEW_DESC m_DescSRV2;
	D3D10_BUFFER_DESC		m_bufferDesc;
	D3D10_BUFFER_DESC		m_cbufferDesc;
	D3D10_BUFFER_DESC		m_bufferDesc2;
	D3D10_BUFFER_DESC		m_cbufferDesc2;
	VertexPositionColor		m_Vertices[680];
};
