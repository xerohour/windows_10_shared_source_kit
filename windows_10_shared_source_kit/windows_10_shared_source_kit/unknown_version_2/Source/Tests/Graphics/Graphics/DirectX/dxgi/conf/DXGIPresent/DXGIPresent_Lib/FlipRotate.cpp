#include <tuple>
#include <type_traits>
#include <memory>
#include <algorithm>
#include <Wincodec.h>
#include <assert.h>
#include <Handle.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "FormatDesc.hpp"
#include "WGFHelpers.h"
#include "FlipRotate.h"

#pragma warning(push, 4)

using namespace DirectX;
using namespace DirectX::PackedVector;
using WindowsTest::CHandle;
using HelperType::Vector2D;
using HelperType::HrException;
using namespace std;

namespace DirectX
{
    GUID _DXGIToWIC(DXGI_FORMAT f)
    {
        const tuple<DXGI_FORMAT, GUID> WICFormats[] =
        {
            make_tuple(DXGI_FORMAT_R32G32B32A32_FLOAT, GUID_WICPixelFormat128bppRGBAFloat),
            make_tuple(DXGI_FORMAT_R16G16B16A16_FLOAT, GUID_WICPixelFormat64bppRGBAHalf),
            make_tuple(DXGI_FORMAT_R16G16B16A16_UNORM, GUID_WICPixelFormat64bppRGBA),
            make_tuple(DXGI_FORMAT_R8G8B8A8_UNORM, GUID_WICPixelFormat32bppRGBA),
            make_tuple(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, GUID_WICPixelFormat32bppRGBA),
            make_tuple(DXGI_FORMAT_B8G8R8A8_UNORM, GUID_WICPixelFormat32bppBGRA),
            make_tuple(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, GUID_WICPixelFormat32bppBGRA),
            make_tuple(DXGI_FORMAT_B8G8R8X8_UNORM, GUID_WICPixelFormat32bppBGR),
            make_tuple(DXGI_FORMAT_B8G8R8X8_UNORM_SRGB, GUID_WICPixelFormat32bppBGR),
            make_tuple(DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, GUID_WICPixelFormat32bppRGBA1010102XR),
            make_tuple(DXGI_FORMAT_R10G10B10A2_UNORM, GUID_WICPixelFormat32bppRGBA1010102),
            make_tuple(DXGI_FORMAT_B5G5R5A1_UNORM, GUID_WICPixelFormat16bppBGRA5551),
            make_tuple(DXGI_FORMAT_B5G6R5_UNORM, GUID_WICPixelFormat16bppBGR565),
            make_tuple(DXGI_FORMAT_R32_FLOAT, GUID_WICPixelFormat32bppGrayFloat),
            make_tuple(DXGI_FORMAT_D32_FLOAT, GUID_WICPixelFormat32bppGrayFloat),
            make_tuple(DXGI_FORMAT_R16_FLOAT, GUID_WICPixelFormat16bppGrayHalf),
            make_tuple(DXGI_FORMAT_R16_UNORM, GUID_WICPixelFormat16bppGray),
            make_tuple(DXGI_FORMAT_D16_UNORM, GUID_WICPixelFormat16bppGray),
            make_tuple(DXGI_FORMAT_R8_UNORM, GUID_WICPixelFormat8bppGray),
            make_tuple(DXGI_FORMAT_A8_UNORM, GUID_WICPixelFormat8bppAlpha),
            make_tuple(DXGI_FORMAT_R1_UNORM, GUID_WICPixelFormatBlackWhite),
        };

        const tuple<DXGI_FORMAT, GUID>* End = WICFormats + extent<decltype(WICFormats)>::value;

        auto it = find_if(WICFormats, End, [f](const tuple<DXGI_FORMAT, GUID>& a)
                                           {
                                               return get<0>(a) == f;
                                           });
        const GUID guid = (it != End) ? get<1>(*it) : GUID_NULL;
        return guid;
    }

    bool _StoreScanline(LPVOID pDestination, size_t size, DXGI_FORMAT format, const XMVECTOR* pSource, size_t count);
    bool _LoadScanline(XMVECTOR* pDestination, size_t count, LPCVOID pSource, size_t size, DXGI_FORMAT format);

    //-------------------------------------------------------------------------------------
    // Convert DXGI image to/from GUID_WICPixelFormat128bppRGBAFloat (no range conversions)
    //-------------------------------------------------------------------------------------
    void _ConvertToR32G32B32A32(const Image& srcImage, ScratchImage& image)
    {
        image.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, srcImage.width, srcImage.height);

        const Image* img = image.GetImage();
        BYTE* pDest = img->pixels;
        const BYTE *pSrc = srcImage.pixels;
        for(size_t h = 0; h < srcImage.height; ++h)
        {
            if (!_LoadScanline(reinterpret_cast<XMVECTOR*>(pDest), srcImage.width, pSrc, srcImage.rowPitch, srcImage.format))
            {
                HrException::Throw(E_FAIL, _T("_LoadScanline failed."));
            }

            pSrc += srcImage.rowPitch;
            pDest += img->rowPitch;
        }
    }

    void _ConvertFromR32G32B32A32(const Image& srcImage, const Image& destImage)
    {
        const BYTE *pSrc = srcImage.pixels;
        BYTE* pDest = destImage.pixels;
        for(size_t h = 0; h < srcImage.height; ++h)
        {
            if (!_StoreScanline(pDest, destImage.rowPitch, destImage.format, reinterpret_cast<const XMVECTOR*>(pSrc), srcImage.width))
            {
                HrException::Throw(E_FAIL, _T("_StoreScanline failed."));
            }

            pSrc += srcImage.rowPitch;
            pDest += destImage.rowPitch;
        }
    }

    CHandle<IWICBitmap> CreateBitmapFromMemory(IWICImagingFactory& factory, const Image& i, const REFWICPixelFormatGUID guid)
    {
        CHandle<IWICBitmap> p;
        HrException::Throw(factory.CreateBitmapFromMemory(static_cast<unsigned>(i.width), static_cast<unsigned>(i.height), guid,
                                                          static_cast<unsigned>(i.rowPitch), static_cast<unsigned>(i.slicePitch),
                                                          i.pixels, &p), _T("IWICImagingFactory::CreateBitmapFromMemory failed."));
        return p;
    }

    CHandle<IWICBitmapFlipRotator> CreateBitmapFlipRotator(IWICImagingFactory& factory, IWICBitmap& bitmap, DWORD flags)
    {
        CHandle<IWICBitmapFlipRotator> p;
        HrException::Throw(factory.CreateBitmapFlipRotator(&p), _T("IWICImagingFactory::CreateBitmapFlipRotator failed."));
        HrException::Throw(p->Initialize(&bitmap, static_cast<WICBitmapTransformOptions>(flags)), _T("IWICBitmapFlipRotator::Initialize failed."));
        return p;
    }

    WICPixelFormatGUID GetPixelFormat(IWICBitmapFlipRotator& fr)
    {
        WICPixelFormatGUID w;
        HrException::Throw(fr.GetPixelFormat(&w), _T("IWICBitmapFlipRotator::GetPixelFormat failed."));
        return w;
    }

    Vector2D<unsigned> GetSize(IWICBitmapFlipRotator& fr)
    {
        unsigned x, y;
        HrException::Throw(fr.GetSize(&x, &y), _T("IWICBitmapFlipRotator::GetSize failed."));
        return Vector2D<unsigned>(x, y);
    }

    //-------------------------------------------------------------------------------------
    // Do flip/rotate operation using WIC
    //-------------------------------------------------------------------------------------
    static void _PerformFlipRotateUsingWIC(IWICImagingFactory& factory, _In_ const Image& srcImage, _In_ DWORD flags, _In_ const WICPixelFormatGUID& pfGUID, _In_ const Image& destImage)
    {
        CHandle<IWICBitmap> source = CreateBitmapFromMemory(factory, srcImage, pfGUID);
        CHandle<IWICBitmapFlipRotator> FR = CreateBitmapFlipRotator(factory, *source, flags);

        assert(GetPixelFormat(*FR) == pfGUID);

        const auto size = GetSize(*FR);

        assert(destImage.width == size.x());
        assert(destImage.height == size.y());

        HrException::Throw(FR->CopyPixels(0, static_cast<unsigned>(destImage.rowPitch), static_cast<unsigned>(destImage.slicePitch), destImage.pixels), _T("IWICBitmapFlipRotator::CopyPixels failed."));
    }

    //-------------------------------------------------------------------------------------
    // Do conversion, flip/rotate using WIC, conversion cycle
    //-------------------------------------------------------------------------------------
    static void _PerformFlipRotateViaF32(IWICImagingFactory& factory, _In_ const Image& srcImage, _In_ DWORD flags, _In_ const Image& destImage)
    {
        assert(srcImage.format == destImage.format);
        ScratchImage rtemp;
        {
            ScratchImage temp;
            _ConvertToR32G32B32A32(srcImage, temp);

            ScratchImage rtemp;
            rtemp.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, destImage.width, destImage.height);

            _PerformFlipRotateUsingWIC(factory, *temp.GetImage(), flags, GUID_WICPixelFormat128bppRGBAFloat, *rtemp.GetImage());
        }
        _ConvertFromR32G32B32A32(*rtemp.GetImage(), destImage);
    }

    //-------------------------------------------------------------------------------------
    // Flip/rotate image
    //-------------------------------------------------------------------------------------
    HRESULT FlipRotate(IWICImagingFactory& factory, const Image& srcImage, DWORD flags, ScratchImage& image)
    {
        const bool SwitchXY = flags == WICBitmapTransformRotate90 || flags == WICBitmapTransformRotate270;
        const size_t nwidth = SwitchXY ? srcImage.height : srcImage.width;
        const size_t nheight = SwitchXY ? srcImage.width : srcImage.height;

        try
        {
            image.Initialize2D(srcImage.format, nwidth, nheight);
            const auto guid = _DXGIToWIC(srcImage.format);
            if (guid != GUID_NULL)
            {
                // Case 1: Source format is supported by Windows Imaging Component
                _PerformFlipRotateUsingWIC(factory, srcImage, flags, guid, *image.GetImage());
            }
            else
            {
                // Case 2: Source format is not supported by WIC, so we have to convert, flip/rotate, and convert back
                _PerformFlipRotateViaF32(factory, srcImage, flags, *image.GetImage());
            }
        }
        catch (const HrException& e)
        {
            image.Release();
            e.log();
            return e.code();
        }

        return S_OK;
    }

    float round_to_nearest(float x)
    {
        // Round to nearest (even)
        float i = floorf(x);
        x -= i;
        if(x < 0.5f)
            return i;
        if(x > 0.5f)
            return i + 1.f;

        float int_part;
        modff( i / 2.f, &int_part );
        if ( (2.f*int_part) == i )
        {
            return i;
        }

        return i + 1.f;
    }

    //-------------------------------------------------------------------------------------
    // Stores an image row from standard RGBA XMVECTOR (aligned) array
    //-------------------------------------------------------------------------------------
    #define STORE_SCANLINE( type, func )\
        if ( size >= sizeof(type) )\
        {\
            type * __restrict dPtr = reinterpret_cast<type*>(pDestination);\
            for( size_t icount = 0; icount < size; icount += sizeof(type) )\
            {\
                if ( sPtr >= ePtr ) break;\
                func( dPtr++, *sPtr++ );\
            }\
        }\
        return true;

    bool _StoreScanline(LPVOID pDestination, size_t size, DXGI_FORMAT format, const XMVECTOR* pSource, size_t count)
    {
        assert( pDestination && size > 0 );
        assert( pSource && count > 0 && (((uintptr_t)pSource & 0xF) == 0) );

        const XMVECTOR* __restrict sPtr = pSource;
        if ( !sPtr )
            return false;

        const XMVECTOR* ePtr = pSource + count;

        switch( format )
        {
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            STORE_SCANLINE( XMFLOAT4, XMStoreFloat4 )

        case DXGI_FORMAT_R32G32B32A32_UINT:
            STORE_SCANLINE( XMUINT4, XMStoreUInt4 )

        case DXGI_FORMAT_R32G32B32A32_SINT:
            STORE_SCANLINE( XMINT4, XMStoreSInt4 )

        case DXGI_FORMAT_R32G32B32_FLOAT:
            STORE_SCANLINE( XMFLOAT3, XMStoreFloat3 )

        case DXGI_FORMAT_R32G32B32_UINT:
            STORE_SCANLINE( XMUINT3, XMStoreUInt3 )

        case DXGI_FORMAT_R32G32B32_SINT:
            STORE_SCANLINE( XMINT3, XMStoreSInt3 )

        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            STORE_SCANLINE( XMHALF4, XMStoreHalf4 )

        case DXGI_FORMAT_R16G16B16A16_UNORM:
            STORE_SCANLINE( XMUSHORTN4, XMStoreUShortN4 )

        case DXGI_FORMAT_R16G16B16A16_UINT:
            STORE_SCANLINE( XMUSHORT4, XMStoreUShort4 )

        case DXGI_FORMAT_R16G16B16A16_SNORM:
            STORE_SCANLINE( XMSHORTN4, XMStoreShortN4 )

        case DXGI_FORMAT_R16G16B16A16_SINT:
            STORE_SCANLINE( XMSHORT4, XMStoreShort4 )

        case DXGI_FORMAT_R32G32_FLOAT:
            STORE_SCANLINE( XMFLOAT2, XMStoreFloat2 )

        case DXGI_FORMAT_R32G32_UINT:
            STORE_SCANLINE( XMUINT2, XMStoreUInt2 )

        case DXGI_FORMAT_R32G32_SINT:
            STORE_SCANLINE( XMINT2, XMStoreSInt2 )

        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            if ( size >= (sizeof(float)+sizeof(unsigned)) )
            {
                float *dPtr = reinterpret_cast<float*>(pDestination);
                for( size_t icount = 0; icount < size; icount += (sizeof(float)+sizeof(unsigned)) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMFLOAT4 f;
                    XMStoreFloat4( &f, *sPtr++ );
                    dPtr[0] = f.x;
                    BYTE* ps8 = reinterpret_cast<BYTE*>( &dPtr[1] );
                    ps8[0] = static_cast<BYTE>( std::min<float>( 255.f, std::max<float>( 0.f, f.y ) ) );
                    ps8[1] = ps8[2] = ps8[3] = 0;
                    dPtr += 2;
                }
            }
            return true;

        case DXGI_FORMAT_R10G10B10A2_UNORM:
            STORE_SCANLINE( XMUDECN4, XMStoreUDecN4 );

        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
            if ( size >= sizeof(XMUDECN4) )
            {
                static const XMVECTORF32  Scale = { 510.0f, 510.0f, 510.0f, 3.0f };
                static const XMVECTORF32  Bias  = { 384.0f, 384.0f, 384.0f, 0.0f };
                static const XMVECTORF32  C     = { 1023.f, 1023.f, 1023.f, 3.f };

                XMUDECN4 * __restrict dPtr = reinterpret_cast<XMUDECN4*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUDECN4) )
                {
                    if ( sPtr >= ePtr ) break;

                    XMVECTOR N = XMVectorMultiply(*sPtr++, Scale);
                    N = XMVectorAdd( N, Bias );
                    N = XMVectorClamp( N, g_XMZero, C );

                    XMFLOAT4A tmp;
                    XMStoreFloat4A(&tmp, N );

                    dPtr->v = ((unsigned)tmp.w << 30)
                               | (((unsigned)tmp.z & 0x3FF) << 20)
                               | (((unsigned)tmp.y & 0x3FF) << 10)
                               | (((unsigned)tmp.x & 0x3FF));
                    ++dPtr;
                }
            }
            return true;

        case DXGI_FORMAT_R10G10B10A2_UINT:
            STORE_SCANLINE( XMUDEC4, XMStoreUDec4 );

        case DXGI_FORMAT_R11G11B10_FLOAT:
            STORE_SCANLINE( XMFLOAT3PK, XMStoreFloat3PK );

        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            STORE_SCANLINE( XMUBYTEN4, XMStoreUByteN4 )

        case DXGI_FORMAT_R8G8B8A8_UINT:
            STORE_SCANLINE( XMUBYTE4, XMStoreUByte4 )

        case DXGI_FORMAT_R8G8B8A8_SNORM:
            STORE_SCANLINE( XMBYTEN4, XMStoreByteN4 )

        case DXGI_FORMAT_R8G8B8A8_SINT:
            STORE_SCANLINE( XMBYTE4, XMStoreByte4 )

        case DXGI_FORMAT_R16G16_FLOAT:
            STORE_SCANLINE( XMHALF2, XMStoreHalf2 )

        case DXGI_FORMAT_R16G16_UNORM:
            STORE_SCANLINE( XMUSHORTN2, XMStoreUShortN2 )

        case DXGI_FORMAT_R16G16_UINT:
            STORE_SCANLINE( XMUSHORT2, XMStoreUShort2 )

        case DXGI_FORMAT_R16G16_SNORM:
            STORE_SCANLINE( XMSHORTN2, XMStoreShortN2 )

        case DXGI_FORMAT_R16G16_SINT:
            STORE_SCANLINE( XMSHORT2, XMStoreShort2 )

        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
            if ( size >= sizeof(float) )
            {
                float * __restrict dPtr = reinterpret_cast<float*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(float) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMStoreFloat( dPtr++, *(sPtr++) );
                }
            }
            return true;

        case DXGI_FORMAT_R32_UINT:
            if ( size >= sizeof(unsigned) )
            {
                unsigned * __restrict dPtr = reinterpret_cast<unsigned*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(unsigned) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMVECTOR v = XMConvertVectorFloatToUInt( *(sPtr++), 0 );
                    XMStoreInt( dPtr++, v );
                }
            }
            return true;

        case DXGI_FORMAT_R32_SINT:
            if ( size >= sizeof(unsigned) )
            {
                unsigned * __restrict dPtr = reinterpret_cast<unsigned*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(unsigned) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMVECTOR v = XMConvertVectorFloatToInt( *(sPtr++), 0 );
                    XMStoreInt( dPtr++, v );
                }
            }
            return true;

        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            if ( size >= sizeof(unsigned) )
            {
                static const XMVECTORF32 clamp = { 1.f, 255.f, 0.f, 0.f };
                XMVECTOR zero = XMVectorZero();
                unsigned *dPtr = reinterpret_cast<unsigned*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(unsigned) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMFLOAT4 f;
                    XMStoreFloat4( &f, XMVectorClamp( *sPtr++, zero, clamp ) );
                    *dPtr++ = (static_cast<unsigned>( f.x * 16777215.f ) & 0xFFFFFF)
                              | ((static_cast<unsigned>( f.y ) & 0xFF) << 24);
                }
            }
            return true;

        case DXGI_FORMAT_R8G8_UNORM:
            STORE_SCANLINE( XMUBYTEN2, XMStoreUByteN2 )

        case DXGI_FORMAT_R8G8_UINT:
            STORE_SCANLINE( XMUBYTE2, XMStoreUByte2 )

        case DXGI_FORMAT_R8G8_SNORM:
            STORE_SCANLINE( XMBYTEN2, XMStoreByteN2 )

        case DXGI_FORMAT_R8G8_SINT:
            STORE_SCANLINE( XMBYTE2, XMStoreByte2 )

        case DXGI_FORMAT_R16_FLOAT:
            if ( size >= sizeof(HALF) )
            {
                HALF * __restrict dPtr = reinterpret_cast<HALF*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(HALF) )
                {
                    if ( sPtr >= ePtr ) break;
                    float v = XMVectorGetX( *sPtr++ );
                    *(dPtr++) = XMConvertFloatToHalf(v);
                }
            }
            return true;

        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
            if ( size >= sizeof(int16_t) )
            {
                int16_t * __restrict dPtr = reinterpret_cast<int16_t*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(int16_t) )
                {
                    if ( sPtr >= ePtr ) break;
                    float v = XMVectorGetX( *sPtr++ );
                    v = std::max<float>( std::min<float>( v, 1.f ), 0.f );
                    *(dPtr++) = static_cast<uint16_t>( v*65535.f + 0.5f );
                }
            }
            return true;

        case DXGI_FORMAT_R16_UINT:
            if ( size >= sizeof(uint16_t) )
            {
                uint16_t * __restrict dPtr = reinterpret_cast<uint16_t*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(uint16_t) )
                {
                    if ( sPtr >= ePtr ) break;
                    float v = XMVectorGetX( *sPtr++ );
                    v = std::max<float>( std::min<float>( v, 65535.f ), 0.f );
                    *(dPtr++) = static_cast<uint16_t>(v);
                }
            }
            return true;

        case DXGI_FORMAT_R16_SNORM:
            if ( size >= sizeof(int16_t) )
            {
                int16_t * __restrict dPtr = reinterpret_cast<int16_t*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(int16_t) )
                {
                    if ( sPtr >= ePtr ) break;
                    float v = XMVectorGetX( *sPtr++ );
                    v = std::max<float>( std::min<float>( v, 1.f ), -1.f );
                    *(dPtr++) = static_cast<uint16_t>( v * 32767.f );
                }
            }
            return true;

        case DXGI_FORMAT_R16_SINT:
            if ( size >= sizeof(int16_t) )
            {
                int16_t * __restrict dPtr = reinterpret_cast<int16_t*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(int16_t) )
                {
                    if ( sPtr >= ePtr ) break;
                    float v = XMVectorGetX( *sPtr++ );
                    v = std::max<float>( std::min<float>( v, 32767.f ), -32767.f );
                    *(dPtr++) = static_cast<int16_t>(v);
                }
            }
            return true;

        case DXGI_FORMAT_R8_UNORM:
            if ( size >= sizeof(BYTE) )
            {
                BYTE * __restrict dPtr = reinterpret_cast<BYTE*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(BYTE) )
                {
                    if ( sPtr >= ePtr ) break;
                    float v = XMVectorGetX( *sPtr++ );
                    v = std::max<float>( std::min<float>( v, 1.f ), 0.f );
                    *(dPtr++) = static_cast<BYTE>( v * 255.f);
                }
            }
            return true;

        case DXGI_FORMAT_R8_UINT:
            if ( size >= sizeof(BYTE) )
            {
                BYTE * __restrict dPtr = reinterpret_cast<BYTE*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(BYTE) )
                {
                    if ( sPtr >= ePtr ) break;
                    float v = XMVectorGetX( *sPtr++ );
                    v = std::max<float>( std::min<float>( v, 255.f ), 0.f );
                    *(dPtr++) = static_cast<BYTE>(v);
                }
            }
            return true;

        case DXGI_FORMAT_R8_SNORM:
            if ( size >= sizeof(char) )
            {
                char * __restrict dPtr = reinterpret_cast<char*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(char) )
                {
                    if ( sPtr >= ePtr ) break;
                    float v = XMVectorGetX( *sPtr++ );
                    v = std::max<float>( std::min<float>( v, 1.f ), -1.f );
                    *(dPtr++) = static_cast<char>( v * 127.f );
                }
            }
            return true;

        case DXGI_FORMAT_R8_SINT:
            if ( size >= sizeof(char) )
            {
                char * __restrict dPtr = reinterpret_cast<char*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(char) )
                {
                    if ( sPtr >= ePtr ) break;
                    float v = XMVectorGetX( *sPtr++ );
                    v = std::max<float>( std::min<float>( v, 127.f ), -127.f );
                    *(dPtr++) = static_cast<char>( v );
                }
            }
            return true;

        case DXGI_FORMAT_A8_UNORM:
            if ( size >= sizeof(BYTE) )
            {
                BYTE * __restrict dPtr = reinterpret_cast<BYTE*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(BYTE) )
                {
                    if ( sPtr >= ePtr ) break;
                    float v = XMVectorGetW( *sPtr++ );
                    v = std::max<float>( std::min<float>( v, 1.f ), 0.f );
                    *(dPtr++) = static_cast<BYTE>( v * 255.f);
                }
            }
            return true;

        case DXGI_FORMAT_R1_UNORM:
            if ( size >= sizeof(BYTE) )
            {
                BYTE * __restrict dPtr = reinterpret_cast<BYTE*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(BYTE) )
                {
                    BYTE pixels = 0;
                    for( size_t bcount = 8; bcount > 0; --bcount )
                    {
                        if ( sPtr >= ePtr ) break;
                        float v = XMVectorGetX( *sPtr++ );

                        // Absolute thresholding generally doesn't give good results for all images
                        // Picking the 'right' threshold automatically requires whole-image analysis

                        if ( v > 0.25f )
                            pixels |= 1 << (bcount-1);
                    }
                    *(dPtr++) = pixels;
                }
            }
            return true;

        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
            if ( size >= sizeof(XMFLOAT3SE) )
            {
                static const float maxf9 = float(0x1FF << 7);
                static const float minf9 = float(1.f / (1 << 16));

                XMFLOAT3SE * __restrict dPtr = reinterpret_cast<XMFLOAT3SE*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(XMFLOAT3SE) )
                {
                    if ( sPtr >= ePtr ) break;

                    XMFLOAT3 rgb;
                    XMStoreFloat3( &rgb, *(sPtr++) );

                    float r = (rgb.x >= 0.f) ? ( (rgb.x > maxf9) ? maxf9 : rgb.x ) : 0.f;
                    float g = (rgb.y >= 0.f) ? ( (rgb.y > maxf9) ? maxf9 : rgb.y ) : 0.f;
                    float b = (rgb.z >= 0.f) ? ( (rgb.z > maxf9) ? maxf9 : rgb.z ) : 0.f;

                    const float max_rg = (r > g) ? r : g;
                    const float max_rgb = (max_rg > b) ? max_rg : b;

                    const float maxColor = (max_rgb > minf9) ? max_rgb : minf9;

                    union { float f; INT32 i; } fi;
                    fi.f = maxColor;
                    fi.i &= 0xFF800000; // cut off fraction

                    dPtr->e = (fi.i - 0x37800000) >> 23;

                    fi.i = 0x83000000 - fi.i;
                    float ScaleR = fi.f;

                    dPtr->xm = static_cast<unsigned>( round_to_nearest(r * ScaleR) );
                    dPtr->ym = static_cast<unsigned>( round_to_nearest(g * ScaleR) );
                    dPtr->zm = static_cast<unsigned>( round_to_nearest(b * ScaleR) );
                    ++dPtr;
                }
            }
            return true;

        case DXGI_FORMAT_R8G8_B8G8_UNORM:
            if ( size >= sizeof(XMUBYTEN4) )
            {
                XMUBYTEN4 * __restrict dPtr = reinterpret_cast<XMUBYTEN4*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMVECTOR v0 = *sPtr++;
                    XMVECTOR v1 = (sPtr < ePtr) ? XMVectorSplatY( *sPtr++ ) : XMVectorZero();
                    XMVECTOR v = XMVectorSelect( v1, v0, g_XMSelect1110 );
                    XMStoreUByteN4( dPtr++, v );
                }
            }
            return true;

        case DXGI_FORMAT_G8R8_G8B8_UNORM:
            if ( size >= sizeof(XMUBYTEN4) )
            {
                static XMVECTORI32 select1101 = {XM_SELECT_1, XM_SELECT_1, XM_SELECT_0, XM_SELECT_1};

                XMUBYTEN4 * __restrict dPtr = reinterpret_cast<XMUBYTEN4*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMVECTOR v0 = XMVectorSwizzle<1, 0, 3, 2>( *sPtr++ );
                    XMVECTOR v1 = (sPtr < ePtr) ? XMVectorSplatY( *sPtr++ ) : XMVectorZero();
                    XMVECTOR v = XMVectorSelect( v1, v0, select1101 );
                    XMStoreUByteN4( dPtr++, v );
                }
            }
            return true;

        case DXGI_FORMAT_B5G6R5_UNORM:
            if ( size >= sizeof(XMU565) )
            {
                static XMVECTORF32 s_Scale = { 31.f, 63.f, 31.f, 1.f };
                XMU565 * __restrict dPtr = reinterpret_cast<XMU565*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(XMU565) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>( *sPtr++ );
                    v = XMVectorMultiply( v, s_Scale );
                    XMStoreU565( dPtr++, v );
                }
            }
            return true;

        case DXGI_FORMAT_B5G5R5A1_UNORM:
            if ( size >= sizeof(XMU555) )
            {
                static XMVECTORF32 s_Scale = { 31.f, 31.f, 31.f, 1.f };
                XMU555 * __restrict dPtr = reinterpret_cast<XMU555*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(XMU555) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>( *sPtr++ );
                    v = XMVectorMultiply( v, s_Scale );
                    XMStoreU555( dPtr++, v );
                }
            }
            return true;

        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            if ( size >= sizeof(XMUBYTEN4) )
            {
                XMUBYTEN4 * __restrict dPtr = reinterpret_cast<XMUBYTEN4*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>( *sPtr++ );
                    XMStoreUByteN4( dPtr++, v );
                }
            }
            return true;

        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            if ( size >= sizeof(XMUBYTEN4) )
            {
                XMUBYTEN4 * __restrict dPtr = reinterpret_cast<XMUBYTEN4*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMVECTOR v = XMVectorPermute<2, 1, 0, 7>( *sPtr++, g_XMIdentityR3 );
                    XMStoreUByteN4( dPtr++, v );
                }
            }
            return true;

        case DXGI_FORMAT_B4G4R4A4_UNORM:
            if ( size >= sizeof(XMUNIBBLE4) )
            {
                static XMVECTORF32 s_Scale = { 15.f, 15.f, 15.f, 15.f };
                XMUNIBBLE4 * __restrict dPtr = reinterpret_cast<XMUNIBBLE4*>(pDestination);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUNIBBLE4) )
                {
                    if ( sPtr >= ePtr ) break;
                    XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>( *sPtr++ );
                    v = XMVectorMultiply( v, s_Scale );
                    XMStoreUNibble4( dPtr++, v );
                }
            }
            return true;

        default:
            return false;
        }
    }

    #define LOAD_SCANLINE( type, func )\
            if ( size >= sizeof(type) )\
            {\
                const type * __restrict sPtr = reinterpret_cast<const type*>(pSource);\
                for( size_t icount = 0; icount < size; icount += sizeof(type) )\
                {\
                    if ( dPtr >= ePtr ) break;\
                    *(dPtr++) = func( sPtr++ );\
                }\
                return true;\
            }\
            return false;

    #define LOAD_SCANLINE3( type, func, defvec )\
            if ( size >= sizeof(type) )\
            {\
                const type * __restrict sPtr = reinterpret_cast<const type*>(pSource);\
                for( size_t icount = 0; icount < size; icount += sizeof(type) )\
                {\
                    XMVECTOR v = func( sPtr++ );\
                    if ( dPtr >= ePtr ) break;\
                    *(dPtr++) = XMVectorSelect( defvec, v, g_XMSelect1110 );\
                }\
                return true;\
            }\
            return false;

    #define LOAD_SCANLINE2( type, func, defvec )\
            if ( size >= sizeof(type) )\
            {\
                const type * __restrict sPtr = reinterpret_cast<const type*>(pSource);\
                for( size_t icount = 0; icount < size; icount += sizeof(type) )\
                {\
                    XMVECTOR v = func( sPtr++ );\
                    if ( dPtr >= ePtr ) break;\
                    *(dPtr++) = XMVectorSelect( defvec, v, g_XMSelect1100 );\
                }\
                return true;\
            }\
            return false;

    bool _LoadScanline( XMVECTOR* pDestination, size_t count, LPCVOID pSource, size_t size, DXGI_FORMAT format )
    {
        assert( pDestination && count > 0 && (((uintptr_t)pDestination & 0xF) == 0) );
        assert( pSource && size > 0 );

        XMVECTOR* __restrict dPtr = pDestination;
        if ( !dPtr )
            return false;

        const XMVECTOR* ePtr = pDestination + count;

        switch( format )
        {
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            {
                size_t msize = (size > (sizeof(XMVECTOR)*count)) ? (sizeof(XMVECTOR)*count) : size;
                memcpy_s( dPtr, sizeof(XMVECTOR)*count, pSource, msize );
            }
            return true;

        case DXGI_FORMAT_R32G32B32A32_UINT:
            LOAD_SCANLINE( XMUINT4, XMLoadUInt4 )

        case DXGI_FORMAT_R32G32B32A32_SINT:
            LOAD_SCANLINE( XMINT4, XMLoadSInt4 )

        case DXGI_FORMAT_R32G32B32_FLOAT:
            LOAD_SCANLINE3( XMFLOAT3, XMLoadFloat3, g_XMIdentityR3 )

        case DXGI_FORMAT_R32G32B32_UINT:
            LOAD_SCANLINE3( XMUINT3, XMLoadUInt3, g_XMIdentityR3 )

        case DXGI_FORMAT_R32G32B32_SINT:
            LOAD_SCANLINE3( XMINT3, XMLoadSInt3, g_XMIdentityR3 )

        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            LOAD_SCANLINE( XMHALF4, XMLoadHalf4 )

        case DXGI_FORMAT_R16G16B16A16_UNORM:
            LOAD_SCANLINE( XMUSHORTN4, XMLoadUShortN4 )

        case DXGI_FORMAT_R16G16B16A16_UINT:
            LOAD_SCANLINE( XMUSHORT4, XMLoadUShort4 )

        case DXGI_FORMAT_R16G16B16A16_SNORM:
            LOAD_SCANLINE( XMSHORTN4, XMLoadShortN4 )

        case DXGI_FORMAT_R16G16B16A16_SINT:
            LOAD_SCANLINE( XMSHORT4, XMLoadShort4 )

        case DXGI_FORMAT_R32G32_FLOAT:
            LOAD_SCANLINE2( XMFLOAT2, XMLoadFloat2, g_XMIdentityR3 )

        case DXGI_FORMAT_R32G32_UINT:
            LOAD_SCANLINE2( XMUINT2, XMLoadUInt2, g_XMIdentityR3 )

        case DXGI_FORMAT_R32G32_SINT:
            LOAD_SCANLINE2( XMINT2, XMLoadSInt2, g_XMIdentityR3 )

        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            if ( size >= (sizeof(float)+sizeof(unsigned)) )
            {
                const float * sPtr = reinterpret_cast<const float*>(pSource);
                for( size_t icount = 0; icount < size; icount += (sizeof(float)+sizeof(unsigned)) )
                {
                    const BYTE* ps8 = reinterpret_cast<const BYTE*>( &sPtr[1] );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( sPtr[0], static_cast<float>( *ps8 ), 0.f, 1.f );
                    sPtr += 2;
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            if ( size >= (sizeof(float)+sizeof(unsigned)) )
            {
                const float * sPtr = reinterpret_cast<const float*>(pSource);
                for( size_t icount = 0; icount < size; icount += (sizeof(float)+sizeof(unsigned)) )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( sPtr[0], 0.f /* typeless component assumed zero */, 0.f, 1.f );
                    sPtr += 2;
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            if ( size >= (sizeof(float)+sizeof(unsigned)) )
            {
                const float * sPtr = reinterpret_cast<const float*>(pSource);
                for( size_t icount = 0; icount < size; icount += (sizeof(float)+sizeof(unsigned)) )
                {
                    const BYTE* pg8 = reinterpret_cast<const BYTE*>( &sPtr[1] );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( 0.f /* typeless component assumed zero */, static_cast<float>( *pg8 ), 0.f, 1.f );
                    sPtr += 2;
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R10G10B10A2_UNORM:
            LOAD_SCANLINE( XMUDECN4, XMLoadUDecN4 );

        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
            if ( size >= sizeof(XMUDECN4) )
            {
                const XMUDECN4 * __restrict sPtr = reinterpret_cast<const XMUDECN4*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUDECN4) )
                {
                    if ( dPtr >= ePtr ) break;

                    int32_t ElementX = sPtr->v & 0x3FF;
                    int32_t ElementY = (sPtr->v >> 10) & 0x3FF;
                    int32_t ElementZ = (sPtr->v >> 20) & 0x3FF;

                    XMVECTORF32 vResult = {
                        (float)(ElementX - 0x180) / 510.0f,
                        (float)(ElementY - 0x180) / 510.0f,
                        (float)(ElementZ - 0x180) / 510.0f,
                        (float)(sPtr->v >> 30) / 3.0f
                    };

                    ++sPtr;

                    *(dPtr++) = vResult.v;
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R10G10B10A2_UINT:
            LOAD_SCANLINE( XMUDEC4, XMLoadUDec4 );

        case DXGI_FORMAT_R11G11B10_FLOAT:
            LOAD_SCANLINE3( XMFLOAT3PK, XMLoadFloat3PK, g_XMIdentityR3 );

        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            LOAD_SCANLINE( XMUBYTEN4, XMLoadUByteN4 )

        case DXGI_FORMAT_R8G8B8A8_UINT:
            LOAD_SCANLINE( XMUBYTE4, XMLoadUByte4 )

        case DXGI_FORMAT_R8G8B8A8_SNORM:
            LOAD_SCANLINE( XMBYTEN4, XMLoadByteN4 )

        case DXGI_FORMAT_R8G8B8A8_SINT:
            LOAD_SCANLINE( XMBYTE4, XMLoadByte4 )

        case DXGI_FORMAT_R16G16_FLOAT:
            LOAD_SCANLINE2( XMHALF2, XMLoadHalf2, g_XMIdentityR3 )

        case DXGI_FORMAT_R16G16_UNORM:
            LOAD_SCANLINE2( XMUSHORTN2, XMLoadUShortN2, g_XMIdentityR3 )

        case DXGI_FORMAT_R16G16_UINT:
            LOAD_SCANLINE2( XMUSHORT2, XMLoadUShort2, g_XMIdentityR3 )

        case DXGI_FORMAT_R16G16_SNORM:
            LOAD_SCANLINE2( XMSHORTN2, XMLoadShortN2, g_XMIdentityR3 )

        case DXGI_FORMAT_R16G16_SINT:
            LOAD_SCANLINE2( XMSHORT2, XMLoadShort2, g_XMIdentityR3 )

        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
            if ( size >= sizeof(float) )
            {
                const float* __restrict sPtr = reinterpret_cast<const float*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(float) )
                {
                    XMVECTOR v = XMLoadFloat( sPtr++ );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1000 );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R32_UINT:
            if ( size >= sizeof(unsigned) )
            {
                const unsigned* __restrict sPtr = reinterpret_cast<const unsigned*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(unsigned) )
                {
                    XMVECTOR v = XMLoadInt( sPtr++ );
                    v = XMConvertVectorUIntToFloat( v, 0 );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1000 );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R32_SINT:
            if ( size >= sizeof(int32_t) )
            {
                const int32_t * __restrict sPtr = reinterpret_cast<const int32_t*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(int32_t) )
                {
                    XMVECTOR v = XMLoadInt( reinterpret_cast<const unsigned*> (sPtr++) );
                    v = XMConvertVectorIntToFloat( v, 0 );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1000 );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            if ( size >= sizeof(unsigned) )
            {
                const unsigned * sPtr = reinterpret_cast<const unsigned*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(unsigned) )
                {
                    float d = static_cast<float>( *sPtr & 0xFFFFFF ) / 16777215.f;
                    float s = static_cast<float>( ( *sPtr & 0xFF000000 ) >> 24 );
                    ++sPtr;
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( d, s, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            if ( size >= sizeof(unsigned) )
            {
                const unsigned * sPtr = reinterpret_cast<const unsigned*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(unsigned) )
                {
                    float r = static_cast<float>( *sPtr & 0xFFFFFF ) / 16777215.f;
                    ++sPtr;
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( r, 0.f /* typeless component assumed zero */, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            if ( size >= sizeof(unsigned) )
            {
                const unsigned * sPtr = reinterpret_cast<const unsigned*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(unsigned) )
                {
                    float g = static_cast<float>( ( *sPtr & 0xFF000000 ) >> 24 );
                    ++sPtr;
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( 0.f /* typeless component assumed zero */, g, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R8G8_UNORM:
            LOAD_SCANLINE2( XMUBYTEN2, XMLoadUByteN2, g_XMIdentityR3 )

        case DXGI_FORMAT_R8G8_UINT:
            LOAD_SCANLINE2( XMUBYTE2, XMLoadUByte2, g_XMIdentityR3 )

        case DXGI_FORMAT_R8G8_SNORM:
            LOAD_SCANLINE2( XMBYTEN2, XMLoadByteN2, g_XMIdentityR3 )

        case DXGI_FORMAT_R8G8_SINT:
            LOAD_SCANLINE2( XMBYTE2, XMLoadByte2, g_XMIdentityR3 )

        case DXGI_FORMAT_R16_FLOAT:
            if ( size >= sizeof(HALF) )
            {
                const HALF * __restrict sPtr = reinterpret_cast<const HALF*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(HALF) )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( XMConvertHalfToFloat(*sPtr++), 0.f, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
            if ( size >= sizeof(uint16_t) )
            {
                const uint16_t* __restrict sPtr = reinterpret_cast<const uint16_t*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(uint16_t) )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++) / 65535.f, 0.f, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R16_UINT:
            if ( size >= sizeof(uint16_t) )
            {
                const uint16_t * __restrict sPtr = reinterpret_cast<const uint16_t*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(uint16_t) )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++), 0.f, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R16_SNORM:
            if ( size >= sizeof(int16_t) )
            {
                const int16_t * __restrict sPtr = reinterpret_cast<const int16_t*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(int16_t) )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++) / 32767.f, 0.f, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R16_SINT:
            if ( size >= sizeof(int16_t) )
            {
                const int16_t * __restrict sPtr = reinterpret_cast<const int16_t*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(int16_t) )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++), 0.f, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R8_UNORM:
            if ( size >= sizeof(BYTE) )
            {
                const BYTE * __restrict sPtr = reinterpret_cast<const BYTE*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(BYTE) )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++) / 255.f, 0.f, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R8_UINT:
            if ( size >= sizeof(BYTE) )
            {
                const BYTE * __restrict sPtr = reinterpret_cast<const BYTE*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(BYTE) )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++), 0.f, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R8_SNORM:
            if ( size >= sizeof(char) )
            {
                const char * __restrict sPtr = reinterpret_cast<const char*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(char) )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++) / 127.f, 0.f, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R8_SINT:
            if ( size >= sizeof(char) )
            {
                const char * __restrict sPtr = reinterpret_cast<const char*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(char) )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++), 0.f, 0.f, 1.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_A8_UNORM:
            if ( size >= sizeof(BYTE) )
            {
                const BYTE * __restrict sPtr = reinterpret_cast<const BYTE*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(BYTE) )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( 0.f, 0.f, 0.f, static_cast<float>(*sPtr++) / 255.f );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R1_UNORM:
            if ( size >= sizeof(BYTE) )
            {
                const BYTE * __restrict sPtr = reinterpret_cast<const BYTE*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(BYTE) )
                {
                    for( size_t bcount = 8; bcount > 0; --bcount )
                    {
                        if ( dPtr >= ePtr ) break;
                        *(dPtr++) = XMVectorSet( (((*sPtr >> (bcount-1)) & 0x1) ? 1.f : 0.f), 0.f, 0.f, 1.f );
                    }

                    ++sPtr;
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
            if ( size >= sizeof(XMFLOAT3SE) )
            {
                const XMFLOAT3SE * __restrict sPtr = reinterpret_cast<const XMFLOAT3SE*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(XMFLOAT3SE) )
                {
                    union { float f; int32_t i; } fi;
                    fi.i = 0x33800000 + (sPtr->e << 23);
                    float Scale = fi.f;

                    XMVECTORF32 v = {
                        Scale * float( sPtr->xm ),
                        Scale * float( sPtr->ym ),
                        Scale * float( sPtr->zm ),
                        1.0f };

                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = v;
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_R8G8_B8G8_UNORM:
            if ( size >= sizeof(XMUBYTEN4) )
            {
                const XMUBYTEN4 * __restrict sPtr = reinterpret_cast<const XMUBYTEN4*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
                {
                    XMVECTOR v = XMLoadUByteN4( sPtr++ );
                    XMVECTOR v1 = XMVectorSwizzle<0, 3, 2, 1>( v );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1110 );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v1, g_XMSelect1110 );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_G8R8_G8B8_UNORM:
            if ( size >= sizeof(XMUBYTEN4) )
            {
                const XMUBYTEN4 * __restrict sPtr = reinterpret_cast<const XMUBYTEN4*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
                {
                    XMVECTOR v = XMLoadUByteN4( sPtr++ );
                    XMVECTOR v0 = XMVectorSwizzle<1, 0, 3, 2>( v );
                    XMVECTOR v1 = XMVectorSwizzle<1, 2, 3, 0>( v );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v0, g_XMSelect1110 );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v1, g_XMSelect1110 );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_B5G6R5_UNORM:
            if ( size >= sizeof(XMU565) )
            {
                static XMVECTORF32 s_Scale = { 1.f/31.f, 1.f/63.f, 1.f/31.f, 1.f };
                const XMU565 * __restrict sPtr = reinterpret_cast<const XMU565*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(XMU565) )
                {
                    XMVECTOR v = XMLoadU565( sPtr++ );
                    v = XMVectorMultiply( v, s_Scale );
                    v = XMVectorSwizzle<2, 1, 0, 3>( v );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1110 );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_B5G5R5A1_UNORM:
            if ( size >= sizeof(XMU555) )
            {
                static XMVECTORF32 s_Scale = { 1.f/31.f, 1.f/31.f, 1.f/31.f, 1.f };
                const XMU555 * __restrict sPtr = reinterpret_cast<const XMU555*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(XMU555) )
                {
                    XMVECTOR v = XMLoadU555( sPtr++ );
                    v = XMVectorMultiply( v, s_Scale );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSwizzle<2, 1, 0, 3>( v );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            if ( size >= sizeof(XMUBYTEN4) )
            {
                const XMUBYTEN4 * __restrict sPtr = reinterpret_cast<const XMUBYTEN4*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
                {
                    XMVECTOR v = XMLoadUByteN4( sPtr++ );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSwizzle<2, 1, 0, 3>( v );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            if ( size >= sizeof(XMUBYTEN4) )
            {
                const XMUBYTEN4 * __restrict sPtr = reinterpret_cast<const XMUBYTEN4*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
                {
                    XMVECTOR v = XMLoadUByteN4( sPtr++ );
                    v = XMVectorSwizzle<2, 1, 0, 3>( v );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1110 );
                }
                return true;
            }
            return false;

        case DXGI_FORMAT_B4G4R4A4_UNORM:
            if ( size >= sizeof(XMUNIBBLE4) )
            {
                static XMVECTORF32 s_Scale = { 1.f/15.f, 1.f/15.f, 1.f/15.f, 1.f/15.f };
                const XMUNIBBLE4 * __restrict sPtr = reinterpret_cast<const XMUNIBBLE4*>(pSource);
                for( size_t icount = 0; icount < size; icount += sizeof(XMUNIBBLE4) )
                {
                    XMVECTOR v = XMLoadUNibble4( sPtr++ );
                    v = XMVectorMultiply( v, s_Scale );
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSwizzle<2, 1, 0, 3>( v );
                }
                return true;
            }
            return false;

        default:
            return false;
        }
    }

    void ComputePitch( DXGI_FORMAT fmt, size_t width, size_t height, size_t& rowPitch, size_t& slicePitch )
    {
            const size_t bpp = CD3D10FormatHelper_Test::GetBitsPerElement(fmt);
            rowPitch = ( width * bpp + 7 ) / 8;
            slicePitch = rowPitch * height;
    }

    void _SetupImageArray(BYTE *pMemory, size_t, const TexMetadata& metadata, Image* images, size_t)
    {
        size_t rowPitch, slicePitch;
        ComputePitch( metadata.format, metadata.width, metadata.height, rowPitch, slicePitch );

        images[0].width = metadata.width;
        images[0].height = metadata.height;
        images[0].format = metadata.format;
        images[0].rowPitch = rowPitch;
        images[0].slicePitch = slicePitch;
        images[0].pixels = pMemory;
    }

    void _DetermineImageArray( const TexMetadata& metadata, size_t& nImages, size_t& pixelSize )
    {
        size_t rowPitch, slicePitch;
        ComputePitch(metadata.format, metadata.width, metadata.height, rowPitch, slicePitch);

        nImages = 1;
        pixelSize = slicePitch;
    }

    void ScratchImage::Release()
    {
        _nimages = 0;
        _size = 0;
        _image.release();
        _memory.release();
        memset(&_metadata, 0, sizeof(_metadata));
    }

    template<typename T> T* alignedmalloc(size_t size, size_t align)
    {
        void* p = _aligned_malloc(size, align);
        if (!p)
        {
            throw bad_alloc();
        }
        return reinterpret_cast<T*>(p);
    }

    void ScratchImage::Initialize2D(DXGI_FORMAT fmt, size_t width, size_t height)
    {
        Release();

        const TexMetadata metadata = {width, height, fmt};

        size_t pixelSize, nimages;
        _DetermineImageArray(metadata, nimages, pixelSize);

        try
        {
            unique_ptr<Image[]> image(new Image[nimages]);
            aligned_ptr<BYTE> pixelmem(alignedmalloc<BYTE>(pixelSize, 16));

            // No more failures.
            _image.reset(image.release());
            _memory.reset(pixelmem.release());
        }
        catch (const bad_alloc&)
        {
            HrException::Throw(E_OUTOFMEMORY, _T("System memory allocation failed."));
        }

        _metadata = metadata;
        _nimages = nimages;
        _size = pixelSize;
        memset(_image.get(), 0, sizeof(Image) * nimages);
        _SetupImageArray(_memory.get(), pixelSize, metadata, _image.get(), nimages);
    }

    const Image* ScratchImage::GetImage() const
    {
        return &_image[0];
    }
}

#pragma warning(pop)