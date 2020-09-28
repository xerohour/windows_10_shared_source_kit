//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D10ValueSets.h
//* 
//* Description: 
//*     Declare classes that implement common ValueSets and filters related to enums
//*
//*****************************************************************************

// Ignore Prefast warnings 26060 (Range Precondition violation) and 28930 (Unused assignment of pointer variable)
#pragma warning( disable: 26060 )
#pragma warning( disable: 28930 )

#pragma once

#include "StringUtils.h"
#include "D3D10EnumsInfo.h"

#include "D3D10Wrapped.h"

DECLARE_NAMED_VALUES( D3D10_INPUT_CLASSIFICATION )
DECLARE_NAMED_VALUES( D3D10_FILL_MODE )
DECLARE_NAMED_VALUES( D3D10_PRIMITIVE_TOPOLOGY )
DECLARE_NAMED_VALUES( D3D10_PRIMITIVE )
DECLARE_NAMED_VALUES( D3D10_CULL_MODE )
DECLARE_NAMED_VALUES( D3D10_RESOURCE_DIMENSION )
DECLARE_NAMED_VALUES( D3D10_SRV_DIMENSION )
DECLARE_NAMED_VALUES( D3D10_DSV_DIMENSION )
DECLARE_NAMED_VALUES( D3D10_RTV_DIMENSION )
DECLARE_NAMED_VALUES( D3D10_USAGE )
//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D10_BIND_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D10BindFlagValueSet - defines a D3D10_BIND_FLAG ValueSet.
class CD3D10BindFlagValueSet : public CUserValueSet< D3D10_BIND_FLAG >
{
public:
	CD3D10BindFlagValueSet( CFilterHandleCommonSource< D3D10_BIND_FLAG > filter = NULL );
};

inline CD3D10BindFlagValueSet *D3D10BindFlagValueSet( CFilterHandleCommonSource< D3D10_BIND_FLAG > filter )
{
	return new CD3D10BindFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D10_CPU_ACCESS_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D10CpuAccessFlagValueSet - defines a D3D10_CPU_ACCESS_FLAG ValueSet.
class CD3D10CpuAccessFlagValueSet : public CUserValueSet< D3D10_CPU_ACCESS_FLAG >
{
public:
	CD3D10CpuAccessFlagValueSet( CFilterHandleCommonSource< D3D10_CPU_ACCESS_FLAG > filter = NULL );
};

inline CD3D10CpuAccessFlagValueSet *D3D10CpuAccessFlagValueSet( CFilterHandleCommonSource< D3D10_CPU_ACCESS_FLAG > filter )
{
	return new CD3D10CpuAccessFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D10_RESOURCE_MISC_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D10ResourceMiscFlagValueSet - defines a D3D10_RESOURCE_MISC_FLAG ValueSet.
class CD3D10ResourceMiscFlagValueSet : public CUserValueSet< D3D10_RESOURCE_MISC_FLAG >
{
public:
	CD3D10ResourceMiscFlagValueSet( CFilterHandleCommonSource< D3D10_RESOURCE_MISC_FLAG > filter = NULL );
};

inline CD3D10ResourceMiscFlagValueSet *D3D10ResourceMiscFlagValueSet( CFilterHandleCommonSource< D3D10_RESOURCE_MISC_FLAG > filter )
{
	return new CD3D10ResourceMiscFlagValueSet( filter );
}


DECLARE_NAMED_VALUES( D3D10_MAP )
//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D10_MAP_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D10MapFlagValueSet - defines a D3D10_MAP_FLAG ValueSet.
class CD3D10MapFlagValueSet : public CUserValueSet< D3D10_MAP_FLAG >
{
public:
	CD3D10MapFlagValueSet( CFilterHandleCommonSource< D3D10_MAP_FLAG > filter = NULL );
};

inline CD3D10MapFlagValueSet *D3D10MapFlagValueSet( CFilterHandleCommonSource< D3D10_MAP_FLAG > filter )
{
	return new CD3D10MapFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D10_RAISE_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D10RaiseFlagValueSet - defines a D3D10_RAISE_FLAG ValueSet.
class CD3D10RaiseFlagValueSet : public CUserValueSet< D3D10_RAISE_FLAG >
{
public:
	CD3D10RaiseFlagValueSet( CFilterHandleCommonSource< D3D10_RAISE_FLAG > filter = NULL );
};

inline CD3D10RaiseFlagValueSet *D3D10RaiseFlagValueSet( CFilterHandleCommonSource< D3D10_RAISE_FLAG > filter )
{
	return new CD3D10RaiseFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D10_CLEAR_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3D10ClearFlagValueSet - defines a D3D10_CLEAR_FLAG ValueSet.
class CD3D10ClearFlagValueSet : public CUserValueSet< D3D10_CLEAR_FLAG >
{
public:
	CD3D10ClearFlagValueSet( CFilterHandleCommonSource< D3D10_CLEAR_FLAG > filter = NULL );
};

inline CD3D10ClearFlagValueSet *D3D10ClearFlagValueSet( CFilterHandleCommonSource< D3D10_CLEAR_FLAG > filter )
{
	return new CD3D10ClearFlagValueSet( filter );
}


DECLARE_NAMED_VALUES( D3D10_COMPARISON_FUNC )
DECLARE_NAMED_VALUES( D3D10_DEPTH_WRITE_MASK )
DECLARE_NAMED_VALUES( D3D10_STENCIL_OP )
DECLARE_NAMED_VALUES( D3D10_BLEND )
DECLARE_NAMED_VALUES( D3D10_BLEND_OP )
DECLARE_NAMED_VALUES( D3D10_COLOR_WRITE_ENABLE )
DECLARE_NAMED_VALUES( D3D10_TEXTURECUBE_FACE )
DECLARE_NAMED_VALUES( D3D10_FILTER )
DECLARE_NAMED_VALUES( D3D10_FILTER_TYPE )
DECLARE_NAMED_VALUES( D3D10_TEXTURE_ADDRESS_MODE )
DECLARE_NAMED_VALUES( D3D10_FORMAT_SUPPORT )
DECLARE_NAMED_VALUES( D3D10_ASYNC_GETDATA_FLAG )
DECLARE_NAMED_VALUES( D3D10_QUERY )
DECLARE_NAMED_VALUES( D3D10_QUERY_MISC_FLAG )
DECLARE_NAMED_VALUES( D3D10_COUNTER )
DECLARE_NAMED_VALUES( D3D10_COUNTER_TYPE )
DECLARE_NAMED_VALUES( D3D10_CREATE_DEVICE_FLAG )
DECLARE_NAMED_VALUES( D3D10_MESSAGE_CATEGORY )
DECLARE_NAMED_VALUES( D3D10_MESSAGE_SEVERITY )
DECLARE_NAMED_VALUES( D3D10_MESSAGE_ID )
DECLARE_NAMED_VALUES( D3D10_DRIVER_TYPE )
DECLARE_NAMED_VALUES( D3D10_FEATURE_LEVEL1 )
DECLARE_NAMED_VALUES( D3D10_SRV_DIMENSION1 )
DECLARE_NAMED_VALUES( D3D10_STANDARD_MULTISAMPLE_QUALITY_LEVELS )
