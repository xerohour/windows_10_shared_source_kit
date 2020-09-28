
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
#pragma once


//
// Win32 headers
//

#include <windows.h>

#include <dxgi.h>

#include <d3d11.h>

#include <d3d10_1.h>
#include <d3dx10.h>


//
// STL headers
//

#include <map>


//
// Framework headers
//

#include <WGFTestCore.h>
#include <DXGITestFramework.h>
#include "HResultHelp.h"

//
// Project headers
//

#include "D3D10SwapChain.h"

#include "WGFHelpers.h"


class CDeviceWrapper10
{
	public:
		CDeviceWrapper10
		(
			CTest * pInitLoggingGroup
		);

		virtual ~CDeviceWrapper10();


	private:
		friend class CSwapChainWrapper10;
		friend class DXGI_RESULT;

		bool m_bAlternateRender;
		UINT m_nxDivisions;
		UINT m_nyDivisions;
		UINT m_nNumVertices;
		UINT m_nNumIndixes;
		UINT m_nStride;

		bool bReadyForTexRender;
		ID3D10Buffer *m_pVB;
		ID3D10Buffer *m_pIB;
		ID3D10Buffer *m_pVSBuffer;
		ID3D10InputLayout *m_pEL;
		ID3D10PixelShader *m_pPixelShader;
        ID3D10PixelShader *m_pPixelShaderRemoveXR_BIAS;
		ID3D10VertexShader *m_pVertexShader;
		ID3D10SamplerState *m_pSampler;
		ID3D10RasterizerState *m_pRastState;
		ID3D10BlendState *m_pBlendState;
		ID3D10DepthStencilState *m_pDepthState;


	protected:
		CTest * const pLoggingGroup;

		ID3D10Device * pD3DDevice;

		DXGI_MODE_ROTATION m_RotationMode;

		HRESULT m_hrLastError;

	public:
		HRESULT WrapDevice
		(
			ID3D10Device * pDevice
		);


		bool IsReadyForTexRender() {return bReadyForTexRender;}

		HRESULT LoadShader
		(
			LPCSTR ShaderCode,
			__out  void **ppShader,
			LPCSTR pFuncName,
			LPCSTR pProfile,
			__out_opt ID3D10Blob **ppRetainCompiledShader = NULL
		);

		bool CreateTexRenderResources
		(
			LPCSTR VertexShaderModel,
			LPCSTR PixelShaderModel
		);

		void ReleaseTexRenderResources();

		bool RenderTexture
		(
			__in     ID3D10RenderTargetView *pRTView,
			__in     ID3D10ShaderResourceView *pTexSRV,
			__in     const D3D10_VIEWPORT *pD3D10ViewPort,
            __in     BOOL renderXR_BIAS
		);

		bool GetFactory
		(
			__inout     IDXGIFactory **pFactoryPointer
		);

		BOOL GetDevicePtr
		(
			__inout     ID3D10Device **pDevicePointer
		);

		void ReleaseAllObjects();

		inline void SetAlternateRenderPath( bool bAlternateRenderPath )
		{
			m_bAlternateRender = bAlternateRenderPath;
		};

		inline void SetRotationMode( DXGI_MODE_ROTATION rotationMode )
		{
			m_RotationMode = rotationMode;
		};

		HRESULT GetLastError() { return m_hrLastError; }
};


class CSwapChainWrapper10
{
	private:
		UINT nDisplayDeviceCount;


	protected:
		CTest * const pLoggingGroup;
		CDeviceWrapper10 * const pParentD3DDevice;
		IDXGISwapChain *pSwapChain;
		HRESULT hrLastError;

	public:

		CSwapChainWrapper10
		(
			CTest * pInitLoggingGroup,
			CDeviceWrapper10 * pInitParentD3D10Device
		)
		:
			pLoggingGroup( pInitLoggingGroup ),
			pParentD3DDevice( pInitParentD3D10Device ),
			pSwapChain( NULL ),
			hrLastError( S_OK )
		{
			clsGDIData  GDIData;

			nDisplayDeviceCount = GDIData.GetAttachedDisplayDeviceCount();
		};

		virtual ~CSwapChainWrapper10();


	public:
		BOOL  InitializeSwapChain( DXGI_SWAP_CHAIN_DESC *pSwapChainDesc );
		BOOL  InitializeFullscreenSwapChain( DXGI_SWAP_CHAIN_DESC* pSwapChainDesc );

		void  ReleaseAllObjects();

		bool  InitBBFrom2DTexture
		(
			UINT uiBBIndex,
			__in ID3D10ShaderResourceView *pTexSRV,
			LPCSTR VertexShaderModel,
			LPCSTR PixelShaderModel,
			const HelperType::Vector2D<unsigned>* = nullptr
		);

		bool  InitBBViaClear( UINT uiBBIndex );

		BOOL  GetSwapChainPtr( IDXGISwapChain **pSwapChainPointer );
        void  WrapSwapchain(__in IDXGISwapChain *pD3DSwapChain);

		HRESULT GetLastError() { return hrLastError; }
};

class DXGI_RESULT
{
public:
	DXGI_RESULT( CDeviceWrapper10* pdevice ) : m_pDevice ( pdevice ), m_sc ( NULL ), m_hr( S_OK ) {}
	DXGI_RESULT( CDeviceWrapper10* pdevice, HRESULT hr ) : m_pDevice ( pdevice ), m_sc ( NULL ) , m_hr( hr )
	{
		*this = hr;
	}
	DXGI_RESULT( IDXGISwapChain* &sc ) : m_pDevice ( NULL ), m_sc ( sc ), m_hr( S_OK )
	{
		if(m_sc) m_sc->AddRef();
	}
	DXGI_RESULT( IDXGISwapChain* &sc, HRESULT hr ) :m_pDevice ( NULL ), m_sc( sc ) , m_hr( hr )
	{
		if(m_sc) m_sc->AddRef();
		*this = hr;
	}
	DXGI_RESULT( IDXGISwapChain1* &sc1 ) : m_pDevice ( NULL ), m_sc ( NULL ), m_hr( S_OK )
	{
		if(sc1) sc1->QueryInterface( __uuidof( m_sc ), (void**)&m_sc);
	}
	DXGI_RESULT( IDXGISwapChain1* &sc1, HRESULT hr ) :m_pDevice ( NULL ), m_sc( NULL ) , m_hr( hr )
	{
		if(sc1) sc1->QueryInterface( __uuidof( m_sc ), (void**)&m_sc);
	}
	DXGI_RESULT( const DXGI_RESULT &dxgihr ) : m_pDevice( dxgihr.m_pDevice ), m_sc( dxgihr.m_sc )
	{
		if(m_sc) m_sc->AddRef();
		*this = dxgihr.m_hr;
	}
	~DXGI_RESULT()
	{
		SAFE_RELEASE( m_sc );
	}

    bool operator==( HRESULT hr ) const { return m_hr == hr; }
    bool operator==( const DXGI_RESULT &tr ) const { return m_hr == tr.m_hr; }

    DXGI_RESULT &operator=( const HRESULT hr )
    {
        m_hr = hr;
        switch( hr )
        {
        case S_OK:
            break;
		case DXGI_ERROR_DEVICE_REMOVED:
			{
				ID3D10Device* pDevice10 = NULL;
				if( m_pDevice && m_pDevice->GetDevicePtr( &pDevice10 ) )
				{
					if( pDevice10 )
					{
						HRESULT hrDR = pDevice10->GetDeviceRemovedReason();
						if( m_pDevice->pLoggingGroup )
							m_pDevice->pLoggingGroup->WriteToLog( _T( "DeviceRemoved: Reason: %s." ), D3DHResultToString(m_hr).c_str() );
					}
				}
				else if( m_sc && SUCCEEDED( m_sc->GetDevice( __uuidof( ID3D10Device ), (void**)&pDevice10 ) ) )
				{
					if( pDevice10 )
					{
						HRESULT hrDR = pDevice10->GetDeviceRemovedReason();
						CWGFTestFramework* pFramework = ::GetFramework();
						if( pFramework ) pFramework->WriteToLogF( _T( "DeviceRemoved: Reason: %s." ), D3DHResultToString(m_hr).c_str() );
					}
				}
				SAFE_RELEASE( pDevice10 );
				// proceed to print out original result string
			}
        default:
            if( m_pDevice && m_pDevice->pLoggingGroup )
                m_pDevice->pLoggingGroup->WriteToLog( _T( "HRESULT: %s." ), D3DHResultToString(m_hr).c_str() );
            else if( m_sc )
			{
				CWGFTestFramework* pFramework = ::GetFramework();
                if( pFramework ) pFramework->WriteToLogF( _T( "HRESULT: %s." ), D3DHResultToString(m_hr).c_str() );
			}
            break;
        }

        return *this;
    }

	operator HRESULT() { return m_hr; }

private:
    HRESULT m_hr;
	CDeviceWrapper10* m_pDevice;
	IDXGISwapChain* m_sc;
};/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    context.c

Abstract:

    This module implements architecture-independent execution context
    manipulation routines.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "arch.h"

// -------------------------------------------------------------------- Globals

//
// Define storage for the two possible execution contexts.
//

EXECUTION_CONTEXT FirmwareExecutionContext;
EXECUTION_CONTEXT ApplicationExecutionContext;

//
// Define the current execution context for the boot application.
//

PEXECUTION_CONTEXT CurrentExecutionContext;

// ------------------------------------------------------------------ Functions

NTSTATUS
ArchInitializeContexts (
    VOID
    )

/*++

Routine Description:

    This routine initializes the firmware and application execution contexts
    for the boot application.

Arguments:

    None.

Return Value:

    NTSTATUS code.

--*/

{

    NTSTATUS ApplicationStatus;
    NTSTATUS FirmwareStatus;
    ULONG FirmwareSupported;

    //
    // Initialize global execution contexts.
    //

    CurrentExecutionContext = NULL;
    FirmwareExecutionContext.ContextType = ExecutionContextFirmware;
    ApplicationExecutionContext.ContextType = ExecutionContextApplication;

    //
    // If an application execution context is supported, then the
    // application should run in its own context.
    //

    ApplicationStatus = ArchInitializeContext(&ApplicationExecutionContext);
    if (NT_SUCCESS(ApplicationStatus)) {
        CurrentExecutionContext = &ApplicationExecutionContext;
    }

    //
    // Initialize the firmware execution contexts, if supported.
    //

    FirmwareSupported =
        CHECK_FLAG(BlPlatformFlags, FW_FLAGS_EXECUTION_CONTEXT_SUPPORTED);

    if (FirmwareSupported != 0) {
        FirmwareStatus = ArchInitializeContext(&FirmwareExecutionContext);
        if (!NT_SUCCESS(FirmwareStatus)) {
            return FirmwareStatus;
        }

        //
        // If a firmware context is supported but the application context is not
        // supported, then the application should run in the firmware's context.
        //
        // Additionally, ensure that the firmware context is set. Some
        // applications can be launched with their own page tables and GDT/IDT,
        // and with interrupts disabled, but need to run in the firmware's
        // context. This will set them back to the firmare's context.
        //

        if (!NT_SUCCESS(ApplicationStatus)) {
            CurrentExecutionContext = &FirmwareExecutionContext;
        }
    }

    ASSERT(CurrentExecutionContext != NULL);
    ArchSetInitialContext(CurrentExecutionContext);

    return STATUS_SUCCESS;
}

VOID
BlpArchSwitchContext (
    __in EXECUTION_CONTEXT_TYPE ContextType
    )

/*++

Routine Description:

    This routine transitions a boot application to execute within the
    specified execution context. On return, the application will be running
    in the new context.

Arguments:

    ContextType - Supplies the type of the target execution context to switch
        the boot application to.

Return Value:

    None.

--*/

{

    PEXECUTION_CONTEXT NewContext;

    BlpInterfaceEnter(InterfaceArch);
    NewContext = ArchGetContext(ContextType);

    //
    // Switch to the target execution context if the application is not already
    // executing within it.
    //

    if (BlpArchQueryCurrentContextType() != ContextType) {
        ArchSwitchContext(NewContext, CurrentExecutionContext);
        CurrentExecutionContext = NewContext;
    }

    BlpInterfaceExit(InterfaceArch);
    return;
}

EXECUTION_CONTEXT_TYPE
BlpArchQueryCurrentContextType (
    VOID
    )

/*++

Routine Description:

    This routine returns the current execution context type for the boot
    application.

Arguments:

    None.

Return Value:

    EXECUTION_CONTEXT_TYPE - Type of the execution context within which the boot
        application is currently running.

--*/

{

    BlpInterfaceEnter(InterfaceArch);

    ASSERT(CurrentExecutionContext != NULL);

    BlpInterfaceExit(InterfaceArch);
    return CurrentExecutionContext->ContextType;
}

PEXECUTION_CONTEXT
ArchGetContext (
    __in EXECUTION_CONTEXT_TYPE ContextType
    )

/*++

Routine Description:

    This routine returns the execution context of the requested type.

Arguments:

    ContextType - Supplies the type of context the caller is requesting.

Return Value:

    PEXECUTION_CONTEXT - The execution context of the type requested.

--*/

{

    PEXECUTION_CONTEXT Context;

    if (ContextType == ExecutionContextFirmware) {
        Context = &FirmwareExecutionContext;

    } else {
        Context = &ApplicationExecutionContext;
    }

    return Context;
}

PEXECUTION_CONTEXT
ArchGetCurrentContext (
    VOID
    )

/*++

Routine Description:

    This routine returns the current execution context.

Arguments:

    None.

Return Value:

    PEXECUTION_CONTEXT - The current execution context.

--*/

{

    return CurrentExecutionContext;
}

#if defined(_X86_) || defined(_AMD64_)

NTSTATUS
ArchExecuteTransition (
    __in PARCH_TRANSITION_ROUTINE TransitionRoutine,
    __inout PBOOT_APPLICATION_PARAMETERS EntryParameters
    )

/*++

Routine Description:

    This routine contains common X86/AMD64 transition code when launching boot
    application.

Arguments:

    TransitionRoutine - Supplies architecture specific transition routine to
        execute final transition to the boot application being launched.

    EntryParameters - Supplies pointer to the input parameters for the boot
        application.

Return Value:

    Status value returned by the application.

--*/

{

    PBOOT_APPLICATION_RETURN_ARGUMENTS ReturnArguments;

    //
    // Restore processor features enabled by the boot application to their
    // original state.
    //

    ArchRestoreProcessorFeatures();

    //
    // If the debugger is currently connected, stop it. That way the new boot
    // application can start a new session.
    //

    BlBdStop();

    //
    // Call into architecture specific routine to transfer control.
    //

    (*TransitionRoutine)();

    //
    // Resume the boot debugger, if it was formerly connected.
    //

    BlBdStart();

    //
    // Enable processor features being used in the boot environment.
    //

    ArchEnableProcessorFeatures();

    //
    // A boot application's returned the status is contained within the
    // input parameter structure.
    //

    ReturnArguments = Add2Ptr(EntryParameters,
                              EntryParameters->ReturnArgumentOffset);

    if (ReturnArguments->Version >= 1) {
        return ReturnArguments->ReturnStatus;
    }

    return STATUS_SUCCESS;
}

#endif

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.0.10011.0
//
// via
// fxc /Od /Zi /T ps_5_1 /E ShadedObjectPixelShader ShadedObjectPixelShader.hlsl /Fh ShadedObjectPixelShader_5_1.h
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
// TEXCOORD                 0   xyzw        1     NONE   float   xyzw
// TEXCOORD                 1   xyzw        2     NONE   float   xyzw
// COLOR                    0   xyzw        3     NONE   float   xyz 
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Target                0   xyzw        0   TARGET   float   xyzw
//
ps_5_1
dcl_globalFlags refactoringAllowed | skipOptimization
dcl_input_ps linear v1.xyzw
dcl_input_ps linear v2.xyzw
dcl_input_ps linear v3.xyz
dcl_output o0.xyzw
dcl_temps 2
//
// Initial variable locations:
//   v0.x <- Input.Position.x; v0.y <- Input.Position.y; v0.z <- Input.Position.z; v0.w <- Input.Position.w; 
//   v1.x <- Input.WorldPosition.x; v1.y <- Input.WorldPosition.y; v1.z <- Input.WorldPosition.z; v1.w <- Input.WorldPosition.w; 
//   v2.x <- Input.Normal.x; v2.y <- Input.Normal.y; v2.z <- Input.Normal.z; v2.w <- Input.Normal.w; 
//   v3.x <- Input.Color.x; v3.y <- Input.Color.y; v3.z <- Input.Color.z; v3.w <- Input.Color.w; 
//   o0.x <- <ShadedObjectPixelShader return value>.x; o0.y <- <ShadedObjectPixelShader return value>.y; o0.z <- <ShadedObjectPixelShader return value>.z; o0.w <- <ShadedObjectPixelShader return value>.w
//
#line 29 "D:\SiGMa_GRFX_Dev\windows\Test\Graphics\DirectX\DXGI\Conf\SwapChainRenderingViaD3D12\ShadedObjectPixelShader.hlsl"
mov r0.xyzw, l(0,4.000000,-3.000000,1.000000)  // r0.x <- LightPosition.x; r0.y <- LightPosition.y; r0.z <- LightPosition.z; r0.w <- LightPosition.w

#line 32
mov r1.xyzw, -v1.xyzw
add r0.xyzw, r0.xyzw, r1.xyzw
dp4 r1.x, r0.xyzw, r0.xyzw
rsq r1.x, r1.x
mul r0.xyzw, r0.xyzw, r1.xxxx  // r0.x <- NormalizedLightPosition.x; r0.y <- NormalizedLightPosition.y; r0.z <- NormalizedLightPosition.z; r0.w <- NormalizedLightPosition.w

#line 35
dp4 r0.x, r0.xyzw, v2.xyzw
mul r0.xyz, r0.xxxx, v3.xyzx  // r0.x <- Diffuse.x; r0.y <- Diffuse.y; r0.z <- Diffuse.z
mul r0.x, r0.x, l(0.500000)
add r0.x, r0.x, l(0.500000)
itof r0.w, l(1)
min r0.x, r0.w, r0.x  // r0.x <- Diffuse.x
mul r0.y, r0.y, l(0.500000)
add r0.y, r0.y, l(0.500000)
itof r0.w, l(1)
min r0.y, r0.w, r0.y  // r0.y <- Diffuse.y
mul r0.z, r0.z, l(0.500000)
add r0.z, r0.z, l(0.500000)
itof r0.w, l(1)
min r0.z, r0.w, r0.z  // r0.z <- Diffuse.z
mov r0.w, l(1.000000)  // r0.w <- Diffuse.w

#line 42
mov o0.x, r0.x
mov o0.y, r0.y
mov o0.z, r0.z
mov o0.w, r0.w
ret 
// Approximately 26 instruction slots used
#endif

const BYTE g_ShadedObjectPixelShader[] =
{
     68,  88,  66,  67, 125, 132, 
    214, 114, 163, 252, 157,  14, 
     23,  60,  54,   0,  95,  12, 
    158, 225,   1,   0,   0,   0, 
    212,  66,   0,   0,   6,   0, 
      0,   0,  56,   0,   0,   0, 
    172,   0,   0,   0,  56,   1, 
      0,   0, 108,   1,   0,   0, 
     48,   4,   0,   0, 204,   4, 
      0,   0,  82,  68,  69,  70, 
    108,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  60,   0, 
      0,   0,   1,   5, 255, 255, 
      5,   5,   0,   0,  60,   0, 
      0,   0,  19,  19,  68,  37, 
     60,   0,   0,   0,  24,   0, 
      0,   0,  40,   0,   0,   0, 
     40,   0,   0,   0,  36,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  49,  48,  46, 
     48,  46,  49,  48,  48,  49, 
     49,  46,  48,   0,  73,  83, 
     71,  78, 132,   0,   0,   0, 
      4,   0,   0,   0,   8,   0, 
      0,   0, 104,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0, 116,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  15,  15, 
      0,   0, 116,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,  15,  15, 
      0,   0, 125,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,  15,   7, 
      0,   0,  83,  86,  95,  80, 
     79,  83,  73,  84,  73,  79, 
     78,   0,  84,  69,  88,  67, 
     79,  79,  82,  68,   0,  67, 
     79,  76,  79,  82,   0, 171, 
     79,  83,  71,  78,  44,   0, 
      0,   0,   1,   0,   0,   0, 
      8,   0,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  84,  97, 114, 103, 101, 
    116,   0, 171, 171,  83,  72, 
     69,  88, 188,   2,   0,   0, 
     81,   0,   0,   0, 175,   0, 
      0,   0, 106, 136,   0,   1, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   1,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   2,   0,   0,   0, 
     98,  16,   0,   3, 114,  16, 
     16,   0,   3,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    104,   0,   0,   2,   2,   0, 
      0,   0,  54,   0,   0,   8, 
    242,   0,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,  64,   0,   0,  64, 192, 
      0,   0, 128,  63,  54,   0, 
      0,   6, 242,   0,  16,   0, 
      1,   0,   0,   0,  70,  30, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   7, 242,   0,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  17,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     68,   0,   0,   5,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,   6,   0, 
     16,   0,   1,   0,   0,   0, 
     17,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  30,  16,   0, 
      2,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      0,   0,   0,   0,   6,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  18,  16,   0,   3,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,  63, 
      0,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,  63,  43,   0, 
      0,   5, 130,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   1,   0,   0,   0, 
     51,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  63,   0,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,  63, 
     43,   0,   0,   5, 130,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   1,   0, 
      0,   0,  51,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,  63,   0,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  63,  43,   0,   0,   5, 
    130,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      1,   0,   0,   0,  51,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    130,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  54,   0, 
      0,   5,  18,  32,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   5,  34,  32, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
     66,  32,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 130,  32,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 148,   0,   0,   0, 
     26,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  83,  80, 
     68,  66,   0,  62,   0,   0, 
     77, 105,  99, 114, 111, 115, 
    111, 102, 116,  32,  67,  47, 
     67,  43,  43,  32,  77,  83, 
     70,  32,  55,  46,  48,  48, 
     13,  10,  26,  68,  83,   0, 
      0,   0,   0,   2,   0,   0, 
      2,   0,   0,   0,  31,   0, 
      0,   0, 164,   0,   0,   0, 
      0,   0,   0,   0,  30,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 192, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255,  56,   0, 
      0, 128, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 
      5,   0,   0,   0,  32,   0, 
      0,   0,  60,   0,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      6,   0,   0,   0,   5,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 148,  46, 
     49,   1, 183,  58, 173,  85, 
      1,   0,   0,   0, 157,  51, 
     85, 118,  59, 139,  68,  72, 
    169,  58,  73, 252, 185, 147, 
     88, 111,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    220,  81,  51,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     10,   9, 102, 108, 111,  97, 
    116,  52,  32,  78, 111, 114, 
    109,  97, 108,  58,  32,  84, 
     69,  88,  67,  79,  79,  82, 
     68,  49,  59,  13,  10,   9, 
    102, 108, 111,  97, 116,  52, 
     32,  67, 111, 108, 111, 114, 
     58,  32,  67,  79,  76,  79, 
     82,  48,  59,  13,  10, 125, 
     59,  13,  10,  13,  10,  13, 
     10,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  13,  10,  47,  47, 
     32,  83, 104,  97, 100, 101, 
    114,  58,  32,  32,  83, 104, 
     97, 100, 101, 100,  79,  98, 
    106, 101,  99, 116,  80, 105, 
    120, 101, 108,  83, 104,  97, 
    100, 101, 114,  13,  10,  47, 
     47,  13,  10,  47,  47,  32, 
     80, 117, 114, 112, 111, 115, 
    101,  58,  13,  10,  47,  47, 
     32,  67,  97, 108,  99, 108, 
    117,  97, 116, 101, 115,  32, 
    112, 105, 120, 101, 108,  32, 
     99, 111, 108, 111, 114,  32, 
     97, 112, 112, 108, 121, 105, 
    110, 103,  32, 115, 104,  97, 
    100, 105, 110, 103,  32, 116, 
    111,  32,  97, 110,  32, 111, 
     98, 106, 101,  99, 116,  13, 
     10,  47,  47,  32, 117, 115, 
    105, 110, 103,  32,  97,  32, 
    115, 105, 109, 112, 108, 101, 
     32, 119, 114,  97, 112,  45, 
    115, 104,  97, 100, 105, 110, 
    103,  32, 116, 101,  99, 104, 
    110, 105, 113, 117, 101,  46, 
     13,  10,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  13,  10, 102, 
    108, 111,  97, 116,  52,  32, 
     83, 104,  97, 100, 101, 100, 
     79,  98, 106, 101,  99, 116, 
     80, 105, 120, 101, 108,  83, 
    104,  97, 100, 101, 114,  13, 
     10,  40,  13,  10,   9,  86, 
     83,  95,  79,  85,  84,  80, 
     85,  84,  32,  73, 110, 112, 
    117, 116,  13,  10,  41,  32, 
     58,  32,  83,  86,  95,  84, 
     97, 114, 103, 101, 116,  13, 
     10, 123,  13,  10,   9,  47, 
     47,  32,  76, 105, 103, 104, 
    116,  32, 112, 111, 115, 105, 
    116, 105, 111, 110,  32, 105, 
    110,  32, 119, 111, 114, 108, 
    100,  32,  99, 111, 111, 114, 
    100, 105, 110,  97, 116, 101, 
    115,  46,  13,  10,   9, 102, 
    108, 111,  97, 116,  52,  32, 
     76, 105, 103, 104, 116,  80, 
    111, 115, 105, 116, 105, 111, 
    110,  32,  61,  32, 102, 108, 
    111,  97, 116,  52,  40,  32, 
     48,  46,  48,  44,  32,  52, 
     46,  48,  44,  32,  45,  51, 
     46,  48,  44,  32,  49,  46, 
     48,  32,  41,  59,  13,  10, 
     13,  10,   9,  47,  47,  32, 
     78, 111, 114, 109,  97, 108, 
    105, 122, 198,  90,   0,   0, 
    130, 101,   1,   0,  69, 103, 
      0,   0,  30, 194,   2,   0, 
     65,  36,   1,   0,  97, 133, 
      1,   0,   0,  16,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  13, 
     10,  47,  47,  32,  83, 116, 
    114, 117,  99, 116, 117, 114, 
    101,  58,  32,  32,  86,  83, 
     95,  79,  85,  84,  80,  85, 
     84,  13,  10,  47,  47,  13, 
     10,  47,  47,  32,  80, 117, 
    114, 112, 111, 115, 101,  58, 
     13,  10,  47,  47,  32,  86, 
    101, 114, 116, 101, 120,  32, 
    115, 104,  97, 100, 101, 114, 
     32, 111, 117, 116, 112, 117, 
    116,  46,  13,  10,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  13, 
     10, 115, 116, 114, 117,  99, 
    116,  32,  86,  83,  95,  79, 
     85,  84,  80,  85,  84,  13, 
     10, 123,  13,  10,   9, 102, 
    108, 111,  97, 116,  52,  32, 
     80, 111, 115, 105, 116, 105, 
    111, 110,  58,  32,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,  59,  13,  10, 
      9, 102, 108, 111,  97, 116, 
     52,  32,  87, 111, 114, 108, 
    100,  80, 111, 115, 105, 116, 
    105, 111, 110,  58,  32,  84, 
     69,  88,  67,  79,  79,  82, 
     68,  48,  59,  13,  10,   9, 
    102, 108, 111,  97, 116,  52, 
     32,  78, 111, 114, 109,  97, 
    108,  58,  32,  84,  69,  88, 
     67,  79,  79,  82,  68,  49, 
     59,  13,  10,   9, 102, 108, 
    111,  97, 116,  52,  32,  67, 
    111, 108, 111, 114,  58,  32, 
     67,  79,  76,  79,  82,  48, 
     59,  13,  10, 125,  59,  13, 
     10,  13,  10,  13,  10,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     13,  10,  47,  47,  32,  83, 
    104,  97, 100, 101, 114,  58, 
     32,  32,  83, 104,  97, 100, 
    101, 100,  79,  98, 106, 101, 
     99, 116,  80, 105, 120, 101, 
    108,  83, 104,  97, 100, 101, 
    114,  13,  10,  47,  47,  13, 
     10,  47,  47,  32,  80, 117, 
    114, 112, 111, 115, 101,  58, 
     13,  10,  47,  47,  32,  67, 
     97, 108,  99, 108, 117,  97, 
    116, 101, 115,  32, 112, 105, 
    120, 101, 108,  32,  99, 111, 
    108, 111, 114,  32,  97, 112, 
    112, 108, 121, 105, 110, 103, 
     32, 115, 104,  97, 100, 105, 
    110, 103,  32, 116, 111,  32, 
     97, 110,  32, 111,  98, 106, 
    101,  99, 116,  13,  10,  47, 
     47,  32, 117, 115, 105, 110, 
    103,  32,  97,  32, 115, 105, 
    109, 112, 108, 101,  32, 119, 
    114,  97, 112,  45, 115, 104, 
     97, 100, 105, 110, 103,  32, 
    116, 101,  99, 104, 110, 105, 
    113, 117, 101,  46,  13,  10, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  13,  10, 102, 108, 111, 
     97, 116,  52,  32,  83, 104, 
     97, 100, 101, 100,  79,  98, 
    106, 101,  99, 116,  80, 105, 
    120, 101, 108,  83, 104,  97, 
    100, 101, 114,  13,  10,  40, 
     13,  10,   9,  86,  83,  95, 
     79,  85,  84,  80,  85,  84, 
     32,  73, 110, 112, 117, 116, 
     13,  10,  41,  32,  58,  32, 
     83,  86,  95,  84,  97, 114, 
    103, 101, 116,  13,  10, 123, 
     13,  10,   9,  47,  47,  32, 
     76, 105, 103, 104, 116,  32, 
    112, 111, 115, 105, 116, 105, 
    111, 110,  32, 105, 110,  32, 
    119, 111, 114, 108, 100,  32, 
     99, 111, 111, 114, 100, 105, 
    110,  97, 116, 101, 115,  46, 
     13,  10,   9, 102, 108, 111, 
     97, 116,  52,  32,  76, 105, 
    103, 104, 116,  80, 111, 115, 
    105, 116, 105, 111, 110,  32, 
     61,  32, 102, 108, 111,  97, 
    116,  52,  40,  32,  48,  46, 
     48,  44,  32,  52,  46,  48, 
     44,  32,  45,  51,  46,  48, 
     44,  32,  49,  46,  48,  32, 
     41,  59,  13,  10,  13,  10, 
      9,  47,  47,  32,  78, 111, 
    114, 109,  97, 108, 105, 122, 
    101,  32, 116, 104, 101,  32, 
    108, 105, 103, 104, 116,  32, 
    112, 111, 115, 105, 116, 105, 
    111, 110,  46,  13,  10,   9, 
    102, 108, 111,  97, 116,  52, 
     32,  78, 111, 114, 109,  97, 
    108, 105, 122, 101, 100,  76, 
    105, 103, 104, 116,  80, 111, 
    115, 105, 116, 105, 111, 110, 
     32,  61,  32, 110, 111, 114, 
    109,  97, 108, 105, 122, 101, 
     40,  32,  76, 105, 103, 104, 
    116,  80, 111, 115, 105, 116, 
    105, 111, 110,  32,  45,  32, 
     73, 110, 112, 117, 116,  46, 
     87, 111, 114, 108, 100,  80, 
    111, 115, 105, 116, 105, 111, 
    110,  32,  41,  59,  13,  10, 
     13,  10,   9,  47,  47,  32, 
     67,  97, 108,  99, 117, 108, 
     97, 116, 101,  32, 116, 104, 
    101,  32, 111, 117, 116, 112, 
    117, 116,  32,  99, 111, 108, 
    111, 114,  32, 102, 111, 114, 
     32, 116, 104, 101,  32, 112, 
    105, 120, 101, 108,  46,  13, 
     10,   9, 102, 108, 111,  97, 
    116,  52,  32,  68, 105, 102, 
    102, 117, 115, 101,  32,  61, 
     32,  73, 110, 112, 117, 116, 
     46,  67, 111, 108, 111, 114, 
     32,  42,  32, 100, 111, 116, 
     40,  32,  78, 111, 114, 109, 
     97, 108, 105, 122, 101, 100, 
     76, 105, 103, 104, 116,  80, 
    111, 115, 105, 116, 105, 111, 
    110,  44,  32,  73, 110, 112, 
    117, 116,  46,  78, 111, 114, 
    109,  97, 108,  32,  41,  59, 
     13,  10,   9,  68, 105, 102, 
    102, 117, 115, 101,  46, 120, 
     32,  61,  32, 109, 105, 110, 
     40,  32,  46,  53,  32,  42, 
     32,  68, 105, 102, 102, 117, 
    115, 101,  46, 120,  32,  43, 
     32,  46,  53,  44,  32,  49, 
     32,  41,  59,  13,  10,   9, 
     68, 105, 102, 102, 117, 115, 
    101,  46, 121,  32,  61,  32, 
    109, 105, 110,  40,  32,  46, 
     53,  32,  42,  32,  68, 105, 
    102, 102, 117, 115, 101,  46, 
    121,  32,  43,  32,  46,  53, 
     44,  32,  49,  32,  41,  59, 
     13,  10,   9,  68, 105, 102, 
    102, 117, 115, 101,  46, 122, 
     32,  61,  32, 109, 105, 110, 
     40,  32,  46,  53,  32,  42, 
     32,  68, 105, 102, 102, 117, 
    115, 101,  46, 122,  32,  43, 
     32,  46,  53,  44,  32,  49, 
     32,  41,  59,  13,  10,   9, 
     68, 105, 102, 102, 117, 115, 
    101,  46, 119,  32,  61,  32, 
     49,  46, 102,  59,  13,  10, 
     13,  10,   9,  47,  47,  32, 
     82, 101, 116, 117, 114, 110, 
     32, 116, 104, 101,  32, 111, 
    117, 116, 112, 117, 116,  32, 
     99, 111, 108, 111, 114,  46, 
     13,  10,   9, 114, 101, 116, 
    117, 114, 110,  32,  68, 105, 
    102, 102, 117, 115, 101,  59, 
     13,  10, 125,  13,  10,  13, 
     10,  13,  10,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  13,  10, 
     47,  47,  32,  83, 104,  97, 
    100, 101, 114,  58,  32,  32, 
     83, 111, 108, 105, 100,  87, 
    104, 105, 116, 101,  80, 105, 
    120, 101, 108,  83, 104,  97, 
    100, 101, 114,  13,  10,  47, 
     47,  13,  10,  47,  47,  32, 
     80, 117, 114, 112, 111, 115, 
    101,  58,  13,  10,  47,  47, 
     32,  67, 111, 108, 111, 114, 
    115,  32,  97, 108, 108,  32, 
    112, 105, 120, 101, 108, 115, 
     32, 119, 104, 105, 116, 101, 
     46,  13,  10,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  13,  10, 
    102, 108, 111,  97, 116,  52, 
     32,  83, 111, 108, 105, 100, 
     87, 104, 105, 116, 101,  80, 
    105, 120, 101, 108,  83, 104, 
     97, 100, 101, 114,  13,  10, 
     40,  13,  10,   9,  86,  83, 
     95,  79,  85,  84,  80,  85, 
     84,  32,  73, 110, 112, 117, 
    116,  13,  10,  41,  32,  58, 
     32,  83,  86,  95,  84,  97, 
    114, 103, 101, 116,  13,  10, 
    123,  13,  10,   9,  47,  47, 
     32,  71, 101, 110, 101, 114, 
     97, 116, 101,  32,  97,  32, 
    104,  97, 114, 100,  45,  99, 
    111, 100, 101, 100,  32, 111, 
    112,  97, 113, 117, 101,  32, 
    119, 104, 105, 116, 101,  32, 
     99, 111, 108, 111, 114,  32, 
    102, 111, 114,  32, 116, 104, 
    101,  32, 112, 105, 120, 101, 
    108,  46,  13,  10,   9, 102, 
    108, 111,  97, 116,  52,  32, 
     68, 105, 102, 102, 117, 115, 
    101,  32,  61,  32, 102, 108, 
    111,  97, 116,  52,  40,  32, 
     49,  44,  32,  49,  44,  32, 
     49,  44,  32,  49,  32,  41, 
     59,  13,  10,  13,  10,   9, 
     47,  47,  32,  82, 101, 116, 
    117, 114, 110,  32, 116, 104, 
    101,  32, 111, 117, 116, 112, 
    117, 116,  32,  99, 111, 108, 
    111, 114,  46,  13,  10,   9, 
    114, 101, 116, 117, 114, 110, 
     32,  68, 105, 102, 102, 117, 
    115, 101,  59,  13,  10, 125, 
     13,  10,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    254, 239, 254, 239,   1,   0, 
      0,   0,  87,   7,   0,   0, 
      0,  68,  58,  92,  83, 105, 
     71,  77,  97,  95,  71,  82, 
     70,  88,  95,  68, 101, 118, 
     92, 119, 105, 110, 100, 111, 
    119, 115,  92,  84, 101, 115, 
    116,  92,  71, 114,  97, 112, 
    104, 105,  99, 115,  92,  68, 
    105, 114, 101,  99, 116,  88, 
     92,  68,  88,  71,  73,  92, 
     67, 111, 110, 102,  92,  83, 
    119,  97, 112,  67, 104,  97, 
    105, 110,  82, 101, 110, 100, 
    101, 114, 105, 110, 103,  86, 
    105,  97,  68,  51,  68,  49, 
     50,  92,  83, 104,  97, 100, 
    101, 100,  79,  98, 106, 101, 
     99, 116,  80, 105, 120, 101, 
    108,  83, 104,  97, 100, 101, 
    114,  46, 104, 108, 115, 108, 
      0,   0, 100,  58,  92, 115, 
    105, 103, 109,  97,  95, 103, 
    114, 102, 120,  95, 100, 101, 
    118,  92, 119, 105, 110, 100, 
    111, 119, 115,  92, 116, 101, 
    115, 116,  92, 103, 114,  97, 
    112, 104, 105,  99, 115,  92, 
    100, 105, 114, 101,  99, 116, 
    120,  92, 100, 120, 103, 105, 
     92,  99, 111, 110, 102,  92, 
    115, 119,  97, 112,  99, 104, 
     97, 105, 110, 114, 101, 110, 
    100, 101, 114, 105, 110, 103, 
    118, 105,  97, 100,  51, 100, 
     49,  50,  92, 115, 104,  97, 
    100, 101, 100, 111,  98, 106, 
    101,  99, 116, 112, 105, 120, 
    101, 108, 115, 104,  97, 100, 
    101, 114,  46, 104, 108, 115, 
    108,   0,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  13,  10,  47, 
     47,  32,  83, 116, 114, 117, 
     99, 116, 117, 114, 101,  58, 
     32,  32,  86,  83,  95,  79, 
     85,  84,  80,  85,  84,  13, 
     10,  47,  47,  13,  10,  47, 
     47,  32,  80, 117, 114, 112, 
    111, 115, 101,  58,  13,  10, 
     47,  47,  32,  86, 101, 114, 
    116, 101, 120,  32, 115, 104, 
     97, 100, 101, 114,  32, 111, 
    117, 116, 112, 117, 116,  46, 
     13,  10,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  47,  47,  47, 
     47,  47,  47,  13,  10, 115, 
    116, 114, 117,  99, 116,  32, 
     86,  83,  95,  79,  85,  84, 
     80,  85,  84,  13,  10, 123, 
     13,  10,   9, 102, 108, 111, 
     97, 116,  52,  32,  80, 111, 
    115, 105, 116, 105, 111, 110, 
     58,  32,  83,  86,  95,  80, 
     79,  83,  73,  84,  73,  79, 
     78,  59,  13,  10,   9, 102, 
    108, 111,  97, 116,  52,  32, 
     87, 111, 114, 108, 100,  80, 
    111, 115, 105, 116, 105, 111, 
    110,  58,  32,  84,  69,  88, 
     67,  79,  79,  82,  68,  48, 
     59,  13,  27, 226,  48,   1, 
    128,   0,   0,   0, 202, 148, 
    100,   9,  24, 195, 208,   1, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   2,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0, 116,   0,   0,   0, 
     40,   0,   0,   0,  27, 226, 
     48,   1, 234, 121,   0, 128, 
    112,   6,   0,   0,   1,   0, 
      0,   0, 115,   0,   0,   0, 
    116,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,  74,   0,  60,  17, 
     16,   1,   0,   0,   0,   1, 
     10,   0,   0,   0,   0,   0, 
    171,  39,  10,   0,   0,   0, 
      0,   0, 171,  39,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  49,  48,  46, 
     48,  46,  49,  48,  49,  53, 
     53,  46,  48,   0,   0,   0, 
     70,   0,  61,  17,   1, 104, 
    108, 115, 108,  70, 108,  97, 
    103, 115,   0,  48, 120,  53, 
      0, 104, 108, 115, 108,  84, 
     97, 114, 103, 101, 116,   0, 
    112, 115,  95,  53,  95,  49, 
      0, 104, 108, 115, 108,  69, 
    110, 116, 114, 121,   0,  83, 
    104,  97, 100, 101, 100,  79, 
     98, 106, 101,  99, 116,  80, 
    105, 120, 101, 108,  83, 104, 
     97, 100, 101, 114,   0,   0, 
     62,   0,  16,  17,   0,   0, 
      0,   0,  12,   5,   0,   0, 
      0,   0,   0,   0, 120,   2, 
      0,   0,   0,   0,   0,   0, 
    120,   2,   0,   0,   5,  16, 
      0,   0,  68,   0,   0,   0, 
      1,   0, 160,  83, 104,  97, 
    100, 101, 100,  79,  98, 106, 
    101,  99, 116,  80, 105, 120, 
    101, 108,  83, 104,  97, 100, 
    101, 114,   0,   0,  46,   0, 
     62,  17,   2,  16,   0,   0, 
      9,   0,  73, 110, 112, 117, 
    116,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
      0,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
      0,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
      4,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
      4,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
      8,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
      8,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     12,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     12,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     16,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     16,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     20,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     20,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     24,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     24,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     28,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     28,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     32,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     32,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     36,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     36,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     40,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     40,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     44,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     44,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     48,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     48,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     52,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     52,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     56,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     56,   0,   0,   0,  22,   0, 
     80,  17,   1,   0,   5,   0, 
     60,   0,   4,   0,  68,   0, 
      0,   0,   1,   0, 120,   2, 
     60,   0,   0,   0,  78,   0, 
     62,  17,   4,  16,   0,   0, 
    136,   0,  60,  83, 104,  97, 
    100, 101, 100,  79,  98, 106, 
    101,  99, 116,  80, 105, 120, 
    101, 108,  83, 104,  97, 100, 
    101, 114,  32, 114, 101, 116, 
    117, 114, 110,  32, 118,  97, 
    108, 117, 101,  62,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     22,   0,  80,  17,   2,   0, 
      5,   0,   0,   0,   4,   0, 
     68,   0,   0,   0,   1,   0, 
    120,   2,   0,   0,   0,   0, 
     22,   0,  80,  17,   2,   0, 
      5,   0,   4,   0,   4,   0, 
     68,   0,   0,   0,   1,   0, 
    120,   2,   4,   0,   0,   0, 
     22,   0,  80,  17,   2,   0, 
      5,   0,   8,   0,   4,   0, 
     68,   0,   0,   0,   1,   0, 
    120,   2,   8,   0,   0,   0, 
     22,   0,  80,  17,   2,   0, 
      5,   0,  12,   0,   4,   0, 
     68,   0,   0,   0,   1,   0, 
    120,   2,  12,   0,   0,   0, 
     54,   0,  62,  17,   0,  16, 
      0,   0,   8,   0,  76, 105, 
    103, 104, 116,  80, 111, 115, 
    105, 116, 105, 111, 110,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  22,   0,  80,  17, 
      0,   0,   5,   0,   0,   0, 
      4,   0, 100,   0,   0,   0, 
      1,   0,  52,   0,   0,   0, 
      0,   0,  22,   0,  80,  17, 
      0,   0,   5,   0,   4,   0, 
      4,   0, 100,   0,   0,   0, 
      1,   0,  52,   0,   4,   0, 
      0,   0,  22,   0,  80,  17, 
      0,   0,   5,   0,   8,   0, 
      4,   0, 100,   0,   0,   0, 
      1,   0,  52,   0,   8,   0, 
      0,   0,  22,   0,  80,  17, 
      0,   0,   5,   0,  12,   0, 
      4,   0, 100,   0,   0,   0, 
      1,   0,  52,   0,  12,   0, 
      0,   0,  62,   0,  62,  17, 
      0,  16,   0,   0,   8,   0, 
     78, 111, 114, 109,  97, 108, 
    105, 122, 101, 100,  76, 105, 
    103, 104, 116,  80, 111, 115, 
    105, 116, 105, 111, 110,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     22,   0,  80,  17,   0,   0, 
      5,   0,   0,   0,   4,   0, 
    228,   0,   0,   0,   1,   0, 
     28,   0,   0,   0,   0,   0, 
     22,   0,  80,  17,   0,   0, 
      5,   0,   4,   0,   4,   0, 
    228,   0,   0,   0,   1,   0, 
     56,   0,   4,   0,   0,   0, 
     22,   0,  80,  17,   0,   0, 
      5,   0,   8,   0,   4,   0, 
    228,   0,   0,   0,   1,   0, 
     56,   0,   8,   0,   0,   0, 
     22,   0,  80,  17,   0,   0, 
      5,   0,  12,   0,   4,   0, 
    228,   0,   0,   0,   1,   0, 
    132,   0,  12,   0,   0,   0, 
     46,   0,  62,  17,   0,  16, 
      0,   0,   8,   0,  68, 105, 
    102, 102, 117, 115, 101,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     26,   0,  80,  17,   0,   0, 
      5,   0,   0,   0,   4,   0, 
     28,   1,   0,   0,   1,   0, 
    160,   1,  28,   0,  76,   0, 
      0,   0,   0,   0,  26,   0, 
     80,  17,   0,   0,   5,   0, 
      4,   0,   4,   0,  28,   1, 
      0,   0,   1,   0, 160,   1, 
    132,   0,  76,   0,   4,   0, 
      0,   0,  26,   0,  80,  17, 
      0,   0,   5,   0,   8,   0, 
      4,   0,  28,   1,   0,   0, 
      1,   0, 160,   1, 236,   0, 
     76,   0,   8,   0,   0,   0, 
     22,   0,  80,  17,   0,   0, 
      5,   0,  12,   0,   4,   0, 
    104,   2,   0,   0,   1,   0, 
     84,   0,  12,   0,   0,   0, 
      2,   0,   6,   0, 244,   0, 
      0,   0,  24,   0,   0,   0, 
      1,   0,   0,   0,  16,   1, 
     43,  17, 204,  59,  60,   8, 
    154,  72, 226, 211,  25, 167, 
     74, 129,  77, 181,   0,   0, 
    242,   0,   0,   0, 136,   2, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   1,   0, 188,   2, 
      0,   0,   0,   0,   0,   0, 
     52,   0,   0,   0, 124,   2, 
      0,   0,  68,   0,   0,   0, 
     29,   0,   0, 128,  68,   0, 
      0,   0,  29,   0,   0,   0, 
    100,   0,   0,   0,  32,   0, 
      0, 128, 100,   0,   0,   0, 
     32,   0,   0,   0, 124,   0, 
      0,   0,  32,   0,   0, 128, 
    124,   0,   0,   0,  32,   0, 
      0,   0, 152,   0,   0,   0, 
     32,   0,   0, 128, 152,   0, 
      0,   0,  32,   0,   0,   0, 
    180,   0,   0,   0,  32,   0, 
      0, 128, 180,   0,   0,   0, 
     32,   0,   0,   0, 200,   0, 
      0,   0,  32,   0,   0, 128, 
    200,   0,   0,   0,  32,   0, 
      0,   0, 228,   0,   0,   0, 
     35,   0,   0, 128, 228,   0, 
      0,   0,  35,   0,   0,   0, 
      0,   1,   0,   0,  35,   0, 
      0, 128,   0,   1,   0,   0, 
     35,   0,   0,   0,  28,   1, 
      0,   0,  36,   0,   0, 128, 
     28,   1,   0,   0,  36,   0, 
      0,   0,  56,   1,   0,   0, 
     36,   0,   0, 128,  56,   1, 
      0,   0,  36,   0,   0,   0, 
     84,   1,   0,   0,  36,   0, 
      0, 128,  84,   1,   0,   0, 
     36,   0,   0,   0, 104,   1, 
      0,   0,  36,   0,   0, 128, 
    104,   1,   0,   0,  36,   0, 
      0,   0, 132,   1,   0,   0, 
     37,   0,   0, 128, 132,   1, 
      0,   0,  37,   0,   0,   0, 
    160,   1,   0,   0,  37,   0, 
      0, 128, 160,   1,   0,   0, 
     37,   0,   0,   0, 188,   1, 
      0,   0,  37,   0,   0, 128, 
    188,   1,   0,   0,  37,   0, 
      0,   0, 208,   1,   0,   0, 
     37,   0,   0, 128, 208,   1, 
      0,   0,  37,   0,   0,   0, 
    236,   1,   0,   0,  38,   0, 
      0, 128, 236,   1,   0,   0, 
     38,   0,   0,   0,   8,   2, 
      0,   0,  38,   0,   0, 128, 
      8,   2,   0,   0,  38,   0, 
      0,   0,  36,   2,   0,   0, 
     38,   0,   0, 128,  36,   2, 
      0,   0,  38,   0,   0,   0, 
     56,   2,   0,   0,  38,   0, 
      0, 128,  56,   2,   0,   0, 
     38,   0,   0,   0,  84,   2, 
      0,   0,  39,   0,   0, 128, 
     84,   2,   0,   0,  39,   0, 
      0,   0, 104,   2,   0,   0, 
     42,   0,   0, 128, 104,   2, 
      0,   0,  42,   0,   0,   0, 
    124,   2,   0,   0,  42,   0, 
      0, 128, 124,   2,   0,   0, 
     42,   0,   0,   0, 144,   2, 
      0,   0,  42,   0,   0, 128, 
    144,   2,   0,   0,  42,   0, 
      0,   0, 164,   2,   0,   0, 
     42,   0,   0, 128, 164,   2, 
      0,   0,  42,   0,   0,   0, 
    184,   2,   0,   0,  42,   0, 
      0, 128, 184,   2,   0,   0, 
     42,   0,   0,   0,   2,   0, 
     54,   0,   9,   0,  53,   0, 
      2,   0,  83,   0,  46,   0, 
     80,   0,   2,   0,  83,   0, 
     46,   0,  80,   0,   2,   0, 
     83,   0,  35,   0,  82,   0, 
      2,   0,  83,   0,  35,   0, 
     82,   0,   2,   0,  83,   0, 
     35,   0,  82,   0,   2,   0, 
     77,   0,  33,   0,  76,   0, 
      2,   0,  77,   0,  19,   0, 
     76,   0,   2,   0,  43,   0, 
     19,   0,  32,   0,   2,   0, 
     43,   0,  19,   0,  37,   0, 
      2,   0,  43,   0,  14,   0, 
     42,   0,   2,   0,  43,   0, 
     14,   0,  42,   0,   2,   0, 
     43,   0,  19,   0,  32,   0, 
      2,   0,  43,   0,  19,   0, 
     37,   0,   2,   0,  43,   0, 
     14,   0,  42,   0,   2,   0, 
     43,   0,  14,   0,  42,   0, 
      2,   0,  43,   0,  19,   0, 
     32,   0,   2,   0,  43,   0, 
     19,   0,  37,   0,   2,   0, 
     43,   0,  14,   0,  42,   0, 
      2,   0,  43,   0,  14,   0, 
     42,   0,   2,   0,  17,   0, 
      2,   0,  16,   0,   2,   0, 
     16,   0,   2,   0,  16,   0, 
      2,   0,  16,   0,   2,   0, 
     16,   0,   2,   0,  16,   0, 
      2,   0,  16,   0,   2,   0, 
     16,   0,   2,   0,  16,   0, 
      2,   0,  16,   0,   2,   0, 
     16,   0, 246,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     11, 202,  49,   1,  56,   0, 
      0,   0,   0,  16,   0,   0, 
      6,  16,   0,   0, 180,   0, 
      0,   0,  10,   0, 255, 255, 
      4,   0,   0,   0, 255, 255, 
      3,   0,   0,   0,   0,   0, 
     24,   0,   0,   0,  24,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,  22,   0,  27,  21, 
     64,   0,   0,   0,   4,   0, 
      0,   0,  16,   0, 102, 108, 
    111,  97, 116,  52,   0, 243, 
    242, 241,  82,   0,   3,  18, 
     13,  21,   3,   0,   0,  16, 
      0,   0,   0,   0,  80, 111, 
    115, 105, 116, 105, 111, 110, 
      0, 241,  13,  21,   3,   0, 
      0,  16,   0,   0,  16,   0, 
     87, 111, 114, 108, 100,  80, 
    111, 115, 105, 116, 105, 111, 
    110,   0,  13,  21,   3,   0, 
      0,  16,   0,   0,  32,   0, 
     78, 111, 114, 109,  97, 108, 
      0, 243, 242, 241,  13,  21, 
      3,   0,   0,  16,   0,   0, 
     48,   0,  67, 111, 108, 111, 
    114,   0,  30,   0,   5,  21, 
      4,   0,   0,   0,   1,  16, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  64,   0, 
     86,  83,  95,  79,  85,  84, 
     80,  85,  84,   0,  10,   0, 
      1,  18,   1,   0,   0,   0, 
      2,  16,   0,   0,  10,   0, 
     24,  21,   0,  16,   0,   0, 
      1,   0,   1,   0,  14,   0, 
      8,  16,   4,  16,   0,   0, 
     23,   0,   1,   0,   3,  16, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  11, 202,  49,   1, 
     56,   0,   0,   0,   0,  16, 
      0,   0,   0,  16,   0,   0, 
      0,   0,   0,   0,  11,   0, 
    255, 255,   4,   0,   0,   0, 
    255, 255,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 101,  32, 
    116, 104, 101,  32, 108, 105, 
    103, 104, 116,  32, 112, 111, 
    115, 105, 116, 105, 111, 110, 
     46,  13,  10,   9, 102, 108, 
    111,  97, 116,  52,  32,  78, 
    111, 114, 109,  97, 108, 105, 
    122, 101, 100,  76, 105, 103, 
    104, 116,  80, 111, 115, 105, 
    116, 105, 111, 110,  32,  61, 
     32, 110, 111, 114, 109,  97, 
    108, 105, 122, 101,  40,  32, 
     76, 105, 103, 104, 116,  80, 
    111, 115, 105, 116, 105, 111, 
    110,  32,  45,  32,  73, 110, 
    112, 117, 116,  46,  87, 111, 
    114, 108, 100,  80, 111, 115, 
    105, 116, 105, 111, 110,  32, 
     41,  59,  13,  10,  13,  10, 
      9,  47,  47,  32,  67,  97, 
    108,  99, 117, 108,  97, 116, 
    101,  32, 116, 104, 101,  32, 
    111, 117, 116, 112, 117, 116, 
     32,  99, 111, 108, 111, 114, 
     32, 102, 111, 114,  32, 116, 
    104, 101,  32, 112, 105, 120, 
    101, 108,  46,  13,  10,   9, 
    102, 108, 111,  97, 116,  52, 
     32,  68, 105, 102, 102, 117, 
    115, 1