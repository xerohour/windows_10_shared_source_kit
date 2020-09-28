#include "pch.h"
#include "CubeRenderer.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;

CubeRenderer::CubeRenderer() :
	m_loadingComplete(false),
	m_indexCount(0)
{
}

CubeRenderer::~CubeRenderer()
{
	if(m_pTextureData1)
		delete [] m_pTextureData1;

	if(m_pTextureData2)
		delete [] m_pTextureData2;
}

void CubeRenderer::CreateDeviceResources()
{
	Direct3DBase::CreateDeviceResources();

	auto loadVSTask = DX::ReadDataAsync("SimpleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync("SimplePixelShader.cso");

	auto createVSTask = loadVSTask.then([this](Platform::Array<byte>^ fileData) {
		DX::ThrowIfFailed(
			m_d3dDevice->CreateVertexShader(
 				fileData->Data,
				fileData->Length,
				nullptr,
				&m_vertexShader
				)
			);

		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
		{
			{ "VERTEXID", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_d3dDevice->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				fileData->Data,
				fileData->Length,
				&m_inputLayout
				)
			);

		Create2DTextures();
	});

	auto createPSTask = loadPSTask.then([this](Platform::Array<byte>^ fileData) {
		DX::ThrowIfFailed(
			m_d3dDevice->CreatePixelShader(
				fileData->Data,
				fileData->Length,
				nullptr,
				&m_pixelShader
				)
			);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_d3dDevice->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
				)
			);

		CD3D11_SAMPLER_DESC sampDesc(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, 0, 1, D3D11_COMPARISON_ALWAYS, 0, 0, 0);
		DX::ThrowIfFailed(
			m_d3dDevice->CreateSamplerState(
				&sampDesc,
				&m_Sampler
				)
			);

		CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, TRUE, 0, 0, 0, TRUE, FALSE, FALSE, FALSE);
		DX::ThrowIfFailed(
			m_d3dDevice->CreateRasterizerState(
				&rastDesc,
				&m_Rasterizer
				)
			);
	});

	auto createCubeTask = (createPSTask && createVSTask).then([this] () {
		UINT cubeVertices[] = {0,1,2,3,4,5,6,7};

		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_d3dDevice->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
				)
			);

		unsigned short cubeIndices[] = 
		{
			0,2,1, // -x
			1,2,3,

			4,5,6, // +x
			5,7,6,

			0,1,5, // -y
			0,5,4,

			2,6,7, // +y
			2,7,3,

			0,4,6, // -z
			0,6,2,

			1,3,7, // +z
			1,7,5,
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_d3dDevice->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
				)
			);

        D3D11_BLEND_DESC1 blendDesc = {};
  
        for (UINT i = 0; i < ARRAYSIZE(blendDesc.RenderTarget); i++)  
        {  
            blendDesc.RenderTarget[i].BlendEnable = TRUE;  
        }  
  
        for (UINT i = 0; i < ARRAYSIZE(blendDesc.RenderTarget); i++)  
        {  
            blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;  
        }  
  
        blendDesc.RenderTarget[0].SrcBlend  = blendDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp   = blendDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;

        DX::ThrowIfFailed(
            m_d3dDevice->CreateBlendState1(
                &blendDesc,
                &m_blendState
                )
            );
	});

	createCubeTask.then([this] () {
		m_loadingComplete = true;
	});
}

void CubeRenderer::Create2DTextures()
{
	uint32 Width = 1024;
	uint32 Height = 1024;
	DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	uint32 BytesPerPixel = 4;
	uint32 SizeInBytes = Width * Height * BytesPerPixel;
	uint32 DataSize = SizeInBytes/sizeof(uint32);

	m_pTextureData1 = new uint32[DataSize];
	m_pTextureData2 = new uint32[DataSize];

	if(!m_pTextureData1 || !m_pTextureData2)
		DX::ThrowIfFailed(E_OUTOFMEMORY);

	for(uint32 i = 0; i < DataSize; i++)
	{
		m_pTextureData1[i] = RESOURCE_PATTERN_1 + i;
		m_pTextureData2[i] = RESOURCE_PATTERN_2 + i;
	}

	D3D11_SUBRESOURCE_DATA subResData1;
	subResData1.pSysMem = m_pTextureData1;
	subResData1.SysMemPitch = SizeInBytes/Height; 
	subResData1.SysMemSlicePitch = 0;
	
	D3D11_SUBRESOURCE_DATA subResData2;
	subResData2.pSysMem = m_pTextureData2;
	subResData2.SysMemPitch = SizeInBytes/Height; 
	subResData2.SysMemSlicePitch = 0;

	CD3D11_TEXTURE2D_DESC Desc(DXGIFormat, Width, Height, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, 1, 0, 0);

	DX::ThrowIfFailed(
		m_d3dDevice->CreateTexture2D(
			&Desc, 
			&subResData1, 
			&m_Tex1)
			);

	DX::ThrowIfFailed(
		m_d3dDevice->CreateTexture2D(
			&Desc, 
			&subResData2, 
			&m_Tex2)
			);

	CD3D11_SHADER_RESOURCE_VIEW_DESC viewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGIFormat, 0, 1, 0, 1, 0);

	DX::ThrowIfFailed(
		m_d3dDevice->CreateShaderResourceView(
			m_Tex1.Get(),
			&viewDesc,
			&m_Tex1View)
			);

	DX::ThrowIfFailed(
		m_d3dDevice->CreateShaderResourceView(
			m_Tex2.Get(),
			&viewDesc,
			&m_Tex2View)
			);
}

void CubeRenderer::CreateWindowSizeDependentResources()
{
	Direct3DBase::CreateWindowSizeDependentResources();

	float aspectRatio = m_windowBounds.Width / m_windowBounds.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// Note that the m_orientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.
	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(
			XMMatrixMultiply(
				XMMatrixPerspectiveFovRH(
					fovAngleY,
					aspectRatio,
					0.01f,
					100.0f
					),
				XMLoadFloat4x4(&m_orientationTransform3D)
				)
			)
		);
}

void CubeRenderer::Update(float timeTotal, float timeDelta)
{
	(void) timeDelta; // Unused parameter.

	XMVECTOR eye = XMVectorSet(0.0f, 0.7f, 1.5f, 0.0f);
	XMVECTOR at = XMVectorSet(0.0f, -0.1f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(timeTotal * XM_PIDIV4)));
}

void CubeRenderer::Render()
{
	const float midnightBlue[] = { 0.098f, 0.098f, 0.439f, 1.000f };
	m_d3dContext->ClearRenderTargetView(
		m_renderTargetView.Get(),
		midnightBlue
		);

	m_d3dContext->ClearDepthStencilView(
		m_depthStencilView.Get(),
		D3D11_CLEAR_DEPTH,
		1.0f,
		0
		);

	// Only draw the cube once it is loaded (loading is asynchronous).
	if (!m_loadingComplete)
	{
		return;
	}

	m_d3dContext->OMSetRenderTargets(
		1,
		m_renderTargetView.GetAddressOf(),
		m_depthStencilView.Get()
		);

    m_d3dContext->OMSetBlendState(m_blendState.Get(), nullptr, UINT_MAX);

    int const NumberOfCubes = 25;

    for (int i = 0; i < NumberOfCubes; ++i)
    {
        if (i != 0)
        {
            Update(i * 0.02f, 0.0f);
        }

	    m_d3dContext->UpdateSubresource(
		    m_constantBuffer.Get(),
		    0,
		    NULL,
		    &m_constantBufferData,
		    0,
		    0
		    );

	    UINT stride = sizeof(UINT);
	    UINT offset = 0;
	    m_d3dContext->IASetVertexBuffers(
		    0,
		    1,
		    m_vertexBuffer.GetAddressOf(),
		    &stride,
		    &offset
		    );

	    m_d3dContext->IASetIndexBuffer(
		    m_indexBuffer.Get(),
		    DXGI_FORMAT_R16_UINT,
		    0
		    );

	    m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

	    m_d3dContext->VSSetShader(
		    m_vertexShader.Get(),
		    nullptr,
		    0
		    );

	    m_d3dContext->VSSetConstantBuffers(
		    0,
		    1,
		    m_constantBuffer.GetAddressOf()
		    );

	    m_d3dContext->PSSetShader(
		    m_pixelShader.Get(),
		    nullptr,
		    0
		    );

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> View;
		
		m_d3dContext->PSGetShaderResources(
			0,
			1,
			&View
			);

		m_d3dContext->PSSetSamplers(0, 1, m_Sampler.GetAddressOf());

		if(View == m_Tex1View)
			View = m_Tex2View;
		else
			View = m_Tex1View;

		m_d3dContext->PSSetShaderResources(
			0,
			1,
			View.GetAddressOf()
			);

		m_d3dContext->PSGetShaderResources(
			0,
			1,
			View.GetAddressOf()
			);

		m_d3dContext->RSSetState(m_Rasterizer.Get());

	    m_d3dContext->DrawIndexed(
		    m_indexCount,
		    0,
		    0
		    );
    }
}
