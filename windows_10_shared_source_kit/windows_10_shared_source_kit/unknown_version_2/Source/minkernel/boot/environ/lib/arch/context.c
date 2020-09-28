INT* >( pOutput );
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