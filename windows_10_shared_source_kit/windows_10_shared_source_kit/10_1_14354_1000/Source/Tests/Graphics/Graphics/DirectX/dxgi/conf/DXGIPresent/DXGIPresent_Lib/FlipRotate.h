#ifndef _FLIP_ROTATE_H_
#define _FLIP_ROTATE_H_

#include <algorithm>
#include <d3d11_2.h>
#include <memory>

namespace DirectX
{
    void ComputePitch( _In_ DXGI_FORMAT fmt, _In_ size_t width, _In_ size_t height, _Out_ size_t& rowPitch, _Out_ size_t& slicePitch );

    struct TexMetadata
    {
        size_t          width;
        size_t          height;
        DXGI_FORMAT     format;
    };

    //---------------------------------------------------------------------------------
    // Bitmap image container
    struct Image
    {
        size_t      width;
        size_t      height;
        DXGI_FORMAT format;
        size_t      rowPitch;
        size_t      slicePitch;
        BYTE*       pixels;
    };

    template<typename T> class aligned_ptr
    {
        public:
            aligned_ptr() : p(nullptr){}
            aligned_ptr(T* ptr) : p(ptr){}
            T* get() const { return p; }
            void reset(T* ptr){ if (p) _aligned_free(p); p = ptr; }
            T* release() { T* ptr = p; p = nullptr; return ptr; }
            ~aligned_ptr()
            {
                if (p) _aligned_free(p);
            }
        private:
            T* p;
    };

    class ScratchImage
    {
    public:
        ScratchImage() : _nimages(0), _size(0){}
        ~ScratchImage() { Release(); }

        void Initialize2D(_In_ DXGI_FORMAT fmt, _In_ size_t width, _In_ size_t height);

        void Release();

        size_t RowPitch() const { return GetImage()->rowPitch; }

        const TexMetadata& GetMetadata() const { return _metadata; }
        const Image* GetImage() const;

        BYTE* GetPixels() const { return _memory.get(); }

    private:
        size_t      _nimages;
        size_t      _size;
        TexMetadata _metadata;
        std::unique_ptr<Image[]> _image;
        aligned_ptr<BYTE> _memory;

        // Hide copy constructor and assignment operator
        ScratchImage(const ScratchImage&);
        ScratchImage& operator=(const ScratchImage&);
    };

    HRESULT FlipRotate(IWICImagingFactory&, const Image&, DWORD, ScratchImage&);
}

#endif