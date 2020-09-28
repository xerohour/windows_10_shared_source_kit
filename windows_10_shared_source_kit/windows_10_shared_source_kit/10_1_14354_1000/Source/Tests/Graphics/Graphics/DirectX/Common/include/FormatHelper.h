#pragma once

#include <vector>

#define INFP_FLOAT32		0x7f800000
#define INFN_FLOAT32		0xff800000
#define INFP_FLOAT16		0x00007c00
#define INFN_FLOAT16		0x0000fc00
#define INFP_FLOAT11		0x000007c0
#define INFN_FLOAT11		0x00000000
#define INFP_FLOAT10		0x000003e0
#define INFN_FLOAT10		0x00000000

#define DENORM_FLOAT32		0x00000001
#define DENORM_FLOAT16		0x00000001
#define DENORM_FLOAT11		0x00000001
#define DENORM_FLOAT10		0x00000002

#define NAN_FLOAT32			0x7f800001
#define NAN_FLOAT16			0x00007c01
#define NAN_FLOAT11			0x000007c1
#define NAN_FLOAT10			0x000007c2

#define MAX_FLOAT16			99999999.f
#define MIN_FLOAT16			-99999999.f
#define MAX_FLOAT11			99999999.f
#define MIN_FLOAT11			0.f
#define MAX_FLOAT10			99999999.f
#define MIN_FLOAT10			0.f

// These are better approximations of max and min values
// But we can enable these later.
//#define MAX_FLOAT16			3.277199609e+004
//#define MIN_FLOAT16			6.103515625e-005
//#define MAX_FLOAT11			3.276824609e+004
//#define MIN_FLOAT11			6.103515625e-005
//#define MAX_FLOAT10			3.276812109e+004
//#define MIN_FLOAT10			6.103515625e-005

#define MASK_EXP_FLOAT32	0x7f800000
#define MASK_MANT_FLOAT32	0x007fffff
#define SHIFT_EXP_FLOAT32	23

template<typename _FloatType>
struct RGBA_COLOR
{
	union
	{
		struct{ _FloatType r,g,b,a; };
		_FloatType comp[ 4 ];
	};

	RGBA_COLOR<_FloatType> operator *( const _FloatType &val ) const
	{
		RGBA_COLOR<_FloatType> res;
		res.r = r * val;
		res.g = g * val;
		res.b = b * val;
		res.a = a * val;

		return res;
	}

	RGBA_COLOR operator +( const RGBA_COLOR<_FloatType> &rhs ) const
	{
		RGBA_COLOR<_FloatType> res;
		res.r = r + rhs.r;
		res.g = g + rhs.g;
		res.b = b + rhs.b;
		res.a = a + rhs.a;

		return res;
	}
};

typedef RGBA_COLOR<float> FLOAT_COLOR;
typedef RGBA_COLOR<double> DOUBLE_COLOR;

enum CONVERSION_TYPE
{
	CONV_TO_FORMAT,
	CONV_FROM_FORMAT
};


class CFormatHelper
{
public:

    enum COMPONENT_ENCODING
    {
        COMPONENT_ENCODING_NONE         ,
        COMPONENT_ENCODING_FP           ,
        COMPONENT_ENCODING_SINT         ,
        COMPONENT_ENCODING_UINT         ,
        COMPONENT_ENCODING_XRBIAS       ,
        COMPONENT_ENCODING_X            ,
        COMPONENT_ENCODING_BC           ,
        COMPONENT_ENCODING_P8           ,
        COMPONENT_ENCODING_SHARED5BITEXP,
        COMPONENT_ENCODING_EXPONENT
    };

	static bool GetMipDimensions(UINT mipSlice, _Inout_ UINT *pWidth, _Inout_opt_ UINT *pHeight = NULL, _Inout_opt_ UINT *pDepth = NULL);
	static UINT ElementBits(DXGI_FORMAT f);
	static UINT ElementBytes(DXGI_FORMAT f);

    static COMPONENT_ENCODING   ComponentEncoding( DXGI_FORMAT f, UINT Channel );
    static UINT                 ComponentBits( DXGI_FORMAT f, UINT Channel );
    static float                MachineEpsilon( COMPONENT_ENCODING, UINT Bits );
    static float                MaxRepresentable( COMPONENT_ENCODING, UINT Bits );
    static float                MinRepresentable( COMPONENT_ENCODING, UINT Bits );
    static bool                 IsComponentPresent( DXGI_FORMAT, UINT Channel );

	static bool		CreateStripes(	DXGI_FORMAT resFormat,
									UINT uHeight, 
									UINT uWidth,
									UINT uNumStripes,
									const FLOAT_COLOR *pColors, 
									UINT uNumColors,
									void *pStripes, 
									bool bVertical,
									UINT rowPitch = 0);

	static bool		CreateGradient(	DXGI_FORMAT resFormat,
									UINT uHeight, 
									UINT uWidth,
									const FLOAT_COLOR pColors[ 4 ], 
									void *pGradient,
									UINT rowPitch = 0);

	static bool		CreateTiles(	DXGI_FORMAT resFormat,
									UINT uHeight, 
									UINT uWidth,
									UINT uEdgeWidth,
									const FLOAT_COLOR *pColors, 
									UINT uNumColors,
									void *pTiles,
									UINT rowPitch = 0);

	static bool		ConvertPixels(	CONVERSION_TYPE convType, 
									DXGI_FORMAT resFormat, 
									FLOAT_COLOR *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									void *pData,
									UINT rowPitch = 0);

	static bool		ConvertPixels(	CONVERSION_TYPE convType, 
									DXGI_FORMAT resFormat, 
									FLOAT_COLOR *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT rowPitch = 0,
									UINT slicePitch = 0);

    static RGBQUAD  CFormatHelper::ConvertYCrCbToRGB(
                                    int y,
                                    int cr,
                                    int cb
                                    );

	static bool		ReformatPixels(	CONVERSION_TYPE convType, 
									DXGI_FORMAT resFormat, 
									FLOAT_COLOR *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
                                    UINT Stride );

	static bool	EncodeCompressed(	DXGI_FORMAT resFormat, 
									const FLOAT_COLOR *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT rowPitch = 0,
									UINT slicePitch = 0 );

	static bool	DecodeCompressed(	DXGI_FORMAT resFormat, 
									const FLOAT_COLOR *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT rowPitch = 0,
									UINT slicePitch = 0
									);

	static void GetBlockSize(		DXGI_FORMAT resFormat,
									UINT* pBlockTexelWidth,
									UINT* pBlockTexelHeight,
									UINT* pBlockByteSize );

	static UINT GetSizeInBlocks(	UINT pixelSize, UINT blockSize);

	static bool ResourceViewContainsBothLuminanceDataAndChrominanceData( const DXGI_FORMAT &resourceFormat );
	static bool GetSRViewFormatsForVideoResourceFormat( const DXGI_FORMAT &resourceFormat, std::vector<DXGI_FORMAT> &luminanceData, std::vector<DXGI_FORMAT> &chrominanceData );
	static bool GetRTViewFormatsForVideoResourceFormat( const DXGI_FORMAT &resourceFormat, std::vector<DXGI_FORMAT> &luminanceData, std::vector<DXGI_FORMAT> &chrominanceData );
	static bool GetUAViewFormatsForVideoResourceFormat( const DXGI_FORMAT &resourceFormat, std::vector<DXGI_FORMAT> &luminanceData, std::vector<DXGI_FORMAT> &chrominanceData );

    static HRESULT GetFirstFormatWithSupport ( ID3D11Device *pDevice, const std::vector<DXGI_FORMAT> &resourceFormats, UINT FormatSupport, UINT &Index );

	static float GetBCFormatMaxTolerance(DXGI_FORMAT format);

	static bool	IsDenorm( float fValue );
	
	static bool	IsNaN( float fValue );

	static bool IsYUV( DXGI_FORMAT format );

	static bool IsPlanar( DXGI_FORMAT format );

	static void SetReferenceDeviceType( bool bUseWarpAsReference );

	static BYTE Clamp(float clr);

	static void Cleanup();
	
	static HRESULT SaveColorArray(
		__in_ecount(width * height) FLOAT_COLOR* colors,
		__in UINT width,
		__in UINT height,
		__in LPWSTR target_file_name_prefix);

	static HRESULT SaveColorArrayAsPNG(
		__in_ecount(width * height) FLOAT_COLOR* colors,
		__in UINT width,
		__in UINT height,
		__in LPWSTR target_file_name_prefix);

	static HRESULT SaveColorArray(
		void * pData,
		DXGI_FORMAT resFormat,
		__in UINT width,
		__in UINT height,
		__in LPWSTR target_file_name_prefix);


private:

	static bool CreateDevice( ID3D11Device **ppDevice );
	

	static HMODULE s_hD3D;

	static ID3D11Device* s_pRefDevice;

	static bool s_bUseWarpAsReference;
};
