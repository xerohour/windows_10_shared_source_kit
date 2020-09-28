//////////////////////////////////////////////////////////////////////////
//  LoadStore.cpp
//  created:	2008/05/21
//
//  purpose: Tests load/store instructions with various resource types
//////////////////////////////////////////////////////////////////////////

#include "LoadStore.h"
#include "new_off.h"
#include "d3dx10math.h"
#include "new_on.h"
#include <list>
#include <vector>

using namespace std;

namespace LoadStoreTest
{

static const char szVSCode[] =
	"vs_4_0\n"
	"dcl_input v[0].xyzw\n"
	"dcl_input v[1].xyzw\n"
	"dcl_input v[2].xyzw\n"
	"dcl_output_sv o[0].xyzw, position\n"
	"dcl_output_typed o[1].xyzw, float\n"
	"dcl_output_typed o[2].xyzw, float\n"
	"mov o[0], v[0]\n"
	"mov o[1], v[1]\n"
	"mov o[2], v[2]";

//-----------------------------------------------------------------------------
BEGIN_NAMED_VALUES( BUFFER_TYPE )
      NAMED_VALUE( _T( "BUFFER_TYPED" ), BUFFER_TYPED )
      NAMED_VALUE( _T( "BUFFER_RAW" ), BUFFER_RAW )
	  NAMED_VALUE( _T( "BUFFER_STRUCTURED" ), BUFFER_STRUCTURED )
END_NAMED_VALUES( BUFFER_TYPE )

//-----------------------------------------------------------------------------
BEGIN_NAMED_VALUES( RESOURCE_TYPE )
      NAMED_VALUE( _T( "RESOURCE_BUFFER" ), RESOURCE_BUFFER )
	  NAMED_VALUE( _T( "RESOURCE_TEXTURE1D" ), RESOURCE_TEXTURE1D )
	  NAMED_VALUE( _T( "RESOURCE_TEXTURE1D_ARRAY" ), RESOURCE_TEXTURE1D_ARRAY )
	  NAMED_VALUE( _T( "RESOURCE_TEXTURE2D" ), RESOURCE_TEXTURE2D )
	  NAMED_VALUE( _T( "RESOURCE_TEXTURE2D_ARRAY" ), RESOURCE_TEXTURE2D_ARRAY )
	  NAMED_VALUE( _T( "RESOURCE_TEXTURE3D" ), RESOURCE_TEXTURE3D )
END_NAMED_VALUES( RESOURCE_TYPE )

//-----------------------------------------------------------------------------
BEGIN_NAMED_VALUES( FORMAT_TYPE )
      NAMED_VALUE( _T( "FORMAT_TYPELESS" ), FORMAT_TYPELESS )
	  NAMED_VALUE( _T( "FORMAT_UINT" ), FORMAT_UINT )
	  NAMED_VALUE( _T( "FORMAT_SINT" ), FORMAT_SINT )
	  NAMED_VALUE( _T( "FORMAT_FLOAT" ), FORMAT_FLOAT )
	  NAMED_VALUE( _T( "FORMAT_UNORM" ), FORMAT_UNORM )
	  NAMED_VALUE( _T( "FORMAT_SNORM" ), FORMAT_SNORM )
END_NAMED_VALUES( FORMAT_TYPE )

//-----------------------------------------------------------------------------
BEGIN_NAMED_VALUES( ADDRESS_OFFSET )
	// Offsets from the begining of the addressed space
	NAMED_VALUE( _T( "Start" ), ADDRESS_OFFSET_START )
	NAMED_VALUE( _T( "Start+4Bytes" ), ADDRESS_OFFSET_START_PLUS_4_BYTES )
	NAMED_VALUE( _T( "Start+8Bytes" ), ADDRESS_OFFSET_START_PLUS_8_BYTES )
	NAMED_VALUE( _T( "Start+12Bytes" ), ADDRESS_OFFSET_START_PLUS_12_BYTES )
	NAMED_VALUE( _T( "Start+16Bytes" ), ADDRESS_OFFSET_START_PLUS_16_BYTES )
	NAMED_VALUE( _T( "Start+20Bytes" ), ADDRESS_OFFSET_START_PLUS_20_BYTES )

	// Offsets from the end of the addressed space
	NAMED_VALUE( _T( "End-4Bytes" ), ADDRESS_OFFSET_END_MINUS_4_BYTES )
	NAMED_VALUE( _T( "End-8Bytes" ), ADDRESS_OFFSET_END_MINUS_8_BYTES )
	NAMED_VALUE( _T( "End-12Bytes" ), ADDRESS_OFFSET_END_MINUS_12_BYTES )
	NAMED_VALUE( _T( "End-16Bytes" ), ADDRESS_OFFSET_END_MINUS_16_BYTES )
	NAMED_VALUE( _T( "End-20Bytes" ), ADDRESS_OFFSET_END_MINUS_20_BYTES )
END_NAMED_VALUES( ADDRESS_OFFSET )

//-----------------------------------------------------------------------------
template<class T>
class TFilterRuleLessEx : public CFilterRule<T>
{
private:

	const CTestCaseParameter<T>* m_pParamRef;

public:

	TFilterRuleLessEx( const CTestCaseParameter<T>* pParam1,
		               const CTestCaseParameter<T>* pParam2 ) :
		CFilterRule<T>( pParam1 ), m_pParamRef( pParam2 ) {}

	bool Match() const
	{
		return m_pParam->IsActive() &&
			   m_pParamRef->IsActive() &&
			   *m_pSource < *m_pParamRef->GetTargetPtr();
	}

	tstring ToString() const
	{
		return m_pParam->GetName() + _T( "<" ) + ::ToString( *m_pParamRef->GetTargetPtr() );
	}
};

//-----------------------------------------------------------------------------
class CFilterValidMipSize : public CFilterRule<UINT>
{
private:

	const CTestCaseParameter<UINT>* m_pParamRef;

public:

	CFilterValidMipSize( const CTestCaseParameter<UINT>* pParam1,
		                 const CTestCaseParameter<UINT>* pParam2 ) :
		CFilterRule<UINT>( pParam1 ), m_pParamRef( pParam2 ) {}

	bool Match() const
	{
		return m_pParam->IsActive() &&
			   m_pParamRef->IsActive() &&
			   (UINT)pow( 2.0f, (int)*m_pSource ) <= *m_pParamRef->GetTargetPtr();
	}

	tstring ToString() const
	{
		return m_pParam->GetName() + _T( "isValidMipSizeOf" ) + ::ToString( *m_pParamRef->GetTargetPtr() );
	}
};

//-----------------------------------------------------------------------------
class CFilterRuleSwizzle : public CFilterRule<AssemblyHelper::Swizzle>
{
private:

	UINT m_compIndex;
	AssemblyHelper::MaskFlag m_compValue;

public:

	CFilterRuleSwizzle( const CTestCaseParameter<AssemblyHelper::Swizzle>* pParam,
		                UINT compIndex,
					    AssemblyHelper::MaskFlag compValue ) :
		CFilterRule<AssemblyHelper::Swizzle>( pParam ),
		m_compIndex( compIndex ),
		m_compValue( compValue ) {}

	bool Match() const
	{
		return m_pParam->IsActive() &&
			   ( m_pSource->flags[m_compIndex] == m_compValue );
	}

	tstring ToString() const
	{
		return m_pParam->GetName() + _T( ".swizzle(" ) + ::ToString( m_compIndex ) + _T(") == ") + ::ToString( m_compValue );
	}
};

//-----------------------------------------------------------------------------
template<class T>
CFilterHandle
FilterLess( CTest* pTest, const tstring& strParam1, const tstring& strParam2 )
{
	const CTestCaseParameter<T>* pParam1 =
		dynamic_cast<CTestCaseParameter<T>*>( pTest->GetTestCaseParameter( strParam1 ) );
	if( !pParam1 )
	{
		WriteError( _T( "Error creating filter: parameter '%s' does not exist" ), strParam1.c_str() );
		return CFilterHandle( NULL );
	}

	const CTestCaseParameter<T>* pParam2 =
		dynamic_cast<CTestCaseParameter<T>*>( pTest->GetTestCaseParameter( strParam2 ) );
	if( !pParam2 )
	{
		WriteError( _T( "Error creating filter: parameter '%s' does not exist" ), strParam2.c_str() );
		return CFilterHandle( NULL );
	}

	return new CFilterNode( new TFilterRuleLessEx<T>( pParam1, pParam2 ) );
}

//-----------------------------------------------------------------------------
CFilterHandle
FilterValidMipSize( CTest* pTest, const tstring& strParam1, const tstring& strParam2 )
{
	const CTestCaseParameter<UINT>* pParam1 =
		dynamic_cast<CTestCaseParameter<UINT>*>( pTest->GetTestCaseParameter( strParam1 ) );
	if( !pParam1 )
	{
		WriteError( _T( "Error creating filter: parameter '%s' does not exist" ), strParam1.c_str() );
		return CFilterHandle( NULL );
	}

	const CTestCaseParameter<UINT>* pParam2 =
		dynamic_cast<CTestCaseParameter<UINT>*>( pTest->GetTestCaseParameter( strParam2 ) );
	if( !pParam2 )
	{
		WriteError( _T( "Error creating filter: parameter '%s' does not exist" ), strParam2.c_str() );
		return CFilterHandle( NULL );
	}

	return new CFilterNode( new CFilterValidMipSize( pParam1, pParam2 ) );
}

//-----------------------------------------------------------------------------
CFilterHandle
FilterSwizzle( CTest* pTest,
			   const tstring& strParam,
			   UINT compIndex,
			   AssemblyHelper::MaskFlag compValue )
{
	const CTestCaseParameter<AssemblyHelper::Swizzle>* pParam =
		dynamic_cast<CTestCaseParameter<AssemblyHelper::Swizzle>*>( pTest->GetTestCaseParameter( strParam ) );
	if( !pParam )
	{
		WriteError( _T( "Error creating filter: parameter '%s' does not exist" ), strParam.c_str() );
		return CFilterHandle( NULL );
	}

	return new CFilterNode( new CFilterRuleSwizzle( pParam, compIndex, compValue ) );
}

//-----------------------------------------------------------------------------
tstring
ToAsmName( RESOURCE_TYPE type )
{
	switch ( type )
	{
	case RESOURCE_BUFFER:
		return _T("buffer");

	case RESOURCE_TEXTURE1D:
		return _T("Texture1D");

	case RESOURCE_TEXTURE1D_ARRAY:
		return _T("Texture1DArray");

	case RESOURCE_TEXTURE2D:
		return _T("Texture2D");

	case RESOURCE_TEXTURE2D_ARRAY:
		return _T("Texture2DArray");

	case RESOURCE_TEXTURE3D:
		return _T("Texture3D");

	default:
		return _T("");
	}
};

//-----------------------------------------------------------------------------
tstring
ToAsmName( FORMAT_TYPE formatType )
{
	switch ( formatType )
	{
	case FORMAT_TYPELESS:
		return _T("TYPELESS");

	case FORMAT_UINT:
		return _T("UINT");

	case FORMAT_SINT:
		return _T("SINT");

	case FORMAT_FLOAT:
		return _T("FLOAT");

	case FORMAT_UNORM:
		return _T("UNORM");

	case FORMAT_SNORM:
		return _T("SNORM");

	default:
		return _T("");
	}
}

//-----------------------------------------------------------------------------
tstring
ToString( AssemblyHelper::Swizzle s, UINT numComponents )
{
	AssemblyHelper::AssertSwizzle( s );

	tstring retVal = "";

	if ( numComponents > 0 )
	{
		if ( s.r != AssemblyHelper::noMask )
		{
			retVal += ToString( s.r );
		}
	}

	if ( numComponents > 1 )
	{
		if ( s.g != AssemblyHelper::noMask )
		{
			retVal += ToString( s.g );
		}
	}

	if ( numComponents > 2 )
	{
		if ( s.b != AssemblyHelper::noMask )
		{
			retVal += ToString( s.b );
		}
	}

	if ( numComponents > 3 )
	{
		if ( s.a != AssemblyHelper::noMask )
		{
			retVal += ToString( s.a );
		}
	}

	return retVal;
}

//-----------------------------------------------------------------------------
FORMAT_TYPE
GetFormatType( DXGI_FORMAT format )
{
	switch ( format )
	{
	default:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		return FORMAT_TYPELESS;

	case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R8_UINT:
		return FORMAT_UINT;

    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_R8_SINT:
		return FORMAT_SINT;

	case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R16_FLOAT:
		return FORMAT_FLOAT;

	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_A8_UNORM:
		return FORMAT_UNORM;

    case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R8_SNORM:
		return FORMAT_SNORM;
	}
}

//------------------------------------------------------------------------------
float
ScaleSINT( float fValue, UINT uiBits )
{
	assert( uiBits >= 2 && uiBits <= 32 );
	const UINT uiMask = min( UINT( ( UINT64( 0x1 ) << ( uiBits - 1 ) ) - 1 ), 127 );
	const INT value = (INT)( fValue * uiMask );
	return (float)( value >= 0 ? min( value, INT32( uiMask ) ) : max( value, INT32( ~uiMask ) ) );
}

//------------------------------------------------------------------------------
float
ScaleUINT( float fValue, UINT uiBits )
{
	assert( uiBits >= 1 && uiBits <= 32 );
    const UINT32 uiMask = min( UINT32( ( UINT64( 0x1 ) << uiBits ) - 1 ), 255 );
	const UINT value = (UINT)( fValue * uiMask );
    return (float)min( value, uiMask );
}

//------------------------------------------------------------------------------
void
ScaleIntegerColor( const FLOAT_COLOR& colorIn,
		           DXGI_FORMAT format,
				   FLOAT_COLOR* pColorOut )
{
	assert( pColorOut );

	*pColorOut = colorIn;

	switch ( format )
	{
	case DXGI_FORMAT_R32G32B32A32_UINT:
		pColorOut->comp[0] = ScaleUINT( colorIn.comp[0], 32 );
		pColorOut->comp[1] = ScaleUINT( colorIn.comp[1], 32 );
		pColorOut->comp[2] = ScaleUINT( colorIn.comp[2], 32 );
		pColorOut->comp[3] = ScaleUINT( colorIn.comp[3], 32 );
		break;

    case DXGI_FORMAT_R16G16B16A16_UINT:
		pColorOut->comp[0] = ScaleUINT( colorIn.comp[0], 16 );
		pColorOut->comp[1] = ScaleUINT( colorIn.comp[1], 16 );
		pColorOut->comp[2] = ScaleUINT( colorIn.comp[2], 16 );
		pColorOut->comp[3] = ScaleUINT( colorIn.comp[3], 16 );
		break;

	 case DXGI_FORMAT_R10G10B10A2_UINT:
		pColorOut->comp[0] = ScaleUINT( colorIn.comp[0], 10 );
		pColorOut->comp[1] = ScaleUINT( colorIn.comp[1], 10 );
		pColorOut->comp[2] = ScaleUINT( colorIn.comp[2], 10 );
		pColorOut->comp[3] = ScaleUINT( colorIn.comp[3], 2 );
		break;

    case DXGI_FORMAT_R8G8B8A8_UINT:
		pColorOut->comp[0] = ScaleUINT( colorIn.comp[0], 8 );
		pColorOut->comp[1] = ScaleUINT( colorIn.comp[1], 8 );
		pColorOut->comp[2] = ScaleUINT( colorIn.comp[2], 8 );
		pColorOut->comp[3] = ScaleUINT( colorIn.comp[3], 8 );
		break;

    case DXGI_FORMAT_R32G32_UINT:
		pColorOut->comp[0] = ScaleUINT( colorIn.comp[0], 32 );
		pColorOut->comp[1] = ScaleUINT( colorIn.comp[1], 32 );
		break;

    case DXGI_FORMAT_R16G16_UINT:
		pColorOut->comp[0] = ScaleUINT( colorIn.comp[0], 16 );
		pColorOut->comp[1] = ScaleUINT( colorIn.comp[1], 16 );
		break;

    case DXGI_FORMAT_R8G8_UINT:
		pColorOut->comp[0] = ScaleUINT( colorIn.comp[0], 8 );
		pColorOut->comp[1] = ScaleUINT( colorIn.comp[1], 8 );
		break;

	case DXGI_FORMAT_R32_UINT:
		pColorOut->comp[0] = ScaleUINT( colorIn.comp[0], 32 );
		break;

    case DXGI_FORMAT_R16_UINT:
		pColorOut->comp[0] = ScaleUINT( colorIn.comp[0], 16 );
		break;

    case DXGI_FORMAT_R8_UINT:
		pColorOut->comp[0] = ScaleUINT( colorIn.comp[0], 8 );
		break;

    case DXGI_FORMAT_R32G32B32A32_SINT:
		pColorOut->comp[0] = ScaleSINT( colorIn.comp[0], 32 );
		pColorOut->comp[1] = ScaleSINT( colorIn.comp[1], 32 );
		pColorOut->comp[2] = ScaleSINT( colorIn.comp[2], 32 );
		pColorOut->comp[3] = ScaleSINT( colorIn.comp[3], 32 );
		break;

    case DXGI_FORMAT_R16G16B16A16_SINT:
		pColorOut->comp[0] = ScaleSINT( colorIn.comp[0], 16 );
		pColorOut->comp[1] = ScaleSINT( colorIn.comp[1], 16 );
		pColorOut->comp[2] = ScaleSINT( colorIn.comp[2], 16 );
		pColorOut->comp[3] = ScaleSINT( colorIn.comp[3], 16 );
		break;

	case DXGI_FORMAT_R8G8B8A8_SINT:
		pColorOut->comp[0] = ScaleSINT( colorIn.comp[0], 8 );
		pColorOut->comp[1] = ScaleSINT( colorIn.comp[1], 8 );
		pColorOut->comp[2] = ScaleSINT( colorIn.comp[2], 8 );
		pColorOut->comp[3] = ScaleSINT( colorIn.comp[3], 8 );
		break;

    case DXGI_FORMAT_R32G32_SINT:
		pColorOut->comp[0] = ScaleSINT( colorIn.comp[0], 32 );
		pColorOut->comp[1] = ScaleSINT( colorIn.comp[1], 32 );
		break;

    case DXGI_FORMAT_R16G16_SINT:
		pColorOut->comp[0] = ScaleSINT( colorIn.comp[0], 16 );
		pColorOut->comp[1] = ScaleSINT( colorIn.comp[1], 16 );
		break;

	case DXGI_FORMAT_R8G8_SINT:
		pColorOut->comp[0] = ScaleSINT( colorIn.comp[0], 8 );
		pColorOut->comp[1] = ScaleSINT( colorIn.comp[1], 8 );
		break;

    case DXGI_FORMAT_R32_SINT:
		pColorOut->comp[0] = ScaleSINT( colorIn.comp[0], 32 );
		break;

    case DXGI_FORMAT_R16_SINT:
		pColorOut->comp[0] = ScaleSINT( colorIn.comp[0], 16 );
		break;

    case DXGI_FORMAT_R8_SINT:
		pColorOut->comp[0] = ScaleSINT( colorIn.comp[0], 8 );
		break;
	}
}

//------------------------------------------------------------------------------
void
ColorTypeCast( const FLOAT_COLOR& colorIn,
			   FORMAT_TYPE formatType,
			   AssemblyHelper::MaskFlag	destMask,
			   FLOAT_COLOR* pColorOut )
{
	assert( pColorOut );

	*pColorOut = colorIn;

	switch ( formatType )
	{
	case FORMAT_UINT:
		if ( destMask & AssemblyHelper::x )
		{
			pColorOut->comp[0] = (float)(*(const UINT*)&pColorOut->comp[0]);
		}

		if ( destMask & AssemblyHelper::y )
		{
			pColorOut->comp[1] = (float)(*(const UINT*)&pColorOut->comp[1]);
		}

		if ( destMask & AssemblyHelper::z )
		{
			pColorOut->comp[2] = (float)(*(const UINT*)&pColorOut->comp[2]);
		}

		if ( destMask & AssemblyHelper::w )
		{
			pColorOut->comp[3] = (float)(*(const UINT*)&pColorOut->comp[3]);
		}
		break;

	case FORMAT_SINT:
		if ( destMask & AssemblyHelper::x )
		{
			pColorOut->comp[0] = (float)(*(const INT*)&pColorOut->comp[0]);
		}

		if ( destMask & AssemblyHelper::y )
		{
			pColorOut->comp[1] = (float)(*(const INT*)&pColorOut->comp[1]);
		}

		if ( destMask & AssemblyHelper::z )
		{
			pColorOut->comp[2] = (float)(*(const INT*)&pColorOut->comp[2]);
		}

		if ( destMask & AssemblyHelper::w )
		{
			pColorOut->comp[3] = (float)(*(const INT*)&pColorOut->comp[3]);
		}
		break;
	}
}

//-----------------------------------------------------------------------------
UINT
CalcMipSize( UINT mipLevels,
			 UINT width,
			 UINT height,
			 UINT depth,
			 UINT* pWidth = NULL,
			 UINT* pHeight = NULL,
			 UINT* pDepth = NULL )
{
	UINT uiCbMipSize = 0;

	while ( mipLevels )
	{
		uiCbMipSize += width * height * depth;

		if ( width > 1 )
		{
			width >>= 1;
		}

		if ( height > 1 )
		{
			height >>= 1;
		}

		if ( depth > 1 )
		{
			depth >>= 1;
		}

		--mipLevels;
	}

	if ( pWidth )
	{
		*pWidth = width;
	}

	if ( pHeight )
	{
		*pHeight = height;
	}

	if ( pDepth )
	{
		*pDepth = depth;
	}

	return uiCbMipSize;
}

//-----------------------------------------------------------------------------
CLoadStoreTest::CLoadStoreTest( bool bIsLoadInstruction, BUFFER_TYPE bufType ) :
	CShader5xTest( DXGI_FORMAT_R32G32B32A32_FLOAT ),
	m_testcaseID( 0 ),
	m_bIsLoadInstruction( bIsLoadInstruction ),
	m_bRenderUsingPointlist( false ),
	m_bufType( bufType ),
	m_resourceType( RESOURCE_BUFFER ),
	m_bufStride( 0 ),
	m_resFormat( DXGI_FORMAT_R32G32B32A32_UINT ),
	m_resWidth( 1 ),
	m_resHeight( 1 ),
	m_resDepth( 1 ),
	m_resMipLevels( 1 ),
	m_resMipSlice( 1 ),
	m_resArraySize( 1 ),
	m_resArraySlice( 1 ),
	m_destMask( AssemblyHelper::noMask ),
	m_srcAddressSwizzle( AssemblyHelper::swizX ),
	m_srcOffsetSwizzle( AssemblyHelper::swizX ),
	m_srcSwizzle( AssemblyHelper::swizXYZW ),
	m_srcOffset( ADDRESS_OFFSET_START ),
	m_pBuffer( NULL ),
	m_pBufferCopy( NULL ),
	m_pTexture1D( NULL ),
	m_pTexture1DCopy( NULL ),
	m_pTexture2D( NULL ),
	m_pTexture2DCopy( NULL ),
	m_pTexture3D( NULL ),
	m_pTexture3DCopy( NULL ),
	m_pInputLayout( NULL ),
	m_pVsInput( NULL ),
	m_pCsInput( NULL ),
	m_pCsInputCopy( NULL ),
	m_pCsInputSRV( NULL ),
	m_pSampler( NULL ),
	m_pbGradients( NULL ),
	m_pUAV( NULL ),
	m_UAVSlot( 0 ),
	m_bUseTGSM( false ),
	m_outOfBoundsOffset( 1.0f )
{}

//-----------------------------------------------------------------------------
TEST_RESULT
CLoadStoreTest::Setup()
{
	TEST_RESULT tr;
	HRESULT hr;

	// Execute the parent setup
	tr = CShader5xTest::Setup();
	if ( tr != RESULT_PASS )
	{
		return tr;
	}

	if (SkipTiled())
	{
		WriteToLog(_T("CLoadStoreTest::Setup - Tiled resource ops not supported."));
		return RESULT_SKIP;
	}

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	//
	// Validate API support
	//

	if ( !g_TestApp.IsFL11OrGreater() &&
		 !g_TestApp.IsFL10AndHWSupportsComputeOn10() &&
		 !g_TestApp.UAVIsAllowed() )
	{
		return RESULT_SKIP;
	}

	//
	// Setup viewport
	//

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width    = (float)DISP_WIDTH;
	vp.Height   = (float)DISP_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	pContext->RSSetViewports( 1, &vp );

	//
	// Create default vertex shader
	//

	ID3D10Blob* pShaderBuf = NULL;
	ID3D10Blob* pErrorBuf = NULL;

	if ( !AssembleShader( szVSCode, &pShaderBuf, &pErrorBuf ) )
	{
		if ( pErrorBuf )
		{
			WriteToLog( _T("CShader5xTest::AssembleShader() failed, %s"), pErrorBuf->GetBufferPointer() );
		}
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
		return RESULT_FAIL;
	}

	if ( g_bDumpDissasm )
	{
		this->MessageDumpShaderDisassembly( (UINT*)pShaderBuf->GetBufferPointer(),
			                                pShaderBuf->GetBufferSize() );
	}

	SAFE_RELEASE( m_pPassThroughVS );
	hr = pDevice->CreateVertexShader( pShaderBuf->GetBufferPointer(),
                                      pShaderBuf->GetBufferSize(),
									  NULL,
									  &m_pPassThroughVS );
	if ( FAILED( hr ) )
	{
		WriteToLog( _T("ID3D11Device::CreateVertexShader() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
		return RESULT_FAIL;
	}

	//
	// Create input layout
	//

	// Declare buffer used for input data and setup input assembler
	D3D11_INPUT_ELEMENT_DESC ILDesc[] =
	{
		// These are all really float inputs, but we'll call them uint to avoid debug spew
		{"0_", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0 },	// Position
		{"1_", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },	// Address
		{"2_", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },	// Offset
	};

	hr = pDevice->CreateInputLayout( ILDesc,
                                     ARRAYSIZE( ILDesc ),
	   								 pShaderBuf->GetBufferPointer(),
									 pShaderBuf->GetBufferSize(),
									 &m_pInputLayout );
	if ( FAILED( hr ) )
	{
		WriteToLog( _T("ID3D11Device::CreateInputLayout() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
		return RESULT_FAIL;
	}

	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	//
	// Create vertex buffer
	//

	D3D11_BUFFER_DESC bufDesc;
	memset( &bufDesc, 0, sizeof( D3D11_BUFFER_DESC ) );
	bufDesc.ByteWidth      = MAX_NUM_VERTICES * sizeof( VSINPUT );
	bufDesc.Usage          = D3D11_USAGE_DYNAMIC;
	bufDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.MiscFlags      = 0;

	hr = pDevice->CreateBuffer( &bufDesc, NULL, &m_pVsInput );
	if ( FAILED( hr ) )
	{
		WriteToLog( _T("ID3D11Device::CreateBuffer(), hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create CS input buffer
	//

	memset( &bufDesc, 0, sizeof( bufDesc ) );
	bufDesc.ByteWidth           = MAX_WIDTH * MAX_HEIGHT * sizeof( CSINPUT );
	bufDesc.Usage               = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
	bufDesc.CPUAccessFlags      = 0;
	bufDesc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufDesc.StructureByteStride = sizeof( CSINPUT );

	hr = pDevice->CreateBuffer( &bufDesc, NULL, &m_pCsInput );
	if ( FAILED( hr ) )
	{
		WriteToLog( _T("ID3D11Device::CreateBuffer() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create CS input buffer staging resource for CPU access
	//

	bufDesc.BindFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_STAGING;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = pDevice->CreateBuffer( &bufDesc, NULL, &m_pCsInputCopy );
	if ( FAILED( hr ) )
	{
		WriteToLog( _T("ID3D11Device::CreateBuffer() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create shader resource view
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format              = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.Flags      = 0;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.NumElements = bufDesc.ByteWidth / bufDesc.StructureByteStride;

	hr = pDevice->CreateShaderResourceView( m_pCsInput, &srvDesc, &m_pCsInputSRV );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("CLoadStoreTest::InitBufferRes - CreateShaderResourceView() failed. hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create RenderTarget
	//

	D3D11_TEXTURE2D_DESC tex2DDesc;
	tex2DDesc.ArraySize = 1;
	tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	tex2DDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex2DDesc.Height = DISP_WIDTH;
	tex2DDesc.Width  = DISP_HEIGHT;
	tex2DDesc.CPUAccessFlags = 0;
	tex2DDesc.MipLevels = 1;
	tex2DDesc.MiscFlags = 0;
	tex2DDesc.SampleDesc.Count = 1;
	tex2DDesc.SampleDesc.Quality = 0;
	tex2DDesc.Usage = D3D11_USAGE_DEFAULT;

	if ( g_TestApp.IsFL11OrGreater() )
	{
		tex2DDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}

	SAFE_RELEASE( m_pRTTexture2D );
	hr = pDevice->CreateTexture2D( &tex2DDesc, NULL, &m_pRTTexture2D );
	if ( FAILED( hr ) )
	{
		WriteError( _T("D3D11Device::CreateTexture2D() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create staging RenderTarget for CPU access
	//

	tex2DDesc.BindFlags = 0;
	tex2DDesc.Usage = D3D11_USAGE_STAGING;
	tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	tex2DDesc.MiscFlags = 0;

	SAFE_RELEASE( m_pRTTexture2DCopy );
	hr = pDevice->CreateTexture2D( &tex2DDesc, NULL, &m_pRTTexture2DCopy );
	if ( FAILED( hr ) )
	{
		WriteError( _T("D3D11Device::CreateTexture2D() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create staging RenderTarget view
	//

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	SAFE_RELEASE( m_pRTBufferView );
	hr = pDevice->CreateRenderTargetView( m_pRTTexture2D, &rtvDesc, &m_pRTBufferView );
	if ( FAILED( hr ) )
	{
		WriteError( _T("D3D11Device::CreateRenderTargetView() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Create RenderTarget UAV
	//
	if ( g_TestApp.IsFL11OrGreater() )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc2;
		uavDesc2.Texture2D.MipSlice = 0;
		uavDesc2.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavDesc2.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

		SAFE_RELEASE( m_pRTUABufferView );
		hr = pDevice->CreateUnorderedAccessView( m_pRTTexture2D, &uavDesc2, &m_pRTUABufferView );
		if ( FAILED( hr ) )
		{
			WriteError( _T("D3D11Device::CreateUnorderedAccessView() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------
void
CLoadStoreTest::Cleanup()
{
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVsInput );
	SAFE_RELEASE( m_pCsInput );
	SAFE_RELEASE( m_pCsInputCopy );
	SAFE_RELEASE( m_pCsInputSRV );

	CShader5xTest::Cleanup();
}

//-----------------------------------------------------------------------------
TEST_RESULT
CLoadStoreTest::SetupTestCase()
{
	TEST_RESULT tr;

	HRESULT hr;

	++m_testcaseID;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	//
	// Determine if it's possible to test out of bounds accessing
	//

	m_outOfBoundsOffset = 1.0f;
	if (m_bUseTGSM)
	{
		// TGSM doesn't support out of bounds accessing. It causes contents of all TGSM memory to become undefined.
		m_outOfBoundsOffset = 0.0f;
	}

	//
	// Initialize the gradients buffer
	//

	if ( m_bIsLoadInstruction )
	{
		if ( !this->InitGradients() )
		{
			WriteError( _T( "CLoadStoreTest::InitGradients() failed" ) );
			return RESULT_FAIL;
		}
	}

	//
	// Determine how rendering will be done
	//

	m_bRenderUsingPointlist = false;
	switch( m_ShaderStage )
	{
	// These stages only render a pointlist and thus will require special case checking
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
	case D3D_SHADER_STAGE_GEOMETRY:
		m_bRenderUsingPointlist = true;
		break;
	}

	//
	//	Initialize the input buffer
	//

	switch ( m_ShaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
	case D3D_SHADER_STAGE_PIXEL:
		if ( !this->InitVSInput() )
		{
			WriteError( _T( "CLoadStoreTest::InitVSInput() failed" ) );
			return RESULT_FAIL;
		}
		break;

	case D3D_SHADER_STAGE_COMPUTE:
		if ( !this->InitCSInput() )
		{
			WriteError( _T( "CLoadStoreTest::InitCSInput() failed" ) );
			return RESULT_FAIL;
		}
		break;
	}

	//
	// Initialize test resources
	//

	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
		if ( !this->InitBuffer() )
		{
			WriteError( _T( "CLoadStoreTest::InitBuffer() failed" ) );
			return RESULT_FAIL;
		}
		break;

	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
		if ( !this->InitTexture1D() )
		{
			WriteError( _T( "CLoadStoreTest::InitTexture1D() failed" ) );
			return RESULT_FAIL;
		}
		break;

	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
		if ( !this->InitTexture2D() )
		{
			WriteError( _T( "CLoadStoreTest::InitTexture2D() failed" ) );
			return RESULT_FAIL;
		}
		break;

	case RESOURCE_TEXTURE3D:
		if ( !this->InitTexture3D() )
		{
			WriteError( _T( "CLoadStoreTest::InitTexture3D() failed" ) );
			return RESULT_FAIL;
		}
		break;
	}

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------
void
CLoadStoreTest::CleanupTestCase()
{
	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	if ( pContext )
	{
		ID3D11ShaderResourceView* const pSRVNulls[] = { NULL, NULL, NULL, NULL };
		ID3D11SamplerState* const pSamplerNulls[]   = { NULL, NULL, NULL, NULL };
		ID3D11Buffer* const pBufferNulls[]          = { NULL, NULL, NULL, NULL };
		ID3D11RenderTargetView* const pRTVNulls[]	= { NULL, NULL, NULL, NULL };
		ID3D11UnorderedAccessView* const pUAVNulls[]= { NULL, NULL, NULL, NULL };

		pContext->VSSetShader( NULL, NULL, 0 );
		pContext->PSSetShader( NULL, NULL, 0 );
		pContext->CSSetShader( NULL, NULL, 0 );

		pContext->VSSetShaderResources( 0, _countof( pSRVNulls ), pSRVNulls );
		pContext->PSSetShaderResources( 0, _countof( pSRVNulls ), pSRVNulls );
		pContext->CSSetShaderResources( 0, _countof( pSRVNulls ), pSRVNulls );

		pContext->VSSetSamplers( 0, _countof( pSamplerNulls ), pSamplerNulls );
		pContext->PSSetSamplers( 0, _countof( pSamplerNulls ), pSamplerNulls );
		pContext->CSSetSamplers( 0, _countof( pSamplerNulls ), pSamplerNulls );

		pContext->VSSetConstantBuffers( 0, _countof( pBufferNulls ), pBufferNulls );
		pContext->PSSetConstantBuffers( 0, _countof( pBufferNulls ), pBufferNulls );
		pContext->CSSetConstantBuffers( 0, _countof( pBufferNulls ), pBufferNulls );

		pContext->OMSetRenderTargets( _countof( pRTVNulls ), pRTVNulls, NULL );

		pContext->CSSetUnorderedAccessViews( 0, _countof( pUAVNulls ), pUAVNulls, NULL );

		pContext->IASetInputLayout( NULL );

		ID3D11Buffer* const pNullBuffer = NULL;
		const UINT uiCbStride = 0;
		const UINT uiOffset = 0;
		pContext->IASetVertexBuffers( 0, 1, &pNullBuffer, &uiCbStride, &uiOffset );
	}

	if ( m_pbGradients )
	{
		delete [] m_pbGradients;
		m_pbGradients = NULL;
	}

	SAFE_RELEASE( m_pBuffer );
	SAFE_RELEASE( m_pBufferCopy );
	SAFE_RELEASE( m_pTexture1D );
	SAFE_RELEASE( m_pTexture1DCopy );
	SAFE_RELEASE( m_pTexture2D );
	SAFE_RELEASE( m_pTexture2DCopy );
	SAFE_RELEASE( m_pTexture3D );
	SAFE_RELEASE( m_pTexture3DCopy );

	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pCS );

	SAFE_RELEASE( m_pUAV );
}

//-----------------------------------------------------------------------------
bool
CLoadStoreTest::InitVSInput()
{
	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();

	float tx0 = 0.0f - m_outOfBoundsOffset;
	float tx1 = 0.0f + m_outOfBoundsOffset;
	float ty0 = 0.0f - m_outOfBoundsOffset;
	float ty1 = 0.0f + m_outOfBoundsOffset;
	float tz0 = 0.0f - m_outOfBoundsOffset;
	float tz1 = 0.0f + m_outOfBoundsOffset;


	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
		tx1 = (float)m_resWidth + m_outOfBoundsOffset;
		break;

	case RESOURCE_TEXTURE1D_ARRAY:
		ty0 = ty1 = (float)m_resArraySlice;
		__fallthrough;

	case RESOURCE_TEXTURE1D:
		tx1 = (float)m_resWidth + m_outOfBoundsOffset;
		break;

	case RESOURCE_TEXTURE2D_ARRAY:
		tz0 = tz1 = (float)m_resArraySlice;
		__fallthrough;

	case RESOURCE_TEXTURE2D:
		tx1 = (float)m_resWidth + m_outOfBoundsOffset;
		ty1 = (float)m_resHeight + m_outOfBoundsOffset;
		break;

	case RESOURCE_TEXTURE3D:
		tx1 = (float)m_resWidth + m_outOfBoundsOffset;
		ty1 = (float)m_resHeight + m_outOfBoundsOffset;
		tz1 = (float)m_resDepth + m_outOfBoundsOffset;
		break;
	}

	// We render using a pointlist, we need to ensure the points are not clipped. (-1, -1), etc... doesn't cause the PS to be invoked.
	float xOffset = 0;
	float yOffset = 0;
	if( m_bRenderUsingPointlist )
	{
		xOffset = 1.0f/DISP_WIDTH;
		yOffset = 1.0f/DISP_HEIGHT;
	}

	VSINPUT vertices[] = {
		// Position											Address					Offest
		{ {-1.0f + xOffset,-1.0f + yOffset, 0.0f, 1.0f }, { tx0, ty1, tz0, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } },
		{ {-1.0f + xOffset, 1.0f - yOffset, 0.0f, 1.0f }, { tx0, ty0, tz0, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } },
		{ { 1.0f - xOffset,-1.0f + yOffset, 0.0f, 1.0f }, { tx1, ty1, tz1, 0.0f }, { 3.0f, 6.0f, 9.0f, 12.0f } },
		{ { 1.0f - xOffset, 1.0f - yOffset, 0.0f, 1.0f }, { tx1, ty0, tz1, 0.0f }, { 3.0f, 6.0f, 9.0f, 12.0f } }
	};

	// In order to make sure that multiple pixels write to different address, the Addresses in the above need to be
	// post swizzel addresses. So reassigning the pre swizzel addresses to VS input is necessary.
	float addressPreSwizzle[4];
	bool addressSet[4];
	int srcAddressIndices[4];
	m_srcAddressSwizzle.ToIndices( srcAddressIndices );
	for ( UINT i = 0; i < 4; i++)
	{
		for (UINT j = 0; j < 4; j++)
		{
			addressPreSwizzle[j] = vertices[i].Address[j];
		}
		memset(addressSet, 0, sizeof(bool) * 4);
		for (UINT j = 0; j < 4; j++)
		{
			if ( !addressSet[srcAddressIndices[j]])
			{
				addressPreSwizzle[srcAddressIndices[j]] = vertices[i].Address[j];
				addressSet[srcAddressIndices[j]] = 1;
			}
		}
		for (UINT j = 0; j < 4; j++)
		{
			vertices[i].Address[j] = addressPreSwizzle[j];
		}
	}

	VSINPUT* const pVsInput = new VSINPUT[NUM_VERTICES];

	memcpy( pVsInput, vertices, sizeof( vertices ) );

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapSubRes;
	if ( FAILED( hr = pContext->Map( m_pVsInput, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSubRes ) ) )
	{
		WriteToLog( _T("ID3D11DeviceContext::Map() for Stream Input Buffer failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		delete [] pVsInput;
		return false;
	}

	memcpy( mapSubRes.pData, pVsInput, NUM_VERTICES * sizeof( VSINPUT ) );

	pContext->Unmap( m_pVsInput, 0 );

	delete [] pVsInput;

	return true;
}

//-----------------------------------------------------------------------------
bool
CLoadStoreTest::InitCSInput()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	float tx0 = 0.0f - m_outOfBoundsOffset;
	float tx1 = 0.0f + m_outOfBoundsOffset;
	float ty0 = 0.0f - m_outOfBoundsOffset;
	float ty1 = 0.0f + m_outOfBoundsOffset;
	float tz0 = 0.0f - m_outOfBoundsOffset;
	float tz1 = 0.0f + m_outOfBoundsOffset;

	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
		tx1 = (float)m_resWidth + m_outOfBoundsOffset;
		break;

	case RESOURCE_TEXTURE1D_ARRAY:
		ty0 = ty1 = (float)m_resArraySlice;
		__fallthrough;

	case RESOURCE_TEXTURE1D:
		tx1 = (float)m_resWidth + m_outOfBoundsOffset;
		break;

	case RESOURCE_TEXTURE2D_ARRAY:
		tz0 = tz1 = (float)m_resArraySlice;
		__fallthrough;

	case RESOURCE_TEXTURE2D:
		tx1 = (float)m_resWidth + m_outOfBoundsOffset;
		ty1 = (float)m_resHeight + m_outOfBoundsOffset;
		break;

	case RESOURCE_TEXTURE3D:
		tx1 = (float)m_resWidth + m_outOfBoundsOffset;
		ty1 = (float)m_resHeight + m_outOfBoundsOffset;
		tz1 = (float)m_resDepth + m_outOfBoundsOffset;
		break;
	}

	const UINT uiXCount = DISP_WIDTH;
	const UINT uiYCount = DISP_HEIGHT;

	const float fdtx = ( tx1 - tx0 ) / uiXCount;
	const float fdty = ( ty1 - ty0 ) / uiYCount;
	const float fdtz = ( tz1 - tz0 ) / uiXCount;

	const float fdtx0 = fdtx / 2;
	const float fdty0 = fdty / 2;
	const float fdtz0 = fdtz / 2;

	const float fdOffsetR = ( 3.0f - 0.0f ) / uiXCount;
	const float fdOffsetG = ( 6.0f - 0.0f ) / uiXCount;
	const float fdOffsetB = ( 9.0f - 0.0f ) / uiXCount;
	const float fdOffsetA = ( 12.0f - 0.0f ) / uiXCount;
	const float fdOffsetR0 = fdOffsetR / 2;
	const float fdOffsetG0 = fdOffsetG / 2;
	const float fdOffsetB0 = fdOffsetB / 2;
	const float fdOffsetA0 = fdOffsetA / 2;

	CSINPUT* pCsInput = new CSINPUT[ uiXCount * uiYCount ];
	CSINPUT* pCsInputCur = pCsInput;

	float ty = ty0;

	for ( UINT y = 0; y < uiYCount; ++y )
	{
		float tx = tx0;
		float tz = tz0;

		float fOffsetR = 0.0f;
		float fOffsetG = 0.0f;
		float fOffsetB = 0.0f;
		float fOffsetA = 0.0f;

		for ( UINT x = 0; x < uiXCount; ++x )
		{
			pCsInputCur->Address[0] = tx + fdtx0;
			pCsInputCur->Address[1] = ty + fdty0;
			pCsInputCur->Address[2] = tz + fdtz0;
			pCsInputCur->Address[3] = 0.0f;

			pCsInputCur->Offset[0] = fOffsetR + fdOffsetR0;
			pCsInputCur->Offset[1] = fOffsetG + fdOffsetG0;
			pCsInputCur->Offset[2] = fOffsetB + fdOffsetB0;
			pCsInputCur->Offset[3] = fOffsetA + fdOffsetA0;

			++pCsInputCur;
			tx += fdtx;
			tz += fdtz;

			fOffsetR += fdOffsetR;
			fOffsetG += fdOffsetG;
			fOffsetB += fdOffsetB;
			fOffsetA += fdOffsetA;
		}
		ty += fdty;
	}
	// In order to make sure that multiple pixels write to different address, the Addresses in the above need to be
	// post swizzel addresses. So reassigning the pre swizzel addresses to CS input is necessary.
	float addressPreSwizzle[4];
	bool addressSet[4];
	int srcAddressIndices[4];
	m_srcAddressSwizzle.ToIndices( srcAddressIndices );
	pCsInputCur = pCsInput;
	for ( UINT i = 0; i < uiXCount * uiYCount; i++)
	{
		for (UINT j = 0; j < 4; j++)
		{
			addressPreSwizzle[j] = pCsInputCur->Address[j];
		}
		memset(addressSet, 0, sizeof(bool) * 4);
		for (UINT j = 0; j < 4; j++)
		{
			if ( !addressSet[srcAddressIndices[j]])
			{
				addressPreSwizzle[srcAddressIndices[j]] = pCsInputCur->Address[j];
				addressSet[srcAddressIndices[j]] = 1;
			}
		}

		for (UINT j = 0; j < 4; j++)
		{
			pCsInputCur->Address[j] = addressPreSwizzle[j];
		}
		++pCsInputCur;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed") );
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE mapSubRes;
	hr = GetImmediateContext()->Map( m_pCsInputCopy, 0, D3D11_MAP_WRITE, 0, &mapSubRes );
	if ( FAILED( hr ) )
	{
		delete [] pCsInput;
		WriteToLog( _T("ID3D11DeviceContext::Map() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	memcpy( mapSubRes.pData, pCsInput, uiXCount * uiYCount * sizeof( CSINPUT ) );

	GetImmediateContext()->Unmap( m_pCsInputCopy, 0 );

	pContext->CopyResource( m_pCsInput, m_pCsInputCopy );;

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed") );
		return false;
	}

	delete [] pCsInput;

	return true;
}

//-----------------------------------------------------------------------------
bool
CLoadStoreTest::InitGradients()
{
	UINT width	   = 1;
	UINT height	   = 1;
	UINT depth	   = 1;
	UINT mipLevels = 1;
	UINT arraySize = 1;

	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
		width = m_resWidth;
		break;

	case RESOURCE_TEXTURE1D_ARRAY:
		arraySize = m_resArraySize;
		__fallthrough;

	case RESOURCE_TEXTURE1D:
		width = m_resWidth;
		mipLevels = m_resMipLevels;
		break;

	case RESOURCE_TEXTURE2D_ARRAY:
		arraySize = m_resArraySize;
		__fallthrough;

	case RESOURCE_TEXTURE2D:
		width  = m_resWidth;
		height = m_resHeight;
		mipLevels = m_resMipLevels;
		break;

	case RESOURCE_TEXTURE3D:
		width  = m_resWidth;
		height = m_resHeight;
		depth  = m_resDepth;
		mipLevels = m_resMipLevels;
		break;
	}

	const UINT uiCbStride = GetBitsPerElement( m_resFormat ) / 8;
	const UINT uiCbArrayPitch = CalcMipSize( mipLevels, width, height, depth ) * uiCbStride;
	const UINT uiCbGradientSize = arraySize * uiCbArrayPitch;

	float fC = 1.0f;
	const float fColorStep = fC / arraySize;

	m_pbGradients = new BYTE[uiCbGradientSize];
	BYTE* pbGradients = m_pbGradients;

	for ( UINT a = 0; a < arraySize; ++a )
	{
		for ( UINT m = 0; m < mipLevels; ++m )
		{
			float fA = 1.0f;
			const float fAlphaStep = fA / depth;

			const UINT uiCbDepthPitch = width * height * uiCbStride;

			for ( UINT z = 0; z < depth; ++z )
			{
				const FLOAT_COLOR colors[] =
				{
					{ fC,   0.0f, 0.0f, fA },
					{ 0.0f, fC,   0.0f, fA },
					{ 0.0f, 0.0f, fC,   fA },
					{ fC,   fC,   0.0f, fA }
				};

				if ( !CFormatHelper::CreateGradient( m_resFormat,
													 height,
													 width,
													 colors,
													 pbGradients ) )
				{
					WriteToLog( _T("CFormatHelper::CreateGradient() failed.") );
					return false;
				}

				pbGradients += uiCbDepthPitch;
				fA -= fAlphaStep;
			}

			if ( width > 1 )
			{
				width >>= 1;
			}

			if ( height > 1 )
			{
				height >>= 1;
			}

			if ( depth > 1 )
			{
				depth >>= 1;
			}
		}

		fC -= fColorStep;
	}

	return true;
}

//-----------------------------------------------------------------------------
bool
CLoadStoreTest::InitBuffer()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.Usage               = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufDesc.CPUAccessFlags      = 0;
	bufDesc.MiscFlags           = 0;
	bufDesc.StructureByteStride = 0;

	switch ( m_bufType )
	{
	case BUFFER_TYPED:
		bufDesc.ByteWidth = m_resWidth * GetBitsPerElement( m_resFormat ) / 8;
		bufDesc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		break;

	case BUFFER_RAW:
		bufDesc.ByteWidth = m_resWidth * GetBitsPerElement( m_resFormat ) / 8;
		bufDesc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		break;

	case BUFFER_STRUCTURED:
		bufDesc.ByteWidth = m_resWidth * m_bufStride;
		bufDesc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufDesc.StructureByteStride = m_bufStride;
		break;
	}

	hr = pDevice->CreateBuffer( &bufDesc, NULL, &m_pBuffer );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11Device::CreateBuffer() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	//
	// Create staging resource
	//

	bufDesc.Usage               = D3D11_USAGE_STAGING;
	bufDesc.BindFlags           = 0;
	bufDesc.MiscFlags           = 0;
	bufDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	hr = pDevice->CreateBuffer( &bufDesc, NULL, &m_pBufferCopy );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11Device::CreateBuffer() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	//
	// Fill in buffer data
	//

	D3D11_MAPPED_SUBRESOURCE mapSubRes;
	hr = GetImmediateContext()->Map( m_pBufferCopy, 0, D3D11_MAP_WRITE, 0, &mapSubRes );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11DeviceContext::Map() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	if ( m_bIsLoadInstruction )
	{
		memcpy( mapSubRes.pData, m_pbGradients, bufDesc.ByteWidth );
	}
	else
	{
		memset( mapSubRes.pData, 0, bufDesc.ByteWidth );
	}

	GetImmediateContext()->Unmap( m_pBufferCopy, 0 );
	pContext->CopyResource( m_pBuffer, m_pBufferCopy );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed") );
		return false;
	}

	//
	// Create unordered access view
	//

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags        = 0;

	switch ( m_bufType )
	{
	case BUFFER_TYPED:
		uavDesc.Format             = m_resFormat;
		uavDesc.Buffer.NumElements = ( bufDesc.ByteWidth * 8 ) / GetBitsPerElement( m_resFormat );
		break;

	case BUFFER_RAW:
		uavDesc.Format             = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.Buffer.Flags       = D3D11_BUFFEREX_SRV_FLAG_RAW;
		uavDesc.Buffer.NumElements = ( bufDesc.ByteWidth * 8 ) / GetBitsPerElement( DXGI_FORMAT_R32_TYPELESS );
		break;

	case BUFFER_STRUCTURED:
		uavDesc.Format             = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.NumElements = bufDesc.ByteWidth / m_bufStride;
		break;
	}

	hr = pDevice->CreateUnorderedAccessView( m_pBuffer, &uavDesc, &m_pUAV );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("CLoadStoreTest::InitBufferRes - CreateUnorderedAccessView() failed. hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
bool
CLoadStoreTest::InitTexture1D()
{
	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width              = m_resWidth;
	texDesc.MipLevels          = m_resMipLevels;
	texDesc.ArraySize          = ( RESOURCE_TEXTURE1D == m_resourceType ) ? 1 : m_resArraySize;
	texDesc.Format             = m_resFormat;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;

	HRESULT hr = pDevice->CreateTexture1D( &texDesc, NULL, &m_pTexture1D );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11Device::CreateTexture1D() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	//
	// Create staging resource
	//

	texDesc.Usage               = D3D11_USAGE_STAGING;
	texDesc.BindFlags           = 0;
	texDesc.MiscFlags           = 0;
	texDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	hr = pDevice->CreateTexture1D( &texDesc, NULL, &m_pTexture1DCopy );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11Device::CreateTexture1D() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	//
	// Fill in texture data
	//

	const BYTE* pbGradients = m_pbGradients;
	const UINT uiCbStride = GetBitsPerElement( m_resFormat ) / 8;

	for ( UINT a = 0; a < texDesc.ArraySize; ++a )
	{
		UINT width = texDesc.Width;

		for ( UINT m = 0; m < texDesc.MipLevels; ++m )
		{
			const UINT idxSubRes = a * texDesc.MipLevels + m;

			D3D11_MAPPED_SUBRESOURCE mapSubRes;
			hr = GetImmediateContext()->Map( m_pTexture1DCopy, idxSubRes, D3D11_MAP_WRITE, 0, &mapSubRes );
			if ( FAILED( hr ) )
			{
				WriteToLog(_T("ID3D11DeviceContext::Map() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
				return false;
			}

			const UINT uiCbRowPitch = width * uiCbStride;

			if ( m_bIsLoadInstruction )
			{
				memcpy( mapSubRes.pData, pbGradients, uiCbRowPitch );
				pbGradients += uiCbRowPitch;
			}
			else
			{
				memset( mapSubRes.pData, 0, uiCbRowPitch );
			}

			GetImmediateContext()->Unmap( m_pTexture1DCopy, idxSubRes );

			if ( width > 1 )
			{
				width >>= 1;
			}
		}
	}

	pContext->CopyResource( m_pTexture1D, m_pTexture1DCopy );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed") );
		return false;
	}

	//
	// Create unordered access view
	//

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = m_resFormat;

	if ( RESOURCE_TEXTURE1D == m_resourceType )
	{
		uavDesc.ViewDimension      = D3D11_UAV_DIMENSION_TEXTURE1D;
		uavDesc.Texture1D.MipSlice = m_resMipSlice;
	}
	else
	{
		uavDesc.ViewDimension                  = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
		uavDesc.Texture1DArray.ArraySize       = texDesc.ArraySize;
		uavDesc.Texture1DArray.FirstArraySlice = 0;
		uavDesc.Texture1DArray.MipSlice        = m_resMipSlice;
	}

	hr = pDevice->CreateUnorderedAccessView(m_pTexture1D, &uavDesc, &m_pUAV );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11Device::::CreateUnorderedAccessView() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
bool
CLoadStoreTest::InitTexture2D()
{
	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width              = m_resWidth;
	texDesc.Height             = m_resHeight;
	texDesc.MipLevels          = m_resMipLevels;
	texDesc.ArraySize          = ( RESOURCE_TEXTURE2D == m_resourceType ) ? 1 : m_resArraySize;
	texDesc.Format             = m_resFormat;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;

	HRESULT hr = pDevice->CreateTexture2D( &texDesc, NULL, &m_pTexture2D );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11Device::CreateTexture2D() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	//
	// Create staging resource
	//

	texDesc.BindFlags = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* pTexture2DCopy;
	hr = pDevice->CreateTexture2D( &texDesc, NULL, &m_pTexture2DCopy );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11Device::CreateTexture2D() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	//
	// Fill in texture data
	//

	const BYTE* pbGradients = m_pbGradients;
	const UINT uiCbStride = GetBitsPerElement( m_resFormat ) / 8;

	for ( UINT a = 0; a < texDesc.ArraySize; ++a )
	{
		UINT width  = texDesc.Width;
		UINT height = texDesc.Height;

		for ( UINT m = 0; m < texDesc.MipLevels; ++m )
		{
			const UINT idxSubRes = a * texDesc.MipLevels + m;

			D3D11_MAPPED_SUBRESOURCE mapSubRes;
			hr = GetImmediateContext()->Map( m_pTexture2DCopy, idxSubRes, D3D11_MAP_WRITE, 0, &mapSubRes );
			if ( FAILED( hr ) )
			{
				WriteToLog(_T("ID3D11DeviceContext::Map() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
				return false;
			}

			BYTE* pbData = reinterpret_cast<BYTE*>( mapSubRes.pData );
			const UINT uiCbRowPitch = width * uiCbStride;

			for ( UINT y = 0; y < height; ++y )
			{
				if ( m_bIsLoadInstruction )
				{
					memcpy( pbData, pbGradients, uiCbRowPitch );
					pbGradients += uiCbRowPitch;
				}
				else
				{
					memset( pbData, 0, uiCbRowPitch );
				}

				pbData += mapSubRes.RowPitch;
			}

			GetImmediateContext()->Unmap( m_pTexture2DCopy, idxSubRes );

			if ( width > 1 )
			{
				width >>= 1;
			}

			if ( height > 1 )
			{
				height >>= 1;
			}
		}
	}

	pContext->CopyResource( m_pTexture2D, m_pTexture2DCopy );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed") );
		return false;
	}

	//
	// Create unordered access view
	//

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = m_resFormat;

	if ( RESOURCE_TEXTURE2D == m_resourceType )
	{
		uavDesc.ViewDimension      = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = m_resMipSlice;
	}
	else
	{
		uavDesc.ViewDimension                  = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.ArraySize       = texDesc.ArraySize;
		uavDesc.Texture2DArray.FirstArraySlice = 0;
		uavDesc.Texture2DArray.MipSlice        = m_resMipSlice;
	}

	hr = pDevice->CreateUnorderedAccessView( m_pTexture2D, &uavDesc, &m_pUAV );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11Device::CreateUnorderedAccessView() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
bool
CLoadStoreTest::InitTexture3D()
{
	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	D3D11_TEXTURE3D_DESC texDesc;
	texDesc.Width              = m_resWidth;
	texDesc.Height             = m_resHeight;
	texDesc.Depth              = m_resDepth;
	texDesc.MipLevels          = m_resMipLevels;
	texDesc.Format             = m_resFormat;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;

	HRESULT hr = pDevice->CreateTexture3D( &texDesc, NULL, &m_pTexture3D );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11Device::CreateTexture3D() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	//
	// Create the staging resource
	//

	texDesc.BindFlags = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;

	ID3D11Texture3D* pTexture3DCopy;
	hr = pDevice->CreateTexture3D( &texDesc, NULL, &m_pTexture3DCopy );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11Device::CreateTexture3D() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	//
	// Fill in texture data
	//

	const BYTE* pbGradients = m_pbGradients;
	const UINT uiCbStride = GetBitsPerElement( m_resFormat ) / 8;

	UINT width  = texDesc.Width;
	UINT height = texDesc.Height;
	UINT depth  = texDesc.Depth;

	for ( UINT m = 0; m < texDesc.MipLevels; ++m )
	{
		D3D11_MAPPED_SUBRESOURCE mapSubRes;
		hr = GetImmediateContext()->Map( m_pTexture3DCopy, m, D3D11_MAP_WRITE, 0, &mapSubRes );
		if ( FAILED( hr ) )
		{
			WriteToLog(_T("ID3D11DeviceContext::Map() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
			return false;
		}

		BYTE* pbData = reinterpret_cast<BYTE*>( mapSubRes.pData );
		const UINT uiCbRowPitch = width * uiCbStride;

		for ( UINT z = 0; z < depth; ++z )
		{
			BYTE* pbDataY = pbData;

			for ( UINT y = 0; y < height; ++y )
			{
				if ( m_bIsLoadInstruction )
				{
					memcpy( pbDataY, pbGradients, uiCbRowPitch );
					pbGradients += uiCbRowPitch;
				}
				else
				{
					memset( pbDataY, 0, uiCbRowPitch );
				}

				pbDataY += mapSubRes.RowPitch;
			}

			pbData += mapSubRes.DepthPitch;
		}

		GetImmediateContext()->Unmap( m_pTexture3DCopy, m );

		if ( width > 1 )
		{
			width >>= 1;
		}

		if ( height > 1 )
		{
			height >>= 1;
		}

		if ( depth > 1 )
		{
			depth >>= 1;
		}
	}

	pContext->CopyResource( m_pTexture3D, m_pTexture3DCopy );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed") );
		return false;
	}

	//
	// Create unordered access view
	//

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format	= m_resFormat;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	uavDesc.Texture3D.FirstWSlice	= 0;
	uavDesc.Texture3D.WSize			= -1;
	uavDesc.Texture3D.MipSlice		= m_resMipSlice;

	hr = pDevice->CreateUnorderedAccessView( m_pTexture3D, &uavDesc, &m_pUAV );
	if ( FAILED( hr ) )
	{
		WriteToLog(_T("ID3D11Device::CreateUnorderedAccessView() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

bool
CLoadStoreTest::CompareResultWithFormatULP( const float &fExpected, const float &fResult, const DXGI_FORMAT &resFormat, const UINT &componentIndex, float &fUlpDiff )
{
	// Lookup the info for the format, and set the current feature level
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(resFormat);
	MinMaxRange::SetD3DFeatureLevel(GetDevice()->GetFeatureLevel());
	float allowedULP = 0.5f;

	if( pTexFormatInfo->dwFlags & FF_FLOAT )
	{
		UINT bitsOfPrecision = 0;

		switch (pTexFormatInfo->pBitsPerComponent[componentIndex])
		{
		default:
			if (!((   pTexFormatInfo->Format == DXGI_FORMAT_R24_UNORM_X8_TYPELESS
				|| pTexFormatInfo->Format == DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS)
				&& componentIndex > 0))
			{
				assert(!"Unrecognized float format");
			}
			// fall-through to 0 bits
		case 0: bitsOfPrecision = 0; break;
		case 32: bitsOfPrecision = 23; break;
		case 16: bitsOfPrecision = 10; break;
		case 11: bitsOfPrecision = 6; break;
		case 10: bitsOfPrecision = 5; break;
		}
		if (pTexFormatInfo->Format == DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
			bitsOfPrecision = (componentIndex < 3) ? 9 : 0;

		allowedULP = MinMaxRange::GetFloatULPVariance( bitsOfPrecision );
	}
	else if( pTexFormatInfo->dwFlags & FF_UNORM ||
		pTexFormatInfo->dwFlags & FF_SNORM )
	{
		// Direct3D 10+ allows 0.6 ULP for these conversions:
		// FLOAT->UNORM
		// FLOAT->SNORM
		allowedULP = 0.6f;
	}
	else
	{
		allowedULP = MinMaxRange::GetFixedFloatULPVariance();
	}

	// Compare the result
	return MinMaxRange::TestFloatsSame( fExpected, fResult, pTexFormatInfo, componentIndex, allowedULP, &fUlpDiff );
}

///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
void
CLd_uav_typed::InitTestParameters()
{
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Define supported shader stages
	CUserValueSet<D3D_SHADER_STAGE>* const pShaderStages = new CUserValueSet<D3D_SHADER_STAGE>;
	pShaderStages->AddValue( D3D_SHADER_STAGE_VERTEX );
	pShaderStages->AddValue( D3D_SHADER_STAGE_GEOMETRY );
	pShaderStages->AddValue( D3D_SHADER_STAGE_PIXEL );
	pShaderStages->AddValue( D3D_SHADER_STAGE_HULL );
	pShaderStages->AddValue( D3D_SHADER_STAGE_DOMAIN );
	testfactor::RFactor rfNonComputeShaders = this->AddParameterValueSet<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), pShaderStages );
	testfactor::RFactor rfPixelShader = this->AddParameterValue<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );
	testfactor::RFactor rfComputeShader = this->AddParameterValue<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_COMPUTE );

	// Define supported resources
	CTestCaseParameter<RESOURCE_TYPE>* const pResourceType = this->AddParameter( _T("ResourceType"), &m_resourceType );
	testfactor::RFactor rfResBuffer     = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_BUFFER );
	testfactor::RFactor rfResTex1D      = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_TEXTURE1D );
	testfactor::RFactor rfResTex1DArray = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_TEXTURE1D_ARRAY );
	testfactor::RFactor rfResTex2D      = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_TEXTURE2D );
	testfactor::RFactor rfResTex2DArray = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_TEXTURE2D_ARRAY );
	testfactor::RFactor rfResTex3D      = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_TEXTURE3D );

	// Define supported formats
	CUserValueSet<DXGI_FORMAT>* const pFormats = new CUserValueSet<DXGI_FORMAT>;
	pFormats->AddValue( DXGI_FORMAT_R32_FLOAT );
    pFormats->AddValue( DXGI_FORMAT_R32_UINT );
    pFormats->AddValue( DXGI_FORMAT_R32_SINT );
	testfactor::RFactor rfFormat = this->AddParameter( _T("ResFormat"), &m_resFormat, pFormats );

	// Define supported resource width values
	CUserValueSet<UINT>* const pWidthValues = new CUserValueSet<UINT>;
	pWidthValues->AddValue( MAX_WIDTH * 1 );
    pWidthValues->AddValue( MAX_WIDTH * 2 );
    pWidthValues->AddValue( MAX_WIDTH * 4 );
	testfactor::RFactor rfResWidth = this->AddParameter( _T("ResWidth"), &m_resWidth, pWidthValues );

	// Define supported resource height values
	CUserValueSet<UINT>* const pHeightValues = new CUserValueSet<UINT>;
	pHeightValues->AddValue( MAX_HEIGHT * 1 );
    pHeightValues->AddValue( MAX_HEIGHT * 2 );
    pHeightValues->AddValue( MAX_HEIGHT * 4 );
	testfactor::RFactor rfResHeight = this->AddParameter( _T("ResHeight"), &m_resHeight, pHeightValues );

	// Define supported resource depth values
	CUserValueSet<UINT>* const pDepthValues = new CUserValueSet<UINT>;
	pDepthValues->AddValue( MAX_DEPTH * 1 );
    pDepthValues->AddValue( MAX_DEPTH * 2 );
    pDepthValues->AddValue( MAX_DEPTH * 4 );
	testfactor::RFactor rfResDepth = this->AddParameter( _T("ResDepth"), &m_resDepth, pDepthValues );

	// Define supported resource mip levels
	CUserValueSet<UINT>* const pMipLevelValues = new CUserValueSet<UINT>;
	for ( UINT i = 1; i <= MAX_MIP_COUNT; ++i )
	{
		pMipLevelValues->AddValue( i );
	}
	testfactor::RFactor rfResMipLevels = this->AddParameter( _T("ResMipLevels"), &m_resMipLevels, pMipLevelValues );

	// Define supported resource mip slice
	CUserValueSet<UINT>* const pMipSliceValues = new CUserValueSet<UINT>;
	for ( UINT i = 0; i < MAX_MIP_COUNT; ++i )
	{
		pMipSliceValues->AddValue( i );
	}
	testfactor::RFactor rfResMipSlice = this->AddParameter( _T("ResMipSlice"), &m_resMipSlice, pMipSliceValues );

	// Define supported resource array size
	CUserValueSet<UINT>* const pArraySizeValues = new CUserValueSet<UINT>;
	for ( UINT i = 1; i <= MAX_ARRAY_SIZE; ++i )
	{
		pArraySizeValues->AddValue( i );
	}
	testfactor::RFactor rfResArraySize = this->AddParameter( _T("ResArraySize"), &m_resArraySize, pArraySizeValues );

	// Define supported resource array slice
	CUserValueSet<UINT>* pArraySliceValues = new CUserValueSet<UINT>;
	for ( UINT i = 0; i < MAX_ARRAY_SIZE; ++i )
	{
		pArraySliceValues->AddValue( i );
	}
	testfactor::RFactor rfResArraySlice = this->AddParameter( _T("ResArraySlice"), &m_resArraySlice, pArraySliceValues );

	// Define supported destination masks
	CUserValueSet<AssemblyHelper::MaskFlag>* const pMaskValues = new CUserValueSet<AssemblyHelper::MaskFlag>();
	for ( UINT i = 0; i < _countof( g_pMasks ); ++i )
	{
		pMaskValues->AddValue( g_pMasks[i] );
	}
	testfactor::RFactor rfDestMask = this->AddParameter( _T("DestMask"), &m_destMask, pMaskValues );

	// Define supported source address values
	CUserValueSet<AssemblyHelper::Swizzle>* const pAddressValues = new CUserValueSet<AssemblyHelper::Swizzle>();
	for ( UINT i = 0; i < _countof( g_pSwizzles ); ++i )
	{
		pAddressValues->AddValue( g_pSwizzles[i] );
	}
	testfactor::RFactor rfSrcAddress = this->AddParameter( _T("SrcAddressSwizzle"), &m_srcAddressSwizzle, pAddressValues );

	// Define supported source swizzle values
	CUserValueSet<AssemblyHelper::Swizzle>* const pSwizzleValues = new CUserValueSet<AssemblyHelper::Swizzle>();
	for ( UINT i = 0; i < _countof( g_pSwizzles ); ++i )
	{
		pSwizzleValues->AddValue( g_pSwizzles[i] );
	}
	testfactor::RFactor rfSrcSwizzle = this->AddParameter( _T("SrcSwizzle"), &m_srcSwizzle, pSwizzleValues );

	// Define supported UAV slots
	CTestCaseParameter<UINT>* const pUAVSlot = this->AddParameter( _T("UAVSlot"), &m_UAVSlot );
	testfactor::RFactor rfUAVSlot0 = this->AddParameterValue<UINT>( pUAVSlot, 0 );
	testfactor::RFactor rfUAVSlot1 = this->AddParameterValue<UINT>( pUAVSlot, 1 );
	testfactor::RFactor rfUAVSlot2 = this->AddParameterValue<UINT>( pUAVSlot, 2 );
	testfactor::RFactor rfUAVSlot3 = this->AddParameterValue<UINT>( pUAVSlot, 3 );
	testfactor::RFactor rfUAVSlot4 = this->AddParameterValue<UINT>( pUAVSlot, 4 );
	testfactor::RFactor rfUAVSlot5 = this->AddParameterValue<UINT>( pUAVSlot, 5 );
	testfactor::RFactor rfUAVSlot6 = this->AddParameterValue<UINT>( pUAVSlot, 6 );
	testfactor::RFactor rfUAVSlot7 = this->AddParameterValue<UINT>( pUAVSlot, 7 );
	testfactor::RFactor rfUAVSlots63 = this->AddParameterValueSet( pUAVSlot, new CRangeValueSet<UINT>(1, 64 - 1, 1) );	// FL11.1+ supports 64 UAV slots
	testfactor::RFactor rfUAVSlots64 = this->AddParameterValueSet( pUAVSlot, new CRangeValueSet<UINT>(0, 64 - 1, 1) );	// FL11.1+ supports 64 UAV slots

	// Set the test root factor
	testfactor::RFactor rfModifier = ( rfDestMask * rfSrcAddress ) % rfFormat;
	testfactor::RFactor rfTexFactor = rfResMipLevels * rfResMipSlice;
	testfactor::RFactor rfTexArrayFactor = rfTexFactor * rfResArraySize * rfResArraySlice;
	testfactor::RFactor rfResFactor =
		rfResBuffer * rfModifier % rfResWidth +
		rfResTex1D * rfModifier % ( rfResWidth * rfTexFactor ) +
		rfResTex1DArray * rfModifier % ( rfResWidth * rfTexArrayFactor ) +
	  	rfResTex2D * rfModifier % ( rfResWidth * rfResHeight * rfTexFactor ) +
		rfResTex2DArray * rfModifier % ( rfResWidth * rfResHeight * rfTexArrayFactor ) +
		rfResTex3D * rfModifier % ( rfResWidth * rfResHeight * rfResDepth * rfTexFactor );

	if( g_TestApp.IsFL11_1OrGreater() )
	{
		this->SetRootTestFactor(
			  ( ( rfNonComputeShaders * rfResFactor ) % rfSrcSwizzle ) % rfUAVSlots63 +	// Don't use UAV slot 0, [1, 63]
			  ( ( rfComputeShader * rfResFactor ) % rfSrcSwizzle ) % rfUAVSlots64 );	// UAV slot [0,63]
	}
	else
	{
		this->SetRootTestFactor(
			  ( ( rfPixelShader * rfResFactor ) % rfSrcSwizzle ) %
				( rfUAVSlot1 + rfUAVSlot2 + rfUAVSlot3 + rfUAVSlot4 + rfUAVSlot5 + rfUAVSlot6 + rfUAVSlot7 ) +
			  ( ( rfComputeShader * rfResFactor ) % rfSrcSwizzle ) %
				( rfUAVSlot0 + rfUAVSlot1 + rfUAVSlot2 + rfUAVSlot3 + rfUAVSlot4 + rfUAVSlot5 + rfUAVSlot6 + rfUAVSlot7 ) );
	}

	// Skip invalid matches
	this->SkipMatching( ( FilterActive<UINT>( _T("ResMipLevels") ) &&
		                  !LoadStoreTest::FilterValidMipSize( this, _T("ResMipLevels"), _T("ResWidth") ) ) ||
				        ( FilterActive<UINT>( _T("ResMipSlice") ) &&
				          !LoadStoreTest::FilterLess<UINT>( this, _T("ResMipSlice"), _T("ResMipLevels") ) ) );

	AddPriorityPatternFilter(FilterEqual<RESOURCE_TYPE>(_T("ResourceType"), RESOURCE_BUFFER), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<RESOURCE_TYPE>(_T("ResourceType"), RESOURCE_TEXTURE2D), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResWidth"), MAX_WIDTH), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResWidth"), MAX_WIDTH * 2), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResHeight"), MAX_HEIGHT), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResHeight"), MAX_HEIGHT * 2), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResDepth"), MAX_DEPTH), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResDepth"), MAX_DEPTH * 2), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResMipLevels"), 1), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResMipSlice"), 0), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//-----------------------------------------------------------------------------
TEST_RESULT
CLd_uav_typed::Setup()
{
	// UAV typed buffers are only supported on DX11 and above
	if ( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog(_T("UAV typed buffers are only supported on FL11.0 and above."));
		return RESULT_SKIP;
	}

	return CLoadStoreTest::Setup();
}

//-----------------------------------------------------------------------------
TEST_RESULT
CLd_uav_typed::SetupTestCase()
{
	// some swizzle may cause the array index to vary for the same test case.
	// Ideally, it should be handled by a more complex AnalyzeResult().
	// Skip them for now, since the overall test cases have covered the range of
	// array indexing.
	int srcAddressIndices[4];
	m_srcAddressSwizzle.ToIndices( srcAddressIndices );
	if ( m_resourceType == RESOURCE_TEXTURE1D_ARRAY )
	{
		if ( srcAddressIndices[0] == srcAddressIndices[1] )
		{
			WriteToLog(_T("Skip test case because of swizzle mismatch."));
			return RESULT_SKIP;
		}
	}
	else if ( m_resourceType == RESOURCE_TEXTURE2D_ARRAY )
	{
		if ( srcAddressIndices[0] == srcAddressIndices[2] ||
			srcAddressIndices[1] == srcAddressIndices[2])
		{
			WriteToLog(_T("Skip test case because of swizzle mismatch."));
			return RESULT_SKIP;
		}
	}

	return CLoadStoreTest::SetupTestCase();
}

//-----------------------------------------------------------------------------
TEST_RESULT
CLd_uav_typed::ExecuteTestCase()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();

	const UINT uiCbStride = sizeof( VSINPUT );
	const UINT uiOffset = 0;

	const UINT uiRTUAVSlot = ( m_UAVSlot + 1 ) % 8;

	switch ( m_ShaderStage )
	{
	case D3D_SHADER_STAGE_PIXEL:
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:

		if ( !this->InitPipelineShaders() )
		{
			WriteError( _T( "CLd_uav_typed::InitPipelineShaders() failed" ) );
			return RESULT_FAIL;
		}

		pContext->OMSetRenderTargetsAndUnorderedAccessViews( 1, &m_pRTBufferView, NULL, m_UAVSlot, 1, &m_pUAV, NULL );
		pContext->RSSetState( m_pRSEnabled );
		pContext->IASetVertexBuffers( 0, 1, &m_pVsInput, &uiCbStride, &uiOffset );
		pContext->IASetInputLayout( m_pInputLayout );

		switch( m_ShaderStage )
		{
		default:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
			break;
		case D3D_SHADER_STAGE_PIXEL:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			break;
		case D3D_SHADER_STAGE_HULL:
		case D3D_SHADER_STAGE_DOMAIN:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST );
			break;
		}

		pContext->Draw( NUM_VERTICES, 0 );
		break;

	case D3D_SHADER_STAGE_COMPUTE:

		if ( !this->InitCSShaders() )
		{
			WriteError( _T( "CLd_uav_typed::InitCSShaders() failed" ) );
			return RESULT_FAIL;
		}

		pContext->CSSetShaderResources( 0, 1, &m_pCsInputSRV );
		pContext->CSSetUnorderedAccessViews( uiRTUAVSlot, 1, &m_pRTUABufferView, NULL );
		pContext->CSSetUnorderedAccessViews( m_UAVSlot, 1, &m_pUAV, NULL );
		pContext->CSSetShader( m_pCS, NULL, 0 );
		pContext->Dispatch( DISP_WIDTH, DISP_HEIGHT, 1 );
		break;
	}

	hr = this->ExecuteEffectiveContext();
	if ( FAILED( hr ) )
	{
		WriteToLog( _T("this->ExecuteEffectiveContext() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	return this->AnalyzeResult() ? RESULT_PASS : RESULT_FAIL;
}

//-----------------------------------------------------------------------------
bool
CLd_uav_typed::InitPipelineShaders()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	const tstring strDestMask	= ToString( m_destMask );
	const tstring strSrcSwizzle	= ToString( m_srcSwizzle );
	const tstring strSrcAddressSwizzle = ToString( m_srcAddressSwizzle );
	const tstring strResType	= ToAsmName( m_resourceType );
	const FORMAT_TYPE formatType = GetFormatType( m_resFormat );
	const tstring strFormat		= ToAsmName( formatType );

	const unsigned TempRegCount = 3;
	AssemblyHelper asmHelper( 0, TempRegCount );

	asmHelper.SetCustomUAVDecl( m_UAVSlot, FormatString( "dcl_uav_typed u%d, %s, %s", m_UAVSlot, strResType.c_str(), strFormat.c_str() ) );

	// Position is in V[0], Address is in V[1], Offset is in V[2]. LD output in r4.
	asmHelper.SetPositionInputRegIdx(0);
	asmHelper.SetUserInputRegMaskTyped( 1, AssemblyHelper::xyzw, "linear", "float" );
	asmHelper.SetUserInputRegMaskTyped( 2, AssemblyHelper::xyzw, "linear", "float" );
	asmHelper.SetupMovToOutputFromTemp( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );

	if( m_ShaderStage != D3D_SHADER_STAGE_PIXEL )
	{
		asmHelper.SetPositionOutputRegIdx(5);
		asmHelper.SetupMovToOutputFromInput( 5, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );	// Move position from 0 to 5
	}

	asmHelper.SetOutputRegMaskTyped( 0, AssemblyHelper::xyzw, "float" );

	tstring strShaderCode = "";
	strShaderCode += FormatString( "mov r0, vec4(%f, %f, %f, %f)\n",
		DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE );

	switch( m_ShaderStage )
	{
	default:
		strShaderCode += "ftou r1, v[1]\n";
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
		strShaderCode += "ftou r1, v[0][1]\n";
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		strShaderCode += "ftou r1, vcp[0][1]\n";
		break;
	}

	const tstring Instruction = ResidencyFeedback() ? "ld_uav_typed_s" : "ld_uav_typed";
	const tstring ResidencyFeedbackReg = ResidencyFeedback() ? "r2.x" : "";
	const tstring ResidencyFeedbackRegWithComma = ResidencyFeedback() ? tstring(_T(" ")) + ResidencyFeedbackReg + _T(",") : _T("");

	strShaderCode += FormatString( "%s r0.%s,%s r1.%s, u%d.%s\n", Instruction.c_str(),
		strDestMask.c_str(), ResidencyFeedbackRegWithComma.c_str(), strSrcAddressSwizzle.c_str(), m_UAVSlot, strSrcSwizzle.c_str() );

	if(ResidencyFeedback())
	{
		// force the driver to use feedback result to avoid UMD optimizing out the modifier
		tstring forceFeedback =
			tstring(_T("check_access_fully_mapped ")) + ResidencyFeedbackReg + _T(", ") + ResidencyFeedbackReg + _T("\n") +
			_T("xor ") + ResidencyFeedbackReg + _T(", ") + ResidencyFeedbackReg + (", 0xffffffff\n") +
			_T("xor r0.") + strDestMask + _T(", r0, ") + ResidencyFeedbackReg;
		strShaderCode += forceFeedback;
	}

	const bool bNeedPassThroughVS = (m_ShaderStage != D3D_SHADER_STAGE_VERTEX);
	const bool bNeedPassThroughPS = (m_ShaderStage != D3D_SHADER_STAGE_PIXEL);


	asmHelper.EnableTiledResources(ResidencyFeedback());

	ID3D10Blob* pShaderBuf = NULL;
	hr = asmHelper.SetupPipelineWithShaderCode( pDevice,
		                                        pContext,
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												strShaderCode.c_str(),
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS,
												NULL,
												0,
												NULL,
												bNeedPassThroughPS,
												bNeedPassThroughVS);
	asmHelper.EnableTiledResources(false);
	if ( FAILED( hr ) )
	{
		WriteError( _T("AssemblyHelper::SetupPipelineWithShaderCode() failed, hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	if ( g_bDumpDissasm )
	{
		this->MessageDumpShaderDisassembly( (UINT*)pShaderBuf->GetBufferPointer(),
			                                pShaderBuf->GetBufferSize() );
	}

	SAFE_RELEASE( pShaderBuf );

	return true;
}

//-----------------------------------------------------------------------------
bool
CLd_uav_typed::InitCSShaders()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	const tstring strDestMask	= ToString( m_destMask );
	const tstring strSrcSwizzle	= ToString( m_srcSwizzle );
	const tstring strSrcAddress = ToString( m_srcAddressSwizzle );
	const tstring strResType	= ToAsmName( m_resourceType );
	const FORMAT_TYPE formatType = GetFormatType( m_resFormat );
	const tstring strFormat		= ToAsmName( formatType );

	const UINT uiRTUAVSlot = ( m_UAVSlot + 1 ) % 8;

	AssemblyHelper asmHelper( 0, 2 );

	asmHelper.SetCustomDecl( 0, FormatString( "dcl_resource_structured t%d, %d", 0, sizeof( CSINPUT ) ) );
	asmHelper.SetCustomUAVDecl( m_UAVSlot, FormatString( "dcl_uav_typed u%d, %s, %s", m_UAVSlot, strResType.c_str(), strFormat.c_str() ) );
	asmHelper.SetOutputRegMaskTyped( uiRTUAVSlot, AssemblyHelper::xyzw, strFormat.c_str() );

	tstring strShaderCode = "";
	strShaderCode += FormatString( "mov r0, vec4(%f, %f, %f, %f)\n",
		DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE );
	strShaderCode += FormatString( "imad r1, vThreadGroupID.y, vec4(%d, 0, 0, 0), vThreadGroupID.x\n", DISP_WIDTH );
	strShaderCode += "ld_structured r1, r1.x, 0, t0\n";
	strShaderCode += "ftou r1, r1\n";
	strShaderCode += FormatString( "ld_uav_typed r0.%s, r1.%s, u%d.%s\n",
		strDestMask.c_str(), strSrcAddress.c_str(), m_UAVSlot, strSrcSwizzle.c_str() );
	strShaderCode += FormatString( "store_uav_typed u%d, vThreadGroupID.xy, r0", uiRTUAVSlot );

	ID3D10Blob* pShaderBuf = NULL;
	hr = asmHelper.SetupPipelineWithShaderCode( pDevice,
		                                        pContext,
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												strShaderCode.c_str(),
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );
	if ( FAILED( hr ) )
	{
		WriteError( _T("AssemblyHelper::SetupPipelineWithShaderCode() failed, hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	if ( g_bDumpDissasm )
	{
		this->MessageDumpShaderDisassembly( (UINT*)pShaderBuf->GetBufferPointer(),
			                                pShaderBuf->GetBufferSize() );
	}

	SAFE_RELEASE( pShaderBuf );

	return true;
}

//-----------------------------------------------------------------------------
bool
CLd_uav_typed::AnalyzeResult()
{
	HRESULT hr;

	bool bResult = true;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	const bool bLogVerbose = ((CShader5xTestApp*)GetFramework())->LogVerbose();
	const UINT numULPFailuresToLog = NUM_FAILURES_TO_LOG;
	const UINT numNonULPFailuresToLog = NUM_FAILURES_TO_LOG;
	UINT numULPFailuresLogged = 0;
	UINT numNonULPFailuresLogged = 0;

	BYTE ZeroBuffer[16];
	memset( ZeroBuffer, 0, sizeof( ZeroBuffer ) );

	int srcSwizzleIndices[4];
	m_srcSwizzle.ToIndices( srcSwizzleIndices );

	int srcAddressIndices[4];
	m_srcAddressSwizzle.ToIndices( srcAddressIndices );

	pContext->CopySubresourceRegion( m_pRTTexture2DCopy, 0, 0, 0, 0, m_pRTTexture2D, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed") );
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE texmap;
	hr = GetImmediateContext()->Map( m_pRTTexture2DCopy, 0, D3D11_MAP_READ, 0, &texmap );
	if ( FAILED( hr ) )
	{
		WriteToLog( _T("ID3D11DeviceContext::Map() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	UINT width		= 1;
	UINT height		= 1;
	UINT depth		= 1;
	UINT mipLevels	= 1;
	UINT mipSlice	= 0;
	UINT arraySize	= 1;
	UINT arraySlice = 0;

	float tx0 = 0.0f - m_outOfBoundsOffset;
	float tx1 = 0.0f + m_outOfBoundsOffset;
	float ty0 = 0.0f - m_outOfBoundsOffset;
	float ty1 = 0.0f + m_outOfBoundsOffset;
	float tz0 = 0.0f - m_outOfBoundsOffset;
	float tz1 = 0.0f + m_outOfBoundsOffset;

	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
		width = m_resWidth;
		tx1 = (float)width + m_outOfBoundsOffset;
		break;

	case RESOURCE_TEXTURE1D_ARRAY:
		arraySize = m_resArraySize;
		arraySlice = m_resArraySlice;
		ty0 = ty1 = (float)arraySlice;
		__fallthrough;

	case RESOURCE_TEXTURE1D:
		width = m_resWidth;
		mipLevels = m_resMipLevels;
		mipSlice = m_resMipSlice;
		tx1 = (float)m_resWidth + m_outOfBoundsOffset;
		break;

	case RESOURCE_TEXTURE2D_ARRAY:
		arraySize = m_resArraySize;
		arraySlice = m_resArraySlice;
		tz0 = tz1 = (float)arraySlice;
		__fallthrough;

	case RESOURCE_TEXTURE2D:
		width  = m_resWidth;
		height = m_resHeight;
		mipLevels = m_resMipLevels;
		mipSlice = m_resMipSlice;
		tx1 = (float)width + m_outOfBoundsOffset;
		ty1 = (float)height + m_outOfBoundsOffset;
		break;

	case RESOURCE_TEXTURE3D:
		width  = m_resWidth;
		height = m_resHeight;
		depth  = m_resDepth;
		mipLevels = m_resMipLevels;
		mipSlice = m_resMipSlice;
		tx1 = (float)width + m_outOfBoundsOffset;
		ty1 = (float)height + m_outOfBoundsOffset;
		tz1 = (float)depth + m_outOfBoundsOffset;
		break;
	}

	const FORMAT_TYPE formatType = GetFormatType( m_resFormat );
	const UINT uiCbRTStride  = GetBitsPerElement( DXGI_FORMAT_R32G32B32A32_FLOAT ) / 8;
	const UINT uiCbRSStride  = GetBitsPerElement( m_resFormat ) / 8;
	const UINT uiCbArraySize = CalcMipSize( mipLevels, width, height, depth );
	const UINT uiCbMipOffset = CalcMipSize( mipSlice,
						         			width,
											height,
											depth,
											&width,
											&height,
											&depth );

	const UINT uiXCount = DISP_WIDTH;
	const UINT uiYCount = DISP_HEIGHT;

	const float fdtx = ( tx1 - tx0 ) / uiXCount;
	const float fdty = ( ty1 - ty0 ) / uiYCount;
	const float fdtz = ( tz1 - tz0 ) / uiXCount;

	const float fdtx0 = fdtx / 2;
	const float fdty0 = fdty / 2;
	const float fdtz0 = fdtz / 2;

	float ty = ty0;

	// The VS etc, only use four verticies and point list, so only the four corners will light up
	assert( NUM_VERTICES == 4 );
	const bool bOnlyCheckFourCorners = m_bRenderUsingPointlist ? true : false;
	float vertexTexCoords[NUM_VERTICES][4] = {
	// Address
	{ tx0, ty0, tz0, 0.0f },
	{ tx0, ty1, tz0, 0.0f },
	{ tx1, ty0, tz1, 0.0f },
	{ tx1, ty1, tz1, 0.0f }};

	for ( UINT y = 0; y < uiYCount; ++y )
	{
		float tx = tx0;
		float tz = tz0;

		if( bOnlyCheckFourCorners && y != 0 && y != (uiYCount - 1) )
		{
			// Not possible to be in a corner...leave the reference buffer zero
			continue;
		}

		for ( UINT x = 0; x < uiXCount; ++x )
		{
			if( bOnlyCheckFourCorners && x != 0 && x != (uiXCount - 1) )
			{
				// Not possible to be in a corner...leave the reference buffer zero
				continue;
			}

			FLOAT_COLOR resultColor;

			// Load the render target pixel result and apply necessary type cast
			ColorTypeCast( *(const FLOAT_COLOR*)((const BYTE*)texmap.pData + x * uiCbRTStride + y * texmap.RowPitch ),
				           formatType,
						   m_destMask,
					       &resultColor );

			//
			// Build the address location for the gradient buffer
			//

			UINT texCoords[4] = { (UINT)( tx + fdtx0 ),
				                  (UINT)( ty + fdty0 ),
								  (UINT)( tz + fdtz0 ),
								  0 };

			if( bOnlyCheckFourCorners )
			{
				// Use the vertex texture coords directly...
				float *pVertices = NULL;
				if( x == 0 && y == 0 )
				{
					pVertices = vertexTexCoords[0];
				}
				else if ( x == 0 && y == (uiYCount - 1) )
				{
					pVertices = vertexTexCoords[1];
				}
				else if ( x == (uiXCount - 1) && y == 0 )
				{
					pVertices = vertexTexCoords[2];
				}
				else if ( x == (uiXCount - 1) && y == (uiYCount - 1) )
				{
					pVertices = vertexTexCoords[3];
				}
				else
				{
					// It is not one of the corners.
					assert(false);
				}

				if( pVertices )
				{
					for( UINT i = 0; i < NUM_VERTICES; i++ )
					{
						const float currFloat = pVertices[i];
						texCoords[i] = currFloat < 0.0f ? 0 : (UINT)currFloat;
					}
				}
			}

			// texCords[4] contains post-swizzle addresses
			UINT u = texCoords[ 0 ];
			UINT v = texCoords[ 1 ];
			UINT w = texCoords[ 2 ];
			if ( srcAddressIndices[0] == srcAddressIndices[1])
				v = u;
			if ( srcAddressIndices[1] == srcAddressIndices[2])
				w = v;
			if ( srcAddressIndices[0] == srcAddressIndices[2])
				w = u;

			UINT uiCbAddress = 0xffffffff;

			switch ( m_resourceType )
			{
			case RESOURCE_BUFFER:
			case RESOURCE_TEXTURE1D:
				if ( u < width )
				{
					uiCbAddress = u;
				}
				break;

			case RESOURCE_TEXTURE1D_ARRAY:
				if ( u < width &&
					 v < arraySize )
				{
					uiCbAddress = u + v * uiCbArraySize;
				}
				break;

			case RESOURCE_TEXTURE2D:
				if ( u < width &&
					 v < height )
				{
					uiCbAddress = u + v * width;
				}
				break;

			case RESOURCE_TEXTURE2D_ARRAY:
				if ( u < width &&
					 v < height &&
					 w < arraySize )
				{
					uiCbAddress = u + v * width + w * uiCbArraySize;
				}
				break;

			case RESOURCE_TEXTURE3D:
				if ( u < width &&
					 v < height &&
					 w < depth )
				{
					uiCbAddress = u + v * width + w * width * height;
				}
				break;
			}

			BYTE* pbExpected;

			if ( 0xffffffff != uiCbAddress )
			{
				pbExpected = m_pbGradients + ( uiCbAddress + uiCbMipOffset ) * uiCbRSStride;
			}
			else
			{
				// The address is out of bound,
				// point the gradient address to the zero buffer
				pbExpected = ZeroBuffer;
			}

			FLOAT_COLOR expectedColor;

			// Load/Convert gradient expected color
			if ( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT,
												m_resFormat,
												&expectedColor,
												1,
												1,
												pbExpected ) )
			{
				WriteToLog( _T("CFormatHelper::ConvertPixels() failed.") );
				pContext->Unmap( m_pRTTexture2DCopy, 0 );
				return false;
			}

			// Iterate thru each color component
			for ( UINT k = 0; k < 4; ++k )
			{
				float fResult = resultColor.comp[k];
				float fExpected = expectedColor.comp[ srcSwizzleIndices[k] ];

				if ( m_destMask != AssemblyHelper::noMask )
				{
					// Apply the destination register mask
					switch ( k )
					{
					case 0: if ( !( m_destMask & AssemblyHelper::x ) ) { fExpected = DEFAULT_VALUE; } break;
					case 1: if ( !( m_destMask & AssemblyHelper::y ) ) { fExpected = DEFAULT_VALUE; } break;
					case 2: if ( !( m_destMask & AssemblyHelper::z ) ) { fExpected = DEFAULT_VALUE; } break;
					case 3: if ( !( m_destMask & AssemblyHelper::w ) ) { fExpected = DEFAULT_VALUE; } break;
					}
				}

				// Compare the result
				FORMAT_TYPE ft = GetFormatType(m_resFormat);
				if( FORMAT_UNORM == ft ||
					FORMAT_SNORM == ft ||
					FORMAT_FLOAT == ft )
				{
					float fUlpDiff = 0;
					if(!CompareResultWithFormatULP( fExpected, fResult, m_resFormat, k, fUlpDiff))
					{
						bResult = false;

						if ( bLogVerbose || (numULPFailuresLogged < numULPFailuresToLog) )
						{
							WriteToLog( _T( "CLd_uav_typed::AnalyzeResult() failed at pixel (%d, %d), component %d is different." ), x, y, k );
							WriteToLog( _T( "  Expected value is %g (%#x)" ), fExpected, *((UINT32*)&fExpected) );
							WriteToLog( _T( "  Output value is %g (%#x)" ), fResult, *((UINT32*)&fResult) );
							WriteToLog( _T( "  Ulp difference: %+ f" ), fUlpDiff );
							numULPFailuresLogged++;
						}
						else if( !bLogVerbose && (numULPFailuresLogged == numULPFailuresToLog) )
						{
							// We will only hit this message once
							WriteToLog( _T( "CLd_uav_typed::AnalyzeResult() has additional failures that are not logged to reduce log size. Please run the group again with -LogVerbose to log the additional failures." ));
							goto Cleanup;
						}
					}
				}
				else
				{
					if( fExpected != fResult )
					{
						bResult = false;

						if ( bLogVerbose || (numNonULPFailuresLogged < numNonULPFailuresToLog) )
						{
							WriteToLog( _T( "CLd_uav_typed::AnalyzeResult() failed at pixel (%d, %d), component %d is different. \tExpected value is %f \tOutput value is %f" ), x, y, k, fExpected, fResult );
							numNonULPFailuresLogged++;
						}
						else if( !bLogVerbose && (numNonULPFailuresLogged == numNonULPFailuresToLog) )
						{
							// We will only hit this message once
							WriteToLog( _T( "CLd_uav_typed::AnalyzeResult() has additional failures that are not logged to reduce log size. Please run the group again with -LogVerbose to log the additional failures." ));
							goto Cleanup;
						}
					}
				}
			}

			tx += fdtx;
			tz += fdtz;
		}

		ty += fdty;
	}

Cleanup:
	GetImmediateContext()->Unmap( m_pRTTexture2DCopy, 0 );

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
void
CStore_uav_typed::InitTestParameters()
{
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Define supported shader stages
	CUserValueSet<D3D_SHADER_STAGE>* const pShaderStages = new CUserValueSet<D3D_SHADER_STAGE>;
	pShaderStages->AddValue( D3D_SHADER_STAGE_VERTEX );
	pShaderStages->AddValue( D3D_SHADER_STAGE_GEOMETRY );
	pShaderStages->AddValue( D3D_SHADER_STAGE_PIXEL );
	pShaderStages->AddValue( D3D_SHADER_STAGE_HULL );
	pShaderStages->AddValue( D3D_SHADER_STAGE_DOMAIN );
	testfactor::RFactor rfNonComputeShaders = this->AddParameterValueSet<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), pShaderStages );
	testfactor::RFactor rfPixelShader = this->AddParameterValue<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );
	testfactor::RFactor rfComputeShader = this->AddParameterValue<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_COMPUTE );

	// Define supported resources
	CTestCaseParameter<RESOURCE_TYPE>* const pResourceType = this->AddParameter( _T("ResourceType"), &m_resourceType );
	testfactor::RFactor rfResBuffer     = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_BUFFER );
	testfactor::RFactor rfResTex1D      = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_TEXTURE1D );
	testfactor::RFactor rfResTex1DArray = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_TEXTURE1D_ARRAY );
	testfactor::RFactor rfResTex2D      = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_TEXTURE2D );
	testfactor::RFactor rfResTex2DArray = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_TEXTURE2D_ARRAY );
	testfactor::RFactor rfResTex3D      = this->AddParameterValue<RESOURCE_TYPE>( pResourceType, RESOURCE_TEXTURE3D );

	// Define supported formats
	CUserValueSet<DXGI_FORMAT>* const pFormats = new CUserValueSet<DXGI_FORMAT>;
	pFormats->AddValue( DXGI_FORMAT_R32G32B32A32_FLOAT );
    pFormats->AddValue( DXGI_FORMAT_R16G16B16A16_FLOAT );
    pFormats->AddValue( DXGI_FORMAT_R32G32_FLOAT );
	pFormats->AddValue( DXGI_FORMAT_R11G11B10_FLOAT );
    pFormats->AddValue( DXGI_FORMAT_R16G16_FLOAT );
    pFormats->AddValue( DXGI_FORMAT_R32_FLOAT );
    pFormats->AddValue( DXGI_FORMAT_R16_FLOAT );
	pFormats->AddValue( DXGI_FORMAT_R16G16B16A16_UNORM );
	pFormats->AddValue( DXGI_FORMAT_R8G8B8A8_UNORM );
	pFormats->AddValue( DXGI_FORMAT_R16G16_UNORM );
	pFormats->AddValue( DXGI_FORMAT_R8G8_UNORM );
	pFormats->AddValue( DXGI_FORMAT_R16_UNORM );
	pFormats->AddValue( DXGI_FORMAT_R8_UNORM );
    pFormats->AddValue( DXGI_FORMAT_R16G16B16A16_SNORM );
	pFormats->AddValue( DXGI_FORMAT_R10G10B10A2_UNORM );
	pFormats->AddValue( DXGI_FORMAT_R8G8B8A8_SNORM );
	pFormats->AddValue( DXGI_FORMAT_R16G16_SNORM );
	pFormats->AddValue( DXGI_FORMAT_R8G8_SNORM );
	pFormats->AddValue( DXGI_FORMAT_R16_SNORM );
	pFormats->AddValue( DXGI_FORMAT_R8_SNORM );
	pFormats->AddValue( DXGI_FORMAT_R32G32B32A32_UINT );
    pFormats->AddValue( DXGI_FORMAT_R16G16B16A16_UINT );
    pFormats->AddValue( DXGI_FORMAT_R32G32_UINT );
    pFormats->AddValue( DXGI_FORMAT_R10G10B10A2_UINT );
    pFormats->AddValue( DXGI_FORMAT_R8G8B8A8_UINT );
    pFormats->AddValue( DXGI_FORMAT_R16G16_UINT );
    pFormats->AddValue( DXGI_FORMAT_R32_UINT );
    pFormats->AddValue( DXGI_FORMAT_R8G8_UINT );
    pFormats->AddValue( DXGI_FORMAT_R16_UINT );
    pFormats->AddValue( DXGI_FORMAT_R8_UINT );
    pFormats->AddValue( DXGI_FORMAT_R8G8B8A8_SINT );
    pFormats->AddValue( DXGI_FORMAT_R32G32B32A32_SINT );
    pFormats->AddValue( DXGI_FORMAT_R16G16B16A16_SINT );
    pFormats->AddValue( DXGI_FORMAT_R32G32_SINT );
    pFormats->AddValue( DXGI_FORMAT_R16G16_SINT );
    pFormats->AddValue( DXGI_FORMAT_R32_SINT );
    pFormats->AddValue( DXGI_FORMAT_R8G8_SINT );
    pFormats->AddValue( DXGI_FORMAT_R16_SINT );
    pFormats->AddValue( DXGI_FORMAT_R8_SINT );
	testfactor::RFactor rfFormat = this->AddParameter( _T("ResFormat"), &m_resFormat, pFormats );

	// Define supported resource width values
	CUserValueSet<UINT>* const pWidthValues = new CUserValueSet<UINT>;
	pWidthValues->AddValue( MAX_WIDTH * 1 );
    pWidthValues->AddValue( MAX_WIDTH * 2 );
    pWidthValues->AddValue( MAX_WIDTH * 4 );
	testfactor::RFactor rfResWidth = this->AddParameter( _T("ResWidth"), &m_resWidth, pWidthValues );

	// Define supported resource height values
	CUserValueSet<UINT>* const pHeightValues = new CUserValueSet<UINT>;
	pHeightValues->AddValue( MAX_HEIGHT * 1 );
    pHeightValues->AddValue( MAX_HEIGHT * 2 );
    pHeightValues->AddValue( MAX_HEIGHT * 4 );
	testfactor::RFactor rfResHeight = this->AddParameter( _T("ResHeight"), &m_resHeight, pHeightValues );

	// Define supported resource depth values
	CUserValueSet<UINT>* const pDepthValues = new CUserValueSet<UINT>;
	pDepthValues->AddValue( MAX_DEPTH * 1 );
    pDepthValues->AddValue( MAX_DEPTH * 2 );
    pDepthValues->AddValue( MAX_DEPTH * 4 );
	testfactor::RFactor rfResDepth = this->AddParameter( _T("ResDepth"), &m_resDepth, pDepthValues );

	// Define supported resource mip levels
	CUserValueSet<UINT>* const pMipLevelValues = new CUserValueSet<UINT>;
	for ( UINT i = 1; i <= MAX_MIP_COUNT; ++i )
	{
		pMipLevelValues->AddValue( i );
	}
	testfactor::RFactor rfResMipLevels = this->AddParameter( _T("ResMipLevels"), &m_resMipLevels, pMipLevelValues );

	// Define supported resource mip slice
	CUserValueSet<UINT>* const pMipSliceValues = new CUserValueSet<UINT>;
	for ( UINT i = 0; i < MAX_MIP_COUNT; ++i )
	{
		pMipSliceValues->AddValue( i );
	}
	testfactor::RFactor rfResMipSlice = this->AddParameter( _T("ResMipSlice"), &m_resMipSlice, pMipSliceValues );

	// Define supported resource array size
	CUserValueSet<UINT>* const pArraySizeValues = new CUserValueSet<UINT>;
	for ( UINT i = 1; i <= MAX_ARRAY_SIZE; ++i )
	{
		pArraySizeValues->AddValue( i );
	}
	testfactor::RFactor rfResArraySize = this->AddParameter( _T("ResArraySize"), &m_resArraySize, pArraySizeValues );

	// Define supported resource array slice
	CUserValueSet<UINT>* pArraySliceValues = new CUserValueSet<UINT>;
	for ( UINT i = 0; i < MAX_ARRAY_SIZE; ++i )
	{
		pArraySliceValues->AddValue( i );
	}
	testfactor::RFactor rfResArraySlice = this->AddParameter( _T("ResArraySlice"), &m_resArraySlice, pArraySliceValues );

	// Define supported destination address values
	CUserValueSet<AssemblyHelper::Swizzle>* const pAddressValues = new CUserValueSet<AssemblyHelper::Swizzle>();
	for ( UINT i = 0; i < _countof( g_pSwizzles ); ++i )
	{
		pAddressValues->AddValue( g_pSwizzles[i] );
	}
	testfactor::RFactor rfSrcAddress = this->AddParameter( _T("SrcAddressSwizzle"), &m_srcAddressSwizzle, pAddressValues );

	// Define supported source swizzle values
	CUserValueSet<AssemblyHelper::Swizzle>* const pSwizzleValues = new CUserValueSet<AssemblyHelper::Swizzle>();
	for ( UINT i = 0; i < _countof( g_pSwizzles ); ++i )
	{
		pSwizzleValues->AddValue( g_pSwizzles[i] );
	}
	testfactor::RFactor rfSrcSwizzle = this->AddParameter( _T("SrcSwizzle"), &m_srcSwizzle, pSwizzleValues );

	// Define supported UAV slots
	CTestCaseParameter<UINT>* const pUAVSlot = this->AddParameter( _T("UAVSlot"), &m_UAVSlot );
	testfactor::RFactor rfUAVSlot0 = this->AddParameterValue<UINT>( pUAVSlot, 0 );
	testfactor::RFactor rfUAVSlot1 = this->AddParameterValue<UINT>( pUAVSlot, 1 );
	testfactor::RFactor rfUAVSlot2 = this->AddParameterValue<UINT>( pUAVSlot, 2 );
	testfactor::RFactor rfUAVSlot3 = this->AddParameterValue<UINT>( pUAVSlot, 3 );
	testfactor::RFactor rfUAVSlot4 = this->AddParameterValue<UINT>( pUAVSlot, 4 );
	testfactor::RFactor rfUAVSlot5 = this->AddParameterValue<UINT>( pUAVSlot, 5 );
	testfactor::RFactor rfUAVSlot6 = this->AddParameterValue<UINT>( pUAVSlot, 6 );
	testfactor::RFactor rfUAVSlot7 = this->AddParameterValue<UINT>( pUAVSlot, 7 );
	testfactor::RFactor rfUAVSlots63 = this->AddParameterValueSet( pUAVSlot, new CRangeValueSet<UINT>(1, 64 - 1, 1) );	// FL11.1+ supports 64 UAV slots
	testfactor::RFactor rfUAVSlots64 = this->AddParameterValueSet( pUAVSlot, new CRangeValueSet<UINT>(0, 64 - 1, 1) );	// FL11.1+ supports 64 UAV slots

	// Set the test root factor
	testfactor::RFactor rfTexFactor = rfResMipLevels * rfResMipSlice;
	testfactor::RFactor rfTexArrayFactor = ( rfResArraySize * rfResArraySlice ) % rfTexFactor;
	testfactor::RFactor rfResFactor =
		rfResBuffer * rfFormat % rfResWidth +
		rfResTex1D * rfFormat % ( rfResWidth * rfTexFactor ) +
		rfResTex1DArray * rfFormat % ( rfResWidth * rfTexArrayFactor ) +
		rfResTex2D * rfFormat % ( rfResWidth * rfResHeight * rfTexFactor ) +
		rfResTex2DArray * rfFormat % ( rfResWidth * rfResHeight * rfTexArrayFactor ) +
		rfResTex3D * rfFormat % ( rfResWidth * rfResHeight * rfResDepth * rfTexFactor );

	if( g_TestApp.IsFL11_1OrGreater() )
	{
		this->SetRootTestFactor(
			  ( ( rfNonComputeShaders * rfSrcAddress * rfResFactor ) % rfSrcSwizzle ) % rfUAVSlots63 +	// Don't use UAV slot 0, [1, 63]
			  ( ( rfComputeShader * rfSrcAddress * rfResFactor ) % rfSrcSwizzle ) % rfUAVSlots64 );		// UAV slot [0,63]
	}
	else
	{
		this->SetRootTestFactor(
			  ( ( rfPixelShader * rfSrcAddress * rfResFactor ) % rfSrcSwizzle ) %
				( rfUAVSlot1 + rfUAVSlot2 + rfUAVSlot3 + rfUAVSlot4 + rfUAVSlot5 + rfUAVSlot6 + rfUAVSlot7 ) +
			  ( ( rfComputeShader * rfSrcAddress * rfResFactor ) % rfSrcSwizzle ) %
				( rfUAVSlot0 + rfUAVSlot1 + rfUAVSlot2 + rfUAVSlot3 + rfUAVSlot4 + rfUAVSlot5 + rfUAVSlot6 + rfUAVSlot7 ) );
	}

	// Skip invalid matches
	this->SkipMatching( ( FilterActive<UINT>( _T("ResMipLevels") ) &&
						  !LoadStoreTest::FilterValidMipSize( this, _T("ResMipLevels"), _T("ResWidth") ) ) ||
						( FilterActive<UINT>( _T("ResMipSlice") ) &&
						  !LoadStoreTest::FilterLess<UINT>( this, _T("ResMipSlice"), _T("ResMipLevels") ) )
						);

	AddPriorityPatternFilter(FilterEqual<RESOURCE_TYPE>(_T("ResourceType"), RESOURCE_BUFFER), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<RESOURCE_TYPE>(_T("ResourceType"), RESOURCE_TEXTURE2D), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResWidth"), MAX_WIDTH), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResWidth"), MAX_WIDTH * 2), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResHeight"), MAX_HEIGHT), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResHeight"), MAX_HEIGHT * 2), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResDepth"), MAX_DEPTH), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResDepth"), MAX_DEPTH * 2), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResMipLevels"), 1), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResMipSlice"), 0), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("ResFormat"), DXGI_FORMAT_R32G32B32A32_FLOAT), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("ResFormat"), DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("ResFormat"), DXGI_FORMAT_R32_UINT), WeightFactorVeryLarge);

	DeclarePriorityLevelByPriorityValue( 1, 3.375f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//-----------------------------------------------------------------------------
TEST_RESULT
CStore_uav_typed::Setup()
{
	// UAV typed buffers are only supported on DX11 and above
	if ( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog(_T("UAV typed buffers are only supported on FL11.0 and above."));
		return RESULT_SKIP;
	}

	return CLoadStoreTest::Setup();
}

//-----------------------------------------------------------------------------
TEST_RESULT
CStore_uav_typed::SetupTestCase()
{
	if( !InitCB() )
	{
		WriteToLog( _T( "CStore_uav_typed::SetupTestCase() - InitCB() failed." ) );
		return RESULT_FAIL;
	}

	// some swizzle (for tex2d and tex3d) may cause undefined results, skip them
	// Also some swizzle (for 1DArray and 2DArray) may cause the array index to vary for the same test case.
	// Ideally, it should be handled by a more complex AnalyzeResult().
	// Skip them for now, since the overall test cases have covered the range of
	// array indexing.
	int srcAddressIndices[4];
	m_srcAddressSwizzle.ToIndices( srcAddressIndices );
	if ( m_resourceType == RESOURCE_TEXTURE1D_ARRAY || m_resourceType == RESOURCE_TEXTURE2D)
	{
		if ( srcAddressIndices[0] == srcAddressIndices[1] )
		{
			WriteToLog(_T("Skip test case because of swizzle mismatch."));
			return RESULT_SKIP;
		}
	}
	else if ( m_resourceType == RESOURCE_TEXTURE2D_ARRAY || m_resourceType == RESOURCE_TEXTURE3D)
	{
		if ( srcAddressIndices[0] == srcAddressIndices[1] ||
			srcAddressIndices[0] == srcAddressIndices[2] ||
			srcAddressIndices[1] == srcAddressIndices[2])
		{
			WriteToLog(_T("Skip test case because of swizzle mismatch."));
			return RESULT_SKIP;
		}
	}


	return CLoadStoreTest::SetupTestCase();
}

//-----------------------------------------------------------------------------
TEST_RESULT
CStore_uav_typed::ExecuteTestCase()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();

	const UINT uiCbStride = sizeof( VSINPUT );
	const UINT uiOffset = 0;

	switch ( m_ShaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
	case D3D_SHADER_STAGE_PIXEL:

		if ( !this->InitPipelineShaders() )
		{
			WriteError( _T( "CStore_uav_typed::InitPipelineShaders() failed" ) );
			return RESULT_FAIL;
		}

		pContext->OMSetRenderTargetsAndUnorderedAccessViews( 0, NULL, NULL, m_UAVSlot, 1, &m_pUAV, NULL );
		pContext->RSSetState( m_pRSEnabled );

		switch( m_ShaderStage )
		{
		default:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
			break;
		case D3D_SHADER_STAGE_PIXEL:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			break;
		case D3D_SHADER_STAGE_HULL:
		case D3D_SHADER_STAGE_DOMAIN:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST );
			break;
		}

		pContext->IASetVertexBuffers( 0, 1, &m_pVsInput, &uiCbStride, &uiOffset );
		pContext->IASetInputLayout( m_pInputLayout );
		pContext->VSSetConstantBuffers( 0, 1, &m_pCB );
		pContext->HSSetConstantBuffers( 0, 1, &m_pCB );
		pContext->DSSetConstantBuffers( 0, 1, &m_pCB );
		pContext->GSSetConstantBuffers( 0, 1, &m_pCB );
		pContext->PSSetConstantBuffers( 0, 1, &m_pCB );
		pContext->Draw( NUM_VERTICES, 0 );
		break;

	case D3D_SHADER_STAGE_COMPUTE:

		if ( !this->InitCSShaders() )
		{
			WriteError( _T( "CStore_uav_typed::InitCSShaders() failed" ) );
			return RESULT_FAIL;
		}

		pContext->CSSetShaderResources( 0, 1, &m_pCsInputSRV );
		pContext->CSSetUnorderedAccessViews( m_UAVSlot, 1, &m_pUAV, NULL );
		pContext->CSSetConstantBuffers( 0, 1, &m_pCB );
		pContext->CSSetShader( m_pCS, NULL, 0 );
		pContext->Dispatch( DISP_WIDTH, DISP_HEIGHT, 1 );
		break;
	}

	hr = this->ExecuteEffectiveContext();
	if ( FAILED( hr ) )
	{
		WriteToLog( _T("this->ExecuteEffectiveContext() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	return this->AnalyzeResult() ? RESULT_PASS : RESULT_FAIL;
}

//-----------------------------------------------------------------------------
void
CStore_uav_typed::CleanupTestCase()
{
	// Cleanup CB
	SAFE_RELEASE( m_pCB );

	// Call parent's cleanup
	CLoadStoreTest::CleanupTestCase();
}

//-----------------------------------------------------------------------------
bool
CStore_uav_typed::InitCB()
{
	//
	// We use the CB to store the multiplier for the src0 of the store_uav_typed instruction
	//

	struct CBufferLayout
	{
		float color[4];
	};

	// Local variables
	CBufferLayout cbData;
	HRESULT hr = S_OK;
	FLOAT_COLOR color = { 1.0f, 0.8f, 0.6f, 0.4f };
	ScaleIntegerColor( color, m_resFormat, &color );
	const float fColorWidth = ( m_resWidth + 2 * m_outOfBoundsOffset );
	const float fColorHeight = ( m_resHeight + 2 * m_outOfBoundsOffset );
	const float fColorDepth = ( m_resDepth + 2 * m_outOfBoundsOffset );

	// Create the buffer
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.ByteWidth = sizeof( cbData );
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;
	buffDesc.StructureByteStride = 0;

	if( FAILED( hr = GetDevice()->CreateBuffer( &buffDesc, NULL, &m_pCB ) ) )
	{
		WriteToLog( _T("CStore_uav_typed::InitCB() - CreateBuffer() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	//
	// Fill the buffer
	//

	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
		color.comp[0] /= fColorWidth;
		color.comp[1] /= fColorWidth;
		color.comp[2] /= fColorWidth;
		color.comp[3] /= fColorWidth;
		break;


	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
		color.comp[0] /= fColorWidth;
		color.comp[1] /= fColorHeight;
		color.comp[2] /= fColorHeight;
		color.comp[3] /= fColorHeight;
		break;


	case RESOURCE_TEXTURE3D:
		color.comp[0] /= fColorWidth;
		color.comp[1] /= fColorHeight;
		color.comp[2] /= fColorDepth;
		color.comp[3] /= fColorDepth;
		break;
	}

	// Put the computed colors in the constant buffer layout struct
	for( UINT i = 0; i < 4; ++i )
		cbData.color[i] = color.comp[i];

	if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pCB, cbData ) ) )
	{
		WriteToLog( _T("CStore_uav_typed::InitCB() failed to map m_pCB, hr = %s." ), D3DHResultToString(hr).c_str() );
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
bool
CStore_uav_typed::InitPipelineShaders()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	const tstring strSrcSwizzle	= ToString( m_srcSwizzle );
	const tstring strSrcAddress = ToString( m_srcAddressSwizzle );
	const tstring strResType	= ToAsmName( m_resourceType );
	const FORMAT_TYPE formatType = GetFormatType( m_resFormat );
	const tstring strFormat		= ToAsmName( formatType );

	tstring strColorSwizzle = "";

	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
		strColorSwizzle = ToString( m_srcAddressSwizzle, 1 );
		break;


	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
		strColorSwizzle = ToString( m_srcAddressSwizzle, 2 );
		break;


	case RESOURCE_TEXTURE3D:
		strColorSwizzle = ToString( m_srcAddressSwizzle, 3 );
		break;
	}

	AssemblyHelper asmHelper( 0, 3 );
	asmHelper.SetCustomUAVDecl( m_UAVSlot, FormatString( "dcl_uav_typed u%d, %s, %s", m_UAVSlot, strResType.c_str(), strFormat.c_str() ) );
	asmHelper.SetConstantBufferSize( 0, 1 );
	asmHelper.SetupMovToTempFromCB( 2, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );

	// Position is in V[0], Address is in V[1], Offset is in V[2]. LD output in r4.
	asmHelper.SetPositionInputRegIdx(0);
	asmHelper.SetUserInputRegMaskTyped( 1, AssemblyHelper::xyzw, "linear", "float" );
	asmHelper.SetUserInputRegMaskTyped( 2, AssemblyHelper::xyzw, "linear", "float" );
	asmHelper.SetupMovToOutputFromTemp( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );

	if( m_ShaderStage != D3D_SHADER_STAGE_PIXEL )
	{
		asmHelper.SetPositionOutputRegIdx(5);
		asmHelper.SetupMovToOutputFromInput( 5, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );	// Move position from 0 to 5
	}

	asmHelper.SetOutputRegMaskTyped( 0, AssemblyHelper::xyzw, "float" );

	tstring strShaderCode = "";

	switch( m_ShaderStage )
	{
	default:
		strShaderCode += FormatString( "mul r1, v[1].%s, r2\n", strColorSwizzle.c_str());
		strShaderCode += "ftou r0, v[1]\n";
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
		strShaderCode += FormatString( "mul r1, v[0][1].%s, r2\n", strColorSwizzle.c_str());
		strShaderCode += "ftou r0, v[0][1]\n";
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		strShaderCode += FormatString( "mul r1, vcp[0][1].%s, r2\n", strColorSwizzle.c_str());
		strShaderCode += "ftou r0, vcp[0][1]\n";
		break;
	}

	switch ( formatType )
	{
	case FORMAT_UINT:
		strShaderCode += _T("ftou r1, r1\n");
		break;

	case FORMAT_SINT:
		strShaderCode += _T("ftoi r1, r1\n");
		break;
	}

	strShaderCode += FormatString( "store_uav_typed u%d.xyzw, r0.%s, r1.%s",
		m_UAVSlot, strSrcAddress.c_str(), strSrcSwizzle.c_str() );

	ID3D10Blob* pShaderBuf = NULL;
	hr = asmHelper.SetupPipelineWithShaderCode( pDevice,
		                                        pContext,
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												strShaderCode.c_str(),
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );
	if ( FAILED( hr ) )
	{
		WriteError( _T("AssemblyHelper::SetupPipelineWithShaderCode() failed, hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	if ( g_bDumpDissasm )
	{
		this->MessageDumpShaderDisassembly( (UINT*)pShaderBuf->GetBufferPointer(),
			                                pShaderBuf->GetBufferSize() );
	}

	SAFE_RELEASE( pShaderBuf );

	return true;
}

//-----------------------------------------------------------------------------
bool
CStore_uav_typed::InitCSShaders()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	const tstring strSrcSwizzle	= ToString( m_srcSwizzle );
	const tstring strSrcAddressSwizzle = ToString( m_srcAddressSwizzle );
	const tstring strResType	= ToAsmName( m_resourceType );
	const FORMAT_TYPE formatType = GetFormatType( m_resFormat );
	const tstring strFormat		= ToAsmName( formatType );

	tstring strColorSwizzle = "";

	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
		strColorSwizzle = ToString( m_srcAddressSwizzle, 1 );
		break;

	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
		strColorSwizzle = ToString( m_srcAddressSwizzle, 2 );
		break;

	case RESOURCE_TEXTURE3D:
		strColorSwizzle = ToString( m_srcAddressSwizzle, 3 );
		break;
	}

	AssemblyHelper asmHelper( 0, 3 );
	asmHelper.SetCustomDecl( 0, FormatString( "dcl_resource_structured t%d, %d", 0, sizeof( CSINPUT ) ) );
	asmHelper.SetOutputRegMaskTyped( m_UAVSlot, AssemblyHelper::xyzw, strFormat.c_str(), strResType.c_str() );
	asmHelper.SetConstantBufferSize( 0, 1 );
	asmHelper.SetupMovToTempFromCB( 2, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );

	tstring strShaderCode = "";
	strShaderCode += FormatString( "imad r1, vThreadGroupID.y, vec4(%d, 0, 0, 0), vThreadGroupID.x\n", DISP_WIDTH );
	strShaderCode += "ld_structured r0, r1.x, 0, t0\n";
	strShaderCode += FormatString( "mul r1, r0.%s, r2\n", strColorSwizzle.c_str() );
	strShaderCode += "ftou r0, r0\n";

	switch ( formatType )
	{
	case FORMAT_UINT:
		strShaderCode += _T("ftou r1, r1\n");
		break;

	case FORMAT_SINT:
		strShaderCode += _T("ftoi r1, r1\n");
		break;
	}

	strShaderCode += FormatString( "store_uav_typed u%d.xyzw, r0.%s, r1.%s",
		m_UAVSlot, strSrcAddressSwizzle.c_str(), strSrcSwizzle.c_str() );

	ID3D10Blob* pShaderBuf = NULL;
	hr = asmHelper.SetupPipelineWithShaderCode( pDevice,
		                                        pContext,
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												strShaderCode.c_str(),
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );
	if ( FAILED( hr ) )
	{
		WriteError( _T("AssemblyHelper::SetupPipelineWithShaderCode() failed, hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	if ( g_bDumpDissasm )
	{
		this->MessageDumpShaderDisassembly( (UINT*)pShaderBuf->GetBufferPointer(),
			                                pShaderBuf->GetBufferSize() );
	}

	SAFE_RELEASE( pShaderBuf );

	return true;
}

//-----------------------------------------------------------------------------
bool
CStore_uav_typed::AnalyzeResult()
{
	HRESULT hr;

	bool bResult = true;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	const bool bLogVerbose = ((CShader5xTestApp*)GetFramework())->LogVerbose();
	const UINT numULPFailuresToLog = NUM_FAILURES_TO_LOG;
	const UINT numNonULPFailuresToLog = NUM_FAILURES_TO_LOG;
	UINT numULPFailuresLogged = 0;
	UINT numNonULPFailuresLogged = 0;

	int srcSwizzleIndices[4];
	m_srcSwizzle.ToIndices( srcSwizzleIndices );

	int srcAddressIndices[4];
	m_srcAddressSwizzle.ToIndices( srcAddressIndices );

	UINT width		= 1;
	UINT height		= 1;
	UINT depth		= 1;
	UINT mipLevels	= 1;
	UINT mipSlice	= 0;
	UINT arraySize	= 1;
	UINT arraySlice = 0;

	double tx0 = 0.0f - m_outOfBoundsOffset;
	double tx1 = 0.0f + m_outOfBoundsOffset;
	double ty0 = 0.0f - m_outOfBoundsOffset;
	double ty1 = 0.0f + m_outOfBoundsOffset;
	double tz0 = 0.0f - m_outOfBoundsOffset;
	double tz1 = 0.0f + m_outOfBoundsOffset;

	ID3D11Resource* pResource;
	ID3D11Resource* pResourceCopy;

	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
		width = m_resWidth;
		tx1 = (double)width + m_outOfBoundsOffset;
		pResource = m_pBuffer;
		pResourceCopy = m_pBufferCopy;
		break;

	case RESOURCE_TEXTURE1D_ARRAY:
		arraySize = m_resArraySize;
		arraySlice = m_resArraySlice;
		ty0 = ty1 = (double)arraySlice;
		__fallthrough;

	case RESOURCE_TEXTURE1D:
		width = m_resWidth;
		mipLevels = m_resMipLevels;
		mipSlice = m_resMipSlice;
		tx1 = (double)width + m_outOfBoundsOffset;
		pResource = m_pTexture1D;
		pResourceCopy = m_pTexture1DCopy;
		break;

	case RESOURCE_TEXTURE2D_ARRAY:
		arraySize = m_resArraySize;
		arraySlice = m_resArraySlice;
		tz0 = tz1 = (double)arraySlice;
		__fallthrough;

	case RESOURCE_TEXTURE2D:
		width  = m_resWidth;
		height = m_resHeight;
		mipLevels = m_resMipLevels;
		mipSlice = m_resMipSlice;
		tx1 = (double)width + m_outOfBoundsOffset;
		ty1 = (double)height + m_outOfBoundsOffset;
		pResource = m_pTexture2D;
		pResourceCopy = m_pTexture2DCopy;
		break;

	case RESOURCE_TEXTURE3D:
		width  = m_resWidth;
		height = m_resHeight;
		depth  = m_resDepth;
		mipLevels = m_resMipLevels;
		mipSlice = m_resMipSlice;
		tx1 = (double)width + m_outOfBoundsOffset;
		ty1 = (double)height + m_outOfBoundsOffset;
		tz1 = (double)depth + m_outOfBoundsOffset;
		pResource = m_pTexture3D;
		pResourceCopy = m_pTexture3DCopy;
		break;

	default:
		WriteToLog( _T("CStore_uav_typed::AnalyzeResult() failed, invalid resource type.") );
		return false;
	}

	const FORMAT_TYPE formatType = GetFormatType( m_resFormat );
	const UINT uiCbStride  = GetBitsPerElement( m_resFormat ) / 8;
	const UINT uiCbMipOffset = CalcMipSize( mipSlice,
						         			width,
											height,
											depth,
											&width,
											&height,
											&depth );

	const UINT uiCbRefBufSize = width * height * depth * uiCbStride;

	BYTE* const pbRefBuffer = new BYTE[uiCbRefBufSize];
	memset( pbRefBuffer, 0, uiCbRefBufSize );

	const UINT uiXCount = DISP_WIDTH;
	const UINT uiYCount = DISP_HEIGHT;

	const double fdtx = ( tx1 - tx0 ) / uiXCount;
	const double fdty = ( ty1 - ty0 ) / uiYCount;
	const double fdtz = ( tz1 - tz0 ) / uiXCount;

	const double fdtx0 = fdtx / 2;
	const double fdty0 = fdty / 2;
	const double fdtz0 = fdtz / 2;

	FLOAT_COLOR color = { 1.0f, 0.8f, 0.6f, 0.4f };
	ScaleIntegerColor( color, m_resFormat, &color );

	const double fColorWidth = ( m_resWidth + 2 * m_outOfBoundsOffset );
	const double fColorHeight = ( m_resHeight + 2 * m_outOfBoundsOffset );
	const double fColorDepth = ( m_resDepth + 2 * m_outOfBoundsOffset );

	double ty = ty0;

	// The VS etc, only use four verticies and point list, so only the four corners will light up
	assert( NUM_VERTICES == 4 );
	const bool bOnlyCheckFourCorners = m_bRenderUsingPointlist ? true : false;
	double vertexTexCoords[NUM_VERTICES][4] = {
	// Address
	{ tx0, ty1, tz0, 0.0f },
	{ tx0, ty0, tz0, 0.0f },
	{ tx1, ty1, tz1, 0.0f },
	{ tx1, ty0, tz1, 0.0f }};

	for ( UINT y = 0; y < uiYCount; ++y )
	{
		double tx = tx0;
		double tz = tz0;

		if( bOnlyCheckFourCorners && y != 0 && y != (uiYCount - 1) )
		{
			// Not possible to be in a corner...
			continue;
		}

		for ( UINT x = 0; x < uiXCount; ++x )
		{
			if( bOnlyCheckFourCorners && x != 0 && x != (uiXCount - 1) )
			{
				// Not possible to be in a corner...
				continue;
			}

			FLOAT_COLOR tmpColor;

			//
			// Build the address location
			//

			double texCoords[4] = { tx + fdtx0,
				                         ty + fdty0,
									     tz + fdtz0,
									     0.0f };

			if( bOnlyCheckFourCorners )
			{
				// Use the vertex texture coords directly...
				double *pVertices = NULL;
				if( x == 0 && y == 0 )
				{
					pVertices = vertexTexCoords[0];
				}
				else if ( x == 0 && y == (uiYCount - 1) )
				{
					pVertices = vertexTexCoords[1];
				}
				else if ( x == (uiXCount - 1) && y == 0 )
				{
					pVertices = vertexTexCoords[2];
				}
				else if ( x == (uiXCount - 1) && y == (uiYCount - 1) )
				{
					pVertices = vertexTexCoords[3];
				}
				else
				{
					// It is not one of the corners.
					assert(false);
				}

				if( pVertices )
				{
					for( UINT i = 0; i < NUM_VERTICES; i++ )
					{
						texCoords[i] = pVertices[i];
					}
				}
			}

			// texCords[4] contains post-swizzle addresses
			double fU = texCoords[ 0 ];
			double fV = texCoords[ 1 ];
			double fW = texCoords[ 2 ];

			if ( srcAddressIndices[0] == srcAddressIndices[1])
				fV = fU;
			if ( srcAddressIndices[1] == srcAddressIndices[2])
				fW = fV;
			if ( srcAddressIndices[0] == srcAddressIndices[2])
				fW = fU;

			const UINT u = fU < 0.0f ? 0 : (UINT)fU;
			const UINT v = fV < 0.0f ? 0 : (UINT)fV;
			const UINT w = fW < 0.0f ? 0 : (UINT)fW;

			UINT uiCbAddress = 0xffffffff;

			switch ( m_resourceType )
			{
			case RESOURCE_TEXTURE1D_ARRAY:
				if ( v >= arraySize )
				{
					break;
				}
				__fallthrough;

			case RESOURCE_BUFFER:
			case RESOURCE_TEXTURE1D:
				if ( u < width )
				{
					uiCbAddress = u;

					tmpColor.comp[0] = (float)(fU * color.comp[0] / fColorWidth);
					tmpColor.comp[1] = (float)(fU * color.comp[1] / fColorWidth);
					tmpColor.comp[2] = (float)(fU * color.comp[2] / fColorWidth);
					tmpColor.comp[3] = (float)(fU * color.comp[3] / fColorWidth);
				}
				break;

			case RESOURCE_TEXTURE2D_ARRAY:
				if ( w >= arraySize )
				{
					break;
				}
				__fallthrough;

			case RESOURCE_TEXTURE2D:
				if ( u < width &&
					 v < height )
				{
					uiCbAddress = u + v * width;

					tmpColor.comp[0] = (float)(fU * color.comp[0] / fColorWidth);
					tmpColor.comp[1] = (float)(fV * color.comp[1] / fColorHeight);
					tmpColor.comp[2] = (float)(fV * color.comp[2] / fColorHeight);
					tmpColor.comp[3] = (float)(fV * color.comp[3] / fColorHeight);
				}
				break;

			case RESOURCE_TEXTURE3D:
				if ( u < width &&
					 v < height &&
					 w < depth )
				{
					uiCbAddress = u + v * width + w * width * height;

					tmpColor.comp[0] = (float)(fU * color.comp[0] / fColorWidth);
					tmpColor.comp[1] = (float)(fV * color.comp[1] / fColorHeight);
					tmpColor.comp[2] = (float)(fW * color.comp[2] / fColorDepth);
					tmpColor.comp[3] = (float)(fW * color.comp[3] / fColorDepth);
				}
				break;
			}

			if ( 0xffffffff != uiCbAddress )
			{
				FLOAT_COLOR curColor;

				// Apply source swizzle
				curColor.comp[0] = tmpColor.comp[ srcSwizzleIndices[0] ];
				curColor.comp[1] = tmpColor.comp[ srcSwizzleIndices[1] ];
				curColor.comp[2] = tmpColor.comp[ srcSwizzleIndices[2] ];
				curColor.comp[3] = tmpColor.comp[ srcSwizzleIndices[3] ];

				BYTE* const pbRefColor = pbRefBuffer + uiCbAddress * uiCbStride;

				// Convert the gradient color to resource format
				// and write it into the reference buffer
				if ( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT,
													m_resFormat,
													&curColor,
													1,
													1,
													pbRefColor ) )
				{
					delete [] pbRefBuffer;
					WriteToLog( _T("CFormatHelper::ConvertPixels() failed.") );
					return false;
				}
			}

			tx += fdtx;
			tz += fdtz;
		}

		ty += fdty;
	}

	//
	// Validate the results
	//

	D3D11_MAPPED_SUBRESOURCE texmap;
	const UINT uiSubResource = arraySlice * mipLevels + mipSlice;
	pContext->CopySubresourceRegion( pResourceCopy, uiSubResource, 0, 0, 0, pResource, uiSubResource, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed") );
		return false;
	}

	hr = GetImmediateContext()->Map( pResourceCopy, uiSubResource, D3D11_MAP_READ, 0, &texmap );
	if ( FAILED( hr ) )
	{
		delete [] pbRefBuffer;
		WriteToLog( _T("ID3D11DeviceContext::Map() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	for ( UINT z = 0; z < depth; ++z )
	{
		for ( UINT y = 0; y < height; ++y )
		{
			for ( UINT x = 0; x < width; ++x )
			{
				//
				// Load/Convert result color
				//

				FLOAT_COLOR resultColor;
				BYTE* const pbResultColor = (BYTE*)texmap.pData + x * uiCbStride + y * texmap.RowPitch + z * texmap.DepthPitch;
				if ( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT,
													m_resFormat,
													&resultColor,
													1,
													1,
													pbResultColor ) )
				{
					pContext->Unmap( pResourceCopy, uiSubResource );
					delete [] pbRefBuffer;
					WriteToLog( _T("CFormatHelper::ConvertPixels() failed.") );
					return false;
				}

				//
				// Load/Convert expected color
				//

				FLOAT_COLOR expectedColor;
				BYTE* const pbRefColor = pbRefBuffer + ( x + y * width + z * width * height ) * uiCbStride;
				if ( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT,
													m_resFormat,
													&expectedColor,
													1,
													1,
													pbRefColor ) )
				{
					pContext->Unmap( pResourceCopy, uiSubResource );
					delete [] pbRefBuffer;
					WriteToLog( _T("CFormatHelper::ConvertPixels() failed.") );
					return false;
				}

				// Iterate thru each color component
				for ( UINT k = 0; k < 4; ++k )
				{
					const float fResult = resultColor.comp[k];
					float fExpected = expectedColor.comp[k];

					if ( m_destMask != AssemblyHelper::noMask )
					{
						// Apply the destination register mask
						switch ( k )
						{
						case 0: if ( !( m_destMask & AssemblyHelper::x ) ) { fExpected = 0.0f; } break;
						case 1: if ( !( m_destMask & AssemblyHelper::y ) ) { fExpected = 0.0f; } break;
						case 2: if ( !( m_destMask & AssemblyHelper::z ) ) { fExpected = 0.0f; } break;
						case 3: if ( !( m_destMask & AssemblyHelper::w ) ) { fExpected = 0.0f; } break;
						}
					}

					// Compare the result
					FORMAT_TYPE ft = GetFormatType(m_resFormat);
					if( FORMAT_UNORM == ft ||
						FORMAT_SNORM == ft ||
						FORMAT_FLOAT == ft )
					{
						float fUlpDiff = 0;
						if(!CompareResultWithFormatULP( fExpected, fResult, m_resFormat, k, fUlpDiff))
						{
							// Win7:717638
							// Errors are introduced due to the test relying on generating expected values and emulating the interpolator.
							// Thus we need to relax the ULP requirements. This relaxed ULP should be removed once the test is fixed. See bug for more details.
							const float relaxedULP = 250.0f;

							if( fUlpDiff > relaxedULP )
							{
								bResult = false;

								if ( bLogVerbose || (numULPFailuresLogged < numULPFailuresToLog) )
								{
									WriteToLog( _T( "CStore_uav_typed::AnalyzeResult() failed at pixel (%d, %d), component %d is different." ), x, y, k );
									WriteToLog( _T( "  Expected value is %g (%#x)" ), fExpected, *((UINT32*)&fExpected) );
									WriteToLog( _T( "  Output value is %g (%#x)" ), fResult, *((UINT32*)&fResult) );
									WriteToLog( _T( "  Ulp difference: %+ f" ), fUlpDiff );
									numULPFailuresLogged++;
								}
								else if( !bLogVerbose && (numULPFailuresLogged == numULPFailuresToLog) )
								{
									// We will only hit this message once
									WriteToLog( _T( "CStore_uav_typed::AnalyzeResult() has additional failures that are not logged to reduce log size. Please run the group again with -LogVerbose to log the additional failures." ));
									goto Cleanup;
								}
							}
						}
					}
					else
					{
						if( fExpected != fResult )
						{
							bResult = false;

							if ( bLogVerbose || (numNonULPFailuresLogged < numNonULPFailuresToLog) )
							{
								WriteToLog( _T( "CStore_uav_typed::AnalyzeResult() failed at pixel (%d, %d), component %d is different. \tExpected value is %f \tOutput value is %f" ), x, y, k, fExpected, fResult );
								numNonULPFailuresLogged++;
							}
							else if( !bLogVerbose && (numULPFailuresLogged == numULPFailuresToLog) )
							{
								// We will only hit this message once
								WriteToLog( _T( "CStore_uav_typed::AnalyzeResult() has additional failures that are not logged to reduce log size. Please run the group again with -LogVerbose to log the additional failures." ));
								goto Cleanup;
							}
						}
					}
				}
			}
		}
	}

Cleanup:
	delete [] pbRefBuffer;
	GetImmediateContext()->Unmap( pResourceCopy, uiSubResource );

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
void
CStore_raw::InitTestParameters()
{
	// Define static parameters
	m_resFormat = DXGI_FORMAT_R32_TYPELESS;
	m_resourceType = RESOURCE_BUFFER;

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Define supported shader stages
	CUserValueSet<D3D_SHADER_STAGE>* const pShaderStages = new CUserValueSet<D3D_SHADER_STAGE>;
	pShaderStages->AddValue( D3D_SHADER_STAGE_VERTEX );
	pShaderStages->AddValue( D3D_SHADER_STAGE_GEOMETRY );
	pShaderStages->AddValue( D3D_SHADER_STAGE_PIXEL );
	pShaderStages->AddValue( D3D_SHADER_STAGE_HULL );
	pShaderStages->AddValue( D3D_SHADER_STAGE_DOMAIN );
	testfactor::RFactor rfNonComputeShaders = this->AddParameterValueSet<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), pShaderStages );
	testfactor::RFactor rfPixelShader = this->AddParameterValue<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );
	testfactor::RFactor rfComputeShader = this->AddParameterValue<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_COMPUTE );

	// Define TGSM usage
	CTestCaseParameter<bool>* const pUseTGSM = AddParameter( _T( "UseTGSM" ), &m_bUseTGSM );
	testfactor::RFactor rfUseTGSM = AddParameterValueSet( pUseTGSM, BoolValueSet() );
	SetParameterDefaultValue( pUseTGSM, false );

	// Define supported resource width values
	CUserValueSet<UINT>* const pWidthValues = new CUserValueSet<UINT>;
	pWidthValues->AddValue( MAX_WIDTH * 1 );
    pWidthValues->AddValue( MAX_WIDTH * 2 );
    pWidthValues->AddValue( MAX_WIDTH * 4 );
	testfactor::RFactor rfResWidth = this->AddParameter( _T("ResWidth"), &m_resWidth, pWidthValues );

	// Define supported destination address values
	CUserValueSet<AssemblyHelper::Swizzle>* const pAddressValues = new CUserValueSet<AssemblyHelper::Swizzle>();
	pAddressValues->AddValue( g_pSwizzles[4] );	// x
	pAddressValues->AddValue( g_pSwizzles[5] );	// y
	pAddressValues->AddValue( g_pSwizzles[6] );	// z
	pAddressValues->AddValue( g_pSwizzles[7] );	// w
	testfactor::RFactor rfSrcAddress = this->AddParameter( _T("SrcAddressSwizzle"), &m_srcAddressSwizzle, pAddressValues );

	// Define supported source swizzle values
	CUserValueSet<AssemblyHelper::Swizzle>* const pSwizzleValues = new CUserValueSet<AssemblyHelper::Swizzle>();
	for ( UINT i = 0; i < _countof( g_pSwizzles ); ++i )
	{
		pSwizzleValues->AddValue( g_pSwizzles[i] );
	}
	testfactor::RFactor rfSrcSwizzle = this->AddParameter( _T("SrcSwizzle"), &m_srcSwizzle, pSwizzleValues );

	// Define supported destination masks
	CUserValueSet<AssemblyHelper::MaskFlag>* const pMaskValues = new CUserValueSet<AssemblyHelper::MaskFlag>();
	pMaskValues->AddValue( g_pMasks[0] );	// xyzw
	pMaskValues->AddValue( g_pMasks[1] );	// xyz
	pMaskValues->AddValue( g_pMasks[3] );	// xy
	pMaskValues->AddValue( g_pMasks[7] );	// x
	testfactor::RFactor rfDestMask = this->AddParameter( _T("DestMask"), &m_destMask, pMaskValues );

	// Define supported UAV slots
	CUserValueSet<UINT>* const pUAVSlots = new CUserValueSet<UINT>;
	UINT uUAVStartSlot = 0;
	UINT uUAVMaxSlot = 0;
	testfactor::RFactor rfUAVSlot;
	if( g_TestApp.IsFL11_1OrGreater() )
	{
		uUAVStartSlot = 1;	// Skip slot 0 because RTV is in slot 0
		uUAVMaxSlot = 63;	// 64 - 1
	}
	else if( g_TestApp.IsFL11OrGreater() )
	{
		uUAVStartSlot = 1;	// Skip slot 0 because RTV is in slot 0
		uUAVMaxSlot = 7;	// 8 - 1
	}
	else
	{
		// Only 1 UAV slot for FL10.X, CS only
		uUAVStartSlot = 0;	// No RTV in CS, so we use slot 0
		uUAVMaxSlot = 0;	// 1 - 1
	}
	rfUAVSlot = this->AddParameter( _T("UAVSlot"), &m_UAVSlot, new CRangeValueSet<UINT>(uUAVStartSlot, uUAVMaxSlot, 1) ); // Skip 0 because the PS uses slot 0 for the RTV

	// Set the test root factor
	testfactor::RFactor rfStageFactor;
	if ( g_TestApp.IsFL11_1OrGreater() )
	{
		rfStageFactor = rfNonComputeShaders + (rfComputeShader * rfUseTGSM);
	}
	else if ( g_TestApp.IsFL11OrGreater() )
	{
		rfStageFactor = rfPixelShader + (rfComputeShader * rfUseTGSM);
	}
	else
	{
		// Down level hardware do not support UAV access.
		rfStageFactor = rfComputeShader;
	}

	this->SetRootTestFactor( ( ( rfSrcAddress *
					             rfDestMask *
					             rfSrcSwizzle *
							     rfStageFactor ) % rfResWidth ) % rfUAVSlot );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResWidth"), MAX_WIDTH), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResWidth"), MAX_WIDTH * 2), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::MaskFlag>(_T("DestMask"), g_pMasks[0]), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

//-----------------------------------------------------------------------------
TEST_RESULT
CStore_raw::ExecuteTestCase()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();

	const UINT uiCbStride = sizeof( VSINPUT );
	const UINT uiOffset = 0;

	// Win8:117229 - wgf11shader5x test is addressing outside the defined TGSM memory. The spec says this causes the TGSM memory to become undefined
	if( (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE) &&
		(m_bUseTGSM == true) )
	{
		// The out-of-bounds doesnt occur when DestMask == x and ResWidth=128, or 64.
		if( !((m_destMask == AssemblyHelper::x) &&
			(m_resWidth == 128 || m_resWidth == 64)))
		{
			WriteToLog( _T( "This test case has been removed because it goes out-of-bounds on TGSM, which the spec says is undefined." ) );
			return RESULT_SKIP;
		}
	}

	switch ( m_ShaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
	case D3D_SHADER_STAGE_PIXEL:

		if ( !this->InitPipelineShaders() )
		{
			WriteError( _T( "CStore_uav_typed::InitPipelineShaders() failed" ) );
			return RESULT_FAIL;
		}

		pContext->OMSetRenderTargetsAndUnorderedAccessViews( 0, NULL, NULL, m_UAVSlot, 1, &m_pUAV, NULL );
		pContext->RSSetState( m_pRSEnabled );

		switch( m_ShaderStage )
		{
		default:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
			break;
		case D3D_SHADER_STAGE_PIXEL:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			break;
		case D3D_SHADER_STAGE_HULL:
		case D3D_SHADER_STAGE_DOMAIN:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST );
			break;
		}

		pContext->IASetVertexBuffers( 0, 1, &m_pVsInput, &uiCbStride, &uiOffset );
		pContext->IASetInputLayout( m_pInputLayout );
		pContext->Draw( NUM_VERTICES, 0 );
		break;

	case D3D_SHADER_STAGE_COMPUTE:

		if ( !this->InitCSShaders() )
		{
			WriteError( _T( "CStore_uav_typed::InitCSShaders() failed" ) );
			return RESULT_FAIL;
		}

		pContext->CSSetShaderResources( 0, 1, &m_pCsInputSRV );
		pContext->CSSetUnorderedAccessViews( m_UAVSlot, 1, &m_pUAV, NULL );
		pContext->CSSetShader( m_pCS, NULL, 0 );
		pContext->Dispatch( DISP_WIDTH, DISP_HEIGHT, 1 );
		break;
	}

	hr = this->ExecuteEffectiveContext();
	if ( FAILED( hr ) )
	{
		WriteToLog( _T("this->ExecuteEffectiveContext() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	return this->AnalyzeResult() ? RESULT_PASS : RESULT_FAIL;
}

//-----------------------------------------------------------------------------
bool
CStore_raw::InitPipelineShaders()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	const tstring strDestMask	= ToString( m_destMask );
	const tstring strSrcSwizzle	= ToString( m_srcSwizzle );
	const tstring strSrcAddressSwizzle = ToString( m_srcAddressSwizzle, 1 );

	const UINT uiCbStride = GetBitsPerElement( m_resFormat ) / 8;
	UINT uiDestMaskSize = uiCbStride;

	switch ( m_destMask )
	{
	case AssemblyHelper::x:
		uiDestMaskSize *= 1;
		break;

	case AssemblyHelper::xy:
		uiDestMaskSize *= 2;
		break;

	case AssemblyHelper::xyz:
		uiDestMaskSize *= 3;
		break;

	case AssemblyHelper::xyzw:
		uiDestMaskSize *= 4;
		break;
	}

	AssemblyHelper asmHelper( 0, 2 );

	asmHelper.SetCustomUAVDecl( m_UAVSlot, FormatString( "dcl_uav_raw u%d", m_UAVSlot ) );

	// Position is in V[0], Address is in V[1], Offset is in V[2]. LD output in r4.
	asmHelper.SetPositionInputRegIdx(0);
	asmHelper.SetUserInputRegMaskTyped( 1, AssemblyHelper::xyzw, "linear", "float" );
	asmHelper.SetUserInputRegMaskTyped( 2, AssemblyHelper::xyzw, "linear", "float" );

	if( m_ShaderStage != D3D_SHADER_STAGE_PIXEL )
	{
		asmHelper.SetPositionOutputRegIdx(5);
		asmHelper.SetupMovToOutputFromInput( 5, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );	// Move position from 0 to 5
	}

	asmHelper.SetOutputRegMaskTyped( 0, AssemblyHelper::xyzw, "float" );

	tstring strShaderCode = "";

	switch( m_ShaderStage )
	{
	default:
		strShaderCode += "ftou r0, v[1]\n";
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
		strShaderCode += "ftou r0, v[0][1]\n";
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		strShaderCode += "ftou r0, vcp[0][1]\n";
		break;
	}

	strShaderCode += FormatString( "imul r1, r0, r0, vec4(%d, %d, %d, %d)\n",
		uiDestMaskSize, uiDestMaskSize, uiDestMaskSize, uiDestMaskSize );
	strShaderCode += FormatString( "iadd r1, r0.%s, vec4(8, 16, 24, 32)\n", strSrcAddressSwizzle.c_str() );
	strShaderCode += FormatString( "store_raw u%d.%s, r0.%s, r1.%s",
		m_UAVSlot, strDestMask.c_str(), strSrcAddressSwizzle.c_str(), strSrcSwizzle.c_str() );

	const bool bNeedPassThroughPS = (m_ShaderStage != D3D_SHADER_STAGE_PIXEL);
	const bool bNeedPassThroughVS = (m_ShaderStage != D3D_SHADER_STAGE_VERTEX);
	ID3D10Blob* pShaderBuf = NULL;
	hr = asmHelper.SetupPipelineWithShaderCode( pDevice,
		                                        pContext,
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												strShaderCode.c_str(),
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS,
												NULL,
												0,
												NULL,
												bNeedPassThroughPS,
												bNeedPassThroughVS);
	if ( FAILED( hr ) )
	{
		WriteError( _T("AssemblyHelper::SetupPipelineWithShaderCode() failed, hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	if ( g_bDumpDissasm )
	{
		this->MessageDumpShaderDisassembly( (UINT*)pShaderBuf->GetBufferPointer(),
			                                pShaderBuf->GetBufferSize() );
	}

	SAFE_RELEASE( pShaderBuf );

	return true;
}

//-----------------------------------------------------------------------------
bool
CStore_raw::InitCSShaders()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	const tstring strDestMask	= ToString( m_destMask );
	const tstring strSrcSwizzle	= ToString( m_srcSwizzle );
	const tstring strSrcAddressSwizzle = ToString( m_srcAddressSwizzle, 1 );

	const UINT uiCbStride = GetBitsPerElement( m_resFormat ) / 8;
	UINT uiDestMaskSize = uiCbStride;

	switch ( m_destMask )
	{
	case AssemblyHelper::x:
		uiDestMaskSize *= 1;
		break;

	case AssemblyHelper::xy:
		uiDestMaskSize *= 2;
		break;

	case AssemblyHelper::xyz:
		uiDestMaskSize *= 3;
		break;

	case AssemblyHelper::xyzw:
		uiDestMaskSize *= 4;
		break;
	}

	AssemblyHelper asmHelper( 0, 3 );
	asmHelper.SetCustomDecl( 0, FormatString( "dcl_resource_structured t%d, %d", 0, sizeof( CSINPUT ) ) );
	asmHelper.SetCustomUAVDecl( 0, FormatString( "dcl_uav_raw u%d", m_UAVSlot ) );
	tstring strShaderCode = "";

	if ( m_bUseTGSM )
	{
		const UINT tgsmWidth = m_resWidth * uiCbStride;
		strShaderCode += FormatString("dcl_tgsm_raw g0, %d\n", tgsmWidth);
		strShaderCode += FormatString( "imad r1, vThreadGroupID.y, vec4(%d, 0, 0, 0), vThreadGroupID.x\n", DISP_WIDTH );
		strShaderCode += "ld_structured r0, r1.x, 0, t0\n";
		strShaderCode += "ftou r0, r0\n";
		strShaderCode += FormatString( "imul r1, r0, r0, vec4(%d, %d, %d, %d)\n",
			uiDestMaskSize, uiDestMaskSize, uiDestMaskSize, uiDestMaskSize );
		strShaderCode += FormatString( "iadd r1, r0.%s, vec4(8, 16, 24, 32)\n", strSrcAddressSwizzle.c_str() );
		strShaderCode += FormatString( "store_raw g0.%s, r0.%s, r1.%s\n",
			strDestMask.c_str(), strSrcAddressSwizzle.c_str(), strSrcSwizzle.c_str() );
		strShaderCode += "sync_uglobal_g\n";
		strShaderCode += FormatString( "ld_raw r2, r0.%s, g0\n", strSrcAddressSwizzle.c_str() );
		strShaderCode += FormatString( "store_raw u%d.%s, r0.%s, r2",
			m_UAVSlot, strDestMask.c_str(), strSrcAddressSwizzle.c_str() );
	}
	else
	{
		strShaderCode += FormatString( "imad r1, vThreadGroupID.y, vec4(%d, 0, 0, 0), vThreadGroupID.x\n", DISP_WIDTH );
		strShaderCode += "ld_structured r0, r1.x, 0, t0\n";
		strShaderCode += "ftou r0, r0\n";
		strShaderCode += FormatString( "imul r1, r0, r0, vec4(%d, %d, %d, %d)\n",
			uiDestMaskSize, uiDestMaskSize, uiDestMaskSize, uiDestMaskSize );
		strShaderCode += FormatString( "iadd r1, r0.%s, vec4(8, 16, 24, 32)\n", strSrcAddressSwizzle.c_str() );
		strShaderCode += FormatString( "store_raw u%d.%s, r0.%s, r1.%s",
			m_UAVSlot, strDestMask.c_str(), strSrcAddressSwizzle.c_str(), strSrcSwizzle.c_str() );
	}

	ID3D10Blob* pShaderBuf = NULL;
	hr = asmHelper.SetupPipelineWithShaderCode( pDevice,
		                                        pContext,
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												strShaderCode.c_str(),
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );
	if ( FAILED( hr ) )
	{
		WriteError( _T("AssemblyHelper::SetupPipelineWithShaderCode() failed, hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	if ( g_bDumpDissasm )
	{
		this->MessageDumpShaderDisassembly( (UINT*)pShaderBuf->GetBufferPointer(),
			                                pShaderBuf->GetBufferSize() );
	}

	SAFE_RELEASE( pShaderBuf );

	return true;
}

//-----------------------------------------------------------------------------
bool
CStore_raw::AnalyzeResult()
{
	HRESULT hr;

	bool bResult = true;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	const bool bLogVerbose = ((CShader5xTestApp*)GetFramework())->LogVerbose();
	const UINT numFailuresToLog = NUM_FAILURES_TO_LOG;
	UINT numFailuresLogged = 0;

	int srcSwizzleIndices[4];
	m_srcSwizzle.ToIndices( srcSwizzleIndices );

	int srcAddressIndices[4];
	m_srcAddressSwizzle.ToIndices( srcAddressIndices );

	UINT destMaskSize = 0;

	switch ( m_destMask )
	{
	case AssemblyHelper::x:
		destMaskSize = 1;
		break;

	case AssemblyHelper::xy:
		destMaskSize = 2;
		break;

	case AssemblyHelper::xyz:
		destMaskSize = 3;
		break;

	case AssemblyHelper::xyzw:
		destMaskSize = 4;
		break;
	}

	UINT width = 1;

	float tx0 = 0.0f - m_outOfBoundsOffset;
	float tx1 = 0.0f + m_outOfBoundsOffset;
	float ty0 = 0.0f - m_outOfBoundsOffset;
	float ty1 = 0.0f + m_outOfBoundsOffset;
	float tz0 = 0.0f - m_outOfBoundsOffset;
	float tz1 = 0.0f + m_outOfBoundsOffset;

	ID3D11Resource* pResource;
	ID3D11Resource* pResourceCopy;

	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
		width = m_resWidth;
		tx1 = (float)width + m_outOfBoundsOffset;
		pResource = m_pBuffer;
		pResourceCopy = m_pBufferCopy;
		break;

	default:
		WriteToLog( _T("CStore_uav_typed::AnalyzeResult() failed, invalid resource type.") );
		return false;
	}

	const UINT uiCbStride  = GetBitsPerElement( m_resFormat ) / 8;
	const UINT uiCbRefBufSize = width * uiCbStride;

	BYTE* const pbRefBuffer = new BYTE[uiCbRefBufSize];
	memset( pbRefBuffer, 0, uiCbRefBufSize );

	const UINT uiXCount = DISP_WIDTH;
	const UINT uiYCount = DISP_HEIGHT;

	const float fdtx = ( tx1 - tx0 ) / uiXCount;
	const float fdty = ( ty1 - ty0 ) / uiYCount;
	const float fdtz = ( tz1 - tz0 ) / uiXCount;

	const float fdtx0 = fdtx / 2;
	const float fdty0 = fdty / 2;
	const float fdtz0 = fdtz / 2;

	float ty = ty0;

	// The VS etc, only use four verticies and point list, so only the corners will light up
	const bool bOnlyCheckCorners = m_bRenderUsingPointlist ? true : false;
	if( m_bRenderUsingPointlist )
	{
		assert( NUM_VERTICES == 4 );
		float vertexTexCoords[NUM_VERTICES][4] = {
		// Address
		{ tx0, ty1, tz0, 0.0f },
		{ tx0, ty0, tz0, 0.0f },
		{ tx1, ty1, tz1, 0.0f },
		{ tx1, ty0, tz1, 0.0f }};

		for( UINT i = 0; i < NUM_VERTICES; i++ )
		{
			UINT data[4];
			float fU = vertexTexCoords[i][ 0 ];
			const UINT u = (fU < 0) ? 0 : (UINT)fU;	// FTOU: Inputs are clamped to the range [0.0f ... 4294967295.999f] prior to conversion
			UINT uiCbAddress = 0xffffffff;

			switch ( m_resourceType )
			{
			case RESOURCE_BUFFER:
				if ( ( u * destMaskSize ) < width )
				{
					uiCbAddress = u * destMaskSize;

					data[0] = uiCbAddress * uiCbStride + 8;
					data[1] = uiCbAddress * uiCbStride + 16;
					data[2] = uiCbAddress * uiCbStride + 24;
					data[3] = uiCbAddress * uiCbStride + 32;
				}
				break;
			}

			if ( 0xffffffff != uiCbAddress )
			{
				UINT _data[4];

				// Apply source swizzle
				_data[0] = data[ srcSwizzleIndices[0] ];
				_data[1] = data[ srcSwizzleIndices[1] ];
				_data[2] = data[ srcSwizzleIndices[2] ];
				_data[3] = data[ srcSwizzleIndices[3] ];

				BYTE* const pbRefColor = pbRefBuffer + uiCbAddress * uiCbStride;
				const UINT bytesLeft = ( width - uiCbAddress ) * uiCbStride;
				memcpy( pbRefColor, _data, min( destMaskSize * uiCbStride, bytesLeft ) );
			}
		}
	}
	else
	{
		for ( UINT y = 0; y < uiYCount; ++y )
		{
			float tx = tx0;
			float tz = tz0;

			for ( UINT x = 0; x < uiXCount; ++x )
			{
				UINT data[4];

				//
				// Build the address location
				//

				const float texCoords[4] = { tx + fdtx0,
											 ty + fdty0,
											 tz + fdtz0,
											 0.0f };

				// texCords[4] contains post-swizzle addresses
				float fU = texCoords[ 0 ];

				const UINT u = (fU < 0) ? 0 : (UINT)fU;	// FTOU: Inputs are clamped to the range [0.0f ... 4294967295.999f] prior to conversion

				UINT uiCbAddress = 0xffffffff;

				switch ( m_resourceType )
				{
				case RESOURCE_BUFFER:
					if ( ( u * destMaskSize ) < width )
					{
						uiCbAddress = u * destMaskSize;

						data[0] = uiCbAddress * uiCbStride + 8;
						data[1] = uiCbAddress * uiCbStride + 16;
						data[2] = uiCbAddress * uiCbStride + 24;
						data[3] = uiCbAddress * uiCbStride + 32;
					}
					break;
				}

				if ( 0xffffffff != uiCbAddress )
				{
					UINT _data[4];

					// Apply source swizzle
					_data[0] = data[ srcSwizzleIndices[0] ];
					_data[1] = data[ srcSwizzleIndices[1] ];
					_data[2] = data[ srcSwizzleIndices[2] ];
					_data[3] = data[ srcSwizzleIndices[3] ];

					BYTE* const pbRefColor = pbRefBuffer + uiCbAddress * uiCbStride;
					const UINT bytesLeft = ( width - uiCbAddress ) * uiCbStride;
					memcpy( pbRefColor, _data, min( destMaskSize * uiCbStride, bytesLeft ) );
				}

				tx += fdtx;
				tz += fdtz;
			}

			ty += fdty;
		}
	}

	//
	// Validate the results
	//

	D3D11_MAPPED_SUBRESOURCE texmap;
	pContext->CopySubresourceRegion( pResourceCopy, 0, 0, 0, 0, pResource, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed") );
		return false;
	}

	hr = GetImmediateContext()->Map( pResourceCopy, 0, D3D11_MAP_READ, 0, &texmap );
	if ( FAILED( hr ) )
	{
		delete [] pbRefBuffer;
		WriteToLog( _T("ID3D11DeviceContext::Map() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	for ( UINT x = 0; x < width; ++x )
	{
		// Load/Convert result color
		BYTE* const pbResultColor = (BYTE*)texmap.pData + x * uiCbStride;

		// Load/Convert expected color
		BYTE* const pbRefColor = pbRefBuffer + x * uiCbStride;

		UINT resultData;
		memcpy( &resultData, pbResultColor, uiCbStride );

		UINT expectedData;
		memcpy( &expectedData, pbRefColor, uiCbStride );

		// Compare the result
		if ( expectedData != resultData )
		{
			bResult = false;

			if ( bLogVerbose || (numFailuresLogged < numFailuresToLog) )
			{
				WriteToLog( _T( "CStore_raw::AnalyzeResult() failed at pixel (%d), is different. \tExpected value is %d \tOutput value is %d" ), x, expectedData, resultData );
				numFailuresLogged++;
			}
			else if( !bLogVerbose && (numFailuresLogged == numFailuresToLog) )
			{
				// We will only hit this message once
				WriteToLog( _T( "CStore_raw::AnalyzeResult() has additional failures that are not logged to reduce log size. Please run the group again with -LogVerbose to log the additional failures." ));
				goto Cleanup;
			}
		}
	}

Cleanup:
	delete [] pbRefBuffer;
	GetImmediateContext()->Unmap( pResourceCopy, 0 );

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
void
CStore_structured::InitTestParameters()
{
	// Define static parameters
	m_resFormat = DXGI_FORMAT_UNKNOWN;
	m_resourceType = RESOURCE_BUFFER;

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Define supported shader stages
	CUserValueSet<D3D_SHADER_STAGE>* const pShaderStages = new CUserValueSet<D3D_SHADER_STAGE>;
	pShaderStages->AddValue( D3D_SHADER_STAGE_VERTEX );
	pShaderStages->AddValue( D3D_SHADER_STAGE_GEOMETRY );
	pShaderStages->AddValue( D3D_SHADER_STAGE_PIXEL );
	pShaderStages->AddValue( D3D_SHADER_STAGE_HULL );
	pShaderStages->AddValue( D3D_SHADER_STAGE_DOMAIN );
	testfactor::RFactor rfNonComputeShaders = this->AddParameterValueSet<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), pShaderStages );
	testfactor::RFactor rfPixelShader = this->AddParameterValue<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );
	testfactor::RFactor rfComputeShader = this->AddParameterValue<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_COMPUTE );

	// Define TGSM usage
	CTestCaseParameter<bool>* const pUseTGSM = AddParameter( _T( "UseTGSM" ), &m_bUseTGSM );
	testfactor::RFactor rfUseTGSM = AddParameterValueSet( pUseTGSM, BoolValueSet() );
	SetParameterDefaultValue( pUseTGSM, false );

	// Define supported resource width values
	CUserValueSet<UINT>* const pWidthValues = new CUserValueSet<UINT>;
	pWidthValues->AddValue( MAX_WIDTH * 1 );
    pWidthValues->AddValue( MAX_WIDTH * 2 );
    pWidthValues->AddValue( MAX_WIDTH * 4 );
	testfactor::RFactor rfResWidth = this->AddParameter( _T("ResWidth"), &m_resWidth, pWidthValues );

	// Define supported resource width values
	CTestCaseParameter<UINT> *pBufStrideParam = AddParameter( _T( "BufStride" ), &m_bufStride );
	testfactor::RFactor rfBufStride32 = AddParameterValue<UINT>( pBufStrideParam, 32 );
	testfactor::RFactor rfBufStride16 = AddParameterValue<UINT>( pBufStrideParam, 16 );
	testfactor::RFactor rfBufStride8 = AddParameterValue<UINT>( pBufStrideParam, 8 );
	testfactor::RFactor rfBufStride4 = AddParameterValue<UINT>( pBufStrideParam, 4 );

	// Define supported destination address values
	CUserValueSet<AssemblyHelper::Swizzle>* const pAddressValues = new CUserValueSet<AssemblyHelper::Swizzle>();
	pAddressValues->AddValue( g_pSwizzles[4] );	// x
	pAddressValues->AddValue( g_pSwizzles[5] );	// y
	pAddressValues->AddValue( g_pSwizzles[6] );	// z
	pAddressValues->AddValue( g_pSwizzles[7] );	// w
	testfactor::RFactor rfSrcAddressSwizzle = this->AddParameter( _T("SrcAddressSwizzle"), &m_srcAddressSwizzle, pAddressValues );

	// Define supported destination offset values
	CTestCaseParameter<ADDRESS_OFFSET> *pAddressOffsetParam = AddParameter( _T( "SrcOffset" ), &m_srcOffset );
	testfactor::RFactor rfAddressOffsetStart			= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_START );
	testfactor::RFactor rfAddressOffsetStartPlus4Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_START_PLUS_4_BYTES );
	testfactor::RFactor rfAddressOffsetStartPlus20Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_START_PLUS_20_BYTES );
	testfactor::RFactor rfAddressOffsetEndMinus4Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_END_MINUS_4_BYTES );
	testfactor::RFactor rfAddressOffsetEndMinus8Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_END_MINUS_8_BYTES );
	testfactor::RFactor rfAddressOffsetEndMinus12Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_END_MINUS_12_BYTES );
	testfactor::RFactor rfAddressOffsetEndMinus16Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_END_MINUS_16_BYTES );
	testfactor::RFactor rfAddressOffsetEndMinus20Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_END_MINUS_20_BYTES );

	CUserValueSet<AssemblyHelper::Swizzle>* const pOffsetValuesSwizzle = new CUserValueSet<AssemblyHelper::Swizzle>();
	pOffsetValuesSwizzle->AddValue( g_pSwizzles[4] );	// x
	pOffsetValuesSwizzle->AddValue( g_pSwizzles[5] );	// y
	pOffsetValuesSwizzle->AddValue( g_pSwizzles[6] );	// z
	pOffsetValuesSwizzle->AddValue( g_pSwizzles[7] );	// w
	testfactor::RFactor rfSrcOffsetSwizzle = this->AddParameter( _T("SrcOffsetSwizzle"), &m_srcOffsetSwizzle, pOffsetValuesSwizzle );

	// Define supported source swizzle values
	CUserValueSet<AssemblyHelper::Swizzle>* const pSwizzleValues = new CUserValueSet<AssemblyHelper::Swizzle>();
	for ( UINT i = 0; i < _countof( g_pSwizzles ); ++i )
	{
		pSwizzleValues->AddValue( g_pSwizzles[i] );
	}
	testfactor::RFactor rfSrcSwizzle = this->AddParameter( _T("SrcSwizzle"), &m_srcSwizzle, pSwizzleValues );

	// Define supported destination masks
	CTestCaseParameter<AssemblyHelper::MaskFlag> *pDestMaskParam = AddParameter( _T( "DestMask" ), &m_destMask );
	testfactor::RFactor rfDestMaskXYZW = AddParameterValue<AssemblyHelper::MaskFlag>( pDestMaskParam, g_pMasks[0] );
	testfactor::RFactor rfDestMaskXYZ = AddParameterValue<AssemblyHelper::MaskFlag>( pDestMaskParam, g_pMasks[1] );
	testfactor::RFactor rfDestMaskXY = AddParameterValue<AssemblyHelper::MaskFlag>( pDestMaskParam, g_pMasks[3] );
	testfactor::RFactor rfDestMaskX = AddParameterValue<AssemblyHelper::MaskFlag>( pDestMaskParam, g_pMasks[7] );

	// Define supported UAV slots
	CUserValueSet<UINT>* const pUAVSlots = new CUserValueSet<UINT>;
	UINT uUAVStartSlot = 0;
	UINT uUAVMaxSlot = 0;
	testfactor::RFactor rfUAVSlot;
	if( g_TestApp.IsFL11_1OrGreater() )
	{
		uUAVStartSlot = 1;	// Skip slot 0 because RTV is in slot 0
		uUAVMaxSlot = 63;	// 64 - 1
	}
	else if( g_TestApp.IsFL11OrGreater() )
	{
		uUAVStartSlot = 1;	// Skip slot 0 because RTV is in slot 0
		uUAVMaxSlot = 7;	// 8 - 1
	}
	else
	{
		// Only 1 UAV slot for FL10.X, CS only
		uUAVStartSlot = 0;	// No RTV in CS, so we use slot 0
		uUAVMaxSlot = 0;	// 1 - 1
	}
	rfUAVSlot = this->AddParameter( _T("UAVSlot"), &m_UAVSlot, new CRangeValueSet<UINT>(uUAVStartSlot, uUAVMaxSlot, 1) ); // Skip 0 because the PS uses slot 0 for the RTV


	// Win7:705720
	// Determine valid testcases for strides. Note that if the offset causes a out of bounds error, the UAV is undefined. Thus we don't test it.

	testfactor::RFactor rfDestMaskWrite1Component	= rfDestMaskX;
	testfactor::RFactor rfDestMaskWrite2Components	= rfDestMaskX + rfDestMaskXY;
	testfactor::RFactor rfDestMaskWrite3Components	= rfDestMaskX + rfDestMaskXY + rfDestMaskXYZ;
	testfactor::RFactor rfDestMaskWrite4Components	= rfDestMaskX + rfDestMaskXY + rfDestMaskXYZ + rfDestMaskXYZW;

	testfactor::RFactor rfStride4Test = rfBufStride4 *					// Stride of 4
										( rfAddressOffsetStart * rfDestMaskWrite1Component );					// No offset, only 1 component

	testfactor::RFactor rfStride8Test = rfBufStride8 * (				// Stride of 8
										( rfAddressOffsetStart * rfDestMaskWrite2Components ) +					// No offset, up to 2 components
										( rfAddressOffsetStartPlus4Bytes * rfDestMaskWrite1Component ) );		// 4Byte offset, only 1 component

	testfactor::RFactor rfStride16Test = rfBufStride16 * (				// Stride of 16
										( rfAddressOffsetStart * rfDestMaskWrite4Components ) +					// No offset, up to 4 components
										( rfAddressOffsetStartPlus4Bytes * rfDestMaskWrite3Components ) +		// 4Byte offset, up to 3 components

										( rfAddressOffsetEndMinus4Bytes * rfDestMaskWrite1Component ) +			// 4Byte offset from end, only 1 component
										( rfAddressOffsetEndMinus8Bytes * rfDestMaskWrite2Components ) );		// 8Byte offset from end, up to 2 components

	testfactor::RFactor rfStride32Test = rfBufStride32 * (				// Stride of 32
										( rfAddressOffsetStart * rfDestMaskWrite4Components ) +					// No offset, up to 4 components
										( rfAddressOffsetStartPlus4Bytes * rfDestMaskWrite3Components ) +		// 4Byte offset, up to 4 components
										( rfAddressOffsetStartPlus20Bytes * rfDestMaskWrite3Components ) +		// 20Byte offset, up to 3 components

										( rfAddressOffsetEndMinus4Bytes * rfDestMaskWrite1Component ) +			// 4Byte offset from end, only 1 component
										( rfAddressOffsetEndMinus8Bytes * rfDestMaskWrite2Components ) +		// 8Byte offset from end, up to 2 components
										( rfAddressOffsetEndMinus12Bytes * rfDestMaskWrite3Components ) +		// 12Byte offset from end, up to 3 components
										( rfAddressOffsetEndMinus16Bytes * rfDestMaskWrite4Components ) +		// 16Byte offset from end, up to 4 components
										( rfAddressOffsetEndMinus20Bytes * rfDestMaskWrite4Components ) );		// 20Byte offset from end, up to 4 components

	testfactor::RFactor rfResWidthStridesTests = ( rfResWidth * ( rfStride4Test + rfStride8Test + rfStride16Test + rfStride32Test ) );
	testfactor::RFactor rfSwizzles = rfSrcSwizzle * rfSrcAddressSwizzle * rfSrcOffsetSwizzle;

	// Set the test root factor
	testfactor::RFactor rfStageFactor;
	if ( g_TestApp.IsFL11_1OrGreater() )
	{
		rfStageFactor = rfNonComputeShaders + (rfComputeShader * rfUseTGSM);
	}
	else if ( g_TestApp.IsFL11OrGreater() )
	{
		rfStageFactor = rfPixelShader + (rfComputeShader * rfUseTGSM);
	}
	else
	{
		rfStageFactor = rfComputeShader;
	}

	this->SetRootTestFactor( ( rfSwizzles * rfStageFactor ) % rfResWidthStridesTests % rfUAVSlot );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResWidth"), MAX_WIDTH), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResWidth"), MAX_WIDTH * 2), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::MaskFlag>(_T("DestMask"), g_pMasks[0]), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

//-----------------------------------------------------------------------------
TEST_RESULT
CStore_structured::ExecuteTestCase()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();

	const UINT uiCbStride = sizeof( VSINPUT );
	const UINT uiOffset = 0;

	switch ( m_ShaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
	case D3D_SHADER_STAGE_PIXEL:

		if ( !this->InitPipelineShaders() )
		{
			WriteError( _T( "CStore_uav_typed::InitPipelineShaders() failed" ) );
			return RESULT_FAIL;
		}

		pContext->OMSetRenderTargetsAndUnorderedAccessViews( 0, NULL, NULL, m_UAVSlot, 1, &m_pUAV, NULL );
		pContext->RSSetState( m_pRSEnabled );

		switch( m_ShaderStage )
		{
		default:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
			break;
		case D3D_SHADER_STAGE_PIXEL:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			break;
		case D3D_SHADER_STAGE_HULL:
		case D3D_SHADER_STAGE_DOMAIN:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST );
			break;
		}

		pContext->IASetVertexBuffers( 0, 1, &m_pVsInput, &uiCbStride, &uiOffset );
		pContext->IASetInputLayout( m_pInputLayout );
		pContext->Draw( NUM_VERTICES, 0 );
		break;

	case D3D_SHADER_STAGE_COMPUTE:

		if ( !this->InitCSShaders() )
		{
			WriteError( _T( "CStore_uav_typed::InitCSShaders() failed" ) );
			return RESULT_FAIL;
		}

		pContext->CSSetShaderResources( 0, 1, &m_pCsInputSRV );
		pContext->CSSetUnorderedAccessViews( m_UAVSlot, 1, &m_pUAV, NULL );
		pContext->CSSetShader( m_pCS, NULL, 0 );
		pContext->Dispatch( DISP_WIDTH, DISP_HEIGHT, 1 );
		break;
	}

	hr = this->ExecuteEffectiveContext();
	if ( FAILED( hr ) )
	{
		WriteToLog( _T("this->ExecuteEffectiveContext() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	return this->AnalyzeResult() ? RESULT_PASS : RESULT_FAIL;
}

//-----------------------------------------------------------------------------
UINT
CStore_structured::GetSrcStructureOffset()
{
	// Description: This function simply returns the number of bytes the offset should be in the struct.
	//				It asserts that the offset is within the stride to ensure the UAV doesn't become undefined, as the spec describes.

	// Local variables
	int offset = 0;

	switch( m_srcOffset )
	{
	case ADDRESS_OFFSET_START:
		offset = 0;
		break;
	case ADDRESS_OFFSET_START_PLUS_4_BYTES:
		offset = 4;
		break;
	case ADDRESS_OFFSET_START_PLUS_8_BYTES:
		offset = 8;
		break;
	case ADDRESS_OFFSET_START_PLUS_12_BYTES:
		offset = 12;
		break;
	case ADDRESS_OFFSET_START_PLUS_16_BYTES:
		offset = 16;
		break;
	case ADDRESS_OFFSET_START_PLUS_20_BYTES:
		offset = 20;
		break;

	case ADDRESS_OFFSET_END_MINUS_4_BYTES:
		offset = m_bufStride - 4;
		break;
	case ADDRESS_OFFSET_END_MINUS_8_BYTES:
		offset = m_bufStride - 8;
		break;
	case ADDRESS_OFFSET_END_MINUS_12_BYTES:
		offset = m_bufStride - 12;
		break;
	case ADDRESS_OFFSET_END_MINUS_16_BYTES:
		offset = m_bufStride - 16;
		break;
	case ADDRESS_OFFSET_END_MINUS_20_BYTES:
		offset = m_bufStride - 20;
		break;
	}

	// Validate the offset is correct. If we go outside this range the UAV becomes undefined and we shouldn't test it.
	// Note: This assert doesn't take into account the numer of components to write.
	assert( offset >= 0 || offset < (int)m_bufStride );
	
	return (UINT)offset;
}

//-----------------------------------------------------------------------------
bool
CStore_structured::InitPipelineShaders()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	const tstring strDestMask	= ToString( m_destMask );
	const tstring strSrcSwizzle	= ToString( m_srcSwizzle );
	const tstring strSrcAddressSwizzle = ToString( m_srcAddressSwizzle, 1 );
	const tstring strSrcOffsetSwizzle = ToString( m_srcOffsetSwizzle, 1 );

	// Build the structure offset based on the stride so we keep the UAV valid
	const UINT srcOffset = GetSrcStructureOffset();

	// Create the string to init the offset register (based on the swizzle used in the test)
	tstring strSrcOffset = "vec4(%d, %d, %d, %d)";

	if( "x" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), srcOffset, 0, 0, 0 );
	}
	else if( "y" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, srcOffset, 0, 0 );
	}
	else if( "z" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, 0, srcOffset, 0 );
	}
	else if( "w" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, 0, 0, srcOffset );
	}
	else
	{
		// Should never get here
		assert(false);
	}

	AssemblyHelper asmHelper( 0, 4 );

	asmHelper.SetCustomUAVDecl( m_UAVSlot, FormatString( "dcl_uav_structured u%d, %d", m_UAVSlot, m_bufStride ) );

	// Position is in V[0], Address is in V[1], Offset is in V[2].
	asmHelper.SetPositionInputRegIdx(0);
	asmHelper.SetUserInputRegMaskTyped( 1, AssemblyHelper::xyzw, "linear", "float" );
	asmHelper.SetUserInputRegMaskTyped( 2, AssemblyHelper::xyzw, "linear", "float" );
	asmHelper.SetupMovToTempFromLiteral(3, AssemblyHelper::xyzw, strSrcOffset);

	if( m_ShaderStage != D3D_SHADER_STAGE_PIXEL )
	{
		asmHelper.SetPositionOutputRegIdx(5);
		asmHelper.SetupMovToOutputFromInput( 5, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );	// Move position from 0 to 5
	}

	asmHelper.SetOutputRegMaskTyped( 0, AssemblyHelper::xyzw, "float" );

	tstring strShaderCode = "";

	switch( m_ShaderStage )
	{
	default:
		strShaderCode += "ftou r1, v[1]\n";
		strShaderCode += "ftou r2, v[2]\n";
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
		strShaderCode += "ftou r1, v[0][1]\n";
		strShaderCode += "ftou r2, v[0][2]\n";
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		strShaderCode += "ftou r1, vcp[0][1]\n";
		strShaderCode += "ftou r2, vcp[0][2]\n";
		break;
	}

	strShaderCode += "imul r0, r2, r2, vec4(4, 4, 4, 4)\n";
	strShaderCode += FormatString( "iadd r0, r1.%s, vec4(8, 16, 24, 32)\n", strSrcAddressSwizzle.c_str() );
	strShaderCode += FormatString( "store_structured u%d.%s, r1.%s, r3.%s, r0.%s",
		m_UAVSlot, strDestMask.c_str(), strSrcAddressSwizzle.c_str(), strSrcOffsetSwizzle.c_str(), strSrcSwizzle.c_str() );

	ID3D10Blob* pShaderBuf = NULL;
	hr = asmHelper.SetupPipelineWithShaderCode( pDevice,
		                                        pContext,
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												strShaderCode.c_str(),
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );
	if ( FAILED( hr ) )
	{
		WriteError( _T("AssemblyHelper::SetupPipelineWithShaderCode() failed, hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	if ( g_bDumpDissasm )
	{
		this->MessageDumpShaderDisassembly( (UINT*)pShaderBuf->GetBufferPointer(),
			                                pShaderBuf->GetBufferSize() );
	}

	SAFE_RELEASE( pShaderBuf );

	return true;
}

//-----------------------------------------------------------------------------
bool
CStore_structured::InitCSShaders()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	const tstring strDestMask	= ToString( m_destMask );
	const tstring strSrcSwizzle	= ToString( m_srcSwizzle );
	const tstring strSrcAddressSwizzle = ToString( m_srcAddressSwizzle, 1 );
	const tstring strSrcOffsetSwizzle  = ToString( m_srcOffsetSwizzle, 1 );

	// Build the structure offset based on the stride so we keep the UAV valid
	const UINT srcOffset = GetSrcStructureOffset();

	// Create the string to init the offset register (based on the swizzle used in the test)
	tstring strSrcOffset = "vec4(%d, %d, %d, %d)";

	if( "x" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), srcOffset, 0, 0, 0 );
	}
	else if( "y" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, srcOffset, 0, 0 );
	}
	else if( "z" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, 0, srcOffset, 0 );
	}
	else if( "w" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, 0, 0, srcOffset );
	}
	else
	{
		// Should never get here
		assert(false);
	}

	AssemblyHelper asmHelper( 0, 5 );
	asmHelper.SetCustomDecl( 0, FormatString( "dcl_resource_structured t%d, %d", 0, sizeof( CSINPUT ) ) );
	asmHelper.SetCustomUAVDecl( 0, FormatString( "dcl_uav_structured u%d, %d", m_UAVSlot, m_bufStride ) );

	if( m_bUseTGSM )
		asmHelper.SetCustomDecl( 1, FormatString( "dcl_tgsm_structured g0, %d, %d\n", m_bufStride, m_resWidth ) );

	asmHelper.SetupMovToTempFromLiteral(4, AssemblyHelper::xyzw, strSrcOffset);
	tstring strShaderCode = "";

	if ( m_bUseTGSM )
	{
		//strShaderCode += FormatString( "dcl_tgsm_structured g0, %d, %d\n", m_bufStride, m_resWidth );
		strShaderCode += FormatString( "imad r0, vThreadGroupID.y, vec4(%d, 0, 0, 0), vThreadGroupID.x\n", DISP_WIDTH );
		strShaderCode += "ld_structured r1, r0.x, 0, t0\n";
		strShaderCode += "ld_structured r2, r0.x, 16, t0\n";
		strShaderCode += "ftou r1, r1\n";
		strShaderCode += "ftou r2, r2\n";
		strShaderCode += "imul r0, r2, r2, vec4(4, 4, 4, 4)\n";
		strShaderCode += FormatString( "iadd r0, r1.%s, vec4(8, 16, 24, 32)\n", strSrcAddressSwizzle.c_str() );
		strShaderCode += FormatString( "store_structured g0.%s, r1.%s, r4.%s, r0.%s\n",
			strDestMask.c_str(), strSrcAddressSwizzle.c_str(), strSrcOffsetSwizzle.c_str(), strSrcSwizzle.c_str() );
		strShaderCode += "sync_uglobal_g\n";
		strShaderCode += FormatString( "ld_structured r3, r1.%s, r4.%s, g0\n", strSrcAddressSwizzle.c_str(), strSrcOffsetSwizzle.c_str() );
		strShaderCode += FormatString( "store_structured u%d.%s, r1.%s, r4.%s, r3",
			m_UAVSlot, strDestMask.c_str(), strSrcAddressSwizzle.c_str(), strSrcOffsetSwizzle.c_str() );
	}
	else
	{
		strShaderCode += FormatString( "imad r0, vThreadGroupID.y, vec4(%d, 0, 0, 0), vThreadGroupID.x\n", DISP_WIDTH );
		strShaderCode += "ld_structured r1, r0.x, 0, t0\n";
		strShaderCode += "ld_structured r2, r0.x, 16, t0\n";
		strShaderCode += "ftou r1, r1\n";
		strShaderCode += "ftou r2, r2\n";
		strShaderCode += "imul r0, r2, r2, vec4(4, 4, 4, 4)\n";
		strShaderCode += FormatString( "iadd r0, r1.%s, vec4(8, 16, 24, 32)\n", strSrcAddressSwizzle.c_str() );
		strShaderCode += FormatString( "store_structured u%d.%s, r1.%s, r4.%s, r0.%s",
			m_UAVSlot, strDestMask.c_str(), strSrcAddressSwizzle.c_str(), strSrcOffsetSwizzle.c_str(), strSrcSwizzle.c_str() );
	}

	ID3D10Blob* pShaderBuf = NULL;
	hr = asmHelper.SetupPipelineWithShaderCode( pDevice,
		                                        pContext,
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												strShaderCode.c_str(),
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );
	if ( FAILED( hr ) )
	{
		WriteError( _T("AssemblyHelper::SetupPipelineWithShaderCode() failed, hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	if ( g_bDumpDissasm )
	{
		this->MessageDumpShaderDisassembly( (UINT*)pShaderBuf->GetBufferPointer(),
			                                pShaderBuf->GetBufferSize() );
	}

	SAFE_RELEASE( pShaderBuf );

	return true;
}

//-----------------------------------------------------------------------------
bool
CStore_structured::AnalyzeResult()
{
	HRESULT hr;

	bool bResult = true;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	const bool bLogVerbose = ((CShader5xTestApp*)GetFramework())->LogVerbose();
	const UINT numFailuresToLog = NUM_FAILURES_TO_LOG;
	UINT numFailuresLogged = 0;

	int srcSwizzleIndices[4];
	m_srcSwizzle.ToIndices( srcSwizzleIndices );

	int srcAddressIndices[4];
	m_srcAddressSwizzle.ToIndices( srcAddressIndices );

	int srcOffsetIndices[4];
	m_srcOffsetSwizzle.ToIndices( srcOffsetIndices );

	UINT width	= 1;
	UINT height	= 1;
	UINT depth	= 1;

	float tx0 = 0.0f - m_outOfBoundsOffset;
	float tx1 = 0.0f + m_outOfBoundsOffset;
	float ty0 = 0.0f - m_outOfBoundsOffset;
	float ty1 = 0.0f + m_outOfBoundsOffset;
	float tz0 = 0.0f - m_outOfBoundsOffset;
	float tz1 = 0.0f + m_outOfBoundsOffset;

	ID3D11Resource* pResource;
	ID3D11Resource* pResourceCopy;

	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
		width = m_resWidth;
		tx1 = (float)width + m_outOfBoundsOffset;
		pResource = m_pBuffer;
		pResourceCopy = m_pBufferCopy;
		break;

	default:
		WriteToLog( _T("CStore_uav_typed::AnalyzeResult() failed, invalid resource type.") );
		return false;
	}

	const UINT uiCbRefBufSize = width * m_bufStride;

	BYTE* const pbRefBuffer = new BYTE[uiCbRefBufSize];
	memset( pbRefBuffer, 0, uiCbRefBufSize );

	const UINT uiXCount = DISP_WIDTH;
	const UINT uiYCount = DISP_HEIGHT;

	const float fdtx = ( tx1 - tx0 ) / uiXCount;
	const float fdty = ( ty1 - ty0 ) / uiYCount;
	const float fdtz = ( tz1 - tz0 ) / uiXCount;

	const float fdtx0 = fdtx / 2;
	const float fdty0 = fdty / 2;
	const float fdtz0 = fdtz / 2;

	const float fdOffsetR = ( 3.0f - 0.0f ) / uiXCount;
	const float fdOffsetG = ( 6.0f - 0.0f ) / uiXCount;
	const float fdOffsetB = ( 9.0f - 0.0f ) / uiXCount;
	const float fdOffsetA = ( 12.0f - 0.0f ) / uiXCount;

	const float fdOffsetR0 = fdOffsetR / 2;
	const float fdOffsetG0 = fdOffsetG / 2;
	const float fdOffsetB0 = fdOffsetB / 2;
	const float fdOffsetA0 = fdOffsetA / 2;

	const UINT uiCbStride = sizeof( UINT );

	UINT uiCbCopySize = uiCbStride;

	switch ( m_destMask )
	{
	case AssemblyHelper::x:
		uiCbCopySize *= 1;
		break;

	case AssemblyHelper::xy:
		uiCbCopySize *= 2;
		break;

	case AssemblyHelper::xyz:
		uiCbCopySize *= 3;
		break;

	case AssemblyHelper::xyzw:
		uiCbCopySize *= 4;
		break;
	}

	float ty = ty0;

	// The VS etc, only use four verticies and point list, so only the corners will light up
	const bool bOnlyCheckCorners = m_bRenderUsingPointlist ? true : false;
	if( m_bRenderUsingPointlist )
	{
		assert( NUM_VERTICES == 4 );
		float vertexTexCoords[NUM_VERTICES][4] = {
		// Address
		{ tx0, ty1, tz0, 0.0f },
		{ tx0, ty0, tz0, 0.0f },
		{ tx1, ty1, tz1, 0.0f },
		{ tx1, ty0, tz1, 0.0f }};

		for( UINT v = 0; v < NUM_VERTICES; v++ )
		{
			UINT data[4];

			//
			// Build the offset position
			//

			UINT offsets[4] = { 0, 0, 0, 0 };
			tstring components[] = { "x", "y", "z", "w" };
			const tstring strSrcOffsetSwizzle  = ToString( m_srcOffsetSwizzle, 1 );
			const UINT srcOffset = GetSrcStructureOffset();

			for( UINT i = 0; i < 4; ++i )
			{
				if( components[i] == strSrcOffsetSwizzle )
					offsets[i] = srcOffset;
			}

			const UINT offset = offsets[ srcOffsetIndices[0] ];

			//
			// Build the address location
			//

			float fU = vertexTexCoords[v][ 0 ];
			const UINT u = (fU < 0) ? 0 : (UINT)fU;	// FTOU: Inputs are clamped to the range [0.0f ... 4294967295.999f] prior to conversion

			UINT uiCbAddress = 0xffffffff;

			switch ( m_resourceType )
			{
			case RESOURCE_BUFFER:
				if ( u < width &&
						offset < m_bufStride )
				{
					uiCbAddress = u * m_bufStride + offset;

					data[0] = u + 8;
					data[1] = u + 16;
					data[2] = u + 24;
					data[3] = u + 32;
				}
				break;
			}

			if ( 0xffffffff != uiCbAddress )
			{
				UINT _data[4];

				// Apply source swizzle
				_data[0] = data[ srcSwizzleIndices[0] ];
				_data[1] = data[ srcSwizzleIndices[1] ];
				_data[2] = data[ srcSwizzleIndices[2] ];
				_data[3] = data[ srcSwizzleIndices[3] ];

				BYTE* const pbRefColor = pbRefBuffer + uiCbAddress;
				const UINT bytesLeft = m_bufStride - offset;
				memcpy( pbRefColor, _data, min( uiCbCopySize, bytesLeft ) );
			}
		}
	}
	else
	{
		for ( UINT y = 0; y < uiYCount; ++y )
		{
			float tx = tx0;
			float tz = tz0;

			float fOffsetR = 0.0f;
			float fOffsetG = 0.0f;
			float fOffsetB = 0.0f;
			float fOffsetA = 0.0f;

			for ( UINT x = 0; x < uiXCount; ++x )
			{
				UINT data[4];

				//
				// Build the offset position
				//

				UINT offsets[4] = { 0, 0, 0, 0 };
				tstring components[] = { "x", "y", "z", "w" };
				const tstring strSrcOffsetSwizzle  = ToString( m_srcOffsetSwizzle, 1 );
				const UINT srcOffset = GetSrcStructureOffset();

				for( UINT i = 0; i < 4; ++i )
				{
					if( components[i] == strSrcOffsetSwizzle )
						offsets[i] = srcOffset;
				}

				const UINT offset = offsets[ srcOffsetIndices[0] ];

				//
				// Build the address location
				//

				const float address[4] = { tx + fdtx0,
										   ty + fdty0,
										   tz + fdtz0,
										   0.0f };

				// texCords[4] contains post-swizzle addresses
				const UINT u = (UINT)address[ 0 ];


				UINT uiCbAddress = 0xffffffff;

				switch ( m_resourceType )
				{
				case RESOURCE_BUFFER:
					if ( u < width &&
						 offset < m_bufStride )
					{
						uiCbAddress = u * m_bufStride + offset;

						data[0] = u + 8;
						data[1] = u + 16;
						data[2] = u + 24;
						data[3] = u + 32;
					}
					break;
				}

				if ( 0xffffffff != uiCbAddress )
				{
					UINT _data[4];

					// Apply source swizzle
					_data[0] = data[ srcSwizzleIndices[0] ];
					_data[1] = data[ srcSwizzleIndices[1] ];
					_data[2] = data[ srcSwizzleIndices[2] ];
					_data[3] = data[ srcSwizzleIndices[3] ];

					BYTE* const pbRefColor = pbRefBuffer + uiCbAddress;
					const UINT bytesLeft = m_bufStride - offset;
					memcpy( pbRefColor, _data, min( uiCbCopySize, bytesLeft ) );
				}

				tx += fdtx;
				tz += fdtz;

				fOffsetR += fdOffsetR;
				fOffsetG += fdOffsetG;
				fOffsetB += fdOffsetB;
				fOffsetA += fdOffsetA;
			}

			ty += fdty;
		}
	}

	//
	// Validate the results
	//

	D3D11_MAPPED_SUBRESOURCE texmap;
	pContext->CopySubresourceRegion( pResourceCopy, 0, 0, 0, 0, pResource, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed") );
		return false;
	}

	hr = GetImmediateContext()->Map( pResourceCopy, 0, D3D11_MAP_READ, 0, &texmap );
	if ( FAILED( hr ) )
	{
		delete [] pbRefBuffer;
		WriteToLog( _T("ID3D11DeviceContext::Map() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	for ( UINT x = 0, n = ( width * m_bufStride ) / uiCbStride; x < n; ++x )
	{
		// Load/Convert result color
		BYTE* const pbResultColor = (BYTE*)texmap.pData + x * uiCbStride;

		// Load/Convert expected color
		BYTE* const pbRefColor = pbRefBuffer + x * uiCbStride;

		UINT resultData;
		memcpy( &resultData, pbResultColor, uiCbStride );

		UINT expectedData;
		memcpy( &expectedData, pbRefColor, uiCbStride );

		// Compare the result
		if ( expectedData != resultData )
		{
			bResult = false;

			if ( bLogVerbose || (numFailuresLogged < numFailuresToLog) )
			{
				WriteToLog( _T( "CStore_structured::AnalyzeResult() failed at pixel (%d), is different. \tExpected value is %d \tOutput value is %d" ), x, expectedData, resultData );
				numFailuresLogged++;
			}
			else if( !bLogVerbose && (numFailuresLogged == numFailuresToLog) )
			{
				// We will only hit this message once
				WriteToLog( _T( "CStore_structured::AnalyzeResult() has additional failures that are not logged to reduce log size. Please run the group again with -LogVerbose to log the additional failures." ));
				goto Cleanup;
			}
		}
	}

	Cleanup:
	delete [] pbRefBuffer;
	GetImmediateContext()->Unmap( pResourceCopy, 0 );

	return bResult;
}

} // namespace LoadStoreTest
