//////////////////////////////////////////////////////////////////////////
//  DXGIValueSets.cpp
//  created:	2004/08/30
//
//  purpose: Defines classes used to categorize DXGI_FORMAT values.
//////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "DXGIValueSets.h"

CFormatValueSet::CFormatValueSet( CFilterHandleCommonSource< DXGI_FORMAT > filter )
{
	DXGI_FORMAT Format;
	
	if( filter.IsValid() )
		filter->SetSource( &Format );
	
	int count = CFormatInfoMap::GetDXGIFormatInfoTableSize();

	for( int i = 0; i < count; i++ )
	{
		Format = CFormatInfoMap::GetDXGIFormatInfo( i ).Format;

		if( !filter.IsValid() || filter->Match() )
			AddValue( Format );		
	}
}


CFormatValueSet::CFormatValueSet( DWORD dwIncludeFlags, DWORD dwExcludeFlags )
{
	int count = CFormatInfoMap::GetDXGIFormatInfoTableSize();

	for( int i = 0; i < count; i++ )
	{
		DXGI_FORMAT Format = CFormatInfoMap::GetDXGIFormatInfo( i ).Format;
		DWORD dwFlags = GetFlags( Format );
		
		if( ( !dwIncludeFlags || ( dwIncludeFlags & dwFlags ) ) &&
			( !dwExcludeFlags || !( dwExcludeFlags & dwFlags ) ) )
			AddValue( Format );
	}
}

DWORD GetFlags( DXGI_FORMAT Format )
{
	const DXGIFormatInfo *pfi = CFormatInfoMap::Find( Format );
	
	if( pfi )
		return pfi->dwFlags;
	
	return 0;
}

UINT GetBitsPerElement( DXGI_FORMAT Format )
{
	const DXGIFormatInfo *pfi = CFormatInfoMap::Find( Format );
	
	if( pfi )
		return pfi->uBitsPerElement;
	
	return 0;
}

UINT GetBitsPerComponent( UINT uComponent, DXGI_FORMAT Format )
{
	const DXGIFormatInfo *pfi = CFormatInfoMap::Find( Format );
	
	if( pfi )
		return pfi->pBitsPerComponent[ uComponent ];
	
	return 0;
}

UINT  GetBitsPerElementVideo( DXGI_FORMAT Format )
{
	switch(Format)
	{
	// 64 bits per pixel
	case DXGI_FORMAT_Y416: 
		return 64; 
	// 32 bits per pixel
	case DXGI_FORMAT_Y410: 
	case DXGI_FORMAT_Y210: 
	case DXGI_FORMAT_Y216:
	case DXGI_FORMAT_AYUV:
		return 32; 
	// 24 bits per pixel
	case DXGI_FORMAT_P010: 
	case DXGI_FORMAT_P016: 
		return 24; 
	// 16 bits per pixel
	case DXGI_FORMAT_YUY2:
		return 16; 
	// 12 bits per pixel
	case DXGI_FORMAT_NV12:  
	case DXGI_FORMAT_NV11:
	case DXGI_FORMAT_420_OPAQUE:
		return 12; 

	// Legacy substream formats
	case DXGI_FORMAT_AI44:
	case DXGI_FORMAT_IA44:
	case DXGI_FORMAT_P8:
		return 8;
	case DXGI_FORMAT_A8P8:
		return 16;
	
	default:
		return 0;
	}
}

tstring WGFTESTAPI ToString( DXGI_FORMAT Format )
{
	const DXGIFormatInfo *pFormatInfo = CFormatInfoMap::Find( Format );
	
	if( pFormatInfo )
		return tstring( pFormatInfo->szDesc );
	else
		return tstring( _T( "(Unknown format type)" ) );
}

bool WGFTESTAPI FromString( DXGI_FORMAT *pFormat, const tstring &str )
{
	const DXGIFormatInfo *pFormatInfo = CFormatInfoMap::Find( str );

	if( pFormatInfo )
	{
		*pFormat = pFormatInfo->Format;
		return true;
	}

	*pFormat = DXGI_FORMAT_UNKNOWN;
	return false;
}



//
// CFormatInfoMap
//

// Instantiate the singleton
CFormatInfoMap CFormatInfoMap::m_Map;

// Construct the lookup table.
CFormatInfoMap::CFormatInfoMap()
{
	int count = CFormatInfoMap::GetDXGIFormatInfoTableSize();

	for( int i = 0; i < count; i++ )
	{
		m_ByValue.insert( ByValue::value_type( m_FormatInfoTable[i].Format, &m_FormatInfoTable[i] ) );
		m_ByDesc.insert( ByDesc::value_type( m_FormatInfoTable[i].szDesc, &m_FormatInfoTable[i] ) );
	}

	// Register flag values.
	REGISTER_FLAG_VALUE( FF_RED, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_GREEN, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_BLUE, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_ALPHA, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_DEPTH, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_STENCIL, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_R, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_G, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_B, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_A, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_RG, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_RGB, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_RGBA, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_D, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_S, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_DS, DXGI_FORMAT );

	REGISTER_FLAG_VALUE( FF_FLOAT, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_TYPELESS, DXGI_FORMAT );	
	REGISTER_FLAG_VALUE( FF_FLOAT, DXGI_FORMAT );	
	REGISTER_FLAG_VALUE( FF_UINT, DXGI_FORMAT );		
	REGISTER_FLAG_VALUE( FF_SINT, DXGI_FORMAT );		
	REGISTER_FLAG_VALUE( FF_UNORM, DXGI_FORMAT );	
	REGISTER_FLAG_VALUE( FF_SNORM, DXGI_FORMAT );	
	REGISTER_FLAG_VALUE( FF_SRGB, DXGI_FORMAT );		
	REGISTER_FLAG_VALUE( FF_SHAREDEXP, DXGI_FORMAT );
	REGISTER_FLAG_VALUE( FF_STRIDED, DXGI_FORMAT );	
	REGISTER_FLAG_VALUE( FF_BC, DXGI_FORMAT );		
	REGISTER_FLAG_VALUE( FF_E, DXGI_FORMAT );		
	REGISTER_FLAG_VALUE( FF_X, DXGI_FORMAT );		

	REGISTER_FLAG_VALUE( FF_ALL, DXGI_FORMAT );
}

const DXGIFormatInfo *CFormatInfoMap::Find( DXGI_FORMAT Format )
{
	ByValue::const_iterator it = m_Map.m_ByValue.find( Format );
	if( it == m_Map.m_ByValue.end() )
		return NULL;
	else
		return (*it).second;
}

const DXGIFormatInfo *CFormatInfoMap::Find( const tstring &Desc )
{
	ByDesc::const_iterator it = m_Map.m_ByDesc.find( Desc );
	if( it == m_Map.m_ByDesc.end() )
		return NULL;
	else
		return (*it).second;
}



