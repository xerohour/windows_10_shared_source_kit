//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D11ValueSets.h
//* 
//* Description: 
//*     Declare classes that implement common ValueSets and filters related to enums
//*
//*****************************************************************************

#pragma once

#include "StringUtils.h"
#include "D3D11EnumsInfo.h"

DECLARE_NAMED_VALUES( D3D11_INPUT_CLASSIFICATION )
DECLARE_NAMED_VALUES( D3D11_FILL_MODE )
DECLARE_NAMED_VALUES( D3D11_PRIMITIVE_TOPOLOGY )
DECLARE_NAMED_VALUES( D3D11_PRIMITIVE )
DECLARE_NAMED_VALUES( D3D11_CULL_MODE )
DECLARE_NAMED_VALUES( D3D11_RESOURCE_DIMENSION )
DECLARE_NAMED_VALUES( D3D11_SRV_DIMENSION )
DECLARE_NAMED_VALUES( D3D11_DSV_DIMENSION )
DECLARE_NAMED_VALUES( D3D11_RTV_DIMENSION )
DECLARE_NAMED_VALUES( D3D11_USAGE )
//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D11_BIND_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D11BindFlagValueSet - defines a D3D11_BIND_FLAG ValueSet.
class CD3D11BindFlagValueSet : public CUserValueSet< D3D11_BIND_FLAG >
{
public:
	CD3D11BindFlagValueSet( CFilterHandleCommonSource< D3D11_BIND_FLAG > filter = NULL );
};

inline CD3D11BindFlagValueSet *D3D11BindFlagValueSet( CFilterHandleCommonSource< D3D11_BIND_FLAG > filter )
{
	return new CD3D11BindFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D11_CPU_ACCESS_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D11CpuAccessFlagValueSet - defines a D3D11_CPU_ACCESS_FLAG ValueSet.
class CD3D11CpuAccessFlagValueSet : public CUserValueSet< D3D11_CPU_ACCESS_FLAG >
{
public:
	CD3D11CpuAccessFlagValueSet( CFilterHandleCommonSource< D3D11_CPU_ACCESS_FLAG > filter = NULL );
};

inline CD3D11CpuAccessFlagValueSet *D3D11CpuAccessFlagValueSet( CFilterHandleCommonSource< D3D11_CPU_ACCESS_FLAG > filter )
{
	return new CD3D11CpuAccessFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D11_RESOURCE_MISC_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D11ResourceMiscFlagValueSet - defines a D3D11_RESOURCE_MISC_FLAG ValueSet.
class CD3D11ResourceMiscFlagValueSet : public CUserValueSet< D3D11_RESOURCE_MISC_FLAG >
{
public:
	CD3D11ResourceMiscFlagValueSet( CFilterHandleCommonSource< D3D11_RESOURCE_MISC_FLAG > filter = NULL );
};

inline CD3D11ResourceMiscFlagValueSet *D3D11ResourceMiscFlagValueSet( CFilterHandleCommonSource< D3D11_RESOURCE_MISC_FLAG > filter )
{
	return new CD3D11ResourceMiscFlagValueSet( filter );
}


DECLARE_NAMED_VALUES( D3D11_MAP )
//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D11_MAP_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D11MapFlagValueSet - defines a D3D11_MAP_FLAG ValueSet.
class CD3D11MapFlagValueSet : public CUserValueSet< D3D11_MAP_FLAG >
{
public:
	CD3D11MapFlagValueSet( CFilterHandleCommonSource< D3D11_MAP_FLAG > filter = NULL );
};

inline CD3D11MapFlagValueSet *D3D11MapFlagValueSet( CFilterHandleCommonSource< D3D11_MAP_FLAG > filter )
{
	return new CD3D11MapFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D11_RAISE_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D11RaiseFlagValueSet - defines a D3D11_RAISE_FLAG ValueSet.
class CD3D11RaiseFlagValueSet : public CUserValueSet< D3D11_RAISE_FLAG >
{
public:
	CD3D11RaiseFlagValueSet( CFilterHandleCommonSource< D3D11_RAISE_FLAG > filter = NULL );
};

inline CD3D11RaiseFlagValueSet *D3D11RaiseFlagValueSet( CFilterHandleCommonSource< D3D11_RAISE_FLAG > filter )
{
	return new CD3D11RaiseFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D11_CLEAR_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D11ClearFlagValueSet - defines a D3D11_CLEAR_FLAG ValueSet.
class CD3D11ClearFlagValueSet : public CUserValueSet< D3D11_CLEAR_FLAG >
{
public:
	CD3D11ClearFlagValueSet( CFilterHandleCommonSource< D3D11_CLEAR_FLAG > filter = NULL );
};

inline CD3D11ClearFlagValueSet *D3D11ClearFlagValueSet( CFilterHandleCommonSource< D3D11_CLEAR_FLAG > filter )
{
	return new CD3D11ClearFlagValueSet( filter );
}


DECLARE_NAMED_VALUES( D3D11_COMPARISON_FUNC )
DECLARE_NAMED_VALUES( D3D11_DEPTH_WRITE_MASK )
DECLARE_NAMED_VALUES( D3D11_STENCIL_OP )
DECLARE_NAMED_VALUES( D3D11_BLEND )
DECLARE_NAMED_VALUES( D3D11_BLEND_OP )
DECLARE_NAMED_VALUES( D3D11_COLOR_WRITE_ENABLE )
DECLARE_NAMED_VALUES( D3D11_TEXTURECUBE_FACE )
DECLARE_NAMED_VALUES( D3D11_FILTER )
DECLARE_NAMED_VALUES( D3D11_FILTER_TYPE )
DECLARE_NAMED_VALUES( D3D11_TEXTURE_ADDRESS_MODE )
DECLARE_NAMED_VALUES( D3D11_FORMAT_SUPPORT )
DECLARE_NAMED_VALUES( D3D11_ASYNC_GETDATA_FLAG )
DECLARE_NAMED_VALUES( D3D11_QUERY )
DECLARE_NAMED_VALUES( D3D11_QUERY_MISC_FLAG )
DECLARE_NAMED_VALUES( D3D11_COUNTER )
DECLARE_NAMED_VALUES( D3D11_COUNTER_TYPE )
DECLARE_NAMED_VALUES( D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS )
DECLARE_NAMED_VALUES( D3D11_CREATE_DEVICE_FLAG )
DECLARE_NAMED_VALUES( D3D11_MESSAGE_CATEGORY )
DECLARE_NAMED_VALUES( D3D11_MESSAGE_SEVERITY )
