#include "SyncTransform.h"

using HelperType::HrException;

template<typename U, typename T> U GetParent(T& a)
{
    U u;
    HrException::Throw(a.GetParent(__uuidof(u), reinterpret_cast<void** const>(&u)), _T("GetParent failed."));
    return u;
}

template<typename U, typename T> U QueryInterface(T& a)
{
    U u;
    HrException::Throw(a.QueryInterface(__uuidof(u), reinterpret_cast<void** const>(&u)), _T("QueryInterface failed."));
    return u;
}

CHandle<IDXGIFactory2> CreateFactory()
{
    CHandle<IDXGIFactory2> Factory;
    HrException::Throw(CreateDXGIFactory2(0, __uuidof(Factory), reinterpret_cast<void** const>(&Factory)), _T("CreateDXGIFactory2 failed."));
    return Factory;
}

CHandle<IDXGIAdapter1> GetAdapter1(IDXGIFactory2& Factory)
{
    CHandle<IDXGIAdapter1> Adapter;
    HrException::Throw(Factory.EnumAdapters1(0, &Adapter), _T("IDXGIFactory2::EnumAdapters1 failed."));
    return Adapter;
}

CHandle<ID3D11Device2> CreateDeviceCore(IDXGIAdapter1& Adapter)
{
    // With feature level promotion this will net the highest possible feature level available.
    const D3D_FEATURE_LEVEL FLs[] = {D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_1};
    const unsigned FLCount = ARRAYSIZE(FLs);

    CHandle<ID3D11Device> DeviceOld;
    HrException::Throw(D3D11CreateDevice(&Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, FLs, FLCount, D3D11_SDK_VERSION, &DeviceOld, nullptr, nullptr), _T("D3D11CreateDevice failed."));

    CHandle<ID3D11Device2> Device;
    HrException::Throw(DeviceOld->QueryInterface(__uuidof(Device), reinterpret_cast<void** const>(&Device)), _T("QueryInterface failed (ID3D11Device -> ID3D11Device2"));
    return Device;
}

CHandle<ID3D11Device2> CreateDevice(IDXGIFactory2& Factory)
{
    return CreateDeviceCore(*GetAdapter1(Factory));
}

CHandle<ID3D11Device2> CreateDevice()
{
    return CreateDevice(*CreateFactory());
}

CHandle<IDXGIFactory2> GetFactory(ID3D11Device2& Device)
{
    CHandle<IDXGIDevice> DxgiDevice = QueryInterface<decltype(DxgiDevice)>(Device);
    CHandle<IDXGIAdapter> Adapter = GetParent<decltype(Adapter)>(*DxgiDevice);
    return GetParent<CHandle<IDXGIFactory2>>(*Adapter);
}

CHandle<IDXGISwapChain2> CreateCompositionSwapChain(ID3D11Device2& Device, IDXGIFactory2& Factory)
{
    DXGI_SWAP_CHAIN_DESC1 d = {};
    d.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    d.Width = 1;
    d.Height = 1;
    d.BufferCount = 2;
    d.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    d.SampleDesc.Count = 1;
    d.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    CHandle<IDXGISwapChain1> sc;
    HrException::Throw(Factory.CreateSwapChainForComposition(&Device, &d, NULL, &sc), _T("Swapchain creation failed."));
    CHandle<IDXGISwapChain2> sc2 = QueryInterface<decltype(sc2)>(*sc);
    return sc2;
}

CHandle<IDXGISwapChain2> CreateWindowedSwapChain(ID3D11Device2& Device, IDXGIFactory2& Factory, HWND h)
{
    DXGI_SWAP_CHAIN_DESC1 d = {};
    d.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    d.BufferCount = 2;
    d.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    d.SampleDesc.Count = 1;
    d.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    CHandle<IDXGISwapChain1> sc;
    HrException::Throw(Factory.CreateSwapChainForHwnd(&Device, h, &d, nullptr, nullptr, &sc), _T("Swapchain creation failed."));
    CHandle<IDXGISwapChain2> sc2 = QueryInterface<decltype(sc2)>(*sc);
    return sc2;
}

void RegisterClass(const TCHAR* const Class, HINSTANCE hInstance, WNDPROC WindowProc)
{
    // Register class
    WNDCLASSEX WndClass = {};
    WndClass.cbSize = sizeof(WNDCLASSEX);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = WindowProc;
    WndClass.cbWndExtra = sizeof(LONG_PTR);
    WndClass.hInstance = hInstance;
    WndClass.lpszClassName = Class;
    HrException::Throw(RegisterClassEx(&WndClass) ? S_OK : E_UNEXPECTED, _T("Window class registration failed."));
}

HWND MakeWindow(const TCHAR* const Class, const TCHAR* const Title, HINSTANCE hInstance, unsigned w = 800, unsigned h = 600)
{
    const HWND Window = CreateWindowEx(0, Class, Title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, w, h, NULL, NULL, hInstance, 0);
    HrException::Throw(Window ? S_OK : E_UNEXPECTED, "Window creation failed.");
    return Window;
}

DXGI_MATRIX_3X2_F GetMatrix(IDXGISwapChain2& sc)
{
    DXGI_MATRIX_3X2_F m;
    HrException::Throw(sc.GetMatrixTransform(&m), _T("GetMatrix failed."));
    return m;
}

void SetMatrix(IDXGISwapChain2& sc, const DXGI_MATRIX_3X2_F& m)
{
    HrException::Throw(sc.SetMatrixTransform(&m), "SetMatrixTransform failed.");
}

void SetRotation(IDXGISwapChain2& sc, DXGI_MODE_ROTATION r)
{
    HrException::Throw(sc.SetRotation(r), "SetRotation failed.");
}

void SetSourceSize(IDXGISwapChain2& sc, unsigned x, unsigned y)
{
    HrException::Throw(sc.SetSourceSize(x, y), "SetSourceSize failed.");
}

tstring ToString(SyncTransform::SpecialCases::TestCase e)
{
    switch(e)
    {
        case SyncTransform::SpecialCases::TestCase::DefaultMatrix: return _T("DefaultMatrix");
        case SyncTransform::SpecialCases::TestCase::InvalidMatrix: return _T("InvalidMatrix");
        case SyncTransform::SpecialCases::TestCase::BadAfterGood: return _T("BadAfterGood");
        case SyncTransform::SpecialCases::TestCase::Zero: return _T("Zero");
        case SyncTransform::SpecialCases::TestCase::FloatMax: return _T("FloatMax");
        case SyncTransform::SpecialCases::TestCase::Infinity: return _T("Infinity");
        case SyncTransform::SpecialCases::TestCase::LocalToSC: return _T("LocalToSC");
        case SyncTransform::SpecialCases::TestCase::SetOnHwnd: return _T("SetOnHwnd");
        case SyncTransform::SpecialCases::TestCase::Nan: return _T("Nan");
        case SyncTransform::SpecialCases::TestCase::SetNullptr: return _T("SetNullptr");
        case SyncTransform::SpecialCases::TestCase::GetNullptr: return _T("GetNullptr");
    }
    return _T("");
}

bool FromString(SyncTransform::SpecialCases::TestCase* e, const tstring& s)
{
    if (s == _T("DefaultMatrix")) { *e = SyncTransform::SpecialCases::TestCase::DefaultMatrix; return true; }
    if (s == _T("InvalidMatrix")) { *e = SyncTransform::SpecialCases::TestCase::InvalidMatrix; return true; }
    if (s == _T("BadAfterGood")) { *e = SyncTransform::SpecialCases::TestCase::BadAfterGood; return true; }
    if (s == _T("Zero")) { *e = SyncTransform::SpecialCases::TestCase::Zero; return true; }
    if (s == _T("FloatMax")) { *e = SyncTransform::SpecialCases::TestCase::FloatMax; return true; }
    if (s == _T("Infinity")) { *e = SyncTransform::SpecialCases::TestCase::Infinity; return true; }
    if (s == _T("LocalToSC")) { *e = SyncTransform::SpecialCases::TestCase::LocalToSC; return true; }
    if (s == _T("SetOnHwnd")) { *e = SyncTransform::SpecialCases::TestCase::SetOnHwnd; return true; }
    if (s == _T("Nan")) { *e = SyncTransform::SpecialCases::TestCase::Nan; return true; }
    if (s == _T("SetNullptr")) { *e = SyncTransform::SpecialCases::TestCase::SetNullptr; return true; }
    if (s == _T("GetNullptr")) { *e = SyncTransform::SpecialCases::TestCase::GetNullptr; return true; }
    return false;
}

namespace SyncTransform
{
    const TCHAR ClassTitle[] = _T("SyncTransformSpecialCasesClass");
    const TCHAR WindowTitle[] = _T("SyncTransformSpecialCasesWindow");

    void SpecialCases::InitTestParameters()
    {
        CreateParameter(_T("TestCase"), &pTestCase);

        auto pTestCases = new CUserValueSet<TestCase>;
        pTestCases->AddValue(TestCase::DefaultMatrix);
        pTestCases->AddValue(TestCase::InvalidMatrix);
        pTestCases->AddValue(TestCase::BadAfterGood);
        pTestCases->AddValue(TestCase::Zero);
        pTestCases->AddValue(TestCase::FloatMax);
        pTestCases->AddValue(TestCase::Infinity);
        pTestCases->AddValue(TestCase::LocalToSC);
        pTestCases->AddValue(TestCase::SetOnHwnd);
        pTestCases->AddValue(TestCase::Nan);
        pTestCases->AddValue(TestCase::SetNullptr);
        pTestCases->AddValue(TestCase::GetNullptr);

        SetRootTestFactor(AddParameterValueSet<TestCase>(pTestCase, pTestCases));
    }

    TEST_RESULT SpecialCases::Setup()
    {
        try
        {
            RegisterClass(ClassTitle, GetModuleHandle(NULL), DefWindowProc);
            decltype(Device) d = CreateDevice();
            decltype(Factory) f = GetFactory(*d);

            // No more exceptions.
            Device = d;
            Factory = f;
        }
        catch (const HrException& e)
        {
            e.log();
            return RESULT_FAIL;
        }
        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::ExecuteTestCase()
    {
        try
        {
            switch(pTestCase->GetValue())
            {
                case TestCase::DefaultMatrix: return DefaultMatrix();
                case TestCase::InvalidMatrix: return InvalidMatrix();
                case TestCase::BadAfterGood: return BadAfterGood();
                case TestCase::Zero: return Zero();
                case TestCase::FloatMax: return FloatMax();
                case TestCase::Infinity: return Infinity();
                case TestCase::LocalToSC: return LocalToSC();
                case TestCase::SetOnHwnd: return SetOnHwnd();
                case TestCase::Nan: return Nan();
                case TestCase::SetNullptr: return SetNullptr();
                case TestCase::GetNullptr: return GetNullptr();
            }
        }
        catch (const HrException& e)
        {
            e.log();
            return RESULT_FAIL;
        }

        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::DefaultMatrix()
    {
        auto sc = CreateCompositionSwapChain(*Device, *Factory);
        auto const m = GetMatrix(*sc);
        const DXGI_MATRIX_3X2_F Default = {1.f, 0.f, 0.f, 1.f, 0.f, 0.f};
        if (!!memcmp(&m, &Default, sizeof(m)))
        {
            WriteToLog(_T("Default matrix was not returned. Matrix returned: %f, %f, %f, %f, %f, %f."), m._11, m._12, m._21, m._22, m._31, m._32);
            return RESULT_FAIL;
        }
        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::InvalidMatrix()
    {
        auto sc = CreateCompositionSwapChain(*Device, *Factory);
        const DXGI_MATRIX_3X2_F InvalidMatrix = {0.f, -0.5f, 0.5f, 0.f, 0.f, 0.f};
        HRESULT hr = sc->SetMatrixTransform(&InvalidMatrix);
        if (SUCCEEDED(hr))
        {
            WriteToLog(_T("Invalid matrix was allowed to be set."));
            return RESULT_FAIL;
        }
        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::BadAfterGood()
    {
        auto sc = CreateCompositionSwapChain(*Device, *Factory);
        const DXGI_MATRIX_3X2_F good = {2.f, 0.f, 0.f, 2.f, 0.f, 0.f};
        SetMatrix(*sc, good);
        SetRotation(*sc, DXGI_MODE_ROTATION_ROTATE90);
        SetSourceSize(*sc, 1, 1);

        const DXGI_MATRIX_3X2_F bad = {0.f, -0.5f, 0.5f, 0.f, 0.f, 0.f};
        HRESULT hr = sc->SetMatrixTransform(&bad);
        if (SUCCEEDED(hr))
        {
            WriteToLog(_T("Invalid matrix was allowed to be set."));
            return RESULT_FAIL;
        }

        const DXGI_MATRIX_3X2_F m = GetMatrix(*sc);
        if (!!memcmp(&m, &good, sizeof(m)))
        {
            WriteToLog(_T("Good matrix was not returned. Matrix returned: %f, %f, %f, %f, %f, %f."), m._11, m._12, m._21, m._22, m._31, m._32);
            return RESULT_FAIL;
        }
        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::Zero()
    {
        auto sc = CreateCompositionSwapChain(*Device, *Factory);
        const DXGI_MATRIX_3X2_F f = {};
        SetMatrix(*sc, f);
        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::FloatMax()
    {
        auto sc = CreateCompositionSwapChain(*Device, *Factory);
        const DXGI_MATRIX_3X2_F f = {FLT_MAX, 0.f, 0.f, FLT_MAX, 0.f, 0.f};
        SetMatrix(*sc, f);
        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::Infinity()
    {
        auto sc = CreateCompositionSwapChain(*Device, *Factory);
        const DXGI_MATRIX_3X2_F f = {std::numeric_limits<float>::infinity(), 0.f, 0.f, std::numeric_limits<float>::infinity(), 0.f, 0.f};
        HRESULT hr = sc->SetMatrixTransform(&f);
        if (SUCCEEDED(hr))
        {
            WriteToLog(_T("Able to set infinite matrix."));
            return RESULT_FAIL;
        }
        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::LocalToSC()
    {
        auto sc = CreateCompositionSwapChain(*Device, *Factory);
        auto sc2 = CreateCompositionSwapChain(*Device, *Factory);
        const DXGI_MATRIX_3X2_F good = {2.f, 0.f, 0.f, 2.f, 0.f, 0.f};
        SetMatrix(*sc, good);

        const auto c = GetMatrix(*sc2);
        if (!memcmp(&good, &c, sizeof(c)))
        {
            WriteToLog(_T("Matrix on one swapchain affected another."));
            return RESULT_FAIL;
        }
        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::SetOnHwnd()
    {
        auto sc = CreateWindowedSwapChain(*Device, *Factory, MakeWindow(ClassTitle, WindowTitle, GetModuleHandle(NULL)));
        const DXGI_MATRIX_3X2_F good = {2.f, 0.f, 0.f, 2.f, 0.f, 0.f};
        HRESULT hr = sc->SetMatrixTransform(&good);
        if (SUCCEEDED(hr))
        {
            WriteToLog(_T("Able to set matrix on hwnd swapchain."));
            return RESULT_FAIL;
        }
        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::Nan()
    {
        auto sc = CreateCompositionSwapChain(*Device, *Factory);
        const DXGI_MATRIX_3X2_F bad = {std::numeric_limits<float>::quiet_NaN(), 0.f, 0.f, std::numeric_limits<float>::quiet_NaN(), 0.f, 0.f};
        HRESULT hr = sc->SetMatrixTransform(&bad);
        if (SUCCEEDED(hr))
        {
            WriteToLog(_T("Able to set a matrix with NaN."));
            return RESULT_FAIL;
        }
        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::SetNullptr()
    {
        auto sc = CreateCompositionSwapChain(*Device, *Factory);
        HRESULT hr = sc->SetMatrixTransform(nullptr);
        if (SUCCEEDED(hr))
        {
            WriteToLog(_T("Successful set with nullptr."));
            return RESULT_FAIL;
        }
        return RESULT_PASS;
    }

    TEST_RESULT SpecialCases::GetNullptr()
    {
        auto sc = CreateCompositionSwapChain(*Device, *Factory);
        HRESULT hr = sc->GetMatrixTransform(nullptr);
        if (SUCCEEDED(hr))
        {
            WriteToLog(_T("Successful get with nullptr."));
            return RESULT_FAIL;
        }
        return RESULT_PASS;
    }
}