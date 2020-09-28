#include "ResourceAccess.h"

#include <WGFTestCore.h>

#include "Compressed.h"
#include "Multisample.h"
#include "ResourceFlush.h"
#include "Discard.h"
#include "ClearView.h"
#include "D2DTileableCopy.h"
#include "Planar.h"

// The test fills in mapped region with this value
static const BYTE RESOURCE_TAG_ODD = 0x1D;
static const BYTE RESOURCE_TAG_EVEN = 0x83; // used for last Map before DO_NOT_WAIT Map

CResourceAccessTestApp g_TestApp;

D3D_TEST_BOX CResourceAccessTest::m_pBoxes [ ] =
{
	D3D_TEST_BOX( 0,		0,				0,		0,				0,		0 ),
	D3D_TEST_BOX( 0,		1,				0,		1,				0,		1 ),
	D3D_TEST_BOX( 4,		8,				4,		8,				0,		1 )
};

D3D_TEST_BOX CResourceAccessTest::m_pBoxesWithDepth [ ] =
{
	D3D_TEST_BOX( 0,		0,				0,		0,				0,		0 ),
	D3D_TEST_BOX( 0,		1,				0,		1,				0,		1 ),
	D3D_TEST_BOX( 4,		8,				4,		8,				0,		1 ),
	D3D_TEST_BOX( 0,		4,				0,		4,				0,		2 ),
	D3D_TEST_BOX( 4,		8,				4,		8,				1,		3 )
};

D3D_TEST_BOX g_pConstantBufferBoxes[] = 
{
	D3D_TEST_BOX(         0,             0,  0, 0, 0, 0 ),
	D3D_TEST_BOX(         0,            16,  0, 1, 0, 1 ),
	D3D_TEST_BOX(         0,            16,  1, 1, 0, 1 ),
	D3D_TEST_BOX(         0,            16,  0, 1, 1, 1 ),
	D3D_TEST_BOX(         0,            16,  0, 0, 0, 1 ),
	D3D_TEST_BOX(         0,            16,  0, 1, 0, 0 ),
	D3D_TEST_BOX(         0,     4096 * 16,  0, 1, 0, 1 ),
	D3D_TEST_BOX(         0, 4096 * 16 * 2,  0, 1, 0, 1 ),
	D3D_TEST_BOX( 2048 * 16,     4096 * 16,  0, 1, 0, 1 ),
	D3D_TEST_BOX(        16,            16,  0, 1, 0, 1 )
};

Point CResourceAccessTest::m_pPoints [ ] =
{
	{ 0,				0,				0 },
	{ 4,				0,				0 },
	{ 0,				4,				0 }
};

Point CResourceAccessTest::m_pPointsWithDepth [ ] =
{
	{ 0,				0,				0 },
	{ 4,				0,				0 },
	{ 0,				0,				1 },
	{ 4,				4,				1 }
};

FLOAT_COLOR CResourceAccessTest::m_ColorSets [ NUM_COLOR_SETS ] [ NUM_COLORS ] = 
{
	{
		{ 1.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f, 1.f },
		{ 0.f, 0.f, 1.f, 1.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	},
	{
		{ 0.f, 0.f, 0.f, 1.f },
		{ 110.f, 1.5f, 0.f, 0.5f },
		{ -23.f, 0.f, 1.f, 0.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	},
	{
		{ 1.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f, 1.f },
		{ 0.f, 0.f, 1.f, 1.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	}
};

BEGIN_NAMED_VALUES( TEST_COPY_FLAGS )
	NAMED_VALUE( _T( "0" ), (TEST_COPY_FLAGS)0 )
	NAMED_VALUE( _T( "D3D11_COPY_NO_OVERWRITE" ), D3D11_COPY_NO_OVERWRITE_TEST )
	NAMED_VALUE( _T( "D3D11_COPY_TILEABLE" ), D3D11_COPY_TILEABLE )
	NAMED_VALUE( _T( "D3D11_COPY_DISCARD" ), D3D11_COPY_DISCARD_TEST )
END_NAMED_VALUES( TEST_COPY_FLAGS )

// Comparator for std::map, this will be called twice, with left and right swapped, to determine ordering as well as matching
// We will use matching order of: Format, Width, Height, ColorSet
bool operator <(const TexDataInfo &left, const TexDataInfo &right)
{
	return ((left.Format < right.Format) ||
		(left.Format == right.Format && left.Width < right.Width) ||
		(left.Format == right.Format && left.Width == right.Width && left.Height < right.Height) ||
		(left.Format == right.Format && left.Width == right.Width && left.Height == right.Height && left.ColorSet < right.ColorSet));
}

void __stdcall Tokenize( const tstring &str, vector<tstring> *pTokens )
{
	list<UINT> vIdx;
	for( UINT i = 0; i < str.length( ); ++i )
		if( str[ i ] == ',' )
			vIdx.push_back( i );

	list<UINT>::const_iterator itr = vIdx.begin( );
	int last = 0;
	while( itr != vIdx.end( ) )
	{
		pTokens->push_back( str.substr( last, *itr - last ) );
		last = *itr++ + 2;
	}
	pTokens->push_back( str.substr( last ) );
}

tstring __stdcall ToString( const D3D_TEST_BOX &box )
{
	return	
		ToString( box.left ) + _T( ", " ) + ToString( box.right ) + _T( ", " ) +
		ToString( box.top ) + _T( ", " ) + ToString( box.bottom ) + _T( ", " ) +
		ToString( box.front ) + _T( ", " ) + ToString( box.back );
}

bool __stdcall FromString( D3D_TEST_BOX *pBox, const tstring &str )
{
	vector<tstring> vStr;
	Tokenize( str, &vStr );

	FromString( &pBox->left, vStr[ 0 ] );
	FromString( &pBox->right, vStr[ 1 ] );
	FromString( &pBox->top, vStr[ 2 ] );
	FromString( &pBox->bottom, vStr[ 3 ] );
	FromString( &pBox->front, vStr[ 4 ] );
	FromString( &pBox->back, vStr[ 5 ] );

	return true;
}

tstring __stdcall ToString( const Point &point )
{
	return 
		ToString( point.x ) + _T( "," ) +
		ToString( point.y ) + _T( "," ) +
		ToString( point.z );
}

bool __stdcall FromString( Point *pPoint, const tstring &str )
{
	vector<tstring> vStr;
	Tokenize( str, &vStr );

	FromString( &pPoint->x, vStr[ 0 ] );
	FromString( &pPoint->y, vStr[ 1 ] );
	FromString( &pPoint->z, vStr[ 2 ] );

	return true;
}

bool GetScenarioFlags( D3D_RESOURCE_SCENARIO scenario, UINT *pFlags )
{
	if( pFlags == NULL )
		return false;

	*pFlags = 0;
	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		*pFlags |= D3D_RS_FLAGS_BUFFER;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pFlags |= D3D_RS_FLAGS_ARRAY;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pFlags |= D3D_RS_FLAGS_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pFlags |= D3D_RS_FLAGS_ARRAY;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pFlags = (*pFlags | D3D_RS_FLAGS_MS | D3D_RS_FLAGS_TEXTURE2D);
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pFlags |= D3D_RS_FLAGS_ARRAY;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pFlags |= D3D_RS_FLAGS_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		*pFlags |= D3D_RS_FLAGS_TEXTURECUBE;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		*pFlags |= D3D_RS_FLAGS_TEXTURE3D;
		break;
	}

	return true;
}

bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_UAV_DIMENSION *pUAVDim )
{
	if( !pUAVDim )
		return false;

	switch( scenario )
	{
	default:
		return false;
	case D3D_RESOURCE_SCENARIO_BUFFER:
		*pUAVDim = D3D11_UAV_DIMENSION_BUFFER;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pUAVDim = D3D11_UAV_DIMENSION_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pUAVDim = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pUAVDim = D3D11_UAV_DIMENSION_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pUAVDim = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		*pUAVDim = D3D11_UAV_DIMENSION_TEXTURE3D;
		break;
	}

	return true;
}

bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_SRV_DIMENSION *pSRVDim )
{
	if( !pSRVDim )
		return false;

	switch( scenario )
	{
	default:
		return false;
	case D3D_RESOURCE_SCENARIO_BUFFER:
		*pSRVDim = D3D11_SRV_DIMENSION_BUFFER;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE3D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURECUBE;
		break;
	}

	return true;
}

bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_RTV_DIMENSION *pRTVDim )
{
	if( !pRTVDim )
		return false;

	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		*pRTVDim = D3D11_RTV_DIMENSION_BUFFER;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE3D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		return false;
	}

	return true;
}

bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_DSV_DIMENSION *pDSVDim )
{
	if( !pDSVDim )
		return false;

	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_BUFFER:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		return false;
	}

	return true;
}

tstring __stdcall ToString( const D3D_RESOURCE_SCENARIO &scenario )
{
	tstring res;
	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		res = _T( "BUFFER" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		res = _T( "TEXTURE1D" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		res = _T( "TEXTURE1D_ARRAY" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		res = _T( "TEXTURE2D" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		res = _T( "TEXTURE2D_ARRAY" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		res = _T( "TEXTURE2D_MS" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		res = _T( "TEXTURE2D_MS_ARRAY" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		res = _T( "TEXTURECUBE" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		res = _T( "TEXTURE3D" );
		break;
	}

	return res;
}

bool __stdcall FromString( D3D_RESOURCE_SCENARIO *pScenario, const tstring &str )
{
	if( str == _T( "BUFFER" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_BUFFER;
	else if( str == _T( "TEXTURE1D" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE1D;
	else if( str == _T( "TEXTURE1D_ARRAY" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY;
	else if( str == _T( "TEXTURE2D" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D;
	else if( str == _T( "TEXTURE2D_ARRAY" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY;
	else if( str == _T( "TEXTURE2D_MS" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D_MS;
	else if( str == _T( "TEXTURE2D_MS_ARRAY" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY;
	else if( str == _T( "TEXTURECUBE" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURECUBE;
	else if( str == _T( "TEXTURE3D" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE3D;
	else
		return false;

	return true;
}

tstring __stdcall ToString( const ACCESS_METHOD &access )
{
	tstring res;
	switch( access )
	{
	case AM_COPY:
		res = _T( "CopyResource" );
		break;
	case AM_COPYREGION:
		res = _T( "CopySubresourceRegion" );
		break;
	case AM_UPDATE:
		res = _T( "UpdateSubresource" );
		break;
	case AM_MAP:
		res = _T( "Map" );
		break;

	}

	return res;
}

bool __stdcall FromString( ACCESS_METHOD *pAccess, const tstring &str )
{
	if( str == _T( "CopyResource" ) )
		*pAccess = AM_COPY;
	else if( str == _T( "CopySubresourceRegion" ) )
		*pAccess = AM_COPYREGION;
	else if( str == _T( "UpdateSubresource" ) )
		*pAccess = AM_UPDATE;
	else if( str == _T( "Map" ) )
		*pAccess = AM_MAP;
	else
		return false;

	return true;
}

static HRESULT CreateStagingCopy(ID3D11Device* pDevice, ID3D11DeviceContext *pContext, ID3D11Resource* pSrc, ID3D11Resource** pDest)
{
	HRESULT hr = E_UNEXPECTED;
	*pDest = NULL;

	D3D11_RESOURCE_DIMENSION dimension;
	pSrc->GetType(&dimension);
	switch (dimension)
	{
	case D3D11_RESOURCE_DIMENSION_UNKNOWN:
		hr = E_FAIL;
		break;
	case D3D11_RESOURCE_DIMENSION_BUFFER:
		{
			ID3D11Buffer *pDestBuf = NULL;
			ID3D11Buffer *pBuf = (ID3D11Buffer*)pSrc;
			D3D11_BUFFER_DESC desc;
			pBuf->GetDesc(&desc);

			desc.MiscFlags &= ~D3D11_RESOURCE_MISC_TILED; // Hide the fact that this resource may be tiled
			desc.BindFlags = 0;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			if (FAILED( hr = pDevice->CreateBuffer(&desc, NULL, &pDestBuf) ))
				break;
			pContext->CopyResource(pDestBuf, pBuf);
			*pDest = pDestBuf;
		}
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		{
			ID3D11Texture1D *pDestTex = NULL;
			ID3D11Texture1D *pTex = (ID3D11Texture1D*)pSrc;
			D3D11_TEXTURE1D_DESC desc;
			pTex->GetDesc(&desc);

			desc.BindFlags = 0;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			if (FAILED( hr = pDevice->CreateTexture1D(&desc, NULL, &pDestTex) ))
				break;
			pContext->CopyResource(pDestTex, pTex);
			*pDest = pDestTex;
		}
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		{
			ID3D11Texture2D *pDestTex = NULL;
			ID3D11Texture2D *pTex = (ID3D11Texture2D*)pSrc;
			D3D11_TEXTURE2D_DESC desc;
			pTex->GetDesc(&desc);
			desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE; // Only inherit one flag

			desc.BindFlags = 0;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			if (FAILED( hr = pDevice->CreateTexture2D(&desc, NULL, &pDestTex) ))
				break;
			pContext->CopyResource(pDestTex, pTex);
			*pDest = pDestTex;
		}
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		{
			ID3D11Texture3D *pDestTex = NULL;
			ID3D11Texture3D *pTex = (ID3D11Texture3D*)pSrc;
			D3D11_TEXTURE3D_DESC desc;
			pTex->GetDesc(&desc);

			desc.BindFlags = 0;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			if (FAILED( hr = pDevice->CreateTexture3D(&desc, NULL, &pDestTex) ))
				break;
			pContext->CopyResource(pDestTex, pTex);
			*pDest = pDestTex;
		}
		break;
	}
	return hr;
}

void CResourceAccessTest::InitTestParameters()
{
	CTestCaseParameter<UINT> *pArrayParam = AddParameter( _T( "ArraySize" ), &m_uArraySize );
	SetParameterDefaultValue<UINT>(pArrayParam, 1);
	CUserValueSet<UINT> *pArrayValues = new CUserValueSet<UINT>( );
	pArrayValues->AddValue( 1 );
	pArrayValues->AddValue( 10 );
	pArrayValues->AddValue( 512 );
	testfactor::RFactor rfArraySize = AddParameterValueSet( pArrayParam, pArrayValues );
	testfactor::RFactor rfArraySizeMed = AddParameterValue<UINT>( pArrayParam, 10 );

	CTestCaseParameter<UINT> *pSrcSubresParam = AddParameter( _T( "SrcSubres" ), &m_uSrcSubres );
	SetParameterDefaultValue<UINT>(pSrcSubresParam, 0);
	testfactor::RFactor rfSrcSubres0 = AddParameterValue<UINT>( pSrcSubresParam, 0 );
	testfactor::RFactor rfSrcSubresFew = AddParameterValueSet( pSrcSubresParam, RangeValueSet<UINT>(0, 3, 3) );
	testfactor::RFactor rfSrcSubresCube = AddParameterValueSet( pSrcSubresParam, RangeValueSet<UINT>(0, 5, 1) );

	CTestCaseParameter<UINT> *pDestSubresParam = AddParameter( _T( "DestSubres" ), &m_uDestSubres );
	SetParameterDefaultValue<UINT>(pDestSubresParam, 0);
	testfactor::RFactor rfDestSubres0 = AddParameterValue<UINT>( pDestSubresParam, 0 );
	testfactor::RFactor rfDestSubresFew = AddParameterValueSet( pDestSubresParam, RangeValueSet<UINT>(0, 3, 3) );
	testfactor::RFactor rfDestSubresCube = AddParameterValueSet( pDestSubresParam, RangeValueSet<UINT>(0, 5, 1) );

	CTestCaseParameter<bool> *pSameResParam = AddParameter( _T( "SameResource" ), &m_bSameResource );
	SetParameterDefaultValue<bool>(pSameResParam, false);
	testfactor::RFactor rfSameResFalse = AddParameterValue( pSameResParam, false );
	testfactor::RFactor rfSameResTrue = AddParameterValue( pSameResParam, true );
	testfactor::RFactor rfSameRes = rfSameResFalse + rfSameResTrue;

	CTestCaseParameter<UINT32> *pWidthParam = AddParameter( _T( "Width" ), &m_uWidth );
	CUserValueSet<UINT32> *pWidthValues = new CUserValueSet<UINT32>( );
	pWidthValues->AddValue( 32 );
	pWidthValues->AddValue( 16 );
	pWidthValues->AddValue( 10 );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );
	testfactor::RFactor rfWidth32 = AddParameterValue( pWidthParam, 32u );
	testfactor::RFactor rfWidth1 = AddParameterValue( pWidthParam, 1u );
	testfactor::RFactor rfWidth2 = AddParameterValue( pWidthParam, 2u );

	CTestCaseParameter<UINT32> *pHeightParam = AddParameter( _T( "Height" ), &m_uHeight );
	CUserValueSet<UINT32> *pHeightValues = new CUserValueSet<UINT32>( );
	pHeightValues->AddValue( 32 );
	pHeightValues->AddValue( 64 );
	pHeightValues->AddValue( 10 );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );
	testfactor::RFactor rfHeight32 = AddParameterValue<UINT32>( pHeightParam, 32 );

	CTestCaseParameter<UINT32> *pDepthParam = AddParameter( _T( "Depth" ), &m_uDepth );
	CUserValueSet<UINT32> *pDepthValues = new CUserValueSet<UINT32>( );
	//pDepthValues->AddValue( 10 );
	pDepthValues->AddValue( 4 );
	pDepthValues->AddValue( 2 );
	pDepthValues->AddValue( 1 );
	testfactor::RFactor rfDepth = AddParameterValueSet( pDepthParam, pDepthValues );
	testfactor::RFactor rfDepth4 = AddParameterValue<UINT32>( pDepthParam, 4 );

	CTestCaseParameter<ACCESS_METHOD> *pAccessParam = AddParameter( _T("AccessMethod"), &m_AccessMethod );
	testfactor::RFactor rfCopy = AddParameterValue( pAccessParam, AM_COPY );
	testfactor::RFactor rfCopyRegion = AddParameterValue( pAccessParam, AM_COPYREGION );
	testfactor::RFactor rfUpdate = AddParameterValue( pAccessParam, AM_UPDATE );
	testfactor::RFactor rfMap = AddParameterValue( pAccessParam, AM_MAP );

	CTestCaseParameter<D3D_RESOURCE_SCENARIO> *pResParam = AddParameter( _T("ResourceScenario"), &m_ResourceScenario );
	testfactor::RFactor rfResBuff = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_BUFFER );
	testfactor::RFactor rfResTex1D = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE1D );
	testfactor::RFactor rfResTex1DArr = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY );
	testfactor::RFactor rfResTex2D = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D );
	testfactor::RFactor rfResTex2DArr = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY );
	testfactor::RFactor rfResTex3D = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE3D );
	testfactor::RFactor rfResTexCube = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURECUBE );
	
	CUserValueSet<DXGI_FORMAT> *pFormatSrcValues = new CUserValueSet<DXGI_FORMAT>( );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R32G32B32A32_FLOAT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R32G32B32A32_UINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R32G32B32_FLOAT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R32G32B32_SINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R16G16B16A16_SNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R16G16B16A16_UINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R16G16B16A16_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R16G16B16A16_SINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R32G32_FLOAT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R32G32_SINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_D32_FLOAT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R32_UINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R32_SINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_D24_UNORM_S8_UINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R24_UNORM_X8_TYPELESS );
	pFormatSrcValues->AddValue( DXGI_FORMAT_X24_TYPELESS_G8_UINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R8G8_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R8G8_UINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R16_FLOAT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_D16_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R16_SNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R16_SINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R16_FLOAT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R8_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R8_SINT );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC1_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC1_UNORM_SRGB );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC2_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC2_UNORM_SRGB );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC3_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC3_UNORM_SRGB );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC4_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC4_SNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC5_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC5_SNORM );

	CUserValueSet<DXGI_FORMAT> *pFormatDestValues = new CUserValueSet<DXGI_FORMAT>( );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32G32B32A32_UINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32G32B32A32_FLOAT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32G32B32_SINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32G32B32_FLOAT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R16G16B16A16_UINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R16G16B16A16_SNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_R16G16B16A16_SINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R16G16B16A16_UNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32G32_SINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32G32_FLOAT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32_SINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_D32_FLOAT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32_UINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_X24_TYPELESS_G8_UINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_D24_UNORM_S8_UINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R24_UNORM_X8_TYPELESS );
	pFormatDestValues->AddValue( DXGI_FORMAT_R8G8_UINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R8G8_UNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_R16_UINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R16_FLOAT );
	pFormatDestValues->AddValue( DXGI_FORMAT_D16_UNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_R16_SNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_R16_SINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R8_SINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R8_UNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_BC1_UNORM_SRGB );
	pFormatDestValues->AddValue( DXGI_FORMAT_BC1_UNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_BC2_UNORM_SRGB );
	pFormatDestValues->AddValue( DXGI_FORMAT_BC2_UNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_BC3_UNORM_SRGB );
	pFormatDestValues->AddValue( DXGI_FORMAT_BC3_UNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_BC4_SNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_BC4_UNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_BC5_SNORM );
	pFormatDestValues->AddValue( DXGI_FORMAT_BC5_UNORM );
	
	CTestCaseParameter<DXGI_FORMAT>* pSrcFormatParam = AddParameter(  _T("SrcFormat"), &m_SrcFormat );
	testfactor::RFactor rfFormatSrc = AddParameterValueSet( pSrcFormatParam, pFormatSrcValues );
	testfactor::RFactor rfFormatDest = AddParameter( _T("DestFormat"), &m_DestFormat, pFormatDestValues );

	// BC 6 and 7 encoding is slow, create test factors that will be cycled less frequently
	CUserValueSet<DXGI_FORMAT> *pFormatBC6and7Values = new CUserValueSet<DXGI_FORMAT>( );
	pFormatBC6and7Values->AddValue( DXGI_FORMAT_BC6H_UF16 );
	pFormatBC6and7Values->AddValue( DXGI_FORMAT_BC6H_SF16 );
	pFormatBC6and7Values->AddValue( DXGI_FORMAT_BC7_UNORM );
	pFormatBC6and7Values->AddValue( DXGI_FORMAT_BC7_UNORM_SRGB );

	testfactor::RFactor rfFormatSrc_BC6and7 = AddParameterValueSet( pSrcFormatParam, pFormatBC6and7Values );
	testfactor::RFactor rfFormatDest_BC6and7 = AddParameterValueSet( _T("DestFormat"), pFormatBC6and7Values );

	// test all formats against themselves, minus those that can't be tested
	CFilterHandleCommonSource<DXGI_FORMAT> formatFilter = !(
		FilterRegExp<DXGI_FORMAT>( _T( "420_OPAQUE" ) ) ||	// BUGBUG: One of the new video formats. This fails and should be removed when the test is updated to support these new formats.

		FilterRegExp<DXGI_FORMAT>( _T( "UNKNOWN" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "TYPELESS" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "SHAREDEXP" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "R1" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "R8G8_B8G8" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "G8R8_G8B8" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "BC6" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "BC7" ) ) );
	testfactor::RFactor rfFormatSrcSame = AddParameter( _T("SrcFormatSame"), &m_SrcFormatSame, FormatValueSet( formatFilter ) );
	testfactor::RFactor rfFormatDestSame = AddParameter( _T("DestFormatSame"), &m_DestFormatSame, FormatValueSet( formatFilter ) );
	testfactor::RFactor rfFormatSrcSame_BC6and7 = AddParameterValueSet( _T("SrcFormatSame"), pFormatBC6and7Values );
	testfactor::RFactor rfFormatDestSame_BC6and7 = AddParameterValueSet( _T("DestFormatSame"), pFormatBC6and7Values );
	testfactor::RFactor rfFormatSrcSame_RGBA8 = AddParameterValue( _T("SrcFormatSame"), DXGI_FORMAT_R8G8B8A8_UNORM );
	testfactor::RFactor rfFormatDestSame_RGBA8 = AddParameterValue( _T("DestFormatSame"), DXGI_FORMAT_R8G8B8A8_UNORM );

	CUserValueSet<DXGI_FORMAT> *pFormatTileableValues = new CUserValueSet<DXGI_FORMAT>( );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R32G32B32A32_FLOAT );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R32G32B32_FLOAT );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R16G16B16A16_FLOAT );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R16G16B16A16_SNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R16G16B16A16_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R32G32_FLOAT );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R8G8_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R16_FLOAT );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R16_SNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R8_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R10G10B10A2_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R11G11B10_FLOAT );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R8G8B8A8_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R8G8B8A8_SNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R16G16_FLOAT );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R16G16_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R16G16_SNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R32_FLOAT );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R8G8_SNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R16_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_R8_SNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_A8_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_B5G6R5_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_B5G5R5A1_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_B8G8R8A8_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_B8G8R8X8_UNORM );
	pFormatTileableValues->AddValue( DXGI_FORMAT_B8G8R8A8_UNORM_SRGB );
	pFormatTileableValues->AddValue( DXGI_FORMAT_B8G8R8X8_UNORM_SRGB );
	testfactor::RFactor rfFormatSrcSame_Tileable = AddParameterValueSet( _T("SrcFormatSame"), pFormatTileableValues );
	testfactor::RFactor rfFormatDestSame_Tileable = AddParameterValueSet( _T("DestFormatSame"), pFormatTileableValues );

	// For texture readback test all formats against themselves, minus those that can't be tested (depth, etc...)
	CFilterHandleCommonSource<DXGI_FORMAT> formatFilter_textureReadback = !(
		FilterRegExp<DXGI_FORMAT>( _T( "420_OPAQUE" ) ) ||	// Video format
		FilterRegExp<DXGI_FORMAT>( _T( "A8P8" ) ) ||		// Video format
		FilterRegExp<DXGI_FORMAT>( _T( "AI44" ) ) ||		// Video format
		FilterRegExp<DXGI_FORMAT>( _T( "IA44" ) ) ||		// Video format
		FilterRegExp<DXGI_FORMAT>( _T( "P8" ) ) ||			// Video format
		FilterRegExp<DXGI_FORMAT>( _T( "NV" ) ) ||			// Video formats NV11, NV12
		FilterRegExp<DXGI_FORMAT>( _T( "P0" ) ) ||			// Video formats P010, P016
		FilterRegExp<DXGI_FORMAT>( _T( "Y2" ) ) ||			// Video formats Y210, Y216
		FilterRegExp<DXGI_FORMAT>( _T( "YUY2" ) ) ||		// Video format
		FilterRegExp<DXGI_FORMAT>( _T( "AYUV" ) ) ||		// Video format
		FilterRegExp<DXGI_FORMAT>( _T( "Y4" ) ) ||			// Video formats Y410, Y416
		FilterRegExp<DXGI_FORMAT>( _T( "UNKNOWN" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "TYPELESS" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "R1" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "D" ) ) );			// Depth
	testfactor::RFactor rfFormatSrcSame_TextureReadback = AddParameterValueSet( _T("SrcFormatSame"), FormatValueSet( formatFilter_textureReadback ) );
	testfactor::RFactor rfFormatDestSame_TextureReadback = AddParameterValueSet( _T("DestFormatSame"), FormatValueSet( formatFilter_textureReadback ) );

	CTestCaseParameter<D3D_TEST_BOX> *pSrcBoxParam = AddParameter( _T("SrcBox"), &m_SrcBox );
	testfactor::RFactor rfSrcBoxDef = AddParameterValue( pSrcBoxParam, m_pBoxes[ 1 ] );
	testfactor::RFactor rfSrcBox = AddParameterValueSet( pSrcBoxParam,
		new CTableValueSet<D3D_TEST_BOX>( m_pBoxes, sizeof( D3D_TEST_BOX ), sizeof( m_pBoxes ) / sizeof( D3D_TEST_BOX ) ) );
	testfactor::RFactor rfSrcBoxNonZero = AddParameterValueSet( pSrcBoxParam,
		new CTableValueSet<D3D_TEST_BOX>( &m_pBoxes[1], sizeof( D3D_TEST_BOX ), sizeof( m_pBoxes ) / sizeof( D3D_TEST_BOX ) - 1 ) );
	testfactor::RFactor rfSrcBoxWithDepth = AddParameterValueSet( pSrcBoxParam,
		new CTableValueSet<D3D_TEST_BOX>( m_pBoxesWithDepth, sizeof( D3D_TEST_BOX ), sizeof( m_pBoxesWithDepth ) / sizeof( D3D_TEST_BOX ) ) );

	CTestCaseParameter<bool> *pNullSrcBoxParam = AddParameter( _T("NullSrcBox"), &m_bNULLSrcBox );
	testfactor::RFactor rfNotNULLSrcBox = AddParameterValue( pNullSrcBoxParam, false );
	testfactor::RFactor rfNULLSrcBox = AddParameterValue( pNullSrcBoxParam, true );
	SetParameterDefaultValue( pNullSrcBoxParam, false );

	CTestCaseParameter<bool> *pDepthResParam = AddParameter( _T( "DepthResource" ), &m_bDepthResource );
	SetParameterDefaultValue<bool>(pDepthResParam, false);
	testfactor::RFactor rfDepthRes = AddParameterValueSet( pDepthResParam, BoolValueSet() );

	CTestCaseParameter<Point> *pDstPointParam = AddParameter( _T( "DestPoint" ), &m_DestPoint );
	testfactor::RFactor rfDstPointDef = AddParameterValue( pDstPointParam, m_pPoints[ 0 ] );
	testfactor::RFactor rfDstPoint = AddParameterValueSet( pDstPointParam, 
		new CTableValueSet<Point>( m_pPoints, sizeof( Point ), sizeof( m_pPoints ) / sizeof( Point ) ) );
	testfactor::RFactor rfDstPointWithDepth = AddParameterValueSet( pDstPointParam, 
		new CTableValueSet<Point>( m_pPointsWithDepth, sizeof( Point ), sizeof( m_pPointsWithDepth ) / sizeof( Point ) ) );

	CTestCaseParameter<D3D11_MAP> *pMapParam = AddParameter( _T( "MapType" ), &m_MapType );
	CUserValueSet<D3D11_MAP> *pMapValues = new CUserValueSet<D3D11_MAP>( );
	pMapValues->AddValue( D3D11_MAP_READ );
	pMapValues->AddValue( D3D11_MAP_WRITE );
	pMapValues->AddValue( D3D11_MAP_READ_WRITE );
	testfactor::RFactor rfMapType = AddParameterValueSet( pMapParam, pMapValues );
	CUserValueSet<D3D11_MAP> *pDynamicMapValues = new CUserValueSet<D3D11_MAP>( );
	pDynamicMapValues->AddValue( D3D11_MAP_WRITE_DISCARD );
	pDynamicMapValues->AddValue( D3D11_MAP_WRITE_NO_OVERWRITE );
	testfactor::RFactor rfDynamicBufMapType = AddParameterValueSet( pMapParam, pDynamicMapValues );

	CTestCaseParameter<D3D11_USAGE> *pMapUsage = AddParameter( _T( "MapUsage" ), &m_MapUsage );
	CUserValueSet<D3D11_USAGE> *pMapUsageValues = new CUserValueSet<D3D11_USAGE>( );
	pMapUsageValues->AddValue( D3D11_USAGE_STAGING );
	pMapUsageValues->AddValue( D3D11_USAGE_DEFAULT );
	testfactor::RFactor rfMapUsage = AddParameterValueSet<D3D11_USAGE>( pMapUsage, pMapUsageValues );
	testfactor::RFactor rfMapUsageStaging = AddParameterValue<D3D11_USAGE>( pMapUsage, D3D11_USAGE_STAGING );
	SetParameterDefaultValue( pMapUsage, D3D11_USAGE_STAGING );
	
	CTestCaseParameter<bool> *pDoNotWaitParam = AddParameter( _T("MapDoNotWait"), &m_bDoNotWait );
	testfactor::RFactor rfDoNotWait = AddParameterValueSet<bool>( pDoNotWaitParam, new CBoolValueSet() );
	testfactor::RFactor rfDoNotWaitF = AddParameterValue<bool>( pDoNotWaitParam, false );

	CTestCaseParameter<D3D11_BIND_FLAG> *pBindFlagsParam = AddParameter( _T( "BindFlags" ), &m_BindFlags );
	CUserValueSet<D3D11_BIND_FLAG> *pBufferBindFlagValues = new CUserValueSet<D3D11_BIND_FLAG>( );
	// m_BindFlags is ignored for the Buffer case
	/*pBufferBindFlagValues->AddValue( D3D11_BIND_CONSTANT_BUFFER );
	pBufferBindFlagValues->AddValue( D3D11_BIND_INDEX_BUFFER );
	pBufferBindFlagValues->AddValue( D3D11_BIND_VERTEX_BUFFER );*/
	pBufferBindFlagValues->AddValue( (D3D11_BIND_FLAG) 0 );
	testfactor::RFactor rfBufferBindFlags = AddParameterValueSet( pBindFlagsParam, pBufferBindFlagValues );
	testfactor::RFactor rfBindRT = AddParameterValue( pBindFlagsParam, D3D11_BIND_RENDER_TARGET );
	SetParameterDefaultValue( pBindFlagsParam, (D3D11_BIND_FLAG)0 );

	CTestCaseParameter<TEST_COPY_FLAGS> *pCopyFlagsParam = AddParameter( _T( "CopyFlags" ), (TEST_COPY_FLAGS*)&m_CopyFlags );
	AddParameterValue( pCopyFlagsParam, (TEST_COPY_FLAGS)0 );
	testfactor::RFactor rfTileable = AddParameterValue<TEST_COPY_FLAGS>( pCopyFlagsParam, D3D11_COPY_TILEABLE );
	testfactor::RFactor rfDiscard = AddParameterValue<TEST_COPY_FLAGS>( pCopyFlagsParam, D3D11_COPY_DISCARD_TEST );
	SetParameterDefaultValue<TEST_COPY_FLAGS>( _T("CopyFlags"), (TEST_COPY_FLAGS)0 );

	CTestCaseParameter<bool> *pTextureReadbackParam = AddParameter( _T("TextureReadback"), &m_bTextureReadback );
	SetParameterDefaultValue<bool>( pTextureReadbackParam, false );
	testfactor::RFactor rfTextureReadbackT = AddParameterValue<bool>( pTextureReadbackParam, true );
	testfactor::RFactor rfTextureReadbackF = AddParameterValue<bool>( pTextureReadbackParam, false );

	CTestCaseParameter<D3D11_CPU_ACCESS_FLAG> *pCPUAccessFlagsParam = AddParameter( _T( "CPUAccessFlags" ), &m_CPUAccessFlags );
	SetParameterDefaultValue<D3D11_CPU_ACCESS_FLAG>( pCPUAccessFlagsParam, (D3D11_CPU_ACCESS_FLAG)0 );
	CUserValueSet<D3D11_CPU_ACCESS_FLAG> *pCPUAccessFlagsValues = new CUserValueSet<D3D11_CPU_ACCESS_FLAG>();
	pCPUAccessFlagsValues->AddValue( D3D11_CPU_ACCESS_READ );
	pCPUAccessFlagsValues->AddValue( D3D11_CPU_ACCESS_WRITE );
	pCPUAccessFlagsValues->AddValue( (D3D11_CPU_ACCESS_FLAG)(D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE));
	testfactor::RFactor rfCPUAccessFlags = AddParameterValueSet( pCPUAccessFlagsParam, pCPUAccessFlagsValues );

	testfactor::RFactor rfUpdateNotNullSrcBox =
		( rfUpdate * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTex1D    * rfWidth * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF ) +
		( rfUpdate * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTex1DArr * rfWidth * rfArraySize * rfNotNULLSrcBox *  rfSrcBox * rfDoNotWaitF ) +
		( rfUpdate * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTex2D    * rfWidth * rfHeight * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF ) +
		( rfUpdate * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTex2DArr * rfWidth * rfHeight * rfArraySize * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF ) +
		( rfUpdate * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTex3D    * rfWidth * rfHeight * rfDepth4 * rfNotNULLSrcBox * rfSrcBoxWithDepth * rfDoNotWaitF ) +
		( rfUpdate * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTexCube  * rfWidth * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF ) +

		( rfUpdate * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfSrcSubres0 * rfResTex2D    * rfWidth32 * rfHeight32 * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF ) +
		( rfUpdate * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfSrcSubres0 * rfResTex2DArr * rfWidth32 * rfHeight32 * rfArraySizeMed * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF ) +
		( rfUpdate * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfSrcSubres0 * rfResTex3D    * rfWidth32 * rfHeight32 * rfDepth4 * rfNotNULLSrcBox * rfSrcBoxWithDepth * rfDoNotWaitF ) +
		( rfUpdate * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfSrcSubres0 * rfResTexCube  * rfWidth32 * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF ) +

		( rfUpdate * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubresFew * rfResTex1DArr * rfWidth32 * rfArraySizeMed * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF ) +
		( rfUpdate * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubresFew * rfResTex2DArr * rfWidth32 * rfHeight32 * rfArraySizeMed * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF ) +
		( rfUpdate * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubresFew * rfResTexCube  * rfWidth32 * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF ) +
		( rfUpdate * ( rfFormatSrcSame_RGBA8 % rfFormatDestSame_RGBA8 ) * rfSrcSubresCube* rfResTexCube  * rfWidth32 * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF ) + 

		( rfUpdate * ( rfFormatSrcSame_RGBA8 % rfFormatDestSame_RGBA8 ) * rfSrcSubresFew * rfResTex1DArr * rfWidth32 * rfArraySizeMed * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF * rfDiscard ) +
		( rfUpdate * ( rfFormatSrcSame_RGBA8 % rfFormatDestSame_RGBA8 ) * rfSrcSubresFew * rfResTex2DArr * rfWidth32 * rfHeight32 * rfArraySizeMed * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF * rfDiscard ) +
		( rfUpdate * ( rfFormatSrcSame_RGBA8 % rfFormatDestSame_RGBA8 ) * rfSrcSubresFew * rfResTexCube  * rfWidth32 * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF * rfDiscard ) +
		( rfUpdate * ( rfFormatSrcSame_RGBA8 % rfFormatDestSame_RGBA8 ) * rfSrcSubresCube* rfResTexCube  * rfWidth32 * rfNotNULLSrcBox * rfSrcBox * rfDoNotWaitF * rfDiscard ) +

		rfUpdate * AddParameterValue( pBindFlagsParam, D3D11_BIND_CONSTANT_BUFFER ) * rfSrcSubres0 * rfResBuff * AddParameterValueSet( pCopyFlagsParam, new CUserValueSet<TEST_COPY_FLAGS>( D3D11_COPY_NO_OVERWRITE_TEST, D3D11_COPY_DISCARD_TEST ) ) * rfNotNULLSrcBox * rfDoNotWaitF * AddParameterValue( pSrcFormatParam, DXGI_FORMAT_R32G32B32A32_UINT ) *
		(
			rfWidth1 * AddParameterValueSet( pSrcBoxParam, new CTableValueSet<D3D_TEST_BOX>( g_pConstantBufferBoxes, sizeof(D3D_TEST_BOX), 6 ) ) +
			rfWidth2 * AddParameterValue( pSrcBoxParam, g_pConstantBufferBoxes[1] ) + 
			rfWidth2 * AddParameterValue( pSrcBoxParam, g_pConstantBufferBoxes[9] ) + 
			AddParameterValue( pWidthParam, 4096u ) * AddParameterValue( pSrcBoxParam, g_pConstantBufferBoxes[6] ) + 
			AddParameterValue( pWidthParam, 4096u ) * AddParameterValueSet( pSrcBoxParam, new CTableValueSet<D3D_TEST_BOX>( g_pConstantBufferBoxes + 8, sizeof(D3D_TEST_BOX), 2 ) ) + 
			AddParameterValue( pWidthParam, 4096u * 2u ) * AddParameterValue( pSrcBoxParam, g_pConstantBufferBoxes[7] ) + 
			AddParameterValue( pWidthParam, 4096u * 3u ) * AddParameterValue( pSrcBoxParam, g_pConstantBufferBoxes[8] )
		);

	testfactor::RFactor rfUpdateNullSrcBox =
		( rfUpdate * rfFormatSrcSame * rfSrcSubres0 * rfResTex1D * rfWidth * rfNULLSrcBox * rfDepthRes * rfDoNotWaitF ) +
		( rfUpdate * rfFormatSrcSame * rfSrcSubres0 * rfResTex1DArr * rfWidth * rfArraySize * rfNULLSrcBox * rfDepthRes * rfDoNotWaitF ) +
		( rfUpdate * rfFormatSrcSame * rfSrcSubres0 * rfResTex2D * rfWidth * rfHeight * rfNULLSrcBox * rfDepthRes * rfDoNotWaitF ) +
		( rfUpdate * rfFormatSrcSame * rfSrcSubres0 * rfResTex2DArr * rfWidth * rfHeight * rfArraySize * rfNULLSrcBox * rfDepthRes * rfDoNotWaitF ) +
		( rfUpdate * rfFormatSrcSame * rfSrcSubres0 * rfResTex3D * rfWidth * rfHeight * rfDepth * rfNULLSrcBox * rfDoNotWaitF ) +
		( rfUpdate * rfFormatSrcSame * rfSrcSubres0 * rfResTexCube * rfWidth * rfNULLSrcBox * rfDepthRes * rfDoNotWaitF ) +
		( rfUpdate * rfSrcSubres0 * rfResBuff * AddParameterValue( pBindFlagsParam, D3D11_BIND_CONSTANT_BUFFER ) * rfNULLSrcBox * rfDoNotWaitF * AddParameterValue( pSrcFormatParam, DXGI_FORMAT_R32G32B32A32_UINT ) *
			( rfWidth1 + AddParameterValue( pWidthParam, 4096u ) + AddParameterValue( pWidthParam, 4096u * 2u ) ) );

	testfactor::RFactor rfTextureReadbackTestCases =
		( rfCopy * ( rfFormatSrcSame_TextureReadback % rfFormatDestSame_TextureReadback ) * rfResTex2D		* rfWidth * rfHeight32 * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame_TextureReadback % rfFormatDestSame_TextureReadback ) * rfResTex2DArr	* rfWidth32 * rfHeight32 * rfArraySizeMed * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame_TextureReadback % rfFormatDestSame_TextureReadback ) * rfResTex3D		* rfWidth32	* rfHeight * rfDepth4 * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame_TextureReadback % rfFormatDestSame_TextureReadback ) * rfResTexCube	* rfWidth32 * rfDoNotWaitF ) +

		( rfCopyRegion * ( rfFormatSrcSame_TextureReadback % rfFormatDestSame_TextureReadback ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex2D    * rfWidth	* rfHeight32	* rfSrcBox		* rfDstPoint		* rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame_TextureReadback % rfFormatDestSame_TextureReadback ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex2DArr * rfWidth32	* rfHeight32	* rfSrcBox		* rfDstPoint		* rfArraySizeMed * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame_TextureReadback % rfFormatDestSame_TextureReadback ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex3D    * rfWidth32	* rfHeight		* rfDepth4		* rfSrcBoxWithDepth * rfDstPointWithDepth * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame_TextureReadback % rfFormatDestSame_TextureReadback ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTexCube  * rfWidth32	* rfSrcBox		* rfDstPoint	* rfDoNotWaitF );
	rfTextureReadbackTestCases = (rfTextureReadbackTestCases * rfTextureReadbackT * rfSameResFalse) % rfCPUAccessFlags;

	SetRootTestFactor(
		( rfCopy * ( rfFormatSrcSame % rfFormatDestSame ) * rfResBuff * rfWidth * rfBufferBindFlags * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame % rfFormatDestSame ) * rfResTex1D * rfWidth * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame % rfFormatDestSame ) * rfResTex1DArr * rfWidth * rfArraySize * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame % rfFormatDestSame ) * rfResTex2D * rfWidth * rfHeight * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame % rfFormatDestSame ) * rfResTex2DArr * rfWidth * rfHeight * rfArraySizeMed * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame % rfFormatDestSame ) * rfResTex3D * rfWidth * rfHeight * rfDepth * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame % rfFormatDestSame ) * rfResTexCube * rfWidth * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrc % rfFormatDest ) * rfResBuff * rfWidth * rfBufferBindFlags * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrc % rfFormatDest ) * rfResTex1D * rfWidth * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrc % rfFormatDest ) * rfResTex1DArr * rfWidth * rfArraySizeMed * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrc % rfFormatDest ) * rfResTex2D * rfWidth * rfHeight * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrc % rfFormatDest ) * rfResTex2DArr * rfWidth * rfHeight * rfArraySize * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrc % rfFormatDest ) * rfResTex3D * rfWidth * rfHeight * rfDepth * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrc % rfFormatDest ) * rfResTexCube * rfWidth * rfDoNotWaitF ) +

		( rfCopy * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfResTex2D * rfWidth32 * rfHeight32 * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfResTex2DArr * rfWidth32 * rfHeight32 * rfArraySizeMed * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfResTex3D * rfWidth32 * rfHeight32 * rfDepth4 * rfDoNotWaitF ) +
		( rfCopy * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfResTexCube * rfWidth32 * rfDoNotWaitF ) +

		( rfCopyRegion * ( rfFormatSrcSame % rfFormatDestSame ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResBuff     * rfWidth * rfSrcBoxDef * rfDstPoint * rfSameRes * rfBufferBindFlags * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame % rfFormatDestSame ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex1D    * rfWidth * rfSrcBoxDef * rfDstPoint * rfSameRes * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame % rfFormatDestSame ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex1DArr * rfWidth * rfSrcBoxDef * rfDstPoint * rfSameRes * rfArraySize * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame % rfFormatDestSame ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex2D    * rfWidth * rfHeight * rfSrcBox * rfDstPoint * rfSameRes * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame % rfFormatDestSame ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex2DArr * rfWidth * rfHeight * rfSrcBox * rfDstPoint * rfSameRes * rfArraySizeMed * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame % rfFormatDestSame ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex3D    * rfWidth * rfHeight * rfDepth4 * rfSrcBoxWithDepth * rfDstPointWithDepth * rfSameRes * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame % rfFormatDestSame ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTexCube  * rfWidth * rfSrcBox * rfDstPoint * rfSameRes * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame % rfFormatDestSame ) * ( rfSrcSubresFew * rfDestSubresFew ) * rfResTex1DArr * rfWidth32 * rfSrcBoxDef * rfDstPoint * rfSameRes * rfArraySizeMed * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame % rfFormatDestSame ) * ( rfSrcSubresFew * rfDestSubresFew ) * rfResTex2DArr * rfWidth32 * rfHeight32 * rfSrcBox * rfDstPoint * rfSameRes * rfArraySizeMed * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame % rfFormatDestSame ) * ( rfSrcSubresFew * rfDestSubresFew ) * rfResTexCube  * rfWidth32 * rfSrcBox * rfDstPoint * rfSameRes * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame_RGBA8 % rfFormatDestSame_RGBA8 ) * ( rfSrcSubresCube * rfDestSubresCube)* rfResTexCube  * rfWidth32 * rfSrcBox * rfDstPoint * rfSameRes * rfDoNotWaitF ) +

		( rfCopyRegion * ( rfFormatSrcSame_RGBA8 % rfFormatDestSame_RGBA8 ) * ( rfSrcSubresFew * rfDestSubresFew ) * rfResTex1DArr * rfWidth32 * rfSrcBoxDef * rfDstPoint * rfSameRes * rfArraySizeMed * rfDoNotWaitF * rfDiscard ) +
		( rfCopyRegion * ( rfFormatSrcSame_RGBA8 % rfFormatDestSame_RGBA8 ) * ( rfSrcSubresFew * rfDestSubresFew ) * rfResTex2DArr * rfWidth32 * rfHeight32 * rfSrcBox * rfDstPoint * rfSameRes * rfArraySizeMed * rfDoNotWaitF * rfDiscard ) +
		( rfCopyRegion * ( rfFormatSrcSame_RGBA8 % rfFormatDestSame_RGBA8 ) * ( rfSrcSubresFew * rfDestSubresFew ) * rfResTexCube  * rfWidth32 * rfSrcBox * rfDstPoint * rfSameRes * rfDoNotWaitF * rfDiscard ) +
		( rfCopyRegion * ( rfFormatSrcSame_RGBA8 % rfFormatDestSame_RGBA8 ) * ( rfSrcSubresCube * rfDestSubresCube)* rfResTexCube  * rfWidth32 * rfSrcBox * rfDstPoint * rfSameRes * rfDoNotWaitF * rfDiscard ) +

		( rfCopyRegion * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex2D    * rfWidth32 * rfHeight32 * rfSrcBox * rfDstPoint * rfSameRes * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex2DArr * rfWidth32 * rfHeight32 * rfSrcBox * rfDstPoint * rfSameRes * rfArraySizeMed * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex3D    * rfWidth32 * rfHeight32 * rfDepth4 * rfSrcBoxWithDepth * rfDstPointWithDepth * rfSameRes * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTexCube  * rfWidth32 * rfSrcBox * rfDstPoint * rfSameRes * rfDoNotWaitF ) +
		( rfCopyRegion * ( rfFormatSrcSame_Tileable % rfFormatDestSame_Tileable ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex2D    * rfWidth * rfHeight * rfSrcBoxNonZero * rfDstPoint * rfSameResFalse * rfBindRT * rfTileable ) +
		( rfCopyRegion * ( rfFormatSrcSame_Tileable % rfFormatDestSame_Tileable ) * ( rfSrcSubres0 % rfDestSubres0 ) * rfResTex2DArr * rfWidth * rfHeight * rfSrcBoxNonZero * rfDstPoint * rfSameResFalse * rfArraySize * rfBindRT * rfTileable ) +

		rfUpdateNotNullSrcBox +
		rfUpdateNullSrcBox +
		( rfMap * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResBuff     * rfWidth * rfMapType * rfDoNotWait * rfMapUsage ) +
		( rfMap * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTex1D    * rfWidth * rfMapType * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTex1DArr * rfWidth * rfMapType * rfArraySize * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTex2D    * rfWidth * rfHeight * rfMapType * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTex2DArr * rfWidth * rfHeight * rfMapType * rfArraySizeMed * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTex3D    * rfWidth * rfHeight * rfDepth4 * rfMapType * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResTexCube  * rfWidth * rfMapType * rfDoNotWait * rfMapUsageStaging ) +

		( rfMap * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfSrcSubres0 * rfResTex2D    * rfWidth32 * rfHeight32 * rfMapType * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfSrcSubres0 * rfResTex2DArr * rfWidth32 * rfHeight32 * rfMapType * rfArraySizeMed * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfSrcSubres0 * rfResTex3D    * rfWidth32 * rfHeight32 * rfDepth4 * rfMapType * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame_BC6and7 % rfFormatDestSame_BC6and7 ) * rfSrcSubres0 * rfResTexCube  * rfWidth32 * rfMapType * rfDoNotWait * rfMapUsageStaging) +

		( rfMap * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubresFew * rfResTex1DArr * rfWidth32 * rfMapType * rfArraySizeMed * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubresFew * rfResTex2DArr * rfWidth32 * rfHeight32 * rfMapType * rfArraySizeMed * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubresFew * rfResTexCube  * rfWidth32 * rfMapType * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame_RGBA8 % rfFormatDestSame_RGBA8 ) * rfSrcSubresCube* rfResTexCube  * rfWidth32 * rfMapType * rfDoNotWait * rfMapUsageStaging ) +
		( rfMap * ( rfFormatSrcSame % rfFormatDestSame ) * rfSrcSubres0 * rfResBuff     * rfWidth * rfDynamicBufMapType * rfDoNotWaitF * rfMapUsage ) +

		rfTextureReadbackTestCases

		);
	
	AddPriorityPatternFilter(FilterEqual<D3D_RESOURCE_SCENARIO>(pResParam, D3D_RESOURCE_SCENARIO_BUFFER), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<D3D_RESOURCE_SCENARIO>(pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D), WeightFactorVeryLarge);

	AddPriorityPatternFilter(FilterEqual<UINT32>(pWidthParam, 32), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT32>(pHeightParam, 32), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(pNullSrcBoxParam, true), WeightFactorLarge);
	
	// let's make some tileable and discard cases important.  the first 3 invert colors in the pixel shader 
	CFilterHandle filterTileableFlag = FilterEqual<TEST_COPY_FLAGS>( _T("CopyFlags"), D3D11_COPY_TILEABLE );
	CFilterHandle filterDiscardFlag = FilterEqual<TEST_COPY_FLAGS>( _T("CopyFlags"), D3D11_COPY_DISCARD_TEST );
	AddPriorityPatternFilter( (filterTileableFlag || filterDiscardFlag) && FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter( (filterTileableFlag || filterDiscardFlag) && FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_R8G8_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter( (filterTileableFlag || filterDiscardFlag) && FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_B8G8R8A8_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter( (filterTileableFlag || filterDiscardFlag) && FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_R32G32B32A32_FLOAT), WeightFactorVeryLarge);

	// let's make some tileable cases less important to save some time
	AddPriorityPatternFilter( (filterTileableFlag || filterDiscardFlag) && 
		( FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_R16G16_SNORM) ||
		  FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_A8_UNORM) ||
		  FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_B5G6R5_UNORM) ||
		  FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_B8G8R8X8_UNORM) ||
		  FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) ||
		  FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_R16G16B16A16_SNORM) ||
		  FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_R16_FLOAT) ||
		  FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) ||
		  FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_R32G32_FLOAT) ||
		  FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_R11G11B10_FLOAT) ||
		  FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_R16G16_UNORM) ||
		  FilterEqual<DXGI_FORMAT>( _T("DestFormatSame"), DXGI_FORMAT_R32G32_FLOAT) ), WeightFactorSmall);

	AddPriorityPatternFilter( FilterEqual<D3D11_MAP>( _T("MapType"), D3D11_MAP_WRITE_NO_OVERWRITE), WeightFactorLarge);

	// Lets make constant buffer cases important
	AddPriorityPatternFilter( FilterEqual<D3D11_BIND_FLAG>( _T("BindFlags"), D3D11_BIND_CONSTANT_BUFFER), WeightFactorVeryLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

TEST_RESULT CResourceAccessTest::Setup()
{
	TEST_RESULT tr = CD3D11Test::Setup( );
	if( tr != RESULT_PASS )
		return tr;

	HRESULT hr;
	ZeroMemory( &m_wddmDriverVersion, sizeof(m_wddmDriverVersion) );

	for( UINT i = 0; i < sizeof( m_pTagData ); i++ )
	{
		m_pTagData[i] = (BYTE)(i & 0xFF);
	}

	
	//
	// Get the AdapterDescriptor and WDDM driver version
	//

	// Get the Adapter Descriptor
	if( m_pAdapterDescriptor )
	{
		delete m_pAdapterDescriptor;
		m_pAdapterDescriptor = nullptr;
	}

	m_pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor( GetDevice() );
	if( nullptr == m_pAdapterDescriptor )
	{
		LogError( __FILEW__, __LINE__, L"GetAdapterDescriptor returned null.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported );
		tr = RESULT_FAIL;
		goto Cleanup;
	}

	// Get the WDDM version
	if ( GetFramework()->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_REFERENCE )
	{
		// Ref doesn't support the GetDisplayDriverModel() call
		m_wddmDriverVersion = KMT_DRIVERVERSION_WDDM_1_2;
	}
	else
	{
		if( FAILED( hr = m_pAdapterDescriptor->GetDisplayDriverModel( m_wddmDriverVersion ) ) )
		{
			LogError( __FILEW__, __LINE__, L"GetDisplayDriverModel failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported );
			tr = RESULT_FAIL;
			goto Cleanup;
		}
	}

Cleanup:
	return tr;
}

TEST_RESULT CResourceAccessTest::SetupTestCase()
{
	HRESULT hr;

	bool bIsArrayScenario = false;
	switch( m_ResourceScenario )
	{
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		{
			bIsArrayScenario = true;
			break;
		}
	}

	if( m_AccessMethod == AM_COPYREGION)
	{
		CFilterHandle skipAccessMethod = FilterEqual( _T("AccessMethod"), AM_COPYREGION );

		if( m_bSameResource && m_uSrcSubres == m_uDestSubres )
		{
			// Overlapping copies are required to be supported for all D3D10+ hardware with D3D11.1 drivers
			D3D11_FEATURE_DATA_D3D11_OPTIONS dataD3D11Options;
			ZeroMemory( &dataD3D11Options, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS) );
			if( FAILED( hr = GetDevice()->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS, &dataD3D11Options, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS) ) ) )
			{
				WriteToLog( _T("CheckFeatureSupport() for D3D11_FEATURE_D3D11_OPTIONS failed. HR = %s"), D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
			
			// Make sure we are running on an 11.1 driver that supports overlapping copies
			if( !dataD3D11Options.CopyWithOverlap )
			{
				WriteToLog(_T("The driver has no support for copyregion on same subresource (Overlapping copy).") );
				SkipMatching( GetSkipMatchingFilter() || (skipAccessMethod && FilterEqual( _T("SameResource"), m_bSameResource ) && FilterEqual( _T("SrcSubres"), m_uSrcSubres ) && FilterEqual( _T("DestSubres"), m_uDestSubres )) );
				return RESULT_SKIP;
			}
		}

		if ( (m_SrcBox.bottom + m_DestPoint.y) > 1 
			&& (m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D || m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY || m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER)
			)
		{
			WriteToLog(_T("No support for this box & offset on resource scenario.") );
			return RESULT_SKIP;
		}
		if ( m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURE3D && (m_SrcBox.back + m_DestPoint.z) > 1 )
		{
			WriteToLog(_T("No support for this box & offset on resource scenario.") );
			return RESULT_SKIP;
		}
		if ( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D && (m_SrcBox.back + m_DestPoint.z) > m_uDepth )
		{
			WriteToLog(_T("No support for this box & offset & depth.") );
			return RESULT_SKIP;
		}
	}

	if( m_AccessMethod == AM_UPDATE )
	{
		if( !m_bNULLSrcBox )
		{
			if ( (m_SrcBox.bottom) > 1 
				&& (m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D || m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY || m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER)
				)
			{
				WriteToLog(_T("No support for this box & offset on resource scenario.") );
				return RESULT_SKIP;
			}

			if ( (m_SrcBox.back) > 1 
				&& (m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURE3D)
				)
			{
				WriteToLog(_T("No support for this box & offset on resource scenario.") );
				return RESULT_SKIP;
			}

			// Partial constant buffer updates only supported for feature level 9 or drivers that support 11.1 DDI's
			if ( !m_bNULLSrcBox
				&& (m_BindFlags & D3D11_BIND_CONSTANT_BUFFER)
				&& GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_0
				&& !GetFramework()->Is11_1DDISupported( GetDevice() )
				)
			{
				WriteToLog(_T("Support for partial constant buffer updates requires feature level 9 or 11.1 DDI support.") );
				return RESULT_SKIP;
			}
		}
	}

	if( m_AccessMethod != AM_COPY)
	{
		if (D3D_RESOURCE_SCENARIO_TEXTURECUBE == m_ResourceScenario)
		{
			if ( m_uSrcSubres >= 6 || m_uDestSubres >= 6 )
			{
				WriteToLog(_T("srcsubres or destsubres >= numsubres.") );
				return RESULT_SKIP;
			}
		}
		else if ( m_uSrcSubres >= m_uArraySize || m_uDestSubres >= m_uArraySize )
		{
			WriteToLog(_T("srcsubres or destsubres >= arraysize.") );
			return RESULT_SKIP;
		}
	}

	if( m_AccessMethod == AM_MAP )
	{
		// Cannot read from resources in a deferred context
		if ( GetFramework()->IsDeferredContextUsed() && ( m_MapType == D3D11_MAP_READ || m_MapType == D3D11_MAP_READ_WRITE || m_ResourceScenario != D3D_RESOURCE_SCENARIO_BUFFER ) )
		{
			return RESULT_SKIP;
		}

		// DISCARD and NO_OVERWRITE are only supported on the 11.1 DDI for buffer SRVs
		if( ( m_MapType == D3D11_MAP_WRITE_DISCARD || m_MapType == D3D11_MAP_WRITE_NO_OVERWRITE ) && !GetFramework()->Is11_1DDISupported( GetDevice() ) )
		{
			// DISCARD and NO_OVERWRITE are only used in this test for DYNAMIC buffer SRVs (with the DYNAMIC and SRV flags added in TestMap)
			assert( m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER );

			WriteToLog(_T("DYNAMIC Map DISCARD or NO_OVERWRITE requires DDI 11.1 support.") );
			CFilterHandle skipMap = FilterEqual( _T("AccessMethod"), m_AccessMethod );
			CFilterHandle skipMapType = FilterEqual( _T("MapType"), D3D11_MAP_WRITE_DISCARD ) || FilterEqual( _T("MapType"), D3D11_MAP_WRITE_DISCARD );
			SkipMatching( GetSkipMatchingFilter() || ( skipMap && skipMapType ) );
			return RESULT_SKIP;
		}

		// DO_NOT_WAIT not supported on deferred context
		if( GetFramework()->IsDeferredContextUsed() && m_bDoNotWait )
		{
			return RESULT_SKIP;
		}
	}

	if( m_uWidth > D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT && 
		!GetFramework()->Is11_1DDISupported( GetDevice() ) && 
		!(GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0) )
	{
		// constant buffers larger than 4096 constants can't be created
		// if the driver isn't wddm1.2
		return RESULT_SKIP;
	}

	bool bAMIsUpdate = ( m_AccessMethod == AM_UPDATE );
	bool bSameFormat = GetTestCaseParameter( _T( "SrcFormatSame" ) )->IsActive( );
	bool bCreateTwo = m_AccessMethod == AM_COPY || ( m_AccessMethod == AM_COPYREGION && !m_bSameResource );
	DXGI_FORMAT srcFormat = bSameFormat ? m_SrcFormatSame : m_SrcFormat;
	DXGI_FORMAT destFormat = bSameFormat ? m_SrcFormatSame : m_DestFormat;

	UINT uSupportSrc = 0;
	UINT uSupportDest = 0;
	if( D3D11_BIND_CONSTANT_BUFFER != m_BindFlags )
	{
		hr = GetDevice()->CheckFormatSupport( srcFormat, &uSupportSrc );
		if (FAILED(hr))
		{
			WriteToLog(_T("CheckFormatSupport failed on srcFormat (%s). hr=%s\n"), ToString(srcFormat).c_str(), D3DHResultToString(hr).c_str() );
			CFilterHandle skipFormat = bSameFormat ? FilterEqual( _T("SrcFormatSame"), srcFormat ) : FilterEqual( _T("SrcFormat"), srcFormat );
			SkipMatching( GetSkipMatchingFilter() || skipFormat );
			return RESULT_SKIP;
		}
		hr = GetDevice()->CheckFormatSupport( destFormat, &uSupportDest );
		if (FAILED(hr))
		{
			WriteToLog(_T("CheckFormatSupport failed on destFormat (%s). hr=%s\n"), ToString(destFormat).c_str(), D3DHResultToString(hr).c_str() );
			CFilterHandle skipFormat = bSameFormat ? FilterEqual( _T("DestFormatSame"), destFormat ) : FilterEqual( _T("DestFormat"), destFormat );
			SkipMatching( GetSkipMatchingFilter() || skipFormat );
			return RESULT_SKIP;
		}
	}

	// Check to see if we need to skip based on the depth stencil and forma
	if( bAMIsUpdate && m_bDepthResource && !( uSupportSrc & D3D10_FORMAT_SUPPORT_DEPTH_STENCIL ) )
		return RESULT_SKIP;

	// BUG: 375391 - D3D11: Whole subresource update for depth test cases failing
	if( bAMIsUpdate && m_bDepthResource )
		return RESULT_SKIP;

	std::map< D3D_RESOURCE_SCENARIO, D3D11_FORMAT_SUPPORT > resTypes;
	resTypes[ D3D_RESOURCE_SCENARIO_BUFFER ] = D3D11_FORMAT_SUPPORT_BUFFER;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE1D ] = D3D11_FORMAT_SUPPORT_TEXTURE1D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY ] = D3D11_FORMAT_SUPPORT_TEXTURE1D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE3D ] = D3D11_FORMAT_SUPPORT_TEXTURE3D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURECUBE ] = D3D11_FORMAT_SUPPORT_TEXTURECUBE;

	if( D3D11_BIND_CONSTANT_BUFFER != m_BindFlags )
	{
		if( (uSupportSrc & resTypes[ m_ResourceScenario ]) == 0 )
		{
			WriteToLog(_T("No support for resource scenario in srcFormat.") );
			CFilterHandle skipFormat = bSameFormat ? FilterEqual( _T("SrcFormatSame"), srcFormat ) : FilterEqual( _T("SrcFormat"), srcFormat );
			SkipMatching( GetSkipMatchingFilter() || (skipFormat && FilterEqual( _T("ResourceScenario"), m_ResourceScenario )) );
			return RESULT_SKIP;
		}

		if( bCreateTwo && (uSupportDest & resTypes[ m_ResourceScenario ]) == 0 )
		{
			WriteToLog(_T("No support for resource scenario in destFormat.") );
			CFilterHandle skipFormat = bSameFormat ? FilterEqual( _T("DestFormatSame"), destFormat ) : FilterEqual( _T("DestFormat"), destFormat );
			SkipMatching( GetSkipMatchingFilter() || (skipFormat && FilterEqual( _T("ResourceScenario"), m_ResourceScenario )) );
			return RESULT_SKIP;
		}
	}

	if( m_CopyFlags & D3D11_COPY_TILEABLE )
	{
		// This test uses FilterEqual( _T("CopyFlags"), D3D11_COPY_TILEABLE ) to skip test cases below 
		assert( m_CopyFlags == D3D11_COPY_TILEABLE );

		CFilterHandle skipCopyFlags = FilterEqual( _T("CopyFlags"), D3D11_COPY_TILEABLE );
		if( (uSupportSrc & D3D11_FORMAT_SUPPORT_RENDER_TARGET) == 0 )
		{
			WriteToLog(_T("No support for render targets in srcFormat.") );
			CFilterHandle skipFormat = FilterEqual( _T("SrcFormatSame"), srcFormat );
			SkipMatching( GetSkipMatchingFilter() || ( skipCopyFlags && skipFormat ) );
			return RESULT_SKIP;
		}
		if( (uSupportDest & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE) == 0 )
		{
			WriteToLog(_T("No support for shader resource views in destFormat.") );
			CFilterHandle skipFormat = FilterEqual( _T("DestFormatSame"), destFormat );
			SkipMatching( GetSkipMatchingFilter() || ( skipCopyFlags && skipFormat ) );
			return RESULT_SKIP;
		}
	}

	const bool bD3D9 = GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0;
	if( bD3D9 && m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY )
	{
		WriteToLog(_T("D3D10Level9 does not support texture arrays.") );
		CFilterHandle skipScenario = FilterEqual( _T("ResourceScenario"), m_ResourceScenario );
		SkipMatching( GetSkipMatchingFilter() || ( skipScenario ) );
		return RESULT_SKIP;
	}
	else if( m_bTextureReadback )
	{
		// don't skip texture readback, unless Texture3D with no pow 2 on FL9
	}
	else if( bD3D9 && (m_BindFlags & D3D11_BIND_CONSTANT_BUFFER) )
	{
		// don't skip these
	}
	else if( bD3D9 && TestOverlappingCopy() )
	{
		// don't skip overlapping copy on D3D9, except for Texture3D cases
		// for these cases we force SRV and RTV bind flags, however Texture3D doesn't work with these
		// Texture3D cases will skip
		if( (uSupportSrc & D3D11_FORMAT_SUPPORT_RENDER_TARGET) == 0 )
		{
			WriteToLog(_T("No support for render targets in srcFormat.") );
			return RESULT_SKIP;
		}
		if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		{
			WriteToLog( _T("D3D9 Texture3D's don't allow the render target bind flag, but fail to copy with just the SRV bind flag.") );
			return RESULT_SKIP;
		}
	}
	else if( bD3D9 && TestSwapbuffer() )
	{
		// Don't skip feature level 9 Swapbuffer cases
	}
	else if( bD3D9 && ((m_CopyFlags & (D3D11_COPY_TILEABLE | D3D11_COPY_DISCARD | D3D11_COPY_NO_OVERWRITE)) == 0) )
	{
		WriteToLog(_T("D3D10Level9 is not necessarily supported unless CopyFlags == D3D11_COPY_TILEABLE, D3D11_COPY_DISCARD, or D3D11_COPY_NO_OVERWRITE.") );
		return RESULT_SKIP;
	}

	if( bD3D9 && m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
	{
		// Texturecubes in d3d9 must be a power of 2
		bool isPowerOf2 = (m_uWidth > 0) && ((m_uWidth & (m_uWidth-1)) == 0);
		if( !isPowerOf2 )
		{
			WriteToLog( _T("D3D9 TextureCubes are required to be a power of 2.") );
			return RESULT_SKIP;
		}
	}

	// Texture3Ds in d3d9 must be a power of 2
	if( bD3D9 && m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D && (
		((m_uWidth > 0) && ((m_uWidth & (m_uWidth - 1)) != 0)) ||		// Width is a not a power of 2
		((m_uHeight > 0) && ((m_uHeight & (m_uHeight - 1)) != 0)) ||	// Height is a not a power of 2
		((m_uDepth > 0) && ((m_uDepth & (m_uDepth - 1)) != 0)) ))		// Depth is a not a power of 2
	{
		WriteToLog( _T("D3D9 Texture3D's don't allow non power of 2 dimensions.") );
		return RESULT_SKIP;
	}

	m_bBC = false;
	switch( srcFormat )
	{  
	case DXGI_FORMAT_BC1_TYPELESS:      
	case DXGI_FORMAT_BC1_UNORM:      
	case DXGI_FORMAT_BC1_UNORM_SRGB: 
	case DXGI_FORMAT_BC2_TYPELESS:      
	case DXGI_FORMAT_BC2_UNORM:      
	case DXGI_FORMAT_BC2_UNORM_SRGB: 
	case DXGI_FORMAT_BC3_TYPELESS:      
	case DXGI_FORMAT_BC3_UNORM:      
	case DXGI_FORMAT_BC3_UNORM_SRGB: 
	case DXGI_FORMAT_BC4_TYPELESS:      
	case DXGI_FORMAT_BC4_UNORM:      
	case DXGI_FORMAT_BC4_SNORM:      
	case DXGI_FORMAT_BC5_TYPELESS:      
	case DXGI_FORMAT_BC5_UNORM:      
	case DXGI_FORMAT_BC5_SNORM:     
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		m_bBC = true;
	}

	if( m_bBC )
	{
		CFilterHandle skipFormat = bSameFormat ? FilterEqual( _T("SrcFormatSame"), srcFormat ) : FilterEqual( _T("SrcFormat"), srcFormat );
		if( m_uWidth % 4 )
		{
			WriteToLog(_T("No support for BC & width.") );
			SkipMatching( GetSkipMatchingFilter() || (skipFormat && FilterEqual( _T("Width"), m_uWidth )) );
			return RESULT_SKIP;
		}

		if( m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURECUBE && m_uHeight % 4 )
		{
			WriteToLog(_T("No support for BC & resource scenario & height.") );
			SkipMatching( GetSkipMatchingFilter() || (skipFormat && FilterEqual( _T("Height"), m_uHeight ) && FilterEqual( _T("ResourceScenario"), m_ResourceScenario )) );
			return RESULT_SKIP;
		}

		if( m_SrcBox == m_pBoxes[ 1 ] )
		{
			WriteToLog(_T("No support for BC & srcbox.") );
			return RESULT_SKIP;
		}
	}
	
	// Note: We could remove this if check to ensure that all test cases are checked for the alignment, however we would need to ensure we are not skipping any additional cases once
	// this is removed.
	if( CD3D10FormatHelper_Test::YUV( srcFormat ) ||
		srcFormat == DXGI_FORMAT_R8G8_B8G8_UNORM ||
		srcFormat == DXGI_FORMAT_G8R8_G8B8_UNORM )
	{
		// When created as a texture array and when D3D11_BIND_DECODER is not set, the maxium texture array size is based on the format
		if( bIsArrayScenario &&								// Texture Array
			((m_BindFlags & D3D11_BIND_DECODER) == 0 ) )	// D3D11_BIND_DECODER is not set
		{
			const UINT maxArraySize = CD3D10FormatHelper_Test::Planar( srcFormat ) ? 2 : UINT_MAX;
			if( m_uArraySize > maxArraySize )
			{
				WriteToLog( _T("%s only supports a max array size of %u. Skipping Test."), ToString(srcFormat).c_str(), maxArraySize );
				return RESULT_SKIP;
			}
		}

		// Video formats need to check that both the box and the actual resource are aligned
		const UINT horizontalAlignment = CD3D10FormatHelper_Test::GetWidthAlignment( srcFormat );
		const UINT verticalAlignment = CD3D10FormatHelper_Test::GetHeightAlignment( srcFormat );

		// Check the box alignment and resource alignment
		switch( m_ResourceScenario )
		{
		default:
			WriteToLog( _T("Invalid m_ResourceScenario = %s (%u)."), ToString(m_ResourceScenario).c_str(), m_ResourceScenario );
			return RESULT_FAIL;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE3D:
			// No box depth alignment checks
			// fall-through
		case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
			if( !m_bNULLSrcBox &&
				(m_SrcBox.bottom % verticalAlignment) != 0 )
			{
				WriteToLog( _T("The box's bottom (%u) doesn't match the format's (%s) vertical alignment (%u)"), m_SrcBox.bottom, ToString(srcFormat).c_str(), verticalAlignment );
				return RESULT_SKIP;
			}
			if( !m_bNULLSrcBox &&
				(m_SrcBox.top % verticalAlignment) != 0 )
			{
				WriteToLog( _T("The box's top (%u) doesn't match the format's (%s) vertical alignment (%u)"), m_SrcBox.top, ToString(srcFormat).c_str(), verticalAlignment );
				return RESULT_SKIP;
			}
			if( (m_uHeight % verticalAlignment) != 0 )
			{
				WriteToLog( _T("The resource's height (%u) doesn't match the format's (%s) vertical alignment (%u)"), m_uHeight, ToString(srcFormat).c_str(), verticalAlignment );
				return RESULT_SKIP;
			}
			// fall-through
		case D3D_RESOURCE_SCENARIO_BUFFER:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
			if( !m_bNULLSrcBox &&
				(m_SrcBox.right % horizontalAlignment) != 0 )
			{
				WriteToLog( _T("The box's right (%u) doesn't match the format's (%s) horizontal alignment (%u)"), m_SrcBox.right, ToString(srcFormat).c_str(), horizontalAlignment );
				return RESULT_SKIP;
			}
			if( !m_bNULLSrcBox &&
				(m_SrcBox.left % horizontalAlignment) != 0 )
			{
				WriteToLog( _T("The box's left (%u) doesn't match the format's (%s) horizontal alignment (%u)"), m_SrcBox.left, ToString(srcFormat).c_str(), horizontalAlignment );
				return RESULT_SKIP;
			}
			if( (m_uWidth % horizontalAlignment) != 0 )
			{
				WriteToLog( _T("The resource's width (%u) doesn't match the format's (%s) horizontal alignment (%u)"), m_uWidth, ToString(srcFormat).c_str(), horizontalAlignment );
				return RESULT_SKIP;
			}
			break;
		}
	}

	TEST_RESULT tr = CreateResources();

	if( tr != RESULT_PASS )
	{
		return tr;
	}

	// Note that D3D11_QUERY_PIPELINE_STATISTICS is not supported in 10Level9
	D3D11_QUERY_DESC descQuery;
	descQuery.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	descQuery.MiscFlags = 0;
	if( !bD3D9 && FAILED( GetDevice()->CreateQuery( &descQuery, &m_pQuery ) ) )
	{
		WriteToLog( _T( "CResourceAccessTest::SetupTestCase() - Failed to create pipeline statistics query" ) );
		return RESULT_FAIL;
	}
		
	return RESULT_PASS;
}

bool CResourceAccessTest::TestTileableCopy() const
{
	return ( m_CopyFlags & D3D11_COPY_TILEABLE ) && ( m_BindFlags & D3D11_BIND_RENDER_TARGET ) && !m_bSameResource &&
		   m_SrcBox.Width() > 0 && m_SrcBox.Height() > 0;
}

bool CResourceAccessTest::TestOverlappingCopy() const
{
	return m_bSameResource && m_uSrcSubres == m_uDestSubres;
}

bool CResourceAccessTest::TestSwapbuffer() const
{
	// Driver must be WDDM1.3 or higher
	// Format must be A8_UNORM, R8_UNORM, or R8G8_UNORM
	bool bWDDM1_3OrHigher = false;
	bool bCorrectFormat = false;

	// Check the WDDM level
	if( m_wddmDriverVersion >= KMT_DRIVERVERSION_WDDM_1_3 )
	{
		bWDDM1_3OrHigher = true;
	}

	// Check the current format
	switch( m_SrcFormatSame )
	{
	case DXGI_FORMAT_A8_UNORM:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8G8_UNORM:
		bCorrectFormat = true;
		break;
	}
	
	return (bWDDM1_3OrHigher && bCorrectFormat);
}

bool CResourceAccessTest::EightBitUNORMFormat() const
{
	switch( m_SrcFormatSame )
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return true;
	default:
		return false;
	}
}

TEST_RESULT CResourceAccessTest::CreateResources()
{
	HRESULT hr;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_TEXTURE1D_DESC tex1DDesc;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	D3D11_TEXTURE3D_DESC tex3DDesc;
	D3D11_SUBRESOURCE_DATA subResData[ MAX_ARRAYSIZE ];

	bool bAMIsUpdate = ( m_AccessMethod == AM_UPDATE );
	bool bSameFormat = GetTestCaseParameter( _T( "SrcFormatSame" ) )->IsActive( );
	bool bCreateTwo = m_AccessMethod == AM_COPY || ( m_AccessMethod == AM_COPYREGION && !m_bSameResource );
	DXGI_FORMAT srcFormat = bSameFormat ? m_SrcFormatSame : m_SrcFormat;
	DXGI_FORMAT destFormat = bSameFormat ? m_SrcFormatSame : m_DestFormat;
	bool bTestTileable = TestTileableCopy();
	const bool bD3D9 = GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0;
	const bool bTestOverLappingCopyD3D9 = TestOverlappingCopy() && bD3D9;
	const bool bTestSwapbufferD3D9 = TestSwapbuffer() && bD3D9;
	const bool bNeedsDest = m_AccessMethod == AM_COPY || m_AccessMethod == AM_COPYREGION;

	UINT scenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &scenFlags );
	bool bOneDim = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER || (scenFlags & D3D_RS_FLAGS_TEXTURE1D);
	bool bCube = (scenFlags & D3D_RS_FLAGS_TEXTURECUBE) != 0;
	bool bArray = (scenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	bool bMS = (scenFlags & D3D_RS_FLAGS_MS) != 0;
	UINT uElementWidth = max( GetBitsPerElement( srcFormat ) / 8, 1 );
	UINT height = bCube ? m_uWidth : m_uHeight;
	
	UINT iA = 0;

	TexDataInfo texDataInfo;
	texDataInfo.Format = srcFormat;

	UINT uBlockPitch = 0;
	switch( srcFormat )
	{
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_SNORM:
	case DXGI_FORMAT_BC4_UNORM:
		uBlockPitch = 8;
		break;
	default:
		uBlockPitch = 16;
		break;
	}

	UINT rowPitch = m_bBC ? uBlockPitch * m_uWidth / 4 : m_uWidth * uElementWidth;
	const UINT texHeight = bOneDim ? 1 : height;
	size_t dataSize = rowPitch * texHeight;

	for( UINT i = 0; i < (bCube ? 6 : (UINT) RA_DATA_ELEMENTS); ++i )
	{
		UINT colorSet = i % NUM_COLOR_SETS;

		// if we're testing BC textures use texture caching
		if( m_bBC )
		{
			// populate texture cache entry info
			texDataInfo.Width = m_uWidth;
			texDataInfo.Height = texHeight;
			texDataInfo.ColorSet = colorSet;

			// if cache entry doesn't exist, create one along with the resource data
			if( m_TexCache.find(texDataInfo) == m_TexCache.end() )
			{
				m_TexCache[texDataInfo] = new BYTE[dataSize];
				memset(m_TexCache[texDataInfo], 0, dataSize);

				if( !CFormatHelper::CreateGradient( srcFormat, texHeight, m_uWidth, m_ColorSets[ colorSet ], m_TexCache[texDataInfo], rowPitch ) )
				{
					WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateGradient() failed when called as part of new texture cache entry." ) );
					return RESULT_FAIL;
				}
			}

			// copy test resource data from either a newly created or existing cache entry
			memcpy(m_pResourceData[ i ], m_TexCache[texDataInfo], dataSize);
		}
		else
		{
			if( !CFormatHelper::CreateGradient( srcFormat, texHeight, m_uWidth, m_ColorSets[ colorSet ], m_pResourceData[i], rowPitch ) )
			{
				WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateGradient() failed" ) );
				return RESULT_FAIL;
			}
		}
	}

	const bool bTestCaseProvidesCPUAccessFlags = m_bTextureReadback;	// Should we determine the CPU Access flags to use, or will the test case provide them?
	D3D11_USAGE usageFlags = ((bCreateTwo && !bTestTileable && !bTestOverLappingCopyD3D9 && !bTestSwapbufferD3D9) || m_bTextureReadback) ? D3D11_USAGE_STAGING : D3D11_USAGE_DEFAULT;
	D3D11_USAGE usageFlagsDest = (bTestTileable || bTestOverLappingCopyD3D9 || m_bTextureReadback) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_STAGING;
	UINT cpuAccessFlags = bCreateTwo && !bTestTileable && !bTestOverLappingCopyD3D9 && !bTestSwapbufferD3D9 ? D3D11_CPU_ACCESS_READ : 0;
	UINT cpuAccessFlagsDest = (bTestTileable || bTestOverLappingCopyD3D9 || m_bTextureReadback) ? 0 : D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	UINT bindFlags = (bAMIsUpdate && m_bDepthResource) ? D3D11_BIND_DEPTH_STENCIL : (D3D11_BIND_FLAG)0;
	UINT bindFlagsDest = (D3D11_BIND_FLAG) 0; 

	if( m_BindFlags & D3D11_BIND_RENDER_TARGET )
	{
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	}

	if( m_BindFlags & D3D11_BIND_CONSTANT_BUFFER )
	{
		bindFlags |= D3D11_BIND_CONSTANT_BUFFER;
	}

	// Texture readback
	if( m_bTextureReadback )
	{
		bindFlagsDest = D3D11_BIND_SHADER_RESOURCE;
	}

	// D3D9 Overlapping copy
	if( bTestOverLappingCopyD3D9 )
	{
		// The debug layer warns that CopyResource FROM a resource with D3D11_BIND_SHADER_RESOURCE does not work unless
		// D3D11_BIND_RENDER_TARGET is also specified for 9.x feature levels
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;
		bindFlags |= D3D11_BIND_RENDER_TARGET;
		bindFlagsDest |= D3D11_BIND_SHADER_RESOURCE;
		bindFlagsDest |= D3D11_BIND_RENDER_TARGET;
	}

	// D3D9 Swapbuffer
	if( bTestSwapbufferD3D9 )
	{
		// Swapbuffer formats will support render-to, texture-from, and copy/update on feature level 9
		if( (usageFlags & D3D11_USAGE_STAGING) == 0 )
		{
			bindFlags |= D3D11_BIND_SHADER_RESOURCE;
			bindFlags |= (m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D) ? 0 : D3D11_BIND_RENDER_TARGET;
		}
	}

	if( bD3D9 && 
		(m_CopyFlags & (D3D11_COPY_DISCARD | D3D11_COPY_NO_OVERWRITE)) &&
		(m_ResourceScenario != D3D_RESOURCE_SCENARIO_BUFFER) &&
		(usageFlags & D3D11_USAGE_STAGING) == 0)
	{
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	if( m_CopyFlags & D3D11_COPY_TILEABLE )
	{
		bindFlagsDest |= D3D11_BIND_SHADER_RESOURCE;
		if( bD3D9 )
		{
			// The debug layer warns that CopyResource FROM a resource with D3D11_BIND_SHADER_RESOURCE does not work unless
			// D3D11_BIND_RENDER_TARGET is also specified for 9.x feature levels
			bindFlagsDest |= D3D11_BIND_RENDER_TARGET;
		}
	}

	if( m_AccessMethod == AM_MAP )
	{
		if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER )
		{
			usageFlags = m_MapUsage;
			cpuAccessFlags = D3D11_CPU_ACCESS_READ;

			if (m_MapUsage == D3D11_USAGE_DEFAULT)
			{
				bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			}
		}

		if( m_MapType == D3D11_MAP_WRITE_DISCARD || m_MapType == D3D11_MAP_WRITE_NO_OVERWRITE )
		{
			// DISCARD and NO_OVERWRITE are only used in this test for DYNAMIC buffer SRVs (we add the DYNAMIC and SRV flags here)
			assert( m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER );
			assert( bindFlagsDest == 0 );

			usageFlagsDest = D3D11_USAGE_DYNAMIC;
			cpuAccessFlagsDest = D3D11_CPU_ACCESS_WRITE;
			bindFlagsDest = D3D11_BIND_SHADER_RESOURCE;
		}
		else if( GetFramework()->IsDeferredContextUsed() )
		{
			usageFlagsDest = D3D11_USAGE_DYNAMIC;
			cpuAccessFlagsDest = D3D11_CPU_ACCESS_WRITE;
			bindFlagsDest = D3D11_BIND_VERTEX_BUFFER;
		}

	}

	switch( m_ResourceScenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		bufferDesc.BindFlags		= bindFlags;
		bufferDesc.ByteWidth		= m_uWidth * uElementWidth;
		bufferDesc.CPUAccessFlags	= bTestCaseProvidesCPUAccessFlags ? m_CPUAccessFlags : cpuAccessFlags;
		bufferDesc.MiscFlags		= 0;
		bufferDesc.Usage			= usageFlags;
		//if( m_BindFlags != 0 )
		//{
		//	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		//	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//}
				
		subResData[ 0 ].pSysMem = m_pResourceData[0];
		subResData[ 0 ].SysMemPitch = m_uWidth * uElementWidth;
		subResData[ 0 ].SysMemSlicePitch = m_uWidth * uElementWidth;

		// See if this is a supported scenario
		if( !IsSupportedBufferScenario(&bufferDesc) )
		{
			WriteToLog( "Not a supported buffer scenario, skipping" );
			return RESULT_SKIP;
		}

		if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufferDesc, subResData, reinterpret_cast<ID3D11Buffer**>(&m_pSrcResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateBuffer() for source resource failed, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufferDesc, subResData, reinterpret_cast<ID3D11Buffer**>(&m_pSrcPreCopyResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateBuffer() for source pre copy resource failed, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}
		
		bufferDesc.BindFlags = bindFlagsDest;
		bufferDesc.Usage = usageFlagsDest;
		bufferDesc.CPUAccessFlags = cpuAccessFlagsDest;
		if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufferDesc, subResData, reinterpret_cast<ID3D11Buffer**>(&m_pDestResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateBuffer() for destination resource failed, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}
		
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		tex1DDesc.ArraySize			= m_uArraySize;
		tex1DDesc.BindFlags			= bindFlags;
		tex1DDesc.CPUAccessFlags	= bTestCaseProvidesCPUAccessFlags ? m_CPUAccessFlags : cpuAccessFlags;
		tex1DDesc.Format			= srcFormat;
		tex1DDesc.MipLevels			= RA_MIPLEVELS;
		tex1DDesc.MiscFlags			= 0;
		tex1DDesc.Usage				= usageFlags;
		tex1DDesc.Width				= m_uWidth;

		for( iA = 0; iA < m_uArraySize; ++iA )
		{
			subResData[ iA ].pSysMem = m_pResourceData[ iA % RA_DATA_ELEMENTS ];
			subResData[ iA ].SysMemPitch = m_uWidth * uElementWidth;
			subResData[ iA ].SysMemSlicePitch = m_uWidth * uElementWidth;
		}

		if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, subResData, reinterpret_cast<ID3D11Texture1D**>(&m_pSrcResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture1D() failed for source resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, subResData, reinterpret_cast<ID3D11Texture1D**>(&m_pSrcPreCopyResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture1D() failed for source pre copy resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		tex1DDesc.BindFlags = bindFlagsDest;
		tex1DDesc.Format = bCreateTwo ? destFormat : srcFormat;
		tex1DDesc.Usage = usageFlagsDest;
		tex1DDesc.CPUAccessFlags = cpuAccessFlagsDest;
		if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, subResData, reinterpret_cast<ID3D11Texture1D**>(&m_pDestResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture1D() failed for destination resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}
	
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		tex2DDesc.ArraySize			= bCube ? 6 : m_uArraySize;
		tex2DDesc.BindFlags			= bindFlags;
		tex2DDesc.CPUAccessFlags	= bTestCaseProvidesCPUAccessFlags ? m_CPUAccessFlags : cpuAccessFlags;
		tex2DDesc.Format			= srcFormat;
		tex2DDesc.Height			= height;
		tex2DDesc.MipLevels			= RA_MIPLEVELS;
		tex2DDesc.MiscFlags			= bCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
		tex2DDesc.SampleDesc.Count	= 1;
		tex2DDesc.SampleDesc.Quality= 0;
		tex2DDesc.Usage				= usageFlags;
		tex2DDesc.Width				= m_uWidth;

		for( iA = 0; iA < (bCube ? 6 : m_uArraySize); ++iA )
		{
			subResData[ iA ].pSysMem = m_pResourceData[ iA % RA_DATA_ELEMENTS ];
			subResData[ iA ].SysMemPitch = m_bBC ? (uBlockPitch * (m_uWidth / 4)) : (m_uWidth * uElementWidth);
			subResData[ iA ].SysMemSlicePitch = subResData[ iA ].SysMemPitch * height;
		}

		if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptional( &tex2DDesc, subResData, reinterpret_cast<ID3D11Texture2D**>(&m_pSrcResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture2D() failed for source resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptional( &tex2DDesc, subResData, reinterpret_cast<ID3D11Texture2D**>(&m_pSrcPreCopyResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture2D() failed for source pre copy resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		tex2DDesc.BindFlags = bindFlagsDest;
		tex2DDesc.Format = bCreateTwo ? destFormat : srcFormat;
		tex2DDesc.Usage = usageFlagsDest;
		tex2DDesc.CPUAccessFlags = cpuAccessFlagsDest;
		if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptional( &tex2DDesc, subResData, reinterpret_cast<ID3D11Texture2D**>(&m_pDestResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture2D() failed for destination resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}
		
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:

		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		tex3DDesc.BindFlags			= bindFlags;
		tex3DDesc.CPUAccessFlags	= bTestCaseProvidesCPUAccessFlags ? m_CPUAccessFlags : cpuAccessFlags;
		tex3DDesc.Depth				= m_uDepth;
		tex3DDesc.Format			= srcFormat;
		tex3DDesc.Height			= m_uHeight;
		tex3DDesc.MipLevels			= RA_MIPLEVELS;
		tex3DDesc.MiscFlags			= 0;
		tex3DDesc.Usage				= usageFlags;
		tex3DDesc.Width				= m_uWidth;

		for( iA = 0; iA < RA_MIPLEVELS; ++iA )
		{
			subResData[ iA ].pSysMem = m_pResourceData;
			subResData[ iA ].SysMemPitch = m_bBC ? (uBlockPitch * (m_uWidth / 4)) : (m_uWidth * uElementWidth);
			subResData[ iA ].SysMemSlicePitch = m_bBC ? (subResData[ iA ].SysMemPitch * (m_uHeight / 4)) : m_uWidth * m_uHeight * uElementWidth ;
		}

		// We won't be able to create a staging copy of the resource to verify the results
		if( bD3D9 && (tex3DDesc.Usage == D3D11_USAGE_DEFAULT) )
		{
			WriteToLog( _T("Texture3D cannot be copied from GPU-accessible memory to CPU-accessible memory.") );
			return RESULT_SKIP;
		}

		if( FAILED( hr = GetDevice()->CreateTexture3D( &tex3DDesc, subResData, reinterpret_cast<ID3D11Texture3D**>(&m_pSrcResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture3D() failed for source resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		if( FAILED( hr = GetDevice()->CreateTexture3D( &tex3DDesc, subResData, reinterpret_cast<ID3D11Texture3D**>(&m_pSrcPreCopyResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture3D() failed for source pre copy resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		tex3DDesc.BindFlags = bindFlagsDest;
		tex3DDesc.Format = bCreateTwo ? destFormat : srcFormat;
		tex3DDesc.Usage = usageFlagsDest;
		tex3DDesc.CPUAccessFlags = cpuAccessFlagsDest;

		// We won't be able to create a staging copy of the resource to verify the results
		if( bD3D9 && bNeedsDest && (tex3DDesc.Usage == D3D11_USAGE_DEFAULT) )
		{
			WriteToLog( _T("Texture3D cannot be copied from GPU-accessible memory to CPU-accessible memory.") );
			return RESULT_SKIP;
		}

		if( FAILED( hr = GetDevice()->CreateTexture3D( &tex3DDesc, subResData, reinterpret_cast<ID3D11Texture3D**>(&m_pDestResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture3D() failed for destination resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		break;
	}

	if( bTestTileable )
	{
		// Create SRV and RTV used for testing D3D11_COPY_TILEABLE
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		srvDesc.Format = destFormat;
		rtvDesc.Format = srcFormat;
		switch( m_ResourceScenario )
		{
		case D3D_RESOURCE_SCENARIO_TEXTURE2D:
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;

			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = 1;
			srvDesc.Texture2DArray.FirstArraySlice = m_uDestSubres;
			srvDesc.Texture2DArray.ArraySize = 1;

			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.MipSlice = 0;
			rtvDesc.Texture2DArray.FirstArraySlice = m_uSrcSubres;
			rtvDesc.Texture2DArray.ArraySize = 1;;
			break;
		case D3D_RESOURCE_SCENARIO_BUFFER:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		case D3D_RESOURCE_SCENARIO_TEXTURE3D:
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - Test factor mismatch. ResourceScenario unexpected." ) );
			return RESULT_FAIL;
		}

		if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pDestResource, &srvDesc, &m_pTilingSRV ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateShaderResourceView() failed, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}
		if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pSrcResource, &rtvDesc, &m_pTilingRTV ) ) )
		{
			// some feature levels or cards may not support RTVs in this format
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateRenderTargetView() failed, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		static const char szShaders[] = 
			"SamplerState samp \n"
			"{ \n"
			"	Filter = MIN_MAG_MIP_POINT; \n"
			"	AddressU = Clamp; \n"
			"	AddressV = Clamp; \n"
			"}; \n"
			"Texture2D tex; \n"
			"Texture2DArray tex2; \n"
			"struct VSIn \n"
			"{ \n"
			"	float2 pos : pos; \n"
			"	float2 tex : texcoord; \n"
			"};  \n"
			"struct VSOut \n"
			"{ \n"
			"	float4 pos : SV_Position; \n"
			"	float2 tex : texcoord; \n"
			"}; \n"
			"VSOut VSMain(VSIn input) \n"
			"{ \n"
			"	VSOut output; \n"
			"	output.pos.xy = input.pos; \n"
			"	output.pos.z = 0; \n"
			"	output.pos.w = 1; \n"
			"	output.tex = input.tex; \n"

			"	return output; \n"
			"} \n"
			"float4 PSMain (VSOut input) : SV_Target \n"
			"{ \n"
			"	return tex.Sample(samp, input.tex); \n"
			"} \n"
			"float4 PSMain_8bit (VSOut input) : SV_Target \n"
			"{ \n"
			"	return 1-tex.Sample(samp, input.tex); \n"
			"} \n"
			"float4 PSMainArray (VSOut input) : SV_Target \n"
			"{ \n"
			"	return tex2.Sample(samp, float3(input.tex,0) ); \n"
			"} \n"
			"float4 PSMainArray_8bit (VSOut input) : SV_Target \n"
			"{ \n"
			"	return 1-tex2.Sample(samp, float3(input.tex,0) ); \n"
			"} \n";
		static const UINT shaderSize = sizeof(szShaders)-1; // minus 1 to remove the '\0'

		// Create VS, IL
		if( m_pTilingIL == NULL )
		{
			m_pTilingVS = NULL;

			ATL::CComPtr<ID3D10Blob> pShaderBlob;
			ATL::CComPtr<ID3D10Blob> pErrorBuf;
			hr = g_TestApp.CompileShaderFromMemory( szShaders, shaderSize, "VSMain", D3D_SHADER_STAGE_VERTEX, D3D_FEATURE_LEVEL_9_1, D3D_FEATURE_LEVEL_9_1, 0, &pShaderBlob, &pErrorBuf );
			if( FAILED( hr ) )
			{
				WriteToLog( "CompileShaderFromMemory() failed with %s", D3DHResultToString(hr).c_str() );
				if( pErrorBuf )
				{
					WriteToLog( "CompileShaderFromMemory() error: %s", pErrorBuf->GetBufferPointer() );
				}
				return RESULT_FAIL;
			}

			if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pTilingVS ) ) )
			{
				WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateVertexShader() failed for tileable copy VS, hr = 0x%x" ), hr );
				return RESULT_FAIL;
			}

			const UINT NumVertexElements = 2;
			D3D11_INPUT_ELEMENT_DESC IADesc[NumVertexElements];
			IADesc[0].InputSlot = 0;
			IADesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			IADesc[0].Format = DXGI_FORMAT_R32G32_FLOAT;
			IADesc[0].SemanticName = "pos";
			IADesc[0].SemanticIndex = 0;
			IADesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			IADesc[0].InstanceDataStepRate = 0;

			IADesc[1].InputSlot = 0;
			IADesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			IADesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
			IADesc[1].SemanticName = "texcoord";
			IADesc[1].SemanticIndex = 0;
			IADesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			IADesc[1].InstanceDataStepRate = 0;

			if( FAILED( hr = GetDevice()->CreateInputLayout( IADesc, NumVertexElements, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pTilingIL ) ) )
			{
				WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateInputLayout() failed for tileable copy, hr = 0x%x" ), hr );
				return RESULT_FAIL;
			}
		}

		// Create PS
		bool bArray = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY;
		bool bInvertibleFormat = EightBitUNORMFormat();
		ATL::CComPtr<ID3D11PixelShader>& pPS = bArray ? ( bInvertibleFormat ? m_pTilingPS_InvertArray : m_pTilingPS_Array ) : ( bInvertibleFormat ? m_pTilingPS_Invert : m_pTilingPS );
		if( pPS == NULL )
		{
			char* pShaderEntrypoint = bArray ? ( bInvertibleFormat ? "PSMainArray_8bit" : "PSMainArray" ) : ( bInvertibleFormat ? "PSMain_8bit" : "PSMain" );
			D3D_FEATURE_LEVEL fl = bArray ? D3D_FEATURE_LEVEL_10_0 : D3D_FEATURE_LEVEL_9_1;

			ATL::CComPtr<ID3D10Blob> pShaderBlob;
			ATL::CComPtr<ID3D10Blob> pErrorBuf;
			hr = g_TestApp.CompileShaderFromMemory( szShaders, shaderSize, pShaderEntrypoint, D3D_SHADER_STAGE_PIXEL, fl, fl, 0, &pShaderBlob, &pErrorBuf );
			if( FAILED( hr ) )
			{
				WriteToLog( "CompileShaderFromMemory() failed %s", pErrorBuf->GetBufferPointer() );
				return RESULT_FAIL;
			}

			if( FAILED( hr = GetDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &pPS ) ) )
			{
				WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreatePixelShader() failed for tileable copy, entrypoint %s, hr = 0x%x" ), pShaderEntrypoint, hr );
				return RESULT_FAIL;
			}
		}

		// Create Sampler
		if( m_pTilingSampler == NULL )
		{
			D3D11_SAMPLER_DESC sampDesc = { D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, 
				0, 1, D3D11_COMPARISON_ALWAYS, { 0.8f, 0.1f, 0.1f, 1.0f }, 0, D3D11_FLOAT32_MAX };
			if( FAILED( hr = GetDevice()->CreateSamplerState( &sampDesc, &m_pTilingSampler ) ) )
			{
				WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateSamplerState() failed for tileable copy, hr = 0x%x" ), hr );
				return RESULT_FAIL;
			}
		}

		// Create VB
		// calculate texture coordinates of the dest box we updated in the SRV
		float dleft = (float)m_DestPoint.x / (float)m_uWidth;
		float dright = (float)( m_DestPoint.x + m_SrcBox.Width() ) / (float)m_uWidth;
		float dtop = (float)m_DestPoint.y / (float)m_uHeight;
		float dbottom = (float)( m_DestPoint.y + m_SrcBox.Height() ) / (float)m_uHeight;
		TilingVertex VBData[4] = { {-1,1,dleft,dtop}, {1,1,dright,dtop}, {-1,-1,dleft,dbottom}, {1,-1,dright,dbottom} };
		D3D11_SUBRESOURCE_DATA bufInit = { VBData, sizeof(VBData), 0 };
		D3D11_BUFFER_DESC bufDesc = { sizeof(VBData), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
		assert( m_pTilingVB == NULL );
		if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, &bufInit, &m_pTilingVB ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateBuffer() failed for tileable copy VB, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		// Create RS
		if( m_pTilingRS == NULL )
		{
			D3D11_RASTERIZER_DESC rastDesc = { D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE, 0, 0, 0, TRUE, FALSE, FALSE, FALSE };
			if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pTilingRS ) ) )
			{
				WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateRasterizerState() failed for tileable copy, hr = 0x%x" ), hr );
				return RESULT_FAIL;
			}
		}

	}

	// Wait for the initial data to be set in the resource before continuing
	if( !BlockingFinish() )
	{
		WriteToLog( _T("CResourceAccessTest::CreateResources() - BlockingFinish() failed.") );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

TEST_RESULT CResourceAccessTest::ValidateResult()
{
	D3D11_MAPPED_SUBRESOURCE p1DSrc, p1DSrcPreCopy, p1DDest;
	D3D11_MAPPED_SUBRESOURCE mapTex2DSrc, mapTex2DSrcPreCopy, mapTex2DDest;
	D3D11_MAPPED_SUBRESOURCE mapTex3DSrc, mapTex3DSrcPreCopy, mapTex3DDest;
	TEST_RESULT tRes = RESULT_PASS;

	UINT scenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &scenFlags );
	bool bCube = (scenFlags & D3D_RS_FLAGS_TEXTURECUBE) != 0;
	bool bArray = (scenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	int numSubresources = RA_MIPLEVELS;
	if( bCube )
		numSubresources = 6 * RA_MIPLEVELS;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER )
		numSubresources = 1;
	else if( bArray )
		numSubresources = m_uArraySize;

	bool bNeedsDest = m_AccessMethod == AM_COPY || m_AccessMethod == AM_COPYREGION;
	bool bNeedsPreCopySrc = m_AccessMethod == AM_COPYREGION && m_bSameResource && m_uSrcSubres == m_uDestSubres;
	ID3D11Resource *pSrcResource = m_pSrcResource;
	if( !bNeedsDest )
	{
		GetImmediateContext()->CopyResource( m_pDestResource, m_pSrcResource );
		pSrcResource = m_pDestResource;
	}
	
	ID3D11Resource* pReadableSrc = NULL;
	ID3D11Resource* pReadableDest = NULL;
	ID3D11Resource* pReadableSrcPreCopy = NULL;
	{
		HRESULT hr;
		if ( FAILED(hr = CreateStagingCopy(GetDevice(), GetImmediateContext(), pSrcResource, &pReadableSrc)) )
		{
			WriteToLog( _T( "CResourceAccessTest::ValidateResult() - Failed to create readable copy of src, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
			tRes = RESULT_FAIL;
			goto validateDone;
		}
		if ( bNeedsDest && FAILED(hr = CreateStagingCopy(GetDevice(), GetImmediateContext(), m_bSameResource ? pSrcResource : m_pDestResource, &pReadableDest)) )
		{
			WriteToLog( _T( "CResourceAccessTest::ValidateResult() - Failed to create readable copy of dest, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
			tRes = RESULT_FAIL;
			goto validateDone;
		}

		// CopySubresourceRegion can have overlapping copies and thus we need to keep a state of the src before the copy to check against
		if ( bNeedsPreCopySrc && FAILED(hr = CreateStagingCopy(GetDevice(), GetImmediateContext(), m_pSrcPreCopyResource, &pReadableSrcPreCopy)) )
		{
			WriteToLog( _T( "CResourceAccessTest::ValidateResult() - Failed to create readable copy of src before CopySubresourceRegion() , hr = %s" ), D3DHResultToString( hr ).c_str( ) );
			return RESULT_FAIL;
		}
	}

	// Show the necessary resources
	GetResourceViewer().HideAll();
	ShowResource(L"Src", pSrcResource, m_uSrcSubres, 0, 0, 8.0f );

	if (bNeedsDest)
	{
		ShowResource(L"Dest", m_bSameResource ? pSrcResource : m_pDestResource, m_uDestSubres, 0, 0, 8.0f);
	}

	if (bNeedsPreCopySrc)
	{
		ShowResource(L"Pre Copy Src", m_pSrcPreCopyResource, m_uSrcSubres, 0, 0, 8.0f);
	}

	// -1 for first iteration means compare srcsubres & dstsubres
	// subsequent iterations just ensure that nothing else got modified
	for( int iSubRes = -1; iSubRes < numSubresources && tRes == RESULT_PASS; ++iSubRes )
	{
		if (m_AccessMethod == AM_COPY && iSubRes < 0)
			continue;
		const UINT srcSubres  = (iSubRes>=0 || m_AccessMethod == AM_COPY) ? iSubRes : m_uSrcSubres;
		const UINT destSubres = (iSubRes>=0 || m_AccessMethod == AM_COPY) ? iSubRes : m_uDestSubres;
		HRESULT hr;
		switch( m_ResourceScenario )
		{
		case D3D_RESOURCE_SCENARIO_BUFFER:
			if (iSubRes < 0)
				break;

			hr = GetImmediateContext()->Map( pReadableSrc,  0, D3D11_MAP_READ, 0, &p1DSrc );
			if( FAILED( hr ) )
			{
				WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableSrc Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
				tRes = RESULT_FAIL;
			}

			if(!CheckMapIsByteAligned(&p1DSrc))
				tRes = RESULT_FAIL;

			if( bNeedsPreCopySrc )
			{
				hr = GetImmediateContext()->Map( pReadableSrcPreCopy, 0, D3D11_MAP_READ, 0, &p1DSrcPreCopy );
				if( FAILED( hr ) )
				{
					WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableSrcPreCopy Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
					tRes = RESULT_FAIL;
				}

				if(!CheckMapIsByteAligned(&p1DSrcPreCopy))
					tRes = RESULT_FAIL;
			}
			
			if( bNeedsDest ) 
			{
				hr = GetImmediateContext()->Map( pReadableDest, 0, D3D11_MAP_READ, 0, &p1DDest );
				if( FAILED( hr ) )
				{
					WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableDest Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
					tRes = RESULT_FAIL;
				}

				if(!CheckMapIsByteAligned(&p1DDest))
					tRes = RESULT_FAIL;
			}
			
			if( tRes == RESULT_PASS )
			{
				if( !CompareSubresources( p1DSrc.pData, 0, 0, p1DDest.pData, 0, 0, p1DSrcPreCopy.pData, 0, 0, iSubRes, iSubRes ) )
					tRes = RESULT_FAIL;
			}

			GetImmediateContext()->Unmap( pReadableSrc, 0 );
			if( bNeedsPreCopySrc ) GetImmediateContext()->Unmap( pReadableSrcPreCopy, 0 );
			if( bNeedsDest ) GetImmediateContext()->Unmap( pReadableDest, 0 );
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
			hr = GetImmediateContext()->Map( pReadableSrc, srcSubres, D3D11_MAP_READ, 0, &p1DSrc );
			if( FAILED( hr ) )
			{
				WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableSrc Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
				tRes = RESULT_FAIL;
			}
			
			if(!CheckMapIsByteAligned(&p1DSrc))
					tRes = RESULT_FAIL;

			if( bNeedsPreCopySrc )
			{
				hr = GetImmediateContext()->Map( pReadableSrcPreCopy, srcSubres, D3D11_MAP_READ, 0, &p1DSrcPreCopy );
				if( FAILED( hr ) )
				{
					WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableSrcPreCopy Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
					tRes = RESULT_FAIL;
				}

				if(!CheckMapIsByteAligned(&p1DSrcPreCopy))
					tRes = RESULT_FAIL;
			}

			if( bNeedsDest ) 
			{
				hr = GetImmediateContext()->Map( pReadableDest, destSubres, D3D11_MAP_READ, 0, &p1DDest );
				if( FAILED( hr ) )
				{
					WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableDest Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
					tRes = RESULT_FAIL;
				}

				if(!CheckMapIsByteAligned(&p1DDest))
					tRes = RESULT_FAIL;
			}

			if( tRes == RESULT_PASS )
			{
				if( !CompareSubresources( p1DSrc.pData, 0, 0, p1DDest.pData, 0, 0, p1DSrcPreCopy.pData, 0, 0, srcSubres, destSubres ) )
					tRes = RESULT_FAIL;
			}

			GetImmediateContext()->Unmap( pReadableSrc, srcSubres );
			if( bNeedsPreCopySrc ) GetImmediateContext()->Unmap( pReadableSrcPreCopy, srcSubres );
			if( bNeedsDest ) GetImmediateContext()->Unmap( pReadableDest, destSubres );
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
			hr = GetImmediateContext()->Map( pReadableSrc, srcSubres, D3D11_MAP_READ, 0, &mapTex2DSrc );
			if( FAILED( hr ) )
			{
				WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableSrc Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
				tRes = RESULT_FAIL;
			}

			if(!CheckMapIsByteAligned(&mapTex2DSrc))
					tRes = RESULT_FAIL;

			if( bNeedsPreCopySrc )
			{
				hr = GetImmediateContext()->Map( pReadableSrcPreCopy, srcSubres, D3D11_MAP_READ, 0, &mapTex2DSrcPreCopy );
				if( FAILED( hr ) )
				{
					WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableSrcPreCopy Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
					tRes = RESULT_FAIL;
				}

				if(!CheckMapIsByteAligned(&mapTex2DSrcPreCopy))
					tRes = RESULT_FAIL;
			}

			if( bNeedsDest )
			{
				hr = GetImmediateContext()->Map( pReadableDest, destSubres, D3D11_MAP_READ, 0, &mapTex2DDest );
				if( FAILED( hr ) )
				{
					WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableDest Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
					tRes = RESULT_FAIL;
				}

				if(!CheckMapIsByteAligned(&mapTex2DDest))
					tRes = RESULT_FAIL;
			}

			if( tRes == RESULT_PASS )
			{
				if( !CompareSubresources( mapTex2DSrc.pData, mapTex2DSrc.RowPitch, 0, mapTex2DDest.pData, mapTex2DDest.RowPitch, 0, mapTex2DSrcPreCopy.pData, mapTex2DSrcPreCopy.RowPitch, 0, srcSubres, destSubres ) )
					tRes = RESULT_FAIL;
			}

			GetImmediateContext()->Unmap( pReadableSrc, srcSubres );
			if( bNeedsPreCopySrc ) GetImmediateContext()->Unmap( pReadableSrcPreCopy, srcSubres );
			if( bNeedsDest ) GetImmediateContext()->Unmap( pReadableDest, destSubres );
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE3D:
			hr = GetImmediateContext()->Map( pReadableSrc, srcSubres, D3D11_MAP_READ, 0, &mapTex3DSrc );
			if( FAILED( hr ) )
			{
				WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableSrc Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
				tRes = RESULT_FAIL;
			}

			if(!CheckMapIsByteAligned(&mapTex3DSrc))
					tRes = RESULT_FAIL;

			if( bNeedsPreCopySrc )
			{
				hr = GetImmediateContext()->Map( pReadableSrcPreCopy, srcSubres, D3D11_MAP_READ, 0, &mapTex3DSrcPreCopy );
				if( FAILED( hr ) )
				{
					WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableSrcPreCopy Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
					tRes = RESULT_FAIL;
				}

				if(!CheckMapIsByteAligned(&mapTex3DSrcPreCopy))
					tRes = RESULT_FAIL;
			}

			if( bNeedsDest )
			{
				hr = GetImmediateContext()->Map( pReadableDest, destSubres, D3D11_MAP_READ, 0, &mapTex3DDest );
				if( FAILED( hr ) )
				{
					WriteToLog( _T( "CResourceAccessTest::ValidateResult() - pReadableDest Map failed, hr = %s" ), D3DHResultToString( hr ).c_str( ) );
					tRes = RESULT_FAIL;
				}

				if(!CheckMapIsByteAligned(&mapTex3DDest))
					tRes = RESULT_FAIL;
			}

			if( tRes == RESULT_PASS )
			{
				if( !CompareSubresources( mapTex3DSrc.pData, mapTex3DSrc.RowPitch, mapTex3DSrc.DepthPitch, mapTex3DDest.pData, mapTex3DDest.RowPitch, mapTex3DDest.DepthPitch, mapTex3DSrcPreCopy.pData, mapTex3DSrcPreCopy.RowPitch, mapTex3DSrcPreCopy.DepthPitch, srcSubres, destSubres ) )
					tRes = RESULT_FAIL;
			}

			GetImmediateContext()->Unmap( pReadableSrc, srcSubres );
			if( bNeedsPreCopySrc ) GetImmediateContext()->Unmap( pReadableSrcPreCopy, srcSubres );
			if( bNeedsDest ) GetImmediateContext()->Unmap( pReadableDest, destSubres );
			break;
		}
	}

validateDone:
	SAFE_RELEASE( pReadableSrc );
	SAFE_RELEASE( pReadableDest );
	SAFE_RELEASE( pReadableSrcPreCopy );

	return tRes;
}

//
// The runtime implementation of deferred contexts has a bug (windows 7 bug 691458)
// This function checks to see if the context used by GetEffectiveContext() would hit this bug
// if so, the src box is offset to work around the bug
//
HRESULT CResourceAccessTest::UpdateSubresource_Workaround(ID3D11Resource *pDstResource,
														  UINT dstSubresource,
														  const D3D11_BOX *pDstBox,
														  const void *pSrcData,
														  UINT srcBytesPerElement,
														  UINT srcRowPitch,
														  UINT srcDepthPitch,
														  bool* pDidWorkAround)
{
	HRESULT hr = S_OK;
	bool needWorkaround = false;
	D3D11_DEVICE_CONTEXT_TYPE contextType = GetEffectiveContext()->GetType();

	if( pDstBox && (D3D11_DEVICE_CONTEXT_DEFERRED == contextType) && m_CopyFlags == 0 )
	{
		D3D11_FEATURE_DATA_THREADING threadingCaps = { FALSE, FALSE };

		hr = GetDevice()->CheckFeatureSupport( D3D11_FEATURE_THREADING, &threadingCaps, sizeof(threadingCaps) );
		if( SUCCEEDED(hr) )
		{
			if( !threadingCaps.DriverCommandLists )
			{
				needWorkaround = true;
			}
		}
	}

	const void* pAdjustedSrcData = pSrcData;

	if( needWorkaround )
	{
		D3D11_BOX alignedBox = *pDstBox;
		
		// convert from pixels to blocks
		if( m_bBC )
		{
			alignedBox.left     /= 4;
			alignedBox.right    /= 4;
			alignedBox.top      /= 4;
			alignedBox.bottom   /= 4;
		}

		pAdjustedSrcData = ((const BYTE*)pSrcData) - (alignedBox.front * srcDepthPitch) - (alignedBox.top * srcRowPitch) - (alignedBox.left * srcBytesPerElement);
	}

	if( m_CopyFlags == 0 )
	{
		GetEffectiveContext()->UpdateSubresource( pDstResource, dstSubresource, pDstBox, pAdjustedSrcData, srcRowPitch, srcDepthPitch );
	}
	else
	{
		GetEffectiveContext1()->UpdateSubresource1( pDstResource, dstSubresource, pDstBox, pAdjustedSrcData, srcRowPitch, srcDepthPitch, m_CopyFlags );
	}

	if( pDidWorkAround )
	{
		*pDidWorkAround = needWorkaround;
	}

	return hr;
}

// This function is used by DO_NOT_WAIT Map tests to make sure drivers don't return S_OK before Map is truly complete.
// If commands are still being run on an incorrect S_OK the final map in this function will not have been executed
// and some expected data will be missing, causing the test case to fail.
HRESULT CResourceAccessTest::RunLargeCommandList()
{
	HRESULT hr = E_FAIL;

	// take up time with several whole resource copies
	for(UINT i = 0; i < NUM_LARGE_COPIES; ++i)
	{
		GetEffectiveContext()->CopyResource(m_pDestResource, m_pSrcResource);
	}

	if(FAILED(hr = ExecuteEffectiveContext()))
	{
		WriteToLog(_T("CResourceAccessTest::RunLargeCommandList - ExecuteEffectiveContext() failed for large command list, hr=%s"), D3DHResultToString(hr));
		goto Done;
	}

	// finally populate resource with some expected data
	D3D11_MAPPED_SUBRESOURCE mappedTex;
	
	const UINT bytesPerElem = GetBitsPerElement(m_SrcFormatSame) / 8;

	UINT scenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &scenFlags );
	bool bCube = (scenFlags & D3D_RS_FLAGS_TEXTURECUBE) != 0;
	bool bArray = (scenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	UINT numSubresources = RA_MIPLEVELS;
	UINT mipLevel = m_uSrcSubres;

	if( bCube )
	{
		numSubresources = 6 * RA_MIPLEVELS;
		mipLevel = m_uSrcSubres % RA_MIPLEVELS;
	}
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER )
	{
		numSubresources = 1;
		mipLevel = 0;
	}
	else if( bArray )
	{
		numSubresources = RA_NUMSUBRES;
		mipLevel = m_uSrcSubres % RA_MIPLEVELS;
	}

	UINT mipWidth = m_uWidth;
	UINT mipHeight = m_ResourceScenario != D3D_RESOURCE_SCENARIO_BUFFER && m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURE1D && m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY ? m_uHeight : 1;
	UINT mipDepth = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D ? m_uDepth : 1;

	GetMipDimensions(mipLevel, mipWidth, mipHeight, mipDepth);
	if(m_bBC)
	{
		mipHeight /= 4;
		mipWidth /= 4;
	}

	if(FAILED(hr = GetImmediateContext()->Map(m_pDestResource, m_uSrcSubres, D3D11_MAP_WRITE, 0, &mappedTex)))
	{
		WriteToLog(_T("CResourceAccessTest::RunLargeCommandList - Map failed, hr=%s"), D3DHResultToString(hr).c_str());
		goto Done;
	}

	UINT dataSize = mipWidth * bytesPerElem;
	if(m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D || m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY)
	{
		dataSize = mappedTex.RowPitch * mipHeight;
	}
	else if(m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURECUBE)
	{
		dataSize = mappedTex.RowPitch * mipWidth;
	}
	else if(m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D)
	{
		dataSize = mappedTex.DepthPitch * mipDepth;
	}

	for(UINT i = 0; i < dataSize; ++i)
	{
		if(i % 2 == 0)
		{
			((BYTE*)mappedTex.pData)[ i ] = RESOURCE_TAG_EVEN;
		}
	}

	GetImmediateContext()->Unmap(m_pDestResource, m_uSrcSubres);

Done:	
	return hr; 
}


TEST_RESULT CResourceAccessTest::ExecuteTestCase()
{
	HRESULT hr;
	UINT uBytesPerElement = GetBitsPerElement( m_SrcFormatSame ) / 8;
	UINT uRowPitch = ( max( uBytesPerElement, 1 ) ) * m_uWidth;
	UINT uDepthPitch = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D ? ( uBytesPerElement ) * m_uWidth * m_uHeight : 0;
	bool didWorkAround = false;
	bool bTestTileable = TestTileableCopy();
	const bool bD3D9 = GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0;

	if( m_bBC )
	{
		UINT uBlockPitch = 0;
		switch( m_SrcFormatSame )
		{
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC4_SNORM:
		case DXGI_FORMAT_BC4_UNORM:
			uBlockPitch = 8;
			break;
		default:
			uBlockPitch = 16;
			break;
		}
		uBytesPerElement = uBlockPitch;
		uRowPitch = uBlockPitch * m_uWidth / 4;
		uDepthPitch =  m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D ? uRowPitch * m_uHeight / 4 : 0;
	}
	
	if( !(m_AccessMethod == AM_MAP && m_bDoNotWait) && !bD3D9 )
		GetEffectiveContext()->Begin( m_pQuery );

	D3D11_MAPPED_SUBRESOURCE mapTex2D;
	D3D11_MAPPED_SUBRESOURCE mapTex3D;
	D3D11_MAPPED_SUBRESOURCE pData;

	if( bTestTileable )
	{
		// The render target must be set before CopySubresourceRegion is called for the COPY_TILEABLE flag to be meaningful
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pTilingRTV, NULL );
	}

	switch( m_AccessMethod )
	{
	case AM_COPY:
		GetEffectiveContext()->CopyResource( m_pDestResource, m_pSrcResource );
		break;
	case AM_COPYREGION:
		// CopySubresourceRegion can have overlapping copies and thus we need to keep a state of the src before the copy to check against
		assert( m_pSrcPreCopyResource );
		GetEffectiveContext()->CopyResource( m_pSrcPreCopyResource, m_pSrcResource );
		if( m_CopyFlags == 0 )
		{
			GetEffectiveContext()->CopySubresourceRegion( m_bSameResource ? m_pSrcResource : m_pDestResource, m_uDestSubres, m_DestPoint.x, m_DestPoint.y, m_DestPoint.z, m_pSrcResource, m_uSrcSubres, &m_SrcBox );
		}
		else
		{
			GetEffectiveContext1()->CopySubresourceRegion1( m_bSameResource ? m_pSrcResource : m_pDestResource, m_uDestSubres, m_DestPoint.x, m_DestPoint.y, m_DestPoint.z, m_pSrcResource, m_uSrcSubres, &m_SrcBox, m_CopyFlags );
		}
		break;
	case AM_MAP:
		if( !TestMap() )
		{
			if( !m_bDoNotWait && !bD3D9 ) GetEffectiveContext()->End( m_pQuery );
			return RESULT_FAIL;
		}
		break;
	case AM_UPDATE:
		if( 0 == m_CopyFlags )
		{
			if(m_bNULLSrcBox)
			{
				hr = UpdateSubresource_Workaround( m_pSrcResource, m_uSrcSubres, NULL, (void*) m_pTagData, uBytesPerElement, uRowPitch, uDepthPitch, &didWorkAround );
			}
			else
			{
				hr = UpdateSubresource_Workaround( m_pSrcResource, m_uSrcSubres, &m_SrcBox, (void*) m_pTagData, uBytesPerElement, uRowPitch, uDepthPitch, &didWorkAround );
			}		

			if( FAILED(hr) )
			{
				WriteToLog(_T("UpdateSubresource_Workaround() failed"));
				return RESULT_FAIL;
			}
		}
		else
		{
			if( m_bNULLSrcBox )
			{
				GetEffectiveContext1()->UpdateSubresource1( m_pSrcResource, m_uSrcSubres, nullptr, m_pTagData, uRowPitch, uDepthPitch, m_CopyFlags );
			}
			else
			{
				GetEffectiveContext1()->UpdateSubresource1( m_pSrcResource, m_uSrcSubres, &m_SrcBox, m_pTagData, uRowPitch, uDepthPitch, m_CopyFlags );
			}
		}

		break;
	}

	if( !(m_AccessMethod == AM_MAP && m_bDoNotWait) && !bD3D9 )
		GetEffectiveContext()->End( m_pQuery );

	if( m_AccessMethod != AM_MAP && FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		return RESULT_FAIL;
	}

	if( !(m_AccessMethod == AM_MAP && m_bDoNotWait) && !bD3D9 )
	{
		D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineData;
		hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pQuery, (void*) &pipelineData, sizeof( pipelineData ), 0 );
		if( hr != S_OK )
		{
			WriteToLog( _T( "CResourceAccessTest::ExecuteTestCase() - ID3D11Query::GetData() failed or timed out" ) );
			return RESULT_FAIL;
		}

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog(_T("ExecuteEffectiveContext() failed"));
			return RESULT_FAIL;
		}
	
		if( pipelineData.CInvocations != 0 &&
			pipelineData.CPrimitives != 0 &&
			pipelineData.GSInvocations != 0 &&
			pipelineData.GSPrimitives != 0 &&
			pipelineData.IAPrimitives != 0 &&
			pipelineData.IAVertices != 0 &&
			pipelineData.PSInvocations != 0 &&
			pipelineData.VSInvocations != 0 )	
		{
			WriteToLog( _T( "Test case failed: Pipeline statistics were modified by the tested function\n"
						"\t\tCInvocations = %d\n"
						"\t\tCPrimitives = %d\n"
						"\t\tGSInvocations = %d\n"
						"\t\tGSPrimitives = %d\n"
						"\t\tIAPrimitives = %d\n"
						"\t\tIAVertices = %d\n"
						"\t\tPSInvocations = %d\n"
						"\t\tVSInvocations = %d\n" ),
						pipelineData.CInvocations, pipelineData.CPrimitives, pipelineData.GSInvocations, pipelineData.GSPrimitives,
						pipelineData.IAPrimitives, pipelineData.IAVertices, pipelineData.PSInvocations, pipelineData.VSInvocations );
			return RESULT_FAIL;
		}
	}

	if( bTestTileable )
	{
		// Test D3D11_COPY_TILEABLE by rendering from the dest resource back to the source

		// m_SrcBox is the box in the RTV we have to render into.
		// m_DestPoint is the top-left point in the SRV we have to render from

		// Let's set the viewport to cover m_SrcBox in the RTV
		// Then render a rectangle that covers the viewport, with texcoords at the four corners of the dest box in the SRV
		// For 8-bit UNORM, the pixel shader will invert the color, otherwise keep the same color

		// The pixel shader changes based on what type of SRV to read (array or not) and whether or not to invert the color
		bool bArray = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY;
		bool bInvertibleFormat = EightBitUNORMFormat();
		ATL::CComPtr<ID3D11PixelShader>& pPS = bArray ? ( bInvertibleFormat ? m_pTilingPS_InvertArray : m_pTilingPS_Array ) : ( bInvertibleFormat ? m_pTilingPS_Invert : m_pTilingPS );

		UINT Stride = sizeof(TilingVertex);
		UINT Offset = 0;

		D3D11_VIEWPORT vp = { (FLOAT)m_SrcBox.left, (FLOAT)m_SrcBox.top, (FLOAT)m_SrcBox.Width(), (FLOAT)m_SrcBox.Height(), 0, 1 };

		assert( m_pTilingIL != NULL );
		assert( m_pTilingVB != NULL );
		assert( m_pTilingVS != NULL );
		assert( m_pTilingSampler != NULL );
		assert( pPS != NULL );
		assert( m_pTilingSRV != NULL );
		assert( m_pTilingRTV != NULL );
		assert( m_pTilingRS != NULL );

		GetEffectiveContext()->IASetInputLayout( m_pTilingIL );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pTilingVB, &Stride, &Offset );
		GetEffectiveContext()->VSSetShader( m_pTilingVS, NULL, 0 );
		GetEffectiveContext()->PSSetSamplers( 0, 1, &m_pTilingSampler );
		GetEffectiveContext()->PSSetShader( pPS, NULL, 0 );
		GetEffectiveContext()->PSSetShaderResources( 0, 1, &m_pTilingSRV );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pTilingRTV, NULL );
		GetEffectiveContext()->RSSetViewports( 1, &vp );
		GetEffectiveContext()->RSSetState( m_pTilingRS );

		GetEffectiveContext()->Draw( 4, 0 );

		// reset the context
		ID3D11RenderTargetView* ppNULLRTV[] = {NULL};
		ID3D11ShaderResourceView* ppNULLSRV[] = {NULL};
		ID3D11Buffer* ppNULLBuff[] = {NULL};
		ID3D11SamplerState* ppNULLSamp[] = {NULL};

		GetEffectiveContext()->IASetInputLayout( NULL );
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, ppNULLBuff, &Stride, &Offset );
		GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->PSSetSamplers( 0, 1, ppNULLSamp );
		GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->PSSetShaderResources( 0, 1, ppNULLSRV );
		GetEffectiveContext()->RSSetViewports( 1, &vp );
		GetEffectiveContext()->RSSetState( NULL );

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog(_T("ExecuteEffectiveContext() failed"));
			return RESULT_FAIL;
		}
	}

	if( m_AccessMethod != AM_MAP && RESULT_PASS != ValidateResult( ) )
	{
		if( didWorkAround )
		{
			WriteToLog(_T("This test case did apply a work around to avoid hitting a bug in the runtime's implementation of UpdateSubresource (deferred context)"));
		}
		return RESULT_FAIL;
	}


	return RESULT_PASS;
}

bool IsWriteMapType( D3D11_MAP MapType )
{
	switch( MapType )
	{
	case D3D11_MAP_WRITE:
	case D3D11_MAP_READ_WRITE:
	case D3D11_MAP_WRITE_DISCARD:
	case D3D11_MAP_WRITE_NO_OVERWRITE:
		return true;
	}
	return false;
}

bool IsWriteOnlyMapType( D3D11_MAP MapType )
{
	switch( MapType )
	{
	case D3D11_MAP_WRITE:
	case D3D11_MAP_WRITE_DISCARD:
	case D3D11_MAP_WRITE_NO_OVERWRITE:
		return true;
	}
	return false;
}

bool CResourceAccessTest::TestMap( )
{
	HRESULT hr;
	UINT scenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &scenFlags );
	bool bCube = (scenFlags & D3D_RS_FLAGS_TEXTURECUBE) != 0;
	bool bArray = (scenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	UINT numSubresources = RA_MIPLEVELS;
	UINT mipLevel = m_uSrcSubres;
	bool bPass = true;

	if( bCube )
	{
		numSubresources = 6 * RA_MIPLEVELS;
		mipLevel = m_uSrcSubres % RA_MIPLEVELS;
	}
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER )
	{
		numSubresources = 1;
		mipLevel = 0;
	}
	else if( bArray )
	{
		numSubresources = RA_NUMSUBRES;
		mipLevel = m_uSrcSubres % RA_MIPLEVELS;
	}

	D3D11_MAPPED_SUBRESOURCE pData;
	D3D11_MAPPED_SUBRESOURCE mapTex2D;
	D3D11_MAPPED_SUBRESOURCE mapTex3D;
	UINT bytesPerElem = GetBitsPerElement( m_SrcFormatSame ) / 8;
	UINT mipWidth = m_uWidth;
	UINT mipHeight = m_ResourceScenario != D3D_RESOURCE_SCENARIO_BUFFER && m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURE1D && m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY ? m_uHeight : 1;
	UINT mipDepth = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D ? m_uDepth : 1;
	
	GetMipDimensions( mipLevel, mipWidth, mipHeight, mipDepth );
	if( m_bBC )
	{
		mipHeight /= 4;
		mipWidth /= 4;
	}

	if(m_bDoNotWait && FAILED(hr = RunLargeCommandList()))
	{
		WriteToLog(_T("CResourceAccessTest::TestMap() - RunLargeCommandList() failed, hr=%s"), D3DHResultToString(hr).c_str());
		return false;
	}

	if( IsWriteMapType( m_MapType ) )
	{
		int numAttempts = 0;

		switch( m_ResourceScenario )
		{
		case D3D_RESOURCE_SCENARIO_BUFFER:
			{
			if( GetFramework()->IsDeferredContextUsed() )
			{
				if( FAILED(GetEffectiveContext()->Map( m_pDestResource, 0, D3D11_MAP_WRITE_DISCARD, m_bDoNotWait ? D3D11_MAP_FLAG_DO_NOT_WAIT : 0, &pData ) ) ) return false;
				GetEffectiveContext()->Unmap(m_pDestResource,0);
			}

			D3D11_MAP MapType = m_MapType;
			switch( m_MapType )
			{
			case D3D11_MAP_READ_WRITE:
			case D3D11_MAP_WRITE:
				MapType = GetFramework()->IsDeferredContextUsed() ? D3D11_MAP_WRITE_NO_OVERWRITE : m_MapType;
				break;
			}

			numAttempts = 0;
			do
			{
				hr = GetEffectiveContext()->Map( m_pDestResource, 0, MapType, m_bDoNotWait ? D3D11_MAP_FLAG_DO_NOT_WAIT : 0, &pData );
				Sleep(DONOTWAIT_DELAY);
				numAttempts++;
			} while( m_bDoNotWait && hr == DXGI_ERROR_WAS_STILL_DRAWING && numAttempts < DONOTWAIT_ATTEMPTS );

			if( hr == DXGI_ERROR_WAS_STILL_DRAWING && numAttempts == DONOTWAIT_ATTEMPTS )
			{
				WriteToLog(_T("Map returning DXGI_ERROR_WAS_STILL_DRAWING indefinitely when DO_NOT_WAIT is used, hr=%s"), D3DHResultToString(hr).c_str());
				return false;
			}
			else if( FAILED( hr ) )
			{
				WriteToLog(_T("Map failed, hr=%s"), D3DHResultToString(hr).c_str());
				return false;
			}

			if(!CheckMapIsByteAligned(&pData))
					bPass = false;

			for( UINT i = 0; i < bytesPerElem * m_uWidth; ++i )
				if( i % 2 )
					( (BYTE*) pData.pData )[ i ] = RESOURCE_TAG_ODD;
			GetEffectiveContext()->Unmap(m_pDestResource,0);
			break;
			}
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
			numAttempts = 0;
			do
			{
				hr = GetEffectiveContext()->Map( m_pDestResource, m_uSrcSubres, GetFramework()->IsDeferredContextUsed() ? D3D11_MAP_WRITE_NO_OVERWRITE : m_MapType, m_bDoNotWait ? D3D11_MAP_FLAG_DO_NOT_WAIT : 0, &pData );
				Sleep(DONOTWAIT_DELAY);
				numAttempts++;
			} while( m_bDoNotWait && hr == DXGI_ERROR_WAS_STILL_DRAWING && numAttempts < DONOTWAIT_ATTEMPTS );

			if( hr == DXGI_ERROR_WAS_STILL_DRAWING && numAttempts == DONOTWAIT_ATTEMPTS )
			{
				WriteToLog(_T("Map returning DXGI_ERROR_WAS_STILL_DRAWING indefinitely when DO_NOT_WAIT is used, hr=%s"), D3DHResultToString(hr).c_str());
				return false;
			}
			else if( FAILED( hr ) )
			{
				WriteToLog(_T("Map failed, hr=%s"), D3DHResultToString(hr).c_str());
				return false;
			}

			if(!CheckMapIsByteAligned(&pData))
					bPass = false;

			for( UINT i = 0; i < bytesPerElem * mipWidth; ++i )	
				if( i % 2 )
					( (BYTE*) pData.pData )[ i ] = RESOURCE_TAG_ODD;
			GetEffectiveContext()->Unmap( m_pDestResource, m_uSrcSubres );
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURECUBE:	
			numAttempts = 0;
			do
			{
				hr = GetEffectiveContext()->Map( m_pDestResource, m_uSrcSubres, GetFramework()->IsDeferredContextUsed() ? D3D11_MAP_WRITE_NO_OVERWRITE : m_MapType, m_bDoNotWait ? D3D11_MAP_FLAG_DO_NOT_WAIT : 0, &mapTex2D );
				Sleep(DONOTWAIT_DELAY);
				numAttempts++;
			} while( m_bDoNotWait && hr == DXGI_ERROR_WAS_STILL_DRAWING && numAttempts < DONOTWAIT_ATTEMPTS );

			if( hr == DXGI_ERROR_WAS_STILL_DRAWING && numAttempts == DONOTWAIT_ATTEMPTS )
			{
				WriteToLog(_T("Map returning DXGI_ERROR_WAS_STILL_DRAWING indefinitely when DO_NOT_WAIT is used, hr=%s"), D3DHResultToString(hr).c_str());
				return false;
			}
			else if( FAILED( hr ) )
			{
				WriteToLog(_T("Map failed, hr=%s"), D3DHResultToString(hr).c_str());
				return false;
			}

			if(!CheckMapIsByteAligned(&mapTex2D))
					bPass = false;

			for( UINT i = 0; i < mapTex2D.RowPitch * (bCube ? mipWidth : mipHeight); ++i )	
				if( i % 2 )
					( (BYTE*) mapTex2D.pData )[ i ] = RESOURCE_TAG_ODD;
			GetEffectiveContext()->Unmap( m_pDestResource, m_uSrcSubres );
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE3D:
			numAttempts = 0;
			do
			{
				hr = GetEffectiveContext()->Map( m_pDestResource, m_uSrcSubres, GetFramework()->IsDeferredContextUsed() ? D3D11_MAP_WRITE_NO_OVERWRITE : m_MapType, m_bDoNotWait ? D3D11_MAP_FLAG_DO_NOT_WAIT : 0, &mapTex3D );
				Sleep(DONOTWAIT_DELAY);
				numAttempts++;
			} while( m_bDoNotWait && hr == DXGI_ERROR_WAS_STILL_DRAWING && numAttempts < DONOTWAIT_ATTEMPTS );

			if( hr == DXGI_ERROR_WAS_STILL_DRAWING && numAttempts == DONOTWAIT_ATTEMPTS )
			{
				WriteToLog(_T("Map returning DXGI_ERROR_WAS_STILL_DRAWING indefinitely when DO_NOT_WAIT is used, hr=%s"), D3DHResultToString(hr).c_str());
				return false;
			}
			else if( FAILED( hr ) )
			{
				WriteToLog(_T("Map failed, hr=%s"), D3DHResultToString(hr).c_str());
				return false;
			}

			if(!CheckMapIsByteAligned(&mapTex3D))
					bPass = false;

			for( UINT i = 0; i < mapTex3D.DepthPitch * mipDepth; ++i )	
				if( i % 2 )
					( (BYTE*) mapTex3D.pData )[ i ] = RESOURCE_TAG_ODD;
			GetEffectiveContext()->Unmap( m_pDestResource, m_uSrcSubres );
			break;
		}
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		bPass = false;
	}

	ID3D11Resource *pResultResource = m_pDestResource;
	if( IsWriteOnlyMapType( m_MapType ) && m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER )
	{
		GetImmediateContext()->CopyResource(m_pSrcResource, m_pDestResource);
		pResultResource = m_pSrcResource;
	}

	switch( m_ResourceScenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		GetImmediateContext()->Map( pResultResource, 0, D3D11_MAP_READ, 0, &pData );
		
		if(!CheckMapIsByteAligned(&pData))
					bPass = false;

		bPass &= CompareSubresources( pData.pData, 0, 0, NULL, 0, 0, NULL, 0, 0, 0, 0 );
		GetImmediateContext()->Unmap( pResultResource, 0 );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		GetImmediateContext()->Map( m_pDestResource, m_uSrcSubres, D3D11_MAP_READ, 0, &pData );

		if(!CheckMapIsByteAligned(&pData))
					bPass = false;

		bPass &= CompareSubresources( pData.pData, 0, 0, NULL, 0, 0, NULL, 0, 0, m_uSrcSubres, 0 );
		GetImmediateContext()->Unmap( m_pDestResource, m_uSrcSubres );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:	
		GetImmediateContext()->Map( m_pDestResource, m_uSrcSubres, D3D11_MAP_READ, 0, &mapTex2D );

		if(!CheckMapIsByteAligned(&mapTex2D))
					bPass = false;

		bPass &= CompareSubresources( mapTex2D.pData, mapTex2D.RowPitch, 0, NULL, 0, 0, NULL, 0, 0, m_uSrcSubres, 0 );
		GetImmediateContext()->Unmap( m_pDestResource, m_uSrcSubres );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		GetImmediateContext()->Map( m_pDestResource, m_uSrcSubres, D3D11_MAP_READ, 0, &mapTex3D );

		if(!CheckMapIsByteAligned(&mapTex3D))
					bPass = false;

		bPass &= CompareSubresources( mapTex3D.pData, mapTex3D.RowPitch, mapTex3D.DepthPitch, NULL, 0, 0, NULL, 0, 0, m_uSrcSubres, 0 );
		GetImmediateContext()->Unmap( m_pDestResource, m_uSrcSubres );
		break;
	}

	return bPass;
}

// Returns true iff either of the following are true:
//   1. b1 and b2 are equal
//   2. b1 and b2 are off by one and the format is an SNORM format
// This is done because -1.0f has two representations in SNORM, 0xf0 and 0xf1 (or 0xf000 and 0xf001)
bool SNORMSingleBitDiff( BYTE b1, BYTE b2, DXGI_FORMAT format )
{
	if( b1 == b2 )
		return true;
	
	bool bOneBitDiff = (b1 ^ b2) == 1;

	if( bOneBitDiff && CD3D10FormatHelper_Test::GetFormatComponentInterpretation( format, 0 ) == D3D10FCI_SNORM )
		return true;
	
	return false;
}

bool CResourceAccessTest::CompareSubresources( void *pSrcData, UINT uSrcRowPitch, UINT uSrcDepthPitch, void *pDestData, UINT uDestRowPitch, UINT uDestDepthPitch, void *pSrcPreCopyData, UINT uSrcPreCopyRowPitch, UINT uSrcPreCopyDepthPitch, UINT uSrcSubres, UINT uDestSubres ) const
{
	bool bPass = true;
	bool bLogAll = GetFramework()->LogVerbose( );
	bool bSameFormat = GetTestCaseParameter( _T( "SrcFormatSame" ) )->IsActive( );
	UINT uElementWidth = max( GetBitsPerElement( bSameFormat ? m_SrcFormatSame : m_SrcFormat ) / 8, 1 );
	UINT scenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &scenFlags );
	bool bBufferScenario = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER;
	bool bOneDim = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER || (scenFlags & D3D_RS_FLAGS_TEXTURE1D);
	bool bCube = (scenFlags & D3D_RS_FLAGS_TEXTURECUBE) != 0;
	bool bArray = (scenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	bool bOverlappingCopy = (m_AccessMethod == AM_COPYREGION) && m_bSameResource && (m_uSrcSubres == m_uDestSubres);
	DXGI_FORMAT srcFormat = bSameFormat ? m_SrcFormatSame : m_SrcFormat;
	UINT uMipSlice = uSrcSubres % RA_MIPLEVELS;
	if( bArray )
		uMipSlice = uSrcSubres % RA_MIPLEVELS;
	else if( bCube )
		uMipSlice = 0;

	UINT width = m_uWidth;
	UINT height = bOneDim ? 1 : ( bCube ? m_uWidth : m_uHeight );
	UINT depth = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D ? m_uDepth : 1;
	GetMipDimensions( uMipSlice, width, height, depth );

	if (m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER)
	{
		// buffers are in bytes
		width = m_uWidth * uElementWidth;
		uElementWidth = 1;
	}

	const bool bTestDiscard = (m_CopyFlags & D3D11_COPY_DISCARD_TEST) > 0;

	UINT uBlockPitch;
	if( m_bBC )
	{
		UINT oldWidth = width;
		UINT oldHeight = height;

		height /= 4;
		width /= 4;
		if( oldWidth % 4 )
			width++;
		if( oldHeight % 4 )
			height++;

		switch( srcFormat )
		{
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC4_SNORM:
		case DXGI_FORMAT_BC4_UNORM:
			uBlockPitch = 8;
			break;
		default:
			uBlockPitch = 16;
			break;
		}
	}

	UINT uBytesPerElem = GetBitsPerElement( srcFormat ) / 8;
	if( uBytesPerElem == 0 )
		uBytesPerElem = 1;
	UINT uExpectedDepth = uSrcDepthPitch == 0 ? 0 : m_uWidth * m_uHeight * uBytesPerElem;
	UINT uExpectedRow = uSrcRowPitch == 0 ? 0 : m_uWidth * uBytesPerElem;
	if( m_bBC )
	{
		uExpectedRow = uBlockPitch * width;
		uExpectedDepth = uExpectedRow * height;
	}

	if( m_AccessMethod == AM_COPY )
	{
		for( UINT z = 0; z < depth; ++z )
		{
			for( UINT y = 0; y < height; ++y )
			{
				for( UINT x = 0; x < width; ++x )
				{
					for( UINT uByte = 0; uByte < uElementWidth; ++uByte )
					{
						// ignore the X channel
						if( srcFormat == DXGI_FORMAT_D32_FLOAT_S8X24_UINT && uByte > 4 )
							continue;
						if( (srcFormat == DXGI_FORMAT_B8G8R8X8_UNORM || srcFormat == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) && uByte > 2 )
							continue;

						BYTE srcByte = ( (BYTE*) pSrcData )[ z * uSrcDepthPitch + y * uSrcRowPitch + x * uElementWidth + uByte ];
						BYTE destByte = ( (BYTE*) pDestData )[ z * uDestDepthPitch + y * uDestRowPitch + x * uElementWidth + uByte ];
						if( srcByte != destByte && ( bPass || bLogAll ) )
						{
							WriteToLog( _T("Test case failed subresource %d, byte %d at (%d,%d,%d) does not match:\n\t\tSource: %x\n\t\tDest: %x" ), uSrcSubres, uByte, x, y, z, srcByte, destByte );
							bPass = false;
						}
					}
				}
			}
		}
	}
	else if( m_AccessMethod == AM_COPYREGION )
	{
		bool bEmptyBox = m_SrcBox == m_pBoxes[ 0 ];
		bool bTestTileable = TestTileableCopy();
		bool bInvertibleFormat = EightBitUNORMFormat();

		D3D_TEST_BOX destBox;
		destBox.left = m_DestPoint.x;
		destBox.right = (m_SrcBox.right - m_SrcBox.left) + m_DestPoint.x;
		destBox.top = m_DestPoint.y;
		destBox.bottom = (m_SrcBox.bottom - m_SrcBox.top) + m_DestPoint.y;
		destBox.front = m_DestPoint.z;
		destBox.back = (m_SrcBox.back - m_SrcBox.front) + m_DestPoint.z;
		D3D_TEST_BOX srcBox = m_SrcBox;

		if( m_bBC )
		{
			destBox.left /= 4;
			destBox.right /= 4;
			destBox.top /= 4;
			destBox.bottom /= 4;

			srcBox.left /= 4;
			srcBox.right /= 4;
			srcBox.top /= 4;
			srcBox.bottom /= 4;
		}

		for( UINT z = 0; z < depth; ++z )
		{
			for( UINT y = 0; y < height; ++y )
			{
				for( UINT x = 0; x < width; ++x )
				{
					if (!m_bBC)
						uBlockPitch = uElementWidth;
					for( UINT uByte = 0; uByte < uBlockPitch; ++uByte )
					{
						// ignore the X channel
						if( srcFormat == DXGI_FORMAT_D32_FLOAT_S8X24_UINT && uByte > 4 )
							continue;
						if( (srcFormat == DXGI_FORMAT_B8G8R8X8_UNORM || srcFormat == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) && uByte > 2 )
							continue;

						BYTE srcByte = ( (BYTE*) pSrcData )[ z * uSrcDepthPitch + y * uSrcRowPitch + x * uBlockPitch + uByte ];
						BYTE destByte = ( (BYTE*) pDestData )[ z * uDestDepthPitch + y * uDestRowPitch + x * uBlockPitch + uByte ];
						BYTE originalByteSrc = m_pResourceData[ uSrcSubres % RA_DATA_ELEMENTS ][ z * uExpectedDepth + y * uExpectedRow + x * uBlockPitch + uByte ];
						BYTE originalByteDest = m_pResourceData[ uDestSubres % RA_DATA_ELEMENTS ][ z * uExpectedDepth + y * uExpectedRow + x * uBlockPitch + uByte ];
						BYTE inverseByteSrc = 255 - originalByteSrc;

						bool bWithinDestBox = WithinBox( destBox, x, y, z );
						bool bWithinUpdatedSrcBox = WithinBox( srcBox, x, y, z ) && m_uSrcSubres == uSrcSubres && bTestTileable && bInvertibleFormat;

						if( !bEmptyBox && bWithinDestBox && m_uDestSubres == uDestSubres )
						{
							if ( m_uSrcSubres == uSrcSubres ) // cannot test the value if we don't have the right one
							{
								BYTE offsetSrcByte;
								if( bOverlappingCopy )
								{
									assert( pSrcPreCopyData );
									offsetSrcByte = ( (BYTE*) pSrcPreCopyData )[ ( z - destBox.front + srcBox.front ) * uSrcPreCopyDepthPitch + ( y - destBox.top + srcBox.top ) * uSrcPreCopyRowPitch + ( x - destBox.left + srcBox.left ) * uBlockPitch + uByte ];
								}
								else
								{
									offsetSrcByte = ( (BYTE*) pSrcData )[ ( z - destBox.front + srcBox.front ) * uSrcDepthPitch + ( y - destBox.top + srcBox.top ) * uSrcRowPitch + ( x - destBox.left + srcBox.left ) * uBlockPitch + uByte ];
								}

								BYTE origOffsetSrcByte = 0;
								if( bTestTileable && bInvertibleFormat )
								{
									// The pixel shader inverts the color values in this case
									origOffsetSrcByte = offsetSrcByte;
									offsetSrcByte = 255 - offsetSrcByte;
								}

								if (offsetSrcByte != destByte && ( bPass || bLogAll ) && !SNORMSingleBitDiff( offsetSrcByte, destByte, srcFormat ) )
								{
									WriteToLog( _T("Test case failed dest subresource %d, byte %d at (%d,%d,%d) does not match:\n\t\tSource: %x\n\t\tDest: %x" ), uDestSubres, uByte, x, y, z, offsetSrcByte, destByte );
									bPass = false;
								}
							}
						}
						else if( ( !bWithinUpdatedSrcBox && srcByte != originalByteSrc ) && !bTestDiscard && ( bPass || bLogAll ) && !SNORMSingleBitDiff( srcByte, originalByteSrc, srcFormat ) )
						{
							WriteToLog( _T("Test case failed src subresource %d, byte %d at (%d,%d,%d) shouldn't have been updated:\n\t\tSource: %x\n\t\tDest: %x\n\t\tOriginal: %x" ), uSrcSubres, uByte, x, y, z, srcByte, destByte, originalByteSrc );
							bPass = false;
						}
						else if( ( bWithinUpdatedSrcBox && srcByte != inverseByteSrc ) && ( bPass || bLogAll ) && !SNORMSingleBitDiff( srcByte, srcByte, srcFormat ) )
						{
							// The pixel shader inverts the color values in this case
							WriteToLog( _T("Test case failed src subresource %d, byte %d at (%d,%d,%d) should have been updated:\n\t\tSource: %x\n\t\tDest: %x\n\t\t1-Original: %x" ), uSrcSubres, uByte, x, y, z, srcByte, destByte, inverseByteSrc );
							bPass = false;
						}
						else if( (destByte != originalByteDest) && !bTestDiscard && ( bPass || bLogAll ) && !SNORMSingleBitDiff( destByte, originalByteDest, srcFormat ) )
						{
							WriteToLog( _T("Test case failed dest subresource %d, byte %d at (%d,%d,%d) shouldn't have been updated:\n\t\tSource: %x\n\t\tDest: %x\n\t\tOriginal: %x" ), uDestSubres, uByte, x, y, z, srcByte, destByte, originalByteDest );
							bPass = false;
						}
					}
				}
			}
		}
		
	}
	else if( m_AccessMethod == AM_UPDATE )
	{
		D3D_TEST_BOX srcBox = m_SrcBox;

		// if m_bNULLSrcBox = true, then srcBox will be uninitialized
		// but srcBox is needed to determine where in m_pTagData to look
		if( m_bNULLSrcBox )
		{
			srcBox.left     = 0;
			srcBox.right    = m_uWidth;
			srcBox.top      = 0;
			srcBox.bottom   = m_uHeight;
			srcBox.front    = 0;
			srcBox.back     = m_uDepth;
		}

		if( m_bBC )
		{
			srcBox.left /= 4;
			srcBox.right /= 4;
			srcBox.top /= 4;
			srcBox.bottom /= 4;
		}
			
		for( UINT z = 0; z < depth; ++z )
		{
			for( UINT y = 0; y < height; ++y )
			{
				for( UINT x = 0; x < width; ++x )
				{
					UINT totalBytes = m_bBC ? uBlockPitch : uElementWidth;

					for( UINT uByte = 0; uByte < totalBytes; ++uByte )
					{
						// ignore the X channel
						if( srcFormat == DXGI_FORMAT_D32_FLOAT_S8X24_UINT && uByte > 4 )
							continue;
						if( (srcFormat == DXGI_FORMAT_B8G8R8X8_UNORM || srcFormat == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) && uByte > 2 )
							continue;

						BYTE srcByte = ( (BYTE*) pSrcData )[ z * uSrcDepthPitch + y * uSrcRowPitch + x * uElementWidth + uByte ];
						BYTE originalByte = m_pResourceData[ uSrcSubres % RA_DATA_ELEMENTS ][ z * uExpectedDepth + y * uExpectedRow + x * uElementWidth + uByte ];

						if( m_bBC )
						{
							srcByte = ( (BYTE*) pSrcData )[ z * uSrcDepthPitch + y * uSrcRowPitch + x * uBlockPitch + uByte ];
							originalByte = m_pResourceData[ uSrcSubres % RA_DATA_ELEMENTS ][ z * uExpectedDepth + y * (uBlockPitch * width) + x * uBlockPitch + uByte ];
						}

						bool bWithinBox = m_bNULLSrcBox || WithinBox( srcBox, x, y, z );

						BYTE expectedByte;

						if(bWithinBox && (m_uSrcSubres == uSrcSubres))
						{
							if( m_bBC )
							{
								expectedByte = m_pTagData[ (z-srcBox.front) * uExpectedDepth + (y-srcBox.top) * (uBlockPitch * width) + (x-srcBox.left) * uBlockPitch + uByte ];
							}
							else
							{
								expectedByte = m_pTagData[ (z-srcBox.front) * uExpectedDepth + (y-srcBox.top) * uExpectedRow + (x-srcBox.left) * uElementWidth + uByte ];
							}
						}
						else
						{
							expectedByte = originalByte;
						}
						
						if( (expectedByte != srcByte) && !(bTestDiscard && !bWithinBox) && ( bPass || bLogAll ) )
						{
							WriteToLog( _T("Test case failed subresource %d, byte %d at (%d,%d,%d) does not match:\n\t\tResource: %x\n\t\tExpected: %x" ), uSrcSubres, uByte, x, y, z, srcByte, expectedByte );
							bPass = false;
						}
					}
				}
			}
		}
	}
	else if( m_AccessMethod == AM_MAP )
	{
		for( UINT z = 0; z < depth; ++z )
		{
			for( UINT y = 0; y < height; ++y )
			{
				for( UINT x = 0; x < width; ++x )
				{
					for( UINT uByte = 0; uByte < uElementWidth; ++uByte )
					{
						// ignore the X channel
						if( srcFormat == DXGI_FORMAT_D32_FLOAT_S8X24_UINT && uByte > 4 )
							continue;
						if( (srcFormat == DXGI_FORMAT_B8G8R8X8_UNORM || srcFormat == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) && uByte > 2 )
							continue;

						UINT uByteIndex = z * uSrcDepthPitch + y * uSrcRowPitch + x * uElementWidth + uByte;
						BYTE srcByte = ( (BYTE*) pSrcData )[ uByteIndex ];
						BYTE originalByte = m_pResourceData[ uSrcSubres % RA_DATA_ELEMENTS ][ z * uExpectedDepth + y * uExpectedRow + x * uElementWidth + uByte ];
						
						if( IsWriteMapType( m_MapType ) ) 
						{
							if( m_uSrcSubres == uSrcSubres && uByteIndex % 2 && srcByte != RESOURCE_TAG_ODD )
							{
								WriteToLog( _T("Test case failed subresource %d, byte %d at (%d,%d,%d) does not match:\n\t\tResource: %x\n\t\tExpected: %x" ), uSrcSubres, uByte, x, y, z, srcByte, RESOURCE_TAG_ODD );
								bPass = false;
							}
							
							if( m_bDoNotWait && m_uSrcSubres == uSrcSubres && uByteIndex % 2 == 0 && srcByte != RESOURCE_TAG_EVEN )
							{
								WriteToLog( _T("Test case failed subresource %d, byte %d at (%d,%d,%d) does not match:\n\t\tResource: %x\n\t\tExpected: %x" ), uSrcSubres, uByte, x, y, z, srcByte, RESOURCE_TAG_ODD );
								WriteToLog( _T("Likely cause of failure is Map returning S_OK too early when called with DO_NOT_WAIT") );
								bPass = false;
							}
						}
						else if( !m_bDoNotWait && srcByte != originalByte && m_uSrcSubres == uSrcSubres && ( bPass || bLogAll ) )
						{
							WriteToLog( _T("Test case failed subresource %d, byte %d at (%d,%d,%d) does not match:\n\t\tResource: %x\n\t\tExpected: %x" ), uSrcSubres, uByte, x, y, z, srcByte, originalByte );
							bPass = false;
						}
					}
				}
			}
		}
	}
	
	return bPass;
}

bool CResourceAccessTest::WithinBox( const D3D_TEST_BOX &box, UINT x, UINT y, UINT z ) const
{
	return 
		x >= box.left && x < box.right &&
		y >= box.top && y < box.bottom &&
		z >= box.front && z < box.back;
}

bool CResourceAccessTest::IsDenorm( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;
	
	return ((uValue & exp) == 0) && ((uValue & frac) != 0);
}

bool CResourceAccessTest::IsNaN( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0x7f800000) && ((uValue & frac) != 0);
}

bool CResourceAccessTest::GetMipDimensions( UINT uMipSlice, UINT &uWidth, UINT &uHeight, UINT &uDepth ) const
{
	if( uWidth == 0 || uHeight == 0 || uDepth == 0 )
		return false;

	bool bSmallestMip = false;

	UINT uCurrMip = 0;
	while( uCurrMip < uMipSlice )
	{
		if( bSmallestMip )
			return false;

		if( uWidth > 1 ) uWidth /= 2;
		if( uHeight > 1 ) uHeight /= 2;
		if( uDepth > 1 ) uDepth /= 2;
		uCurrMip++;

		if( uWidth == 1 && uHeight == 1 && uDepth == 1 )
			bSmallestMip = true;
	}

	return true;
}

bool CResourceAccessTest::WithinOneULP( float fReference, float fOutput )
{
	// WARNING:
	// This function does not correctly handle edge cases involving negative floats, zeros
	// and NaNs. If future test result discrepancies are discovered to have been masked by this function,
	// please start using CompareFloatULP() function in Framework\Core\FloatUtils.cpp which
	// does not suffer from these problems.

	if( fReference == fOutput )
		return true;

	UINT32 uRef = *( (UINT32*) &fReference );
	UINT32 uOut = *( (UINT32*) &fOutput );
	if( (int)max( uRef, uOut ) - (int)min( uRef, uOut ) <= 1 )
		return true;

	return false;
}

void CResourceAccessTest::CleanupTestCase()
{
	SAFE_RELEASE( m_pSrcResource );
	SAFE_RELEASE( m_pDestResource );
	SAFE_RELEASE( m_pSrcPreCopyResource );
	SAFE_RELEASE( m_pQuery );

	// The following are auto-released.
	m_pTilingSRV = NULL;
	m_pTilingRTV = NULL;
	m_pTilingVB = NULL;
}

void CResourceAccessTest::Cleanup()
{
	// release memory held by texture cache
	map<TexDataInfo, BYTE*>::iterator itr = m_TexCache.begin();
	while(itr != m_TexCache.end())
	{
		delete itr->second;
		itr++;
	}

	m_TexCache.clear();

	// Cleanup the Adapter Descriptor
	if( m_pAdapterDescriptor )
	{
		delete m_pAdapterDescriptor;
		m_pAdapterDescriptor = nullptr;
	}
	
	CD3D11Test::Cleanup();
}

DXGI_FORMAT CResourceAccessTest::GetTypelessFormat( DXGI_FORMAT format )
{
	DXGI_FORMAT typelessFmt = DXGI_FORMAT_UNKNOWN;

	switch( format )
	{
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:  
	case DXGI_FORMAT_R32G32B32A32_SINT:
		typelessFmt = DXGI_FORMAT_R32G32B32A32_TYPELESS;
		break;
	case DXGI_FORMAT_R32G32B32_FLOAT:    
	case DXGI_FORMAT_R32G32B32_UINT:     
	case DXGI_FORMAT_R32G32B32_SINT:     
		typelessFmt = DXGI_FORMAT_R32G32B32_TYPELESS;
		break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM: 
	case DXGI_FORMAT_R16G16B16A16_UINT:  
	case DXGI_FORMAT_R16G16B16A16_SNORM: 
	case DXGI_FORMAT_R16G16B16A16_SINT:  
		typelessFmt = DXGI_FORMAT_R16G16B16A16_TYPELESS;
		break;
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:        
	case DXGI_FORMAT_R32G32_SINT:        
		typelessFmt = DXGI_FORMAT_R32G32_TYPELESS;
		break;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:    
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: 
		typelessFmt = DXGI_FORMAT_R32G8X24_TYPELESS;
		break;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
		typelessFmt = DXGI_FORMAT_R10G10B10A2_TYPELESS;
		break;
	case DXGI_FORMAT_R11G11B10_FLOAT:
		typelessFmt = DXGI_FORMAT_R11G11B10_FLOAT;
		break;
	case DXGI_FORMAT_R8G8B8A8_UNORM:         
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:           
	case DXGI_FORMAT_R8G8B8A8_SNORM:          
	case DXGI_FORMAT_R8G8B8A8_SINT:
		typelessFmt = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		break;         
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:            
	case DXGI_FORMAT_R16G16_UINT:             
	case DXGI_FORMAT_R16G16_SNORM:            
	case DXGI_FORMAT_R16G16_SINT:
		typelessFmt = DXGI_FORMAT_R16G16_TYPELESS;
		break;           
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:             
	case DXGI_FORMAT_R32_UINT:              
	case DXGI_FORMAT_R32_SINT:
		typelessFmt = DXGI_FORMAT_R32_TYPELESS;
		break;           
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: 
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		typelessFmt = DXGI_FORMAT_R24G8_TYPELESS;
		break;            
	case DXGI_FORMAT_R8G8_UNORM:            
	case DXGI_FORMAT_R8G8_UINT:             
	case DXGI_FORMAT_R8G8_SNORM:            
	case DXGI_FORMAT_R8G8_SINT:
		typelessFmt = DXGI_FORMAT_R8G8_TYPELESS;
		break;             
	case DXGI_FORMAT_R16_FLOAT:             
	case DXGI_FORMAT_D16_UNORM:             
	case DXGI_FORMAT_R16_UNORM:             
	case DXGI_FORMAT_R16_UINT:              
	case DXGI_FORMAT_R16_SNORM:             
	case DXGI_FORMAT_R16_SINT:
		typelessFmt = DXGI_FORMAT_R16_TYPELESS;
		break;           
	case DXGI_FORMAT_R8_UNORM:          
	case DXGI_FORMAT_R8_UINT:         
	case DXGI_FORMAT_R8_SNORM:          
	case DXGI_FORMAT_R8_SINT:
		typelessFmt = DXGI_FORMAT_R8_TYPELESS;
		break;
	case DXGI_FORMAT_A8_UNORM:
		typelessFmt = DXGI_FORMAT_A8_UNORM;
		break;
	case DXGI_FORMAT_R1_UNORM:
		typelessFmt = DXGI_FORMAT_R1_UNORM;
		break;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		typelessFmt = DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
		break;
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
		typelessFmt = DXGI_FORMAT_R8G8_B8G8_UNORM;
		break;
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		typelessFmt = DXGI_FORMAT_G8R8_G8B8_UNORM;
		break;
	case DXGI_FORMAT_BC1_UNORM:         
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		typelessFmt = DXGI_FORMAT_BC1_TYPELESS;
		break;
	case DXGI_FORMAT_BC2_UNORM:         
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		typelessFmt = DXGI_FORMAT_BC2_TYPELESS;
		break;         
	case DXGI_FORMAT_BC3_UNORM:         
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		typelessFmt = DXGI_FORMAT_BC3_TYPELESS;
		break;         
	case DXGI_FORMAT_BC4_UNORM:         
	case DXGI_FORMAT_BC4_SNORM:
		typelessFmt = DXGI_FORMAT_BC4_TYPELESS;
		break;         
	case DXGI_FORMAT_BC5_UNORM:         
	case DXGI_FORMAT_BC5_SNORM:
		typelessFmt = DXGI_FORMAT_BC5_TYPELESS;
		break;
	case DXGI_FORMAT_BC6H_UF16:         
	case DXGI_FORMAT_BC6H_SF16:
		typelessFmt = DXGI_FORMAT_BC6H_TYPELESS;
		break;
	case DXGI_FORMAT_BC7_UNORM:         
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		typelessFmt = DXGI_FORMAT_BC7_TYPELESS;
		break;
	}

	return typelessFmt;
}

bool CResourceAccessTest::CheckMapIsByteAligned( D3D11_MAPPED_SUBRESOURCE*  pMapData )
{
	// Check the pointer is 16 byte aligned if not running feature level 9
	if( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_0 && !CheckNByteAlignedPointer(BYTE_ALIGNED_16, pMapData->pData ) )
	{
		WriteToLog( _T( "CResourceAccessTest::CheckMapIsByteAligned() - Address returned by map is not %u-byte aligned. Pointer = %#I64x"), BYTE_ALIGNED_16, (UINT64)(pMapData->pData));
		return false;
	}

	// If we got here, the ptr is aligned correctly to BYTE_ALIGNED_16
	return true;
}

bool CResourceAccessTest::CheckNByteAlignedPointer(UINT n, void * ptr)
{
	// This function returns true if the pointer is n-byte aligned. False if otherwise.
	// Note: A memory address a, is said to be n-byte aligned when n is a power of two and a is a multiple of n bytes.
	
	// Local variables
	bool result = true;

	// Check n is a power of two
	if ( 0 != ((n - 1) & n) )
		result = false;

	// Check the address is a multiple of n bytes
	if( (size_t)(ptr) % n != 0 )
		result = false;

	return result;
}

bool CResourceAccessTest::IsSupportedBufferScenario( const D3D11_BUFFER_DESC* bufferDesc )
{
	HRESULT hr;

	// Check the buffer description to see if we should be able to create the buffer with CPU access flags
	if ( bufferDesc->Usage == D3D11_USAGE_DEFAULT ) 
	{
		if ( bufferDesc->CPUAccessFlags != 0 && (bufferDesc->BindFlags == D3D11_BIND_SHADER_RESOURCE || bufferDesc->BindFlags == D3D11_BIND_UNORDERED_ACCESS || bufferDesc->BindFlags == (D3D11_BIND_SHADER_RESOURCE |  D3D11_BIND_UNORDERED_ACCESS)) )
		{
			D3D11_FEATURE_DATA_D3D11_OPTIONS1 Caps;
			ZeroMemory(&Caps, sizeof(Caps));
			if ( FAILED ( hr = GetDevice()->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, (void*)&Caps, sizeof(Caps)) ) )
			{
				WriteToLog( "CResourceAccessTest::IsSupportedBufferScenario() - CheckFeatureSupport() failed, hr = %s", D3DHResultToString(hr).c_str() );
				return false;
			}

			if ( Caps.MapOnDefaultBuffers == FALSE )
			{
				WriteToLog( "MapOnDefaultBuffers features not supported" );
				return false;
			}
		}
		else if ( bufferDesc->CPUAccessFlags != 0 )
		{
			return false;
		}
	}
	
	return true;
}

//---------------------------------------------------------------

/******************************
* CResourceAccessTestApp methods
******************************/

void CResourceAccessTestApp::InitOptionVariables( )
{
	CD3D11TestFramework::InitOptionVariables();
	int n = AddOptionCategory( _T( "Debug" ) );
	int nGeneral = AddOptionCategory( _T( "General" ) );

	UnRegisterOptionVariable( _T("srconly") );
	m_D3DOptions.SrcOnly = true;
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;

}

//---------------------------------------------------------------
extern DXGI_FORMAT MSAACheckFormats[];
extern DXGI_FORMAT MSAACheckFormats_Video[];
extern DXGI_FORMAT MSAACheckFormats_Video_Mips[];

bool CResourceAccessTestApp::InitTestGroups( )
{
	if( !ADD_TEST( _T( "ResourceAccess" ), CResourceAccessTest ) ) return false;
	if( !ADD_TEST_AND_DESC( _T( "CopyCompressed" ), CCopyCompressedTest, _T( DESC_TEST_COPYCOMPRESSED ) ) ) return false;
	if( !ADD_TEST( _T( "CopyMultisample" ), CCopyMultisampleTest ) ) return false;
	if( !ADD_TEST( _T( "Discard\\Resource" ), CDiscardResourceTest ) ) return false;
	if( !ADD_TEST( _T( "Discard\\ResourceView\\DepthStencil" ), CDiscardDSViewTest ) ) return false;
	if( !ADD_TEST( _T( "Discard\\ResourceView\\RenderTarget" ), CDiscardRTViewTest ) ) return false;
	if( !ADD_TEST( _T( "Discard\\ResourceView\\ShaderResource" ), CDiscardSRViewTest ) ) return false;
	if( !ADD_TEST( _T( "Discard\\ResourceView\\UnorderedAccess" ), CDiscardUAViewTest ) ) return false;
	if( !ADD_TEST( _T( "Discard\\ResourceView1\\DepthStencil" ), CDiscardDSView1Test ) ) return false;
	if( !ADD_TEST( _T( "Discard\\ResourceView1\\RenderTarget" ), CDiscardRTView1Test ) ) return false;
	if( !ADD_TEST( _T( "D2DTileableCopy" ), CD2DTileableCopyTest ) ) return false;
	if( !ADD_TEST( _T( "PlanarTextures" ), CPlanarTexturesTest ) ) return false;
	if( !ADD_TEST( _T("ClearView_1_Samples"), CClearViewTest<1 > ) ) return false; 

	//
	// Multisample
	//

	// need a device... or else there will be a ton (2000lbs) of skips
	if ( RESULT_PASS != SetupDXGIAndD3D() )
	{
		return true;
	}

	for( UINT uSampleCount = 2; uSampleCount <= 32; ++uSampleCount )
	{
		UINT uMaxLevel = 0;

		// Check the test's priority level
		if( 1 == GetExecutionPriorityLevel() )
		{
			// For pri 1, only use these sample counts
			if( 4 != uSampleCount &&
				2 != uSampleCount &&
				1 != uSampleCount )
			{
				// If it's not in the list, skip the sample count
				continue;
			}
		}
		else if( 2 == GetExecutionPriorityLevel() )
		{
			// For pri 2, only use these sample counts
			if( 8 != uSampleCount &&
				4 != uSampleCount &&
				2 != uSampleCount &&
				1 != uSampleCount )
			{
				// If it's not in the list, skip the sample count
				continue;
			}
		}

		// Check Multisample Quality Levels
		UINT32 SampleCounts = 0;
		for (UINT uFormat = 0; MSAACheckFormats[uFormat] != DXGI_FORMAT_UNKNOWN; ++uFormat)
		{
			UINT Bits = 0;
			if( SUCCEEDED( GetDevice()->CheckFormatSupport( MSAACheckFormats[uFormat], & Bits ) ) )
			{
				if ( Bits & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET )
				{
					UINT uLevels = 0;
					if( SUCCEEDED( GetDevice()->CheckMultisampleQualityLevels( MSAACheckFormats[uFormat], uSampleCount, &uLevels ) ) )
					{
						if ( uLevels  > 0 )
						{
							SampleCounts |= (1 << uSampleCount);
						}
					}
				}
			}
		}

		// We always add sample count 1 because it's not MSAA. See above. The other groups are only added if MSAA is supported for a particular format at the specified sample count.
		if ( SampleCounts & ( 1 << 2  ) ) if( !ADD_TEST( _T("ClearView_2_Samples"), CClearViewTest<2 > ) ) return false; 
		if ( SampleCounts & ( 1 << 3  ) ) if( !ADD_TEST( _T("ClearView_3_Samples"), CClearViewTest<3 > ) ) return false; 
		if ( SampleCounts & ( 1 << 4  ) ) if( !ADD_TEST( _T("ClearView_4_Samples"), CClearViewTest<4 > ) ) return false; 
		if ( SampleCounts & ( 1 << 5  ) ) if( !ADD_TEST( _T("ClearView_5_Samples"), CClearViewTest<5 > ) ) return false; 
		if ( SampleCounts & ( 1 << 6  ) ) if( !ADD_TEST( _T("ClearView_6_Samples"), CClearViewTest<6 > ) ) return false; 
		if ( SampleCounts & ( 1 << 7  ) ) if( !ADD_TEST( _T("ClearView_7_Samples"), CClearViewTest<7 > ) ) return false; 
		if ( SampleCounts & ( 1 << 8  ) ) if( !ADD_TEST( _T("ClearView_8_Samples"), CClearViewTest<8 > ) ) return false; 
		if ( SampleCounts & ( 1 << 9  ) ) if( !ADD_TEST( _T("ClearView_9_Samples"), CClearViewTest<9 > ) ) return false; 
		if ( SampleCounts & ( 1 << 10 ) ) if( !ADD_TEST( _T("ClearView_10Samples"), CClearViewTest<10> ) ) return false; 
		if ( SampleCounts & ( 1 << 11 ) ) if( !ADD_TEST( _T("ClearView_11Samples"), CClearViewTest<11> ) ) return false; 
		if ( SampleCounts & ( 1 << 12 ) ) if( !ADD_TEST( _T("ClearView_12Samples"), CClearViewTest<12> ) ) return false; 
		if ( SampleCounts & ( 1 << 13 ) ) if( !ADD_TEST( _T("ClearView_13Samples"), CClearViewTest<13> ) ) return false; 
		if ( SampleCounts & ( 1 << 14 ) ) if( !ADD_TEST( _T("ClearView_14Samples"), CClearViewTest<14> ) ) return false; 
		if ( SampleCounts & ( 1 << 15 ) ) if( !ADD_TEST( _T("ClearView_15Samples"), CClearViewTest<15> ) ) return false; 
		if ( SampleCounts & ( 1 << 16 ) ) if( !ADD_TEST( _T("ClearView_16Samples"), CClearViewTest<16> ) ) return false; 
		if ( SampleCounts & ( 1 << 17 ) ) if( !ADD_TEST( _T("ClearView_17Samples"), CClearViewTest<17> ) ) return false; 
		if ( SampleCounts & ( 1 << 18 ) ) if( !ADD_TEST( _T("ClearView_18Samples"), CClearViewTest<18> ) ) return false; 
		if ( SampleCounts & ( 1 << 19 ) ) if( !ADD_TEST( _T("ClearView_19Samples"), CClearViewTest<19> ) ) return false; 
		if ( SampleCounts & ( 1 << 20 ) ) if( !ADD_TEST( _T("ClearView_20Samples"), CClearViewTest<20> ) ) return false; 
		if ( SampleCounts & ( 1 << 21 ) ) if( !ADD_TEST( _T("ClearView_21Samples"), CClearViewTest<21> ) ) return false; 
		if ( SampleCounts & ( 1 << 22 ) ) if( !ADD_TEST( _T("ClearView_22Samples"), CClearViewTest<22> ) ) return false; 
		if ( SampleCounts & ( 1 << 23 ) ) if( !ADD_TEST( _T("ClearView_23Samples"), CClearViewTest<23> ) ) return false; 
		if ( SampleCounts & ( 1 << 24 ) ) if( !ADD_TEST( _T("ClearView_24Samples"), CClearViewTest<24> ) ) return false; 
		if ( SampleCounts & ( 1 << 25 ) ) if( !ADD_TEST( _T("ClearView_25Samples"), CClearViewTest<25> ) ) return false; 
		if ( SampleCounts & ( 1 << 26 ) ) if( !ADD_TEST( _T("ClearView_26Samples"), CClearViewTest<26> ) ) return false; 
		if ( SampleCounts & ( 1 << 27 ) ) if( !ADD_TEST( _T("ClearView_27Samples"), CClearViewTest<27> ) ) return false; 
		if ( SampleCounts & ( 1 << 28 ) ) if( !ADD_TEST( _T("ClearView_28Samples"), CClearViewTest<28> ) ) return false; 
		if ( SampleCounts & ( 1 << 29 ) ) if( !ADD_TEST( _T("ClearView_29Samples"), CClearViewTest<29> ) ) return false; 
		if ( SampleCounts & ( 1 << 30 ) ) if( !ADD_TEST( _T("ClearView_30Samples"), CClearViewTest<30> ) ) return false; 
		if ( SampleCounts & ( 1 << 31 ) ) if( !ADD_TEST( _T("ClearView_31Samples"), CClearViewTest<31> ) ) return false; 
#if 0
			if (( Bits & D3D11_FORMAT_SUPPORT_RENDER_TARGET ) ||
				( Bits & D3D11_FORMAT_SUPPORT_DECODER_OUTPUT ) ||
				( Bits & D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT ) ||
				( Bits & D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW ) )
			{
				UINT uLevels;
				if( SUCCEEDED( GetDevice()->CheckMultisampleQualityLevels( MSAACheckFormats[uFormat], uSampleCount, &uLevels ) ) )
				{
					if ( uLevels )
					{
						MSAAQualities Caps;
						Caps.Qualities = uLevels;
						Caps.Count = uSampleCount;
				
				
						TMSAACaps::iterator it = m_MSAACaps.find( MSAACheckFormats[uFormat] );
						if ( it == m_MSAACaps.end() )
						{
							MSAACapabilities caps;
							caps.FormatSupportBits = Bits;
							m_MSAACaps.insert( std::pair< DXGI_FORMAT, MSAACapabilities >( MSAACheckFormats[uFormat], caps ) );
							it = m_MSAACaps.find( MSAACheckFormats[uFormat] );
						}
						it->second.Qualities.push_back( Caps );
					}
				}
			}
#endif
	}

	CleanupD3D();
	CleanupDXGI();


//	BUG: This test group contains only one test case that needs to be rewritten and is causing stability issues.
//	See Win7 704810 and 715747
//	if( !ADD_TEST( _T( "ResourceFlush" ), CResourceFlushTest ) ) return false;
		
	return true;
}

