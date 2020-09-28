#ifndef _SYNC_TRANSFORM_H_
#define _SYNC_TRANSFORM_H_

#include "DXGIPresent.h"

#pragma warning(push, 4)

#include <type_traits>

#undef ARRAYSIZE

#define ARRAYSIZE(x) std::extent<decltype( x )>::value

namespace SyncTransform
{
    using testfactor::RFactor;
    class ValidTransformMatrix : public CPresentWinConvertStretch_<true>
    {
        public:
            ValidTransformMatrix() : CPresentWinConvertStretch_(DXGI_FORMAT_R8G8B8A8_UNORM){}
            virtual void InitTestParametersImpl();
            virtual RFactor InitStretchFactor(CVariationParameter<StretchFactor_t>&);
            virtual RFactor InitOffset(CVariationParameter<Offset_t>&);
            virtual RFactor InitSourceSize(CVariationParameter<SourceSize_t>&);
            virtual RFactor InitRotation(CVariationParameter<Rotation_t>&);
            virtual RFactor InitCallOrder(CVariationParameter<CallOrder_t>&);

        protected:
            static StretchFactor_t xStretch(float x) { return StretchFactor_t(x, 1.f); }
            static StretchFactor_t yStretch(float y) { return StretchFactor_t(1.f, y); }
            static Offset_t xOffset(int x) { return Offset_t(x, 0); }
            static Offset_t yOffset(int y) { return Offset_t(0, y); }

        private:
            ValidTransformMatrix& operator=(const ValidTransformMatrix&);
    };

    #define Use(param) protected: bool Use##param () const { return true; } RFactor Init##param (CVariationParameter< param##_t >&)

    class StretchFactorX : public ValidTransformMatrix
    {
        Use(StretchFactor);
    };

    class StretchFactorY : public ValidTransformMatrix
    {
        Use(StretchFactor);
    };

    class OffsetX : public ValidTransformMatrix
    {
        Use(Offset);
    };

    class OffsetY : public ValidTransformMatrix
    {
        Use(Offset);
    };

    class StretchOffsetCombo : public ValidTransformMatrix
    {
        Use(StretchFactor);
        Use(Offset);
    };

    class PlainRotation : public ValidTransformMatrix
    {
        Use(Rotation);
    };

    class RotationFactor : public ValidTransformMatrix
    {
        Use(StretchFactor);
        Use(Offset);
        Use(Rotation);
    };

    class DirtyScroll : public ValidTransformMatrix
    {
        Use(StretchFactor);
        Use(Offset);
        public:
            void InitTestParametersImpl();
    };

    class PlainSourceSize : public ValidTransformMatrix
    {
        Use(SourceSize);
    };

    class AllCombined : public ValidTransformMatrix
    {
        Use(Offset);
        Use(StretchFactor);
        Use(Rotation);
        Use(SourceSize);
    };

    // Mtx (Stretch + Offset); Rotation; SourceSize;
    class OrderRotSrcSizeMtx : public ValidTransformMatrix
    {
        Use(CallOrder);
        Use(Rotation);
        Use(SourceSize);

        // Matrix
        Use(StretchFactor);
        Use(Offset);
    };

    // Rotation; SourceSize;
    class OrderRotSrcSize : public ValidTransformMatrix
    {
        Use(CallOrder);
        Use(Rotation);
        Use(SourceSize);
    };

    // Rotation; Mtx(Offset);
    class OrderRotOffset : public ValidTransformMatrix
    {
        Use(CallOrder);
        Use(Rotation);
        Use(Offset);
    };

    // Rotation; Mtx(Stretch)
    class OrderRotStretch : public ValidTransformMatrix
    {
        Use(CallOrder);
        Use(Rotation);
        Use(StretchFactor);
    };

    // Rotation; Mtx (Stretch + Offset);
    class OrderRotStretchOff : public ValidTransformMatrix
    {
        Use(CallOrder);
        Use(Rotation);
        Use(StretchFactor);
        Use(Offset);
    };

    // SourceSize; Mtx(Offset)
    class OrderSrcSizeOff : public ValidTransformMatrix
    {
        Use(CallOrder);
        Use(SourceSize);
        Use(Offset);
    };

    // SourceSize; Mtx(Stretch)
    class OrderSrcSizeStretch : public ValidTransformMatrix
    {
        Use(CallOrder);
        Use(SourceSize);
        Use(StretchFactor);
    };

    // SourceSize; Mtx(Stretch + Offset)
    class OrderSrcSizeStretchOff: public ValidTransformMatrix
    {
        Use(CallOrder);
        Use(SourceSize);
        Use(Offset);
        Use(StretchFactor);
    };

    #undef Use
}

namespace SyncTransform
{
    class SpecialCases : public CTest
    {
        public:
            void InitTestParameters();
            TEST_RESULT ExecuteTestCase();
            TEST_RESULT Setup();

            enum class TestCase
            {
                DefaultMatrix,
                InvalidMatrix,
                BadAfterGood,
                Zero,
                FloatMax,
                Infinity,
                LocalToSC,
                SetOnHwnd,
                Nan,
                SetNullptr,
                GetNullptr
            };

        private:
            TEST_RESULT DefaultMatrix();
            TEST_RESULT InvalidMatrix();
            TEST_RESULT BadAfterGood();
            TEST_RESULT Zero();
            TEST_RESULT FloatMax();
            TEST_RESULT Infinity();
            TEST_RESULT LocalToSC();
            TEST_RESULT SetOnHwnd();
            TEST_RESULT Nan();
            TEST_RESULT SetNullptr();
            TEST_RESULT GetNullptr();

            CVariationParameter<TestCase>* pTestCase;

            CHandle<IDXGIFactory2> Factory;
            CHandle<ID3D11Device2> Device;
    };
}

#pragma warning(pop)

#endif