//////////////////////////////////////////////////////////////////////////
//  DXGIValueSets2.h
//  created:	2004/08/30
//
//  purpose: Provides classes that implement common ValueSets and filters
//			 related to DXGI.
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <windows.h>
#include <dxgitype.h>

#include <stringutils.h>
#include <DXGIStrings.h>

#undef new
#undef delete

#include <WGFTestCore.h>


enum DXGIFORMAT_FLAG
{
	FF_NONE			= 0x00000000,
	FF_RED			= 0x00000001,
	FF_GREEN		= 0x00000002,
	FF_BLUE			= 0x00000004,
	FF_ALPHA		= 0x00000008,
	FF_DEPTH		= 0x00000020,
	FF_STENCIL		= 0x00000040,
	FF_R			= FF_RED,
	FF_G			= FF_GREEN,
	FF_B			= FF_BLUE,
	FF_A			= FF_ALPHA,
	FF_RG			= FF_R | FF_G,
	FF_RGB			= FF_R | FF_G | FF_B,
	FF_RGBA			= FF_R | FF_G | FF_B | FF_ALPHA,
	FF_D			= FF_DEPTH,
	FF_S			= FF_STENCIL,
	FF_DS			= FF_D | FF_S,
	
	FF_TYPELESS		= 0x00000100,
	FF_FLOAT		= 0x00000200,
	FF_UINT			= 0x00000400,
	FF_SINT			= 0x00000800,
	FF_UNORM		= 0x00001000,
	FF_SNORM		= 0x00002000,
	FF_SRGB			= 0x00004000,
	FF_SHAREDEXP	= 0x00008000,
	FF_STRIDED		= 0x00010000,
	FF_BC			= 0x00020000,
	FF_X			= 0x00040000,
	FF_E			= 0x00080000,
	FF_VIDEO		= 0x00100000,
	
	// BUGBUG - TODO: Add intended usage flags (buffer, texture, index, vertex, etc)...

	FF_ALL			= 0xffffffff,
};

//temp hack until generator is fixed
#define FF_BGR FF_RGB
#define FF_BGRA FF_RGBA

//////////////////////////////////////////////////////////////////////////
// DXGIFormatInfo
//
// Structure that describes the properties of a DXGI_FORMAT.
//////////////////////////////////////////////////////////////////////////
struct DXGIFormatInfo
{
	DXGI_FORMAT Format;
	UINT		pBitsPerComponent[4];
	UINT		uBitsPerElement;
	DWORD		dwFlags;
	const TCHAR *szDesc;
};

//////////////////////////////////////////////////////////////////////////
// CFormatInfoMap
//
// Static singleton object used to lookup a format information
// given a DXGI_FORMAT.
// 
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED.
//////////////////////////////////////////////////////////////////////////
class CFormatInfoMap
{
public:
	static const DXGIFormatInfo &GetDXGIFormatInfo( int nIndex ) { return m_FormatInfoTable[nIndex]; }
		// Returns the DXGIFormatInfo at nIndex in the internal table.
	
	static const DXGIFormatInfo *Find( DXGI_FORMAT Format );
	static const DXGIFormatInfo *Find( const tstring &Desc );
		// Returns a pointer to the DXGIFormatInfo associated with the 
		// given format.  This is the only method directly accessible to
		// developers.

	static const size_t GetDXGIFormatInfoTableSize();
		// Returns number on entries in m_D3D10FillModeInfoTable
		
private:
	CFormatInfoMap();

	typedef	std::map< DXGI_FORMAT, const DXGIFormatInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const DXGIFormatInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CFormatInfoMap m_Map;	
	friend class CFormatInfoMap;
	
	static const DXGIFormatInfo m_FormatInfoTable[];
};


extern DWORD GetFlags( DXGI_FORMAT Format );
extern UINT  GetBitsPerElement( DXGI_FORMAT Format );
extern UINT  GetBitsPerComponent( UINT uComponent, DXGI_FORMAT Format );
extern UINT  GetBitsPerElementVideo( DXGI_FORMAT Format );

//////////////////////////////////////////////////////////////////////////
// Common typedefs
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CFormatValueSet
//
// Defines a DXGI_FORMAT ValueSet based on the include/exclude flags
// provided at creation time.
//////////////////////////////////////////////////////////////////////////
class CFormatValueSet : public CUserValueSet< DXGI_FORMAT >
{
public:
	CFormatValueSet( CFilterHandleCommonSource< DXGI_FORMAT > filter = NULL );
	CFormatValueSet( DWORD dwIncludeFlags, DWORD dwExcludeFlags );
};

inline CFormatValueSet *FormatValueSet( CFilterHandleCommonSource< DXGI_FORMAT > filter = NULL )
{
	return new CFormatValueSet( filter );
}

inline CFormatValueSet *FormatValueSet( DWORD dwIncludeFlags, DWORD dwExcludeFlags )
{
	return new CFormatValueSet( dwIncludeFlags, dwExcludeFlags );
}
