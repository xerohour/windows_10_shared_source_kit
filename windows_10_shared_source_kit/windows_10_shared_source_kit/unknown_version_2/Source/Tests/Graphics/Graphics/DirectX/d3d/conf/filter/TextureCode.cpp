#include "Filter.h"
#include "FormatDesc.hpp"
#include "FormatHelper.h"
#include <D3DDepthHelper.h>
#include <d3dx10.h> // needed for BC texture format conversion

#define ZeroStructMem(x) ZeroMemory(&x, sizeof(x))

// TODO: ideally most of this functionality will be exposed through framework or d3dx

namespace Texture_ALU
{

struct Vec4
{
public:
	static const UINT R = 0;
	static const UINT G = 1;
	static const UINT B = 2;
	static const UINT A = 3;
	union
	{
		UINT u[4];
		INT32  i[4];
		float f[4];
	};
};

float
round_z( float a )
{
	modff( a, &a );
	return a;
}

double
round_z( double a )
{
	modf( a, &a );
	return a;
}

float
flush( float a )
{
	static const minNormalizedFloat = 0x00800000;
	static const signBit = 0x80000000;
	static const signBitComplement = 0x7fffffff;
	UINT b = (*(UINT*)&a) & signBitComplement; // fabs()
	if( b < minNormalizedFloat ) // UINT comparison. NaN/INF do test false here
	{
		b = signBit & (*(UINT*)&a);
		return *(float*)&b;
	}
	return a;
}

float
add( float a, float b )
{
	return flush( flush( a ) + flush( b ) );
}

float
sub( float a, float b )
{
	return flush( flush( a ) - flush( b ) );
}

float
mul( float a, float b )
{
	return flush( flush( a )* flush( b ) );
}

float
exp( float a )
{
	return flush( powf( 2, flush( a ) ) );
}

UINT 
ge( float a, float b )
{
	return (flush(a) >= flush(b)) ? 0xffffffff : 0;
}

float 
div( float a, float b )
{
	return flush( flush( a ) / flush( b ) );
}

float
linearToGamma( float a )
{
	if( ( a < D3D10_FLOAT_TO_SRGB_THRESHOLD ) )
	{
		return ( a * D3D10_FLOAT_TO_SRGB_SCALE_1 );
	}
	else
	{
		static const float scale = D3D10_FLOAT_TO_SRGB_EXPONENT_NUMERATOR/D3D10_FLOAT_TO_SRGB_EXPONENT_DENOMINATOR;
		float temp = pow( a, scale );
		temp = ( temp * D3D10_FLOAT_TO_SRGB_SCALE_2 );
		return ( temp - D3D10_FLOAT_TO_SRGB_OFFSET );
	}
}

float
gammaToLinear( float a )
{
	if( ( a < D3D10_SRGB_TO_FLOAT_THRESHOLD ) )
	{
		return ( a / D3D10_SRGB_TO_FLOAT_DENOMINATOR_1 );
	}
	else
	{
		float temp = ( a + D3D10_SRGB_TO_FLOAT_OFFSET );
		temp = ( temp / D3D10_SRGB_TO_FLOAT_DENOMINATOR_2 );
		return pow( temp, D3D10_SRGB_TO_FLOAT_EXPONENT );
	}
}

BOOL
isNaN( float a )
{
	static const exponentMask = 0x7f800000;
	static const mantissaMask = 0x007fffff;
	UINT u = *(UINT*)&a;
	return ( ( ( u & exponentMask ) == exponentMask ) && ( u & mantissaMask ) ); // NaN
}

UINT
float32_to_uint32( float a )
{
	a = round_z( a );
	if( a < 0 )
	{
		return 0;
	}
	else
	{
		return (UINT32)a;
	}
}

UINT
float64_to_uint32( double a )
{
	a = round_z( a );
	if( a < 0 )
	{
		return 0;
	}
	else
	{
		return (UINT32)a;
	}
}

UINT8
float32_to_unorm1( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 1 ); // 1 == ( 1 << 1 ) - 1

	a = add( a, 0.5 );

	return UINT8( float32_to_uint32( a ) ); // rounds to zero
}

UINT8
float32_to_unorm2( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 3 ); // 3 == ( 1 << 2 ) - 1

	a = add( a, 0.5 );

	return UINT8( float32_to_uint32( a ) ); // rounds to zero
}

UINT8
float32_to_unorm4( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 15 ); // 15 == ( 1 << 4 ) - 1

	a = add( a, 0.5 );

	return UINT8( float32_to_uint32( a ) ); // rounds to zero
}

UINT8
float32_to_unorm5( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 31 ); // 31 == ( 1 << 5 ) - 1

	a = add( a, 0.5 );

	return UINT8( float32_to_uint32( a ) ); // rounds to zero
}

UINT8
float32_to_unorm6( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 63 ); // 63 == ( 1 << 6 ) - 1

	a = add( a, 0.5 );

	return UINT8( float32_to_uint32( a ) ); // rounds to zero
}

UINT8
float32_to_unorm8( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 255 ); // 255 == ( 1 << 8 ) - 1

	a = add( a, 0.5 );

	return UINT8( float32_to_uint32( a ) ); // rounds to zero
}

UINT8
float32_to_unorm8_srgb( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = linearToGamma( a );

	a = mul( a, 255 ); // 255 == ( 1 << 8 ) - 1

	a = add( a, 0.5 );

	return UINT8( float32_to_uint32( a ) ); // rounds to zero
}

UINT16
float32_to_unorm10( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 1023 ); // 1023 == ( 1 << 10 ) - 1

	a = add( a, 0.5 );

	return UINT16( float32_to_uint32( a ) ); // rounds to zero
}

UINT16
float32_to_unorm11( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 2047 ); // 2047 == ( 1 << 11 ) - 1

	a = add( a, 0.5 );

	return UINT16( float32_to_uint32( a ) ); // rounds to zero
}

UINT16
float32_to_unorm16( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 65535 ); // 65535 == ( 1 << 16 ) - 1

	a = add( a, 0.5 );

	return UINT16( float32_to_uint32( a ) ); // rounds to zero
}

UINT
float32_to_unorm24( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// need more precision since unorm24 has more precision bits than float32
	double a2 = max(min(a, 1.0f), 0.0f);
	a2 = a2 * 16777215;
	a2 = a2 + 0.5;
	return UINT( float64_to_uint32( a2 ) ); // rounds to zero
}

INT32
float32_to_sint32( float a )
{
	a = round_z( a );
	return (INT32)a;
}

INT8
float32_to_snorm8( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ -1.0f, 1.0f ]
	a = max( min( a, 1.0f ), -1.0f );

	a = mul( a, 127 ); // 127 == ( 1 << ( 8 - 1 ) ) - 1

	if( ge( a, 0 ) )
	{
		a = add( a, 0.5f );
	}
	else
	{
		a = sub( a, 0.5f );
	}

	return INT8( float32_to_sint32( a ) ); // rounds to zero
}

INT16 
float32_to_snorm16( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ -1.0f, 1.0f ]
	a = max( min( a, 1.0f ), -1.0f );

	a = mul( a, 32767 ); // 32767 == ( 1 << ( 16 - 1 ) ) - 1

	if( ge( a, 0 ) )
	{
		a = add( a, 0.5f );
	}
	else
	{
		a = sub( a, 0.5f );
	}

	return INT16( float32_to_sint32( a ) ); // rounds to zero
}



float
uint32_to_float32( UINT a )
{
	return (float)a;
}

float
unorm1_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 1 ); // 1 = ( 1 << 1 ) - 1
}

float
unorm2_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 3 ); // 3 = ( 1 << 2 ) - 1
}

float
unorm4_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 15 ); // 15 = ( 1 << 4 ) - 1
}

float
unorm5_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 31 ); // 31 = ( 1 << 5 ) - 1
}

float
unorm6_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 63 ); // 63 = ( 1 << 6 ) - 1
}

float 
unorm8_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 255 ); // 255 = ( 1 << 8 ) - 1
}

float
unorm8_srgb_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	b = div( b, 255); // 255 = ( 1 << 8 ) - 1
	return gammaToLinear( b );
}

float
unorm10_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 1023 ); // 1023 = ( 1 << 10 ) - 1
}

float
unorm11_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 2047 ); // 2047 = ( 1 << 11 ) - 1
}

float
unorm16_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 65535 ); // 65535 = ( 1 << 16 ) - 1
}

float
unorm24_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 16777215 ); // 16777215 = ( 1 << 24 ) - 1
}

float
sint32_to_float32( INT32 a )
{
	return (float)a;
}

float
snorm8_to_float32( INT32 a )
{
	float b = sint32_to_float32( a );
	b = div( b, 127 ); // 127 == ( 1 << ( 8 - 1 ) ) - 1
	return max( b, -1.0f );
}

float
snorm16_to_float32( INT32 a )
{
	float b = sint32_to_float32( a );
	b = div( b, 32767 ); // 32767 == ( 1 << ( 16 - 1 ) ) - 1
	return max( b, -1.0f );
}

float
frc9_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 512 ); // 512 = ( 1 << 9 )
}

void
float32_to_float9(FLOAT f32, UINT& frac, UINT& exp)
{
	if(f32 < 1.0f/(FLOAT)(1 << 24))
	{
		frac = 0;
		exp = 0;
	}
	else if(f32 >= (FLOAT)(0x1FF << 7))
	{
		frac = 0x1FF;
		exp = 0x1F;
	}
	else
	{
		UINT u = *((UINT*) &f32);
		UINT e = (u >> 23) & 0xFF;
		if(e == 0xFF)
		{
			// INF or NaN
			if((u & (~0xff800000)) != 0)  // NaN
			{
				frac = exp = 0;
			}
			else                          // INF
			{
				frac = 0x1FF;
				exp = 0x1F;
			}
		}
		else
		{
			if(e)   // Normal
			{
				exp = e + 16 - 127;
				frac = ((u >> 15) & 0x0FF) | 0x100;
			}
			else    // Denormal
			{
				exp = 15;
				frac = (u >> 14) & 0x1FF;
			}
		}
	}
}

//-----------------------------------------------------------------------------
void 
WriteData(const TEST_VEC4F* pInputColor, 
		  DXGI_FORMAT DataType, 
		  BYTE* pOutput)
{
	const Vec4* pInput = ( Vec4* )( pInputColor );
	switch( DataType )
	{
	case( DXGI_FORMAT_R32G32B32A32_FLOAT ):
		{
			R32G32B32A32_UINT* output = 
				reinterpret_cast< R32G32B32A32_UINT* >( pOutput ); // treating as UINT to prevent any altering of data (e.g. SNAN->QNAN)
			output->Pack( pInput->u[Vec4::R],
				pInput->u[Vec4::G],
				pInput->u[Vec4::B],
				pInput->u[Vec4::A] );
			break;
		}
	case( DXGI_FORMAT_R32G32B32A32_SINT ):
		{
			R32G32B32A32_SINT* output =
				reinterpret_cast< R32G32B32A32_SINT* >( pOutput );
			output->Pack( pInput->i[Vec4::R],
				pInput->i[Vec4::G],
				pInput->i[Vec4::B],
				pInput->i[Vec4::A] );
			break;
		}
	case( DXGI_FORMAT_R32G32B32A32_UINT ):
		{
			R32G32B32A32_UINT* output =
				reinterpret_cast< R32G32B32A32_UINT* >( pOutput );
			output->Pack( pInput->u[Vec4::R],
				pInput->u[Vec4::G],
				pInput->u[Vec4::B],
				pInput->u[Vec4::A] );
			break;
		}
	case( DXGI_FORMAT_R32G32B32_FLOAT ):
		{
			R32G32B32_UINT* output = // treating as UINT to prevent any altering of data (e.g. SNAN->QNAN)
				reinterpret_cast< R32G32B32_UINT* >( pOutput );
			output->Pack(
				pInput->u[Vec4::R],
				pInput->u[Vec4::G],
				pInput->u[Vec4::B] );
			break;
		}
	case( DXGI_FORMAT_R32G32B32_SINT ):
		{
			R32G32B32_SINT* output =
				reinterpret_cast< R32G32B32_SINT* >( pOutput );
			output->Pack( pInput->i[Vec4::R],
				pInput->i[Vec4::G],
				pInput->i[Vec4::B] );
			break;
		}
	case( DXGI_FORMAT_R32G32B32_UINT ):
		{
			R32G32B32_UINT* output =
				reinterpret_cast< R32G32B32_UINT* >( pOutput );
			output->Pack( pInput->u[Vec4::R],
				pInput->u[Vec4::G],
				pInput->u[Vec4::B] );
			break;
		}
	case( DXGI_FORMAT_R16G16B16A16_FLOAT ):
		{
			R16G16B16A16_FLOAT* output =
				reinterpret_cast< R16G16B16A16_FLOAT* >( pOutput );
			output->Pack(
				float16::make(&pInput->f[Vec4::R]),
				float16::make(&pInput->f[Vec4::G]),
				float16::make(&pInput->f[Vec4::B]),
				float16::make(&pInput->f[Vec4::A]) );
			break;
		}
	case( DXGI_FORMAT_R16G16B16A16_UNORM ):
		{
			R16G16B16A16_UINT* output =
				reinterpret_cast< R16G16B16A16_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm16( pInput->f[Vec4::R] ),
				float32_to_unorm16( pInput->f[Vec4::G] ),
				float32_to_unorm16( pInput->f[Vec4::B] ),
				float32_to_unorm16( pInput->f[Vec4::A] ) );
			break;
		}
	case( DXGI_FORMAT_R16G16B16A16_SNORM ):
		{
			R16G16B16A16_SINT* output =
				reinterpret_cast< R16G16B16A16_SINT* >( pOutput );
			output->Pack(
				float32_to_snorm16( pInput->f[Vec4::R] ),
				float32_to_snorm16( pInput->f[Vec4::G] ),
				float32_to_snorm16( pInput->f[Vec4::B] ),
				float32_to_snorm16( pInput->f[Vec4::A] ) );
			break;
		}
	case( DXGI_FORMAT_R16G16B16A16_SINT ):
		{
			R16G16B16A16_SINT* output =
				reinterpret_cast< R16G16B16A16_SINT* >( pOutput );
			output->Pack(
				pInput->i[Vec4::R],
				pInput->i[Vec4::G],
				pInput->i[Vec4::B],
				pInput->i[Vec4::A] );
			break;
		}
	case( DXGI_FORMAT_R16G16B16A16_UINT ):
		{
			R16G16B16A16_UINT* output =
				reinterpret_cast< R16G16B16A16_UINT* >( pOutput );
			output->Pack(
				pInput->u[Vec4::R],
				pInput->u[Vec4::G],
				pInput->u[Vec4::B],
				pInput->u[Vec4::A] );
			break;
		}
	case( DXGI_FORMAT_R32G32_FLOAT ):
		{
			R32G32_UINT* output =
				reinterpret_cast< R32G32_UINT* >( pOutput ); // treating as UINT to prevent any altering of data (e.g. SNAN->QNAN)
			output->Pack(
				pInput->u[Vec4::R],
				pInput->u[Vec4::G] );
			break;
		}
	case( DXGI_FORMAT_R32G32_SINT ):
		{
			R32G32_SINT* output =
				reinterpret_cast< R32G32_SINT* >( pOutput );
			output->Pack(
				pInput->i[Vec4::R],
				pInput->i[Vec4::G] );
			break;
		}
	case( DXGI_FORMAT_R32G32_UINT ):
		{
			R32G32_UINT* output =
				reinterpret_cast< R32G32_UINT* >( pOutput );
			output->Pack(
				pInput->u[Vec4::R],
				pInput->u[Vec4::G] );
			break;
		}
	case( DXGI_FORMAT_R10G10B10A2_UNORM ):
		{
			R10G10B10A2_UINT* output =
				reinterpret_cast< R10G10B10A2_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm10( pInput->f[Vec4::R] ),
				float32_to_unorm10( pInput->f[Vec4::G] ),
				float32_to_unorm10( pInput->f[Vec4::B] ),
				float32_to_unorm2(  pInput->f[Vec4::A] ) );
			break;
		}
	case( DXGI_FORMAT_R10G10B10A2_UINT ):
		{
			R10G10B10A2_UINT* output =
				reinterpret_cast< R10G10B10A2_UINT* >( pOutput );
			output->Pack(
				pInput->u[Vec4::R],
				pInput->u[Vec4::G],
				pInput->u[Vec4::B],
				pInput->u[Vec4::A] );
			break;
		}
	case( DXGI_FORMAT_R11G11B10_FLOAT ):
		{
			R11G11B10_FLOAT* output = 
				reinterpret_cast< R11G11B10_FLOAT* >( pOutput );
			output->Pack(
				float11::make(&pInput->f[Vec4::R]),
				float11::make(&pInput->f[Vec4::G]),
				float10::make(&pInput->f[Vec4::B]));
			break;
		}
	case( DXGI_FORMAT_R8G8B8A8_UNORM ):
		{
			R8G8B8A8_UINT* output =
				reinterpret_cast< R8G8B8A8_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm8( pInput->f[Vec4::R] ),
				float32_to_unorm8( pInput->f[Vec4::G] ),
				float32_to_unorm8( pInput->f[Vec4::B] ),
				float32_to_unorm8( pInput->f[Vec4::A] ) );
			break;
		}
	case( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ):
		{
			R8G8B8A8_UINT* output =
				reinterpret_cast< R8G8B8A8_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm8_srgb( pInput->f[Vec4::R] ),
				float32_to_unorm8_srgb( pInput->f[Vec4::G] ),
				float32_to_unorm8_srgb( pInput->f[Vec4::B] ),
				float32_to_unorm8( pInput->f[Vec4::A] ) );
			break;
		}
	case( DXGI_FORMAT_B8G8R8X8_UNORM ):
	case( DXGI_FORMAT_B8G8R8A8_UNORM ):
		{
			R8G8B8A8_UINT* output =
				reinterpret_cast< R8G8B8A8_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm8( pInput->f[Vec4::B] ),
				float32_to_unorm8( pInput->f[Vec4::G] ),
				float32_to_unorm8( pInput->f[Vec4::R] ),
				float32_to_unorm8( pInput->f[Vec4::A] ) );
			break;
		}
	case( DXGI_FORMAT_B5G6R5_UNORM ):
		{
			// See DXGI_FORMAT_B8G8R8X8_UNORM casts to R8G8B8A8_UINT,
			// since we cast to a BGRA here we need to move it to RGBA format from BGRA
			B5G6R5_UINT* output =
				reinterpret_cast< B5G6R5_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm5( pInput->f[Vec4::R] ),
				float32_to_unorm6( pInput->f[Vec4::G] ),
				float32_to_unorm5( pInput->f[Vec4::B] ) );
			break;
		}
	case( DXGI_FORMAT_B5G5R5A1_UNORM ):
		{
			// See DXGI_FORMAT_B8G8R8X8_UNORM casts to R8G8B8A8_UINT,
			// since we cast to a BGRA here we need to move it to RGBA format from BGRA
			B5G5R5A1_UINT* output =
				reinterpret_cast< B5G5R5A1_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm5( pInput->f[Vec4::R] ),
				float32_to_unorm5( pInput->f[Vec4::G] ),
				float32_to_unorm5( pInput->f[Vec4::B] ),
				float32_to_unorm1( pInput->f[Vec4::A] ) );
			break;
		}
	case( DXGI_FORMAT_B4G4R4A4_UNORM ):
		{
			// See DXGI_FORMAT_B8G8R8X8_UNORM casts to R8G8B8A8_UINT,
			// since we cast to a BGRA here we need to move it to RGBA format from BGRA
			B4G4R4A4_UINT* output =
				reinterpret_cast< B4G4R4A4_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm4( pInput->f[Vec4::R] ),
				float32_to_unorm4( pInput->f[Vec4::G] ),
				float32_to_unorm4( pInput->f[Vec4::B] ),
				float32_to_unorm4( pInput->f[Vec4::A] ) );
			break;
		}
	case( DXGI_FORMAT_B8G8R8X8_UNORM_SRGB ):
	case( DXGI_FORMAT_B8G8R8A8_UNORM_SRGB ):
		{
			R8G8B8A8_UINT* output =
				reinterpret_cast< R8G8B8A8_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm8_srgb( pInput->f[Vec4::B] ),
				float32_to_unorm8_srgb( pInput->f[Vec4::G] ),
				float32_to_unorm8_srgb( pInput->f[Vec4::R] ),
				float32_to_unorm8( pInput->f[Vec4::A] ) );
			break;
		}
	case( DXGI_FORMAT_R8G8B8A8_SNORM ):
		{
			R8G8B8A8_SINT* output =
				reinterpret_cast< R8G8B8A8_SINT* >( pOutput );
			output->Pack(
				float32_to_snorm8( pInput->f[Vec4::R] ),
				float32_to_snorm8( pInput->f[Vec4::G] ),
				float32_to_snorm8( pInput->f[Vec4::B] ),
				float32_to_snorm8( pInput->f[Vec4::A] ) );
			break;
		}
	case( DXGI_FORMAT_R8G8B8A8_SINT ):
		{
			R8G8B8A8_SINT* output =
				reinterpret_cast< R8G8B8A8_SINT* >( pOutput );
			output->Pack(
				pInput->i[Vec4::R],
				pInput->i[Vec4::G],
				pInput->i[Vec4::B],
				pInput->i[Vec4::A] );
			break;
		}
	case( DXGI_FORMAT_R8G8B8A8_UINT ):
		{
			R8G8B8A8_UINT* output =
				reinterpret_cast< R8G8B8A8_UINT* >( pOutput );
			output->Pack(
				pInput->u[Vec4::R],
				pInput->u[Vec4::G],
				pInput->u[Vec4::B],
				pInput->u[Vec4::A] );
			break;
		}
	case( DXGI_FORMAT_R16G16_UNORM ):
		{
			R16G16_UINT* output =
				reinterpret_cast< R16G16_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm16( pInput->f[Vec4::R] ),
				float32_to_unorm16( pInput->f[Vec4::G] ) );
			break;
		}
	case( DXGI_FORMAT_R16G16_SNORM ):
		{
			R16G16_SINT* output =
				reinterpret_cast< R16G16_SINT* >( pOutput );
			output->Pack(
				float32_to_snorm16( pInput->f[Vec4::R] ),
				float32_to_snorm16( pInput->f[Vec4::G] ) );
			break;
		}

	case( DXGI_FORMAT_R16G16_SINT ):
		{
			R16G16_SINT* output =
				reinterpret_cast< R16G16_SINT* >( pOutput );
			output->Pack(
				pInput->i[Vec4::R],
				pInput->i[Vec4::G] );
			break;
		}

	case( DXGI_FORMAT_R16G16_UINT ):
		{
			R16G16_UINT* output =
				reinterpret_cast< R16G16_UINT* >( pOutput );
			output->Pack(
				pInput->u[Vec4::R],
				pInput->u[Vec4::G] );
			break;
		}

	case( DXGI_FORMAT_R16G16_FLOAT ):
		{
			R16G16_FLOAT* output =
				reinterpret_cast< R16G16_FLOAT* >( pOutput );
			output->Pack(
				float16::make( &pInput->f[Vec4::R] ),
				float16::make( &pInput->f[Vec4::G] ) );
			break;
		}

	case( DXGI_FORMAT_R32_SINT ):
		{
			R32_SINT* output =
				reinterpret_cast< R32_SINT* >( pOutput );
			output->Pack(
				pInput->i[Vec4::R] );
			break;
		}
	case( DXGI_FORMAT_R32_UINT ):
		{
			R32_UINT* output =
				reinterpret_cast< R32_UINT* >( pOutput );
			output->Pack(
				pInput->u[Vec4::R] );
			break;
		}
	case( DXGI_FORMAT_R32_FLOAT ):
		{
			R32_UINT* output =
				reinterpret_cast< R32_UINT* >( pOutput ); // treating as UINT to prevent any altering of data (e.g. SNAN->QNAN)
			output->Pack(
				pInput->u[Vec4::R] );
			break;
		}
	case( DXGI_FORMAT_R8G8_UNORM ):
		{
			R8G8_UINT* output =
				reinterpret_cast< R8G8_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm8( pInput->f[Vec4::R] ),
				float32_to_unorm8( pInput->f[Vec4::G] ) );
			break;
		}
	case( DXGI_FORMAT_R8G8_SNORM ):
		{
			R8G8_SINT* output =
				reinterpret_cast< R8G8_SINT* >( pOutput );
			output->Pack(
				float32_to_snorm8( pInput->f[Vec4::R] ),
				float32_to_snorm8( pInput->f[Vec4::G] ) );
			break;
		}
	case( DXGI_FORMAT_R8G8_SINT ):
		{
			R8G8_SINT* output =
				reinterpret_cast< R8G8_SINT* >( pOutput );
			output->Pack(
				pInput->i[Vec4::R],
				pInput->i[Vec4::G] );
			break;
		}
	case( DXGI_FORMAT_R8G8_UINT ):
		{
			R8G8_UINT* output =
				reinterpret_cast< R8G8_UINT* >( pOutput );
			output->Pack(
				pInput->u[Vec4::R],
				pInput->u[Vec4::G] );
			break;
		}
	case( DXGI_FORMAT_R16_UNORM ):
		{
			R16_UINT* output =
				reinterpret_cast< R16_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm16( pInput->f[Vec4::R] ) );
			break;
		}
	case( DXGI_FORMAT_R16_SNORM ):
		{
			R16_SINT* output =
				reinterpret_cast< R16_SINT* >( pOutput );
			output->Pack(
				float32_to_snorm16( pInput->f[Vec4::R] ) );
			break;
		}
	case( DXGI_FORMAT_R16_SINT ):
		{
			R16_SINT* output =
				reinterpret_cast< R16_SINT* >( pOutput );
			output->Pack(
				pInput->i[Vec4::R] );
			break;
		}
	case( DXGI_FORMAT_R16_UINT ):
		{
			R16_UINT* output =
				reinterpret_cast< R16_UINT* >( pOutput );
			output->Pack(
				pInput->u[Vec4::R] );
			break;
		}
	case( DXGI_FORMAT_R16_FLOAT ):
		{
			R16_FLOAT* output =
				reinterpret_cast< R16_FLOAT* >( pOutput );
			output->Pack(
				float16::make(&pInput->f[Vec4::R]));
			break;
		}
	case( DXGI_FORMAT_R8_UNORM ):
		{
			R8_UINT* output =
				reinterpret_cast< R8_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm8( pInput->f[Vec4::R] ) );
			break;
		}
	case( DXGI_FORMAT_R8_SNORM ):
		{
			R8_SINT* output =
				reinterpret_cast< R8_SINT* >( pOutput );
			output->Pack(
				float32_to_snorm8( pInput->f[Vec4::R] ) );
			break;
		}
	case( DXGI_FORMAT_R8_SINT ):
		{
			R8_SINT* output =
				reinterpret_cast< R8_SINT* >( pOutput );
			output->Pack(
				pInput->i[Vec4::R] );
			break;
		}
	case( DXGI_FORMAT_R8_UINT ):
		{
			R8_UINT* output =
				reinterpret_cast< R8_UINT* >( pOutput );
			output->Pack(
				pInput->u[Vec4::R] );
			break;
		}
	case( DXGI_FORMAT_A8_UNORM ):
		{
			A8_UINT* output =
				reinterpret_cast< A8_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm8( pInput->f[Vec4::A] ) );
			break;
		}

	// the following are hacks to support more formats than originally
	case( DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS ):
		{
			R32X8X24_FLOAT* output =
				reinterpret_cast< R32X8X24_FLOAT* >( pOutput );
			output->Pack(
				pInput->f[Vec4::R] );
			break;
		}
	case( DXGI_FORMAT_R24_UNORM_X8_TYPELESS ):
		{
			R24X8_UINT* output =
				reinterpret_cast< R24X8_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm24( pInput->f[Vec4::R] ) );
			break;
		}
	case( DXGI_FORMAT_X32_TYPELESS_G8X24_UINT ):
		{
			X32G8X24_UINT* output =
				reinterpret_cast< X32G8X24_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm8(pInput->f[Vec4::G]) );
			break;
		}
	case( DXGI_FORMAT_X24_TYPELESS_G8_UINT ):
		{
			X24G8_UINT* output =
				reinterpret_cast< X24G8_UINT* >( pOutput );
			output->Pack(
				float32_to_unorm8(pInput->f[Vec4::G]) );
			break;
		}

	case( DXGI_FORMAT_R9G9B9E5_SHAREDEXP ):
		{
			//sharedExponent =  max(redExponent, greenExponent, blueExponent); [note, these are treated as unbiased]
			//foreach component {R,G,B}:
			//	convert fraction to 9 bits
			//	output fraction = (converted fraction) >> (sharedExponent - componentExponent))
			//bias exponent (add 15) and store

			UINT32 fractions[3];
			UINT32 exponents[3];
			for (int n = 0; n < 3; ++n)
				float32_to_float9(pInputColor->f[n], fractions[n], exponents[n]);
			UINT32 sharedExponent = max(exponents[0], max(exponents[1], exponents[2]));
			for (int n = 0; n < 3; ++n)
				fractions[n] = fractions[n] >> (sharedExponent - exponents[n]);

			R9G9B9E5_UINT* output =
				reinterpret_cast< R9G9B9E5_UINT* >( pOutput );
			output->Pack(fractions[0], fractions[1], fractions[2], sharedExponent);
			break;
		}


	case( DXGI_FORMAT_D32_FLOAT_S8X24_UINT ):
	case( DXGI_FORMAT_D32_FLOAT ):
	case( DXGI_FORMAT_D24_UNORM_S8_UINT ):

	case( DXGI_FORMAT_R32G32B32A32_TYPELESS ):
	case( DXGI_FORMAT_R32G32B32_TYPELESS ):
	case( DXGI_FORMAT_R16G16B16A16_TYPELESS ):
	case( DXGI_FORMAT_R32G32_TYPELESS ):
	case( DXGI_FORMAT_R32G8X24_TYPELESS ):
	case( DXGI_FORMAT_R10G10B10A2_TYPELESS ):
	case( DXGI_FORMAT_R8G8B8A8_TYPELESS ):
	case( DXGI_FORMAT_R16G16_TYPELESS ):
	case( DXGI_FORMAT_R32_TYPELESS ):
	case( DXGI_FORMAT_R24G8_TYPELESS ):
	case( DXGI_FORMAT_R8G8_TYPELESS ):
	case( DXGI_FORMAT_R16_TYPELESS ):
	case( DXGI_FORMAT_D16_UNORM ):
	case( DXGI_FORMAT_R1_UNORM ):
	case( DXGI_FORMAT_UNKNOWN ):
	case( DXGI_FORMAT_R8G8_B8G8_UNORM ):
	case( DXGI_FORMAT_G8R8_G8B8_UNORM ):
	case( DXGI_FORMAT_BC1_TYPELESS ):
	case( DXGI_FORMAT_BC1_UNORM ):
	case( DXGI_FORMAT_BC1_UNORM_SRGB ):
	case( DXGI_FORMAT_BC2_TYPELESS ):
	case( DXGI_FORMAT_BC2_UNORM ):
	case( DXGI_FORMAT_BC2_UNORM_SRGB ):
	case( DXGI_FORMAT_BC3_TYPELESS ):
	case( DXGI_FORMAT_BC3_UNORM ):
	case( DXGI_FORMAT_BC3_UNORM_SRGB ):
	case( DXGI_FORMAT_BC4_TYPELESS ):
	case( DXGI_FORMAT_BC4_UNORM ):
	case( DXGI_FORMAT_BC4_SNORM ):
	case( DXGI_FORMAT_BC5_TYPELESS ):
	case( DXGI_FORMAT_BC5_UNORM ):
	case( DXGI_FORMAT_BC5_SNORM ):
	default:
		assert( !"Unexpected texture format" );
		break;
	}
}

//-----------------------------------------------------------------------------
void
ReadDataWithDefaults(const void* pInput, 
					 UINT uOffset, 
					 DXGI_FORMAT DataType, 
					 TEST_VEC4F* pOutputColor)
{
	Vec4* pOutput = ( Vec4* )( pOutputColor );
	switch( DataType )
	{
	case( DXGI_FORMAT_R32G32B32A32_FLOAT ):
		{
			const R32G32B32A32_UINT* input =
				reinterpret_cast< const R32G32B32A32_UINT* >( pInput );// treating as UINT to prevent any altering of data (e.g. SNAN->QNAN)
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = input->B();
			pOutput->u[Vec4::A] = input->A();
			break;
		}
	case( DXGI_FORMAT_R32G32B32A32_SINT ):
		{
			const R32G32B32A32_SINT* input =
				reinterpret_cast< const R32G32B32A32_SINT* >( pInput );
			pOutput->i[Vec4::R] = input->R();
			pOutput->i[Vec4::G] = input->G();
			pOutput->i[Vec4::B] = input->B();
			pOutput->i[Vec4::A] = input->A();
			break;
		}
	case( DXGI_FORMAT_R32G32B32A32_UINT ):
		{
			const R32G32B32A32_UINT* input =
				reinterpret_cast< const R32G32B32A32_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = input->B();
			pOutput->u[Vec4::A] = input->A();
			break;
		}
	case( DXGI_FORMAT_R32G32B32_FLOAT ):
		{
			const R32G32B32_UINT* input =
				reinterpret_cast< const R32G32B32_UINT* >( pInput );// treating as UINT to prevent any altering of data (e.g. SNAN->QNAN)
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = input->B();
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R32G32B32_SINT ):
		{
			const R32G32B32_SINT* input =
				reinterpret_cast< const R32G32B32_SINT* >( pInput );
			pOutput->i[Vec4::R] = input->R();
			pOutput->i[Vec4::G] = input->G();
			pOutput->i[Vec4::B] = input->B();
			pOutput->i[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R32G32B32_UINT ):
		{
			const R32G32B32_UINT* input =
				reinterpret_cast< const R32G32B32_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = input->B();
			pOutput->u[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R16G16B16A16_FLOAT ):
		{
			const R16G16B16A16_FLOAT* input =
				reinterpret_cast< const R16G16B16A16_FLOAT* >( pInput );
			pOutput->u[Vec4::R] = input->R().toFloat32_AsUINT32Bits(); // prevents SNaN->QNaN quash during float copy (preserves bits)
			pOutput->u[Vec4::G] = input->G().toFloat32_AsUINT32Bits();
			pOutput->u[Vec4::B] = input->B().toFloat32_AsUINT32Bits();
			pOutput->u[Vec4::A] = input->A().toFloat32_AsUINT32Bits();
			break;
		}
	case( DXGI_FORMAT_R16G16B16A16_UNORM ):
		{
			const R16G16B16A16_UINT* input =
				reinterpret_cast< const R16G16B16A16_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm16_to_float32( input->R() );
			pOutput->f[Vec4::G] = unorm16_to_float32( input->G() );
			pOutput->f[Vec4::B] = unorm16_to_float32( input->B() );
			pOutput->f[Vec4::A] = unorm16_to_float32( input->A() );
			break;
		}

	case( DXGI_FORMAT_R16G16B16A16_SNORM ):
		{
			const R16G16B16A16_SINT* input =
				reinterpret_cast< const R16G16B16A16_SINT* >( pInput );
			pOutput->f[Vec4::R] = snorm16_to_float32( input->R() );
			pOutput->f[Vec4::G] = snorm16_to_float32( input->G() );
			pOutput->f[Vec4::B] = snorm16_to_float32( input->B() );
			pOutput->f[Vec4::A] = snorm16_to_float32( input->A() );
			break;
		}
	case( DXGI_FORMAT_R16G16B16A16_SINT ):
		{
			const R16G16B16A16_SINT* input =
				reinterpret_cast< const R16G16B16A16_SINT* >( pInput );
			pOutput->i[Vec4::R] = input->R();
			pOutput->i[Vec4::G] = input->G();
			pOutput->i[Vec4::B] = input->B();
			pOutput->i[Vec4::A] = input->A();
			break;
		}
	case( DXGI_FORMAT_R16G16B16A16_UINT ):
		{
			const R16G16B16A16_UINT* input =
				reinterpret_cast< const R16G16B16A16_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = input->B();
			pOutput->u[Vec4::A] = input->A();
			break;
		}
	case( DXGI_FORMAT_R32G32_FLOAT ):
		{
			const R32G32_UINT* input = // treating as UINT to prevent any altering of data (e.g. SNAN->QNAN)
				reinterpret_cast< const R32G32_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = input->G();
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R32G32_SINT ):
		{
			const R32G32_SINT* input =
				reinterpret_cast< const R32G32_SINT* >( pInput );
			pOutput->i[Vec4::R] = input->R();
			pOutput->i[Vec4::G] = input->G();
			pOutput->i[Vec4::B] = 0;
			pOutput->i[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R32G32_UINT ):
		{
			const R32G32_UINT* input =
				reinterpret_cast< const R32G32_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = 0;
			pOutput->u[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS ):
		{
			const R32X8X24_FLOAT* input =
				reinterpret_cast< const R32X8X24_FLOAT* >( pInput );
			float r = input->R();
			pOutput->u[Vec4::R] = *(UINT*)&r; // treating as UINT to prevent any altering of data (e.g. SNAN->QNAN)
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_X32_TYPELESS_G8X24_UINT ):
		{
			const X32G8X24_UINT* input =
				reinterpret_cast< const X32G8X24_UINT* >( pInput );
			pOutput->u[Vec4::R] = 0;
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = 0;
			pOutput->u[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R10G10B10A2_UNORM ):
		{
			const R10G10B10A2_UINT* input =
				reinterpret_cast< const R10G10B10A2_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm10_to_float32( input->R() );
			pOutput->f[Vec4::G] = unorm10_to_float32( input->G() );
			pOutput->f[Vec4::B] = unorm10_to_float32( input->B() );
			pOutput->f[Vec4::A] = unorm2_to_float32( input->A() );
			break;
		}
	case( DXGI_FORMAT_R10G10B10A2_UINT ):
		{
			const R10G10B10A2_UINT* input =
				reinterpret_cast< const R10G10B10A2_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = input->B();
			pOutput->u[Vec4::A] = input->A();
			break;
		}
	case( DXGI_FORMAT_R11G11B10_FLOAT ):
		{
			const R11G11B10_FLOAT* input = 
				reinterpret_cast< const R11G11B10_FLOAT*>( pInput );
			pOutput->u[Vec4::R] = input->R().toFloat32_AsUINT32Bits(); // prevents SNaN->QNaN quash during float copy (preserves bits)
			pOutput->u[Vec4::G] = input->G().toFloat32_AsUINT32Bits();
			pOutput->u[Vec4::B] = input->B().toFloat32_AsUINT32Bits();
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R8G8B8A8_UNORM ):
		{
			const R8G8B8A8_UINT* input =
				reinterpret_cast< const R8G8B8A8_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm8_to_float32( input->R() );
			pOutput->f[Vec4::G] = unorm8_to_float32( input->G() );
			pOutput->f[Vec4::B] = unorm8_to_float32( input->B() );
			pOutput->f[Vec4::A] = unorm8_to_float32( input->A() );
			break;
		}
	case( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ):
		{
			const R8G8B8A8_UINT* input =
				reinterpret_cast< const R8G8B8A8_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm8_srgb_to_float32( input->R() );
			pOutput->f[Vec4::G] = unorm8_srgb_to_float32( input->G() );
			pOutput->f[Vec4::B] = unorm8_srgb_to_float32( input->B() );
			pOutput->f[Vec4::A] = unorm8_to_float32( input->A() );
			break;
		}
	case( DXGI_FORMAT_B8G8R8A8_UNORM ):
	case( DXGI_FORMAT_B8G8R8X8_UNORM ):
		{
			const R8G8B8A8_UINT* input =
				reinterpret_cast< const R8G8B8A8_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm8_to_float32( input->B() );
			pOutput->f[Vec4::G] = unorm8_to_float32( input->G() );
			pOutput->f[Vec4::B] = unorm8_to_float32( input->R() );
			pOutput->f[Vec4::A] = (DataType==DXGI_FORMAT_B8G8R8X8_UNORM) ? 1 : unorm8_to_float32( input->A() );
			break;
		}
	case( DXGI_FORMAT_B5G6R5_UNORM ):
		{
			// See DXGI_FORMAT_B8G8R8X8_UNORM casts to R8G8B8A8_UINT, thus when if sets output it needs to account for RGBA vs BGRA
			// since we use a BGRA format here to case, we don't need to do this.
			const B5G6R5_UINT* input =
				reinterpret_cast< const B5G6R5_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm5_to_float32( input->R() );
			pOutput->f[Vec4::G] = unorm6_to_float32( input->G() );
			pOutput->f[Vec4::B] = unorm5_to_float32( input->B() );
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_B5G5R5A1_UNORM ):
		{
			// See DXGI_FORMAT_B8G8R8X8_UNORM casts to R8G8B8A8_UINT, thus when if sets output it needs to account for RGBA vs BGRA
			// since we use a BGRA format here to case, we don't need to do this.
			const B5G5R5A1_UINT* input =
				reinterpret_cast< const B5G5R5A1_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm5_to_float32( input->R() );
			pOutput->f[Vec4::G] = unorm5_to_float32( input->G() );
			pOutput->f[Vec4::B] = unorm5_to_float32( input->B() );
			pOutput->f[Vec4::A] = unorm1_to_float32( input->A() );
			break;
		}
	case( DXGI_FORMAT_B4G4R4A4_UNORM ):
		{
			// See DXGI_FORMAT_B8G8R8X8_UNORM casts to R8G8B8A8_UINT, thus when if sets output it needs to account for RGBA vs BGRA
			// since we use a BGRA format here to case, we don't need to do this.
			const B4G4R4A4_UINT* input =
				reinterpret_cast< const B4G4R4A4_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm4_to_float32( input->R() );
			pOutput->f[Vec4::G] = unorm4_to_float32( input->G() );
			pOutput->f[Vec4::B] = unorm4_to_float32( input->B() );
			pOutput->f[Vec4::A] = unorm4_to_float32( input->A() );
			break;
		}
	case( DXGI_FORMAT_B8G8R8A8_UNORM_SRGB ):
	case( DXGI_FORMAT_B8G8R8X8_UNORM_SRGB ):
		{
			const R8G8B8A8_UINT* input =
				reinterpret_cast< const R8G8B8A8_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm8_srgb_to_float32( input->B() );
			pOutput->f[Vec4::G] = unorm8_srgb_to_float32( input->G() );
			pOutput->f[Vec4::B] = unorm8_srgb_to_float32( input->R() );
			pOutput->f[Vec4::A] = (DataType==DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) ? 1 : unorm8_to_float32( input->A() );
			break;
		}
	case( DXGI_FORMAT_R8G8B8A8_SNORM ):
		{
			const R8G8B8A8_SINT* input =
				reinterpret_cast< const R8G8B8A8_SINT* >( pInput );
			pOutput->f[Vec4::R] = snorm8_to_float32( input->R() );
			pOutput->f[Vec4::G] = snorm8_to_float32( input->G() );
			pOutput->f[Vec4::B] = snorm8_to_float32( input->B() );
			pOutput->f[Vec4::A] = snorm8_to_float32( input->A() );
			break;
		}
	case( DXGI_FORMAT_R8G8B8A8_SINT ):
		{
			const R8G8B8A8_SINT* input =
				reinterpret_cast< const R8G8B8A8_SINT* >( pInput );
			pOutput->i[Vec4::R] = input->R();
			pOutput->i[Vec4::G] = input->G();
			pOutput->i[Vec4::B] = input->B();
			pOutput->i[Vec4::A] = input->A();
			break;
		}
	case( DXGI_FORMAT_R8G8B8A8_UINT ):
		{
			const R8G8B8A8_UINT* input =
				reinterpret_cast< const R8G8B8A8_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = input->B();
			pOutput->u[Vec4::A] = input->A();
			break;
		}
	case( DXGI_FORMAT_R16G16_UNORM ):
		{
			const R16G16_UINT* input =
				reinterpret_cast< const R16G16_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm16_to_float32( input->R() );
			pOutput->f[Vec4::G] = unorm16_to_float32( input->G() );
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R16G16_SNORM ):
		{
			const R16G16_SINT* input =
				reinterpret_cast< const R16G16_SINT* >( pInput );
			pOutput->f[Vec4::R] = snorm16_to_float32( input->R() );
			pOutput->f[Vec4::G] = snorm16_to_float32( input->G() );
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R16G16_SINT ):
		{
			const R16G16_SINT* input =
				reinterpret_cast< const R16G16_SINT* >( pInput );
			pOutput->i[Vec4::R] = input->R();
			pOutput->i[Vec4::G] = input->G();
			pOutput->i[Vec4::B] = 0;
			pOutput->i[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R16G16_UINT ):
		{
			const R16G16_UINT* input =
				reinterpret_cast< const R16G16_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = 0;
			pOutput->u[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R16G16_FLOAT ):
		{
			const R16G16_FLOAT* input =
				reinterpret_cast< const R16G16_FLOAT* >( pInput );
			pOutput->u[Vec4::R] = input->R().toFloat32_AsUINT32Bits(); // prevents SNaN->QNaN quash during float copy (preserves bits)
			pOutput->u[Vec4::G] = input->G().toFloat32_AsUINT32Bits();
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R32_SINT ):
		{
			const R32_SINT* input =
				reinterpret_cast< const R32_SINT* >( pInput );
			pOutput->i[Vec4::R] = input->R();
			pOutput->i[Vec4::G] = 0;
			pOutput->i[Vec4::B] = 0;
			pOutput->i[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R32_UINT ):
		{
			const R32_UINT* input =
				reinterpret_cast< const R32_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = 0;
			pOutput->u[Vec4::B] = 0;
			pOutput->u[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R32_FLOAT ):
		{
			const R32_UINT* input =
				reinterpret_cast< const R32_UINT* >( pInput ); // treating as UINT to prevent any altering of data (e.g. SNAN->QNAN)
			pOutput->u[Vec4::R] = input->R();
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R24_UNORM_X8_TYPELESS ):
		{
			const R24X8_UINT* input =
				reinterpret_cast< const R24X8_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm24_to_float32( input->R() );
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_X24_TYPELESS_G8_UINT ):
		{
			const X24G8_UINT* input =
				reinterpret_cast< const X24G8_UINT* >( pInput );
			pOutput->u[Vec4::R] = 0;
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = 0;
			pOutput->u[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R8G8_UNORM ):
		{
			const R8G8_UINT* input =
				reinterpret_cast< const R8G8_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm8_to_float32( input->R() );
			pOutput->f[Vec4::G] = unorm8_to_float32( input->G() );
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R8G8_SNORM ):
		{
			const R8G8_SINT* input =
				reinterpret_cast< const R8G8_SINT* >( pInput );
			pOutput->f[Vec4::R] = snorm8_to_float32( input->R() );
			pOutput->f[Vec4::G] = snorm8_to_float32( input->G() );
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R8G8_SINT ):
		{
			const R8G8_SINT* input =
				reinterpret_cast< const R8G8_SINT* >( pInput );
			pOutput->i[Vec4::R] = input->R();
			pOutput->i[Vec4::G] = input->G();
			pOutput->i[Vec4::B] = 0;
			pOutput->i[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R8G8_UINT ):
		{
			const R8G8_UINT* input =
				reinterpret_cast< const R8G8_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = input->G();
			pOutput->u[Vec4::B] = 0,
				pOutput->u[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R16_UNORM ):
		{
			const R16_UINT* input =
				reinterpret_cast< const R16_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm16_to_float32( input->R() );
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R16_SNORM ):
		{
			const R16_SINT* input =
				reinterpret_cast< const R16_SINT* >( pInput );
			pOutput->f[Vec4::R] = snorm16_to_float32( input->R() );
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R16_SINT ):
		{
			const R16_SINT* input =
				reinterpret_cast< const R16_SINT* >( pInput );
			pOutput->i[Vec4::R] = input->R();
			pOutput->i[Vec4::G] = 0;
			pOutput->i[Vec4::B] = 0;
			pOutput->i[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R16_UINT ):
		{
			const R16_UINT* input =
				reinterpret_cast< const R16_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = 0;
			pOutput->u[Vec4::B] = 0;
			pOutput->u[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R16_FLOAT ):
		{
			const R16_FLOAT* input =
				reinterpret_cast< const R16_FLOAT* >( pInput );
			pOutput->u[Vec4::R] = input->R().toFloat32_AsUINT32Bits(); // prevents SNaN->QNaN quash during float copy (preserves bits)
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R8_UNORM ):
		{
			const R8_UINT* input =
				reinterpret_cast< const R8_UINT* >( pInput );
			pOutput->f[Vec4::R] = unorm8_to_float32( input->R() );
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R8_SNORM ):
		{
			const R8_SINT* input =
				reinterpret_cast< const R8_SINT* >( pInput );
			pOutput->f[Vec4::R] = snorm8_to_float32( input->R() );
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R8_SINT ):
		{
			const R8_SINT* input =
				reinterpret_cast< const R8_SINT* >( pInput );
			pOutput->i[Vec4::R] = input->R();
			pOutput->i[Vec4::G] = 0;
			pOutput->i[Vec4::B] = 0;
			pOutput->i[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_R8_UINT ):
		{
			const R8_UINT* input =
				reinterpret_cast< const R8_UINT* >( pInput );
			pOutput->u[Vec4::R] = input->R();
			pOutput->u[Vec4::G] = 0;
			pOutput->u[Vec4::B] = 0;
			pOutput->u[Vec4::A] = 0x00000001;
			break;
		}
	case( DXGI_FORMAT_A8_UNORM ):
		{
			const A8_UINT* input =
				reinterpret_cast< const A8_UINT* >( pInput );
			pOutput->f[Vec4::R] = 0.0f;
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = unorm8_to_float32( input->A() );
			break;
		}
	case( DXGI_FORMAT_R9G9B9E5_SHAREDEXP ):
		{
			const R9G9B9E5_UINT* input = 
				reinterpret_cast< const R9G9B9E5_UINT* >( pInput );
			float scale = exp(sub(uint32_to_float32(input->E()), 15.0f));
			pOutput->f[Vec4::R] = mul(frc9_to_float32(input->R()), scale);
			pOutput->f[Vec4::G] = mul(frc9_to_float32(input->G()), scale);
			pOutput->f[Vec4::B] = mul(frc9_to_float32(input->B()), scale);
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
/*	case( DXGI_FORMAT_BC1_UNORM ):
		{
			const BC1_UINT<true>* input =
				reinterpret_cast< const BC1_UINT<true>* >( pInput );
			pOutput->f[Vec4::R] = unorm8_to_float32( input->R(uOffset) );
			pOutput->f[Vec4::G] = unorm8_to_float32( input->G(uOffset) );
			pOutput->f[Vec4::B] = unorm8_to_float32( input->B(uOffset) );
			pOutput->f[Vec4::A] = unorm8_to_float32( input->A(uOffset) );
			break;
		}
	case( DXGI_FORMAT_BC1_UNORM_SRGB ):
		{
			const BC1_UINT<true>* input =
				reinterpret_cast< const BC1_UINT<true>* >( pInput );
			pOutput->f[Vec4::R] = unorm8_srgb_to_float32( input->R(uOffset) );
			pOutput->f[Vec4::G] = unorm8_srgb_to_float32( input->G(uOffset) );
			pOutput->f[Vec4::B] = unorm8_srgb_to_float32( input->B(uOffset) );
			pOutput->f[Vec4::A] = unorm8_to_float32( input->A(uOffset) );
			break;
		}
	case( DXGI_FORMAT_BC2_UNORM ):
		{
			const BC1_UINT<false>* input =
				reinterpret_cast< const BC1_UINT<false>* >( (BYTE*)pInput + 8);
			pOutput->f[Vec4::R] = unorm8_to_float32( input->R(uOffset) );
			pOutput->f[Vec4::G] = unorm8_to_float32( input->G(uOffset) );
			pOutput->f[Vec4::B] = unorm8_to_float32( input->B(uOffset) );

			const BC2_ALPHA* alphainput = 
				reinterpret_cast< const BC2_ALPHA* >( pInput );
			pOutput->f[Vec4::A] = unorm8_to_float32( alphainput->A(uOffset) );
			break;
		}
	case( DXGI_FORMAT_BC2_UNORM_SRGB ):
		{
			const BC1_UINT<false>* input =
				reinterpret_cast< const BC1_UINT<false>* >( (BYTE*)pInput + 8);
			pOutput->f[Vec4::R] = unorm8_srgb_to_float32( input->R(uOffset) );
			pOutput->f[Vec4::G] = unorm8_srgb_to_float32( input->G(uOffset) );
			pOutput->f[Vec4::B] = unorm8_srgb_to_float32( input->B(uOffset) );

			const BC2_ALPHA* alphainput = 
				reinterpret_cast< const BC2_ALPHA* >( pInput );
			pOutput->f[Vec4::A] = unorm8_to_float32( alphainput->A(uOffset) );
			break;
		}
	case( DXGI_FORMAT_BC3_UNORM ):
		{
			const BC1_UINT<false>* input =
				reinterpret_cast< const BC1_UINT<false>* >( (BYTE*)pInput + 8);
			pOutput->f[Vec4::R] = unorm8_to_float32( input->R(uOffset) );
			pOutput->f[Vec4::G] = unorm8_to_float32( input->G(uOffset) );
			pOutput->f[Vec4::B] = unorm8_to_float32( input->B(uOffset) );

			const BC4_UNORM* alphainput = 
				reinterpret_cast< const BC4_UNORM* >( pInput );
			pOutput->f[Vec4::A] = alphainput->R(uOffset);
			break;
		}
	case( DXGI_FORMAT_BC3_UNORM_SRGB ):
		{
			const BC1_UINT<false>* input =
				reinterpret_cast< const BC1_UINT<false>* >( (BYTE*)pInput + 8);
			pOutput->f[Vec4::R] = unorm8_srgb_to_float32( input->R(uOffset) );
			pOutput->f[Vec4::G] = unorm8_srgb_to_float32( input->G(uOffset) );
			pOutput->f[Vec4::B] = unorm8_srgb_to_float32( input->B(uOffset) );

			const BC4_UNORM* alphainput = 
				reinterpret_cast< const BC4_UNORM* >( pInput );
			pOutput->f[Vec4::A] = alphainput->R(uOffset);
			break;
		}
	case( DXGI_FORMAT_BC4_UNORM ):
		{
			const BC4_UNORM* input =
				reinterpret_cast< const BC4_UNORM* >( pInput );
			pOutput->f[Vec4::R] = input->R(uOffset);
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_BC4_SNORM ):
		{
			const BC4_SNORM* input =
				reinterpret_cast< const BC4_SNORM* >( pInput );
			pOutput->f[Vec4::R] = input->R(uOffset);
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_BC5_UNORM ):
		{
			const BC4_UNORM* inputR =
				reinterpret_cast< const BC4_UNORM* >( pInput );
			const BC4_UNORM* inputG =
				reinterpret_cast< const BC4_UNORM* >( (BYTE*)pInput + 8);
			pOutput->f[Vec4::R] = inputR->R(uOffset);
			pOutput->f[Vec4::G] = inputG->R(uOffset);
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_BC5_SNORM ):
		{
			const BC4_SNORM* inputR =
				reinterpret_cast< const BC4_SNORM* >( pInput );
			const BC4_SNORM* inputG =
				reinterpret_cast< const BC4_SNORM* >( (BYTE*)pInput + 8);
			pOutput->f[Vec4::R] = inputR->R(uOffset);
			pOutput->f[Vec4::G] = inputG->R(uOffset);
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R8G8_B8G8_UNORM ): 
	case( DXGI_FORMAT_G8R8_G8B8_UNORM ):
		{        
			// Setup shifts
			UINT rbShift = (DataType == DXGI_FORMAT_R8G8_B8G8_UNORM) ? 0 : 8;
			UINT gShift = 8 - rbShift;

			// Decode RGB stream
			UINT16 *pRGB = (UINT16*) pInput;
			pOutput->f[Vec4::G] = unorm8_to_float32((pRGB[0] >> gShift) & 0xff);
			if (uOffset == 0)
			{
				pOutput->f[Vec4::R] = unorm8_to_float32((pRGB[0] >> rbShift) & 0xff);
				pOutput->f[Vec4::B] = unorm8_to_float32((pRGB[1] >> rbShift) & 0xff);                
			}
			else
			{
				pOutput->f[Vec4::R] = unorm8_to_float32((pRGB[-1] >> rbShift) & 0xff);
				pOutput->f[Vec4::B] = unorm8_to_float32((pRGB[0] >> rbShift) & 0xff);                            
			}
			break;
		}*/
	case( DXGI_FORMAT_R1_UNORM ):
		{
			// LSB is leftmost pixel.
			pOutput->f[Vec4::R] = (*((BYTE*)pInput) >> uOffset) & 0x1 ?
				1.0f : 0.0f;
			pOutput->f[Vec4::G] = 0.0f;
			pOutput->f[Vec4::B] = 0.0f;
			pOutput->f[Vec4::A] = 1.0f;
			break;
		}
	case( DXGI_FORMAT_R32G32B32A32_TYPELESS ):
	case( DXGI_FORMAT_R32G32B32_TYPELESS ):
	case( DXGI_FORMAT_R16G16B16A16_TYPELESS ):
	case( DXGI_FORMAT_R32G32_TYPELESS ):
	case( DXGI_FORMAT_R32G8X24_TYPELESS ):
	case( DXGI_FORMAT_R10G10B10A2_TYPELESS ):
	case( DXGI_FORMAT_R8G8B8A8_TYPELESS ):
	case( DXGI_FORMAT_R16G16_TYPELESS ):
	case( DXGI_FORMAT_R32_TYPELESS ):
	case( DXGI_FORMAT_R24G8_TYPELESS ):
	case( DXGI_FORMAT_R8G8_TYPELESS ):
	case( DXGI_FORMAT_R16_TYPELESS ):
	case( DXGI_FORMAT_R8_TYPELESS ):
	case( DXGI_FORMAT_BC1_TYPELESS ):
	case( DXGI_FORMAT_BC2_TYPELESS ):
	case( DXGI_FORMAT_BC3_TYPELESS ):
	case( DXGI_FORMAT_BC4_TYPELESS ):
	case( DXGI_FORMAT_BC5_TYPELESS ):
	case( DXGI_FORMAT_D32_FLOAT_S8X24_UINT ):
	case( DXGI_FORMAT_D32_FLOAT ):
	case( DXGI_FORMAT_D24_UNORM_S8_UINT ):
	case( DXGI_FORMAT_D16_UNORM ):
	case( DXGI_FORMAT_UNKNOWN ):
	default:
		assert( false );
		break;
	}
}

//-----------------------------------------------------------------------------
void
setBits(void* pData,
		int startBit,
		int numBits)
{
	assert(startBit >= 0);
	assert(numBits >= 0);
	unsigned char* pBytes = (unsigned char*)pData;
	pBytes += startBit/8;
	int shift = startBit%8;
	for (int n = 0; n < numBits; ++n)
	{
		*pBytes |= 1 << shift;
		shift += 1;
		if (shift >= 8)
		{
			shift = 0;
			pBytes += 1;
		}
	}
}

//-----------------------------------------------------------------------------
void
writeD3DColor1(void* buffer, 
			   DXGI_FORMAT format, 
			   size_t index, 
			   D3D11_COLOR_WRITE_ENABLE colorChannels)
{
	const UINT32 val_1_r32_f = 0x3f800000;
	const UINT16 val_1_r16_f = 0x3c00;
	const UINT16 val_1_r16_s = 0x7fff;
	const UINT32 val_1_r11g11b10_f = 0x781e03c0;
	const UINT32 val_1_r10g10b10a2_s = 0xde0781e0;

	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(format);
	UINT32* f32Base = (UINT32*)((char*)buffer + index*(pTexFormatInfo->uBitsPerElement/8));
	UINT16* u16Base= (UINT16*)((char*)buffer + index*(pTexFormatInfo->uBitsPerElement/8));
	UINT8* u8Base  = (UINT8*)((char*)buffer + index*(pTexFormatInfo->uBitsPerElement/8));

	ZeroMemory(u8Base, pTexFormatInfo->uBitsPerElement/8);

	if (pTexFormatInfo->dwFlags & FF_UNORM)
	{
		if (colorChannels == D3D11_COLOR_WRITE_ENABLE_ALL)
		{
			memset(u8Base, 0xff, pTexFormatInfo->uBitsPerElement/8);
		}
		else
		{
			int startBits[4];
			startBits[0] = 0;
			startBits[1] = startBits[0] + pTexFormatInfo->pBitsPerComponent[0];
			startBits[2] = startBits[1] + pTexFormatInfo->pBitsPerComponent[1];
			startBits[3] = startBits[2] + pTexFormatInfo->pBitsPerComponent[2];
			if (colorChannels & D3D11_COLOR_WRITE_ENABLE_RED)
				setBits(u8Base, startBits[0], pTexFormatInfo->pBitsPerComponent[0]);
			if (colorChannels & D3D11_COLOR_WRITE_ENABLE_GREEN)
				setBits(u8Base, startBits[1], pTexFormatInfo->pBitsPerComponent[1]);
			if (colorChannels & D3D11_COLOR_WRITE_ENABLE_BLUE)
				setBits(u8Base, startBits[2], pTexFormatInfo->pBitsPerComponent[2]);
			if (colorChannels & D3D11_COLOR_WRITE_ENABLE_ALPHA)
				setBits(u8Base, startBits[3], pTexFormatInfo->pBitsPerComponent[3]);
		}
		return;
	}

	switch (format)
	{
	default:
		assert(!"Invalid format");
		break;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_ALPHA)
			f32Base[3] = val_1_r32_f;
	case DXGI_FORMAT_R32G32B32_FLOAT:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_BLUE)
			f32Base[2] = val_1_r32_f;
	case DXGI_FORMAT_R32G32_FLOAT:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_GREEN)
			f32Base[1] = val_1_r32_f;
	case DXGI_FORMAT_R32_FLOAT:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_RED)
			f32Base[0] = val_1_r32_f;
		break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_ALPHA)
			u16Base[3] = val_1_r16_f;
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_BLUE)
			u16Base[2] = val_1_r16_f;
	case DXGI_FORMAT_R16G16_FLOAT:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_GREEN)
			u16Base[1] = val_1_r16_f;
	case DXGI_FORMAT_R16_FLOAT:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_RED)
			u16Base[0] = val_1_r16_f;
		break;
	case DXGI_FORMAT_R11G11B10_FLOAT:
		f32Base[0] = val_1_r11g11b10_f;
		if (!(colorChannels & D3D11_COLOR_WRITE_ENABLE_RED))
			f32Base[0] &= ~0x7ff;
		if (!(colorChannels & D3D11_COLOR_WRITE_ENABLE_GREEN))
			f32Base[0] &= ~(0x7ff<<11);
		if (!(colorChannels & D3D11_COLOR_WRITE_ENABLE_BLUE))
			f32Base[0] &= ~(0x3ff<<22);
		break;

	case DXGI_FORMAT_R16G16B16A16_SNORM:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_ALPHA)
			u16Base[3] = val_1_r16_s;
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_BLUE)
			u16Base[2] = val_1_r16_s;
	case DXGI_FORMAT_R16G16_SNORM:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_GREEN)
			u16Base[1] = val_1_r16_s;
	case DXGI_FORMAT_R16_SNORM:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_RED)
			u16Base[0] = val_1_r16_s;
		break;
	case DXGI_FORMAT_R8G8B8A8_SNORM:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_ALPHA)
			u8Base[3] = 0x7f;
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_BLUE)
			u8Base[2] = 0x7f;
	case DXGI_FORMAT_R8G8_SNORM:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_GREEN)
			u8Base[1] = 0x7f;
	case DXGI_FORMAT_R8_SNORM:
		if (colorChannels & D3D11_COLOR_WRITE_ENABLE_RED)
			u8Base[0] = 0x7f;
		break;
	}
}

//-----------------------------------------------------------------------------
void
writeD3DColor(void* buffer, 
			  TEST_COLOR color,
			  DXGI_FORMAT format, 
			  size_t index, 
			  D3D11_COLOR_WRITE_ENABLE colorChannels)
{
	if (!(colorChannels&D3D11_COLOR_WRITE_ENABLE_RED))
		color.r = 0;
	if (!(colorChannels&D3D11_COLOR_WRITE_ENABLE_GREEN))
		color.g = 0;
	if (!(colorChannels&D3D11_COLOR_WRITE_ENABLE_BLUE))
		color.b = 0;
	if (!(colorChannels&D3D11_COLOR_WRITE_ENABLE_ALPHA))
		color.a = 0;

	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(format);
	BYTE* pTexel = ((BYTE*)buffer) + index*pTexFormatInfo->uBitsPerElement/8;
	TEST_VEC4F vColor = {
		(float)color.r,
		(float)color.g,
		(float)color.b,
		(float)color.a
	};
	WriteData(&vColor, format, pTexel);
}

//-----------------------------------------------------------------------------
void
readD3DColor(void* buffer, 
			 TEST_COLOR* color,
			 DXGI_FORMAT format, 
			 size_t index)
{
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(format);
	BYTE* pTexel = ((BYTE*)buffer) + index*pTexFormatInfo->uBitsPerElement/8;
	TEST_VEC4F vColor;
	ReadDataWithDefaults( pTexel, 0, format, &vColor );
	color->r = vColor.x;
	color->g = vColor.y;
	color->b = vColor.z;
	color->a = vColor.w;
}

//-----------------------------------------------------------------------------
void
readD3DColor(BYTE* pTexel, 
			 TEST_COLOR* color,
			 DXGI_FORMAT format)
{
	TEST_VEC4F vColor;
	ReadDataWithDefaults( pTexel, 0, format, &vColor );
	color->r = vColor.x;
	color->g = vColor.y;
	color->b = vColor.z;
	color->a = vColor.w;
}

} //Texture_ALU

static bool isFormatDepthCompatible(DXGI_FORMAT input)
{
	switch (input)
	{
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
		return true;
	}
	return false;
}

static DXGI_FORMAT getTypelessFormat(DXGI_FORMAT input)
{
	switch (input)
	{
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R1_UNORM:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	default: 
		input = DXGI_FORMAT_UNKNOWN;
		break;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		input = DXGI_FORMAT_R32G32B32A32_TYPELESS;
		break;
	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		input = DXGI_FORMAT_R32G32B32_TYPELESS;
		break;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
		input = DXGI_FORMAT_R16G16B16A16_TYPELESS;
		break;
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
		input = DXGI_FORMAT_R32G32_TYPELESS;
		break;
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		input = DXGI_FORMAT_R32G8X24_TYPELESS;
		break;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
		input = DXGI_FORMAT_R10G10B10A2_TYPELESS;
		break;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		input = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		break;
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
		input = DXGI_FORMAT_R16G16_TYPELESS;
		break;
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
		input = DXGI_FORMAT_R32_TYPELESS;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		input = DXGI_FORMAT_R24G8_TYPELESS;
		break;
	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
		input = DXGI_FORMAT_R8G8_TYPELESS;
		break;
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
		input = DXGI_FORMAT_R16_TYPELESS;
		break;
	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		input = DXGI_FORMAT_R8_TYPELESS;
		break;
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		input = DXGI_FORMAT_BC1_TYPELESS;
		break;
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		input = DXGI_FORMAT_BC2_TYPELESS;
		break;
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		input = DXGI_FORMAT_BC3_TYPELESS;
		break;
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		input = DXGI_FORMAT_BC4_TYPELESS;
		break;
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		input = DXGI_FORMAT_BC5_TYPELESS;
		break;
	}
	return input;
}

static DXGI_FORMAT getDepthFormat(DXGI_FORMAT input)
{
	switch (input)
	{
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
		input = DXGI_FORMAT_D32_FLOAT;
		break;
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		input = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		input = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
		input = DXGI_FORMAT_D16_UNORM;
		break;
	default:
		break;
	}
	return input;
}

//-----------------------------------------------------------------------------
HRESULT
CFilterConfTest::CreateTextureNoPattern(ID3D11Resource** ppRsc,
										bool bStaging)
{
	assert(m_nDimension >= 1 && m_nDimension <= 4);
	assert(ppRsc != NULL);
	assert(m_nTextureSize[0] > 0 && m_nTextureSize[1] > 0 && m_nTextureSize[2] > 0);

	HRESULT hr = E_FAIL;
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_eFormat);
	switch (m_nDimension)
	{
	default:
		hr = E_INVALIDARG;
		WriteToLog( _T("%s Invalid texture dimension %d"), LOG_HEADER, m_nDimension );
		assert(!"Invalid dimension");
		goto failedCreate;
	case 1:
		{
			D3D11_TEXTURE1D_DESC desc_1d;
			ZeroMemory(&desc_1d, sizeof(desc_1d));
			desc_1d.ArraySize = (m_nArraySize>0)?m_nArraySize:1;
			desc_1d.BindFlags = bStaging?0:D3D11_BIND_SHADER_RESOURCE;
			desc_1d.CPUAccessFlags = bStaging?D3D11_CPU_ACCESS_READ:0;
			desc_1d.Format = m_eFormat;
			desc_1d.MipLevels = m_nNumMips;
			desc_1d.MiscFlags = 0;
			desc_1d.Usage = bStaging?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;
			desc_1d.Width = m_nTextureSize[0];

			ID3D11Texture1D* pTex1d = NULL;
			hr = m_pDevice->CreateTexture1D(&desc_1d, NULL, &pTex1d);
			// all array elements use the same mips
			if (FAILED(hr))
			{
				WriteToLog( _T("%s CreateTexture1D failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
				goto failedCreate;
			}

			*ppRsc = pTex1d;
		}
		break;
	case 2:
		{
			D3D11_TEXTURE2D_DESC desc_2d;
			ZeroMemory(&desc_2d, sizeof(desc_2d));
			desc_2d.ArraySize = (m_nArraySize>0)?m_nArraySize:1;
			desc_2d.BindFlags = bStaging?0:D3D11_BIND_SHADER_RESOURCE;
			desc_2d.CPUAccessFlags = bStaging?D3D11_CPU_ACCESS_READ:0;
			desc_2d.Format = m_eFormat;
			desc_2d.MipLevels = m_nNumMips;
			desc_2d.MiscFlags = 0;
			desc_2d.SampleDesc.Count = 1;
			desc_2d.SampleDesc.Quality = 0;
			desc_2d.Usage = bStaging?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;
			desc_2d.Width = m_nTextureSize[0];
			desc_2d.Height = m_nTextureSize[1];

			ID3D11Texture2D* pTex2d = NULL;
			hr = GetFramework()->CreateTexture2DTiledOptional(&desc_2d, NULL, &pTex2d);
			// all array elements use the same mips
			if (FAILED(hr))
			{
				WriteToLog( _T("%s CreateTexture2D failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
				goto failedCreate;
			}

			*ppRsc = pTex2d;
		}
		break;
	case 3:
		{
			D3D11_TEXTURE3D_DESC desc_3d;
			ZeroMemory(&desc_3d, sizeof(desc_3d));
			desc_3d.BindFlags = bStaging?0:D3D11_BIND_SHADER_RESOURCE;
			desc_3d.CPUAccessFlags = bStaging?D3D11_CPU_ACCESS_READ:0;
			desc_3d.Format = m_eFormat;
			desc_3d.MipLevels = m_nNumMips;
			desc_3d.MiscFlags = 0;
			desc_3d.Usage = bStaging?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;
			desc_3d.Width = m_nTextureSize[0];
			desc_3d.Height = m_nTextureSize[1];
			desc_3d.Depth = m_nTextureSize[2];

			ID3D11Texture3D* pTex3d = NULL;
			hr = m_pDevice->CreateTexture3D(&desc_3d, NULL, &pTex3d);
			if (FAILED(hr))
			{
				WriteToLog( _T("%s CreateTexture3D failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
				goto failedCreate;
			}

			*ppRsc = pTex3d;
		}
		break;
	case 4:
		{
			D3D11_TEXTURE2D_DESC desc_cube;
			ZeroMemory(&desc_cube, sizeof(desc_cube));
			desc_cube.ArraySize = 6;
			desc_cube.BindFlags = bStaging?0:D3D11_BIND_SHADER_RESOURCE;
			desc_cube.CPUAccessFlags = bStaging?D3D11_CPU_ACCESS_READ:0;
			desc_cube.Format = m_eFormat;
			desc_cube.MipLevels = m_nNumMips;
			desc_cube.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			desc_cube.SampleDesc.Count = 1;
			desc_cube.SampleDesc.Quality = 0;
			desc_cube.Usage = bStaging?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;
			desc_cube.Width = m_nTextureSize[0];
			desc_cube.Height = m_nTextureSize[0];

			ID3D11Texture2D* pTexCube = NULL;
			hr = GetFramework()->CreateTexture2DTiledOptional(&desc_cube, NULL, &pTexCube);
			if (FAILED(hr))
			{
				WriteToLog( _T("%s CreateTextureCube failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
				goto failedCreate;
			}

			*ppRsc = pTexCube;
		}
		break;
	}

failedCreate:
	return hr;
}

HRESULT
VerifyTexturesMatch(ID3D10Device* pDeviceA, ID3D10Resource* pResourceA,
					ID3D10Device* pDeviceB, ID3D10Resource* pResourceB, 
					int texDimension, UINT arraySize, UINT mipLevels, bool isBC)
{
	HRESULT hr = S_OK;
	ID3D10Texture1D* pTex1d[2] = {NULL,NULL};
	ID3D10Texture2D* pTex2d[2] = {NULL,NULL};
	ID3D10Texture3D* pTex3d[2] = {NULL,NULL};

	ID3D10Device* pDevices[2] = {pDeviceA, pDeviceB};
	ID3D10Resource* pResources[2] = {pResourceA, pResourceB};

	for (UINT arrIdx = 0; arrIdx < arraySize; ++arrIdx)
	{
		int baseDimensions[3] = {1,1,1};
		for (UINT mipNum = 0; mipNum < mipLevels; ++mipNum)
		{
			const UINT subResourceID = arrIdx*mipLevels + mipNum;
			BYTE* pData[2] = {NULL, NULL};
			size_t linePitch[2] = {0, 0};
			size_t facePitch[2] = {0, 0};
			switch(texDimension)
			{
			case 1:
				for (int n = 0; n < 2; ++n)
				{
					hr = pResources[n]->QueryInterface(__uuidof(ID3D10Texture1D), (void**)(&pTex1d[n]));
					if (FAILED(hr))
						goto doneVerify;
					D3D10_TEXTURE1D_DESC tex1d_desc;
					pTex1d[n]->GetDesc(&tex1d_desc);
					baseDimensions[0] = tex1d_desc.Width;
					hr = pTex1d[n]->Map(subResourceID, D3D10_MAP_READ, 0, (void**)(&pData[n]));
					if (FAILED(hr))
						goto doneVerify;
				}
				break;
			case 2:
			case 4:
				for (int n = 0; n < 2; ++n)
				{
					hr = pResources[n]->QueryInterface(__uuidof(ID3D10Texture2D), (void**)(&pTex2d[n]));
					if (FAILED(hr))
						goto doneVerify;
					D3D10_TEXTURE2D_DESC tex2d_desc;
					pTex2d[n]->GetDesc(&tex2d_desc);
					baseDimensions[0] = tex2d_desc.Width;
					baseDimensions[1] = tex2d_desc.Height;
					D3D10_MAPPED_TEXTURE2D tex2d_map;
					hr = pTex2d[n]->Map(subResourceID, D3D10_MAP_READ, 0, &tex2d_map);
					if (FAILED(hr))
						goto doneVerify;
					linePitch[n] = tex2d_map.RowPitch;
					pData[n] = (BYTE*)(tex2d_map.pData);
				}
				break;
			case 3:
				for (int n = 0; n < 2; ++n)
				{
					hr = pResources[n]->QueryInterface(__uuidof(ID3D10Texture3D), (void**)(&pTex3d[n]));
					if (FAILED(hr))
						goto doneVerify;
					D3D10_TEXTURE3D_DESC tex3d_desc;
					pTex3d[n]->GetDesc(&tex3d_desc);
					baseDimensions[0] = tex3d_desc.Width;
					baseDimensions[1] = tex3d_desc.Height;
					baseDimensions[2] = tex3d_desc.Depth;
					D3D10_MAPPED_TEXTURE3D tex3d_map;
					hr = pTex3d[n]->Map(subResourceID, D3D10_MAP_READ, 0, &tex3d_map);
					if (FAILED(hr))
						goto doneVerify;
					linePitch[n] = tex3d_map.RowPitch;
					facePitch[n] = tex3d_map.DepthPitch;
					pData[n] = (BYTE*)(tex3d_map.pData);
				}
				break;
			}
			int mipDimensions[3];
			CFilterConfTest::CalcMipSize(baseDimensions, mipNum, mipDimensions, texDimension);
			size_t mipSize = mipDimensions[0] * mipDimensions[1] * mipDimensions[2];
			for (UINT z = 0; z < (UINT)mipDimensions[2]; ++z)
			{
				for (UINT y = 0; y < (UINT)mipDimensions[1]; ++y)
				{
					size_t dataSize = min(linePitch[0], linePitch[1]);
					size_t offset0 = y*linePitch[0] + z*facePitch[0];
					size_t offset1 = y*linePitch[1] + z*facePitch[1];
					int memResult = memcmp(pData[0], pData[1], dataSize);
					if (memResult != 0)
					{
						if (hr == S_OK)
						{
							GetFramework()->WriteToLogF(_T("First texture content mismatch at: row=%u, slice=%u, mip=%u, array=%u (subresource=%u)"),
								y, z, mipNum, arrIdx, subResourceID);

						}
						hr = E_FAIL;
					}
					if (isBC)
						y += 3;
				}
			}
			for (int n = 0; n < 2; ++n)
			{
				switch(texDimension)
				{
				case 1:
					pTex1d[n]->Unmap(subResourceID);
					SAFE_RELEASE(pTex1d[n]);
					break;
				case 2:
				case 4:
					pTex2d[n]->Unmap(subResourceID);
					SAFE_RELEASE(pTex2d[n]);
					break;
				case 3:
					pTex3d[n]->Unmap(subResourceID);
					SAFE_RELEASE(pTex3d[n]);
					break;
				}
			}
		}
	}

doneVerify:
	return hr;
}

static void getPatternColor(TEST_COLOR* pColor, 
							int x, int y, int z, int mipWHD[3], 
							CFilterConfTest::eTexPattern texPattern, 
							int dimension, bool bHDRFormat, int faceNum = -1)
{
	float redCoeff = (mipWHD[0]<=1)?1:1-((float)x)/(mipWHD[0]-1);
	float greenCoeff = (mipWHD[1]<=1)?1:1-((float)y)/(mipWHD[1]-1);
	if (dimension < 2)
		greenCoeff = 0.5;
	float blueCoeff = (mipWHD[2]<=1)?1:((float)z)/(mipWHD[2]-1);
	if (dimension < 3)
		blueCoeff = 0.5;
	if (dimension == 3)
	{
		redCoeff += blueCoeff * 0.25f;
	}
	float alphaCoeff = (redCoeff+greenCoeff+blueCoeff)/3.0f;
	switch(texPattern)
	{
	case CFilterConfTest::TexPattern_Solid:
		{
			pColor->r = 0.75;
			pColor->g = 0.75;
			pColor->b = 0.75;
			pColor->a = 0.75;
		}
		break;
	case CFilterConfTest::TexPattern_Gradient:
		{
			if (dimension == 4)
			{
				pColor->r = ((float)x+1)/mipWHD[0];
				pColor->g = ((float)y+1)/mipWHD[0];
				pColor->b = (((float)x+y)/2+1)/mipWHD[0];
				pColor->a = pColor->b*(TEST_COLOR::realType)((z/2)*0.125 + 0.625);
			}
			else
			{
				pColor->r = redCoeff;
				pColor->g = greenCoeff;
				pColor->b = blueCoeff;
				pColor->a = alphaCoeff;
			}
		}
		break;
	case CFilterConfTest::TexPattern_Checker:
		{
			int checkerPattern = (y%2+z%2)%2;
			bool bCheckerOn = ((x+checkerPattern)%2 == 0);
			if (dimension == 4)
			{
				float horizGrad = ((float)x+1)/mipWHD[0];
				float vertGrad = ((float)y+1)/mipWHD[0];
				float diagGrad = (((float)x+y)/2+1)/mipWHD[0];
				if (bCheckerOn)
				{
					pColor->r = horizGrad*(TEST_COLOR::realType)((z/2!=0)?1:0.25);
					pColor->g = vertGrad*(TEST_COLOR::realType)((z/2!=1)?1:0.25);
					pColor->b = diagGrad*(TEST_COLOR::realType)((z/2!=2)?1:0.25);
					pColor->a = diagGrad*(TEST_COLOR::realType)((z/2)*0.125 + 0.625);
				}
				else
				{
					pColor->r = horizGrad*(TEST_COLOR::realType)((z/2!=0)?0.5:0.125);
					pColor->g = vertGrad*(TEST_COLOR::realType)((z/2!=1)?0.5:0.125);
					pColor->b = diagGrad*(TEST_COLOR::realType)((z/2!=2)?0.5:0.125);
					pColor->a = diagGrad*(TEST_COLOR::realType)((z/2)*0.125 + 0.125);
				}
			}
			else
			{
				if (bCheckerOn)
				{
					pColor->r = redCoeff;
					pColor->g = greenCoeff;
					pColor->b = blueCoeff;
					pColor->a = alphaCoeff;
				}
				else
				{
					pColor->r = 1-redCoeff;
					pColor->g = 1-greenCoeff;
					pColor->b = 1-blueCoeff;
					pColor->a = 1-alphaCoeff;
				}
			}
		}
		break;
	default:
		assert(!"Unexpected TexPattern value");
		break;
	}
	if (bHDRFormat)
	{
		for (int n = 0; n < 4; ++n)
			pColor->f[n] = (pColor->f[n] * 3) - 1;
	}
}


static void getRandNoiseColor(BYTE* pData, int numBytes)
{
	for (int n = 0; n < numBytes; ++n)
	{
		pData[n] = rand() % 256;
	}
}

static void fixupBC6Block(BYTE* pBlock)
{
	// 32 possible bit patterns for 5 LSB bits, but only 14 established configs
	// allowable LSB patterns (MSB->LSB)
	// 00, 01
	// 00010, 00110, 01010, 01110, 10010, 10110, 11010, 11110
	// 00011, 00111, 01011, 01111
	int bitPattern = rand() % 14;
	switch (bitPattern)
	{
	case 0:
	case 1:
		pBlock[0] = (pBlock[0] & 0xfc) | bitPattern;
		break;
	default:
		{
			BYTE patterns[12] = 
			{
				0x02, 0x06, 0x0a, 0x0e, 0x12, 0x16, 0x1a, 0x1e,
				0x03, 0x07, 0x0b, 0x0f
			};
			pBlock[0] = (pBlock[0] & 0xe0) | patterns[bitPattern-2];
		}
		break;
	}
}

static void fixupBC7Block(BYTE* pBlock)
{
	// only 8 established configs
	// allowable LSB patterns (MSB->LSB)
	// 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000
	int bitPattern = rand() % 8;
	BYTE blankMask = 0xff << (bitPattern + 1); // ie for [2] we get 11111000b = 0xf8
	BYTE patternMask = 0x01 << bitPattern;     // ie for [2] we get 00000100b = 0x04
	pBlock[0] = (pBlock[0] & blankMask) | patternMask;
}

//-----------------------------------------------------------------------------
HRESULT
CFilterConfTest::CreateTexturePattern(ID3D11Resource** ppRsc,
									  eTexPattern texPattern,
									  bool bHDRFormat,
									  bool bMipGen)
{
	assert(m_nDimension >= 1 && m_nDimension <= 4);
	assert(ppRsc != NULL);
	assert(m_nTextureSize[0] > 0 && m_nTextureSize[1] > 0 && m_nTextureSize[2] > 0);

	HRESULT hr = E_FAIL;

	const DXGI_FORMAT tempFormat = m_eFormat;
	DXGI_FORMAT resourceFormat = tempFormat;

	// we want to avoid calling the D3DX compression algorithm for BC6 and BC7 because it's very slow
	// we will generate random noise for these formats and decompress them using D3DX
	const bool bIsBC6
		= (m_eFormat == DXGI_FORMAT_BC6H_TYPELESS)
		||(m_eFormat == DXGI_FORMAT_BC6H_UF16)
		||(m_eFormat == DXGI_FORMAT_BC6H_SF16)
		;
	const bool bIsBC7 
		= (m_eFormat == DXGI_FORMAT_BC7_TYPELESS)
		||(m_eFormat == DXGI_FORMAT_BC7_UNORM)
		||(m_eFormat == DXGI_FORMAT_BC7_UNORM_SRGB)
		;
	const bool bIsBC6Or7 = bIsBC6 || bIsBC7;
	srand( GetFramework()->GetCurrentTestCase() );

	tstring sPath = GetPath().ToString( GetPath().Depth() - 2, 1 );
	bool bUseDepthHelper = (sPath == "Depth");
	FLOAT* pDepthInit = NULL;
	if (bUseDepthHelper)
	{
		size_t sampleCount = 1;
		size_t arraySize = (m_nDimension == 4)?6:(m_nArraySize>0)?m_nArraySize:1;
		assert(m_nDimension != 4 || m_nTextureSize[0] == m_nTextureSize[1]);
		size_t depthSize = arraySize * m_nTextureSize[0] * m_nTextureSize[1] * sampleCount;
		pDepthInit = new FLOAT[depthSize];
	}

	UINT depthFlagOption = 0;
	if (m_nNumMips == 1 && isFormatDepthCompatible(m_eFormat))
	{
		assert(resourceFormat == m_eFormat);
		resourceFormat = getTypelessFormat(m_eFormat);
		depthFlagOption = D3D11_BIND_DEPTH_STENCIL;
	}

	UINT miscFlags = 0;
	if (bMipGen)
	{
		miscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		assert( 0.0f == m_fResourceMinLOD );
	}
	else if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
	{
		miscFlags |= D3D11_RESOURCE_MISC_RESOURCE_CLAMP; // mutually exclusive with D3D11_RESOURCE_MISC_GENERATE_MIPS
	}

	try{

	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(tempFormat);
	switch (m_nDimension)
	{
	default:
		hr = E_INVALIDARG;
		WriteToLog( _T("%s Invalid texture dimension %d"), LOG_HEADER, m_nDimension );
		assert(!"Invalid dimension");
		goto failedCreate;
	case 1:
		{
			D3D11_TEXTURE1D_DESC desc_1d;
			ZeroMemory(&desc_1d, sizeof(desc_1d));
			desc_1d.ArraySize = (m_nArraySize>0)?m_nArraySize:1;
			desc_1d.BindFlags = D3D11_BIND_SHADER_RESOURCE | (bMipGen?D3D11_BIND_RENDER_TARGET:depthFlagOption);
			desc_1d.CPUAccessFlags = 0;
			desc_1d.Format = resourceFormat;
			desc_1d.MipLevels = m_nNumMips;
			desc_1d.MiscFlags = miscFlags;
			desc_1d.Usage = (bMipGen||depthFlagOption)?D3D11_USAGE_DEFAULT:D3D11_USAGE_IMMUTABLE;
			desc_1d.Width = m_nTextureSize[0];

			D3D11_SUBRESOURCE_DATA* initData = new D3D11_SUBRESOURCE_DATA[m_nNumMips*desc_1d.ArraySize];
			ZeroMemory(initData, m_nNumMips*sizeof(D3D11_SUBRESOURCE_DATA));
			UINT texelBlockPitch, texelBlockWidth;
			CFormatHelper::GetBlockSize(tempFormat, &texelBlockWidth, NULL, &texelBlockPitch);

			for (int mipNum = 0; mipNum < m_nNumMips; ++mipNum)
			{
				int mipWHD[3];
				CalcMipSize(m_nTextureSize, mipNum, mipWHD, m_nDimension);
				FLOAT_COLOR* pConvColor = new FLOAT_COLOR[mipWHD[0]];

				int x;
				for (x = 0; x < mipWHD[0]; ++x)
				{
					size_t baseIndex = x;
					float redCoeff = (mipWHD[0]<=1)?1:1-((float)x)/(mipWHD[0]-1);
					float greenCoeff = 0.5;
					float blueCoeff = 0.5;
					float alphaCoeff = (redCoeff+greenCoeff+blueCoeff)/3.0f;
					TEST_COLOR* texData = m_pTexData[mipNum];
					TEST_COLOR* pColor = texData+baseIndex;
					bool bCheckerOn = (x%2 == 0);

					getPatternColor(pColor, x, 0, 0, mipWHD, texPattern, m_nDimension, bHDRFormat);
					for (int c = 0; c < 4; ++c)
						pConvColor[baseIndex].comp[c] = (float)pColor->f[c];
					if (bUseDepthHelper)
					{
						assert(m_nNumMips == 1);
						for (UINT arrayNum = 0; arrayNum < desc_1d.ArraySize; ++arrayNum)
							pDepthInit[x + arrayNum*m_nTextureSize[0]] = (FLOAT)pColor->r;
					}

				}
				const size_t dataPitch = texelBlockPitch * CFormatHelper::GetSizeInBlocks(mipWHD[0], texelBlockWidth);
				char* pData = new char[dataPitch];

				for (UINT arrayNum = 0; arrayNum < desc_1d.ArraySize; ++arrayNum)
				{
					const int arrayOffset = arrayNum*m_nNumMips;
					const int rscIndex = mipNum + arrayOffset;
					initData[rscIndex].pSysMem = pData;
					initData[rscIndex].SysMemPitch = dataPitch;
					initData[rscIndex].SysMemSlicePitch = dataPitch;
				}
				if (bIsBC6Or7)
				{
					WriteToLog( _T("%s BC Tex1D no expected."), LOG_HEADER );
					goto failedCreate;
				}
				else
				{
					if (!CFormatHelper::ConvertPixels( CONV_TO_FORMAT, tempFormat, pConvColor, 1, mipWHD[0], pData, dataPitch ))
					{
						WriteToLog( _T("%s ConvertPixels(TO_FORMAT) failed."), LOG_HEADER );
						goto failedCreate;
					}
				}
				if (!CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, tempFormat, pConvColor, 1, mipWHD[0], pData, dataPitch ))
				{
					WriteToLog( _T("%s ConvertPixels(FROM_FORMAT) failed."), LOG_HEADER );
					goto failedCreate;
				}
				for (x = 0; x < mipWHD[0]; ++x)
				{
					size_t baseIndex = x;
					TEST_COLOR* texData = m_pTexData[mipNum];
					TEST_COLOR* pColor = texData+baseIndex;
					for (int c = 0; c < 4; ++c)
						pColor->f[c] = pConvColor[baseIndex].comp[c];
				}
				delete[] pConvColor;
			}

			ID3D11Texture1D* pTex1d = NULL;
			hr = m_pDevice->CreateTexture1D(&desc_1d, initData, &pTex1d);
			// all array elements use the same mips
			for (int mipNum = 0; mipNum < m_nNumMips; ++mipNum)
				delete[] initData[mipNum].pSysMem;
			delete[] initData;
			if (FAILED(hr))
			{
				WriteToLog( _T("%s CreateTexture1D failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
				goto failedCreate;
			}

			*ppRsc = pTex1d;
		}
		break;
	case 2:
		{
			D3D11_TEXTURE2D_DESC desc_2d;
			ZeroMemory(&desc_2d, sizeof(desc_2d));
			desc_2d.ArraySize = (m_nArraySize>0)?m_nArraySize:1;
			desc_2d.BindFlags = D3D11_BIND_SHADER_RESOURCE | (bMipGen?D3D11_BIND_RENDER_TARGET:depthFlagOption);
			desc_2d.CPUAccessFlags = 0;
			desc_2d.Format = resourceFormat;
			desc_2d.MipLevels = m_nNumMips;
			desc_2d.MiscFlags = miscFlags;
			desc_2d.SampleDesc.Count = 1;
			desc_2d.SampleDesc.Quality = 0;
			desc_2d.Usage = (bMipGen||depthFlagOption)?D3D11_USAGE_DEFAULT:D3D11_USAGE_IMMUTABLE;
			desc_2d.Width = m_nTextureSize[0];
			desc_2d.Height = m_nTextureSize[1];

			D3D11_SUBRESOURCE_DATA* initData = new D3D11_SUBRESOURCE_DATA[m_nNumMips*desc_2d.ArraySize];
			ZeroMemory(initData, m_nNumMips*sizeof(D3D11_SUBRESOURCE_DATA));
			UINT texelBlockPitch, texelBlockWidth, texelBlockHeight;
			CFormatHelper::GetBlockSize(tempFormat, &texelBlockWidth, &texelBlockHeight, &texelBlockPitch);
			for (int mipNum = 0; mipNum < m_nNumMips; ++mipNum)
			{
				int mipWHD[3];
				CalcMipSize(m_nTextureSize, mipNum, mipWHD, m_nDimension);
				FLOAT_COLOR* pConvColor = new FLOAT_COLOR[mipWHD[0] * mipWHD[1]];

				for (int y = 0; y < mipWHD[1]; ++y)
				{
					int checkerPattern = y%2;
					for (int x = 0; x < mipWHD[0]; ++x)
					{
						size_t baseIndex = (x)+y*mipWHD[0];
						float redCoeff = (mipWHD[0]<=1)?1:1-((float)x)/(mipWHD[0]-1);
						float greenCoeff = (mipWHD[1]<=1)?1:1-((float)y)/(mipWHD[1]-1);
						float blueCoeff = 0.5;
						float alphaCoeff = (redCoeff+greenCoeff+blueCoeff)/3.0f;
						TEST_COLOR* texData = m_pTexData[mipNum];
						TEST_COLOR* pColor = texData+baseIndex;
						bool bCheckerOn = ((x+checkerPattern)%2 == 0);

						getPatternColor(pColor, x, y, 0, mipWHD, texPattern, m_nDimension, bHDRFormat);
						for (int c = 0; c < 4; ++c)
							pConvColor[baseIndex].comp[c] = (float)pColor->f[c];
						if (bUseDepthHelper)
						{
							assert(m_nNumMips == 1);
							for (UINT arrayNum = 0; arrayNum < desc_2d.ArraySize; ++arrayNum)
								pDepthInit[x + y*m_nTextureSize[0] + arrayNum*m_nTextureSize[0]*m_nTextureSize[1]] = (FLOAT)pColor->r;
						}

					}
				}
				const size_t dataPitch = texelBlockPitch * CFormatHelper::GetSizeInBlocks(mipWHD[0], texelBlockWidth);
				const size_t dataPitchSlice = dataPitch * CFormatHelper::GetSizeInBlocks(mipWHD[1], texelBlockHeight);
				BYTE* pData = new BYTE[dataPitchSlice];

				for (UINT arrayNum = 0; arrayNum < desc_2d.ArraySize; ++arrayNum)
				{
					const int arrayOffset = arrayNum*m_nNumMips;
					const int rscIndex = mipNum + arrayOffset;
					initData[rscIndex].pSysMem = pData;
					initData[rscIndex].SysMemPitch = dataPitch;
					initData[rscIndex].SysMemSlicePitch = dataPitch;
				}
				if (bIsBC6Or7)
				{
					const UINT TexBlocksWidth = CFormatHelper::GetSizeInBlocks(mipWHD[0], texelBlockWidth);
					const UINT TexBlocksHeight = CFormatHelper::GetSizeInBlocks(mipWHD[1], texelBlockHeight);
					for (UINT y = 0; y < TexBlocksHeight; ++y)
					{
						for (UINT x = 0; x < TexBlocksWidth; ++x)
						{
							BYTE* pBlock = pData + x * texelBlockPitch + y * dataPitch;
							getRandNoiseColor(pBlock, texelBlockPitch);
							if (bIsBC6)
								fixupBC6Block(pBlock);
							if (bIsBC7)
								fixupBC7Block(pBlock);
						}
					}
				}
				else
				{
					if (!CFormatHelper::ConvertPixels( CONV_TO_FORMAT, tempFormat, pConvColor, mipWHD[1], mipWHD[0], pData, dataPitch ))
					{
						WriteToLog( _T("%s ConvertPixels(TO_FORMAT) failed."), LOG_HEADER );
						goto failedCreate;
					}
				}
				if (!CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, tempFormat, pConvColor, mipWHD[1], mipWHD[0], pData, dataPitch ))
				{
					WriteToLog( _T("%s ConvertPixels(FROM_FORMAT) failed."), LOG_HEADER );
					goto failedCreate;
				}
				for (int y = 0; y < mipWHD[1]; ++y)
				{
					for (int x = 0; x < mipWHD[0]; ++x)
					{
						size_t baseIndex = (x)+y*mipWHD[0];
						TEST_COLOR* texData = m_pTexData[mipNum];
						TEST_COLOR* pColor = texData+baseIndex;
						for (int c = 0; c < 4; ++c)
							pColor->f[c] = pConvColor[baseIndex].comp[c];
					}
				}
				delete[] pConvColor;
			}

			ID3D11Texture2D* pTex2d = NULL;
			hr = GetFramework()->CreateTexture2DTiledOptional(&desc_2d, initData, &pTex2d);
			// all array elements use the same mips
			for (int mipNum = 0; mipNum < m_nNumMips; ++mipNum)
				delete[] initData[mipNum].pSysMem;
			delete[] initData;
			if (FAILED(hr))
			{
				WriteToLog( _T("%s CreateTexture2D failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
				goto failedCreate;
			}

			*ppRsc = pTex2d;
		}
		break;
	case 3:
		{
			D3D11_TEXTURE3D_DESC desc_3d;
			ZeroMemory(&desc_3d, sizeof(desc_3d));
			desc_3d.BindFlags = D3D11_BIND_SHADER_RESOURCE | (bMipGen?D3D11_BIND_RENDER_TARGET:0);
			desc_3d.CPUAccessFlags = 0;
			desc_3d.Format = resourceFormat;
			desc_3d.MipLevels = m_nNumMips;
			desc_3d.MiscFlags = miscFlags;
			desc_3d.Usage = bMipGen?D3D11_USAGE_DEFAULT:D3D11_USAGE_IMMUTABLE;
			desc_3d.Width = m_nTextureSize[0];
			desc_3d.Height = m_nTextureSize[1];
			desc_3d.Depth = m_nTextureSize[2];

			D3D11_SUBRESOURCE_DATA* initData = new D3D11_SUBRESOURCE_DATA[m_nNumMips];
			ZeroMemory(initData, m_nNumMips*sizeof(D3D11_SUBRESOURCE_DATA));
			UINT texelBlockPitch, texelBlockWidth, texelBlockHeight;
			CFormatHelper::GetBlockSize(tempFormat, &texelBlockWidth, &texelBlockHeight, &texelBlockPitch);
			for (int mipNum = 0; mipNum < m_nNumMips; ++mipNum)
			{
				int mipWHD[3];
				CalcMipSize(m_nTextureSize, mipNum, mipWHD, m_nDimension);
				const size_t dataPitch = texelBlockPitch * CFormatHelper::GetSizeInBlocks(mipWHD[0], texelBlockWidth);
				const size_t dataPitchSlice = dataPitch * CFormatHelper::GetSizeInBlocks(mipWHD[1], texelBlockHeight);
				BYTE* pData = new BYTE[dataPitchSlice*mipWHD[2]];
				initData[mipNum].pSysMem = pData;
				initData[mipNum].SysMemPitch = dataPitch;
				initData[mipNum].SysMemSlicePitch = dataPitchSlice;

				FLOAT_COLOR* pConvColor = new FLOAT_COLOR[mipWHD[0] * mipWHD[1] * mipWHD[2]];
				int z;
				for (z = 0; z < mipWHD[2]; ++z)
				{
					for (int y = 0; y < mipWHD[1]; ++y)
					{
						int checkerPattern = (y%2+z%2)%2;
						for (int x = 0; x < mipWHD[0]; ++x)
						{
							size_t baseIndex = (x) + mipWHD[0]*(y + mipWHD[1]*z);
							float redCoeff = (mipWHD[0]<=1)?1:1-((float)x)/(mipWHD[0]-1);
							float greenCoeff = (mipWHD[1]<=1)?1:1-((float)y)/(mipWHD[1]-1);
							float blueCoeff = (mipWHD[2]<=1)?1:1-((float)z)/(mipWHD[2]-1);
							float alphaCoeff = (redCoeff+greenCoeff+blueCoeff)/3.0f;
							bool bCheckerOn = ((x+checkerPattern)%2 == 0);
							TEST_COLOR* texData = m_pTexData[mipNum];
							TEST_COLOR* pColor = texData+baseIndex;

							getPatternColor(pColor, x, y, z, mipWHD, texPattern, m_nDimension, bHDRFormat);
							for (int c = 0; c < 4; ++c)
								pConvColor[baseIndex].comp[c] = (float)pColor->f[c];
							assert(!bUseDepthHelper);

						}
					}
				}
				if (bIsBC6Or7)
				{
					const UINT TexBlocksWidth = CFormatHelper::GetSizeInBlocks(mipWHD[0], texelBlockWidth);
					const UINT TexBlocksHeight = CFormatHelper::GetSizeInBlocks(mipWHD[1], texelBlockHeight);
					for (z = 0; z < mipWHD[2]; ++z)
					{
						for (UINT y = 0; y < TexBlocksHeight; ++y)
						{
							for (UINT x = 0; x < TexBlocksWidth; ++x)
							{
								BYTE* pBlock = pData + x * texelBlockPitch + y * dataPitch + z * dataPitchSlice;
								getRandNoiseColor(pBlock, texelBlockPitch);
								if (bIsBC6)
									fixupBC6Block(pBlock);
								if (bIsBC7)
									fixupBC7Block(pBlock);
							}
						}
					}
				}
				else
				{
					if (!CFormatHelper::ConvertPixels( CONV_TO_FORMAT, tempFormat, pConvColor, mipWHD[1], mipWHD[0], mipWHD[2], pData, dataPitch, dataPitchSlice ))
					{
						WriteToLog( _T("%s ConvertPixels(TO_FORMAT) failed."), LOG_HEADER );
						goto failedCreate;
					}
				}
				if (!CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, tempFormat, pConvColor, mipWHD[1], mipWHD[0], mipWHD[2], pData, dataPitch, dataPitchSlice ))
				{
					WriteToLog( _T("%s ConvertPixels(FROM_FORMAT) failed."), LOG_HEADER );
					goto failedCreate;
				}
				for (z = 0; z < mipWHD[2]; ++z)
				{
					for (int y = 0; y < mipWHD[1]; ++y)
					{
						for (int x = 0; x < mipWHD[0]; ++x)
						{
							size_t baseIndex = (x) + mipWHD[0]*(y + mipWHD[1]*z);
							TEST_COLOR* texData = m_pTexData[mipNum];
							TEST_COLOR* pColor = texData+baseIndex;
							for (int c = 0; c < 4; ++c)
								pColor->f[c] = pConvColor[baseIndex].comp[c];
						}
					}
				}
				delete[] pConvColor;
			}

			ID3D11Texture3D* pTex3d = NULL;
			hr = m_pDevice->CreateTexture3D(&desc_3d, initData, &pTex3d);
			for (int mipNum = 0; mipNum < m_nNumMips; ++mipNum)
				delete[] initData[mipNum].pSysMem;
			delete[] initData;
			if (FAILED(hr))
			{
				WriteToLog( _T("%s CreateTexture3D failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
				goto failedCreate;
			}

			*ppRsc = pTex3d;
		}
		break;
	case 4:
		{
			D3D11_TEXTURE2D_DESC desc_cube;
			ZeroMemory(&desc_cube, sizeof(desc_cube));
			desc_cube.ArraySize = 6;
			desc_cube.BindFlags = D3D11_BIND_SHADER_RESOURCE | (bMipGen?D3D11_BIND_RENDER_TARGET:depthFlagOption);
			desc_cube.CPUAccessFlags = 0;
			desc_cube.Format = resourceFormat;
			desc_cube.MipLevels = m_nNumMips;
			desc_cube.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | miscFlags;
			desc_cube.SampleDesc.Count = 1;
			desc_cube.SampleDesc.Quality = 0;
			desc_cube.Usage = (bMipGen||depthFlagOption)?D3D11_USAGE_DEFAULT:D3D11_USAGE_IMMUTABLE;
			desc_cube.Width = m_nTextureSize[0];
			desc_cube.Height = m_nTextureSize[0];


			D3D11_SUBRESOURCE_DATA* initData = new D3D11_SUBRESOURCE_DATA[6*m_nNumMips];
			ZeroMemory(initData, 6*m_nNumMips*sizeof(D3D11_SUBRESOURCE_DATA));
			UINT texelBlockPitch, texelBlockWidth, texelBlockHeight;
			CFormatHelper::GetBlockSize(tempFormat, &texelBlockWidth, &texelBlockHeight, &texelBlockPitch);
			for (int mipNum = 0; mipNum < m_nNumMips; ++mipNum)
			{
				int mipWHD[3];
				CalcMipSize(m_nTextureSize, mipNum, mipWHD, m_nDimension);
				const size_t dataPitch = texelBlockPitch * CFormatHelper::GetSizeInBlocks(mipWHD[0], texelBlockWidth);
				const size_t dataPitchSlice = dataPitch * CFormatHelper::GetSizeInBlocks(mipWHD[0], texelBlockHeight);

				assert(mipWHD[0] == mipWHD[1] && mipWHD[2] == 1);

				for (int z = 0; z < 6; ++z)
				{
					FLOAT_COLOR* pConvColor = new FLOAT_COLOR[mipWHD[0] * mipWHD[1]];
					for (int y = 0; y < mipWHD[0]; ++y)
					{
						int checkerPattern = (y%2+z%2)%2;
						for (int x = 0; x < mipWHD[0]; ++x)
						{
							const size_t baseIndex = (x) + mipWHD[0]*y;
							TEST_COLOR* texData = m_pTexData[mipNum];
							const size_t faceTexels = mipWHD[0]*mipWHD[0];
							D3D11_COLOR_WRITE_ENABLE colorMask;
							switch(z/2)
							{
							case 0:
								colorMask = D3D11_COLOR_WRITE_ENABLE_RED;
								colorMask = (D3D11_COLOR_WRITE_ENABLE)(D3D11_COLOR_WRITE_ENABLE_GREEN|D3D11_COLOR_WRITE_ENABLE_BLUE);
							break;
							case 1:
								colorMask = D3D11_COLOR_WRITE_ENABLE_GREEN;
								colorMask = (D3D11_COLOR_WRITE_ENABLE)(D3D11_COLOR_WRITE_ENABLE_RED|D3D11_COLOR_WRITE_ENABLE_BLUE);
								break;
							case 2:
								colorMask = D3D11_COLOR_WRITE_ENABLE_BLUE;
								colorMask = (D3D11_COLOR_WRITE_ENABLE)(D3D11_COLOR_WRITE_ENABLE_RED|D3D11_COLOR_WRITE_ENABLE_GREEN);
							break;
							}
							colorMask = (D3D11_COLOR_WRITE_ENABLE)(colorMask|D3D11_COLOR_WRITE_ENABLE_ALPHA);

							TEST_COLOR* pColor = texData+baseIndex+z*faceTexels;
							bool bCheckerOn = ((x+checkerPattern)%2 == 0);

							getPatternColor(pColor, x, y, z, mipWHD, texPattern, m_nDimension, bHDRFormat);
							for (int c = 0; c < 4; ++c)
								pConvColor[baseIndex].comp[c] = (float)pColor->f[c];
							if (bUseDepthHelper)
							{
								assert(m_nNumMips == 1);
								for (UINT arrayNum = 0; arrayNum < desc_cube.ArraySize; ++arrayNum)
									pDepthInit[x + y*m_nTextureSize[0] + z*m_nTextureSize[0]*m_nTextureSize[1]] = (FLOAT)pColor->r;
							}

						}
					}
					BYTE* pData = new BYTE[dataPitchSlice];
					initData[m_nNumMips*z+mipNum].pSysMem = pData;
					initData[m_nNumMips*z+mipNum].SysMemPitch = dataPitch;
					initData[m_nNumMips*z+mipNum].SysMemSlicePitch = dataPitchSlice;

					if (bIsBC6Or7)
					{
						const UINT TexBlocksWidth = CFormatHelper::GetSizeInBlocks(mipWHD[0], texelBlockWidth);
						const UINT TexBlocksHeight = CFormatHelper::GetSizeInBlocks(mipWHD[1], texelBlockHeight);
						for (UINT y = 0; y < TexBlocksHeight; ++y)
						{
							for (UINT x = 0; x < TexBlocksWidth; ++x)
							{
								BYTE* pBlock = pData + x * texelBlockPitch + y * dataPitch;
								getRandNoiseColor(pBlock, texelBlockPitch);
								if (bIsBC6)
									fixupBC6Block(pBlock);
								if (bIsBC7)
									fixupBC7Block(pBlock);
							}
						}
					}
					else
					{
						if (!CFormatHelper::ConvertPixels( CONV_TO_FORMAT, tempFormat, pConvColor, mipWHD[1], mipWHD[0], pData, dataPitch ))
						{
							WriteToLog( _T("%s ConvertPixels(TO_FORMAT) failed."), LOG_HEADER );
							goto failedCreate;
						}
					}
					if (!CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, tempFormat, pConvColor, mipWHD[1], mipWHD[0], pData, dataPitch ))
					{
						WriteToLog( _T("%s ConvertPixels(FROM_FORMAT) failed."), LOG_HEADER );
						goto failedCreate;
					}
					for (int y = 0; y < mipWHD[0]; ++y)
					{
						for (int x = 0; x < mipWHD[0]; ++x)
						{
							size_t baseIndex = (x)+y*mipWHD[0];
							TEST_COLOR* texData = m_pTexData[mipNum];
							const size_t faceTexels = mipWHD[0]*mipWHD[0];
							TEST_COLOR* pColor = texData+baseIndex+z*faceTexels;
							for (int c = 0; c < 4; ++c)
								pColor->f[c] = pConvColor[baseIndex].comp[c];
						}
					}
					delete[] pConvColor;
				}
			}

			ID3D11Texture2D* pTexCube = NULL;
			hr = GetFramework()->CreateTexture2DTiledOptional(&desc_cube, initData, &pTexCube);
			for (int mipNum = 0; mipNum < 6*m_nNumMips; ++mipNum)
				delete[] initData[mipNum].pSysMem;
			delete[] initData;
			if (FAILED(hr))
			{
				WriteToLog( _T("%s CreateTextureCube failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
				goto failedCreate;
			}

			*ppRsc = pTexCube;
		}
		break;
	}

	}
	catch(std::bad_alloc)
	{
		hr = E_OUTOFMEMORY;
		goto failedCreate;
	}

	if ((miscFlags & D3D11_RESOURCE_MISC_RESOURCE_CLAMP) && (*ppRsc))
	{
		GetImmediateContext()->SetResourceMinLOD( *ppRsc, m_fResourceMinLOD );
	}
	else
	{
		assert( 0.0f == m_fResourceMinLOD );
	}

	if (bUseDepthHelper)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ZeroStructMem(dsvDesc);
		dsvDesc.Format = getDepthFormat(m_eFormat);
		switch(m_nDimension)
		{
		case 1:
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
			dsvDesc.Texture1D.MipSlice = 0;
			break;
		case 2:
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
			break;
		case 4:
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.ArraySize = 6;
			dsvDesc.Texture2DArray.FirstArraySlice = 0;
			dsvDesc.Texture2DArray.MipSlice = 0;
			break;
		}
		ID3D11DepthStencilView* pDSView = NULL;
		hr = m_pDevice->CreateDepthStencilView(*ppRsc, &dsvDesc, &pDSView);
		if (SUCCEEDED(hr))
		{
			DepthHelper depthHelper;
			depthHelper.Setup(GetFramework(), pDSView);
			depthHelper.Fill(pDepthInit, m_nTextureSize[0], m_nTextureSize[1]);
			depthHelper.Cleanup();
		}
		SAFE_RELEASE(pDSView);
	}

failedCreate:
	delete[] pDepthInit;
	return hr;
}

