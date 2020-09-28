////////////////////////////////////////////////////////////////////////////////
// Teapots.cpp
////////////////////////////////////////////////////////////////////////////////

#include "Frequency.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
using namespace DirectX;
#include "Teapots.h"
#define NUM_TEAPOTS 2000

////////////////////////////////////////////////////////////////////////////////
// CFrequencyTest_Teapot
////////////////////////////////////////////////////////////////////////////////

/// Constructor for our renderer, just set some default parameters
CFrequencyTest_Teapot::CFrequencyTest_Teapot() :
feature_level(D3D_FEATURE_LEVEL_11_0), m_device(NULL), m_immediate_context(NULL), m_swap_chain(NULL), m_deferred_context(NULL), m_context(NULL), m_index_buffer(NULL), m_saved_scene_blob(NULL),
m_back_buffer(NULL), m_render_target_view(NULL), m_depth_stencil_view(NULL), m_depth_stencil(NULL), m_vertex_buffer(NULL), teapot_data(NULL),
m_vertex_shader(NULL), m_pixel_shader(NULL), m_pixel_shader_solid(NULL), m_vertex_layout(NULL), m_constant_buffer(NULL), m_instance_buffer(NULL),
window_handle(NULL), m_staging_texture(NULL), CFrequencyTest()
{
}

// Cleanup!
CFrequencyTest_Teapot::~CFrequencyTest_Teapot()
{
    if(!GetFramework()->IsCompat())
    {
        if (m_context) m_context->ClearState();
        SAFE_RELEASE(m_render_target_view);
        SAFE_RELEASE(m_depth_stencil);
        SAFE_RELEASE(m_depth_stencil_view);
        SAFE_RELEASE(m_immediate_context);
        SAFE_RELEASE(m_deferred_context);
        SAFE_RELEASE(m_swap_chain);
        SAFE_RELEASE(m_back_buffer);
        SAFE_RELEASE(m_vertex_buffer);
        SAFE_RELEASE(m_index_buffer);
        SAFE_RELEASE(m_device);
        SAFE_RELEASE(m_vertex_shader);
        SAFE_RELEASE(m_pixel_shader);
        SAFE_RELEASE(m_pixel_shader_solid);
        SAFE_RELEASE(m_vertex_layout);
        SAFE_RELEASE(m_constant_buffer);
        SAFE_RELEASE(m_instance_buffer);
        SAFE_RELEASE(m_saved_scene_blob);
        SAFE_RELEASE(m_staging_texture);
        _aligned_free(teapot_data);
    }
}

HRESULT CFrequencyTest_Teapot::InitializeWindow()
{
    // Register a window class
    return S_OK;
}

/// Initialize the vertex data for our teapots
HRESULT CFrequencyTest_Teapot::InitializeVertexData()
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex)* NUMTEAPOTVERTICES;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = teapot_vertices;
    hr = m_device->CreateBuffer(&bd, &InitData, &m_vertex_buffer);
    if (FAILED(hr))
        return hr;

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

    // Set instance buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(XMMATRIX)* maximum_instances;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = NULL;
    hr = m_device->CreateBuffer(&bd, NULL, &m_instance_buffer);
    if (FAILED(hr))
        return hr;

    // Set vertex instance buffer
    stride = sizeof(XMMATRIX);
    offset = 0;
    m_context->IASetVertexBuffers(1, 1, &m_instance_buffer, &stride, &offset);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD)* NUMTEAPOTINDICES;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = teapot_indices;
    hr = m_device->CreateBuffer(&bd, &InitData, &m_index_buffer);
    if (FAILED(hr))
        return hr;

    // Set index buffer
    m_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_device->CreateBuffer(&bd, NULL, &m_constant_buffer);

    return hr;
}

HRESULT CFrequencyTest_Teapot::InitializeShaders()
{
    HRESULT hr = S_OK;
    DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shader_flags |= D3DCOMPILE_DEBUG;
#endif

    // Vertex shader first
    hr = m_device->CreateVertexShader(vertex_shader_bytes, ARRAYSIZE(vertex_shader_bytes), NULL, &m_vertex_shader);

    if (SUCCEEDED(hr))
    {
        // Define the input layout
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        };
        UINT numElements = ARRAYSIZE(layout);

        // Create the input layout
        hr = m_device->CreateInputLayout(layout, numElements, vertex_shader_bytes, ARRAYSIZE(vertex_shader_bytes), &m_vertex_layout);
        if (SUCCEEDED(hr))
        {
            // Set the input layout
            m_context->IASetInputLayout(m_vertex_layout);
        }
    }

    // Now the pixel shader
    hr = m_device->CreatePixelShader(pixel_shader_bytes, ARRAYSIZE(pixel_shader_bytes), NULL, &m_pixel_shader);

    return hr;
}

BYTE GetByte(DWORD number)
{
    return (BYTE)(number & 0xFF);
}
FLOAT GetColor(DWORD color)
{
    return GetByte(color) / 255.0f;
}

void CFrequencyTest_Teapot::UpdateTeapotData(DWORD number_objects)
{
    // Update all of the teapot data
    for (DWORD i = 0; i < number_objects; i++)
    {
        float x = teapot_data[i].Position.x + teapot_data[i].Velocity.x;
        float y = teapot_data[i].Position.y + teapot_data[i].Velocity.y;
        float z = teapot_data[i].Position.z + teapot_data[i].Velocity.z;
        teapot_data[i].Position = XMFLOAT3(x, y, z);
        teapot_data[i].Rotation += teapot_data[i].RotationSpeed;
    }
}

HRESULT CFrequencyTest_Teapot::Render(DWORD number_objects, DWORD camera_zoom, DWORD box_size, DWORD light_color)
{
    HRESULT hr = S_OK;
    ID3D11DeviceContext2 *pContext2 = nullptr;
    hr = m_context->QueryInterface(__uuidof(ID3D11DeviceContext2), (void **)&pContext2);
    if (FAILED(hr))
    {
        return hr;
    }

    ID3DUserDefinedAnnotation* g_pPerf = nullptr;
    hr = m_context->QueryInterface(__uuidof(g_pPerf), reinterpret_cast<void**>(&g_pPerf));	//get perf info interface
    if (FAILED(hr))
    {
        return hr;
    }

    // Clamp the max number of objects
    number_objects = (number_objects <= maximum_instances) ? number_objects : maximum_instances;

    hr = InitializeTeapot(number_objects, box_size);
    if (FAILED(hr))
    {
        return hr;
    }

    // Update the rendering settings
    // Set up our matrices
    XMMATRIX world;
    XMMATRIX view;
    XMMATRIX proj;
    float zoom = (FLOAT)camera_zoom;

    world = XMMatrixIdentity();
    XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -zoom, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    view = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_width / (FLOAT)m_height, 0.01f, 100.0f);

    // Setup our lighting parameters
    XMFLOAT4 vLightDirs[2] =
    {
        XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f),
        XMFLOAT4(0.0f, -0.3f, -1.0f, 1.0f),
    };

    XMFLOAT4 vLightColors[2] =
    {
        XMFLOAT4(GetColor(light_color >> 16), GetColor(light_color >> 8), GetColor(light_color), 1.0f),
        XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f)
    };

    // Rotate the second light around the origin
    XMMATRIX mRotate = XMMatrixRotationY(-2.0f);
    XMVECTOR vLightDir = XMLoadFloat4(&vLightDirs[1]);
    vLightDir = XMVector3Transform(vLightDir, mRotate);
    XMStoreFloat4(&vLightDirs[1], vLightDir);

    // Clear the back buffer and the depth buffer, use corn flower blue for old times sake
    float ClearColor[4] = { 0.3921569f, 0.5843138f, 0.9294118f, 1 };

    // Update matrix variables and lighting variables
    ConstantBuffer cb1;
    cb1.World = XMMatrixTranspose(world);
    cb1.View = XMMatrixTranspose(view);
    cb1.Projection = XMMatrixTranspose(proj);
    cb1.LightDir[0] = vLightDirs[0];
    cb1.LightDir[1] = vLightDirs[1];
    cb1.LightColor[0] = vLightColors[0];
    cb1.LightColor[1] = vLightColors[1];
    cb1.OutputColor = XMFLOAT4(0, 0, 0, 0);
    m_context->UpdateSubresource(m_constant_buffer, 0, NULL, &cb1, 0, 0);

    m_context->VSSetShader(m_vertex_shader, NULL, 0);
    m_context->VSSetConstantBuffers(0, 1, &m_constant_buffer);
    m_context->PSSetShader(m_pixel_shader, NULL, 0);
    m_context->PSSetConstantBuffers(0, 1, &m_constant_buffer);

    XMMATRIX* world_matrices = (XMMATRIX*)_aligned_malloc(sizeof(XMMATRIX)* maximum_instances, 16);
    if (world_matrices == NULL)
        return E_OUTOFMEMORY;

    DWORD* color_data = new DWORD[m_width * m_height];
    if (color_data == NULL)
		return E_OUTOFMEMORY;

	UINT frame = 0;
	__int64 startTime, endTime, frequency;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);

	m_context->OMSetRenderTargets(1, &m_render_target_view, m_depth_stencil_view);

	// Clear targets
	m_context->ClearRenderTargetView(m_render_target_view, ClearColor);
	m_context->ClearDepthStencilView(m_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Render the teapots
	for (DWORD teapot_number = 0; teapot_number < number_objects; teapot_number++)
	{
		world_matrices[teapot_number] = XMMatrixMultiply(
			XMMatrixTranslation(teapot_data[teapot_number].Position.x, teapot_data[teapot_number].Position.y, teapot_data[teapot_number].Position.z),
			XMMatrixRotationAxis(teapot_data[teapot_number].RotationAxis, teapot_data[teapot_number].Rotation));
	}
	m_context->UpdateSubresource(m_instance_buffer, 0, NULL, world_matrices, 0, 0);
	m_context->DrawIndexedInstanced(6, 1, 0, 0, 81);                      //draw one triangle to make sure ClearRTV completed.
	for (int y = 0; y < 9; y++)
	{
		if (UseMarkers())
		{
			if (y == 5)
			{
				pContext2->SetMarkerInt(MARKER_NAME_START, MARKER_UID);
			}
			if (y >= 3)
			{
				wchar_t Buffer[128];
				swprintf_s(Buffer, _countof(Buffer), L"%d then %d", y, 9 - y);
				g_pPerf->BeginEvent(Buffer);
			}
		}
		m_context->DrawIndexedInstanced(NUMTEAPOTINDICES, y, 0, 0, y * 9);
		m_context->DrawIndexedInstanced(NUMTEAPOTINDICES, 9 - y, 0, 0, y * 9 + y);
		if (UseMarkers())
		{
			if (y >= 3)
			{
				g_pPerf->EndEvent();
			}
		}
	}
	if (IsDeferred())
	{
		ID3D11CommandList* pCommandList = NULL;
		hr = m_context->FinishCommandList(TRUE, &pCommandList);
		if (FAILED(hr))
		{
			return hr;
		}
		m_immediate_context->ExecuteCommandList(pCommandList, TRUE);
		m_immediate_context->ExecuteCommandList(pCommandList, TRUE);
		pCommandList->Release();
	}
	if (SUCCEEDED(hr))
	{
		SaveViewToBmp(pContext2, m_staging_texture, m_back_buffer, "teapots.bmp");
	}
	hr = m_swap_chain->Present(0, 0);

	_aligned_free(world_matrices);
	SAFE_DELETE_ARRAY(color_data);
	SAFE_RELEASE(pContext2);
    SAFE_RELEASE(g_pPerf);

    return hr;
}

HRESULT CFrequencyTest_Teapot::InitializeTeapot(DWORD number_objects, DWORD box_size)
{
    // Do we need to create our teapot data yet?
    if (teapot_data == NULL)
    {
        srand(1234);
        teapot_data = (TeapotObjectData*)_aligned_malloc(number_objects * sizeof(TeapotObjectData), 16);
        if (teapot_data == NULL)
        {
            return E_OUTOFMEMORY;
        }
        ZeroMemory(teapot_data, sizeof(teapot_data)* number_objects);
        TeapotObjectData teapot_good = {};
        teapot_good.RotationAxis = XMVectorSet(GetRandomFloat(20), GetRandomFloat(20), GetRandomFloat(20), 1.0f);

        //
        // Make of grid of 9 x 8 teapots
        //


        for (DWORD i = 0; i < number_objects; i++)
        {
            int x = (i%9) - 4;
            int y = (i / 9) - 4;
            teapot_data[i] = teapot_good;
            teapot_data[i].Position.x = 4.0f * x;
            teapot_data[i].Position.y = 3.0f * y;
            teapot_data[i].Position.z = 0.0f;
        }
    }
    return S_OK;
}

float CFrequencyTest_Teapot::GetRandomFloat(float max)
{
    float val = (float)rand() / (float)RAND_MAX * max;
    float check_neg = (float)rand() / (float)RAND_MAX;

    if (check_neg > 0.5)
        val *= -1;

    return val;
}

HRESULT CFrequencyTest_Teapot::Initialize()
{
    HRESULT hr = InitializeWindow();
    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    //
    // Alternatively, use the framework device and CreateSwapChainForHwnd.
    //
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = m_width;
    sd.BufferDesc.Height = m_height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = window_handle;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, numFeatureLevels,
        D3D11_SDK_VERSION, &sd, &m_swap_chain, &m_device, &feature_level, &m_immediate_context);

    m_context = m_immediate_context;
    if (SUCCEEDED(hr) && IsDeferred())
    {
        hr = m_device->CreateDeferredContext(0, &m_deferred_context);
        if (SUCCEEDED(hr))
        {
            m_context = m_deferred_context;
        }
    }

    // Create a render target view
    if (SUCCEEDED(hr))
    {
        hr = m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_back_buffer);

        if (SUCCEEDED(hr))
        {
            hr = m_device->CreateRenderTargetView(m_back_buffer, NULL, &m_render_target_view);
        }
    }

    //
    // The rest of the code is independent of the swap chain code.
    //
    // Create the staging texture
    if (SUCCEEDED(hr))
    {
        // get the description
        D3D11_TEXTURE2D_DESC desc = { 0 };
        m_back_buffer->GetDesc(&desc);
        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        hr = m_device->CreateTexture2D(&desc, NULL, &m_staging_texture);
    }

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC depth_description;
    ZeroMemory(&depth_description, sizeof(depth_description));
    if (SUCCEEDED(hr))
    {
        depth_description.Width = m_width;
        depth_description.Height = m_height;
        depth_description.MipLevels = 1;
        depth_description.ArraySize = 1;
        depth_description.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_description.SampleDesc.Count = 1;
        depth_description.SampleDesc.Quality = 0;
        depth_description.Usage = D3D11_USAGE_DEFAULT;
        depth_description.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depth_description.CPUAccessFlags = 0;
        depth_description.MiscFlags = 0;
        hr = m_device->CreateTexture2D(&depth_description, NULL, &m_depth_stencil);
    }

    // Create the depth stencil view
    if (SUCCEEDED(hr))
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC depth_desc;
        ZeroMemory(&depth_desc, sizeof(depth_desc));
        depth_desc.Format = depth_description.Format;
        depth_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depth_desc.Texture2D.MipSlice = 0;
        hr = m_device->CreateDepthStencilView(m_depth_stencil, &depth_desc, &m_depth_stencil_view);

        if (SUCCEEDED(hr))
        {
            m_context->OMSetRenderTargets(1, &m_render_target_view, m_depth_stencil_view);
        }
    }

    // Setup the viewport
    if (SUCCEEDED(hr))
    {
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)m_width;
        vp.Height = (FLOAT)m_height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        m_context->RSSetViewports(1, &vp);
    }

    // Initialize the shaders
    if (SUCCEEDED(hr))
    {
        hr = InitializeShaders();
    }

    // Initialize the vertex data
    if (SUCCEEDED(hr))
    {
        hr = InitializeVertexData();
    }

    return hr;
}

TEST_RESULT CFrequencyTest_Teapot::Setup()
{
    if(GetFramework()->IsCompat())
        return RESULT_SKIP;
    return  CTimingDataTest::Setup();
}

TEST_RESULT CFrequencyTest_Teapot::SetupPipeline()
{
    HRESULT hr = Initialize();
    if (!SUCCEEDED(hr))
    {
        return RESULT_FAIL;
    }
    if (!SUCCEEDED(hr))
    {
        return RESULT_FAIL;
    }
    return RESULT_PASS;
}

void CFrequencyTest_Teapot::CleanupPipeline()
{
}

TEST_RESULT CFrequencyTest_Teapot::SetupTestCase()
{
    if(GetFramework()->IsCompat())
        return RESULT_SKIP;
    TEST_RESULT tr = CTimingDataTest::SetupTestCase();
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    m_width = 640;
    m_height = 480;
    m_display = true;
    //
    // Start tracing before the context is created
    //
    tr = StartETWTracing();
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    return SetupPipeline();
}

TEST_RESULT CFrequencyTest_Teapot::ExecuteTestCase()
{
    TEST_RESULT tr;
    HRESULT hr = E_FAIL;
    if (m_display)
    {
        ShowWindow(window_handle, SW_SHOW);
    }
    Render(NUM_TEAPOTS, 35, 10, 0xffff00ff);
    tr = StopETWTracing();
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    tr = ValidateFrequency();
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    return RESULT_PASS;
}


TEST_RESULT CFrequencyTest_Teapot::ValidateFrequency()
{
    TEST_RESULT tr = CFrequencyTest::ValidateFrequency();
    //
    // Verify Precision. All the draws should take within 80ns of each other.
    //
    vector<CEvent_HistoryBuffer*> historyBufferList;
    vector<UINT64> timestamps;
    vector<UINT64> ElapsedTime;

    tr = FilterHistoryBufferEvents(&historyBufferList);
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    tr = CorrelateTimestamps(&historyBufferList, &timestamps, nullptr, &ElapsedTime);
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    return RESULT_PASS;
}
