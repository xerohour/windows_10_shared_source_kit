#include "SyncTransform.h"

#pragma warning(push, 4)

using namespace std;

namespace SyncTransform
{
    RFactor& operator*=(RFactor& l, const RFactor& r)
    {
        l = l * r;
        return l;
    }

    void ValidTransformMatrix::InitTestParametersImpl()
    {
        RFactor RootFactor;
        {
            auto* p = new CUserValueSet<DXGISwapEffectRecord>();
            for(unsigned i = 0; i < cuiNoOfSwapEffects; ++i)
            {
                if (DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) p->AddValue(DXGISwapEffects[i]);
            }
            RootFactor = AddParameter(_T("SwapEffect"), &m_SwapEffect, p);
        }

        {
            auto* p = new CUserValueSet<bool>(true);
            RootFactor *= AddParameter(_T("UseDCOMP"), &m_tp_bWindowless, p);
        }

        SetRootTestFactor(RootFactor * m_FeatureLevelFactor * m_globalRFactorDwmOnBpp * InitSyncTransformParams(*this));
    }

    template<typename T> RFactor init(CVariationGenerator& p, CVariationParameter<T>& c, T d)
    {
        return p.AddParameterValueSet<T>(&c, new CUserValueSet<T>(d));
    }

    #define Init(x) return init( *this, c, x )

    RFactor ValidTransformMatrix::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        Init(StretchFactor_t(1.0f, 1.0f));
    }

    RFactor ValidTransformMatrix::InitOffset(CVariationParameter<Offset_t>& c)
    {
        Init(Offset_t(0, 0));
    }

    RFactor ValidTransformMatrix::InitSourceSize(CVariationParameter<SourceSize_t>& c)
    {
        Init(SourceSize_t(300, 200));
    }

    RFactor ValidTransformMatrix::InitRotation(CVariationParameter<Rotation_t>& c)
    {
        Init(DXGI_MODE_ROTATION_IDENTITY);
    }

    RFactor ValidTransformMatrix::InitCallOrder(CVariationParameter<CallOrder_t>& c)
    {
        Init(make_tuple(TransformType::Matrix, TransformType::SourceSize, TransformType::Rotation));
    }

    #undef Init
}

// StrechFactor
namespace SyncTransform
{
    #define x(v) xStretch( v )
    RFactor StretchFactorX::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        const StretchFactor_t s[] = { x(1.f), x(0.5f), x(2.f), x(-1.f), x(-2.f) };
        return AddParameterValueSet(&c, new CUserValueSet<StretchFactor_t>(s, ARRAYSIZE(s)));
    }
    #undef x

    #define y(v) yStretch( v )
    RFactor StretchFactorY::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        const StretchFactor_t s[] = { y(1.f), y(0.75f), y(1.5f), y(-1.f), y(-1.5f) };
        return AddParameterValueSet(&c, new CUserValueSet<StretchFactor_t>(s, ARRAYSIZE(s)));
    }
    #undef y
}

// Offset
namespace SyncTransform
{
    #define x(v) xOffset( v )
    RFactor OffsetX::InitOffset(CVariationParameter<Offset_t>& c)
    {
        const Offset_t o[] = { x(0), x(100), x(-100) };
        return AddParameterValueSet(&c, new CUserValueSet<Offset_t>(o, ARRAYSIZE(o)));
    }
    #undef x

    #define y(v) yOffset( v )
    RFactor OffsetY::InitOffset(CVariationParameter<Offset_t>& c)
    {
        const Offset_t o[] = { y(0), y(51), y(-51) };
        return AddParameterValueSet(&c, new CUserValueSet<Offset_t>(o, ARRAYSIZE(o)));
    }
    #undef y
}

// Both Stretch & Offset.
namespace SyncTransform
{
    RFactor StretchOffsetCombo::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<StretchFactor_t>(StretchFactor_t(2.0f, 1.5f)));
    }

    RFactor StretchOffsetCombo::InitOffset(CVariationParameter<Offset_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Offset_t>(Offset_t(100, -51)));
    }
}

// Only Rotation
namespace SyncTransform
{
    RFactor PlainRotation::InitRotation(CVariationParameter<Rotation_t>& c)
    {
        const Rotation_t r[] = { DXGI_MODE_ROTATION_IDENTITY, DXGI_MODE_ROTATION_ROTATE90, DXGI_MODE_ROTATION_ROTATE180, DXGI_MODE_ROTATION_ROTATE270 };
        return AddParameterValueSet(&c, new CUserValueSet<Rotation_t>(r, ARRAYSIZE(r)));
    }
}

// Rotation
namespace SyncTransform
{
    RFactor RotationFactor::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        const StretchFactor_t s[] = { StretchFactor_t(1.f, 1.f), StretchFactor_t(2.f, 2.f) };
        return AddParameterValueSet(&c, new CUserValueSet<StretchFactor_t>(s, ARRAYSIZE(s)));
    }

    RFactor RotationFactor::InitOffset(CVariationParameter<Offset_t>& c)
    {
        const Offset_t o[] = { Offset_t(0, 0), Offset_t(100, 100) };
        return AddParameterValueSet(&c, new CUserValueSet<Offset_t>(o, ARRAYSIZE(o)));
    }

    RFactor RotationFactor::InitRotation(CVariationParameter<Rotation_t>& c)
    {
        const Rotation_t r[] = { DXGI_MODE_ROTATION_IDENTITY, DXGI_MODE_ROTATION_ROTATE90, DXGI_MODE_ROTATION_ROTATE180, DXGI_MODE_ROTATION_ROTATE270 };
        return AddParameterValueSet(&c, new CUserValueSet<Rotation_t>(r, ARRAYSIZE(r)));
    }
}

// DirtyScroll
namespace SyncTransform
{
    RFactor DirtyScroll::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        const StretchFactor_t s[] = { StretchFactor_t(1.0f, 1.0f), StretchFactor_t(2.0f, 2.0f) };
        return AddParameterValueSet(&c, new CUserValueSet<StretchFactor_t>(s, ARRAYSIZE(s)));
    }

    RFactor DirtyScroll::InitOffset(CVariationParameter<Offset_t>& c)
    {
        const Offset_t o[] = { Offset_t(0, 0), Offset_t(100, 100) };
        return AddParameterValueSet(&c, new CUserValueSet<Offset_t>(o, ARRAYSIZE(o)));
    }

    void DirtyScroll::InitTestParametersImpl()
    {
        ValidTransformMatrix::InitTestParametersImpl();

        RFactor RootFactor = AddParameter<bool>(_T("DirtyRects"), &m_bUseDirtyRects, new CUserValueSet<bool>(true));
        RootFactor *= AddParameter<SCROLLINGINFO>(_T("ScrollInfo"), &m_ScrollInfo, new CUserValueSet<SCROLLINGINFO>(g_ScrollingInfo, g_uNumOfScrollInfoElements));
        SetRootTestFactor(GetRootTestFactor() * RootFactor);
    }
}

// Only source size.
namespace SyncTransform
{
    RFactor PlainSourceSize::InitSourceSize(CVariationParameter<SourceSize_t>& c)
    {
        const SourceSize_t s[] = { SourceSize_t(278, 304), SourceSize_t(405, 399) };
        return AddParameterValueSet(&c, new CUserValueSet<SourceSize_t>(s, extent<decltype(s)>::value));
    }
}

// Everything.
namespace SyncTransform
{
    RFactor AllCombined::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        const StretchFactor_t s[] = { StretchFactor_t(1.1f, -0.5f), StretchFactor_t(-2.1f, 0.7f) };
        return AddParameterValueSet(&c, new CUserValueSet<StretchFactor_t>(s, ARRAYSIZE(s)));
    }

    RFactor AllCombined::InitOffset(CVariationParameter<Offset_t>& c)
    {
        const Offset_t o[] = { Offset_t(0, 0), Offset_t(105, 46) };
        return AddParameterValueSet(&c, new CUserValueSet<Offset_t>(o, ARRAYSIZE(o)));
    }

    RFactor AllCombined::InitSourceSize(CVariationParameter<SourceSize_t>& c)
    {
        const SourceSize_t s[] = { SourceSize_t(344, 267), SourceSize_t(294, 359) };
        return AddParameterValueSet(&c, new CUserValueSet<SourceSize_t>(s, ARRAYSIZE(s)));
    }

    RFactor AllCombined::InitRotation(CVariationParameter<Rotation_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Rotation_t>(DXGI_MODE_ROTATION_ROTATE90));
    }
}

// Use everything.
namespace SyncTransform
{
    RFactor OrderRotSrcSizeMtx::InitCallOrder(CVariationParameter<CallOrder_t>& c)
    {
        #define Permute(x,y,z) make_tuple(x,y,z), make_tuple(x,z,y), make_tuple(y,x,z), make_tuple(y,z,x), make_tuple(z,x,y), make_tuple(z,y,x)
        const CallOrder_t o[] = { Permute(TransformType::Matrix, TransformType::SourceSize, TransformType::Rotation) };
        #undef Permute
        return AddParameterValueSet(&c, new CUserValueSet<CallOrder_t>(o, ARRAYSIZE(o)));
    }

    RFactor OrderRotSrcSizeMtx::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<StretchFactor_t>(StretchFactor_t(-1.5f, 0.9f)));
    }

    RFactor OrderRotSrcSizeMtx::InitOffset(CVariationParameter<Offset_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Offset_t>(Offset_t(132, 2)));
    }

    RFactor OrderRotSrcSizeMtx::InitSourceSize(CVariationParameter<SourceSize_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<SourceSize_t>(SourceSize_t(257, 243)));
    }

    RFactor OrderRotSrcSizeMtx::InitRotation(CVariationParameter<Rotation_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Rotation_t>(DXGI_MODE_ROTATION_ROTATE270));
    }
}

namespace SyncTransform
{
    RFactor OrderRotSrcSize::InitCallOrder(CVariationParameter<CallOrder_t>& c)
    {
        const CallOrder_t o[] = { make_tuple(TransformType::Rotation, TransformType::SourceSize, TransformType::None),
                                  make_tuple(TransformType::SourceSize, TransformType::Rotation, TransformType::None) };
        return AddParameterValueSet(&c, new CUserValueSet<CallOrder_t>(o, ARRAYSIZE(o)));
    }

    RFactor OrderRotSrcSize::InitSourceSize(CVariationParameter<SourceSize_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<SourceSize_t>(SourceSize_t(322, 311)));
    }

    RFactor OrderRotSrcSize::InitRotation(CVariationParameter<Rotation_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Rotation_t>(DXGI_MODE_ROTATION_ROTATE90));
    }
}

namespace SyncTransform
{
    RFactor OrderRotOffset::InitCallOrder(CVariationParameter<CallOrder_t>& c)
    {
        const CallOrder_t o[] = { make_tuple(TransformType::Rotation, TransformType::Matrix, TransformType::None),
                                  make_tuple(TransformType::Matrix, TransformType::Rotation, TransformType::None) };
        return AddParameterValueSet(&c, new CUserValueSet<CallOrder_t>(o, ARRAYSIZE(o)));
    }

    RFactor OrderRotOffset::InitRotation(CVariationParameter<Rotation_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Rotation_t>(DXGI_MODE_ROTATION_ROTATE180));
    }

    RFactor OrderRotOffset::InitOffset(CVariationParameter<Offset_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Offset_t>(Offset_t(-3, 13)));
    }
}

namespace SyncTransform
{
    RFactor OrderRotStretch::InitCallOrder(CVariationParameter<CallOrder_t>& c)
    {
        const CallOrder_t o[] = { make_tuple(TransformType::Rotation, TransformType::Matrix, TransformType::None),
                                  make_tuple(TransformType::Matrix, TransformType::Rotation, TransformType::None) };
        return AddParameterValueSet(&c, new CUserValueSet<CallOrder_t>(o, ARRAYSIZE(o)));
    }

    RFactor OrderRotStretch::InitRotation(CVariationParameter<Rotation_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Rotation_t>(DXGI_MODE_ROTATION_ROTATE270));
    }

    RFactor OrderRotStretch::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<StretchFactor_t>(StretchFactor_t(-1.5f, -0.9f)));
    }
}

namespace SyncTransform
{
    RFactor OrderRotStretchOff::InitCallOrder(CVariationParameter<CallOrder_t>& c)
    {
        const CallOrder_t o[] = { make_tuple(TransformType::Rotation, TransformType::Matrix, TransformType::None),
                                  make_tuple(TransformType::Matrix, TransformType::Rotation, TransformType::None) };
        return AddParameterValueSet(&c, new CUserValueSet<CallOrder_t>(o, ARRAYSIZE(o)));
    }

    RFactor OrderRotStretchOff::InitRotation(CVariationParameter<Rotation_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Rotation_t>(DXGI_MODE_ROTATION_ROTATE270));
    }

    RFactor OrderRotStretchOff::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<StretchFactor_t>(StretchFactor_t(-0.322f, 0.34f)));
    }

    RFactor OrderRotStretchOff::InitOffset(CVariationParameter<Offset_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Offset_t>(Offset_t(-65, 34)));
    }
}

namespace SyncTransform
{
    RFactor OrderSrcSizeOff::InitCallOrder(CVariationParameter<CallOrder_t>& c)
    {
        const CallOrder_t o[] = { make_tuple(TransformType::SourceSize, TransformType::Matrix, TransformType::None),
                                  make_tuple(TransformType::Matrix, TransformType::SourceSize, TransformType::None) };
        return AddParameterValueSet(&c, new CUserValueSet<CallOrder_t>(o, ARRAYSIZE(o)));
    }

    RFactor OrderSrcSizeOff::InitOffset(CVariationParameter<Offset_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Offset_t>(Offset_t(11, -99)));
    }

    RFactor OrderSrcSizeOff::InitSourceSize(CVariationParameter<SourceSize_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<SourceSize_t>(SourceSize_t(432, 324)));
    }
}

namespace SyncTransform
{
    RFactor OrderSrcSizeStretch::InitCallOrder(CVariationParameter<CallOrder_t>& c)
    {
        const CallOrder_t o[] = { make_tuple(TransformType::SourceSize, TransformType::Matrix, TransformType::None),
                                  make_tuple(TransformType::Matrix, TransformType::SourceSize, TransformType::None) };
        return AddParameterValueSet(&c, new CUserValueSet<CallOrder_t>(o, ARRAYSIZE(o)));
    }

    RFactor OrderSrcSizeStretch::InitSourceSize(CVariationParameter<SourceSize_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<SourceSize_t>(SourceSize_t(243, 411)));
    }

    RFactor OrderSrcSizeStretch::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<StretchFactor_t>(StretchFactor_t(0.55f, -0.941f)));
    }
}

namespace SyncTransform
{
    RFactor OrderSrcSizeStretchOff::InitCallOrder(CVariationParameter<CallOrder_t>& c)
    {
        const CallOrder_t o[] = { make_tuple(TransformType::SourceSize, TransformType::Matrix, TransformType::None),
                                  make_tuple(TransformType::Matrix, TransformType::SourceSize, TransformType::None) };
        return AddParameterValueSet(&c, new CUserValueSet<CallOrder_t>(o, ARRAYSIZE(o)));
    }

    RFactor OrderSrcSizeStretchOff::InitSourceSize(CVariationParameter<SourceSize_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<SourceSize_t>(SourceSize_t(301, 234)));
    }

    RFactor OrderSrcSizeStretchOff::InitStretchFactor(CVariationParameter<StretchFactor_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<StretchFactor_t>(StretchFactor_t(1.11f, 0.91f)));
    }

    RFactor OrderSrcSizeStretchOff::InitOffset(CVariationParameter<Offset_t>& c)
    {
        return AddParameterValueSet(&c, new CUserValueSet<Offset_t>(Offset_t(-133, 156)));
    }
}

#pragma warning(pop)