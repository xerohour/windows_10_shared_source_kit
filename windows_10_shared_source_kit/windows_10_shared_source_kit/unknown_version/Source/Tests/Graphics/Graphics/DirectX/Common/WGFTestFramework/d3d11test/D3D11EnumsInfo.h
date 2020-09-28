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
#include "d3d11_2.h"

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
class CD3D11BindFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11BindFlagInfoMap();

	static size_t GetD3D11BindFlagInfoTableSize();

	static const D3D11BindFlagInfo CD3D11BindFlagInfoMap::m_D3D11BindFlagInfoTable[];
	
	static CD3D11BindFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11BindFlagInfoMap m_this;
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
class CD3D11CpuAccessFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11CpuAccessFlagInfoMap();

	static size_t GetD3D11CpuAccessFlagInfoTableSize();

	static const D3D11CpuAccessFlagInfo CD3D11CpuAccessFlagInfoMap::m_D3D11CpuAccessFlagInfoTable[];
	
	static CD3D11CpuAccessFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11CpuAccessFlagInfoMap m_this;
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
class CD3D11ResourceMiscFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11ResourceMiscFlagInfoMap();

	static size_t GetD3D11ResourceMiscFlagInfoTableSize();

	static const D3D11ResourceMiscFlagInfo CD3D11ResourceMiscFlagInfoMap::m_D3D11ResourceMiscFlagInfoTable[];
	
	static CD3D11ResourceMiscFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11ResourceMiscFlagInfoMap m_this;
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
class CD3D11MapFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11MapFlagInfoMap();

	static size_t GetD3D11MapFlagInfoTableSize();

	static const D3D11MapFlagInfo CD3D11MapFlagInfoMap::m_D3D11MapFlagInfoTable[];
	
	static CD3D11MapFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11MapFlagInfoMap m_this;
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
class CD3D11RaiseFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11RaiseFlagInfoMap();

	static size_t GetD3D11RaiseFlagInfoTableSize();

	static const D3D11RaiseFlagInfo CD3D11RaiseFlagInfoMap::m_D3D11RaiseFlagInfoTable[];
	
	static CD3D11RaiseFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11RaiseFlagInfoMap m_this;
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
class CD3D11ClearFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11ClearFlagInfoMap();

	static size_t GetD3D11ClearFlagInfoTableSize();

	static const D3D11ClearFlagInfo CD3D11ClearFlagInfoMap::m_D3D11ClearFlagInfoTable[];
	
	static CD3D11ClearFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11ClearFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_BUFFEREX_SRV_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_BUFFEREX_SRV_FLAG D3D11BufferexSrvFlag );
extern bool WGFTESTAPI FromString( D3D11_BUFFEREX_SRV_FLAG *pD3D11BufferexSrvFlag, const tstring &str );

// D3D11BufferexSrvFlagInfo - Structure that describes the properties of a D3DD3D11BufferexSrvFlagCAP
struct D3D11BufferexSrvFlagInfo
{
	D3D11_BUFFEREX_SRV_FLAG D3D11BufferexSrvFlag;
	const TCHAR *szName;
};

// CD3D11BufferexSrvFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11BufferexSrvFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11BufferexSrvFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11BufferexSrvFlagInfoMap();

	static size_t GetD3D11BufferexSrvFlagInfoTableSize();

	static const D3D11BufferexSrvFlagInfo CD3D11BufferexSrvFlagInfoMap::m_D3D11BufferexSrvFlagInfoTable[];
	
	static CD3D11BufferexSrvFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11BufferexSrvFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_DSV_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_DSV_FLAG D3D11DsvFlag );
extern bool WGFTESTAPI FromString( D3D11_DSV_FLAG *pD3D11DsvFlag, const tstring &str );

// D3D11DsvFlagInfo - Structure that describes the properties of a D3DD3D11DsvFlagCAP
struct D3D11DsvFlagInfo
{
	D3D11_DSV_FLAG D3D11DsvFlag;
	const TCHAR *szName;
};

// CD3D11DsvFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11DsvFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11DsvFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11DsvFlagInfoMap();

	static size_t GetD3D11DsvFlagInfoTableSize();

	static const D3D11DsvFlagInfo CD3D11DsvFlagInfoMap::m_D3D11DsvFlagInfoTable[];
	
	static CD3D11DsvFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11DsvFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_BUFFER_UAV_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_BUFFER_UAV_FLAG D3D11BufferUavFlag );
extern bool WGFTESTAPI FromString( D3D11_BUFFER_UAV_FLAG *pD3D11BufferUavFlag, const tstring &str );

// D3D11BufferUavFlagInfo - Structure that describes the properties of a D3DD3D11BufferUavFlagCAP
struct D3D11BufferUavFlagInfo
{
	D3D11_BUFFER_UAV_FLAG D3D11BufferUavFlag;
	const TCHAR *szName;
};

// CD3D11BufferUavFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11BufferUavFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11BufferUavFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11BufferUavFlagInfoMap();

	static size_t GetD3D11BufferUavFlagInfoTableSize();

	static const D3D11BufferUavFlagInfo CD3D11BufferUavFlagInfoMap::m_D3D11BufferUavFlagInfoTable[];
	
	static CD3D11BufferUavFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11BufferUavFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_ASYNC_GETDATA_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_ASYNC_GETDATA_FLAG D3D11AsyncGetdataFlag );
extern bool WGFTESTAPI FromString( D3D11_ASYNC_GETDATA_FLAG *pD3D11AsyncGetdataFlag, const tstring &str );

// D3D11AsyncGetdataFlagInfo - Structure that describes the properties of a D3DD3D11AsyncGetdataFlagCAP
struct D3D11AsyncGetdataFlagInfo
{
	D3D11_ASYNC_GETDATA_FLAG D3D11AsyncGetdataFlag;
	const TCHAR *szName;
};

// CD3D11AsyncGetdataFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11AsyncGetdataFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11AsyncGetdataFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11AsyncGetdataFlagInfoMap();

	static size_t GetD3D11AsyncGetdataFlagInfoTableSize();

	static const D3D11AsyncGetdataFlagInfo CD3D11AsyncGetdataFlagInfoMap::m_D3D11AsyncGetdataFlagInfoTable[];
	
	static CD3D11AsyncGetdataFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11AsyncGetdataFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_QUERY_MISC_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_QUERY_MISC_FLAG D3D11QueryMiscFlag );
extern bool WGFTESTAPI FromString( D3D11_QUERY_MISC_FLAG *pD3D11QueryMiscFlag, const tstring &str );

// D3D11QueryMiscFlagInfo - Structure that describes the properties of a D3DD3D11QueryMiscFlagCAP
struct D3D11QueryMiscFlagInfo
{
	D3D11_QUERY_MISC_FLAG D3D11QueryMiscFlag;
	const TCHAR *szName;
};

// CD3D11QueryMiscFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11QueryMiscFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11QueryMiscFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11QueryMiscFlagInfoMap();

	static size_t GetD3D11QueryMiscFlagInfoTableSize();

	static const D3D11QueryMiscFlagInfo CD3D11QueryMiscFlagInfoMap::m_D3D11QueryMiscFlagInfoTable[];
	
	static CD3D11QueryMiscFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11QueryMiscFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_CREATE_DEVICE_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_CREATE_DEVICE_FLAG D3D11CreateDeviceFlag );
extern bool WGFTESTAPI FromString( D3D11_CREATE_DEVICE_FLAG *pD3D11CreateDeviceFlag, const tstring &str );

// D3D11CreateDeviceFlagInfo - Structure that describes the properties of a D3DD3D11CreateDeviceFlagCAP
struct D3D11CreateDeviceFlagInfo
{
	D3D11_CREATE_DEVICE_FLAG D3D11CreateDeviceFlag;
	const TCHAR *szName;
};

// CD3D11CreateDeviceFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11CreateDeviceFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11CreateDeviceFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11CreateDeviceFlagInfoMap();

	static size_t GetD3D11CreateDeviceFlagInfoTableSize();

	static const D3D11CreateDeviceFlagInfo CD3D11CreateDeviceFlagInfoMap::m_D3D11CreateDeviceFlagInfoTable[];
	
	static CD3D11CreateDeviceFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11CreateDeviceFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_COPY_FLAGS related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_COPY_FLAGS D3D11CopyFlags );
extern bool WGFTESTAPI FromString( D3D11_COPY_FLAGS *pD3D11CopyFlags, const tstring &str );

// D3D11CopyFlagsInfo - Structure that describes the properties of a D3DD3D11CopyFlagsCAP
struct D3D11CopyFlagsInfo
{
	D3D11_COPY_FLAGS D3D11CopyFlags;
	const TCHAR *szName;
};

// CD3D11CopyFlagsInfoMap - Static singleton object used to lookup info given a D3DD3D11CopyFlags.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11CopyFlagsInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11CopyFlagsInfoMap();

	static size_t GetD3D11CopyFlagsInfoTableSize();

	static const D3D11CopyFlagsInfo CD3D11CopyFlagsInfoMap::m_D3D11CopyFlagsInfoTable[];
	
	static CD3D11CopyFlagsInfoMap &Get() { return m_this; }

protected:
	static CD3D11CopyFlagsInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_1_CREATE_DEVICE_CONTEXT_STATE_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_1_CREATE_DEVICE_CONTEXT_STATE_FLAG D3D111CreateDeviceContextStateFlag );
extern bool WGFTESTAPI FromString( D3D11_1_CREATE_DEVICE_CONTEXT_STATE_FLAG *pD3D111CreateDeviceContextStateFlag, const tstring &str );

// D3D111CreateDeviceContextStateFlagInfo - Structure that describes the properties of a D3DD3D111CreateDeviceContextStateFlagCAP
struct D3D111CreateDeviceContextStateFlagInfo
{
	D3D11_1_CREATE_DEVICE_CONTEXT_STATE_FLAG D3D111CreateDeviceContextStateFlag;
	const TCHAR *szName;
};

// CD3D111CreateDeviceContextStateFlagInfoMap - Static singleton object used to lookup info given a D3DD3D111CreateDeviceContextStateFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D111CreateDeviceContextStateFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D111CreateDeviceContextStateFlagInfoMap();

	static size_t GetD3D111CreateDeviceContextStateFlagInfoTableSize();

	static const D3D111CreateDeviceContextStateFlagInfo CD3D111CreateDeviceContextStateFlagInfoMap::m_D3D111CreateDeviceContextStateFlagInfoTable[];
	
	static CD3D111CreateDeviceContextStateFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D111CreateDeviceContextStateFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_TILE_MAPPING_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_TILE_MAPPING_FLAG D3D11TileMappingFlag );
extern bool WGFTESTAPI FromString( D3D11_TILE_MAPPING_FLAG *pD3D11TileMappingFlag, const tstring &str );

// D3D11TileMappingFlagInfo - Structure that describes the properties of a D3DD3D11TileMappingFlagCAP
struct D3D11TileMappingFlagInfo
{
	D3D11_TILE_MAPPING_FLAG D3D11TileMappingFlag;
	const TCHAR *szName;
};

// CD3D11TileMappingFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11TileMappingFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11TileMappingFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11TileMappingFlagInfoMap();

	static size_t GetD3D11TileMappingFlagInfoTableSize();

	static const D3D11TileMappingFlagInfo CD3D11TileMappingFlagInfoMap::m_D3D11TileMappingFlagInfoTable[];
	
	static CD3D11TileMappingFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11TileMappingFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_TILE_RANGE_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_TILE_RANGE_FLAG D3D11TileRangeFlag );
extern bool WGFTESTAPI FromString( D3D11_TILE_RANGE_FLAG *pD3D11TileRangeFlag, const tstring &str );

// D3D11TileRangeFlagInfo - Structure that describes the properties of a D3DD3D11TileRangeFlagCAP
struct D3D11TileRangeFlagInfo
{
	D3D11_TILE_RANGE_FLAG D3D11TileRangeFlag;
	const TCHAR *szName;
};

// CD3D11TileRangeFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11TileRangeFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11TileRangeFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11TileRangeFlagInfoMap();

	static size_t GetD3D11TileRangeFlagInfoTableSize();

	static const D3D11TileRangeFlagInfo CD3D11TileRangeFlagInfoMap::m_D3D11TileRangeFlagInfoTable[];
	
	static CD3D11TileRangeFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11TileRangeFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_FLAG D3D11CheckMultisampleQualityLevelsFlag );
extern bool WGFTESTAPI FromString( D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_FLAG *pD3D11CheckMultisampleQualityLevelsFlag, const tstring &str );

// D3D11CheckMultisampleQualityLevelsFlagInfo - Structure that describes the properties of a D3DD3D11CheckMultisampleQualityLevelsFlagCAP
struct D3D11CheckMultisampleQualityLevelsFlagInfo
{
	D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_FLAG D3D11CheckMultisampleQualityLevelsFlag;
	const TCHAR *szName;
};

// CD3D11CheckMultisampleQualityLevelsFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11CheckMultisampleQualityLevelsFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11CheckMultisampleQualityLevelsFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11CheckMultisampleQualityLevelsFlagInfoMap();

	static size_t GetD3D11CheckMultisampleQualityLevelsFlagInfoTableSize();

	static const D3D11CheckMultisampleQualityLevelsFlagInfo CD3D11CheckMultisampleQualityLevelsFlagInfoMap::m_D3D11CheckMultisampleQualityLevelsFlagInfoTable[];
	
	static CD3D11CheckMultisampleQualityLevelsFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11CheckMultisampleQualityLevelsFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D11_TILE_COPY_FLAG related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D11_TILE_COPY_FLAG D3D11TileCopyFlag );
extern bool WGFTESTAPI FromString( D3D11_TILE_COPY_FLAG *pD3D11TileCopyFlag, const tstring &str );

// D3D11TileCopyFlagInfo - Structure that describes the properties of a D3DD3D11TileCopyFlagCAP
struct D3D11TileCopyFlagInfo
{
	D3D11_TILE_COPY_FLAG D3D11TileCopyFlag;
	const TCHAR *szName;
};

// CD3D11TileCopyFlagInfoMap - Static singleton object used to lookup info given a D3DD3D11TileCopyFlag.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3D11TileCopyFlagInfoMap : public CNamedFlagsTable32
{
public:
	CD3D11TileCopyFlagInfoMap();

	static size_t GetD3D11TileCopyFlagInfoTableSize();

	static const D3D11TileCopyFlagInfo CD3D11TileCopyFlagInfoMap::m_D3D11TileCopyFlagInfoTable[];
	
	static CD3D11TileCopyFlagInfoMap &Get() { return m_this; }

protected:
	static CD3D11TileCopyFlagInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D_SHADER_VARIABLE_FLAGS related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D_SHADER_VARIABLE_FLAGS D3DShaderVariableFlags );
extern bool WGFTESTAPI FromString( D3D_SHADER_VARIABLE_FLAGS *pD3DShaderVariableFlags, const tstring &str );

// D3DShaderVariableFlagsInfo - Structure that describes the properties of a D3DD3DShaderVariableFlagsCAP
struct D3DShaderVariableFlagsInfo
{
	D3D_SHADER_VARIABLE_FLAGS D3DShaderVariableFlags;
	const TCHAR *szName;
};

// CD3DShaderVariableFlagsInfoMap - Static singleton object used to lookup info given a D3DD3DShaderVariableFlags.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3DShaderVariableFlagsInfoMap : public CNamedFlagsTable32
{
public:
	CD3DShaderVariableFlagsInfoMap();

	static size_t GetD3DShaderVariableFlagsInfoTableSize();

	static const D3DShaderVariableFlagsInfo CD3DShaderVariableFlagsInfoMap::m_D3DShaderVariableFlagsInfoTable[];
	
	static CD3DShaderVariableFlagsInfoMap &Get() { return m_this; }

protected:
	static CD3DShaderVariableFlagsInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D_SHADER_INPUT_FLAGS related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D_SHADER_INPUT_FLAGS D3DShaderInputFlags );
extern bool WGFTESTAPI FromString( D3D_SHADER_INPUT_FLAGS *pD3DShaderInputFlags, const tstring &str );

// D3DShaderInputFlagsInfo - Structure that describes the properties of a D3DD3DShaderInputFlagsCAP
struct D3DShaderInputFlagsInfo
{
	D3D_SHADER_INPUT_FLAGS D3DShaderInputFlags;
	const TCHAR *szName;
};

// CD3DShaderInputFlagsInfoMap - Static singleton object used to lookup info given a D3DD3DShaderInputFlags.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3DShaderInputFlagsInfoMap : public CNamedFlagsTable32
{
public:
	CD3DShaderInputFlagsInfoMap();

	static size_t GetD3DShaderInputFlagsInfoTableSize();

	static const D3DShaderInputFlagsInfo CD3DShaderInputFlagsInfoMap::m_D3DShaderInputFlagsInfoTable[];
	
	static CD3DShaderInputFlagsInfoMap &Get() { return m_this; }

protected:
	static CD3DShaderInputFlagsInfoMap m_this;
};


//-------------------------------------------------------------------------
// D3D_SHADER_CBUFFER_FLAGS related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( D3D_SHADER_CBUFFER_FLAGS D3DShaderCbufferFlags );
extern bool WGFTESTAPI FromString( D3D_SHADER_CBUFFER_FLAGS *pD3DShaderCbufferFlags, const tstring &str );

// D3DShaderCbufferFlagsInfo - Structure that describes the properties of a D3DD3DShaderCbufferFlagsCAP
struct D3DShaderCbufferFlagsInfo
{
	D3D_SHADER_CBUFFER_FLAGS D3DShaderCbufferFlags;
	const TCHAR *szName;
};

// CD3DShaderCbufferFlagsInfoMap - Static singleton object used to lookup info given a D3DD3DShaderCbufferFlags.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CD3DShaderCbufferFlagsInfoMap : public CNamedFlagsTable32
{
public:
	CD3DShaderCbufferFlagsInfoMap();

	static size_t GetD3DShaderCbufferFlagsInfoTableSize();

	static const D3DShaderCbufferFlagsInfo CD3DShaderCbufferFlagsInfoMap::m_D3DShaderCbufferFlagsInfoTable[];
	
	static CD3DShaderCbufferFlagsInfoMap &Get() { return m_this; }

protected:
	static CD3DShaderCbufferFlagsInfoMap m_this;
};


