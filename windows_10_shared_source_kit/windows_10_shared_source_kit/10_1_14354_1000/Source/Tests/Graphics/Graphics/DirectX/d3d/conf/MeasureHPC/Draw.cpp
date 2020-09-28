////////////////////////////////////////////////////////////////////////////////
// Draw.cpp
////////////////////////////////////////////////////////////////////////////////

#include "Frequency.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// CFrequencyTest_Draw
////////////////////////////////////////////////////////////////////////////////
TEST_RESULT CFrequencyTest_Draw::SetupTestCase()
{
    TEST_RESULT tr = CTimingDataTest::SetupTestCase();
    if (tr != RESULT_PASS)
    {
        return tr;
    }

    struct VertexPos
    {
        XMFLOAT3 Pos;
    };
    const int width = 640;
    const int height = 480;
    const int vertex_width = validate ? 16 : width / 4;
    const int vertex_height = validate ? 16 : height / 4;

    static VertexPos vertices[vertex_width * vertex_height * 3] =
    {
        XMFLOAT3(-1.0f,                 -1.0f,                  0.0f),
        XMFLOAT3(-1.0f + 1.0f / width,  -1.0f + 2.0f / height,  0.0f),
        XMFLOAT3(-1.0f + 2.0f / width,  -1.0f,                  0.0f),
    };

    // Prepare vertex buffer
    for (int i = 3; i < _countof(vertices); i++)
    {
        int k = i % 3;
        float x = (float)((i / 3) % vertex_width ) * 2.0f / width;
        float y = (float)((i / 3) / vertex_width ) * 2.0f / height;
        vertices[i].Pos = XMFLOAT3(vertices[k].Pos.x + 4 * x, vertices[k].Pos.y + 4 * y, vertices[k].Pos.z);
    }

    m_vertices = (void *)vertices;
    m_vertices_size = sizeof(vertices);
    m_vertices_count = _countof(vertices);
    m_stride = sizeof(VertexPos);
    m_offset = 0;

    // Define the input layout
    static D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    static string my_VS = "float4 VSMain(float4 Pos : POSITION) : SV_POSITION \n{\n return Pos; \n}\n";
    static string my_PS = "float4 PSMain(float4 Pos : SV_POSITION) : SV_Target \n{\n return float4(1.0f, 1.0f, 0.0f, 1.0f);\n}\n";

    //override Vertex and Pixel Shaders
    m_display = true;
    m_szVS = my_VS;
    m_szPS = my_PS;
    m_szPS_Alt = my_PS;
    m_layout = layout;
    m_layout_count = _countof(layout);
    m_width = width;
    m_height = height;
    m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    tr = SetupPipeline();
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    return RESULT_PASS;
}

TEST_RESULT CFrequencyTest_Draw::ExecuteTestCase()
{
    TEST_RESULT tr;
    HRESULT hr = E_FAIL;
    FILE *fp = NULL;
    PTCHAR comma = _T("");
    if (!validate)
    {
        fp = fopen("times.csv", "a");
    }
    // pass 3 is our normal test.
    int firstpass = validate ? 3 : 0;
    for (int pass = firstpass; pass <= 3; pass++)
    {
        LARGE_INTEGER QPF, QPC_Init, QPC_Max, QPC_Now, Elapsed;
        ID3D11Query *pQuery = nullptr;
        UINT64 queryData;
        D3D11_QUERY_DESC desc = {};

        switch (pass)
        {
        case 1:
        case 2:
            desc.Query = D3D11_QUERY_TIMESTAMP;
            m_pDevice->CreateQuery(&desc, &pQuery);
            break;
        case 3:
            if (m_bTracing == true)        //deanwi
            {
                fprintf(fp, "error tracng\n");
            }
            tr = StartETWTracing();
            if (tr != RESULT_PASS)
            {
                return tr;
            }
            break;
        }
        QueryPerformanceFrequency(&QPF);
        QueryPerformanceCounter(&QPC_Init);
        QPC_Max.QuadPart = seconds * QPF.QuadPart;
        int loopcount = 0;
        do
        {
            m_pDeviceContext->ClearRenderTargetView(m_pRTView, Colors::MidnightBlue);
            switch (pass)
            {
            case 1:
            case 2:
                for (UINT u = 0; u < m_vertices_count; u += 3)
                {
                    m_pDeviceContext->Draw(3, u);
                    m_pDeviceContext->End(pQuery);
                    if (pass == 2)
                    {
                        GetFramework()->GetDataWithTimeout(m_pDeviceContext, pQuery, &queryData, sizeof(queryData), 0);
                    }
                }
                break;
            case 0:
            case 3:
                for (UINT u = 0; u < m_vertices_count; u += 3)
                {
                    m_pDeviceContext->Draw(3, u);
                }
                break;
            }
            loopcount++;
            m_pDeviceContext->Flush();
            QueryPerformanceCounter(&QPC_Now);
        } while ((!validate) && ((QPC_Now.QuadPart - QPC_Init.QuadPart) < QPC_Max.QuadPart));
        if (!validate)
        {
            Elapsed.QuadPart = (QPC_Now.QuadPart - QPC_Init.QuadPart) / QPF.QuadPart;
            INT64 rate = ((INT64)loopcount * m_vertices_count) / Elapsed.QuadPart;
            fprintf(fp, _T("%s%lld"), comma, rate);
            comma = _T(",");
        }
        if (FAILED(hr = ExecuteEffectiveContext()))
        {
            LogError(__FILEW__, __LINE__, L"ExecuteEffectiveContext() failed",
                hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute);
            return RESULT_FAIL;
        }
        switch (pass)
        {
        case 3:
            tr = StopETWTracing();
            if (tr != RESULT_PASS)
            {
                return tr;
            }
            SaveViewToBmp(m_pDeviceContext, m_pStagingTex, m_pRTTex, "mesh.bmp");
            //
            // validation
            //
            if (validate)
            {
                tr = ValidateFrequency();
                if (tr != RESULT_PASS)
                {
                    return tr;
                }
            }
            break;
        }
    }   // end of loop
    if (!validate)
    {
        fprintf(fp, _T("\n"));
        fclose(fp);
    }
    return RESULT_PASS;
}


TEST_RESULT CFrequencyTest_Draw::ValidateFrequency()
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
    //
    // Disabled, because does not work, and won't measure what we want.
    //
#if 0
    UINT64 Mean = 0;
    for (auto it = ElapsedTime.begin(); it != ElapsedTime.end(); ++it)
    {
        Mean += *it;
    }// InstancedWarp.cpp : Defines the entry point for the application.
    //
    Mean /= ElapsedTime.size();

    INT64 MaxClockError = (8 * CalculatedGpuFrequency) / 100000000;             //number of clocks in 80ns
    for (auto it = ElapsedTime.begin(); it != ElapsedTime.end(); ++it)
    {
        INT64 diff = Mean - *it;
        if (diff < 0)
        {
            diff = -diff;
        }
        if (diff>MaxClockError)
        {
            WriteToLog(_T("Draw time delta exceeds 80ns.\n"));
        }
    }
#endif
}

