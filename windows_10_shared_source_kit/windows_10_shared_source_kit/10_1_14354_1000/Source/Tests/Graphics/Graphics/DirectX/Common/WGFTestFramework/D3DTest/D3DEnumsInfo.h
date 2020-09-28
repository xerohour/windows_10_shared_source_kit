//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3DEnumsInfo.h
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
#include "D3DWrappedTypes.h"

//-------------------------------------------------------------------------
// D3D_BIND_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D_BIND_FLAG D3DBindFlag );
extern bool WGFTESTAPI FromString( D3D_BIND_FLAG *pD3DBindFlag, const tstring &str );

// D3DBindFlagInfo - Structure that describes the properties of a D3DD3DBindFlagCAP
struct D3DBindFlagInfo
{
	D3D_BIND_FLAG D3DBindFlag;
	const TCHAR *szName;
};

// CD3DBindFlagInfoMap - Static singleton object used to lookup info given a D3DD3DBindFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3DBindFlagInfoMap
{
public:
	static const D3DBindFlagInfo &GetD3DBindFlagInfo( size_t nIndex ) { return m_D3DBindFlagInfoTable[nIndex]; }
		// Returns the D3DD3DBindFlagInfo at nIndex in the internal table.
	
	static const D3DBindFlagInfo *Find( D3D_BIND_FLAG D3DBindFlag );
	static const D3DBindFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3DBindFlagInfo associated with the given string value.
		
	static const size_t GetD3DBindFlagInfoTableSize();
		// Returns number on entries in m_D3DBindFlagInfoTable
		
private:
	CD3DBindFlagInfoMap();

	typedef	std::map< D3D_BIND_FLAG, const D3DBindFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3DBindFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3DBindFlagInfoMap m_Map;	
	friend class CD3DBindFlagInfoMap;
	
	static const D3DBindFlagInfo m_D3DBindFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D_CPU_ACCESS_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D_CPU_ACCESS_FLAG D3DCpuAccessFlag );
extern bool WGFTESTAPI FromString( D3D_CPU_ACCESS_FLAG *pD3DCpuAccessFlag, const tstring &str );

// D3DCpuAccessFlagInfo - Structure that describes the properties of a D3DD3DCpuAccessFlagCAP
struct D3DCpuAccessFlagInfo
{
	D3D_CPU_ACCESS_FLAG D3DCpuAccessFlag;
	const TCHAR *szName;
};

// CD3DCpuAccessFlagInfoMap - Static singleton object used to lookup info given a D3DD3DCpuAccessFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3DCpuAccessFlagInfoMap
{
public:
	static const D3DCpuAccessFlagInfo &GetD3DCpuAccessFlagInfo( size_t nIndex ) { return m_D3DCpuAccessFlagInfoTable[nIndex]; }
		// Returns the D3DD3DCpuAccessFlagInfo at nIndex in the internal table.
	
	static const D3DCpuAccessFlagInfo *Find( D3D_CPU_ACCESS_FLAG D3DCpuAccessFlag );
	static const D3DCpuAccessFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3DCpuAccessFlagInfo associated with the given string value.
		
	static const size_t GetD3DCpuAccessFlagInfoTableSize();
		// Returns number on entries in m_D3DCpuAccessFlagInfoTable
		
private:
	CD3DCpuAccessFlagInfoMap();

	typedef	std::map< D3D_CPU_ACCESS_FLAG, const D3DCpuAccessFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3DCpuAccessFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3DCpuAccessFlagInfoMap m_Map;	
	friend class CD3DCpuAccessFlagInfoMap;
	
	static const D3DCpuAccessFlagInfo m_D3DCpuAccessFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D_RESOURCE_MISC_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D_RESOURCE_MISC_FLAG D3DResourceMiscFlag );
extern bool WGFTESTAPI FromString( D3D_RESOURCE_MISC_FLAG *pD3DResourceMiscFlag, const tstring &str );

// D3DResourceMiscFlagInfo - Structure that describes the properties of a D3DD3DResourceMiscFlagCAP
struct D3DResourceMiscFlagInfo
{
	D3D_RESOURCE_MISC_FLAG D3DResourceMiscFlag;
	const TCHAR *szName;
};

// CD3DResourceMiscFlagInfoMap - Static singleton object used to lookup info given a D3DD3DResourceMiscFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3DResourceMiscFlagInfoMap
{
public:
	static const D3DResourceMiscFlagInfo &GetD3DResourceMiscFlagInfo( size_t nIndex ) { return m_D3DResourceMiscFlagInfoTable[nIndex]; }
		// Returns the D3DD3DResourceMiscFlagInfo at nIndex in the internal table.
	
	static const D3DResourceMiscFlagInfo *Find( D3D_RESOURCE_MISC_FLAG D3DResourceMiscFlag );
	static const D3DResourceMiscFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3DResourceMiscFlagInfo associated with the given string value.
		
	static const size_t GetD3DResourceMiscFlagInfoTableSize();
		// Returns number on entries in m_D3DResourceMiscFlagInfoTable
		
private:
	CD3DResourceMiscFlagInfoMap();

	typedef	std::map< D3D_RESOURCE_MISC_FLAG, const D3DResourceMiscFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3DResourceMiscFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3DResourceMiscFlagInfoMap m_Map;	
	friend class CD3DResourceMiscFlagInfoMap;
	
	static const D3DResourceMiscFlagInfo m_D3DResourceMiscFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D_MAP_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D_MAP_FLAG D3DMapFlag );
extern bool WGFTESTAPI FromString( D3D_MAP_FLAG *pD3DMapFlag, const tstring &str );

// D3DMapFlagInfo - Structure that describes the properties of a D3DD3DMapFlagCAP
struct D3DMapFlagInfo
{
	D3D_MAP_FLAG D3DMapFlag;
	const TCHAR *szName;
};

// CD3DMapFlagInfoMap - Static singleton object used to lookup info given a D3DD3DMapFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3DMapFlagInfoMap
{
public:
	static const D3DMapFlagInfo &GetD3DMapFlagInfo( size_t nIndex ) { return m_D3DMapFlagInfoTable[nIndex]; }
		// Returns the D3DD3DMapFlagInfo at nIndex in the internal table.
	
	static const D3DMapFlagInfo *Find( D3D_MAP_FLAG D3DMapFlag );
	static const D3DMapFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3DMapFlagInfo associated with the given string value.
		
	static const size_t GetD3DMapFlagInfoTableSize();
		// Returns number on entries in m_D3DMapFlagInfoTable
		
private:
	CD3DMapFlagInfoMap();

	typedef	std::map< D3D_MAP_FLAG, const D3DMapFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3DMapFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3DMapFlagInfoMap m_Map;	
	friend class CD3DMapFlagInfoMap;
	
	static const D3DMapFlagInfo m_D3DMapFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D_RAISE_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D_RAISE_FLAG D3DRaiseFlag );
extern bool WGFTESTAPI FromString( D3D_RAISE_FLAG *pD3DRaiseFlag, const tstring &str );

// D3DRaiseFlagInfo - Structure that describes the properties of a D3DD3DRaiseFlagCAP
struct D3DRaiseFlagInfo
{
	D3D_RAISE_FLAG D3DRaiseFlag;
	const TCHAR *szName;
};

// CD3DRaiseFlagInfoMap - Static singleton object used to lookup info given a D3DD3DRaiseFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3DRaiseFlagInfoMap
{
public:
	static const D3DRaiseFlagInfo &GetD3DRaiseFlagInfo( size_t nIndex ) { return m_D3DRaiseFlagInfoTable[nIndex]; }
		// Returns the D3DD3DRaiseFlagInfo at nIndex in the internal table.
	
	static const D3DRaiseFlagInfo *Find( D3D_RAISE_FLAG D3DRaiseFlag );
	static const D3DRaiseFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3DRaiseFlagInfo associated with the given string value.
		
	static const size_t GetD3DRaiseFlagInfoTableSize();
		// Returns number on entries in m_D3DRaiseFlagInfoTable
		
private:
	CD3DRaiseFlagInfoMap();

	typedef	std::map< D3D_RAISE_FLAG, const D3DRaiseFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3DRaiseFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3DRaiseFlagInfoMap m_Map;	
	friend class CD3DRaiseFlagInfoMap;
	
	static const D3DRaiseFlagInfo m_D3DRaiseFlagInfoTable[];
};


//-------------------------------------------------------------------------
// D3D_CLEAR_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D_CLEAR_FLAG D3DClearFlag );
extern bool WGFTESTAPI FromString( D3D_CLEAR_FLAG *pD3DClearFlag, const tstring &str );

// D3DClearFlagInfo - Structure that describes the properties of a D3DD3DClearFlagCAP
struct D3DClearFlagInfo
{
	D3D_CLEAR_FLAG D3DClearFlag;
	const TCHAR *szName;
};

// CD3DClearFlagInfoMap - Static singleton object used to lookup info given a D3DD3DClearFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3DClearFlagInfoMap
{
public:
	static const D3DClearFlagInfo &GetD3DClearFlagInfo( size_t nIndex ) { return m_D3DClearFlagInfoTable[nIndex]; }
		// Returns the D3DD3DClearFlagInfo at nIndex in the internal table.
	
	static const D3DClearFlagInfo *Find( D3D_CLEAR_FLAG D3DClearFlag );
	static const D3DClearFlagInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DD3DClearFlagInfo associated with the given string value.
		
	static const size_t GetD3DClearFlagInfoTableSize();
		// Returns number on entries in m_D3DClearFlagInfoTable
		
private:
	CD3DClearFlagInfoMap();

	typedef	std::map< D3D_CLEAR_FLAG, const D3DClearFlagInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const D3DClearFlagInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CD3DClearFlagInfoMap m_Map;	
	friend class CD3DClearFlagInfoMap;
	
	static const D3DClearFlagInfo m_D3DClearFlagInfoTable[];
};


