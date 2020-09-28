//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D11EnumsInfo.cpp
//* 
//* Description: 
//*     Defines classes used to categorize enums
//*
//*****************************************************************************

#include "pch.h"
#include "D3D11EnumsInfo.h"

//////////////////////////////////////////////////////////////////////////
// D3D11_BIND_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_BIND_FLAG D3D11BindFlag )
{
	return CD3D11BindFlagInfoMap::Get().ToString((UINT) D3D11BindFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_BIND_FLAG *pD3D11BindFlag, const tstring &str )
{
	return CD3D11BindFlagInfoMap::Get().FromString((UINT *) pD3D11BindFlag, str);
}

//------------------------------------------------------------------------

const D3D11BindFlagInfo CD3D11BindFlagInfoMap::m_D3D11BindFlagInfoTable[] =
{
    { (D3D11_BIND_FLAG) (0), _T( "0" ) },
    { D3D11_BIND_VERTEX_BUFFER, _T("D3D11_BIND_VERTEX_BUFFER") },
    { D3D11_BIND_INDEX_BUFFER, _T("D3D11_BIND_INDEX_BUFFER") },
    { D3D11_BIND_CONSTANT_BUFFER, _T("D3D11_BIND_CONSTANT_BUFFER") },
    { D3D11_BIND_SHADER_RESOURCE, _T("D3D11_BIND_SHADER_RESOURCE") },
    { D3D11_BIND_STREAM_OUTPUT, _T("D3D11_BIND_STREAM_OUTPUT") },
    { D3D11_BIND_RENDER_TARGET, _T("D3D11_BIND_RENDER_TARGET") },
    { D3D11_BIND_DEPTH_STENCIL, _T("D3D11_BIND_DEPTH_STENCIL") },
    { D3D11_BIND_UNORDERED_ACCESS, _T("D3D11_BIND_UNORDERED_ACCESS") },
    { D3D11_BIND_DECODER, _T("D3D11_BIND_DECODER") },
    { D3D11_BIND_VIDEO_ENCODER, _T("D3D11_BIND_VIDEO_ENCODER") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11BindFlagInfoMap CD3D11BindFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11BindFlagInfoMap::GetD3D11BindFlagInfoTableSize()
{
	return _countof(CD3D11BindFlagInfoMap::m_D3D11BindFlagInfoTable);
}

// Construct the lookup table.
CD3D11BindFlagInfoMap::CD3D11BindFlagInfoMap()
{
	size_t count = GetD3D11BindFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11BindFlagInfoTable[i].szName, m_D3D11BindFlagInfoTable[i].D3D11BindFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_CPU_ACCESS_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_CPU_ACCESS_FLAG D3D11CpuAccessFlag )
{
	return CD3D11CpuAccessFlagInfoMap::Get().ToString((UINT) D3D11CpuAccessFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_CPU_ACCESS_FLAG *pD3D11CpuAccessFlag, const tstring &str )
{
	return CD3D11CpuAccessFlagInfoMap::Get().FromString((UINT *) pD3D11CpuAccessFlag, str);
}

//------------------------------------------------------------------------

const D3D11CpuAccessFlagInfo CD3D11CpuAccessFlagInfoMap::m_D3D11CpuAccessFlagInfoTable[] =
{
    { (D3D11_CPU_ACCESS_FLAG) (0), _T( "0" ) },
    { D3D11_CPU_ACCESS_WRITE, _T("D3D11_CPU_ACCESS_WRITE") },
    { D3D11_CPU_ACCESS_READ, _T("D3D11_CPU_ACCESS_READ") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11CpuAccessFlagInfoMap CD3D11CpuAccessFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11CpuAccessFlagInfoMap::GetD3D11CpuAccessFlagInfoTableSize()
{
	return _countof(CD3D11CpuAccessFlagInfoMap::m_D3D11CpuAccessFlagInfoTable);
}

// Construct the lookup table.
CD3D11CpuAccessFlagInfoMap::CD3D11CpuAccessFlagInfoMap()
{
	size_t count = GetD3D11CpuAccessFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11CpuAccessFlagInfoTable[i].szName, m_D3D11CpuAccessFlagInfoTable[i].D3D11CpuAccessFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_RESOURCE_MISC_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_RESOURCE_MISC_FLAG D3D11ResourceMiscFlag )
{
	return CD3D11ResourceMiscFlagInfoMap::Get().ToString((UINT) D3D11ResourceMiscFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_RESOURCE_MISC_FLAG *pD3D11ResourceMiscFlag, const tstring &str )
{
	return CD3D11ResourceMiscFlagInfoMap::Get().FromString((UINT *) pD3D11ResourceMiscFlag, str);
}

//------------------------------------------------------------------------

const D3D11ResourceMiscFlagInfo CD3D11ResourceMiscFlagInfoMap::m_D3D11ResourceMiscFlagInfoTable[] =
{
    { (D3D11_RESOURCE_MISC_FLAG) (0), _T( "0" ) },
    { D3D11_RESOURCE_MISC_GENERATE_MIPS, _T("D3D11_RESOURCE_MISC_GENERATE_MIPS") },
    { D3D11_RESOURCE_MISC_SHARED, _T("D3D11_RESOURCE_MISC_SHARED") },
    { D3D11_RESOURCE_MISC_TEXTURECUBE, _T("D3D11_RESOURCE_MISC_TEXTURECUBE") },
    { D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS, _T("D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS") },
    { D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, _T("D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS") },
    { D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, _T("D3D11_RESOURCE_MISC_BUFFER_STRUCTURED") },
    { D3D11_RESOURCE_MISC_RESOURCE_CLAMP, _T("D3D11_RESOURCE_MISC_RESOURCE_CLAMP") },
    { D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX, _T("D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX") },
    { D3D11_RESOURCE_MISC_GDI_COMPATIBLE, _T("D3D11_RESOURCE_MISC_GDI_COMPATIBLE") },
    { D3D11_RESOURCE_MISC_SHARED_NTHANDLE, _T("D3D11_RESOURCE_MISC_SHARED_NTHANDLE") },
    { D3D11_RESOURCE_MISC_RESTRICTED_CONTENT, _T("D3D11_RESOURCE_MISC_RESTRICTED_CONTENT") },
    { D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE, _T("D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE") },
    { D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE_DRIVER, _T("D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE_DRIVER") },
    { D3D11_RESOURCE_MISC_GUARDED, _T("D3D11_RESOURCE_MISC_GUARDED") },
    { D3D11_RESOURCE_MISC_TILE_POOL, _T("D3D11_RESOURCE_MISC_TILE_POOL") },
    { D3D11_RESOURCE_MISC_TILED, _T("D3D11_RESOURCE_MISC_TILED") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11ResourceMiscFlagInfoMap CD3D11ResourceMiscFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11ResourceMiscFlagInfoMap::GetD3D11ResourceMiscFlagInfoTableSize()
{
	return _countof(CD3D11ResourceMiscFlagInfoMap::m_D3D11ResourceMiscFlagInfoTable);
}

// Construct the lookup table.
CD3D11ResourceMiscFlagInfoMap::CD3D11ResourceMiscFlagInfoMap()
{
	size_t count = GetD3D11ResourceMiscFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11ResourceMiscFlagInfoTable[i].szName, m_D3D11ResourceMiscFlagInfoTable[i].D3D11ResourceMiscFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_MAP_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_MAP_FLAG D3D11MapFlag )
{
	return CD3D11MapFlagInfoMap::Get().ToString((UINT) D3D11MapFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_MAP_FLAG *pD3D11MapFlag, const tstring &str )
{
	return CD3D11MapFlagInfoMap::Get().FromString((UINT *) pD3D11MapFlag, str);
}

//------------------------------------------------------------------------

const D3D11MapFlagInfo CD3D11MapFlagInfoMap::m_D3D11MapFlagInfoTable[] =
{
    { (D3D11_MAP_FLAG) (0), _T( "0" ) },
    { D3D11_MAP_FLAG_DO_NOT_WAIT, _T("D3D11_MAP_FLAG_DO_NOT_WAIT") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11MapFlagInfoMap CD3D11MapFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11MapFlagInfoMap::GetD3D11MapFlagInfoTableSize()
{
	return _countof(CD3D11MapFlagInfoMap::m_D3D11MapFlagInfoTable);
}

// Construct the lookup table.
CD3D11MapFlagInfoMap::CD3D11MapFlagInfoMap()
{
	size_t count = GetD3D11MapFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11MapFlagInfoTable[i].szName, m_D3D11MapFlagInfoTable[i].D3D11MapFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_RAISE_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_RAISE_FLAG D3D11RaiseFlag )
{
	return CD3D11RaiseFlagInfoMap::Get().ToString((UINT) D3D11RaiseFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_RAISE_FLAG *pD3D11RaiseFlag, const tstring &str )
{
	return CD3D11RaiseFlagInfoMap::Get().FromString((UINT *) pD3D11RaiseFlag, str);
}

//------------------------------------------------------------------------

const D3D11RaiseFlagInfo CD3D11RaiseFlagInfoMap::m_D3D11RaiseFlagInfoTable[] =
{
    { (D3D11_RAISE_FLAG) (0), _T( "0" ) },
    { D3D11_RAISE_FLAG_DRIVER_INTERNAL_ERROR, _T("D3D11_RAISE_FLAG_DRIVER_INTERNAL_ERROR") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11RaiseFlagInfoMap CD3D11RaiseFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11RaiseFlagInfoMap::GetD3D11RaiseFlagInfoTableSize()
{
	return _countof(CD3D11RaiseFlagInfoMap::m_D3D11RaiseFlagInfoTable);
}

// Construct the lookup table.
CD3D11RaiseFlagInfoMap::CD3D11RaiseFlagInfoMap()
{
	size_t count = GetD3D11RaiseFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11RaiseFlagInfoTable[i].szName, m_D3D11RaiseFlagInfoTable[i].D3D11RaiseFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_CLEAR_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_CLEAR_FLAG D3D11ClearFlag )
{
	return CD3D11ClearFlagInfoMap::Get().ToString((UINT) D3D11ClearFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_CLEAR_FLAG *pD3D11ClearFlag, const tstring &str )
{
	return CD3D11ClearFlagInfoMap::Get().FromString((UINT *) pD3D11ClearFlag, str);
}

//------------------------------------------------------------------------

const D3D11ClearFlagInfo CD3D11ClearFlagInfoMap::m_D3D11ClearFlagInfoTable[] =
{
    { (D3D11_CLEAR_FLAG) (0), _T( "0" ) },
    { D3D11_CLEAR_DEPTH, _T("D3D11_CLEAR_DEPTH") },
    { D3D11_CLEAR_STENCIL, _T("D3D11_CLEAR_STENCIL") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11ClearFlagInfoMap CD3D11ClearFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11ClearFlagInfoMap::GetD3D11ClearFlagInfoTableSize()
{
	return _countof(CD3D11ClearFlagInfoMap::m_D3D11ClearFlagInfoTable);
}

// Construct the lookup table.
CD3D11ClearFlagInfoMap::CD3D11ClearFlagInfoMap()
{
	size_t count = GetD3D11ClearFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11ClearFlagInfoTable[i].szName, m_D3D11ClearFlagInfoTable[i].D3D11ClearFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_BUFFEREX_SRV_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_BUFFEREX_SRV_FLAG D3D11BufferexSrvFlag )
{
	return CD3D11BufferexSrvFlagInfoMap::Get().ToString((UINT) D3D11BufferexSrvFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_BUFFEREX_SRV_FLAG *pD3D11BufferexSrvFlag, const tstring &str )
{
	return CD3D11BufferexSrvFlagInfoMap::Get().FromString((UINT *) pD3D11BufferexSrvFlag, str);
}

//------------------------------------------------------------------------

const D3D11BufferexSrvFlagInfo CD3D11BufferexSrvFlagInfoMap::m_D3D11BufferexSrvFlagInfoTable[] =
{
    { (D3D11_BUFFEREX_SRV_FLAG) (0), _T( "0" ) },
    { D3D11_BUFFEREX_SRV_FLAG_RAW, _T("D3D11_BUFFEREX_SRV_FLAG_RAW") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11BufferexSrvFlagInfoMap CD3D11BufferexSrvFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11BufferexSrvFlagInfoMap::GetD3D11BufferexSrvFlagInfoTableSize()
{
	return _countof(CD3D11BufferexSrvFlagInfoMap::m_D3D11BufferexSrvFlagInfoTable);
}

// Construct the lookup table.
CD3D11BufferexSrvFlagInfoMap::CD3D11BufferexSrvFlagInfoMap()
{
	size_t count = GetD3D11BufferexSrvFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11BufferexSrvFlagInfoTable[i].szName, m_D3D11BufferexSrvFlagInfoTable[i].D3D11BufferexSrvFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_DSV_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_DSV_FLAG D3D11DsvFlag )
{
	return CD3D11DsvFlagInfoMap::Get().ToString((UINT) D3D11DsvFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_DSV_FLAG *pD3D11DsvFlag, const tstring &str )
{
	return CD3D11DsvFlagInfoMap::Get().FromString((UINT *) pD3D11DsvFlag, str);
}

//------------------------------------------------------------------------

const D3D11DsvFlagInfo CD3D11DsvFlagInfoMap::m_D3D11DsvFlagInfoTable[] =
{
    { (D3D11_DSV_FLAG) (0), _T( "0" ) },
    { D3D11_DSV_READ_ONLY_DEPTH, _T("D3D11_DSV_READ_ONLY_DEPTH") },
    { D3D11_DSV_READ_ONLY_STENCIL, _T("D3D11_DSV_READ_ONLY_STENCIL") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11DsvFlagInfoMap CD3D11DsvFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11DsvFlagInfoMap::GetD3D11DsvFlagInfoTableSize()
{
	return _countof(CD3D11DsvFlagInfoMap::m_D3D11DsvFlagInfoTable);
}

// Construct the lookup table.
CD3D11DsvFlagInfoMap::CD3D11DsvFlagInfoMap()
{
	size_t count = GetD3D11DsvFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11DsvFlagInfoTable[i].szName, m_D3D11DsvFlagInfoTable[i].D3D11DsvFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_BUFFER_UAV_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_BUFFER_UAV_FLAG D3D11BufferUavFlag )
{
	return CD3D11BufferUavFlagInfoMap::Get().ToString((UINT) D3D11BufferUavFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_BUFFER_UAV_FLAG *pD3D11BufferUavFlag, const tstring &str )
{
	return CD3D11BufferUavFlagInfoMap::Get().FromString((UINT *) pD3D11BufferUavFlag, str);
}

//------------------------------------------------------------------------

const D3D11BufferUavFlagInfo CD3D11BufferUavFlagInfoMap::m_D3D11BufferUavFlagInfoTable[] =
{
    { (D3D11_BUFFER_UAV_FLAG) (0), _T( "0" ) },
    { D3D11_BUFFER_UAV_FLAG_RAW, _T("D3D11_BUFFER_UAV_FLAG_RAW") },
    { D3D11_BUFFER_UAV_FLAG_APPEND, _T("D3D11_BUFFER_UAV_FLAG_APPEND") },
    { D3D11_BUFFER_UAV_FLAG_COUNTER, _T("D3D11_BUFFER_UAV_FLAG_COUNTER") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11BufferUavFlagInfoMap CD3D11BufferUavFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11BufferUavFlagInfoMap::GetD3D11BufferUavFlagInfoTableSize()
{
	return _countof(CD3D11BufferUavFlagInfoMap::m_D3D11BufferUavFlagInfoTable);
}

// Construct the lookup table.
CD3D11BufferUavFlagInfoMap::CD3D11BufferUavFlagInfoMap()
{
	size_t count = GetD3D11BufferUavFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11BufferUavFlagInfoTable[i].szName, m_D3D11BufferUavFlagInfoTable[i].D3D11BufferUavFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_ASYNC_GETDATA_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_ASYNC_GETDATA_FLAG D3D11AsyncGetdataFlag )
{
	return CD3D11AsyncGetdataFlagInfoMap::Get().ToString((UINT) D3D11AsyncGetdataFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_ASYNC_GETDATA_FLAG *pD3D11AsyncGetdataFlag, const tstring &str )
{
	return CD3D11AsyncGetdataFlagInfoMap::Get().FromString((UINT *) pD3D11AsyncGetdataFlag, str);
}

//------------------------------------------------------------------------

const D3D11AsyncGetdataFlagInfo CD3D11AsyncGetdataFlagInfoMap::m_D3D11AsyncGetdataFlagInfoTable[] =
{
    { (D3D11_ASYNC_GETDATA_FLAG) (0), _T( "0" ) },
    { D3D11_ASYNC_GETDATA_DONOTFLUSH, _T("D3D11_ASYNC_GETDATA_DONOTFLUSH") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11AsyncGetdataFlagInfoMap CD3D11AsyncGetdataFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11AsyncGetdataFlagInfoMap::GetD3D11AsyncGetdataFlagInfoTableSize()
{
	return _countof(CD3D11AsyncGetdataFlagInfoMap::m_D3D11AsyncGetdataFlagInfoTable);
}

// Construct the lookup table.
CD3D11AsyncGetdataFlagInfoMap::CD3D11AsyncGetdataFlagInfoMap()
{
	size_t count = GetD3D11AsyncGetdataFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11AsyncGetdataFlagInfoTable[i].szName, m_D3D11AsyncGetdataFlagInfoTable[i].D3D11AsyncGetdataFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_QUERY_MISC_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_QUERY_MISC_FLAG D3D11QueryMiscFlag )
{
	return CD3D11QueryMiscFlagInfoMap::Get().ToString((UINT) D3D11QueryMiscFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_QUERY_MISC_FLAG *pD3D11QueryMiscFlag, const tstring &str )
{
	return CD3D11QueryMiscFlagInfoMap::Get().FromString((UINT *) pD3D11QueryMiscFlag, str);
}

//------------------------------------------------------------------------

const D3D11QueryMiscFlagInfo CD3D11QueryMiscFlagInfoMap::m_D3D11QueryMiscFlagInfoTable[] =
{
    { (D3D11_QUERY_MISC_FLAG) (0), _T( "0" ) },
    { D3D11_QUERY_MISC_PREDICATEHINT, _T("D3D11_QUERY_MISC_PREDICATEHINT") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11QueryMiscFlagInfoMap CD3D11QueryMiscFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11QueryMiscFlagInfoMap::GetD3D11QueryMiscFlagInfoTableSize()
{
	return _countof(CD3D11QueryMiscFlagInfoMap::m_D3D11QueryMiscFlagInfoTable);
}

// Construct the lookup table.
CD3D11QueryMiscFlagInfoMap::CD3D11QueryMiscFlagInfoMap()
{
	size_t count = GetD3D11QueryMiscFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11QueryMiscFlagInfoTable[i].szName, m_D3D11QueryMiscFlagInfoTable[i].D3D11QueryMiscFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_CREATE_DEVICE_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_CREATE_DEVICE_FLAG D3D11CreateDeviceFlag )
{
	return CD3D11CreateDeviceFlagInfoMap::Get().ToString((UINT) D3D11CreateDeviceFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_CREATE_DEVICE_FLAG *pD3D11CreateDeviceFlag, const tstring &str )
{
	return CD3D11CreateDeviceFlagInfoMap::Get().FromString((UINT *) pD3D11CreateDeviceFlag, str);
}

//------------------------------------------------------------------------

const D3D11CreateDeviceFlagInfo CD3D11CreateDeviceFlagInfoMap::m_D3D11CreateDeviceFlagInfoTable[] =
{
    { (D3D11_CREATE_DEVICE_FLAG) (0), _T( "0" ) },
    { D3D11_CREATE_DEVICE_SINGLETHREADED, _T("D3D11_CREATE_DEVICE_SINGLETHREADED") },
    { D3D11_CREATE_DEVICE_DEBUG, _T("D3D11_CREATE_DEVICE_DEBUG") },
    { D3D11_CREATE_DEVICE_SWITCH_TO_REF, _T("D3D11_CREATE_DEVICE_SWITCH_TO_REF") },
    { D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS, _T("D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS") },
    { D3D11_CREATE_DEVICE_BGRA_SUPPORT, _T("D3D11_CREATE_DEVICE_BGRA_SUPPORT") },
    { D3D11_CREATE_DEVICE_DEBUGGABLE, _T("D3D11_CREATE_DEVICE_DEBUGGABLE") },
    { D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY, _T("D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY") },
    { D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT, _T("D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT") },
    { D3D11_CREATE_DEVICE_VIDEO_SUPPORT, _T("D3D11_CREATE_DEVICE_VIDEO_SUPPORT") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11CreateDeviceFlagInfoMap CD3D11CreateDeviceFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11CreateDeviceFlagInfoMap::GetD3D11CreateDeviceFlagInfoTableSize()
{
	return _countof(CD3D11CreateDeviceFlagInfoMap::m_D3D11CreateDeviceFlagInfoTable);
}

// Construct the lookup table.
CD3D11CreateDeviceFlagInfoMap::CD3D11CreateDeviceFlagInfoMap()
{
	size_t count = GetD3D11CreateDeviceFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11CreateDeviceFlagInfoTable[i].szName, m_D3D11CreateDeviceFlagInfoTable[i].D3D11CreateDeviceFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_COPY_FLAGS related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_COPY_FLAGS D3D11CopyFlags )
{
	return CD3D11CopyFlagsInfoMap::Get().ToString((UINT) D3D11CopyFlags);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_COPY_FLAGS *pD3D11CopyFlags, const tstring &str )
{
	return CD3D11CopyFlagsInfoMap::Get().FromString((UINT *) pD3D11CopyFlags, str);
}

//------------------------------------------------------------------------

const D3D11CopyFlagsInfo CD3D11CopyFlagsInfoMap::m_D3D11CopyFlagsInfoTable[] =
{
    { (D3D11_COPY_FLAGS) (0), _T( "0" ) },
    { D3D11_COPY_NO_OVERWRITE, _T("D3D11_COPY_NO_OVERWRITE") },
    { D3D11_COPY_DISCARD, _T("D3D11_COPY_DISCARD") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11CopyFlagsInfoMap CD3D11CopyFlagsInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11CopyFlagsInfoMap::GetD3D11CopyFlagsInfoTableSize()
{
	return _countof(CD3D11CopyFlagsInfoMap::m_D3D11CopyFlagsInfoTable);
}

// Construct the lookup table.
CD3D11CopyFlagsInfoMap::CD3D11CopyFlagsInfoMap()
{
	size_t count = GetD3D11CopyFlagsInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11CopyFlagsInfoTable[i].szName, m_D3D11CopyFlagsInfoTable[i].D3D11CopyFlags);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_1_CREATE_DEVICE_CONTEXT_STATE_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_1_CREATE_DEVICE_CONTEXT_STATE_FLAG D3D111CreateDeviceContextStateFlag )
{
	return CD3D111CreateDeviceContextStateFlagInfoMap::Get().ToString((UINT) D3D111CreateDeviceContextStateFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_1_CREATE_DEVICE_CONTEXT_STATE_FLAG *pD3D111CreateDeviceContextStateFlag, const tstring &str )
{
	return CD3D111CreateDeviceContextStateFlagInfoMap::Get().FromString((UINT *) pD3D111CreateDeviceContextStateFlag, str);
}

//------------------------------------------------------------------------

const D3D111CreateDeviceContextStateFlagInfo CD3D111CreateDeviceContextStateFlagInfoMap::m_D3D111CreateDeviceContextStateFlagInfoTable[] =
{
    { (D3D11_1_CREATE_DEVICE_CONTEXT_STATE_FLAG) (0), _T( "0" ) },
    { D3D11_1_CREATE_DEVICE_CONTEXT_STATE_SINGLETHREADED, _T("D3D11_1_CREATE_DEVICE_CONTEXT_STATE_SINGLETHREADED") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D111CreateDeviceContextStateFlagInfoMap CD3D111CreateDeviceContextStateFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D111CreateDeviceContextStateFlagInfoMap::GetD3D111CreateDeviceContextStateFlagInfoTableSize()
{
	return _countof(CD3D111CreateDeviceContextStateFlagInfoMap::m_D3D111CreateDeviceContextStateFlagInfoTable);
}

// Construct the lookup table.
CD3D111CreateDeviceContextStateFlagInfoMap::CD3D111CreateDeviceContextStateFlagInfoMap()
{
	size_t count = GetD3D111CreateDeviceContextStateFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D111CreateDeviceContextStateFlagInfoTable[i].szName, m_D3D111CreateDeviceContextStateFlagInfoTable[i].D3D111CreateDeviceContextStateFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_TILE_MAPPING_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_TILE_MAPPING_FLAG D3D11TileMappingFlag )
{
	return CD3D11TileMappingFlagInfoMap::Get().ToString((UINT) D3D11TileMappingFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_TILE_MAPPING_FLAG *pD3D11TileMappingFlag, const tstring &str )
{
	return CD3D11TileMappingFlagInfoMap::Get().FromString((UINT *) pD3D11TileMappingFlag, str);
}

//------------------------------------------------------------------------

const D3D11TileMappingFlagInfo CD3D11TileMappingFlagInfoMap::m_D3D11TileMappingFlagInfoTable[] =
{
    { (D3D11_TILE_MAPPING_FLAG) (0), _T( "0" ) },
    { D3D11_TILE_MAPPING_NO_OVERWRITE, _T("D3D11_TILE_MAPPING_NO_OVERWRITE") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11TileMappingFlagInfoMap CD3D11TileMappingFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11TileMappingFlagInfoMap::GetD3D11TileMappingFlagInfoTableSize()
{
	return _countof(CD3D11TileMappingFlagInfoMap::m_D3D11TileMappingFlagInfoTable);
}

// Construct the lookup table.
CD3D11TileMappingFlagInfoMap::CD3D11TileMappingFlagInfoMap()
{
	size_t count = GetD3D11TileMappingFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11TileMappingFlagInfoTable[i].szName, m_D3D11TileMappingFlagInfoTable[i].D3D11TileMappingFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_TILE_RANGE_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_TILE_RANGE_FLAG D3D11TileRangeFlag )
{
	return CD3D11TileRangeFlagInfoMap::Get().ToString((UINT) D3D11TileRangeFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_TILE_RANGE_FLAG *pD3D11TileRangeFlag, const tstring &str )
{
	return CD3D11TileRangeFlagInfoMap::Get().FromString((UINT *) pD3D11TileRangeFlag, str);
}

//------------------------------------------------------------------------

const D3D11TileRangeFlagInfo CD3D11TileRangeFlagInfoMap::m_D3D11TileRangeFlagInfoTable[] =
{
    { (D3D11_TILE_RANGE_FLAG) (0), _T( "0" ) },
    { D3D11_TILE_RANGE_NULL, _T("D3D11_TILE_RANGE_NULL") },
    { D3D11_TILE_RANGE_SKIP, _T("D3D11_TILE_RANGE_SKIP") },
    { D3D11_TILE_RANGE_REUSE_SINGLE_TILE, _T("D3D11_TILE_RANGE_REUSE_SINGLE_TILE") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11TileRangeFlagInfoMap CD3D11TileRangeFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11TileRangeFlagInfoMap::GetD3D11TileRangeFlagInfoTableSize()
{
	return _countof(CD3D11TileRangeFlagInfoMap::m_D3D11TileRangeFlagInfoTable);
}

// Construct the lookup table.
CD3D11TileRangeFlagInfoMap::CD3D11TileRangeFlagInfoMap()
{
	size_t count = GetD3D11TileRangeFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11TileRangeFlagInfoTable[i].szName, m_D3D11TileRangeFlagInfoTable[i].D3D11TileRangeFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_FLAG D3D11CheckMultisampleQualityLevelsFlag )
{
	return CD3D11CheckMultisampleQualityLevelsFlagInfoMap::Get().ToString((UINT) D3D11CheckMultisampleQualityLevelsFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_FLAG *pD3D11CheckMultisampleQualityLevelsFlag, const tstring &str )
{
	return CD3D11CheckMultisampleQualityLevelsFlagInfoMap::Get().FromString((UINT *) pD3D11CheckMultisampleQualityLevelsFlag, str);
}

//------------------------------------------------------------------------

const D3D11CheckMultisampleQualityLevelsFlagInfo CD3D11CheckMultisampleQualityLevelsFlagInfoMap::m_D3D11CheckMultisampleQualityLevelsFlagInfoTable[] =
{
    { (D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_FLAG) (0), _T( "0" ) },
    { D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_TILED_RESOURCE, _T("D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_TILED_RESOURCE") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11CheckMultisampleQualityLevelsFlagInfoMap CD3D11CheckMultisampleQualityLevelsFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11CheckMultisampleQualityLevelsFlagInfoMap::GetD3D11CheckMultisampleQualityLevelsFlagInfoTableSize()
{
	return _countof(CD3D11CheckMultisampleQualityLevelsFlagInfoMap::m_D3D11CheckMultisampleQualityLevelsFlagInfoTable);
}

// Construct the lookup table.
CD3D11CheckMultisampleQualityLevelsFlagInfoMap::CD3D11CheckMultisampleQualityLevelsFlagInfoMap()
{
	size_t count = GetD3D11CheckMultisampleQualityLevelsFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11CheckMultisampleQualityLevelsFlagInfoTable[i].szName, m_D3D11CheckMultisampleQualityLevelsFlagInfoTable[i].D3D11CheckMultisampleQualityLevelsFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D11_TILE_COPY_FLAG related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D11_TILE_COPY_FLAG D3D11TileCopyFlag )
{
	return CD3D11TileCopyFlagInfoMap::Get().ToString((UINT) D3D11TileCopyFlag);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D11_TILE_COPY_FLAG *pD3D11TileCopyFlag, const tstring &str )
{
	return CD3D11TileCopyFlagInfoMap::Get().FromString((UINT *) pD3D11TileCopyFlag, str);
}

//------------------------------------------------------------------------

const D3D11TileCopyFlagInfo CD3D11TileCopyFlagInfoMap::m_D3D11TileCopyFlagInfoTable[] =
{
    { (D3D11_TILE_COPY_FLAG) (0), _T( "0" ) },
    { D3D11_TILE_COPY_NO_OVERWRITE, _T("D3D11_TILE_COPY_NO_OVERWRITE") },
    { D3D11_TILE_COPY_LINEAR_BUFFER_TO_SWIZZLED_TILED_RESOURCE, _T("D3D11_TILE_COPY_LINEAR_BUFFER_TO_SWIZZLED_TILED_RESOURCE") },
    { D3D11_TILE_COPY_SWIZZLED_TILED_RESOURCE_TO_LINEAR_BUFFER, _T("D3D11_TILE_COPY_SWIZZLED_TILED_RESOURCE_TO_LINEAR_BUFFER") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3D11TileCopyFlagInfoMap CD3D11TileCopyFlagInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3D11TileCopyFlagInfoMap::GetD3D11TileCopyFlagInfoTableSize()
{
	return _countof(CD3D11TileCopyFlagInfoMap::m_D3D11TileCopyFlagInfoTable);
}

// Construct the lookup table.
CD3D11TileCopyFlagInfoMap::CD3D11TileCopyFlagInfoMap()
{
	size_t count = GetD3D11TileCopyFlagInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3D11TileCopyFlagInfoTable[i].szName, m_D3D11TileCopyFlagInfoTable[i].D3D11TileCopyFlag);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D_SHADER_VARIABLE_FLAGS related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D_SHADER_VARIABLE_FLAGS D3DShaderVariableFlags )
{
	return CD3DShaderVariableFlagsInfoMap::Get().ToString((UINT) D3DShaderVariableFlags);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D_SHADER_VARIABLE_FLAGS *pD3DShaderVariableFlags, const tstring &str )
{
	return CD3DShaderVariableFlagsInfoMap::Get().FromString((UINT *) pD3DShaderVariableFlags, str);
}

//------------------------------------------------------------------------

const D3DShaderVariableFlagsInfo CD3DShaderVariableFlagsInfoMap::m_D3DShaderVariableFlagsInfoTable[] =
{
    { (D3D_SHADER_VARIABLE_FLAGS) (0), _T( "0" ) },
    { D3D_SVF_USERPACKED, _T("D3D_SVF_USERPACKED") },
    { D3D_SVF_USED, _T("D3D_SVF_USED") },
    { D3D_SVF_INTERFACE_POINTER, _T("D3D_SVF_INTERFACE_POINTER") },
    { D3D_SVF_INTERFACE_PARAMETER, _T("D3D_SVF_INTERFACE_PARAMETER") },
    { D3D_SVF_FORCE_DWORD, _T("D3D_SVF_FORCE_DWORD") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3DShaderVariableFlagsInfoMap CD3DShaderVariableFlagsInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3DShaderVariableFlagsInfoMap::GetD3DShaderVariableFlagsInfoTableSize()
{
	return _countof(CD3DShaderVariableFlagsInfoMap::m_D3DShaderVariableFlagsInfoTable);
}

// Construct the lookup table.
CD3DShaderVariableFlagsInfoMap::CD3DShaderVariableFlagsInfoMap()
{
	size_t count = GetD3DShaderVariableFlagsInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3DShaderVariableFlagsInfoTable[i].szName, m_D3DShaderVariableFlagsInfoTable[i].D3DShaderVariableFlags);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D_SHADER_INPUT_FLAGS related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D_SHADER_INPUT_FLAGS D3DShaderInputFlags )
{
	return CD3DShaderInputFlagsInfoMap::Get().ToString((UINT) D3DShaderInputFlags);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D_SHADER_INPUT_FLAGS *pD3DShaderInputFlags, const tstring &str )
{
	return CD3DShaderInputFlagsInfoMap::Get().FromString((UINT *) pD3DShaderInputFlags, str);
}

//------------------------------------------------------------------------

const D3DShaderInputFlagsInfo CD3DShaderInputFlagsInfoMap::m_D3DShaderInputFlagsInfoTable[] =
{
    { (D3D_SHADER_INPUT_FLAGS) (0), _T( "0" ) },
    { D3D_SIF_USERPACKED, _T("D3D_SIF_USERPACKED") },
    { D3D_SIF_COMPARISON_SAMPLER, _T("D3D_SIF_COMPARISON_SAMPLER") },
    { D3D_SIF_TEXTURE_COMPONENT_0, _T("D3D_SIF_TEXTURE_COMPONENT_0") },
    { D3D_SIF_TEXTURE_COMPONENT_1, _T("D3D_SIF_TEXTURE_COMPONENT_1") },
    { D3D_SIF_TEXTURE_COMPONENTS, _T("D3D_SIF_TEXTURE_COMPONENTS") },
    { D3D_SIF_UNUSED, _T("D3D_SIF_UNUSED") },
    { D3D_SIF_FORCE_DWORD, _T("D3D_SIF_FORCE_DWORD") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3DShaderInputFlagsInfoMap CD3DShaderInputFlagsInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3DShaderInputFlagsInfoMap::GetD3DShaderInputFlagsInfoTableSize()
{
	return _countof(CD3DShaderInputFlagsInfoMap::m_D3DShaderInputFlagsInfoTable);
}

// Construct the lookup table.
CD3DShaderInputFlagsInfoMap::CD3DShaderInputFlagsInfoMap()
{
	size_t count = GetD3DShaderInputFlagsInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3DShaderInputFlagsInfoTable[i].szName, m_D3DShaderInputFlagsInfoTable[i].D3DShaderInputFlags);
	}
}

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// D3D_SHADER_CBUFFER_FLAGS related definitions
//////////////////////////////////////////////////////////////////////////

tstring WGFTESTAPI ToString( D3D_SHADER_CBUFFER_FLAGS D3DShaderCbufferFlags )
{
	return CD3DShaderCbufferFlagsInfoMap::Get().ToString((UINT) D3DShaderCbufferFlags);
}

//------------------------------------------------------------------------

bool WGFTESTAPI FromString( D3D_SHADER_CBUFFER_FLAGS *pD3DShaderCbufferFlags, const tstring &str )
{
	return CD3DShaderCbufferFlagsInfoMap::Get().FromString((UINT *) pD3DShaderCbufferFlags, str);
}

//------------------------------------------------------------------------

const D3DShaderCbufferFlagsInfo CD3DShaderCbufferFlagsInfoMap::m_D3DShaderCbufferFlagsInfoTable[] =
{
    { (D3D_SHADER_CBUFFER_FLAGS) (0), _T( "0" ) },
    { D3D_CBF_USERPACKED, _T("D3D_CBF_USERPACKED") },
    { D3D_CBF_FORCE_DWORD, _T("D3D_CBF_FORCE_DWORD") },

};

//------------------------------------------------------------------------

// Instantiate the singleton
CD3DShaderCbufferFlagsInfoMap CD3DShaderCbufferFlagsInfoMap::m_this;

//------------------------------------------------------------------------


size_t CD3DShaderCbufferFlagsInfoMap::GetD3DShaderCbufferFlagsInfoTableSize()
{
	return _countof(CD3DShaderCbufferFlagsInfoMap::m_D3DShaderCbufferFlagsInfoTable);
}

// Construct the lookup table.
CD3DShaderCbufferFlagsInfoMap::CD3DShaderCbufferFlagsInfoMap()
{
	size_t count = GetD3DShaderCbufferFlagsInfoTableSize();

	for(size_t i = 0; i < count; i++ )
	{
		AddValue(m_D3DShaderCbufferFlagsInfoTable[i].szName, m_D3DShaderCbufferFlagsInfoTable[i].D3DShaderCbufferFlags);
	}
}

//------------------------------------------------------------------------


