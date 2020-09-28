#include <d3d10_1.h> // needed for SRGB conversions
#include <d3dx10.h>  // needed for BC texture format conversion
#include "FormatDesc.hpp"
#include <d3d10wrapped.h>
#include <D3DDepthHelper.h>

#include "GetDispSurfData.hpp"


#define ZeroStructMem(x) ZeroMemory(&x, sizeof(x))

// Andy Hartman - TODO: ideally most of this functionality will be exposed through framework or d3dx

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
unorm2_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 3 ); // 3 = ( 1 << 2 ) - 1
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

} //Texture_ALU
