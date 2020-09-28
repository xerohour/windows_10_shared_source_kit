//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D11EnumsInfo.h
//* 
//* Description: 
//*     Declares classes used to categorize enums
//*
//*****************************************************************************

#pragma once

#include "WGFTestCore.h"
#include "d3d11.h"

//-------------------------------------------------------------------------
// D3D11_BIND_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_BIND_FLAG D3D11BindFlag );
extern bool WGFTESTAPI FromString( D3D11_BIND_FLAG *pD3D11BindFlag, const tstring &str );

// D3D11BindFlagInfo - Structure that describes the properties of a D3DD3D11BindFlagCAP
struct D3D11BindFlagInfo
{
	D3D11_BIND_FLAG D3D11BindFlag;
	const TCHAR *szName;
};

// CD3D11BindFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11BindFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11BindFlagInfoMap
{
public:
	static const D3D11BindFlagInfo &GetD3D11BindFlagInfo( size_t nIndex ) { return m_D3D11BindFlagInfoTable[nIndex]; }
		// Returns the D3DD3D11BindFlagInfo at nIndex in the internal table.
	
	static const D3D11BindFlagInfo *Find( D3D11_BIND_FLAG D3D11BindFlag );
	static const D3D11BindFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D11BindFlagInfo associated with the given string value.
		
	static const size_t GetD3D11BindFlagInfoTableSize();
		// Returns number on entries in m_D3D11BindFlagInfoTable
		
private:
	CD3D11BindFlagInfoMap();

	typedef	std::map< D3D11_BIND_FLAG, const D3D11BindFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D11BindFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D11BindFlagInfoMap m_Map;	
	friend class CD3D11BindFlagInfoMap;
	
	static const D3D11BindFlagInfo m_D3D11BindFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D11_CPU_ACCESS_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_CPU_ACCESS_FLAG D3D11CpuAccessFlag );
extern bool WGFTESTAPI FromString( D3D11_CPU_ACCESS_FLAG *pD3D11CpuAccessFlag, const tstring &str );

// D3D11CpuAccessFlagInfo - Structure that describes the properties of a D3DD3D11CpuAccessFlagCAP
struct D3D11CpuAccessFlagInfo
{
	D3D11_CPU_ACCESS_FLAG D3D11CpuAccessFlag;
	const TCHAR *szName;
};

// CD3D11CpuAccessFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11CpuAccessFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11CpuAccessFlagInfoMap
{
public:
	static const D3D11CpuAccessFlagInfo &GetD3D11CpuAccessFlagInfo( size_t nIndex ) { return m_D3D11CpuAccessFlagInfoTable[nIndex]; }
		// Returns the D3DD3D11CpuAccessFlagInfo at nIndex in the internal table.
	
	static const D3D11CpuAccessFlagInfo *Find( D3D11_CPU_ACCESS_FLAG D3D11CpuAccessFlag );
	static const D3D11CpuAccessFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D11CpuAccessFlagInfo associated with the given string value.
		
	static const size_t GetD3D11CpuAccessFlagInfoTableSize();
		// Returns number on entries in m_D3D11CpuAccessFlagInfoTable
		
private:
	CD3D11CpuAccessFlagInfoMap();

	typedef	std::map< D3D11_CPU_ACCESS_FLAG, const D3D11CpuAccessFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D11CpuAccessFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D11CpuAccessFlagInfoMap m_Map;	
	friend class CD3D11CpuAccessFlagInfoMap;
	
	static const D3D11CpuAccessFlagInfo m_D3D11CpuAccessFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D11_RESOURCE_MISC_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_RESOURCE_MISC_FLAG D3D11ResourceMiscFlag );
extern bool WGFTESTAPI FromString( D3D11_RESOURCE_MISC_FLAG *pD3D11ResourceMiscFlag, const tstring &str );

// D3D11ResourceMiscFlagInfo - Structure that describes the properties of a D3DD3D11ResourceMiscFlagCAP
struct D3D11ResourceMiscFlagInfo
{
	D3D11_RESOURCE_MISC_FLAG D3D11ResourceMiscFlag;
	const TCHAR *szName;
};

// CD3D11ResourceMiscFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11ResourceMiscFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11ResourceMiscFlagInfoMap
{
public:
	static const D3D11ResourceMiscFlagInfo &GetD3D11ResourceMiscFlagInfo( size_t nIndex ) { return m_D3D11ResourceMiscFlagInfoTable[nIndex]; }
		// Returns the D3DD3D11ResourceMiscFlagInfo at nIndex in the internal table.
	
	static const D3D11ResourceMiscFlagInfo *Find( D3D11_RESOURCE_MISC_FLAG D3D11ResourceMiscFlag );
	static const D3D11ResourceMiscFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D11ResourceMiscFlagInfo associated with the given string value.
		
	static const size_t GetD3D11ResourceMiscFlagInfoTableSize();
		// Returns number on entries in m_D3D11ResourceMiscFlagInfoTable
		
private:
	CD3D11ResourceMiscFlagInfoMap();

	typedef	std::map< D3D11_RESOURCE_MISC_FLAG, const D3D11ResourceMiscFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D11ResourceMiscFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D11ResourceMiscFlagInfoMap m_Map;	
	friend class CD3D11ResourceMiscFlagInfoMap;
	
	static const D3D11ResourceMiscFlagInfo m_D3D11ResourceMiscFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D11_MAP_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_MAP_FLAG D3D11MapFlag );
extern bool WGFTESTAPI FromString( D3D11_MAP_FLAG *pD3D11MapFlag, const tstring &str );

// D3D11MapFlagInfo - Structure that describes the properties of a D3DD3D11MapFlagCAP
struct D3D11MapFlagInfo
{
	D3D11_MAP_FLAG D3D11MapFlag;
	const TCHAR *szName;
};

// CD3D11MapFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11MapFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11MapFlagInfoMap
{
public:
	static const D3D11MapFlagInfo &GetD3D11MapFlagInfo( size_t nIndex ) { return m_D3D11MapFlagInfoTable[nIndex]; }
		// Returns the D3DD3D11MapFlagInfo at nIndex in the internal table.
	
	static const D3D11MapFlagInfo *Find( D3D11_MAP_FLAG D3D11MapFlag );
	static const D3D11MapFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D11MapFlagInfo associated with the given string value.
		
	static const size_t GetD3D11MapFlagInfoTableSize();
		// Returns number on entries in m_D3D11MapFlagInfoTable
		
private:
	CD3D11MapFlagInfoMap();

	typedef	std::map< D3D11_MAP_FLAG, const D3D11MapFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D11MapFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D11MapFlagInfoMap m_Map;	
	friend class CD3D11MapFlagInfoMap;
	
	static const D3D11MapFlagInfo m_D3D11MapFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D11_RAISE_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_RAISE_FLAG D3D11RaiseFlag );
extern bool WGFTESTAPI FromString( D3D11_RAISE_FLAG *pD3D11RaiseFlag, const tstring &str );

// D3D11RaiseFlagInfo - Structure that describes the properties of a D3DD3D11RaiseFlagCAP
struct D3D11RaiseFlagInfo
{
	D3D11_RAISE_FLAG D3D11RaiseFlag;
	const TCHAR *szName;
};

// CD3D11RaiseFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11RaiseFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11RaiseFlagInfoMap
{
public:
	static const D3D11RaiseFlagInfo &GetD3D11RaiseFlagInfo( size_t nIndex ) { return m_D3D11RaiseFlagInfoTable[nIndex]; }
		// Returns the D3DD3D11RaiseFlagInfo at nIndex in the internal table.
	
	static const D3D11RaiseFlagInfo *Find( D3D11_RAISE_FLAG D3D11RaiseFlag );
	static const D3D11RaiseFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D11RaiseFlagInfo associated with the given string value.
		
	static const size_t GetD3D11RaiseFlagInfoTableSize();
		// Returns number on entries in m_D3D11RaiseFlagInfoTable
		
private:
	CD3D11RaiseFlagInfoMap();

	typedef	std::map< D3D11_RAISE_FLAG, const D3D11RaiseFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D11RaiseFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D11RaiseFlagInfoMap m_Map;	
	friend class CD3D11RaiseFlagInfoMap;
	
	static const D3D11RaiseFlagInfo m_D3D11RaiseFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D11_CLEAR_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_CLEAR_FLAG D3D11ClearFlag );
extern bool WGFTESTAPI FromString( D3D11_CLEAR_FLAG *pD3D11ClearFlag, const tstring &str );

// D3D11ClearFlagInfo - Structure that describes the properties of a D3DD3D11ClearFlagCAP
struct D3D11ClearFlagInfo
{
	D3D11_CLEAR_FLAG D3D11ClearFlag;
	const TCHAR *szName;
};

// CD3D11ClearFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11ClearFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11ClearFlagInfoMap
{
public:
	static const D3D11ClearFlagInfo &GetD3D11ClearFlagInfo( size_t nIndex ) { return m_D3D11ClearFlagInfoTable[nIndex]; }
		// Returns the D3DD3D11ClearFlagInfo at nIndex in the internal table.
	
	static const D3D11ClearFlagInfo *Find( D3D11_CLEAR_FLAG D3D11ClearFlag );
	static const D3D11ClearFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D11ClearFlagInfo associated with the given string value.
		
	static const size_t GetD3D11ClearFlagInfoTableSize();
		// Returns number on entries in m_D3D11ClearFlagInfoTable
		
private:
	CD3D11ClearFlagInfoMap();

	typedef	std::map< D3D11_CLEAR_FLAG, const D3D11ClearFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D11ClearFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D11ClearFlagInfoMap m_Map;	
	friend class CD3D11ClearFlagInfoMap;
	
	static const D3D11ClearFlagInfo m_D3D11ClearFlagInfoTable[];
};


