
#ifndef __WGFHELPERS_H_
#define __WGFHELPERS_H_

//=============================================================================
// WGFHelpers.h
//
// This is a short include file that contains a global version of WriteToLog
// which can be used in any WGFFramework test by functions and classes that
// do not inherit directly from CTest.  It also includes some helpful test
// macros (some of which are also defined in other helper files but with
// alternate names)
//
// Author: mahaigh
//
//=============================================================================

//TODO: Replace dxerr9 with dxerr10 as soon as it contains a DXGetErrorString function
#include "dxerr9.h"

// Need to include this for the 'GetFramework()' function
// Including this can lead to missmatched heap calls because it intrusively imports its own memory manager.
// Be sure to include this in all source files for a project to avoid the issue.
#include "WGFTestCore.h"

//=============================================================================
// Define WriteToLog the same way that CTest does it so that we can write to
// the log from any function.
//=============================================================================

inline void WriteToLog(const TCHAR *szFormat, ...)
{
	va_list arglist;
	va_start( arglist, szFormat );
	GetFramework()->WriteToLog( 1, FormatStringVA( szFormat, arglist ) );
	va_end( arglist );
}

//=============================================================================

const char g_NullAsString[] = "NULL";

//=============================================================================

#ifndef HR_TO_STRING
#define HR_TO_STRING(hr) \
	DXGetErrorString9(hr)
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(ptr) \
	if(ptr) { ptr->Release(); ptr = NULL; }
#endif

#ifndef SAFE_RELEASE_PPIUNKNOWN
#define SAFE_RELEASE_PPIUNKNOWN( ppIUnknown ) \
    { \
        IUnknown*  pIUnk; \
        if ( ((PDWORD_PTR*)ppIUnknown) ) \
        { \
            pIUnk = *(IUnknown**)( ((PDWORD_PTR*)ppIUnknown) ); \
	        if ( pIUnk ) \
            { \
                pIUnk->Release(); \
                pIUnk = NULL; \
            } \
        } \
    }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) \
	if(ptr) { delete ptr; ptr = NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(ptr) \
	if(ptr) { delete[] ptr; ptr = NULL; }
#endif

#define VBDONE(expr) if (!(expr)) { WriteToLog("BOOL Failure, line[%i], file[%s]\n", __LINE__, __FILE__); goto Done; }
#define VHRDONE(expr) hr = (expr); if (FAILED(hr)) { WriteToLog("HR Failure, hr[%s], line[%i], file[%s]\n", HR_TO_STRING(hr), __LINE__, __FILE__); goto Done; }

//=============================================================================

#include <tuple>
#include "DXGLogger.h"
#include <dxgi1_3.h>

namespace HelperType
{
    template<typename T> struct Vector2D : public std::tuple<T, T>
    {
        typedef std::tuple<T, T> Underlying_t;
        Vector2D(){}
        Vector2D(T x, T y) : tuple(x, y){}
        Vector2D(const Vector2D& v) { *static_cast<Underlying_t*>(this) = v; }
        Vector2D(const std::tuple<T, T>& v) : tuple(v){}
        Vector2D& operator=(const Vector2D& v) { *static_cast<Underlying_t*>(this) = *static_cast<const Underlying_t*>(&v); return *this; }
        Vector2D& operator=(const std::tuple<T, T>& v){ *static_cast<Underlying_t*>(this) = v; return *this; }
        T x() const { return std::get<0>(*this); }
        T y() const { return std::get<1>(*this); }
        static Vector2D x(T t){ return Vector2D(x, static_cast<T>(0)); }
        static Vector2D y(T t){ return Vector2D(static_cast<T>(0), y); }
        static tstring ToString(const Vector2D<T>& t)
        {
            std::basic_stringstream<TCHAR> s;
            s << "(" << std::get<0>(t) << ", " << std::get<1>(t) << ")";
            return s.str();
        }
        static bool FromString(Vector2D<T>* t, const tstring& str)
        {
            T n[2] = {};
            std::basic_istringstream<TCHAR> s(str);
            TCHAR BurnChar;
            s >> BurnChar >> n[0] >> BurnChar >> n[1];
            *t = (decltype(*t))(n[0], n[1]);
            return true;
        }
        bool signbit() const { return Helper::signbit(*this); }
        Vector2D<T> inverse() const { return Helper::inverse(*this); }
        Vector2D<T> abs() const { return Helper::abs(*this); }
        Vector2D<T> swap(DXGI_MODE_ROTATION r) const { return swap(Helper::OddRotation(r)); }
        Vector2D<T> swap(bool b = true) const { if (b) return Helper::swap(*this); return *this; }
    };

    class HrException
    {
        public:
            HrException(HRESULT h, const TCHAR* const st) : hr(h), s(st) {}
            HrException(const HrException& e) : hr(e.code()), s(e.what()) {}
            HRESULT code() const { return hr; }
            const TCHAR* what() const { return s; }
            void HrException::log() const
            {
                CTest::WriteToLog(_T("%s. Error: 0x%x."), what(), code());
            }
            static void Throw(HRESULT hr, const TCHAR* const s)
            {
                if (FAILED(hr)) throw HrException(hr, s);
            }

        private:
            const HRESULT hr;
            const TCHAR* const s;

            // No assignment - all members are const.
            HrException& operator=(const HrException&);
    };
}

inline HelperType::Vector2D<int> operator+(const HelperType::Vector2D<unsigned>& a, const HelperType::Vector2D<int>& b)
{
    return HelperType::Vector2D<int>(static_cast<int>(a.x()) + b.x(), static_cast<int>(a.y()) + b.y());
}

#define StringFunc(x) inline tstring ToString(const HelperType::Vector2D< x >& t){ return HelperType::Vector2D< x >::ToString(t); } \
                      inline bool FromString(HelperType::Vector2D< x >* t, const tstring& str){ return HelperType::Vector2D< x >::FromString(t, str); }

StringFunc(float);
StringFunc(int);
StringFunc(unsigned);

#undef StringFunc

class Helper
{
    public:
        static bool OddRotation(DXGI_MODE_ROTATION r)
        {
            return r == DXGI_MODE_ROTATION_ROTATE90 || r == DXGI_MODE_ROTATION_ROTATE270;
        }

        template<typename T> static HelperType::Vector2D<T> abs(const HelperType::Vector2D<T>& v)
        {
            return HelperType::Vector2D<T>(std::abs(v.x()), std::abs(v.y()));
        }

        template<typename T> static HelperType::Vector2D<T> swap(const HelperType::Vector2D<T>& v)
        {
            return HelperType::Vector2D<T>(v.y(), v.x());
        }

        template<typename T> static HelperType::Vector2D<T> inverse(const HelperType::Vector2D<T>& v)
        {
            return HelperType::Vector2D<T>( static_cast<T>(1) / v.x(), static_cast<T>(1) / v.y());
        }

        static HelperType::Vector2D<unsigned> Dimensions(const RECT& r)
        {
            return HelperType::Vector2D<unsigned>(r.right - r.left, r.bottom - r.top);
        }

        template<typename T> static T Box2D(unsigned right, unsigned bottom)
        {
            const T b = {0, 0, 0, right, bottom, 1};
            return b;
        }

        static bool signbit(const float f)
        {
            return f < 0.f;
        }

        template<typename T> static bool signbit(const HelperType::Vector2D<T>& v)
        {
            return signbit(v.x()) || signbit(v.y());
        }

        template<typename T> static RECT Scale(const RECT& r, const HelperType::Vector2D<T>& s)
        {
            return Rect(r.left, r.top, static_cast<T>(r.left + (r.right - r.left) * s.x()), static_cast<T>(r.top + (r.bottom - r.top) * s.y()));
        }

        static HelperType::Vector2D<unsigned> Scale(const HelperType::Vector2D<unsigned>& v, const HelperType::Vector2D<float>& f)
        {
            return HelperType::Vector2D<unsigned>(static_cast<unsigned>(v.x() * f.x()), static_cast<unsigned>(v.y() * f.y()));
        }

        static RECT RotationAdjust(const RECT& r, DXGI_MODE_ROTATION rot)
        {
            if (!OddRotation(rot)) return r;
            const auto d = RotationAdjust(Dimensions(r), rot);
            return Rect(r.left, r.top, r.left + d.x(), r.top + d.y());
        }

        template<typename T> static HelperType::Vector2D<T> RotationAdjust(const HelperType::Vector2D<T>& d, DXGI_MODE_ROTATION r)
        {
            if (!OddRotation(r)) return d;
            return d.swap();
        }

        template<typename T> static RECT Rect(unsigned l, unsigned t, T r, T b)
        {
            const RECT rect = {l, t, static_cast<unsigned>(r), static_cast<unsigned>(b)};
            return rect;
        }
};

#endif //__WGFHELPERS_H_