#pragma once

#include "Direct3DBase.h"

#define RESOURCE_PATTERN_1 0xBAADBEEF
#define RESOURCE_PATTERN_2 0xDEADBEEF

struct ModelViewProjectionConstantBuffer
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

// This class renders a simple spinning cube.
ref class CubeRenderer sealed : public Direct3DBase
{
public:
	CubeRenderer();

	// Direct3DBase methods.
	void Create2DTextures();
	virtual void CreateDeviceResources() override;
	virtual void CreateWindowSizeDependentResources() override;
	virtual void Render() override;
	
	// Method for updating time-dependent objects.
	void Update(float timeTotal, float timeDelta);

private:
	~CubeRenderer();

	bool m_loadingComplete;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11BlendState1> m_blendState;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Tex1;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Tex2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Tex1View;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Tex2View;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_Sampler;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_Rasterizer;
	uint32* m_pTextureData1;
	uint32* m_pTextureData2;

	uint32 m_indexCount;
	ModelViewProjectionConstantBuffer m_constantBufferData;
};
