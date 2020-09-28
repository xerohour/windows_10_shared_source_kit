//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3DValueSets.h
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
#include "D3DEnumsInfo.h"

#include "D3DWrapped.h"

DECLARE_NAMED_VALUES( D3D_INPUT_CLASSIFICATION )
DECLARE_NAMED_VALUES( D3D_FILL_MODE )
DECLARE_NAMED_VALUES( D3D_PRIMITIVE_TOPOLOGY )
DECLARE_NAMED_VALUES( D3D_PRIMITIVE )
DECLARE_NAMED_VALUES( D3D_CULL_MODE )
DECLARE_NAMED_VALUES( D3D_RESOURCE_DIMENSION )
DECLARE_NAMED_VALUES( D3D_SRV_DIMENSION )
DECLARE_NAMED_VALUES( D3D_DSV_DIMENSION )
DECLARE_NAMED_VALUES( D3D_RTV_DIMENSION )
DECLARE_NAMED_VALUES( D3D_USAGE )
//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D_BIND_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3DBindFlagValueSet - defines a D3D_BIND_FLAG ValueSet.
class CD3DBindFlagValueSet : public CUserValueSet< D3D_BIND_FLAG >
{
public:
	CD3DBindFlagValueSet( CFilterHandleCommonSource< D3D_BIND_FLAG > filter = NULL );
};

inline CD3DBindFlagValueSet *D3DBindFlagValueSet( CFilterHandleCommonSource< D3D_BIND_FLAG > filter )
{
	return new CD3DBindFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D_CPU_ACCESS_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3DCpuAccessFlagValueSet - defines a D3D_CPU_ACCESS_FLAG ValueSet.
class CD3DCpuAccessFlagValueSet : public CUserValueSet< D3D_CPU_ACCESS_FLAG >
{
public:
	CD3DCpuAccessFlagValueSet( CFilterHandleCommonSource< D3D_CPU_ACCESS_FLAG > filter = NULL );
};

inline CD3DCpuAccessFlagValueSet *D3DCpuAccessFlagValueSet( CFilterHandleCommonSource< D3D_CPU_ACCESS_FLAG > filter )
{
	return new CD3DCpuAccessFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D_RESOURCE_MISC_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3DResourceMiscFlagValueSet - defines a D3D_RESOURCE_MISC_FLAG ValueSet.
class CD3DResourceMiscFlagValueSet : public CUserValueSet< D3D_RESOURCE_MISC_FLAG >
{
public:
	CD3DResourceMiscFlagValueSet( CFilterHandleCommonSource< D3D_RESOURCE_MISC_FLAG > filter = NULL );
};

inline CD3DResourceMiscFlagValueSet *D3DResourceMiscFlagValueSet( CFilterHandleCommonSource< D3D_RESOURCE_MISC_FLAG > filter )
{
	return new CD3DResourceMiscFlagValueSet( filter );
}


DECLARE_NAMED_VALUES( D3D_MAP )
//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D_MAP_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3DMapFlagValueSet - defines a D3D_MAP_FLAG ValueSet.
class CD3DMapFlagValueSet : public CUserValueSet< D3D_MAP_FLAG >
{
public:
	CD3DMapFlagValueSet( CFilterHandleCommonSource< D3D_MAP_FLAG > filter = NULL );
};

inline CD3DMapFlagValueSet *D3DMapFlagValueSet( CFilterHandleCommonSource< D3D_MAP_FLAG > filter )
{
	return new CD3DMapFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D_RAISE_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3DRaiseFlagValueSet - defines a D3D_RAISE_FLAG ValueSet.
class CD3DRaiseFlagValueSet : public CUserValueSet< D3D_RAISE_FLAG >
{
public:
	CD3DRaiseFlagValueSet( CFilterHandleCommonSource< D3D_RAISE_FLAG > filter = NULL );
};

inline CD3DRaiseFlagValueSet *D3DRaiseFlagValueSet( CFilterHandleCommonSource< D3D_RAISE_FLAG > filter )
{
	return new CD3DRaiseFlagValueSet( filter );
}


//////////////////////////////////////////////////////////////////////////
// ValueSets and filters related to D3D_CLEAR_FLAG
//////////////////////////////////////////////////////////////////////////

// CD3DClearFlagValueSet - defines a D3D_CLEAR_FLAG ValueSet.
class CD3DClearFlagValueSet : public CUserValueSet< D3D_CLEAR_FLAG >
{
public:
	CD3DClearFlagValueSet( CFilterHandleCommonSource< D3D_CLEAR_FLAG > filter = NULL );
};

inline CD3DClearFlagValueSet *D3DClearFlagValueSet( CFilterHandleCommonSource< D3D_CLEAR_FLAG > filter )
{
	return new CD3DClearFlagValueSet( filter );
}


DECLARE_NAMED_VALUES( D3D_COMPARISON_FUNC )
DECLARE_NAMED_VALUES( D3D_DEPTH_WRITE_MASK )
DECLARE_NAMED_VALUES( D3D_STENCIL_OP )
DECLARE_NAMED_VALUES( D3D_BLEND )
DECLARE_NAMED_VALUES( D3D_BLEND_OP )
DECLARE_NAMED_VALUES( D3D_COLOR_WRITE_ENABLE )
DECLARE_NAMED_VALUES( D3D_TEXTURECUBE_FACE )
DECLARE_NAMED_VALUES( D3D_FILTER )
DECLARE_NAMED_VALUES( D3D_FILTER_TYPE )
DECLARE_NAMED_VALUES( D3D_TEXTURE_ADDRESS_MODE )
DECLARE_NAMED_VALUES( D3D_FORMAT_SUPPORT )
DECLARE_NAMED_VALUES( D3D_ASYNC_GETDATA_FLAG )
DECLARE_NAMED_VALUES( D3D_QUERY )
DECLARE_NAMED_VALUES( D3D_QUERY_MISC_FLAG )
DECLARE_NAMED_VALUES( D3D_COUNTER )
DECLARE_NAMED_VALUES( D3D_COUNTER_TYPE )
DECLARE_NAMED_VALUES( D3D_CREATE_DEVICE_FLAG )
DECLARE_NAMED_VALUES( D3D_MESSAGE_CATEGORY )
DECLARE_NAMED_VALUES( D3D_MESSAGE_SEVERITY )
DECLARE_NAMED_VALUES( D3D_MESSAGE_ID )
DECLARE_NAMED_VALUES( D3D_DRIVER_TYPE )
DECLARE_NAMED_VALUES( D3D_FEATURE_LEVEL1 )
DECLARE_NAMED_VALUES( D3D_STANDARD_MULTISAMPLE_QUALITY_LEVELS )
