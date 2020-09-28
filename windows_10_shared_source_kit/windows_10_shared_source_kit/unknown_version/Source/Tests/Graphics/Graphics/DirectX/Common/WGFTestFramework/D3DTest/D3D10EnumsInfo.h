//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D10EnumsInfo.h
//* 
//* Description: 
//*     Declares classes used to categorize enums
//*
//*****************************************************************************

// Ignore Prefast warnings 26060 (Range Precondition violation) and 28930 (Unused assignment of pointer variable)
#pragma warning( disable: 26060 )
#pragma warning( disable: 28930 )

#pragma once

#include "WGFTestCore.h"
#include "d3d10_1.h"

//-------------------------------------------------------------------------
// D3D10_BIND_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D10_BIND_FLAG D3D10BindFlag );
extern bool WGFTESTAPI FromString( D3D10_BIND_FLAG *pD3D10BindFlag, const tstring &str );

// D3D10BindFlagInfo - Structure that describes the properties of a D3DD3D10BindFlagCAP
struct D3D10BindFlagInfo
{
	D3D10_BIND_FLAG D3D10BindFlag;
	const TCHAR *szName;
};

// CD3D10BindFlagInfoMap - Static singleton object used to lookup info given a D3DD3D10BindFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D10BindFlagInfoMap
{
public:
	static const D3D10BindFlagInfo &GetD3D10BindFlagInfo( size_t nIndex ) { return m_D3D10BindFlagInfoTable[nIndex]; }
		// Returns the D3DD3D10BindFlagInfo at nIndex in the internal table.
	
	static const D3D10BindFlagInfo *Find( D3D10_BIND_FLAG D3D10BindFlag );
	static const D3D10BindFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D10BindFlagInfo associated with the given string value.
		
	static const size_t GetD3D10BindFlagInfoTableSize();
		// Returns number on entries in m_D3D10BindFlagInfoTable
		
private:
	CD3D10BindFlagInfoMap();

	typedef	std::map< D3D10_BIND_FLAG, const D3D10BindFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D10BindFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D10BindFlagInfoMap m_Map;	
	friend class CD3D10BindFlagInfoMap;
	
	static const D3D10BindFlagInfo m_D3D10BindFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D10_CPU_ACCESS_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D10_CPU_ACCESS_FLAG D3D10CpuAccessFlag );
extern bool WGFTESTAPI FromString( D3D10_CPU_ACCESS_FLAG *pD3D10CpuAccessFlag, const tstring &str );

// D3D10CpuAccessFlagInfo - Structure that describes the properties of a D3DD3D10CpuAccessFlagCAP
struct D3D10CpuAccessFlagInfo
{
	D3D10_CPU_ACCESS_FLAG D3D10CpuAccessFlag;
	const TCHAR *szName;
};

// CD3D10CpuAccessFlagInfoMap - Static singleton object used to lookup info given a D3DD3D10CpuAccessFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D10CpuAccessFlagInfoMap
{
public:
	static const D3D10CpuAccessFlagInfo &GetD3D10CpuAccessFlagInfo( size_t nIndex ) { return m_D3D10CpuAccessFlagInfoTable[nIndex]; }
		// Returns the D3DD3D10CpuAccessFlagInfo at nIndex in the internal table.
	
	static const D3D10CpuAccessFlagInfo *Find( D3D10_CPU_ACCESS_FLAG D3D10CpuAccessFlag );
	static const D3D10CpuAccessFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D10CpuAccessFlagInfo associated with the given string value.
		
	static const size_t GetD3D10CpuAccessFlagInfoTableSize();
		// Returns number on entries in m_D3D10CpuAccessFlagInfoTable
		
private:
	CD3D10CpuAccessFlagInfoMap();

	typedef	std::map< D3D10_CPU_ACCESS_FLAG, const D3D10CpuAccessFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D10CpuAccessFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D10CpuAccessFlagInfoMap m_Map;	
	friend class CD3D10CpuAccessFlagInfoMap;
	
	static const D3D10CpuAccessFlagInfo m_D3D10CpuAccessFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D10_RESOURCE_MISC_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D10_RESOURCE_MISC_FLAG D3D10ResourceMiscFlag );
extern bool WGFTESTAPI FromString( D3D10_RESOURCE_MISC_FLAG *pD3D10ResourceMiscFlag, const tstring &str );

// D3D10ResourceMiscFlagInfo - Structure that describes the properties of a D3DD3D10ResourceMiscFlagCAP
struct D3D10ResourceMiscFlagInfo
{
	D3D10_RESOURCE_MISC_FLAG D3D10ResourceMiscFlag;
	const TCHAR *szName;
};

// CD3D10ResourceMiscFlagInfoMap - Static singleton object used to lookup info given a D3DD3D10ResourceMiscFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D10ResourceMiscFlagInfoMap
{
public:
	static const D3D10ResourceMiscFlagInfo &GetD3D10ResourceMiscFlagInfo( size_t nIndex ) { return m_D3D10ResourceMiscFlagInfoTable[nIndex]; }
		// Returns the D3DD3D10ResourceMiscFlagInfo at nIndex in the internal table.
	
	static const D3D10ResourceMiscFlagInfo *Find( D3D10_RESOURCE_MISC_FLAG D3D10ResourceMiscFlag );
	static const D3D10ResourceMiscFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D10ResourceMiscFlagInfo associated with the given string value.
		
	static const size_t GetD3D10ResourceMiscFlagInfoTableSize();
		// Returns number on entries in m_D3D10ResourceMiscFlagInfoTable
		
private:
	CD3D10ResourceMiscFlagInfoMap();

	typedef	std::map< D3D10_RESOURCE_MISC_FLAG, const D3D10ResourceMiscFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D10ResourceMiscFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D10ResourceMiscFlagInfoMap m_Map;	
	friend class CD3D10ResourceMiscFlagInfoMap;
	
	static const D3D10ResourceMiscFlagInfo m_D3D10ResourceMiscFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D10_MAP_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D10_MAP_FLAG D3D10MapFlag );
extern bool WGFTESTAPI FromString( D3D10_MAP_FLAG *pD3D10MapFlag, const tstring &str );

// D3D10MapFlagInfo - Structure that describes the properties of a D3DD3D10MapFlagCAP
struct D3D10MapFlagInfo
{
	D3D10_MAP_FLAG D3D10MapFlag;
	const TCHAR *szName;
};

// CD3D10MapFlagInfoMap - Static singleton object used to lookup info given a D3DD3D10MapFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D10MapFlagInfoMap
{
public:
	static const D3D10MapFlagInfo &GetD3D10MapFlagInfo( size_t nIndex ) { return m_D3D10MapFlagInfoTable[nIndex]; }
		// Returns the D3DD3D10MapFlagInfo at nIndex in the internal table.
	
	static const D3D10MapFlagInfo *Find( D3D10_MAP_FLAG D3D10MapFlag );
	static const D3D10MapFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D10MapFlagInfo associated with the given string value.
		
	static const size_t GetD3D10MapFlagInfoTableSize();
		// Returns number on entries in m_D3D10MapFlagInfoTable
		
private:
	CD3D10MapFlagInfoMap();

	typedef	std::map< D3D10_MAP_FLAG, const D3D10MapFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D10MapFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D10MapFlagInfoMap m_Map;	
	friend class CD3D10MapFlagInfoMap;
	
	static const D3D10MapFlagInfo m_D3D10MapFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D10_RAISE_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D10_RAISE_FLAG D3D10RaiseFlag );
extern bool WGFTESTAPI FromString( D3D10_RAISE_FLAG *pD3D10RaiseFlag, const tstring &str );

// D3D10RaiseFlagInfo - Structure that describes the properties of a D3DD3D10RaiseFlagCAP
struct D3D10RaiseFlagInfo
{
	D3D10_RAISE_FLAG D3D10RaiseFlag;
	const TCHAR *szName;
};

// CD3D10RaiseFlagInfoMap - Static singleton object used to lookup info given a D3DD3D10RaiseFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D10RaiseFlagInfoMap
{
public:
	static const D3D10RaiseFlagInfo &GetD3D10RaiseFlagInfo( size_t nIndex ) { return m_D3D10RaiseFlagInfoTable[nIndex]; }
		// Returns the D3DD3D10RaiseFlagInfo at nIndex in the internal table.
	
	static const D3D10RaiseFlagInfo *Find( D3D10_RAISE_FLAG D3D10RaiseFlag );
	static const D3D10RaiseFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D10RaiseFlagInfo associated with the given string value.
		
	static const size_t GetD3D10RaiseFlagInfoTableSize();
		// Returns number on entries in m_D3D10RaiseFlagInfoTable
		
private:
	CD3D10RaiseFlagInfoMap();

	typedef	std::map< D3D10_RAISE_FLAG, const D3D10RaiseFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D10RaiseFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D10RaiseFlagInfoMap m_Map;	
	friend class CD3D10RaiseFlagInfoMap;
	
	static const D3D10RaiseFlagInfo m_D3D10RaiseFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D10_CLEAR_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D10_CLEAR_FLAG D3D10ClearFlag );
extern bool WGFTESTAPI FromString( D3D10_CLEAR_FLAG *pD3D10ClearFlag, const tstring &str );

// D3D10ClearFlagInfo - Structure that describes the properties of a D3DD3D10ClearFlagCAP
struct D3D10ClearFlagInfo
{
	D3D10_CLEAR_FLAG D3D10ClearFlag;
	const TCHAR *szName;
};

// CD3D10ClearFlagInfoMap - Static singleton object used to lookup info given a D3DD3D10ClearFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D10ClearFlagInfoMap
{
public:
	static const D3D10ClearFlagInfo &GetD3D10ClearFlagInfo( size_t nIndex ) { return m_D3D10ClearFlagInfoTable[nIndex]; }
		// Returns the D3DD3D10ClearFlagInfo at nIndex in the internal table.
	
	static const D3D10ClearFlagInfo *Find( D3D10_CLEAR_FLAG D3D10ClearFlag );
	static const D3D10ClearFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3D10ClearFlagInfo associated with the given string value.
		
	static const size_t GetD3D10ClearFlagInfoTableSize();
		// Returns number on entries in m_D3D10ClearFlagInfoTable
		
private:
	CD3D10ClearFlagInfoMap();

	typedef	std::map< D3D10_CLEAR_FLAG, const D3D10ClearFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3D10ClearFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3D10ClearFlagInfoMap m_Map;	
	friend class CD3D10ClearFlagInfoMap;
	
	static const D3D10ClearFlagInfo m_D3D10ClearFlagInfoTable[];
};


