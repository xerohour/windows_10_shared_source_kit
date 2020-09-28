//
// RedBlue
//
// This is a conformance test application, it is designed to render a simple scene that uses unsynchronized shared surfaces
// One of the goals of the High Performance Timestamp feature in Windows Blue is to be able to reproduce the order of usage
// of a shared surface by looking at the timestamps of when individual calls were executed. This is a worst case test.
//
// A device on one thread alternates a 1x1 texture surface between Red and Blue, with a Flush between the color changes
// The other devices renders a single pixel of the final image at a time using the shared texture as a source color
//
// The top half of the result image has a Flush between each pixel rendered, the bottom half has no flushes.
//
//

#include "TimingData.h"
#include "d3d11sdklayers.h"
#include "EndOfPipeline.h"
#include "Frequency.h"
#include "MultipleEngine.h"
#include "MarkerValidation.h"
#include "PState.h"
#include "CustomEvent.h"
#include "provider.h"
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <stdio.h>
#include <process.h>
#include <wincodec.h>
#include <wincodecsdk.h>
#include "RedBlue.h"
//
// Save the given render target as a lossless PNG file
//
TEST_RESULT CRedBlueTest::SaveAsPNG(ID3D11DeviceContext* pD3DContext, ID3D11Resource* pBackBuffer, const wchar_t* FileName)
{
    // Get the D3D device and Texture2D surface description
    ID3D11DeviceChild* pDeviceChild = NULL;
    HRESULT hr = pBackBuffer->QueryInterface(__uuidof(ID3D11DeviceChild), (void**)&pDeviceChild);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed QueryInterface(ID3D11DeviceChild)"));
        return RESULT_FAIL;
    }

    ID3D11Device* pD3DDevice = NULL;
    pDeviceChild->GetDevice(&pD3DDevice);

    ID3D11Texture2D * pSourceTexture2D = NULL;
    hr = pBackBuffer->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pSourceTexture2D);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed QueryInterface(ID3D11Texture2D)"));
        return RESULT_FAIL;
    }

    D3D11_TEXTURE2D_DESC desc;
    pSourceTexture2D->GetDesc(&desc);
    if (desc.Format != DXGI_FORMAT_B8G8R8A8_UNORM)
    {
        WriteToLog(TEXT("Only DXGI_FORMAT_B8G8R8A8_UNORM render targets supported"));
        return RESULT_FAIL;
    }

    // Change the render target surface description into a staging surface
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    desc.ArraySize = 1;
    desc.MipLevels = 1;

    ID3D11Texture2D * pStagingSurface = NULL;
    hr = pD3DDevice->CreateTexture2D(&desc, NULL, &pStagingSurface);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not create staging surface"));
        return RESULT_FAIL;
    }

    // Copy render target to staging surface so we can read it
    pD3DContext->CopyResource(pStagingSurface, pSourceTexture2D);

    D3D11_MAPPED_SUBRESOURCE mapData;
    hr = pD3DContext->Map(pStagingSurface, 0, D3D11_MAP_READ, 0, &mapData);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not Map staging surface"));
        return RESULT_FAIL;
    }
    // Save as BMP
    SaveMapToBmp(&mapData, m_width, m_height, TEXT("RedBlue2.bmp"), false);
    //
    // Now WIC setup for saving as a PNG
    //
    IWICImagingFactory *piFactory = NULL;
    IWICBitmapEncoder *piEncoder = NULL;
    IWICBitmapFrameEncode *piBitmapFrame = NULL;
    IPropertyBag2 *pPropertybag = NULL;
    IWICStream *piStream = NULL;

    CoInitializeEx(NULL, 0);
    hr = CoCreateInstance(CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IWICImagingFactory,
        (LPVOID*)&piFactory);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not CoCreateInstance(CLSID_WICImagingFactory)"));
        return RESULT_FAIL;
    }

    hr = piFactory->CreateStream(&piStream);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not CreateStream"));
        return RESULT_FAIL;
    }

    hr = piStream->InitializeFromFilename(FileName, GENERIC_WRITE);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not InitializeFromFilename"));
        return RESULT_FAIL;
    }

    hr = piFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &piEncoder);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not CreateEncoder"));
        return RESULT_FAIL;
    }

    hr = piEncoder->Initialize(piStream, WICBitmapEncoderNoCache);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not Initialize(WICBitmapEncoderNoCache)"));
        return RESULT_FAIL;
    }

    hr = piEncoder->CreateNewFrame(&piBitmapFrame, &pPropertybag);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not CreateNewFrame"));
        return RESULT_FAIL;
    }

    PROPBAG2 option = { 0 };
    option.pstrName = L"FilterOption";
    VARIANT varValue;
    VariantInit(&varValue);
    varValue.vt = VT_UI1;
    varValue.bVal = WICPngFilterNone;
    hr = pPropertybag->Write(1, &option, &varValue);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not Write options"));
        return RESULT_FAIL;
    }

    hr = piBitmapFrame->Initialize(pPropertybag);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not Initialize properties"));
        return RESULT_FAIL;
    }

    hr = piBitmapFrame->SetSize(desc.Width, desc.Height);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not SetSize"));
        return RESULT_FAIL;
    }

    WICPixelFormatGUID formatGUID = GUID_WICPixelFormat32bppBGRA;
    hr = piBitmapFrame->SetPixelFormat(&formatGUID);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not SetPixelFormat"));
        return RESULT_FAIL;
    }

    hr = piBitmapFrame->WritePixels(desc.Height, mapData.RowPitch, mapData.RowPitch * desc.Height, (BYTE*)mapData.pData);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed WritePixels"));
        return RESULT_FAIL;
    }

    // Cleanup WIC resoruces
    piBitmapFrame->Commit();
    piEncoder->Commit();
    piFactory->Release();
    piBitmapFrame->Release();
    piEncoder->Release();
    piStream->Release();

    // Cleanup D3D resources
    pD3DContext->Unmap(pStagingSurface, 0);
    pStagingSurface->Release();
    pSourceTexture2D->Release();
    pD3DDevice->Release();
    pDeviceChild->Release();
    return RESULT_PASS;
}

HANDLE sharedSurfaceCreatedEvent = NULL;
HANDLE terminatEvent = NULL;
HANDLE sharedSurfaceHandle = NULL;

const D3D_FEATURE_LEVEL featureLevels10[] =
{
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
};
UINT numFeatureLevels10 = ARRAYSIZE(featureLevels10);
const D3D_FEATURE_LEVEL featureLevels9[] =
{
    D3D_FEATURE_LEVEL_9_1,
};
UINT numFeatureLevels9 = ARRAYSIZE(featureLevels9);

const D3D_FEATURE_LEVEL *featureLevels;
UINT numFeatureLevels;

unsigned __stdcall SecondThreadFunc(void* pArguments)
{
    CRedBlueTest *pClass = (CRedBlueTest *)pArguments;
    pClass->Second();
    _endthreadex(0);
    return 0;
}

TEST_RESULT CRedBlueTest::Setup()
{
    HRESULT hr = S_OK;
    if (GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0)
    {
        featureLevels = featureLevels9;
        numFeatureLevels = numFeatureLevels9;
    }
    else
    {
        featureLevels = featureLevels10;
        numFeatureLevels = numFeatureLevels10;
    }

    TEST_RESULT tr = CD3D11Test::Setup();
    if (tr != RESULT_PASS)
    {
        return tr;
    }

    m_pDevice = GetDevice2();
    m_pDeviceContext = GetEffectiveContext2();

    // Ensure device supports markers
    D3D11_FEATURE_DATA_MARKER_SUPPORT MarkerData;
    if (FAILED(hr = m_pDevice->CheckFeatureSupport(D3D11_FEATURE_MARKER_SUPPORT, &MarkerData, sizeof(MarkerData))))
    {
        LogError(__FILEW__, __LINE__, L"Call to CheckFeatureSupport() failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCheckFeatureSupport);
        return RESULT_FAIL;
    }

    if (MarkerData.Profile == FALSE && !GetFramework()->Is11On12() && !GetFramework()->IsCompat())
    {
        // D3D10WARP.DLL does not have marker support, but is scheduled to be replaced by D3D12WARP.DLL.
        bool IsLegacyWarp10Dll;
        if (FAILED(hr = GetFramework()->IsLegacyD3D10WarpDll(GetDXGIAdapter(), m_pDevice, IsLegacyWarp10Dll)))
        {
            LogError(__FILEW__, __LINE__, L"Call to CheckFeatureSupport() failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCheckFeatureSupport);
            return RESULT_FAIL;
        }
        
        if (GetFramework()->GetTestMode() == WGFTEST_MODE_WHQL && !IsLegacyWarp10Dll)
        {
            LogError(__FILEW__, __LINE__, L"Failing due to lack of marker data support on WDDM 1.3 hardware", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported);
            return RESULT_FAIL;
        }
        else
        {
#if 0
            WriteToLog(_T("No Marker Support. Attempting to Verify files in Execution Directory"));
            fSkipExecution = true;
            return RESULT_PASS;
#else
            WriteToLog(_T("Skipping due to lack of marker data support"));
            return RESULT_SKIP;
#endif
        }
    }
    return tr;
}


TEST_RESULT CRedBlueTest::Initialize()
{
    return RESULT_PASS;
}

TEST_RESULT CRedBlueTest::SetupTestCase()
{
    if (fSkipExecution)
    {
        return RESULT_PASS;
    }
    TEST_RESULT tr = CTimingDataTest::SetupTestCase();
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    m_width = 80;
    m_height = 240;
    m_display = true;
    //
    // Start tracing before the context is created
    //
    if (!GetFramework()->Is11On12())
    {
        tr = StartETWTracing(false);
    }
    if (tr != RESULT_PASS)
    {
        return tr;
    }
    return SetupPipeline();
}


TEST_RESULT CRedBlueTest::ExecuteTestCase()
{
    LARGE_INTEGER StartTime = {};
    QueryPerformanceCounter(&StartTime);
    TEST_RESULT tr = RESULT_PASS;
    if (!fSkipExecution)
    {
        tr = DoExecuteTestCase();
        if (RESULT_PASS != tr)
        {
            return tr;
        }
    }
    //
    // validate...
    //
    HRESULT hr = E_FAIL;
    bool fResult = true;
    vector<EventRuntimeMarkerData> SortedRuntimeMarkers;
    SortedRuntimeMarkers.clear();
    FilterAndSortRuntimeMarkers(&SortedRuntimeMarkers, StartTime.QuadPart);
    sort(SortedRuntimeMarkers.begin(), SortedRuntimeMarkers.end());
    UINT32 width = m_width;
    UINT32 height = m_height;
    enum Colors
    {
        RED, BLUE
    };
    UINT32 colors[2] =
    {
        0xffff0000,         //red
        0xff0000ff,         //blue
    };
    UINT32 red = 0xffffff & colors[RED];
    UINT32 blue = 0xffffff & colors[BLUE];
    UINT32 padding = 0;
    ULONG s = 0;
    UCHAR *orig = nullptr;

    HANDLE file = CreateFile("redblue2.bmp", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    BOOL ok = (file != INVALID_HANDLE_VALUE);
    if (ok)
    {
        {
            BITMAPFILEHEADER bmfh = {};
            ok = ReadFile(file, &bmfh, sizeof(BITMAPFILEHEADER), NULL, NULL);
        }
        if (ok)
        {
            BITMAPINFOHEADER info = {};
            ok = ReadFile(file, &info, sizeof(BITMAPINFOHEADER), NULL, NULL);
            width = info.biWidth;
            height = info.biHeight;
            padding = width % 4;
            s = height * (width * 3 + padding);
            //
            // verify that we have space for at least one pixel and there are no overflows
            // note that if (width<=800)&&(height<=600), (s>=3)=> ((width>0) && (height>0))
            //

            if ((width > 800) || (height > 600) || (width == 0) || (height == 0) || (s < 3))
            {
                return RESULT_FAIL;
            }
            orig = new UCHAR[s];
        }
        if (ok)
        {
            ok = ReadFile(file, orig, s, NULL, NULL);
        }
        CloseHandle(file);
    }
    if (ok != TRUE)
    {

        return RESULT_FAIL;
    }
    if (GetFramework()->IsCompat())
    {
        UCHAR* dest = new UCHAR[s];
        ZeroMemory(dest, s);        //only to suppress OACR error. Otherwise, you can prove this is not needed.
        UCHAR *pDest = dest;
        UCHAR *pOrig = orig;
        UINT32 ApiIndex = 0;
        UINT64 Draws = 0;
        bool indeterminate = false;
        //
        // The first ClearRenderTargetView() is to Red, so that means none=Blue
        //
        int state = BLUE;              //predicted color
        bool inverted = false;
        ApiIndex = 0;
        DWORD Value = 0;
        auto Marker = SortedRuntimeMarkers[ApiIndex];
        UINT64 ValidationDraw = 3;
        UINT64 ValidationClear = 18;
        if (GetFramework()->Is11On12())
        {
            ValidationDraw = (MSG_Map_D3D12_MARKER_API_DRAWINSTANCED - MSG_Map_D3D12_MARKER_API_SETMARKER);
            ValidationClear = (MSG_Map_D3D12_MARKER_API_CLEARRENDERTARGETVIEW - MSG_Map_D3D12_MARKER_API_SETMARKER);
        }
        for (UINT32 y0 = 0; y0 < height; y0++)
        {
            for (UINT32 x0 = 0; x0 < width; x0++)
            {
#pragma warning(suppress: 26019)    // TODO: Fix invalid OACR error
                UINT32 color_orig0 = (pOrig[2] << 16) | (pOrig[1] << 8) | (pOrig[0]);
                UINT32 color_orig;

                if (color_orig0 == red || color_orig0 == blue)
                {
                    color_orig = color_orig0;
                }
                else
                {
                    WriteToLog(L"Color other than red/blue detected @%d,%d\n", x0, y0);
                    fResult = false;
                    if (color_orig0 & 0xff)
                    {
                        color_orig = blue;
                    }
                    else
                    {
                        color_orig = red;
                    }
                }
                //int orig_state = (color_orig == blue);             //0==red, 1==blue

                for (; ApiIndex < SortedRuntimeMarkers.size(); ApiIndex++)
                {
                    Marker = SortedRuntimeMarkers[ApiIndex];
                    auto MarkerAPI = Marker.m_MarkerApi;
                    if (MarkerAPI == ValidationDraw)
                    {
                        Value = colors[state];
                        Draws++;
                        break;
                    }
                    else if (MarkerAPI == ValidationClear)
                    {
                        state = 1 - state;
                        //indeterminate = true;   //one Draw indeterminate after ClearRenderTargetView
                    }
                    else
                    {
                        continue;
                    }
                }
                ApiIndex++;
                //
                // We synchronize with the first pixel because an older test version switched blue and red in the BMP
                //
                if (x0 == 0 && y0 == 0)
                {
                    inverted = (state != (color_orig == blue));     //inverted bitmap
                    state = (color_orig == blue);                   //0==red, 1==blue
                    Value = colors[state];
                }

                // Copy pixel to destination bmp
                //
#pragma warning(suppress: 26019)    // TODO: Fix invalid OACR error
                pDest[0] = (Value)& 0xff;
#pragma warning(suppress: 26019)
                pDest[1] = (Value >> 8) & 0xff;
#pragma warning(suppress: 26019)
                pDest[2] = (Value >> 16) & 0xff;
                UINT32 color_dest = Value & 0xffffff;
                //
                // compare with known good
                //

                if (!indeterminate && (color_orig != color_dest))
                {

                    fResult = false;
                    WriteToLog(L"Ordering Error between Draw and ClearRenderTargetView at (%d,%d)\n", x0, y0);
                    WriteToLog(L"Bitmap contains %s", (color_orig == red) ? L"Red" : L"Blue");
                    if (inverted)
                    {
                        WriteToLog(L"(reversed)");
                    }
                    WriteToLog(L", Predicted %s\n", (color_dest == red) ? L"Red" : L"Blue");
                    //
                    // A diff is a white spot.
                    //
#pragma warning(suppress: 26019)
                    pDest[0] = 0xff;
#pragma warning(suppress: 26019)
                    pDest[1] = 0xff;
#pragma warning(suppress: 26019)
                    pDest[2] = 0xff;
                }
                indeterminate = false;
                pDest += 3;
                pOrig += 3;
            }
            for (UINT32 x = 0; x < padding; x++)
            {
#pragma warning(suppress: 26019)
                *pDest++ = '\0';
#pragma warning(suppress: 26019)
                *pOrig++ = '\0';
            }
        }
        SaveBMP(dest, width, height, s, "delta.bmp");
        delete[] dest;
        delete[] orig;
        if (!fResult)
        {
            return RESULT_FAIL;
        }
        else
        {
            hr = S_OK;
        }
        if (!SUCCEEDED(hr))
        {
            tr = RESULT_FAIL;
        }
    }
    else
    {
        if (m_pXperfHelper == NULL)
        {
            m_pXperfHelper = new XperfHelper(NORMAL, true, LARGE, FALSE);
            m_pXperfHelper->SetMergedFile(ETLFileName());
        }
        hr = m_pXperfHelper->Action(L"gputiming -redblue redblue2.bmp -noheader", RESULTS_FILE, L"perf_gputiming.dll");
        hr = S_OK;              //return code from Action() currently unreliable - todo
        tr = ReadResultFile();
    }
    if (tr == RESULT_FAIL)
    {
        LogError(__FILEW__, __LINE__, L"Could not reconstruct Red/Blue image", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure);
        return RESULT_FAIL;
    }
    return RESULT_PASS;
}

static CRITICAL_SECTION CriticalSection;
struct WrapCS
{
    bool m_fWrap;
    WrapCS(bool fWrap)
    {
        m_fWrap = fWrap;
        if (m_fWrap)
        {
            EnterCriticalSection(&CriticalSection);
        }
    }
    ~WrapCS()
    {
        if (m_fWrap)
        {
            LeaveCriticalSection(&CriticalSection);
        }
    }
};

struct HandleCS
{
    bool m_fUseCS;
    HandleCS(bool fUseCS)
    {
        m_fUseCS = fUseCS;
        if (m_fUseCS)
        {
            InitializeCriticalSectionAndSpinCount(&CriticalSection, 0x00000400);
        }
    }
    ~HandleCS()
    {
        if (m_fUseCS)
        {
            DeleteCriticalSection(&CriticalSection);
        }
    }
};

TEST_RESULT CRedBlueTest::DoExecuteTestCase()
{
    HRESULT hr;

    struct HandleCS initCS = HandleCS(GetFramework()->IsSetRedBlueCS()); //initialize the CriticalSection if needed.

    // Create an event that will signal whent he second thread should shut down
    terminatEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // Create an event that will signal when a shared surface is available
    sharedSurfaceCreatedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // Create the Red/Blue thread
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &SecondThreadFunc, this, 0, NULL);

    // Is the shared surface available?
    if (WaitForSingleObject(sharedSurfaceCreatedEvent, 10000) != WAIT_OBJECT_0)
    {
        WriteToLog(TEXT("Shared surface not created"));
        return RESULT_FAIL;
    }
    // Reset this event so we can use it to signal that the 2nd thread has exited
    ResetEvent(sharedSurfaceCreatedEvent);

    //
    // Create the main thread that renders pixel by pixel using the shared surface
    //

    ID3D11Device*           pD3DDevice = NULL;
    ID3D11DeviceContext*    pD3DDeviceContext = NULL;

    D3D_FEATURE_LEVEL outFL;
    if (GetFramework()->Is11On12())
    {
        hr = GetFramework()->GetD3DDeviceManager()->CreateDevice(NULL,
            GetFramework()->m_D3DOptions.DriverType,
            D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG,
            *featureLevels,
            (ID3D11Device**)&pD3DDevice,
            &pD3DDeviceContext);
    }
    else if(GetFramework()->IsMobile())
    {
                D3D_FEATURE_LEVEL outFL;

        hr = D3D11CreateDevice(NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            D3D11_CREATE_DEVICE_SINGLETHREADED,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            (ID3D11Device**)&pD3DDevice,
            &outFL,
            &pD3DDeviceContext);
    }
    else
    {
        hr = D3D11CreateDevice(NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            (ID3D11Device**)&pD3DDevice,
            &outFL,
            &pD3DDeviceContext);
    }

    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed to create D3D11 Device"));
        return RESULT_FAIL;
    }

    ID3D11Texture2D* pBackBuffer = NULL;
    D3D11_TEXTURE2D_DESC bufferDesc = {};
    bufferDesc.Width = m_width;
    bufferDesc.Height = m_height;
    bufferDesc.MipLevels = 1;
    bufferDesc.ArraySize = 1;
    bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.CPUAccessFlags = 0;
    pD3DDevice->CreateTexture2D(&bufferDesc, NULL, &pBackBuffer);

    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not get swap chain buffer"));
        return RESULT_FAIL;
    }

    ID3D11RenderTargetView* pRTView = NULL;
    hr = pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRTView);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed CreateRenderTargetView"));
        return RESULT_FAIL;
    }
    pD3DDeviceContext->OMSetRenderTargets(1, &pRTView, NULL);

    // Get shared surface
    IDXGIResource* pSharedResource = NULL;
    hr = pD3DDevice->OpenSharedResource(sharedSurfaceHandle, __uuidof(IDXGIResource), (void**)&pSharedResource);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not OpenSharedResource"));
        return RESULT_FAIL;
    }

    ID3D11Texture2D* pShared2DTexture = NULL;
    hr = pSharedResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pShared2DTexture);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not QueryInterface"));
        return RESULT_FAIL;
    }

    ID3D11ShaderResourceView* pSRView = NULL;
    hr = pD3DDevice->CreateShaderResourceView(pShared2DTexture, NULL, &pSRView);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed CreateShaderResourceView"));
        return RESULT_FAIL;
    }
    pD3DDeviceContext->PSSetShaderResources(0, 1, &pSRView);

    // Setup rendering state
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)m_width;
    vp.Height = (FLOAT)m_height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pD3DDeviceContext->RSSetViewports(1, &vp);

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ID3D11SamplerState* pSamplerState;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = pD3DDevice->CreateSamplerState(&sampDesc, &pSamplerState);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed CreateSamplerState"));
        return RESULT_FAIL;
    }
    pD3DDeviceContext->PSSetSamplers(0, 1, &pSamplerState);

    //
    // Compile vertex shader
    //
    char vertexShader[] =
        "float4 VS( float4 input : POSITION ) : SV_POSITION"
        "{"
        "       return input;"
        "}";

    ID3D10Blob* errorMessage = NULL;
    ID3D10Blob* vertexShaderBlob = NULL;
    hr = D3DCompile(vertexShader, sizeof(vertexShader), NULL, NULL, NULL, "VS", "vs_4_0_level_9_1", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBlob, &errorMessage);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed vertex shader compile - %s)"), errorMessage->GetBufferPointer());
        return RESULT_FAIL;
    }
    ID3D11VertexShader* pVertexShader;
    hr = pD3DDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &pVertexShader);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed CreateVertexShader"));
        return RESULT_FAIL;
    }
    pD3DDeviceContext->VSSetShader(pVertexShader, NULL, 0);

    //
    // Compile pixel shader
    //
    char pixelShader[] =
        "sampler MySamp;"
        "Texture2D <float4> MyTex;"
        ""
        "float4 PS( float4 input : SV_POSITION ) : SV_Target"
        "{"
        "return MyTex.Sample( MySamp, float2(0.5f, 0.5f));"
        "}";

    ID3D10Blob* pixelShaderBlob = NULL;
    hr = D3DCompile(pixelShader, sizeof(pixelShader), NULL, NULL, NULL, "PS", "ps_4_0_level_9_1", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBlob, &errorMessage);
    if (FAILED(hr))
    {
        WriteToLog(L"Failed pixel shader compile - %s", errorMessage->GetBufferPointer());
        return RESULT_FAIL;
    }
    ID3D11PixelShader* pPixelShader;
    hr = pD3DDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &pPixelShader);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed CreatePixelShader"));
        return RESULT_FAIL;
    }
    pD3DDeviceContext->PSSetShader(pPixelShader, NULL, 0);

    //
    // Define the input layout (simple 2D coords)
    //
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    ID3D11InputLayout* pInputLayout;
    hr = pD3DDevice->CreateInputLayout(layout, _countof(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &pInputLayout);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed CreateInputLayout"));
        return RESULT_FAIL;
    }
    pD3DDeviceContext->IASetInputLayout(pInputLayout);

    //
    // Create a vertex buffer
    //
    struct
    {
        float X; float Y;       // Position
    } pVertices[] =
    {
        0.0f, 0.0f
    };

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(pVertices)* m_width * m_height;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    float* pInitialData = (float*)malloc(sizeof(pVertices)* m_width * m_height);
    for (unsigned y = 0; y < m_height; y++)
    {
        for (unsigned x = 0; x < m_width; x++)
        {
            pInitialData[2 * (x + (y * m_width)) + 0] = ((1.0f + 2.0f * x) / m_width) - 1.0f;
            pInitialData[2 * (x + (y * m_width)) + 1] = ((1.0f + 2.0f * y) / m_height) - 1.0f;
        }
    }

    D3D11_SUBRESOURCE_DATA InitData = { pInitialData, 0, 0 };
    ID3D11Buffer* pVertexBuffer = NULL;
    hr = pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not create Vertex Buffer"));
        return RESULT_FAIL;
    }
    free(pInitialData);

    UINT stride = sizeof(pVertices);
    UINT offset = 0;
    pD3DDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    pD3DDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

    //
    // Main loop
    //

    {
        // Fill each pixel in the window one by one
        for (unsigned y = 0; y < m_height; y++)
        {
            for (unsigned x = 0; x < m_width; x++)
            {
                {
                    // The top half has a Flush between each draw call
                    // The bottom half has no flush, so the color should only change each RenderCb the card executes (Unless the color clear overlaps)
                    if (y < (m_height / 2))
                    {
                        WrapCS cs = WrapCS(GetFramework()->IsSetRedBlueCS());
                        pD3DDeviceContext->Draw(1, x + (y * m_width));
                        pD3DDeviceContext->Flush();
                    }
                    else
                    {
                        pD3DDeviceContext->Draw(1, x + (y * m_width));
                    }
                }

            }
        }

        // Final flush must occur otherwise work will not execute on warp
        pD3DDeviceContext->Flush();

        // Save the final conformance image to disk
        SaveAsPNG(pD3DDeviceContext, pBackBuffer, L"RedBlue.png");

        if (FAILED(hr))
        {
            WriteToLog(TEXT("Failed Present"));
            return RESULT_FAIL;
        }

    }

    // Release any references to shared resource
    pSRView->Release();
    pShared2DTexture->Release();
    pSharedResource->Release();

    // Tell 2nd thread to cleanup and exit
    SetEvent(terminatEvent);

    TEST_RESULT tr = StopETWTracing();
    if (tr != RESULT_PASS)
    {
        return tr;
    }

    if (WaitForSingleObject(sharedSurfaceCreatedEvent, 10000) != WAIT_OBJECT_0)
    {
        WriteToLog(TEXT("2nd thread wont shut down"));
        return RESULT_FAIL;
    }

    // Clean-up D3D
    pD3DDeviceContext->ClearState();
    pVertexBuffer->Release();
    pPixelShader->Release();
    pVertexShader->Release();
    pInputLayout->Release();
    pSamplerState->Release();
    pRTView->Release();
    pBackBuffer->Release();
    pD3DDeviceContext->Release();
    pD3DDevice->Release();
    return RESULT_PASS;
}

TEST_RESULT CRedBlueTest::Second()
{
    HRESULT hr;
    ID3D11Device*           pD3DDevice = NULL;
    ID3D11DeviceContext*    pD3DDeviceContext = NULL;

    // Create the device

    if (GetFramework()->Is11On12())
    {
        hr = GetFramework()->GetD3DDeviceManager()->CreateDevice(NULL,
            GetFramework()->m_D3DOptions.DriverType,
            D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_1_CREATE_DEVICE_CONTEXT_STATE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG,
            *featureLevels,
            (ID3D11Device**)&pD3DDevice,
            &pD3DDeviceContext);
    }
    else
    {

        D3D_FEATURE_LEVEL outFL;
        hr = D3D11CreateDevice(NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            D3D11_CREATE_DEVICE_SINGLETHREADED,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            (ID3D11Device**)&pD3DDevice,
            &outFL,
            &pD3DDeviceContext);
    }

    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed to create 2nd Thread D3D11 Device"));
        return RESULT_FAIL;
    }

    // Create the shared surface to render into
    D3D11_TEXTURE2D_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Width = 1;
    bufferDesc.Height = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.ArraySize = 1;
    bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

    ID3D11Texture2D* pD3DSharedTexture;
    hr = pD3DDevice->CreateTexture2D(&bufferDesc, NULL, &pD3DSharedTexture);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not create shared texture"));
        return RESULT_FAIL;

    }

    IDXGIResource* pResource = NULL;
    hr = pD3DSharedTexture->QueryInterface(__uuidof(IDXGIResource), (void**)&pResource);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not QueryInterface for IDXGIResource on 2nd Thread"));
        return RESULT_FAIL;
    }

    hr = pResource->GetSharedHandle(&sharedSurfaceHandle);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not GetSharedHandle on 2nd Thread"));
        return RESULT_FAIL;
    }

    ID3D11RenderTargetView* pView = NULL;
    hr = pD3DDevice->CreateRenderTargetView(pD3DSharedTexture, NULL, &pView);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Failed CreateRenderTargetView on 2nd Thread"));
        return RESULT_FAIL;
    }

    float RedColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    float BlueColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
    ID3D11DeviceContext2 *pContext2 = nullptr;
    hr = pD3DDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext2), (void **)&pContext2);
    if (FAILED(hr))
    {
        WriteToLog(TEXT("Could not QueryInterface for ID3D11DeviceContext2 on 2nd Thread"));
        return RESULT_FAIL;
    }
    pD3DDeviceContext->OMSetRenderTargets(1, &pView, nullptr);
    pD3DDeviceContext->ClearRenderTargetView(pView, BlueColor);
    pD3DDeviceContext->Flush();

    // Tell main thread we are ready to render
    SetEvent(sharedSurfaceCreatedEvent);

    //int EventCount = 10000;
    // Loop forever alternating the color of the shared surface from red to blue
    while (WaitForSingleObject(terminatEvent, 0) != WAIT_OBJECT_0)
    {
        {
            WrapCS cs = WrapCS(GetFramework()->IsSetRedBlueCS());
            pD3DDeviceContext->ClearRenderTargetView(pView, RedColor);
            pD3DDeviceContext->Flush();
        }

        {
            WrapCS cs = WrapCS(GetFramework()->IsSetRedBlueCS());
            pD3DDeviceContext->ClearRenderTargetView(pView, BlueColor);
            pD3DDeviceContext->Flush();
        }
    }

    // Cleanup
    pView->Release();
    pResource->Release();
    pD3DSharedTexture->Release();
    pD3DDeviceContext->Release();
    pContext2->Release();
    pD3DDevice->Release();

    // Tell main thread we have cleanedup
    SetEvent(sharedSurfaceCreatedEvent);

    return RESULT_PASS;
}


