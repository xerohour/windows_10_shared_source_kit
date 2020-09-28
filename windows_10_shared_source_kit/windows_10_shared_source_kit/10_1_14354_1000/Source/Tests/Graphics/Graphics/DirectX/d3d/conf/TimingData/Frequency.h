////////////////////////////////////////////////////////////////////////////////
// Frequency.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TimingData.h"

#define TEST_CASE_ITERATIONS    100
#define VIDEO_WIDTH             640
#define VIDEO_HEIGHT            480
#define VIDEO_FPS               60

class CFrequencyTest : public CTimingDataTest
{
public:
    CFrequencyTest();
    virtual TEST_RESULT SetupTestCase();
    virtual void CleanupTestCase();
    virtual TEST_RESULT ValidateFrequency();

protected:
};

class CFrequencyTest_Draw : public CFrequencyTest
{
public:
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT ValidateFrequency();
protected:
};

class CFrequencyTest_Copy : public CFrequencyTest
{
public:
    CFrequencyTest_Copy();

    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual void CleanupTestCase();

protected:
    ID3D11Texture2D *m_pDestTex;
};

class CFrequencyTest_Compute : public CFrequencyTest
{
public:
    virtual TEST_RESULT Setup();
    virtual TEST_RESULT ExecuteTestCase();

protected:
};

class CFrequencyTest_Video : public CFrequencyTest
{
public:
    CFrequencyTest_Video();

    virtual TEST_RESULT Setup();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual void CleanupTestCase();

protected:
    ID3D11VideoDevice *m_pVideoDevice;
    ID3D11VideoContext *m_pVideoContext;
    ID3D11VideoProcessorEnumerator *m_pVideoProcessorEnumerator;
    ID3D11VideoProcessor *m_pVideoProcessor;
    ID3D11Texture2D *m_pVideoInputTex;
    ID3D11Texture2D *m_pVideoOutputTex;
    ID3D11VideoProcessorInputView *m_pVideoInputView;
    ID3D11VideoProcessorOutputView *m_pVideoOutputView;
};

struct TeapotObjectData;
class CFrequencyTest_Teapot : public CFrequencyTest
{
public:
    virtual TEST_RESULT Setup();
    virtual TEST_RESULT SetupPipeline();
    virtual void CleanupPipeline();
    CFrequencyTest_Teapot::CFrequencyTest_Teapot();
    CFrequencyTest_Teapot::~CFrequencyTest_Teapot();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT ValidateFrequency();
    // Methods
    HRESULT Initialize();
    HRESULT Render(DWORD number_objects, DWORD camera_zoom, DWORD box_size, DWORD light_color);
    virtual bool UseMarkers()
    {
        return true;
    }
    virtual bool IsDeferred()
    {
        return false;
    }
private:
    HRESULT InitializeWindow();
    HRESULT InitializeVertexData();
    HRESULT InitializeShaders();
    HRESULT InitializeTeapot(DWORD number_objects, DWORD box_size);
    float GetRandomFloat(float max);
    void UpdateTeapotData(DWORD number_objects);

    // Fields
    HWND window_handle;
    D3D_FEATURE_LEVEL feature_level;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_immediate_context;
    ID3D11DeviceContext* m_deferred_context;
    ID3D11DeviceContext* m_context;
    IDXGISwapChain* m_swap_chain;
    ID3D11Texture2D* m_back_buffer;
    ID3D11RenderTargetView* m_render_target_view;
    ID3D11DepthStencilView* m_depth_stencil_view;
    ID3D11Texture2D* m_depth_stencil;
    ID3D11Buffer* m_vertex_buffer;
    ID3D11Buffer* m_index_buffer;
    ID3D11Buffer* m_constant_buffer;
    ID3D11Buffer* m_instance_buffer;
    ID3D11VertexShader* m_vertex_shader;
    ID3D11PixelShader* m_pixel_shader;
    ID3D11PixelShader* m_pixel_shader_solid;
    ID3D11InputLayout* m_vertex_layout;
    ID3D11Texture2D* m_staging_texture;
    ID3DBlob* m_saved_scene_blob;

    // Information about our moving teapots
    TeapotObjectData* teapot_data;

    static const DWORD maximum_instances = 5000;
    static const DWORD maximum_frames = 1800;
    static const DWORD frame_rate = 60; 
protected:
};

class CFrequencyTest_TeapotDeferredMarkers : public CFrequencyTest_Teapot
{
    virtual bool IsDeferred()
    {
        return true;
    }
    virtual bool UseMarkers()
    {
        return true;
    }
};

class CFrequencyTest_TeapotDeferred : public CFrequencyTest_Teapot
{
    virtual bool IsDeferred()
    {
        return true;
    }
    virtual bool UseMarkers()
    {
        return false;
    }
};
