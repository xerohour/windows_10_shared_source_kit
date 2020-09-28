#include "DXGIPresent.h"

using HelperType::HrException;

#pragma warning(push, 4)

struct MatrixHelper : public DXGI_MATRIX_3X2_F
{
    static const DXGI_MATRIX_3X2_F NullTransform;
    MatrixHelper(const Vector2D<float>& Stretch, const Vector2D<int>& Offset) : DXGI_MATRIX_3X2_F(NullTransform)
    {
        SetStretch(Stretch);
        SetOffset(Offset);
    }

    void SetStretch(const Vector2D<float>& s)
    {
        DXGI_MATRIX_3X2_F::_11 = s.x();
        DXGI_MATRIX_3X2_F::_22 = s.y();
    }

    void SetOffset(const Vector2D<int>& o)
    {
        DXGI_MATRIX_3X2_F::_31 = static_cast<float>(o.x());
        DXGI_MATRIX_3X2_F::_32 = static_cast<float>(o.y());
    }
};

decltype(MatrixHelper::NullTransform) MatrixHelper::NullTransform = {1.f, 0.f, 0.f, 1.f, 0.f, 0.f};

RFactor SyncTransformTest<true>::InitSyncTransformParams(CVariationGenerator& c)
{
    CreateStretchFactor(c);
    CreateOffset(c);
    CreateSourceSize(c);
    CreateRotation(c);
    CreateCallOrder(c);

    return InitStretchFactor(*StretchFactor) * InitOffset(*Offset) * InitSourceSize(*SourceSize) * InitRotation(*Rotation) * InitCallOrder(*CallOrder);
}

Vector2D<unsigned> Dimensions(IDXGISwapChain2& sc)
{
    DXGI_SWAP_CHAIN_DESC1 d;
    sc.GetDesc1(&d);
    return Vector2D<unsigned>(d.Width, d.Height);
}

void SyncTransformTest<true>::SetTransformTypes(IDXGISwapChain2& sc, const DXGI_MATRIX_3X2_F& m, const TransformOrder_t& order)
{
    const TransformType o[] = {get<0>(order), get<1>(order), get<2>(order)};
    for (auto i = 0; i < std::extent<decltype(o)>::value; ++i)
    {
        switch(o[i])
        {
            case TransformType::Matrix:
            {
                if (UseStretchFactor() || UseOffset()) HrException::Throw(sc.SetMatrixTransform(&m), _T("IDXGISwapChain2::SetMatrixTransform failed."));
                break;
            }

            case TransformType::SourceSize:
            {
                const auto s = Helper::Scale(GetSourceSize(), GetStretchFactor().inverse().abs());
                if (UseSourceSize()) HrException::Throw(sc.SetSourceSize(s.x(), s.y()), "IDXGISwapChain2::SetSourceSize failed.");
                break;
            }

            case TransformType::Rotation:
            {
                if (UseRotation()) HrException::Throw(sc.SetRotation(GetRotation()), _T("IDXGISwapChain1::SetRotation failed."));
                break;
            }
        }
    }

    WriteOrderToLog(order);
}

const TCHAR* TypeString(TransformType t)
{
    switch(t)
    {
        case TransformType::None: return _T("None");
        case TransformType::Matrix: return _T("SetMatrixTransform");
        case TransformType::SourceSize: return _T("SetSourceSize");
        case TransformType::Rotation: return _T("SetRotation");
    }
    assert(false);
    return nullptr;
}

tstring ToString(const TransformOrder_t& c)
{
    tstringstream s;
    s << TypeString(get<0>(c)) << _T(", ") << TypeString(get<1>(c)) << _T(", ") << TypeString(get<2>(c));
    return s.str();
}

bool FromString(TransformOrder_t*, const tstring &)
{
    return false;
}

void SyncTransformTest<true>::WriteOrderToLog(const TransformOrder_t& o)
{
    CTest::WriteToLog(_T("Synchronized transforms applied in order: %s, %s, %s"), TypeString(get<0>(o)), TypeString(get<1>(o)), TypeString(get<2>(o)));
}

decltype(SyncTransformTest<true>::WindowRect) SyncTransformTest<true>::WindowRect = {0, 0, 642, 477};

CHandle<IDXGISwapChain2> GetSwapChain(IDXGISwapChain& SwapChain)
{
    CHandle<IDXGISwapChain2> sp;
    HrException::Throw(SwapChain.QueryInterface(__uuidof(sp), reinterpret_cast<void**>(&sp)), _T("IDXGISwapChain::QueryInterface failed."));
    return sp;
}

bool SyncTransformTest<true>::SetSyncTransform(IDXGISwapChain& SwapChain, const RECT& WindowRect)
{
    try
    {
        CHandle<IDXGISwapChain2> sp = GetSwapChain(SwapChain);
        IDXGISwapChain2& sc = *sp;

        SetTransformTypes(sc, TransformMatrix(GetStretchFactor(), GetOffset(), GetRotation(), WindowRect), GetCallOrder());
    }
    catch(const HrException& e)
    {
        e.log();
        return false;
    }

    return true;
}

auto SyncTransformTest<true>::AccountForNegativeStretch(const Offset_t& o, const StretchFactor_t& s, const Vector2D<int>& NegativeStretchOffset) -> Offset_t
{
    const auto additive = Offset_t(Helper::signbit(s.x()) ? NegativeStretchOffset.x() : 0,
                                   Helper::signbit(s.y()) ? NegativeStretchOffset.y() : 0);
    return Offset_t(o.x() + additive.x(), o.y() + additive.y());
}

DXGI_MATRIX_3X2_F SyncTransformTest<true>::TransformMatrix(const StretchFactor_t& s, const Offset_t& o, const Rotation_t& rot, const RECT& WindowRect)
{
    // Our (this test) concept of StretchFactor_t is based on the swapchain buffer size. StretchFactor_t::x corresponds to width. StretchFactor_t::y corresponds to height.
    // This differs from DXGI's concept when we are dealing with a swapchain that will be rotated by 90 or 270 degress (OddRotation() == odd multiple of 90).
    // Hence the swap before setting things in DXGI_MATRIX_3X2_F which will be given to DXGI.

    // WindowRect also corresponds to width & height of the swapchain. The actual window rect may be different (x & y may be switched).
    const auto DxgiStretch = s.swap(rot);
    const auto DxgiWindowDimOff = Helper::Dimensions(WindowRect).swap(rot) + Vector2D<int>(WindowRect.left, WindowRect.top);

    return MatrixHelper(DxgiStretch, AccountForNegativeStretch(o, DxgiStretch, DxgiWindowDimOff));
}

#pragma warning(pop)