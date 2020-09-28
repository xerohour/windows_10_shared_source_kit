2 a )
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
				reinterpret_cast< R8G8B8A8_U