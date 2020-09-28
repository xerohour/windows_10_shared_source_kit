//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D10Convert.cpp
//* 
//* Description: 
//*     Interface specific D3D wrapper class declarations.
//*
//*****************************************************************************

// Ignore Prefast warnings 26060 (Range Precondition violation) and 28930 (Unused assignment of pointer variable)
#pragma warning( disable: 26060 )
#pragma warning( disable: 28930 )


#include "pch.h"
#include "D3D10Wrapped.h"
#include "D3D10Convert.h"


////////////////////////////////////////////////////////////
//
// Conversion Functions
//
////////////////////////////////////////////////////////////


//
// Enum Conversions
//

void D3DWrapperConvertEnum( D3D_INPUT_CLASSIFICATION *d3d_input_classification, D3D10_INPUT_CLASSIFICATION d3d_input_classification_10 )
{
    switch( d3d_input_classification_10 )
    {
        default:
            *d3d_input_classification = (D3D_INPUT_CLASSIFICATION)d3d_input_classification_10;
            break;
        case D3D10_INPUT_PER_VERTEX_DATA:
            *d3d_input_classification = D3D_INPUT_PER_VERTEX_DATA;
            break;
        case D3D10_INPUT_PER_INSTANCE_DATA:
            *d3d_input_classification = D3D_INPUT_PER_INSTANCE_DATA;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_INPUT_CLASSIFICATION *d3d_input_classification_10, D3D_INPUT_CLASSIFICATION d3d_input_classification )
{
    switch( d3d_input_classification )
    {
        default:
            *d3d_input_classification_10 = (D3D10_INPUT_CLASSIFICATION)d3d_input_classification;
            break;
        case D3D_INPUT_PER_VERTEX_DATA:
            *d3d_input_classification_10 = D3D10_INPUT_PER_VERTEX_DATA;
            break;
        case D3D_INPUT_PER_INSTANCE_DATA:
            *d3d_input_classification_10 = D3D10_INPUT_PER_INSTANCE_DATA;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_FILL_MODE *d3d_fill_mode, D3D10_FILL_MODE d3d_fill_mode_10 )
{
    switch( d3d_fill_mode_10 )
    {
        default:
            *d3d_fill_mode = (D3D_FILL_MODE)d3d_fill_mode_10;
            break;
        case D3D10_FILL_WIREFRAME:
            *d3d_fill_mode = D3D_FILL_WIREFRAME;
            break;
        case D3D10_FILL_SOLID:
            *d3d_fill_mode = D3D_FILL_SOLID;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_FILL_MODE *d3d_fill_mode_10, D3D_FILL_MODE d3d_fill_mode )
{
    switch( d3d_fill_mode )
    {
        default:
            *d3d_fill_mode_10 = (D3D10_FILL_MODE)d3d_fill_mode;
            break;
        case D3D_FILL_WIREFRAME:
            *d3d_fill_mode_10 = D3D10_FILL_WIREFRAME;
            break;
        case D3D_FILL_SOLID:
            *d3d_fill_mode_10 = D3D10_FILL_SOLID;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_PRIMITIVE_TOPOLOGY *d3d_primitive_topology, D3D_PRIMITIVE_TOPOLOGY d3d_primitive_topology_10 )
{
    *d3d_primitive_topology = d3d_primitive_topology_10;
}

void D3DWrapperConvertEnum( D3D_PRIMITIVE *d3d_primitive, D3D_PRIMITIVE d3d_primitive_10 )
{
    *d3d_primitive = d3d_primitive_10;
}

void D3DWrapperConvertEnum( D3D_CULL_MODE *d3d_cull_mode, D3D10_CULL_MODE d3d_cull_mode_10 )
{
    switch( d3d_cull_mode_10 )
    {
        default:
            *d3d_cull_mode = (D3D_CULL_MODE)d3d_cull_mode_10;
            break;
        case D3D10_CULL_NONE:
            *d3d_cull_mode = D3D_CULL_NONE;
            break;
        case D3D10_CULL_FRONT:
            *d3d_cull_mode = D3D_CULL_FRONT;
            break;
        case D3D10_CULL_BACK:
            *d3d_cull_mode = D3D_CULL_BACK;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_CULL_MODE *d3d_cull_mode_10, D3D_CULL_MODE d3d_cull_mode )
{
    switch( d3d_cull_mode )
    {
        default:
            *d3d_cull_mode_10 = (D3D10_CULL_MODE)d3d_cull_mode;
            break;
        case D3D_CULL_NONE:
            *d3d_cull_mode_10 = D3D10_CULL_NONE;
            break;
        case D3D_CULL_FRONT:
            *d3d_cull_mode_10 = D3D10_CULL_FRONT;
            break;
        case D3D_CULL_BACK:
            *d3d_cull_mode_10 = D3D10_CULL_BACK;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_RESOURCE_DIMENSION *d3d_resource_dimension, D3D10_RESOURCE_DIMENSION d3d_resource_dimension_10 )
{
    switch( d3d_resource_dimension_10 )
    {
        default:
            *d3d_resource_dimension = (D3D_RESOURCE_DIMENSION)d3d_resource_dimension_10;
            break;
        case D3D10_RESOURCE_DIMENSION_UNKNOWN:
            *d3d_resource_dimension = D3D_RESOURCE_DIMENSION_UNKNOWN;
            break;
        case D3D10_RESOURCE_DIMENSION_BUFFER:
            *d3d_resource_dimension = D3D_RESOURCE_DIMENSION_BUFFER;
            break;
        case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
            *d3d_resource_dimension = D3D_RESOURCE_DIMENSION_TEXTURE1D;
            break;
        case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
            *d3d_resource_dimension = D3D_RESOURCE_DIMENSION_TEXTURE2D;
            break;
        case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
            *d3d_resource_dimension = D3D_RESOURCE_DIMENSION_TEXTURE3D;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_RESOURCE_DIMENSION *d3d_resource_dimension_10, D3D_RESOURCE_DIMENSION d3d_resource_dimension )
{
    switch( d3d_resource_dimension )
    {
        default:
            *d3d_resource_dimension_10 = (D3D10_RESOURCE_DIMENSION)d3d_resource_dimension;
            break;
        case D3D_RESOURCE_DIMENSION_UNKNOWN:
            *d3d_resource_dimension_10 = D3D10_RESOURCE_DIMENSION_UNKNOWN;
            break;
        case D3D_RESOURCE_DIMENSION_BUFFER:
            *d3d_resource_dimension_10 = D3D10_RESOURCE_DIMENSION_BUFFER;
            break;
        case D3D_RESOURCE_DIMENSION_TEXTURE1D:
            *d3d_resource_dimension_10 = D3D10_RESOURCE_DIMENSION_TEXTURE1D;
            break;
        case D3D_RESOURCE_DIMENSION_TEXTURE2D:
            *d3d_resource_dimension_10 = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
            break;
        case D3D_RESOURCE_DIMENSION_TEXTURE3D:
            *d3d_resource_dimension_10 = D3D10_RESOURCE_DIMENSION_TEXTURE3D;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_SRV_DIMENSION *d3d_srv_dimension, D3D_SRV_DIMENSION d3d_srv_dimension_10 )
{
    *d3d_srv_dimension = d3d_srv_dimension_10;
}

void D3DWrapperConvertEnum( D3D_DSV_DIMENSION *d3d_dsv_dimension, D3D10_DSV_DIMENSION d3d_dsv_dimension_10 )
{
    switch( d3d_dsv_dimension_10 )
    {
        default:
            *d3d_dsv_dimension = (D3D_DSV_DIMENSION)d3d_dsv_dimension_10;
            break;
        case D3D10_DSV_DIMENSION_UNKNOWN:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_UNKNOWN;
            break;
        case D3D10_DSV_DIMENSION_TEXTURE1D:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE1D;
            break;
        case D3D10_DSV_DIMENSION_TEXTURE1DARRAY:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE1DARRAY;
            break;
        case D3D10_DSV_DIMENSION_TEXTURE2D:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE2D;
            break;
        case D3D10_DSV_DIMENSION_TEXTURE2DARRAY:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE2DARRAY;
            break;
        case D3D10_DSV_DIMENSION_TEXTURE2DMS:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE2DMS;
            break;
        case D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE2DMSARRAY;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_DSV_DIMENSION *d3d_dsv_dimension_10, D3D_DSV_DIMENSION d3d_dsv_dimension )
{
    switch( d3d_dsv_dimension )
    {
        default:
            *d3d_dsv_dimension_10 = (D3D10_DSV_DIMENSION)d3d_dsv_dimension;
            break;
        case D3D_DSV_DIMENSION_UNKNOWN:
            *d3d_dsv_dimension_10 = D3D10_DSV_DIMENSION_UNKNOWN;
            break;
        case D3D_DSV_DIMENSION_TEXTURE1D:
            *d3d_dsv_dimension_10 = D3D10_DSV_DIMENSION_TEXTURE1D;
            break;
        case D3D_DSV_DIMENSION_TEXTURE1DARRAY:
            *d3d_dsv_dimension_10 = D3D10_DSV_DIMENSION_TEXTURE1DARRAY;
            break;
        case D3D_DSV_DIMENSION_TEXTURE2D:
            *d3d_dsv_dimension_10 = D3D10_DSV_DIMENSION_TEXTURE2D;
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DARRAY:
            *d3d_dsv_dimension_10 = D3D10_DSV_DIMENSION_TEXTURE2DARRAY;
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DMS:
            *d3d_dsv_dimension_10 = D3D10_DSV_DIMENSION_TEXTURE2DMS;
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DMSARRAY:
            *d3d_dsv_dimension_10 = D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_RTV_DIMENSION *d3d_rtv_dimension, D3D10_RTV_DIMENSION d3d_rtv_dimension_10 )
{
    switch( d3d_rtv_dimension_10 )
    {
        default:
            *d3d_rtv_dimension = (D3D_RTV_DIMENSION)d3d_rtv_dimension_10;
            break;
        case D3D10_RTV_DIMENSION_UNKNOWN:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_UNKNOWN;
            break;
        case D3D10_RTV_DIMENSION_BUFFER:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_BUFFER;
            break;
        case D3D10_RTV_DIMENSION_TEXTURE1D:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE1D;
            break;
        case D3D10_RTV_DIMENSION_TEXTURE1DARRAY:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE1DARRAY;
            break;
        case D3D10_RTV_DIMENSION_TEXTURE2D:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE2D;
            break;
        case D3D10_RTV_DIMENSION_TEXTURE2DARRAY:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE2DARRAY;
            break;
        case D3D10_RTV_DIMENSION_TEXTURE2DMS:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE2DMS;
            break;
        case D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE2DMSARRAY;
            break;
        case D3D10_RTV_DIMENSION_TEXTURE3D:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE3D;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_RTV_DIMENSION *d3d_rtv_dimension_10, D3D_RTV_DIMENSION d3d_rtv_dimension )
{
    switch( d3d_rtv_dimension )
    {
        default:
            *d3d_rtv_dimension_10 = (D3D10_RTV_DIMENSION)d3d_rtv_dimension;
            break;
        case D3D_RTV_DIMENSION_UNKNOWN:
            *d3d_rtv_dimension_10 = D3D10_RTV_DIMENSION_UNKNOWN;
            break;
        case D3D_RTV_DIMENSION_BUFFER:
            *d3d_rtv_dimension_10 = D3D10_RTV_DIMENSION_BUFFER;
            break;
        case D3D_RTV_DIMENSION_TEXTURE1D:
            *d3d_rtv_dimension_10 = D3D10_RTV_DIMENSION_TEXTURE1D;
            break;
        case D3D_RTV_DIMENSION_TEXTURE1DARRAY:
            *d3d_rtv_dimension_10 = D3D10_RTV_DIMENSION_TEXTURE1DARRAY;
            break;
        case D3D_RTV_DIMENSION_TEXTURE2D:
            *d3d_rtv_dimension_10 = D3D10_RTV_DIMENSION_TEXTURE2D;
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DARRAY:
            *d3d_rtv_dimension_10 = D3D10_RTV_DIMENSION_TEXTURE2DARRAY;
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DMS:
            *d3d_rtv_dimension_10 = D3D10_RTV_DIMENSION_TEXTURE2DMS;
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DMSARRAY:
            *d3d_rtv_dimension_10 = D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY;
            break;
        case D3D_RTV_DIMENSION_TEXTURE3D:
            *d3d_rtv_dimension_10 = D3D10_RTV_DIMENSION_TEXTURE3D;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_USAGE *d3d_usage, D3D10_USAGE d3d_usage_10 )
{
    switch( d3d_usage_10 )
    {
        default:
            *d3d_usage = (D3D_USAGE)d3d_usage_10;
            break;
        case D3D10_USAGE_DEFAULT:
            *d3d_usage = D3D_USAGE_DEFAULT;
            break;
        case D3D10_USAGE_IMMUTABLE:
            *d3d_usage = D3D_USAGE_IMMUTABLE;
            break;
        case D3D10_USAGE_DYNAMIC:
            *d3d_usage = D3D_USAGE_DYNAMIC;
            break;
        case D3D10_USAGE_STAGING:
            *d3d_usage = D3D_USAGE_STAGING;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_USAGE *d3d_usage_10, D3D_USAGE d3d_usage )
{
    switch( d3d_usage )
    {
        default:
            *d3d_usage_10 = (D3D10_USAGE)d3d_usage;
            break;
        case D3D_USAGE_DEFAULT:
            *d3d_usage_10 = D3D10_USAGE_DEFAULT;
            break;
        case D3D_USAGE_IMMUTABLE:
            *d3d_usage_10 = D3D10_USAGE_IMMUTABLE;
            break;
        case D3D_USAGE_DYNAMIC:
            *d3d_usage_10 = D3D10_USAGE_DYNAMIC;
            break;
        case D3D_USAGE_STAGING:
            *d3d_usage_10 = D3D10_USAGE_STAGING;
            break;
    }
}

void D3DFlagsConvertFrom10_BIND_FLAG( UINT *bind_flag, UINT bind_flag_10 )
{
    *bind_flag = 0;
    if( bind_flag_10 & D3D10_BIND_VERTEX_BUFFER )
        *bind_flag |= D3D_BIND_VERTEX_BUFFER;
    if( bind_flag_10 & D3D10_BIND_INDEX_BUFFER )
        *bind_flag |= D3D_BIND_INDEX_BUFFER;
    if( bind_flag_10 & D3D10_BIND_CONSTANT_BUFFER )
        *bind_flag |= D3D_BIND_CONSTANT_BUFFER;
    if( bind_flag_10 & D3D10_BIND_SHADER_RESOURCE )
        *bind_flag |= D3D_BIND_SHADER_RESOURCE;
    if( bind_flag_10 & D3D10_BIND_STREAM_OUTPUT )
        *bind_flag |= D3D_BIND_STREAM_OUTPUT;
    if( bind_flag_10 & D3D10_BIND_RENDER_TARGET )
        *bind_flag |= D3D_BIND_RENDER_TARGET;
    if( bind_flag_10 & D3D10_BIND_DEPTH_STENCIL )
        *bind_flag |= D3D_BIND_DEPTH_STENCIL;
}

void D3DFlagsConvertTo10_BIND_FLAG( UINT *bind_flag_10, UINT bind_flag )
{
    *bind_flag_10 = 0;
    if( bind_flag & D3D_BIND_VERTEX_BUFFER )
        *bind_flag_10 |= D3D10_BIND_VERTEX_BUFFER;
    if( bind_flag & D3D_BIND_INDEX_BUFFER )
        *bind_flag_10 |= D3D10_BIND_INDEX_BUFFER;
    if( bind_flag & D3D_BIND_CONSTANT_BUFFER )
        *bind_flag_10 |= D3D10_BIND_CONSTANT_BUFFER;
    if( bind_flag & D3D_BIND_SHADER_RESOURCE )
        *bind_flag_10 |= D3D10_BIND_SHADER_RESOURCE;
    if( bind_flag & D3D_BIND_STREAM_OUTPUT )
        *bind_flag_10 |= D3D10_BIND_STREAM_OUTPUT;
    if( bind_flag & D3D_BIND_RENDER_TARGET )
        *bind_flag_10 |= D3D10_BIND_RENDER_TARGET;
    if( bind_flag & D3D_BIND_DEPTH_STENCIL )
        *bind_flag_10 |= D3D10_BIND_DEPTH_STENCIL;
}

void D3DFlagsConvertFrom10_CPU_ACCESS_FLAG( UINT *cpu_access_flag, UINT cpu_access_flag_10 )
{
    *cpu_access_flag = 0;
    if( cpu_access_flag_10 & D3D10_CPU_ACCESS_WRITE )
        *cpu_access_flag |= D3D_CPU_ACCESS_WRITE;
    if( cpu_access_flag_10 & D3D10_CPU_ACCESS_READ )
        *cpu_access_flag |= D3D_CPU_ACCESS_READ;
}

void D3DFlagsConvertTo10_CPU_ACCESS_FLAG( UINT *cpu_access_flag_10, UINT cpu_access_flag )
{
    *cpu_access_flag_10 = 0;
    if( cpu_access_flag & D3D_CPU_ACCESS_WRITE )
        *cpu_access_flag_10 |= D3D10_CPU_ACCESS_WRITE;
    if( cpu_access_flag & D3D_CPU_ACCESS_READ )
        *cpu_access_flag_10 |= D3D10_CPU_ACCESS_READ;
}

void D3DFlagsConvertFrom10_RESOURCE_MISC_FLAG( UINT *resource_misc_flag, UINT resource_misc_flag_10 )
{
    *resource_misc_flag = 0;
    if( resource_misc_flag_10 & D3D10_RESOURCE_MISC_GENERATE_MIPS )
        *resource_misc_flag |= D3D_RESOURCE_MISC_GENERATE_MIPS;
    if( resource_misc_flag_10 & D3D10_RESOURCE_MISC_SHARED )
        *resource_misc_flag |= D3D_RESOURCE_MISC_SHARED;
    if( resource_misc_flag_10 & D3D10_RESOURCE_MISC_TEXTURECUBE )
        *resource_misc_flag |= D3D_RESOURCE_MISC_TEXTURECUBE;
    if( resource_misc_flag_10 & D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX )
        *resource_misc_flag |= D3D_RESOURCE_MISC_SHARED_KEYEDMUTEX;
    if( resource_misc_flag_10 & D3D10_RESOURCE_MISC_GDI_COMPATIBLE )
        *resource_misc_flag |= D3D_RESOURCE_MISC_GDI_COMPATIBLE;
}

void D3DFlagsConvertTo10_RESOURCE_MISC_FLAG( UINT *resource_misc_flag_10, UINT resource_misc_flag )
{
    *resource_misc_flag_10 = 0;
    if( resource_misc_flag & D3D_RESOURCE_MISC_GENERATE_MIPS )
        *resource_misc_flag_10 |= D3D10_RESOURCE_MISC_GENERATE_MIPS;
    if( resource_misc_flag & D3D_RESOURCE_MISC_SHARED )
        *resource_misc_flag_10 |= D3D10_RESOURCE_MISC_SHARED;
    if( resource_misc_flag & D3D_RESOURCE_MISC_TEXTURECUBE )
        *resource_misc_flag_10 |= D3D10_RESOURCE_MISC_TEXTURECUBE;
    if( resource_misc_flag & D3D_RESOURCE_MISC_SHARED_KEYEDMUTEX )
        *resource_misc_flag_10 |= D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX;
    if( resource_misc_flag & D3D_RESOURCE_MISC_GDI_COMPATIBLE )
        *resource_misc_flag_10 |= D3D10_RESOURCE_MISC_GDI_COMPATIBLE;
}

void D3DWrapperConvertEnum( D3D_MAP *d3d_map, D3D10_MAP d3d_map_10 )
{
    switch( d3d_map_10 )
    {
        default:
            *d3d_map = (D3D_MAP)d3d_map_10;
            break;
        case D3D10_MAP_READ:
            *d3d_map = D3D_MAP_READ;
            break;
        case D3D10_MAP_WRITE:
            *d3d_map = D3D_MAP_WRITE;
            break;
        case D3D10_MAP_READ_WRITE:
            *d3d_map = D3D_MAP_READ_WRITE;
            break;
        case D3D10_MAP_WRITE_DISCARD:
            *d3d_map = D3D_MAP_WRITE_DISCARD;
            break;
        case D3D10_MAP_WRITE_NO_OVERWRITE:
            *d3d_map = D3D_MAP_WRITE_NO_OVERWRITE;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_MAP *d3d_map_10, D3D_MAP d3d_map )
{
    switch( d3d_map )
    {
        default:
            *d3d_map_10 = (D3D10_MAP)d3d_map;
            break;
        case D3D_MAP_READ:
            *d3d_map_10 = D3D10_MAP_READ;
            break;
        case D3D_MAP_WRITE:
            *d3d_map_10 = D3D10_MAP_WRITE;
            break;
        case D3D_MAP_READ_WRITE:
            *d3d_map_10 = D3D10_MAP_READ_WRITE;
            break;
        case D3D_MAP_WRITE_DISCARD:
            *d3d_map_10 = D3D10_MAP_WRITE_DISCARD;
            break;
        case D3D_MAP_WRITE_NO_OVERWRITE:
            *d3d_map_10 = D3D10_MAP_WRITE_NO_OVERWRITE;
            break;
    }
}

void D3DFlagsConvertFrom10_MAP_FLAG( UINT *map_flag, UINT map_flag_10 )
{
    *map_flag = 0;
    if( map_flag_10 & D3D10_MAP_FLAG_DO_NOT_WAIT )
        *map_flag |= D3D_MAP_FLAG_DO_NOT_WAIT;
}

void D3DFlagsConvertTo10_MAP_FLAG( UINT *map_flag_10, UINT map_flag )
{
    *map_flag_10 = 0;
    if( map_flag & D3D_MAP_FLAG_DO_NOT_WAIT )
        *map_flag_10 |= D3D10_MAP_FLAG_DO_NOT_WAIT;
}

void D3DFlagsConvertFrom10_RAISE_FLAG( UINT *raise_flag, UINT raise_flag_10 )
{
    *raise_flag = 0;
    if( raise_flag_10 & D3D10_RAISE_FLAG_DRIVER_INTERNAL_ERROR )
        *raise_flag |= D3D_RAISE_FLAG_DRIVER_INTERNAL_ERROR;
}

void D3DFlagsConvertTo10_RAISE_FLAG( UINT *raise_flag_10, UINT raise_flag )
{
    *raise_flag_10 = 0;
    if( raise_flag & D3D_RAISE_FLAG_DRIVER_INTERNAL_ERROR )
        *raise_flag_10 |= D3D10_RAISE_FLAG_DRIVER_INTERNAL_ERROR;
}

void D3DFlagsConvertFrom10_CLEAR_FLAG( UINT *clear_flag, UINT clear_flag_10 )
{
    *clear_flag = 0;
    if( clear_flag_10 & D3D10_CLEAR_DEPTH )
        *clear_flag |= D3D_CLEAR_DEPTH;
    if( clear_flag_10 & D3D10_CLEAR_STENCIL )
        *clear_flag |= D3D_CLEAR_STENCIL;
}

void D3DFlagsConvertTo10_CLEAR_FLAG( UINT *clear_flag_10, UINT clear_flag )
{
    *clear_flag_10 = 0;
    if( clear_flag & D3D_CLEAR_DEPTH )
        *clear_flag_10 |= D3D10_CLEAR_DEPTH;
    if( clear_flag & D3D_CLEAR_STENCIL )
        *clear_flag_10 |= D3D10_CLEAR_STENCIL;
}

void D3DWrapperConvertEnum( D3D_COMPARISON_FUNC *d3d_comparison_func, D3D10_COMPARISON_FUNC d3d_comparison_func_10 )
{
    switch( d3d_comparison_func_10 )
    {
        default:
            *d3d_comparison_func = (D3D_COMPARISON_FUNC)d3d_comparison_func_10;
            break;
        case D3D10_COMPARISON_NEVER:
            *d3d_comparison_func = D3D_COMPARISON_NEVER;
            break;
        case D3D10_COMPARISON_LESS:
            *d3d_comparison_func = D3D_COMPARISON_LESS;
            break;
        case D3D10_COMPARISON_EQUAL:
            *d3d_comparison_func = D3D_COMPARISON_EQUAL;
            break;
        case D3D10_COMPARISON_LESS_EQUAL:
            *d3d_comparison_func = D3D_COMPARISON_LESS_EQUAL;
            break;
        case D3D10_COMPARISON_GREATER:
            *d3d_comparison_func = D3D_COMPARISON_GREATER;
            break;
        case D3D10_COMPARISON_NOT_EQUAL:
            *d3d_comparison_func = D3D_COMPARISON_NOT_EQUAL;
            break;
        case D3D10_COMPARISON_GREATER_EQUAL:
            *d3d_comparison_func = D3D_COMPARISON_GREATER_EQUAL;
            break;
        case D3D10_COMPARISON_ALWAYS:
            *d3d_comparison_func = D3D_COMPARISON_ALWAYS;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_COMPARISON_FUNC *d3d_comparison_func_10, D3D_COMPARISON_FUNC d3d_comparison_func )
{
    switch( d3d_comparison_func )
    {
        default:
            *d3d_comparison_func_10 = (D3D10_COMPARISON_FUNC)d3d_comparison_func;
            break;
        case D3D_COMPARISON_NEVER:
            *d3d_comparison_func_10 = D3D10_COMPARISON_NEVER;
            break;
        case D3D_COMPARISON_LESS:
            *d3d_comparison_func_10 = D3D10_COMPARISON_LESS;
            break;
        case D3D_COMPARISON_EQUAL:
            *d3d_comparison_func_10 = D3D10_COMPARISON_EQUAL;
            break;
        case D3D_COMPARISON_LESS_EQUAL:
            *d3d_comparison_func_10 = D3D10_COMPARISON_LESS_EQUAL;
            break;
        case D3D_COMPARISON_GREATER:
            *d3d_comparison_func_10 = D3D10_COMPARISON_GREATER;
            break;
        case D3D_COMPARISON_NOT_EQUAL:
            *d3d_comparison_func_10 = D3D10_COMPARISON_NOT_EQUAL;
            break;
        case D3D_COMPARISON_GREATER_EQUAL:
            *d3d_comparison_func_10 = D3D10_COMPARISON_GREATER_EQUAL;
            break;
        case D3D_COMPARISON_ALWAYS:
            *d3d_comparison_func_10 = D3D10_COMPARISON_ALWAYS;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_DEPTH_WRITE_MASK *d3d_depth_write_mask, D3D10_DEPTH_WRITE_MASK d3d_depth_write_mask_10 )
{
    switch( d3d_depth_write_mask_10 )
    {
        default:
            *d3d_depth_write_mask = (D3D_DEPTH_WRITE_MASK)d3d_depth_write_mask_10;
            break;
        case D3D10_DEPTH_WRITE_MASK_ZERO:
            *d3d_depth_write_mask = D3D_DEPTH_WRITE_MASK_ZERO;
            break;
        case D3D10_DEPTH_WRITE_MASK_ALL:
            *d3d_depth_write_mask = D3D_DEPTH_WRITE_MASK_ALL;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_DEPTH_WRITE_MASK *d3d_depth_write_mask_10, D3D_DEPTH_WRITE_MASK d3d_depth_write_mask )
{
    switch( d3d_depth_write_mask )
    {
        default:
            *d3d_depth_write_mask_10 = (D3D10_DEPTH_WRITE_MASK)d3d_depth_write_mask;
            break;
        case D3D_DEPTH_WRITE_MASK_ZERO:
            *d3d_depth_write_mask_10 = D3D10_DEPTH_WRITE_MASK_ZERO;
            break;
        case D3D_DEPTH_WRITE_MASK_ALL:
            *d3d_depth_write_mask_10 = D3D10_DEPTH_WRITE_MASK_ALL;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_STENCIL_OP *d3d_stencil_op, D3D10_STENCIL_OP d3d_stencil_op_10 )
{
    switch( d3d_stencil_op_10 )
    {
        default:
            *d3d_stencil_op = (D3D_STENCIL_OP)d3d_stencil_op_10;
            break;
        case D3D10_STENCIL_OP_KEEP:
            *d3d_stencil_op = D3D_STENCIL_OP_KEEP;
            break;
        case D3D10_STENCIL_OP_ZERO:
            *d3d_stencil_op = D3D_STENCIL_OP_ZERO;
            break;
        case D3D10_STENCIL_OP_REPLACE:
            *d3d_stencil_op = D3D_STENCIL_OP_REPLACE;
            break;
        case D3D10_STENCIL_OP_INCR_SAT:
            *d3d_stencil_op = D3D_STENCIL_OP_INCR_SAT;
            break;
        case D3D10_STENCIL_OP_DECR_SAT:
            *d3d_stencil_op = D3D_STENCIL_OP_DECR_SAT;
            break;
        case D3D10_STENCIL_OP_INVERT:
            *d3d_stencil_op = D3D_STENCIL_OP_INVERT;
            break;
        case D3D10_STENCIL_OP_INCR:
            *d3d_stencil_op = D3D_STENCIL_OP_INCR;
            break;
        case D3D10_STENCIL_OP_DECR:
            *d3d_stencil_op = D3D_STENCIL_OP_DECR;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_STENCIL_OP *d3d_stencil_op_10, D3D_STENCIL_OP d3d_stencil_op )
{
    switch( d3d_stencil_op )
    {
        default:
            *d3d_stencil_op_10 = (D3D10_STENCIL_OP)d3d_stencil_op;
            break;
        case D3D_STENCIL_OP_KEEP:
            *d3d_stencil_op_10 = D3D10_STENCIL_OP_KEEP;
            break;
        case D3D_STENCIL_OP_ZERO:
            *d3d_stencil_op_10 = D3D10_STENCIL_OP_ZERO;
            break;
        case D3D_STENCIL_OP_REPLACE:
            *d3d_stencil_op_10 = D3D10_STENCIL_OP_REPLACE;
            break;
        case D3D_STENCIL_OP_INCR_SAT:
            *d3d_stencil_op_10 = D3D10_STENCIL_OP_INCR_SAT;
            break;
        case D3D_STENCIL_OP_DECR_SAT:
            *d3d_stencil_op_10 = D3D10_STENCIL_OP_DECR_SAT;
            break;
        case D3D_STENCIL_OP_INVERT:
            *d3d_stencil_op_10 = D3D10_STENCIL_OP_INVERT;
            break;
        case D3D_STENCIL_OP_INCR:
            *d3d_stencil_op_10 = D3D10_STENCIL_OP_INCR;
            break;
        case D3D_STENCIL_OP_DECR:
            *d3d_stencil_op_10 = D3D10_STENCIL_OP_DECR;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_BLEND *d3d_blend, D3D10_BLEND d3d_blend_10 )
{
    switch( d3d_blend_10 )
    {
        default:
            *d3d_blend = (D3D_BLEND)d3d_blend_10;
            break;
        case D3D10_BLEND_ZERO:
            *d3d_blend = D3D_BLEND_ZERO;
            break;
        case D3D10_BLEND_ONE:
            *d3d_blend = D3D_BLEND_ONE;
            break;
        case D3D10_BLEND_SRC_COLOR:
            *d3d_blend = D3D_BLEND_SRC_COLOR;
            break;
        case D3D10_BLEND_INV_SRC_COLOR:
            *d3d_blend = D3D_BLEND_INV_SRC_COLOR;
            break;
        case D3D10_BLEND_SRC_ALPHA:
            *d3d_blend = D3D_BLEND_SRC_ALPHA;
            break;
        case D3D10_BLEND_INV_SRC_ALPHA:
            *d3d_blend = D3D_BLEND_INV_SRC_ALPHA;
            break;
        case D3D10_BLEND_DEST_ALPHA:
            *d3d_blend = D3D_BLEND_DEST_ALPHA;
            break;
        case D3D10_BLEND_INV_DEST_ALPHA:
            *d3d_blend = D3D_BLEND_INV_DEST_ALPHA;
            break;
        case D3D10_BLEND_DEST_COLOR:
            *d3d_blend = D3D_BLEND_DEST_COLOR;
            break;
        case D3D10_BLEND_INV_DEST_COLOR:
            *d3d_blend = D3D_BLEND_INV_DEST_COLOR;
            break;
        case D3D10_BLEND_SRC_ALPHA_SAT:
            *d3d_blend = D3D_BLEND_SRC_ALPHA_SAT;
            break;
        case D3D10_BLEND_BLEND_FACTOR:
            *d3d_blend = D3D_BLEND_BLEND_FACTOR;
            break;
        case D3D10_BLEND_INV_BLEND_FACTOR:
            *d3d_blend = D3D_BLEND_INV_BLEND_FACTOR;
            break;
        case D3D10_BLEND_SRC1_COLOR:
            *d3d_blend = D3D_BLEND_SRC1_COLOR;
            break;
        case D3D10_BLEND_INV_SRC1_COLOR:
            *d3d_blend = D3D_BLEND_INV_SRC1_COLOR;
            break;
        case D3D10_BLEND_SRC1_ALPHA:
            *d3d_blend = D3D_BLEND_SRC1_ALPHA;
            break;
        case D3D10_BLEND_INV_SRC1_ALPHA:
            *d3d_blend = D3D_BLEND_INV_SRC1_ALPHA;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_BLEND *d3d_blend_10, D3D_BLEND d3d_blend )
{
    switch( d3d_blend )
    {
        default:
            *d3d_blend_10 = (D3D10_BLEND)d3d_blend;
            break;
        case D3D_BLEND_ZERO:
            *d3d_blend_10 = D3D10_BLEND_ZERO;
            break;
        case D3D_BLEND_ONE:
            *d3d_blend_10 = D3D10_BLEND_ONE;
            break;
        case D3D_BLEND_SRC_COLOR:
            *d3d_blend_10 = D3D10_BLEND_SRC_COLOR;
            break;
        case D3D_BLEND_INV_SRC_COLOR:
            *d3d_blend_10 = D3D10_BLEND_INV_SRC_COLOR;
            break;
        case D3D_BLEND_SRC_ALPHA:
            *d3d_blend_10 = D3D10_BLEND_SRC_ALPHA;
            break;
        case D3D_BLEND_INV_SRC_ALPHA:
            *d3d_blend_10 = D3D10_BLEND_INV_SRC_ALPHA;
            break;
        case D3D_BLEND_DEST_ALPHA:
            *d3d_blend_10 = D3D10_BLEND_DEST_ALPHA;
            break;
        case D3D_BLEND_INV_DEST_ALPHA:
            *d3d_blend_10 = D3D10_BLEND_INV_DEST_ALPHA;
            break;
        case D3D_BLEND_DEST_COLOR:
            *d3d_blend_10 = D3D10_BLEND_DEST_COLOR;
            break;
        case D3D_BLEND_INV_DEST_COLOR:
            *d3d_blend_10 = D3D10_BLEND_INV_DEST_COLOR;
            break;
        case D3D_BLEND_SRC_ALPHA_SAT:
            *d3d_blend_10 = D3D10_BLEND_SRC_ALPHA_SAT;
            break;
        case D3D_BLEND_BLEND_FACTOR:
            *d3d_blend_10 = D3D10_BLEND_BLEND_FACTOR;
            break;
        case D3D_BLEND_INV_BLEND_FACTOR:
            *d3d_blend_10 = D3D10_BLEND_INV_BLEND_FACTOR;
            break;
        case D3D_BLEND_SRC1_COLOR:
            *d3d_blend_10 = D3D10_BLEND_SRC1_COLOR;
            break;
        case D3D_BLEND_INV_SRC1_COLOR:
            *d3d_blend_10 = D3D10_BLEND_INV_SRC1_COLOR;
            break;
        case D3D_BLEND_SRC1_ALPHA:
            *d3d_blend_10 = D3D10_BLEND_SRC1_ALPHA;
            break;
        case D3D_BLEND_INV_SRC1_ALPHA:
            *d3d_blend_10 = D3D10_BLEND_INV_SRC1_ALPHA;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_BLEND_OP *d3d_blend_op, D3D10_BLEND_OP d3d_blend_op_10 )
{
    switch( d3d_blend_op_10 )
    {
        default:
            *d3d_blend_op = (D3D_BLEND_OP)d3d_blend_op_10;
            break;
        case D3D10_BLEND_OP_ADD:
            *d3d_blend_op = D3D_BLEND_OP_ADD;
            break;
        case D3D10_BLEND_OP_SUBTRACT:
            *d3d_blend_op = D3D_BLEND_OP_SUBTRACT;
            break;
        case D3D10_BLEND_OP_REV_SUBTRACT:
            *d3d_blend_op = D3D_BLEND_OP_REV_SUBTRACT;
            break;
        case D3D10_BLEND_OP_MIN:
            *d3d_blend_op = D3D_BLEND_OP_MIN;
            break;
        case D3D10_BLEND_OP_MAX:
            *d3d_blend_op = D3D_BLEND_OP_MAX;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_BLEND_OP *d3d_blend_op_10, D3D_BLEND_OP d3d_blend_op )
{
    switch( d3d_blend_op )
    {
        default:
            *d3d_blend_op_10 = (D3D10_BLEND_OP)d3d_blend_op;
            break;
        case D3D_BLEND_OP_ADD:
            *d3d_blend_op_10 = D3D10_BLEND_OP_ADD;
            break;
        case D3D_BLEND_OP_SUBTRACT:
            *d3d_blend_op_10 = D3D10_BLEND_OP_SUBTRACT;
            break;
        case D3D_BLEND_OP_REV_SUBTRACT:
            *d3d_blend_op_10 = D3D10_BLEND_OP_REV_SUBTRACT;
            break;
        case D3D_BLEND_OP_MIN:
            *d3d_blend_op_10 = D3D10_BLEND_OP_MIN;
            break;
        case D3D_BLEND_OP_MAX:
            *d3d_blend_op_10 = D3D10_BLEND_OP_MAX;
            break;
    }
}

void D3DFlagsConvertFrom10_COLOR_WRITE_ENABLE( UINT8 *color_write_enable, UINT8 color_write_enable_10 )
{
    *color_write_enable = 0;
    if( color_write_enable_10 & D3D10_COLOR_WRITE_ENABLE_RED )
        *color_write_enable |= D3D_COLOR_WRITE_ENABLE_RED;
    if( color_write_enable_10 & D3D10_COLOR_WRITE_ENABLE_GREEN )
        *color_write_enable |= D3D_COLOR_WRITE_ENABLE_GREEN;
    if( color_write_enable_10 & D3D10_COLOR_WRITE_ENABLE_BLUE )
        *color_write_enable |= D3D_COLOR_WRITE_ENABLE_BLUE;
    if( color_write_enable_10 & D3D10_COLOR_WRITE_ENABLE_ALPHA )
        *color_write_enable |= D3D_COLOR_WRITE_ENABLE_ALPHA;
}

void D3DFlagsConvertTo10_COLOR_WRITE_ENABLE( UINT8 *color_write_enable_10, UINT8 color_write_enable )
{
    *color_write_enable_10 = 0;
    if( color_write_enable & D3D_COLOR_WRITE_ENABLE_RED )
        *color_write_enable_10 |= D3D10_COLOR_WRITE_ENABLE_RED;
    if( color_write_enable & D3D_COLOR_WRITE_ENABLE_GREEN )
        *color_write_enable_10 |= D3D10_COLOR_WRITE_ENABLE_GREEN;
    if( color_write_enable & D3D_COLOR_WRITE_ENABLE_BLUE )
        *color_write_enable_10 |= D3D10_COLOR_WRITE_ENABLE_BLUE;
    if( color_write_enable & D3D_COLOR_WRITE_ENABLE_ALPHA )
        *color_write_enable_10 |= D3D10_COLOR_WRITE_ENABLE_ALPHA;
}

void D3DWrapperConvertEnum( D3D_TEXTURECUBE_FACE *d3d_texturecube_face, D3D10_TEXTURECUBE_FACE d3d_texturecube_face_10 )
{
    switch( d3d_texturecube_face_10 )
    {
        default:
            *d3d_texturecube_face = (D3D_TEXTURECUBE_FACE)d3d_texturecube_face_10;
            break;
        case D3D10_TEXTURECUBE_FACE_POSITIVE_X:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_POSITIVE_X;
            break;
        case D3D10_TEXTURECUBE_FACE_NEGATIVE_X:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_NEGATIVE_X;
            break;
        case D3D10_TEXTURECUBE_FACE_POSITIVE_Y:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_POSITIVE_Y;
            break;
        case D3D10_TEXTURECUBE_FACE_NEGATIVE_Y:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_NEGATIVE_Y;
            break;
        case D3D10_TEXTURECUBE_FACE_POSITIVE_Z:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_POSITIVE_Z;
            break;
        case D3D10_TEXTURECUBE_FACE_NEGATIVE_Z:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_NEGATIVE_Z;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_TEXTURECUBE_FACE *d3d_texturecube_face_10, D3D_TEXTURECUBE_FACE d3d_texturecube_face )
{
    switch( d3d_texturecube_face )
    {
        default:
            *d3d_texturecube_face_10 = (D3D10_TEXTURECUBE_FACE)d3d_texturecube_face;
            break;
        case D3D_TEXTURECUBE_FACE_POSITIVE_X:
            *d3d_texturecube_face_10 = D3D10_TEXTURECUBE_FACE_POSITIVE_X;
            break;
        case D3D_TEXTURECUBE_FACE_NEGATIVE_X:
            *d3d_texturecube_face_10 = D3D10_TEXTURECUBE_FACE_NEGATIVE_X;
            break;
        case D3D_TEXTURECUBE_FACE_POSITIVE_Y:
            *d3d_texturecube_face_10 = D3D10_TEXTURECUBE_FACE_POSITIVE_Y;
            break;
        case D3D_TEXTURECUBE_FACE_NEGATIVE_Y:
            *d3d_texturecube_face_10 = D3D10_TEXTURECUBE_FACE_NEGATIVE_Y;
            break;
        case D3D_TEXTURECUBE_FACE_POSITIVE_Z:
            *d3d_texturecube_face_10 = D3D10_TEXTURECUBE_FACE_POSITIVE_Z;
            break;
        case D3D_TEXTURECUBE_FACE_NEGATIVE_Z:
            *d3d_texturecube_face_10 = D3D10_TEXTURECUBE_FACE_NEGATIVE_Z;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_FILTER *d3d_filter, D3D10_FILTER d3d_filter_10 )
{
    switch( d3d_filter_10 )
    {
        default:
            *d3d_filter = (D3D_FILTER)d3d_filter_10;
            break;
        case D3D10_FILTER_MIN_MAG_MIP_POINT:
            *d3d_filter = D3D_FILTER_MIN_MAG_MIP_POINT;
            break;
        case D3D10_FILTER_MIN_MAG_POINT_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_MIN_MAG_POINT_MIP_LINEAR;
            break;
        case D3D10_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
            *d3d_filter = D3D_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
            break;
        case D3D10_FILTER_MIN_POINT_MAG_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_MIN_POINT_MAG_MIP_LINEAR;
            break;
        case D3D10_FILTER_MIN_LINEAR_MAG_MIP_POINT:
            *d3d_filter = D3D_FILTER_MIN_LINEAR_MAG_MIP_POINT;
            break;
        case D3D10_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            break;
        case D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT:
            *d3d_filter = D3D_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            break;
        case D3D10_FILTER_MIN_MAG_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        case D3D10_FILTER_ANISOTROPIC:
            *d3d_filter = D3D_FILTER_ANISOTROPIC;
            break;
        case D3D10_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
            break;
        case D3D10_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
            break;
        case D3D10_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
            break;
        case D3D10_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
            break;
        case D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
            break;
        case D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            break;
        case D3D10_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
            break;
        case D3D10_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
            break;
        case D3D10_FILTER_COMPARISON_ANISOTROPIC:
            *d3d_filter = D3D_FILTER_COMPARISON_ANISOTROPIC;
            break;
        case D3D10_FILTER_TEXT_1BIT:
            *d3d_filter = D3D_FILTER_TEXT_1BIT;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_FILTER *d3d_filter_10, D3D_FILTER d3d_filter )
{
    switch( d3d_filter )
    {
        default:
            *d3d_filter_10 = (D3D10_FILTER)d3d_filter;
            break;
        case D3D_FILTER_MIN_MAG_MIP_POINT:
            *d3d_filter_10 = D3D10_FILTER_MIN_MAG_MIP_POINT;
            break;
        case D3D_FILTER_MIN_MAG_POINT_MIP_LINEAR:
            *d3d_filter_10 = D3D10_FILTER_MIN_MAG_POINT_MIP_LINEAR;
            break;
        case D3D_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
            *d3d_filter_10 = D3D10_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
            break;
        case D3D_FILTER_MIN_POINT_MAG_MIP_LINEAR:
            *d3d_filter_10 = D3D10_FILTER_MIN_POINT_MAG_MIP_LINEAR;
            break;
        case D3D_FILTER_MIN_LINEAR_MAG_MIP_POINT:
            *d3d_filter_10 = D3D10_FILTER_MIN_LINEAR_MAG_MIP_POINT;
            break;
        case D3D_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
            *d3d_filter_10 = D3D10_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            break;
        case D3D_FILTER_MIN_MAG_LINEAR_MIP_POINT:
            *d3d_filter_10 = D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            break;
        case D3D_FILTER_MIN_MAG_MIP_LINEAR:
            *d3d_filter_10 = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        case D3D_FILTER_ANISOTROPIC:
            *d3d_filter_10 = D3D10_FILTER_ANISOTROPIC;
            break;
        case D3D_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
            *d3d_filter_10 = D3D10_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
            break;
        case D3D_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
            *d3d_filter_10 = D3D10_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
            break;
        case D3D_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
            *d3d_filter_10 = D3D10_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
            break;
        case D3D_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
            *d3d_filter_10 = D3D10_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
            break;
        case D3D_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
            *d3d_filter_10 = D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
            break;
        case D3D_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
            *d3d_filter_10 = D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            break;
        case D3D_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
            *d3d_filter_10 = D3D10_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
            break;
        case D3D_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
            *d3d_filter_10 = D3D10_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
            break;
        case D3D_FILTER_COMPARISON_ANISOTROPIC:
            *d3d_filter_10 = D3D10_FILTER_COMPARISON_ANISOTROPIC;
            break;
        case D3D_FILTER_TEXT_1BIT:
            *d3d_filter_10 = D3D10_FILTER_TEXT_1BIT;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_FILTER_TYPE *d3d_filter_type, D3D10_FILTER_TYPE d3d_filter_type_10 )
{
    switch( d3d_filter_type_10 )
    {
        default:
            *d3d_filter_type = (D3D_FILTER_TYPE)d3d_filter_type_10;
            break;
        case D3D10_FILTER_TYPE_POINT:
            *d3d_filter_type = D3D_FILTER_TYPE_POINT;
            break;
        case D3D10_FILTER_TYPE_LINEAR:
            *d3d_filter_type = D3D_FILTER_TYPE_LINEAR;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_FILTER_TYPE *d3d_filter_type_10, D3D_FILTER_TYPE d3d_filter_type )
{
    switch( d3d_filter_type )
    {
        default:
            *d3d_filter_type_10 = (D3D10_FILTER_TYPE)d3d_filter_type;
            break;
        case D3D_FILTER_TYPE_POINT:
            *d3d_filter_type_10 = D3D10_FILTER_TYPE_POINT;
            break;
        case D3D_FILTER_TYPE_LINEAR:
            *d3d_filter_type_10 = D3D10_FILTER_TYPE_LINEAR;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_TEXTURE_ADDRESS_MODE *d3d_texture_address_mode, D3D10_TEXTURE_ADDRESS_MODE d3d_texture_address_mode_10 )
{
    switch( d3d_texture_address_mode_10 )
    {
        default:
            *d3d_texture_address_mode = (D3D_TEXTURE_ADDRESS_MODE)d3d_texture_address_mode_10;
            break;
        case D3D10_TEXTURE_ADDRESS_WRAP:
            *d3d_texture_address_mode = D3D_TEXTURE_ADDRESS_WRAP;
            break;
        case D3D10_TEXTURE_ADDRESS_MIRROR:
            *d3d_texture_address_mode = D3D_TEXTURE_ADDRESS_MIRROR;
            break;
        case D3D10_TEXTURE_ADDRESS_CLAMP:
            *d3d_texture_address_mode = D3D_TEXTURE_ADDRESS_CLAMP;
            break;
        case D3D10_TEXTURE_ADDRESS_BORDER:
            *d3d_texture_address_mode = D3D_TEXTURE_ADDRESS_BORDER;
            break;
        case D3D10_TEXTURE_ADDRESS_MIRROR_ONCE:
            *d3d_texture_address_mode = D3D_TEXTURE_ADDRESS_MIRROR_ONCE;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_TEXTURE_ADDRESS_MODE *d3d_texture_address_mode_10, D3D_TEXTURE_ADDRESS_MODE d3d_texture_address_mode )
{
    switch( d3d_texture_address_mode )
    {
        default:
            *d3d_texture_address_mode_10 = (D3D10_TEXTURE_ADDRESS_MODE)d3d_texture_address_mode;
            break;
        case D3D_TEXTURE_ADDRESS_WRAP:
            *d3d_texture_address_mode_10 = D3D10_TEXTURE_ADDRESS_WRAP;
            break;
        case D3D_TEXTURE_ADDRESS_MIRROR:
            *d3d_texture_address_mode_10 = D3D10_TEXTURE_ADDRESS_MIRROR;
            break;
        case D3D_TEXTURE_ADDRESS_CLAMP:
            *d3d_texture_address_mode_10 = D3D10_TEXTURE_ADDRESS_CLAMP;
            break;
        case D3D_TEXTURE_ADDRESS_BORDER:
            *d3d_texture_address_mode_10 = D3D10_TEXTURE_ADDRESS_BORDER;
            break;
        case D3D_TEXTURE_ADDRESS_MIRROR_ONCE:
            *d3d_texture_address_mode_10 = D3D10_TEXTURE_ADDRESS_MIRROR_ONCE;
            break;
    }
}

void D3DFlagsConvertFrom10_FORMAT_SUPPORT( UINT *format_support, UINT format_support_10 )
{
    *format_support = 0;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_BUFFER )
        *format_support |= D3D_FORMAT_SUPPORT_BUFFER;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_IA_VERTEX_BUFFER )
        *format_support |= D3D_FORMAT_SUPPORT_IA_VERTEX_BUFFER;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_IA_INDEX_BUFFER )
        *format_support |= D3D_FORMAT_SUPPORT_IA_INDEX_BUFFER;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_SO_BUFFER )
        *format_support |= D3D_FORMAT_SUPPORT_SO_BUFFER;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_TEXTURE1D )
        *format_support |= D3D_FORMAT_SUPPORT_TEXTURE1D;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_TEXTURE2D )
        *format_support |= D3D_FORMAT_SUPPORT_TEXTURE2D;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_TEXTURE3D )
        *format_support |= D3D_FORMAT_SUPPORT_TEXTURE3D;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_TEXTURECUBE )
        *format_support |= D3D_FORMAT_SUPPORT_TEXTURECUBE;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_SHADER_LOAD )
        *format_support |= D3D_FORMAT_SUPPORT_SHADER_LOAD;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_SHADER_SAMPLE )
        *format_support |= D3D_FORMAT_SUPPORT_SHADER_SAMPLE;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON )
        *format_support |= D3D_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT )
        *format_support |= D3D_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_MIP )
        *format_support |= D3D_FORMAT_SUPPORT_MIP;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_MIP_AUTOGEN )
        *format_support |= D3D_FORMAT_SUPPORT_MIP_AUTOGEN;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_RENDER_TARGET )
        *format_support |= D3D_FORMAT_SUPPORT_RENDER_TARGET;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_BLENDABLE )
        *format_support |= D3D_FORMAT_SUPPORT_BLENDABLE;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_DEPTH_STENCIL )
        *format_support |= D3D_FORMAT_SUPPORT_DEPTH_STENCIL;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_CPU_LOCKABLE )
        *format_support |= D3D_FORMAT_SUPPORT_CPU_LOCKABLE;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE )
        *format_support |= D3D_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_DISPLAY )
        *format_support |= D3D_FORMAT_SUPPORT_DISPLAY;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT )
        *format_support |= D3D_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET )
        *format_support |= D3D_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_MULTISAMPLE_LOAD )
        *format_support |= D3D_FORMAT_SUPPORT_MULTISAMPLE_LOAD;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_SHADER_GATHER )
        *format_support |= D3D_FORMAT_SUPPORT_SHADER_GATHER;
    if( format_support_10 & D3D10_FORMAT_SUPPORT_BACK_BUFFER_CAST )
        *format_support |= D3D_FORMAT_SUPPORT_BACK_BUFFER_CAST;
}

void D3DFlagsConvertTo10_FORMAT_SUPPORT( UINT *format_support_10, UINT format_support )
{
    *format_support_10 = 0;
    if( format_support & D3D_FORMAT_SUPPORT_BUFFER )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_BUFFER;
    if( format_support & D3D_FORMAT_SUPPORT_IA_VERTEX_BUFFER )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_IA_VERTEX_BUFFER;
    if( format_support & D3D_FORMAT_SUPPORT_IA_INDEX_BUFFER )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_IA_INDEX_BUFFER;
    if( format_support & D3D_FORMAT_SUPPORT_SO_BUFFER )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_SO_BUFFER;
    if( format_support & D3D_FORMAT_SUPPORT_TEXTURE1D )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_TEXTURE1D;
    if( format_support & D3D_FORMAT_SUPPORT_TEXTURE2D )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_TEXTURE2D;
    if( format_support & D3D_FORMAT_SUPPORT_TEXTURE3D )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_TEXTURE3D;
    if( format_support & D3D_FORMAT_SUPPORT_TEXTURECUBE )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_TEXTURECUBE;
    if( format_support & D3D_FORMAT_SUPPORT_SHADER_LOAD )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_SHADER_LOAD;
    if( format_support & D3D_FORMAT_SUPPORT_SHADER_SAMPLE )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_SHADER_SAMPLE;
    if( format_support & D3D_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON;
    if( format_support & D3D_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT;
    if( format_support & D3D_FORMAT_SUPPORT_MIP )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_MIP;
    if( format_support & D3D_FORMAT_SUPPORT_MIP_AUTOGEN )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_MIP_AUTOGEN;
    if( format_support & D3D_FORMAT_SUPPORT_RENDER_TARGET )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_RENDER_TARGET;
    if( format_support & D3D_FORMAT_SUPPORT_BLENDABLE )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_BLENDABLE;
    if( format_support & D3D_FORMAT_SUPPORT_DEPTH_STENCIL )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_DEPTH_STENCIL;
    if( format_support & D3D_FORMAT_SUPPORT_CPU_LOCKABLE )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_CPU_LOCKABLE;
    if( format_support & D3D_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE;
    if( format_support & D3D_FORMAT_SUPPORT_DISPLAY )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_DISPLAY;
    if( format_support & D3D_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT;
    if( format_support & D3D_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET;
    if( format_support & D3D_FORMAT_SUPPORT_MULTISAMPLE_LOAD )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_MULTISAMPLE_LOAD;
    if( format_support & D3D_FORMAT_SUPPORT_SHADER_GATHER )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_SHADER_GATHER;
    if( format_support & D3D_FORMAT_SUPPORT_BACK_BUFFER_CAST )
        *format_support_10 |= D3D10_FORMAT_SUPPORT_BACK_BUFFER_CAST;
}

void D3DFlagsConvertFrom10_ASYNC_GETDATA_FLAG( UINT *async_getdata_flag, UINT async_getdata_flag_10 )
{
    *async_getdata_flag = 0;
    if( async_getdata_flag_10 & D3D10_ASYNC_GETDATA_DONOTFLUSH )
        *async_getdata_flag |= D3D_ASYNC_GETDATA_DONOTFLUSH;
}

void D3DFlagsConvertTo10_ASYNC_GETDATA_FLAG( UINT *async_getdata_flag_10, UINT async_getdata_flag )
{
    *async_getdata_flag_10 = 0;
    if( async_getdata_flag & D3D_ASYNC_GETDATA_DONOTFLUSH )
        *async_getdata_flag_10 |= D3D10_ASYNC_GETDATA_DONOTFLUSH;
}

void D3DWrapperConvertEnum( D3D_QUERY *d3d_query, D3D10_QUERY d3d_query_10 )
{
    switch( d3d_query_10 )
    {
        default:
            *d3d_query = (D3D_QUERY)d3d_query_10;
            break;
        case D3D10_QUERY_EVENT:
            *d3d_query = D3D_QUERY_EVENT;
            break;
        case D3D10_QUERY_OCCLUSION:
            *d3d_query = D3D_QUERY_OCCLUSION;
            break;
        case D3D10_QUERY_TIMESTAMP:
            *d3d_query = D3D_QUERY_TIMESTAMP;
            break;
        case D3D10_QUERY_TIMESTAMP_DISJOINT:
            *d3d_query = D3D_QUERY_TIMESTAMP_DISJOINT;
            break;
        case D3D10_QUERY_PIPELINE_STATISTICS:
            *d3d_query = D3D_QUERY_PIPELINE_STATISTICS;
            break;
        case D3D10_QUERY_OCCLUSION_PREDICATE:
            *d3d_query = D3D_QUERY_OCCLUSION_PREDICATE;
            break;
        case D3D10_QUERY_SO_STATISTICS:
            *d3d_query = D3D_QUERY_SO_STATISTICS;
            break;
        case D3D10_QUERY_SO_OVERFLOW_PREDICATE:
            *d3d_query = D3D_QUERY_SO_OVERFLOW_PREDICATE;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_QUERY *d3d_query_10, D3D_QUERY d3d_query )
{
    switch( d3d_query )
    {
        default:
            *d3d_query_10 = (D3D10_QUERY)d3d_query;
            break;
        case D3D_QUERY_EVENT:
            *d3d_query_10 = D3D10_QUERY_EVENT;
            break;
        case D3D_QUERY_OCCLUSION:
            *d3d_query_10 = D3D10_QUERY_OCCLUSION;
            break;
        case D3D_QUERY_TIMESTAMP:
            *d3d_query_10 = D3D10_QUERY_TIMESTAMP;
            break;
        case D3D_QUERY_TIMESTAMP_DISJOINT:
            *d3d_query_10 = D3D10_QUERY_TIMESTAMP_DISJOINT;
            break;
        case D3D_QUERY_PIPELINE_STATISTICS:
            *d3d_query_10 = D3D10_QUERY_PIPELINE_STATISTICS;
            break;
        case D3D_QUERY_OCCLUSION_PREDICATE:
            *d3d_query_10 = D3D10_QUERY_OCCLUSION_PREDICATE;
            break;
        case D3D_QUERY_SO_STATISTICS:
            *d3d_query_10 = D3D10_QUERY_SO_STATISTICS;
            break;
        case D3D_QUERY_SO_OVERFLOW_PREDICATE:
            *d3d_query_10 = D3D10_QUERY_SO_OVERFLOW_PREDICATE;
            break;
    }
}

void D3DFlagsConvertFrom10_QUERY_MISC_FLAG( UINT *query_misc_flag, UINT query_misc_flag_10 )
{
    *query_misc_flag = 0;
    if( query_misc_flag_10 & D3D10_QUERY_MISC_PREDICATEHINT )
        *query_misc_flag |= D3D_QUERY_MISC_PREDICATEHINT;
}

void D3DFlagsConvertTo10_QUERY_MISC_FLAG( UINT *query_misc_flag_10, UINT query_misc_flag )
{
    *query_misc_flag_10 = 0;
    if( query_misc_flag & D3D_QUERY_MISC_PREDICATEHINT )
        *query_misc_flag_10 |= D3D10_QUERY_MISC_PREDICATEHINT;
}

void D3DWrapperConvertEnum( D3D_COUNTER *d3d_counter, D3D10_COUNTER d3d_counter_10 )
{
    switch( d3d_counter_10 )
    {
        default:
            if (d3d_counter_10 >= D3D10_COUNTER_DEVICE_DEPENDENT_0)
                *d3d_counter = (D3D_COUNTER)((d3d_counter_10 - D3D10_COUNTER_DEVICE_DEPENDENT_0) + D3D_COUNTER_DEVICE_DEPENDENT_0);
            else
                assert(!"Unknown enum value");
            break;
        case D3D10_COUNTER_GPU_IDLE:
            *d3d_counter = D3D_COUNTER_GPU_IDLE;
            break;
        case D3D10_COUNTER_VERTEX_PROCESSING:
            *d3d_counter = D3D_COUNTER_VERTEX_PROCESSING;
            break;
        case D3D10_COUNTER_GEOMETRY_PROCESSING:
            *d3d_counter = D3D_COUNTER_GEOMETRY_PROCESSING;
            break;
        case D3D10_COUNTER_PIXEL_PROCESSING:
            *d3d_counter = D3D_COUNTER_PIXEL_PROCESSING;
            break;
        case D3D10_COUNTER_OTHER_GPU_PROCESSING:
            *d3d_counter = D3D_COUNTER_OTHER_GPU_PROCESSING;
            break;
        case D3D10_COUNTER_HOST_ADAPTER_BANDWIDTH_UTILIZATION:
            *d3d_counter = D3D_COUNTER_HOST_ADAPTER_BANDWIDTH_UTILIZATION;
            break;
        case D3D10_COUNTER_LOCAL_VIDMEM_BANDWIDTH_UTILIZATION:
            *d3d_counter = D3D_COUNTER_LOCAL_VIDMEM_BANDWIDTH_UTILIZATION;
            break;
        case D3D10_COUNTER_VERTEX_THROUGHPUT_UTILIZATION:
            *d3d_counter = D3D_COUNTER_VERTEX_THROUGHPUT_UTILIZATION;
            break;
        case D3D10_COUNTER_TRIANGLE_SETUP_THROUGHPUT_UTILIZATION:
            *d3d_counter = D3D_COUNTER_TRIANGLE_SETUP_THROUGHPUT_UTILIZATION;
            break;
        case D3D10_COUNTER_FILLRATE_THROUGHPUT_UTILIZATION:
            *d3d_counter = D3D_COUNTER_FILLRATE_THROUGHPUT_UTILIZATION;
            break;
        case D3D10_COUNTER_VS_MEMORY_LIMITED:
            *d3d_counter = D3D_COUNTER_VS_MEMORY_LIMITED;
            break;
        case D3D10_COUNTER_VS_COMPUTATION_LIMITED:
            *d3d_counter = D3D_COUNTER_VS_COMPUTATION_LIMITED;
            break;
        case D3D10_COUNTER_GS_MEMORY_LIMITED:
            *d3d_counter = D3D_COUNTER_GS_MEMORY_LIMITED;
            break;
        case D3D10_COUNTER_GS_COMPUTATION_LIMITED:
            *d3d_counter = D3D_COUNTER_GS_COMPUTATION_LIMITED;
            break;
        case D3D10_COUNTER_PS_MEMORY_LIMITED:
            *d3d_counter = D3D_COUNTER_PS_MEMORY_LIMITED;
            break;
        case D3D10_COUNTER_PS_COMPUTATION_LIMITED:
            *d3d_counter = D3D_COUNTER_PS_COMPUTATION_LIMITED;
            break;
        case D3D10_COUNTER_POST_TRANSFORM_CACHE_HIT_RATE:
            *d3d_counter = D3D_COUNTER_POST_TRANSFORM_CACHE_HIT_RATE;
            break;
        case D3D10_COUNTER_TEXTURE_CACHE_HIT_RATE:
            *d3d_counter = D3D_COUNTER_TEXTURE_CACHE_HIT_RATE;
            break;
        case D3D10_COUNTER_DEVICE_DEPENDENT_0:
            *d3d_counter = D3D_COUNTER_DEVICE_DEPENDENT_0;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_COUNTER *d3d_counter_10, D3D_COUNTER d3d_counter )
{
    switch( d3d_counter )
    {
        default:
            if (d3d_counter >= D3D_COUNTER_DEVICE_DEPENDENT_0)
                *d3d_counter_10 = (D3D10_COUNTER)((d3d_counter - D3D_COUNTER_DEVICE_DEPENDENT_0) + D3D10_COUNTER_DEVICE_DEPENDENT_0);
            else
                assert(!"Unknown enum value");
            break;
        case D3D_COUNTER_GPU_IDLE:
            *d3d_counter_10 = D3D10_COUNTER_GPU_IDLE;
            break;
        case D3D_COUNTER_VERTEX_PROCESSING:
            *d3d_counter_10 = D3D10_COUNTER_VERTEX_PROCESSING;
            break;
        case D3D_COUNTER_GEOMETRY_PROCESSING:
            *d3d_counter_10 = D3D10_COUNTER_GEOMETRY_PROCESSING;
            break;
        case D3D_COUNTER_PIXEL_PROCESSING:
            *d3d_counter_10 = D3D10_COUNTER_PIXEL_PROCESSING;
            break;
        case D3D_COUNTER_OTHER_GPU_PROCESSING:
            *d3d_counter_10 = D3D10_COUNTER_OTHER_GPU_PROCESSING;
            break;
        case D3D_COUNTER_HOST_ADAPTER_BANDWIDTH_UTILIZATION:
            *d3d_counter_10 = D3D10_COUNTER_HOST_ADAPTER_BANDWIDTH_UTILIZATION;
            break;
        case D3D_COUNTER_LOCAL_VIDMEM_BANDWIDTH_UTILIZATION:
            *d3d_counter_10 = D3D10_COUNTER_LOCAL_VIDMEM_BANDWIDTH_UTILIZATION;
            break;
        case D3D_COUNTER_VERTEX_THROUGHPUT_UTILIZATION:
            *d3d_counter_10 = D3D10_COUNTER_VERTEX_THROUGHPUT_UTILIZATION;
            break;
        case D3D_COUNTER_TRIANGLE_SETUP_THROUGHPUT_UTILIZATION:
            *d3d_counter_10 = D3D10_COUNTER_TRIANGLE_SETUP_THROUGHPUT_UTILIZATION;
            break;
        case D3D_COUNTER_FILLRATE_THROUGHPUT_UTILIZATION:
            *d3d_counter_10 = D3D10_COUNTER_FILLRATE_THROUGHPUT_UTILIZATION;
            break;
        case D3D_COUNTER_VS_MEMORY_LIMITED:
            *d3d_counter_10 = D3D10_COUNTER_VS_MEMORY_LIMITED;
            break;
        case D3D_COUNTER_VS_COMPUTATION_LIMITED:
            *d3d_counter_10 = D3D10_COUNTER_VS_COMPUTATION_LIMITED;
            break;
        case D3D_COUNTER_GS_MEMORY_LIMITED:
            *d3d_counter_10 = D3D10_COUNTER_GS_MEMORY_LIMITED;
            break;
        case D3D_COUNTER_GS_COMPUTATION_LIMITED:
            *d3d_counter_10 = D3D10_COUNTER_GS_COMPUTATION_LIMITED;
            break;
        case D3D_COUNTER_PS_MEMORY_LIMITED:
            *d3d_counter_10 = D3D10_COUNTER_PS_MEMORY_LIMITED;
            break;
        case D3D_COUNTER_PS_COMPUTATION_LIMITED:
            *d3d_counter_10 = D3D10_COUNTER_PS_COMPUTATION_LIMITED;
            break;
        case D3D_COUNTER_POST_TRANSFORM_CACHE_HIT_RATE:
            *d3d_counter_10 = D3D10_COUNTER_POST_TRANSFORM_CACHE_HIT_RATE;
            break;
        case D3D_COUNTER_TEXTURE_CACHE_HIT_RATE:
            *d3d_counter_10 = D3D10_COUNTER_TEXTURE_CACHE_HIT_RATE;
            break;
        case D3D_COUNTER_DEVICE_DEPENDENT_0:
            *d3d_counter_10 = D3D10_COUNTER_DEVICE_DEPENDENT_0;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_COUNTER_TYPE *d3d_counter_type, D3D10_COUNTER_TYPE d3d_counter_type_10 )
{
    switch( d3d_counter_type_10 )
    {
        default:
            *d3d_counter_type = (D3D_COUNTER_TYPE)d3d_counter_type_10;
            break;
        case D3D10_COUNTER_TYPE_FLOAT32:
            *d3d_counter_type = D3D_COUNTER_TYPE_FLOAT32;
            break;
        case D3D10_COUNTER_TYPE_UINT16:
            *d3d_counter_type = D3D_COUNTER_TYPE_UINT16;
            break;
        case D3D10_COUNTER_TYPE_UINT32:
            *d3d_counter_type = D3D_COUNTER_TYPE_UINT32;
            break;
        case D3D10_COUNTER_TYPE_UINT64:
            *d3d_counter_type = D3D_COUNTER_TYPE_UINT64;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_COUNTER_TYPE *d3d_counter_type_10, D3D_COUNTER_TYPE d3d_counter_type )
{
    switch( d3d_counter_type )
    {
        default:
            *d3d_counter_type_10 = (D3D10_COUNTER_TYPE)d3d_counter_type;
            break;
        case D3D_COUNTER_TYPE_FLOAT32:
            *d3d_counter_type_10 = D3D10_COUNTER_TYPE_FLOAT32;
            break;
        case D3D_COUNTER_TYPE_UINT16:
            *d3d_counter_type_10 = D3D10_COUNTER_TYPE_UINT16;
            break;
        case D3D_COUNTER_TYPE_UINT32:
            *d3d_counter_type_10 = D3D10_COUNTER_TYPE_UINT32;
            break;
        case D3D_COUNTER_TYPE_UINT64:
            *d3d_counter_type_10 = D3D10_COUNTER_TYPE_UINT64;
            break;
    }
}

void D3DFlagsConvertFrom10_CREATE_DEVICE_FLAG( UINT *create_device_flag, UINT create_device_flag_10 )
{
    *create_device_flag = 0;
    if( create_device_flag_10 & D3D10_CREATE_DEVICE_SINGLETHREADED )
        *create_device_flag |= D3D_CREATE_DEVICE_SINGLETHREADED;
    if( create_device_flag_10 & D3D10_CREATE_DEVICE_DEBUG )
        *create_device_flag |= D3D_CREATE_DEVICE_DEBUG;
    if( create_device_flag_10 & D3D10_CREATE_DEVICE_SWITCH_TO_REF )
        *create_device_flag |= D3D_CREATE_DEVICE_SWITCH_TO_REF;
    if( create_device_flag_10 & D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS )
        *create_device_flag |= D3D_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
    if( create_device_flag_10 & D3D10_CREATE_DEVICE_ALLOW_NULL_FROM_MAP )
        *create_device_flag |= D3D_CREATE_DEVICE_ALLOW_NULL_FROM_MAP;
    if( create_device_flag_10 & D3D10_CREATE_DEVICE_BGRA_SUPPORT )
        *create_device_flag |= D3D_CREATE_DEVICE_BGRA_SUPPORT;
    if( create_device_flag_10 & D3D10_CREATE_DEVICE_STRICT_VALIDATION )
        *create_device_flag |= D3D_CREATE_DEVICE_STRICT_VALIDATION;
}

void D3DFlagsConvertTo10_CREATE_DEVICE_FLAG( UINT *create_device_flag_10, UINT create_device_flag )
{
    *create_device_flag_10 = 0;
    if( create_device_flag & D3D_CREATE_DEVICE_SINGLETHREADED )
        *create_device_flag_10 |= D3D10_CREATE_DEVICE_SINGLETHREADED;
    if( create_device_flag & D3D_CREATE_DEVICE_DEBUG )
        *create_device_flag_10 |= D3D10_CREATE_DEVICE_DEBUG;
    if( create_device_flag & D3D_CREATE_DEVICE_SWITCH_TO_REF )
        *create_device_flag_10 |= D3D10_CREATE_DEVICE_SWITCH_TO_REF;
    if( create_device_flag & D3D_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS )
        *create_device_flag_10 |= D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
    if( create_device_flag & D3D_CREATE_DEVICE_ALLOW_NULL_FROM_MAP )
        *create_device_flag_10 |= D3D10_CREATE_DEVICE_ALLOW_NULL_FROM_MAP;
    if( create_device_flag & D3D_CREATE_DEVICE_BGRA_SUPPORT )
        *create_device_flag_10 |= D3D10_CREATE_DEVICE_BGRA_SUPPORT;
    if( create_device_flag & D3D_CREATE_DEVICE_STRICT_VALIDATION )
        *create_device_flag_10 |= D3D10_CREATE_DEVICE_STRICT_VALIDATION;
}

void D3DWrapperConvertEnum( D3D_MESSAGE_CATEGORY *d3d_message_category, D3D10_MESSAGE_CATEGORY d3d_message_category_10 )
{
    switch( d3d_message_category_10 )
    {
        default:
            *d3d_message_category = (D3D_MESSAGE_CATEGORY)d3d_message_category_10;
            break;
        case D3D10_MESSAGE_CATEGORY_APPLICATION_DEFINED:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_APPLICATION_DEFINED;
            break;
        case D3D10_MESSAGE_CATEGORY_MISCELLANEOUS:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_MISCELLANEOUS;
            break;
        case D3D10_MESSAGE_CATEGORY_INITIALIZATION:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_INITIALIZATION;
            break;
        case D3D10_MESSAGE_CATEGORY_CLEANUP:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_CLEANUP;
            break;
        case D3D10_MESSAGE_CATEGORY_COMPILATION:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_COMPILATION;
            break;
        case D3D10_MESSAGE_CATEGORY_STATE_CREATION:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_STATE_CREATION;
            break;
        case D3D10_MESSAGE_CATEGORY_STATE_SETTING:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_STATE_SETTING;
            break;
        case D3D10_MESSAGE_CATEGORY_STATE_GETTING:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_STATE_GETTING;
            break;
        case D3D10_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_RESOURCE_MANIPULATION;
            break;
        case D3D10_MESSAGE_CATEGORY_EXECUTION:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_EXECUTION;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_MESSAGE_CATEGORY *d3d_message_category_10, D3D_MESSAGE_CATEGORY d3d_message_category )
{
    switch( d3d_message_category )
    {
        default:
            *d3d_message_category_10 = (D3D10_MESSAGE_CATEGORY)d3d_message_category;
            break;
        case D3D_MESSAGE_CATEGORY_APPLICATION_DEFINED:
            *d3d_message_category_10 = D3D10_MESSAGE_CATEGORY_APPLICATION_DEFINED;
            break;
        case D3D_MESSAGE_CATEGORY_MISCELLANEOUS:
            *d3d_message_category_10 = D3D10_MESSAGE_CATEGORY_MISCELLANEOUS;
            break;
        case D3D_MESSAGE_CATEGORY_INITIALIZATION:
            *d3d_message_category_10 = D3D10_MESSAGE_CATEGORY_INITIALIZATION;
            break;
        case D3D_MESSAGE_CATEGORY_CLEANUP:
            *d3d_message_category_10 = D3D10_MESSAGE_CATEGORY_CLEANUP;
            break;
        case D3D_MESSAGE_CATEGORY_COMPILATION:
            *d3d_message_category_10 = D3D10_MESSAGE_CATEGORY_COMPILATION;
            break;
        case D3D_MESSAGE_CATEGORY_STATE_CREATION:
            *d3d_message_category_10 = D3D10_MESSAGE_CATEGORY_STATE_CREATION;
            break;
        case D3D_MESSAGE_CATEGORY_STATE_SETTING:
            *d3d_message_category_10 = D3D10_MESSAGE_CATEGORY_STATE_SETTING;
            break;
        case D3D_MESSAGE_CATEGORY_STATE_GETTING:
            *d3d_message_category_10 = D3D10_MESSAGE_CATEGORY_STATE_GETTING;
            break;
        case D3D_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
            *d3d_message_category_10 = D3D10_MESSAGE_CATEGORY_RESOURCE_MANIPULATION;
            break;
        case D3D_MESSAGE_CATEGORY_EXECUTION:
            *d3d_message_category_10 = D3D10_MESSAGE_CATEGORY_EXECUTION;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_MESSAGE_SEVERITY *d3d_message_severity, D3D10_MESSAGE_SEVERITY d3d_message_severity_10 )
{
    switch( d3d_message_severity_10 )
    {
        default:
            *d3d_message_severity = (D3D_MESSAGE_SEVERITY)d3d_message_severity_10;
            break;
        case D3D10_MESSAGE_SEVERITY_CORRUPTION:
            *d3d_message_severity = D3D_MESSAGE_SEVERITY_CORRUPTION;
            break;
        case D3D10_MESSAGE_SEVERITY_ERROR:
            *d3d_message_severity = D3D_MESSAGE_SEVERITY_ERROR;
            break;
        case D3D10_MESSAGE_SEVERITY_WARNING:
            *d3d_message_severity = D3D_MESSAGE_SEVERITY_WARNING;
            break;
        case D3D10_MESSAGE_SEVERITY_INFO:
            *d3d_message_severity = D3D_MESSAGE_SEVERITY_INFO;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_MESSAGE_SEVERITY *d3d_message_severity_10, D3D_MESSAGE_SEVERITY d3d_message_severity )
{
    switch( d3d_message_severity )
    {
        default:
            *d3d_message_severity_10 = (D3D10_MESSAGE_SEVERITY)d3d_message_severity;
            break;
        case D3D_MESSAGE_SEVERITY_CORRUPTION:
            *d3d_message_severity_10 = D3D10_MESSAGE_SEVERITY_CORRUPTION;
            break;
        case D3D_MESSAGE_SEVERITY_ERROR:
            *d3d_message_severity_10 = D3D10_MESSAGE_SEVERITY_ERROR;
            break;
        case D3D_MESSAGE_SEVERITY_WARNING:
            *d3d_message_severity_10 = D3D10_MESSAGE_SEVERITY_WARNING;
            break;
        case D3D_MESSAGE_SEVERITY_INFO:
            *d3d_message_severity_10 = D3D10_MESSAGE_SEVERITY_INFO;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_MESSAGE_ID *d3d_message_id, D3D10_MESSAGE_ID d3d_message_id_10 )
{
    switch( d3d_message_id_10 )
    {
        default:
            *d3d_message_id = (D3D_MESSAGE_ID)d3d_message_id_10;
            break;
        case D3D10_MESSAGE_ID_UNKNOWN:
            *d3d_message_id = D3D_MESSAGE_ID_UNKNOWN;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_HAZARD:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_HAZARD;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_HAZARD:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_HAZARD;
            break;
        case D3D10_MESSAGE_ID_DEVICE_VSSETSHADERRESOURCES_HAZARD:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_VSSETSHADERRESOURCES_HAZARD;
            break;
        case D3D10_MESSAGE_ID_DEVICE_VSSETCONSTANTBUFFERS_HAZARD:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_VSSETCONSTANTBUFFERS_HAZARD;
            break;
        case D3D10_MESSAGE_ID_DEVICE_GSSETSHADERRESOURCES_HAZARD:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_GSSETSHADERRESOURCES_HAZARD;
            break;
        case D3D10_MESSAGE_ID_DEVICE_GSSETCONSTANTBUFFERS_HAZARD:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_GSSETCONSTANTBUFFERS_HAZARD;
            break;
        case D3D10_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD;
            break;
        case D3D10_MESSAGE_ID_DEVICE_PSSETCONSTANTBUFFERS_HAZARD:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_PSSETCONSTANTBUFFERS_HAZARD;
            break;
        case D3D10_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD;
            break;
        case D3D10_MESSAGE_ID_DEVICE_SOSETTARGETS_HAZARD:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_SOSETTARGETS_HAZARD;
            break;
        case D3D10_MESSAGE_ID_STRING_FROM_APPLICATION:
            *d3d_message_id = D3D_MESSAGE_ID_STRING_FROM_APPLICATION;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_THIS:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_THIS;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER1:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER1;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER2:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER2;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER3:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER3;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER4:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER4;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER5:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER5;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER6:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER6;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER7:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER7;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER8:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER8;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER9:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER9;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER10:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER10;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER11:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER11;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER12:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER12;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER13:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER13;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER14:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER14;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_PARAMETER15:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_PARAMETER15;
            break;
        case D3D10_MESSAGE_ID_CORRUPTED_MULTITHREADING:
            *d3d_message_id = D3D_MESSAGE_ID_CORRUPTED_MULTITHREADING;
            break;
        case D3D10_MESSAGE_ID_MESSAGE_REPORTING_OUTOFMEMORY:
            *d3d_message_id = D3D_MESSAGE_ID_MESSAGE_REPORTING_OUTOFMEMORY;
            break;
        case D3D10_MESSAGE_ID_IASETINPUTLAYOUT_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_IASETINPUTLAYOUT_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_IASETVERTEXBUFFERS_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_IASETVERTEXBUFFERS_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_IASETINDEXBUFFER_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_IASETINDEXBUFFER_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_VSSETSHADER_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_VSSETSHADER_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_VSSETSHADERRESOURCES_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_VSSETSHADERRESOURCES_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_VSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_VSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_VSSETSAMPLERS_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_VSSETSAMPLERS_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_GSSETSHADER_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_GSSETSHADER_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_GSSETSHADERRESOURCES_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_GSSETSHADERRESOURCES_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_GSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_GSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_GSSETSAMPLERS_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_GSSETSAMPLERS_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_SOSETTARGETS_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_SOSETTARGETS_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_PSSETSHADER_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_PSSETSHADER_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_PSSETSHADERRESOURCES_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_PSSETSHADERRESOURCES_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_PSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_PSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_PSSETSAMPLERS_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_PSSETSAMPLERS_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_RSSETSTATE_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_RSSETSTATE_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_OMSETBLENDSTATE_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_OMSETBLENDSTATE_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_OMSETDEPTHSTENCILSTATE_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_OMSETDEPTHSTENCILSTATE_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_OMSETRENDERTARGETS_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_OMSETRENDERTARGETS_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_SETPREDICATION_UNBINDDELETINGOBJECT:
            *d3d_message_id = D3D_MESSAGE_ID_SETPREDICATION_UNBINDDELETINGOBJECT;
            break;
        case D3D10_MESSAGE_ID_GETPRIVATEDATA_MOREDATA:
            *d3d_message_id = D3D_MESSAGE_ID_GETPRIVATEDATA_MOREDATA;
            break;
        case D3D10_MESSAGE_ID_SETPRIVATEDATA_INVALIDFREEDATA:
            *d3d_message_id = D3D_MESSAGE_ID_SETPRIVATEDATA_INVALIDFREEDATA;
            break;
        case D3D10_MESSAGE_ID_SETPRIVATEDATA_INVALIDIUNKNOWN:
            *d3d_message_id = D3D_MESSAGE_ID_SETPRIVATEDATA_INVALIDIUNKNOWN;
            break;
        case D3D10_MESSAGE_ID_SETPRIVATEDATA_INVALIDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_SETPRIVATEDATA_INVALIDFLAGS;
            break;
        case D3D10_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS:
            *d3d_message_id = D3D_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS;
            break;
        case D3D10_MESSAGE_ID_SETPRIVATEDATA_OUTOFMEMORY:
            *d3d_message_id = D3D_MESSAGE_ID_SETPRIVATEDATA_OUTOFMEMORY;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDSAMPLES:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_INVALIDSAMPLES;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDUSAGE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDUSAGE;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDBINDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDBINDFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDCPUACCESSFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDCPUACCESSFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDMISCFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDMISCFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDCPUACCESSFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_INVALIDCPUACCESSFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDBINDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_INVALIDBINDFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDINITIALDATA:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_INVALIDINITIALDATA;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDDIMENSIONS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_INVALIDDIMENSIONS;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDMIPLEVELS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_INVALIDMIPLEVELS;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDMISCFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_INVALIDMISCFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDARG_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_INVALIDARG_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_OUTOFMEMORY_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_OUTOFMEMORY_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_NULLDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_NULLDESC;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDCONSTANTBUFFERBINDINGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_INVALIDCONSTANTBUFFERBINDINGS;
            break;
        case D3D10_MESSAGE_ID_CREATEBUFFER_LARGEALLOCATION:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBUFFER_LARGEALLOCATION;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_UNSUPPORTEDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_UNSUPPORTEDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDSAMPLES:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDSAMPLES;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDUSAGE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDUSAGE;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDBINDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDBINDFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDCPUACCESSFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDCPUACCESSFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDMISCFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDMISCFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDCPUACCESSFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDCPUACCESSFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDBINDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDBINDFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDINITIALDATA:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDINITIALDATA;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDDIMENSIONS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDDIMENSIONS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDMIPLEVELS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDMIPLEVELS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDMISCFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDMISCFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDARG_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDARG_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_OUTOFMEMORY_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_OUTOFMEMORY_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_NULLDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_NULLDESC;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE1D_LARGEALLOCATION:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE1D_LARGEALLOCATION;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_UNSUPPORTEDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_UNSUPPORTEDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDSAMPLES:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDSAMPLES;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDUSAGE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDUSAGE;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDBINDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDBINDFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDCPUACCESSFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDCPUACCESSFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDMISCFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDMISCFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDCPUACCESSFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDCPUACCESSFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDBINDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDBINDFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDINITIALDATA:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDINITIALDATA;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDDIMENSIONS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDDIMENSIONS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDMIPLEVELS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDMIPLEVELS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDMISCFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDMISCFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDARG_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDARG_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_OUTOFMEMORY_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_OUTOFMEMORY_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_NULLDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_NULLDESC;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE2D_LARGEALLOCATION:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE2D_LARGEALLOCATION;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_UNSUPPORTEDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_UNSUPPORTEDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDSAMPLES:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDSAMPLES;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDUSAGE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDUSAGE;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDBINDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDBINDFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDCPUACCESSFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDCPUACCESSFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDMISCFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDMISCFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDCPUACCESSFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDCPUACCESSFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDBINDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDBINDFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDINITIALDATA:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDINITIALDATA;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDDIMENSIONS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDDIMENSIONS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDMIPLEVELS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDMIPLEVELS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDMISCFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDMISCFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDARG_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDARG_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_OUTOFMEMORY_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_OUTOFMEMORY_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_NULLDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_NULLDESC;
            break;
        case D3D10_MESSAGE_ID_CREATETEXTURE3D_LARGEALLOCATION:
            *d3d_message_id = D3D_MESSAGE_ID_CREATETEXTURE3D_LARGEALLOCATION;
            break;
        case D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_UNRECOGNIZEDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_UNRECOGNIZEDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDDESC;
            break;
        case D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDDIMENSIONS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDDIMENSIONS;
            break;
        case D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDRESOURCE;
            break;
        case D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_TOOMANYOBJECTS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_TOOMANYOBJECTS;
            break;
        case D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDARG_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDARG_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_OUTOFMEMORY_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_OUTOFMEMORY_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_UNRECOGNIZEDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_UNRECOGNIZEDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_UNSUPPORTEDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_UNSUPPORTEDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDDESC;
            break;
        case D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDDIMENSIONS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDDIMENSIONS;
            break;
        case D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDRESOURCE;
            break;
        case D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_TOOMANYOBJECTS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_TOOMANYOBJECTS;
            break;
        case D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDARG_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDARG_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_OUTOFMEMORY_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_OUTOFMEMORY_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_UNRECOGNIZEDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_UNRECOGNIZEDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDDESC;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDDIMENSIONS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDDIMENSIONS;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDRESOURCE;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_TOOMANYOBJECTS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_TOOMANYOBJECTS;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDARG_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDARG_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_OUTOFMEMORY_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_OUTOFMEMORY_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_OUTOFMEMORY:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_OUTOFMEMORY;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_TOOMANYELEMENTS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_TOOMANYELEMENTS;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INCOMPATIBLEFORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INCOMPATIBLEFORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSLOT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSLOT;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDINPUTSLOTCLASS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDINPUTSLOTCLASS;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_STEPRATESLOTCLASSMISMATCH:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_STEPRATESLOTCLASSMISMATCH;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSLOTCLASSCHANGE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSLOTCLASSCHANGE;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSTEPRATECHANGE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSTEPRATECHANGE;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDALIGNMENT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDALIGNMENT;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_DUPLICATESEMANTIC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_DUPLICATESEMANTIC;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_UNPARSEABLEINPUTSIGNATURE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_UNPARSEABLEINPUTSIGNATURE;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_NULLSEMANTIC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_NULLSEMANTIC;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_MISSINGELEMENT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_MISSINGELEMENT;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_NULLDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_NULLDESC;
            break;
        case D3D10_MESSAGE_ID_CREATEVERTEXSHADER_OUTOFMEMORY:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEVERTEXSHADER_OUTOFMEMORY;
            break;
        case D3D10_MESSAGE_ID_CREATEVERTEXSHADER_INVALIDSHADERBYTECODE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEVERTEXSHADER_INVALIDSHADERBYTECODE;
            break;
        case D3D10_MESSAGE_ID_CREATEVERTEXSHADER_INVALIDSHADERTYPE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEVERTEXSHADER_INVALIDSHADERTYPE;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADER_OUTOFMEMORY:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADER_OUTOFMEMORY;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADER_INVALIDSHADERBYTECODE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADER_INVALIDSHADERBYTECODE;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADER_INVALIDSHADERTYPE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADER_INVALIDSHADERTYPE;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTOFMEMORY:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTOFMEMORY;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSHADERBYTECODE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSHADERBYTECODE;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSHADERTYPE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSHADERTYPE;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDNUMENTRIES:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDNUMENTRIES;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTPUTSTREAMSTRIDEUNUSED:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTPUTSTREAMSTRIDEUNUSED;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_UNEXPECTEDDECL:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_UNEXPECTEDDECL;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_EXPECTEDDECL:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_EXPECTEDDECL;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTPUTSLOT0EXPECTED:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTPUTSLOT0EXPECTED;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDOUTPUTSLOT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDOUTPUTSLOT;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_ONLYONEELEMENTPERSLOT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_ONLYONEELEMENTPERSLOT;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDCOMPONENTCOUNT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDCOMPONENTCOUNT;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSTARTCOMPONENTANDCOMPONENTCOUNT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSTARTCOMPONENTANDCOMPONENTCOUNT;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDGAPDEFINITION:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDGAPDEFINITION;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_REPEATEDOUTPUT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_REPEATEDOUTPUT;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDOUTPUTSTREAMSTRIDE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDOUTPUTSTREAMSTRIDE;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MISSINGSEMANTIC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MISSINGSEMANTIC;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MASKMISMATCH:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MASKMISMATCH;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_CANTHAVEONLYGAPS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_CANTHAVEONLYGAPS;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_DECLTOOCOMPLEX:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_DECLTOOCOMPLEX;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MISSINGOUTPUTSIGNATURE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MISSINGOUTPUTSIGNATURE;
            break;
        case D3D10_MESSAGE_ID_CREATEPIXELSHADER_OUTOFMEMORY:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEPIXELSHADER_OUTOFMEMORY;
            break;
        case D3D10_MESSAGE_ID_CREATEPIXELSHADER_INVALIDSHADERBYTECODE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEPIXELSHADER_INVALIDSHADERBYTECODE;
            break;
        case D3D10_MESSAGE_ID_CREATEPIXELSHADER_INVALIDSHADERTYPE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEPIXELSHADER_INVALIDSHADERTYPE;
            break;
        case D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDFILLMODE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDFILLMODE;
            break;
        case D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDCULLMODE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDCULLMODE;
            break;
        case D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDDEPTHBIASCLAMP:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDDEPTHBIASCLAMP;
            break;
        case D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDSLOPESCALEDDEPTHBIAS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDSLOPESCALEDDEPTHBIAS;
            break;
        case D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_TOOMANYOBJECTS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERASTERIZERSTATE_TOOMANYOBJECTS;
            break;
        case D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_NULLDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERASTERIZERSTATE_NULLDESC;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDDEPTHWRITEMASK:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDDEPTHWRITEMASK;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDDEPTHFUNC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDDEPTHFUNC;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILFAILOP:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILFAILOP;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILZFAILOP:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILZFAILOP;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILPASSOP:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILPASSOP;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILFUNC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILFUNC;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILFAILOP:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILFAILOP;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILZFAILOP:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILZFAILOP;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILPASSOP:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILPASSOP;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILFUNC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILFUNC;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_TOOMANYOBJECTS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_TOOMANYOBJECTS;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_NULLDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_NULLDESC;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDSRCBLEND:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDSRCBLEND;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDDESTBLEND:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDDESTBLEND;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDBLENDOP:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDBLENDOP;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDSRCBLENDALPHA:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDSRCBLENDALPHA;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDDESTBLENDALPHA:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDDESTBLENDALPHA;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDBLENDOPALPHA:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDBLENDOPALPHA;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDRENDERTARGETWRITEMASK:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDRENDERTARGETWRITEMASK;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_TOOMANYOBJECTS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_TOOMANYOBJECTS;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_NULLDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_NULLDESC;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDFILTER:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDFILTER;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSU:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSU;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSV:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSV;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSW:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSW;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMIPLODBIAS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMIPLODBIAS;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMAXANISOTROPY:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMAXANISOTROPY;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDCOMPARISONFUNC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDCOMPARISONFUNC;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMINLOD:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMINLOD;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMAXLOD:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMAXLOD;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_TOOMANYOBJECTS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_TOOMANYOBJECTS;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_NULLDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_NULLDESC;
            break;
        case D3D10_MESSAGE_ID_CREATEQUERYORPREDICATE_INVALIDQUERY:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEQUERYORPREDICATE_INVALIDQUERY;
            break;
        case D3D10_MESSAGE_ID_CREATEQUERYORPREDICATE_INVALIDMISCFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEQUERYORPREDICATE_INVALIDMISCFLAGS;
            break;
        case D3D10_MESSAGE_ID_CREATEQUERYORPREDICATE_UNEXPECTEDMISCFLAG:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEQUERYORPREDICATE_UNEXPECTEDMISCFLAG;
            break;
        case D3D10_MESSAGE_ID_CREATEQUERYORPREDICATE_NULLDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEQUERYORPREDICATE_NULLDESC;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_TOPOLOGY_UNRECOGNIZED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_TOPOLOGY_UNRECOGNIZED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_TOPOLOGY_UNDEFINED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_TOPOLOGY_UNDEFINED;
            break;
        case D3D10_MESSAGE_ID_IASETVERTEXBUFFERS_INVALIDBUFFER:
            *d3d_message_id = D3D_MESSAGE_ID_IASETVERTEXBUFFERS_INVALIDBUFFER;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_OFFSET_TOO_LARGE:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_OFFSET_TOO_LARGE;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_IASETINDEXBUFFER_INVALIDBUFFER:
            *d3d_message_id = D3D_MESSAGE_ID_IASETINDEXBUFFER_INVALIDBUFFER;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_FORMAT_INVALID:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_FORMAT_INVALID;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_OFFSET_TOO_LARGE:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_OFFSET_TOO_LARGE;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_OFFSET_UNALIGNED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_OFFSET_UNALIGNED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_VSSETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_VSSETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_VSSETCONSTANTBUFFERS_INVALIDBUFFER:
            *d3d_message_id = D3D_MESSAGE_ID_VSSETCONSTANTBUFFERS_INVALIDBUFFER;
            break;
        case D3D10_MESSAGE_ID_DEVICE_VSSETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_VSSETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_VSSETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_VSSETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_GSSETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_GSSETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_GSSETCONSTANTBUFFERS_INVALIDBUFFER:
            *d3d_message_id = D3D_MESSAGE_ID_GSSETCONSTANTBUFFERS_INVALIDBUFFER;
            break;
        case D3D10_MESSAGE_ID_DEVICE_GSSETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_GSSETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_GSSETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_GSSETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_SOSETTARGETS_INVALIDBUFFER:
            *d3d_message_id = D3D_MESSAGE_ID_SOSETTARGETS_INVALIDBUFFER;
            break;
        case D3D10_MESSAGE_ID_DEVICE_SOSETTARGETS_OFFSET_UNALIGNED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_SOSETTARGETS_OFFSET_UNALIGNED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_PSSETCONSTANTBUFFERS_INVALIDBUFFER:
            *d3d_message_id = D3D_MESSAGE_ID_PSSETCONSTANTBUFFERS_INVALIDBUFFER;
            break;
        case D3D10_MESSAGE_ID_DEVICE_PSSETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_PSSETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_PSSETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_PSSETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_INVALIDVIEWPORT:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_INVALIDVIEWPORT;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_INVALIDSCISSOR:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_INVALIDSCISSOR;
            break;
        case D3D10_MESSAGE_ID_CLEARRENDERTARGETVIEW_DENORMFLUSH:
            *d3d_message_id = D3D_MESSAGE_ID_CLEARRENDERTARGETVIEW_DENORMFLUSH;
            break;
        case D3D10_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_DENORMFLUSH:
            *d3d_message_id = D3D_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_DENORMFLUSH;
            break;
        case D3D10_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_INVALID:
            *d3d_message_id = D3D_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_INVALID;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IAGETVERTEXBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IAGETVERTEXBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_VSGETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_VSGETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_VSGETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_VSGETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_VSGETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_VSGETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_GSGETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_GSGETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_GSGETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_GSGETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_GSGETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_GSGETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_SOGETTARGETS_BUFFERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_SOGETTARGETS_BUFFERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_PSGETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_PSGETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_PSGETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_PSGETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_PSGETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_PSGETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RSGETVIEWPORTS_VIEWPORTS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RSGETVIEWPORTS_VIEWPORTS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RSGETSCISSORRECTS_RECTS_EMPTY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RSGETSCISSORRECTS_RECTS_EMPTY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_GENERATEMIPS_RESOURCE_INVALID:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_GENERATEMIPS_RESOURCE_INVALID;
            break;
        case D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDDESTINATIONSUBRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDDESTINATIONSUBRESOURCE;
            break;
        case D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCESUBRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCESUBRESOURCE;
            break;
        case D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCEBOX:
            *d3d_message_id = D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCEBOX;
            break;
        case D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCE;
            break;
        case D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDDESTINATIONSTATE:
            *d3d_message_id = D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDDESTINATIONSTATE;
            break;
        case D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCESTATE:
            *d3d_message_id = D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCESTATE;
            break;
        case D3D10_MESSAGE_ID_COPYRESOURCE_INVALIDSOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_COPYRESOURCE_INVALIDSOURCE;
            break;
        case D3D10_MESSAGE_ID_COPYRESOURCE_INVALIDDESTINATIONSTATE:
            *d3d_message_id = D3D_MESSAGE_ID_COPYRESOURCE_INVALIDDESTINATIONSTATE;
            break;
        case D3D10_MESSAGE_ID_COPYRESOURCE_INVALIDSOURCESTATE:
            *d3d_message_id = D3D_MESSAGE_ID_COPYRESOURCE_INVALIDSOURCESTATE;
            break;
        case D3D10_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONSUBRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONSUBRESOURCE;
            break;
        case D3D10_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONBOX:
            *d3d_message_id = D3D_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONBOX;
            break;
        case D3D10_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONSTATE:
            *d3d_message_id = D3D_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONSTATE;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_DESTINATION_INVALID:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_DESTINATION_INVALID;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_DESTINATION_SUBRESOURCE_INVALID:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_DESTINATION_SUBRESOURCE_INVALID;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_SOURCE_INVALID:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_SOURCE_INVALID;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_SOURCE_SUBRESOURCE_INVALID:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_SOURCE_SUBRESOURCE_INVALID;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_FORMAT_INVALID:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_FORMAT_INVALID;
            break;
        case D3D10_MESSAGE_ID_BUFFER_MAP_INVALIDMAPTYPE:
            *d3d_message_id = D3D_MESSAGE_ID_BUFFER_MAP_INVALIDMAPTYPE;
            break;
        case D3D10_MESSAGE_ID_BUFFER_MAP_INVALIDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_BUFFER_MAP_INVALIDFLAGS;
            break;
        case D3D10_MESSAGE_ID_BUFFER_MAP_ALREADYMAPPED:
            *d3d_message_id = D3D_MESSAGE_ID_BUFFER_MAP_ALREADYMAPPED;
            break;
        case D3D10_MESSAGE_ID_BUFFER_MAP_DEVICEREMOVED_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_BUFFER_MAP_DEVICEREMOVED_RETURN;
            break;
        case D3D10_MESSAGE_ID_BUFFER_UNMAP_NOTMAPPED:
            *d3d_message_id = D3D_MESSAGE_ID_BUFFER_UNMAP_NOTMAPPED;
            break;
        case D3D10_MESSAGE_ID_TEXTURE1D_MAP_INVALIDMAPTYPE:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE1D_MAP_INVALIDMAPTYPE;
            break;
        case D3D10_MESSAGE_ID_TEXTURE1D_MAP_INVALIDSUBRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE1D_MAP_INVALIDSUBRESOURCE;
            break;
        case D3D10_MESSAGE_ID_TEXTURE1D_MAP_INVALIDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE1D_MAP_INVALIDFLAGS;
            break;
        case D3D10_MESSAGE_ID_TEXTURE1D_MAP_ALREADYMAPPED:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE1D_MAP_ALREADYMAPPED;
            break;
        case D3D10_MESSAGE_ID_TEXTURE1D_MAP_DEVICEREMOVED_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE1D_MAP_DEVICEREMOVED_RETURN;
            break;
        case D3D10_MESSAGE_ID_TEXTURE1D_UNMAP_INVALIDSUBRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE1D_UNMAP_INVALIDSUBRESOURCE;
            break;
        case D3D10_MESSAGE_ID_TEXTURE1D_UNMAP_NOTMAPPED:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE1D_UNMAP_NOTMAPPED;
            break;
        case D3D10_MESSAGE_ID_TEXTURE2D_MAP_INVALIDMAPTYPE:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE2D_MAP_INVALIDMAPTYPE;
            break;
        case D3D10_MESSAGE_ID_TEXTURE2D_MAP_INVALIDSUBRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE2D_MAP_INVALIDSUBRESOURCE;
            break;
        case D3D10_MESSAGE_ID_TEXTURE2D_MAP_INVALIDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE2D_MAP_INVALIDFLAGS;
            break;
        case D3D10_MESSAGE_ID_TEXTURE2D_MAP_ALREADYMAPPED:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE2D_MAP_ALREADYMAPPED;
            break;
        case D3D10_MESSAGE_ID_TEXTURE2D_MAP_DEVICEREMOVED_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE2D_MAP_DEVICEREMOVED_RETURN;
            break;
        case D3D10_MESSAGE_ID_TEXTURE2D_UNMAP_INVALIDSUBRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE2D_UNMAP_INVALIDSUBRESOURCE;
            break;
        case D3D10_MESSAGE_ID_TEXTURE2D_UNMAP_NOTMAPPED:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE2D_UNMAP_NOTMAPPED;
            break;
        case D3D10_MESSAGE_ID_TEXTURE3D_MAP_INVALIDMAPTYPE:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE3D_MAP_INVALIDMAPTYPE;
            break;
        case D3D10_MESSAGE_ID_TEXTURE3D_MAP_INVALIDSUBRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE3D_MAP_INVALIDSUBRESOURCE;
            break;
        case D3D10_MESSAGE_ID_TEXTURE3D_MAP_INVALIDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE3D_MAP_INVALIDFLAGS;
            break;
        case D3D10_MESSAGE_ID_TEXTURE3D_MAP_ALREADYMAPPED:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE3D_MAP_ALREADYMAPPED;
            break;
        case D3D10_MESSAGE_ID_TEXTURE3D_MAP_DEVICEREMOVED_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE3D_MAP_DEVICEREMOVED_RETURN;
            break;
        case D3D10_MESSAGE_ID_TEXTURE3D_UNMAP_INVALIDSUBRESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE3D_UNMAP_INVALIDSUBRESOURCE;
            break;
        case D3D10_MESSAGE_ID_TEXTURE3D_UNMAP_NOTMAPPED:
            *d3d_message_id = D3D_MESSAGE_ID_TEXTURE3D_UNMAP_NOTMAPPED;
            break;
        case D3D10_MESSAGE_ID_CHECKFORMATSUPPORT_FORMAT_DEPRECATED:
            *d3d_message_id = D3D_MESSAGE_ID_CHECKFORMATSUPPORT_FORMAT_DEPRECATED;
            break;
        case D3D10_MESSAGE_ID_CHECKMULTISAMPLEQUALITYLEVELS_FORMAT_DEPRECATED:
            *d3d_message_id = D3D_MESSAGE_ID_CHECKMULTISAMPLEQUALITYLEVELS_FORMAT_DEPRECATED;
            break;
        case D3D10_MESSAGE_ID_SETEXCEPTIONMODE_UNRECOGNIZEDFLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_SETEXCEPTIONMODE_UNRECOGNIZEDFLAGS;
            break;
        case D3D10_MESSAGE_ID_SETEXCEPTIONMODE_INVALIDARG_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_SETEXCEPTIONMODE_INVALIDARG_RETURN;
            break;
        case D3D10_MESSAGE_ID_SETEXCEPTIONMODE_DEVICEREMOVED_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_SETEXCEPTIONMODE_DEVICEREMOVED_RETURN;
            break;
        case D3D10_MESSAGE_ID_REF_SIMULATING_INFINITELY_FAST_HARDWARE:
            *d3d_message_id = D3D_MESSAGE_ID_REF_SIMULATING_INFINITELY_FAST_HARDWARE;
            break;
        case D3D10_MESSAGE_ID_REF_THREADING_MODE:
            *d3d_message_id = D3D_MESSAGE_ID_REF_THREADING_MODE;
            break;
        case D3D10_MESSAGE_ID_REF_UMDRIVER_EXCEPTION:
            *d3d_message_id = D3D_MESSAGE_ID_REF_UMDRIVER_EXCEPTION;
            break;
        case D3D10_MESSAGE_ID_REF_KMDRIVER_EXCEPTION:
            *d3d_message_id = D3D_MESSAGE_ID_REF_KMDRIVER_EXCEPTION;
            break;
        case D3D10_MESSAGE_ID_REF_HARDWARE_EXCEPTION:
            *d3d_message_id = D3D_MESSAGE_ID_REF_HARDWARE_EXCEPTION;
            break;
        case D3D10_MESSAGE_ID_REF_ACCESSING_INDEXABLE_TEMP_OUT_OF_RANGE:
            *d3d_message_id = D3D_MESSAGE_ID_REF_ACCESSING_INDEXABLE_TEMP_OUT_OF_RANGE;
            break;
        case D3D10_MESSAGE_ID_REF_PROBLEM_PARSING_SHADER:
            *d3d_message_id = D3D_MESSAGE_ID_REF_PROBLEM_PARSING_SHADER;
            break;
        case D3D10_MESSAGE_ID_REF_OUT_OF_MEMORY:
            *d3d_message_id = D3D_MESSAGE_ID_REF_OUT_OF_MEMORY;
            break;
        case D3D10_MESSAGE_ID_REF_INFO:
            *d3d_message_id = D3D_MESSAGE_ID_REF_INFO;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEXPOS_OVERFLOW:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_VERTEXPOS_OVERFLOW;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAWINDEXED_INDEXPOS_OVERFLOW:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAWINDEXED_INDEXPOS_OVERFLOW;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAWINSTANCED_VERTEXPOS_OVERFLOW:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAWINSTANCED_VERTEXPOS_OVERFLOW;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAWINSTANCED_INSTANCEPOS_OVERFLOW:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAWINSTANCED_INSTANCEPOS_OVERFLOW;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAWINDEXEDINSTANCED_INSTANCEPOS_OVERFLOW:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAWINDEXEDINSTANCED_INSTANCEPOS_OVERFLOW;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAWINDEXEDINSTANCED_INDEXPOS_OVERFLOW:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAWINDEXEDINSTANCED_INDEXPOS_OVERFLOW;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_SHADER_NOT_SET:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_SHADER_NOT_SET;
            break;
        case D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_SEMANTICNAME_NOT_FOUND:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_SEMANTICNAME_NOT_FOUND;
            break;
        case D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_REGISTERINDEX:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_REGISTERINDEX;
            break;
        case D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_COMPONENTTYPE:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_COMPONENTTYPE;
            break;
        case D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_REGISTERMASK:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_REGISTERMASK;
            break;
        case D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_SYSTEMVALUE:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_SYSTEMVALUE;
            break;
        case D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_NEVERWRITTEN_ALWAYSREADS:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_NEVERWRITTEN_ALWAYSREADS;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_NOT_SET:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_NOT_SET;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_INPUTLAYOUT_NOT_SET:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_INPUTLAYOUT_NOT_SET;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_CONSTANT_BUFFER_NOT_SET:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_CONSTANT_BUFFER_NOT_SET;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_CONSTANT_BUFFER_TOO_SMALL:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_CONSTANT_BUFFER_TOO_SMALL;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_SAMPLER_NOT_SET:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_SAMPLER_NOT_SET;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_SHADERRESOURCEVIEW_NOT_SET:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_SHADERRESOURCEVIEW_NOT_SET;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_VIEW_DIMENSION_MISMATCH:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_VIEW_DIMENSION_MISMATCH;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_STRIDE_TOO_SMALL:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_STRIDE_TOO_SMALL;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_TOO_SMALL:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_TOO_SMALL;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_NOT_SET:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_NOT_SET;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_FORMAT_INVALID:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_FORMAT_INVALID;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_TOO_SMALL:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_TOO_SMALL;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_GS_INPUT_PRIMITIVE_MISMATCH:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_GS_INPUT_PRIMITIVE_MISMATCH;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_RETURN_TYPE_MISMATCH:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_RETURN_TYPE_MISMATCH;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_POSITION_NOT_PRESENT:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_POSITION_NOT_PRESENT;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_OUTPUT_STREAM_NOT_SET:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_OUTPUT_STREAM_NOT_SET;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_BOUND_RESOURCE_MAPPED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_BOUND_RESOURCE_MAPPED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_INVALID_PRIMITIVETOPOLOGY:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_INVALID_PRIMITIVETOPOLOGY;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_OFFSET_UNALIGNED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_OFFSET_UNALIGNED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_STRIDE_UNALIGNED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_STRIDE_UNALIGNED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_INDEX_OFFSET_UNALIGNED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_INDEX_OFFSET_UNALIGNED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_OUTPUT_STREAM_OFFSET_UNALIGNED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_OUTPUT_STREAM_OFFSET_UNALIGNED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_LD_UNSUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_LD_UNSUPPORTED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_SAMPLE_UNSUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_SAMPLE_UNSUPPORTED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_SAMPLE_C_UNSUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_SAMPLE_C_UNSUPPORTED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_MULTISAMPLE_UNSUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_MULTISAMPLE_UNSUPPORTED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_SO_TARGETS_BOUND_WITHOUT_SOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_SO_TARGETS_BOUND_WITHOUT_SOURCE;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_SO_STRIDE_LARGER_THAN_BUFFER:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_SO_STRIDE_LARGER_THAN_BUFFER;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_OM_RENDER_TARGET_DOES_NOT_SUPPORT_BLENDING:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_OM_RENDER_TARGET_DOES_NOT_SUPPORT_BLENDING;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_OM_DUAL_SOURCE_BLENDING_CAN_ONLY_HAVE_RENDER_TARGET_0:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_OM_DUAL_SOURCE_BLENDING_CAN_ONLY_HAVE_RENDER_TARGET_0;
            break;
        case D3D10_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_AT_FAULT:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_AT_FAULT;
            break;
        case D3D10_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_POSSIBLY_AT_FAULT:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_POSSIBLY_AT_FAULT;
            break;
        case D3D10_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_NOT_AT_FAULT:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_NOT_AT_FAULT;
            break;
        case D3D10_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_INVALIDARG_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_INVALIDARG_RETURN;
            break;
        case D3D10_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_OUTOFMEMORY_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_OUTOFMEMORY_RETURN;
            break;
        case D3D10_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_BADINTERFACE_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_BADINTERFACE_RETURN;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_VIEWPORT_NOT_SET:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_VIEWPORT_NOT_SET;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_TRAILING_DIGIT_IN_SEMANTIC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_TRAILING_DIGIT_IN_SEMANTIC;
            break;
        case D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_TRAILING_DIGIT_IN_SEMANTIC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_TRAILING_DIGIT_IN_SEMANTIC;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_DENORMFLUSH:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_DENORMFLUSH;
            break;
        case D3D10_MESSAGE_ID_OMSETRENDERTARGETS_INVALIDVIEW:
            *d3d_message_id = D3D_MESSAGE_ID_OMSETRENDERTARGETS_INVALIDVIEW;
            break;
        case D3D10_MESSAGE_ID_DEVICE_SETTEXTFILTERSIZE_INVALIDDIMENSIONS:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_SETTEXTFILTERSIZE_INVALIDDIMENSIONS;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_SAMPLER_MISMATCH:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_SAMPLER_MISMATCH;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_TYPE_MISMATCH:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_TYPE_MISMATCH;
            break;
        case D3D10_MESSAGE_ID_BLENDSTATE_GETDESC_LEGACY:
            *d3d_message_id = D3D_MESSAGE_ID_BLENDSTATE_GETDESC_LEGACY;
            break;
        case D3D10_MESSAGE_ID_SHADERRESOURCEVIEW_GETDESC_LEGACY:
            *d3d_message_id = D3D_MESSAGE_ID_SHADERRESOURCEVIEW_GETDESC_LEGACY;
            break;
        case D3D10_MESSAGE_ID_CREATEQUERY_OUTOFMEMORY_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEQUERY_OUTOFMEMORY_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATEPREDICATE_OUTOFMEMORY_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEPREDICATE_OUTOFMEMORY_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATECOUNTER_OUTOFRANGE_COUNTER:
            *d3d_message_id = D3D_MESSAGE_ID_CREATECOUNTER_OUTOFRANGE_COUNTER;
            break;
        case D3D10_MESSAGE_ID_CREATECOUNTER_SIMULTANEOUS_ACTIVE_COUNTERS_EXHAUSTED:
            *d3d_message_id = D3D_MESSAGE_ID_CREATECOUNTER_SIMULTANEOUS_ACTIVE_COUNTERS_EXHAUSTED;
            break;
        case D3D10_MESSAGE_ID_CREATECOUNTER_UNSUPPORTED_WELLKNOWN_COUNTER:
            *d3d_message_id = D3D_MESSAGE_ID_CREATECOUNTER_UNSUPPORTED_WELLKNOWN_COUNTER;
            break;
        case D3D10_MESSAGE_ID_CREATECOUNTER_OUTOFMEMORY_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATECOUNTER_OUTOFMEMORY_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATECOUNTER_NONEXCLUSIVE_RETURN:
            *d3d_message_id = D3D_MESSAGE_ID_CREATECOUNTER_NONEXCLUSIVE_RETURN;
            break;
        case D3D10_MESSAGE_ID_CREATECOUNTER_NULLDESC:
            *d3d_message_id = D3D_MESSAGE_ID_CREATECOUNTER_NULLDESC;
            break;
        case D3D10_MESSAGE_ID_CHECKCOUNTER_OUTOFRANGE_COUNTER:
            *d3d_message_id = D3D_MESSAGE_ID_CHECKCOUNTER_OUTOFRANGE_COUNTER;
            break;
        case D3D10_MESSAGE_ID_CHECKCOUNTER_UNSUPPORTED_WELLKNOWN_COUNTER:
            *d3d_message_id = D3D_MESSAGE_ID_CHECKCOUNTER_UNSUPPORTED_WELLKNOWN_COUNTER;
            break;
        case D3D10_MESSAGE_ID_SETPREDICATION_INVALID_PREDICATE_STATE:
            *d3d_message_id = D3D_MESSAGE_ID_SETPREDICATION_INVALID_PREDICATE_STATE;
            break;
        case D3D10_MESSAGE_ID_QUERY_BEGIN_UNSUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_QUERY_BEGIN_UNSUPPORTED;
            break;
        case D3D10_MESSAGE_ID_PREDICATE_BEGIN_DURING_PREDICATION:
            *d3d_message_id = D3D_MESSAGE_ID_PREDICATE_BEGIN_DURING_PREDICATION;
            break;
        case D3D10_MESSAGE_ID_QUERY_BEGIN_DUPLICATE:
            *d3d_message_id = D3D_MESSAGE_ID_QUERY_BEGIN_DUPLICATE;
            break;
        case D3D10_MESSAGE_ID_QUERY_BEGIN_ABANDONING_PREVIOUS_RESULTS:
            *d3d_message_id = D3D_MESSAGE_ID_QUERY_BEGIN_ABANDONING_PREVIOUS_RESULTS;
            break;
        case D3D10_MESSAGE_ID_PREDICATE_END_DURING_PREDICATION:
            *d3d_message_id = D3D_MESSAGE_ID_PREDICATE_END_DURING_PREDICATION;
            break;
        case D3D10_MESSAGE_ID_QUERY_END_ABANDONING_PREVIOUS_RESULTS:
            *d3d_message_id = D3D_MESSAGE_ID_QUERY_END_ABANDONING_PREVIOUS_RESULTS;
            break;
        case D3D10_MESSAGE_ID_QUERY_END_WITHOUT_BEGIN:
            *d3d_message_id = D3D_MESSAGE_ID_QUERY_END_WITHOUT_BEGIN;
            break;
        case D3D10_MESSAGE_ID_QUERY_GETDATA_INVALID_DATASIZE:
            *d3d_message_id = D3D_MESSAGE_ID_QUERY_GETDATA_INVALID_DATASIZE;
            break;
        case D3D10_MESSAGE_ID_QUERY_GETDATA_INVALID_FLAGS:
            *d3d_message_id = D3D_MESSAGE_ID_QUERY_GETDATA_INVALID_FLAGS;
            break;
        case D3D10_MESSAGE_ID_QUERY_GETDATA_INVALID_CALL:
            *d3d_message_id = D3D_MESSAGE_ID_QUERY_GETDATA_INVALID_CALL;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_PS_OUTPUT_TYPE_MISMATCH:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_PS_OUTPUT_TYPE_MISMATCH;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_GATHER_UNSUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_GATHER_UNSUPPORTED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_INVALID_USE_OF_CENTER_MULTISAMPLE_PATTERN:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_INVALID_USE_OF_CENTER_MULTISAMPLE_PATTERN;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_STRIDE_TOO_LARGE:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_STRIDE_TOO_LARGE;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_INVALIDRANGE:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_INVALIDRANGE;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_EMPTY_LAYOUT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_EMPTY_LAYOUT;
            break;
        case D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_SAMPLE_COUNT_MISMATCH:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_SAMPLE_COUNT_MISMATCH;
            break;
        case D3D10_MESSAGE_ID_D3D10_MESSAGES_END:
            *d3d_message_id = D3D_MESSAGE_ID_D3D_MESSAGES_END;
            break;
        case D3D10_MESSAGE_ID_D3D10L9_MESSAGES_START:
            *d3d_message_id = D3D_MESSAGE_ID_D3D_L9_MESSAGES_START;
            break;
        case D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_STENCIL_NO_TWO_SIDED:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_STENCIL_NO_TWO_SIDED;
            break;
        case D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_DepthBiasClamp_NOT_SUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERASTERIZERSTATE_DepthBiasClamp_NOT_SUPPORTED;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_NO_COMPARISON_SUPPORT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_NO_COMPARISON_SUPPORT;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_EXCESSIVE_ANISOTROPY:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_EXCESSIVE_ANISOTROPY;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_BORDER_OUT_OF_RANGE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_BORDER_OUT_OF_RANGE;
            break;
        case D3D10_MESSAGE_ID_VSSETSAMPLERS_NOT_SUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_VSSETSAMPLERS_NOT_SUPPORTED;
            break;
        case D3D10_MESSAGE_ID_VSSETSAMPLERS_TOO_MANY_SAMPLERS:
            *d3d_message_id = D3D_MESSAGE_ID_VSSETSAMPLERS_TOO_MANY_SAMPLERS;
            break;
        case D3D10_MESSAGE_ID_PSSETSAMPLERS_TOO_MANY_SAMPLERS:
            *d3d_message_id = D3D_MESSAGE_ID_PSSETSAMPLERS_TOO_MANY_SAMPLERS;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_NO_ARRAYS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_NO_ARRAYS;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_NO_VB_AND_IB_BIND:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_NO_VB_AND_IB_BIND;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_NO_TEXTURE_1D:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_NO_TEXTURE_1D;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_DIMENSION_OUT_OF_RANGE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_DIMENSION_OUT_OF_RANGE;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_NOT_BINDABLE_AS_SHADER_RESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_NOT_BINDABLE_AS_SHADER_RESOURCE;
            break;
        case D3D10_MESSAGE_ID_OMSETRENDERTARGETS_TOO_MANY_RENDER_TARGETS:
            *d3d_message_id = D3D_MESSAGE_ID_OMSETRENDERTARGETS_TOO_MANY_RENDER_TARGETS;
            break;
        case D3D10_MESSAGE_ID_OMSETRENDERTARGETS_NO_DIFFERING_BIT_DEPTHS:
            *d3d_message_id = D3D_MESSAGE_ID_OMSETRENDERTARGETS_NO_DIFFERING_BIT_DEPTHS;
            break;
        case D3D10_MESSAGE_ID_IASETVERTEXBUFFERS_BAD_BUFFER_INDEX:
            *d3d_message_id = D3D_MESSAGE_ID_IASETVERTEXBUFFERS_BAD_BUFFER_INDEX;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_TOO_MANY_VIEWPORTS:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_TOO_MANY_VIEWPORTS;
            break;
        case D3D10_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_ADJACENCY_UNSUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_ADJACENCY_UNSUPPORTED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_TOO_MANY_SCISSORS:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_TOO_MANY_SCISSORS;
            break;
        case D3D10_MESSAGE_ID_COPYRESOURCE_ONLY_TEXTURE_2D_WITHIN_GPU_MEMORY:
            *d3d_message_id = D3D_MESSAGE_ID_COPYRESOURCE_ONLY_TEXTURE_2D_WITHIN_GPU_MEMORY;
            break;
        case D3D10_MESSAGE_ID_COPYRESOURCE_NO_TEXTURE_3D_READBACK:
            *d3d_message_id = D3D_MESSAGE_ID_COPYRESOURCE_NO_TEXTURE_3D_READBACK;
            break;
        case D3D10_MESSAGE_ID_COPYRESOURCE_NO_TEXTURE_ONLY_READBACK:
            *d3d_message_id = D3D_MESSAGE_ID_COPYRESOURCE_NO_TEXTURE_ONLY_READBACK;
            break;
        case D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_UNSUPPORTED_FORMAT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEINPUTLAYOUT_UNSUPPORTED_FORMAT;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_NO_ALPHA_TO_COVERAGE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_NO_ALPHA_TO_COVERAGE;
            break;
        case D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_DepthClipEnable_MUST_BE_TRUE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERASTERIZERSTATE_DepthClipEnable_MUST_BE_TRUE;
            break;
        case D3D10_MESSAGE_ID_DRAWINDEXED_STARTINDEXLOCATION_MUST_BE_POSITIVE:
            *d3d_message_id = D3D_MESSAGE_ID_DRAWINDEXED_STARTINDEXLOCATION_MUST_BE_POSITIVE;
            break;
        case D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_MUST_USE_LOWEST_LOD:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_MUST_USE_LOWEST_LOD;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_MINLOD_MUST_NOT_BE_FRACTIONAL:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_MINLOD_MUST_NOT_BE_FRACTIONAL;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_MAXLOD_MUST_BE_FLT_MAX:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_MAXLOD_MUST_BE_FLT_MAX;
            break;
        case D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_FIRSTARRAYSLICE_MUST_BE_ZERO:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_FIRSTARRAYSLICE_MUST_BE_ZERO;
            break;
        case D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_CUBES_MUST_HAVE_6_SIDES:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_CUBES_MUST_HAVE_6_SIDES;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_NOT_BINDABLE_AS_RENDER_TARGET:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_NOT_BINDABLE_AS_RENDER_TARGET;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_NO_DWORD_INDEX_BUFFER:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_NO_DWORD_INDEX_BUFFER;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_MSAA_PRECLUDES_SHADER_RESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_MSAA_PRECLUDES_SHADER_RESOURCE;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_PRESENTATION_PRECLUDES_SHADER_RESOURCE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_PRESENTATION_PRECLUDES_SHADER_RESOURCE;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_NO_INDEPENDENT_BLEND_ENABLE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_NO_INDEPENDENT_BLEND_ENABLE;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_NO_INDEPENDENT_WRITE_MASKS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_NO_INDEPENDENT_WRITE_MASKS;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_NO_STREAM_OUT:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_NO_STREAM_OUT;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_ONLY_VB_IB_FOR_BUFFERS:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_ONLY_VB_IB_FOR_BUFFERS;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_NO_AUTOGEN_FOR_VOLUMES:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_NO_AUTOGEN_FOR_VOLUMES;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_DXGI_FORMAT_R8G8B8A8_CANNOT_BE_SHARED:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_DXGI_FORMAT_R8G8B8A8_CANNOT_BE_SHARED;
            break;
        case D3D10_MESSAGE_ID_VSSHADERRESOURCES_NOT_SUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_VSSHADERRESOURCES_NOT_SUPPORTED;
            break;
        case D3D10_MESSAGE_ID_GEOMETRY_SHADER_NOT_SUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_GEOMETRY_SHADER_NOT_SUPPORTED;
            break;
        case D3D10_MESSAGE_ID_STREAM_OUT_NOT_SUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_STREAM_OUT_NOT_SUPPORTED;
            break;
        case D3D10_MESSAGE_ID_TEXT_FILTER_NOT_SUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_TEXT_FILTER_NOT_SUPPORTED;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_NO_SEPARATE_ALPHA_BLEND:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_NO_SEPARATE_ALPHA_BLEND;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_NO_MRT_BLEND:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_NO_MRT_BLEND;
            break;
        case D3D10_MESSAGE_ID_CREATEBLENDSTATE_OPERATION_NOT_SUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_CREATEBLENDSTATE_OPERATION_NOT_SUPPORTED;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_NO_MIRRORONCE:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_NO_MIRRORONCE;
            break;
        case D3D10_MESSAGE_ID_DRAWINSTANCED_NOT_SUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_DRAWINSTANCED_NOT_SUPPORTED;
            break;
        case D3D10_MESSAGE_ID_DRAWINDEXEDINSTANCED_NOT_SUPPORTED_BELOW_9_3:
            *d3d_message_id = D3D_MESSAGE_ID_DRAWINDEXEDINSTANCED_NOT_SUPPORTED_BELOW_9_3;
            break;
        case D3D10_MESSAGE_ID_DRAWINDEXED_POINTLIST_UNSUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_DRAWINDEXED_POINTLIST_UNSUPPORTED;
            break;
        case D3D10_MESSAGE_ID_SETBLENDSTATE_SAMPLE_MASK_CANNOT_BE_ZERO:
            *d3d_message_id = D3D_MESSAGE_ID_SETBLENDSTATE_SAMPLE_MASK_CANNOT_BE_ZERO;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_DIMENSION_EXCEEDS_FEATURE_LEVEL_DEFINITION:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_DIMENSION_EXCEEDS_FEATURE_LEVEL_DEFINITION;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_ONLY_SINGLE_MIP_LEVEL_DEPTH_STENCIL_SUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_ONLY_SINGLE_MIP_LEVEL_DEPTH_STENCIL_SUPPORTED;
            break;
        case D3D10_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_NEGATIVESCISSOR:
            *d3d_message_id = D3D_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_NEGATIVESCISSOR;
            break;
        case D3D10_MESSAGE_ID_SLOT_ZERO_MUST_BE_D3D10_INPUT_PER_VERTEX_DATA:
            *d3d_message_id = D3D_MESSAGE_ID_SLOT_ZERO_MUST_BE_D3D_INPUT_PER_VERTEX_DATA;
            break;
        case D3D10_MESSAGE_ID_CREATERESOURCE_NON_POW_2_MIPMAP:
            *d3d_message_id = D3D_MESSAGE_ID_CREATERESOURCE_NON_POW_2_MIPMAP;
            break;
        case D3D10_MESSAGE_ID_CREATESAMPLERSTATE_BORDER_NOT_SUPPORTED:
            *d3d_message_id = D3D_MESSAGE_ID_CREATESAMPLERSTATE_BORDER_NOT_SUPPORTED;
            break;
        case D3D10_MESSAGE_ID_OMSETRENDERTARGETS_NO_SRGB_MRT:
            *d3d_message_id = D3D_MESSAGE_ID_OMSETRENDERTARGETS_NO_SRGB_MRT;
            break;
        case D3D10_MESSAGE_ID_D3D10L9_MESSAGES_END:
            *d3d_message_id = D3D_MESSAGE_ID_D3D_L9_MESSAGES_END;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_MESSAGE_ID *d3d_message_id_10, D3D_MESSAGE_ID d3d_message_id )
{
    switch( d3d_message_id )
    {
        default:
            *d3d_message_id_10 = (D3D10_MESSAGE_ID)d3d_message_id;
            break;
        case D3D_MESSAGE_ID_UNKNOWN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_UNKNOWN;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_HAZARD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_HAZARD;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_HAZARD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_HAZARD;
            break;
        case D3D_MESSAGE_ID_DEVICE_VSSETSHADERRESOURCES_HAZARD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_VSSETSHADERRESOURCES_HAZARD;
            break;
        case D3D_MESSAGE_ID_DEVICE_VSSETCONSTANTBUFFERS_HAZARD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_VSSETCONSTANTBUFFERS_HAZARD;
            break;
        case D3D_MESSAGE_ID_DEVICE_GSSETSHADERRESOURCES_HAZARD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_GSSETSHADERRESOURCES_HAZARD;
            break;
        case D3D_MESSAGE_ID_DEVICE_GSSETCONSTANTBUFFERS_HAZARD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_GSSETCONSTANTBUFFERS_HAZARD;
            break;
        case D3D_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD;
            break;
        case D3D_MESSAGE_ID_DEVICE_PSSETCONSTANTBUFFERS_HAZARD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_PSSETCONSTANTBUFFERS_HAZARD;
            break;
        case D3D_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD;
            break;
        case D3D_MESSAGE_ID_DEVICE_SOSETTARGETS_HAZARD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_SOSETTARGETS_HAZARD;
            break;
        case D3D_MESSAGE_ID_STRING_FROM_APPLICATION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_STRING_FROM_APPLICATION;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_THIS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_THIS;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER1:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER1;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER2:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER2;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER3:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER3;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER4:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER4;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER5:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER5;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER6:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER6;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER7:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER7;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER8:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER8;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER9:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER9;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER10:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER10;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER11:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER11;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER12:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER12;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER13:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER13;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER14:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER14;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_PARAMETER15:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_PARAMETER15;
            break;
        case D3D_MESSAGE_ID_CORRUPTED_MULTITHREADING:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CORRUPTED_MULTITHREADING;
            break;
        case D3D_MESSAGE_ID_MESSAGE_REPORTING_OUTOFMEMORY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_MESSAGE_REPORTING_OUTOFMEMORY;
            break;
        case D3D_MESSAGE_ID_IASETINPUTLAYOUT_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_IASETINPUTLAYOUT_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_IASETVERTEXBUFFERS_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_IASETVERTEXBUFFERS_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_IASETINDEXBUFFER_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_IASETINDEXBUFFER_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_VSSETSHADER_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_VSSETSHADER_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_VSSETSHADERRESOURCES_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_VSSETSHADERRESOURCES_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_VSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_VSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_VSSETSAMPLERS_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_VSSETSAMPLERS_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_GSSETSHADER_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_GSSETSHADER_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_GSSETSHADERRESOURCES_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_GSSETSHADERRESOURCES_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_GSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_GSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_GSSETSAMPLERS_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_GSSETSAMPLERS_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_SOSETTARGETS_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SOSETTARGETS_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_PSSETSHADER_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_PSSETSHADER_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_PSSETSHADERRESOURCES_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_PSSETSHADERRESOURCES_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_PSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_PSSETCONSTANTBUFFERS_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_PSSETSAMPLERS_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_PSSETSAMPLERS_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_RSSETSTATE_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_RSSETSTATE_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_OMSETBLENDSTATE_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_OMSETBLENDSTATE_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_OMSETDEPTHSTENCILSTATE_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_OMSETDEPTHSTENCILSTATE_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_OMSETRENDERTARGETS_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_OMSETRENDERTARGETS_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_SETPREDICATION_UNBINDDELETINGOBJECT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SETPREDICATION_UNBINDDELETINGOBJECT;
            break;
        case D3D_MESSAGE_ID_GETPRIVATEDATA_MOREDATA:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_GETPRIVATEDATA_MOREDATA;
            break;
        case D3D_MESSAGE_ID_SETPRIVATEDATA_INVALIDFREEDATA:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SETPRIVATEDATA_INVALIDFREEDATA;
            break;
        case D3D_MESSAGE_ID_SETPRIVATEDATA_INVALIDIUNKNOWN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SETPRIVATEDATA_INVALIDIUNKNOWN;
            break;
        case D3D_MESSAGE_ID_SETPRIVATEDATA_INVALIDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SETPRIVATEDATA_INVALIDFLAGS;
            break;
        case D3D_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS;
            break;
        case D3D_MESSAGE_ID_SETPRIVATEDATA_OUTOFMEMORY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SETPRIVATEDATA_OUTOFMEMORY;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_INVALIDSAMPLES:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDSAMPLES;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDUSAGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDUSAGE;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDBINDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDBINDFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDCPUACCESSFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDCPUACCESSFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDMISCFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDMISCFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_INVALIDCPUACCESSFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDCPUACCESSFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_INVALIDBINDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDBINDFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_INVALIDINITIALDATA:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDINITIALDATA;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_INVALIDDIMENSIONS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDDIMENSIONS;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_INVALIDMIPLEVELS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDMIPLEVELS;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_INVALIDMISCFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDMISCFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_INVALIDARG_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDARG_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_OUTOFMEMORY_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_OUTOFMEMORY_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_NULLDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_NULLDESC;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_INVALIDCONSTANTBUFFERBINDINGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_INVALIDCONSTANTBUFFERBINDINGS;
            break;
        case D3D_MESSAGE_ID_CREATEBUFFER_LARGEALLOCATION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBUFFER_LARGEALLOCATION;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_UNSUPPORTEDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_UNSUPPORTEDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDSAMPLES:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDSAMPLES;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDUSAGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDUSAGE;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDBINDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDBINDFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDCPUACCESSFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDCPUACCESSFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDMISCFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDMISCFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDCPUACCESSFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDCPUACCESSFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDBINDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDBINDFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDINITIALDATA:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDINITIALDATA;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDDIMENSIONS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDDIMENSIONS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDMIPLEVELS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDMIPLEVELS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDMISCFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDMISCFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_INVALIDARG_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_INVALIDARG_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_OUTOFMEMORY_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_OUTOFMEMORY_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_NULLDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_NULLDESC;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE1D_LARGEALLOCATION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE1D_LARGEALLOCATION;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_UNSUPPORTEDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_UNSUPPORTEDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDSAMPLES:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDSAMPLES;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDUSAGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDUSAGE;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDBINDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDBINDFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDCPUACCESSFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDCPUACCESSFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDMISCFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_UNRECOGNIZEDMISCFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDCPUACCESSFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDCPUACCESSFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDBINDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDBINDFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDINITIALDATA:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDINITIALDATA;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDDIMENSIONS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDDIMENSIONS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDMIPLEVELS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDMIPLEVELS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDMISCFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDMISCFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_INVALIDARG_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_INVALIDARG_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_OUTOFMEMORY_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_OUTOFMEMORY_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_NULLDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_NULLDESC;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE2D_LARGEALLOCATION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE2D_LARGEALLOCATION;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_UNSUPPORTEDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_UNSUPPORTEDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDSAMPLES:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDSAMPLES;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDUSAGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDUSAGE;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDBINDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDBINDFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDCPUACCESSFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDCPUACCESSFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDMISCFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_UNRECOGNIZEDMISCFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDCPUACCESSFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDCPUACCESSFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDBINDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDBINDFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDINITIALDATA:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDINITIALDATA;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDDIMENSIONS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDDIMENSIONS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDMIPLEVELS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDMIPLEVELS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDMISCFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDMISCFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_INVALIDARG_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_INVALIDARG_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_OUTOFMEMORY_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_OUTOFMEMORY_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_NULLDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_NULLDESC;
            break;
        case D3D_MESSAGE_ID_CREATETEXTURE3D_LARGEALLOCATION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATETEXTURE3D_LARGEALLOCATION;
            break;
        case D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_UNRECOGNIZEDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_UNRECOGNIZEDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDDESC;
            break;
        case D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDDIMENSIONS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDDIMENSIONS;
            break;
        case D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDRESOURCE;
            break;
        case D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_TOOMANYOBJECTS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_TOOMANYOBJECTS;
            break;
        case D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDARG_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_INVALIDARG_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_OUTOFMEMORY_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_OUTOFMEMORY_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_UNRECOGNIZEDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_UNRECOGNIZEDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_UNSUPPORTEDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_UNSUPPORTEDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDDESC;
            break;
        case D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDDIMENSIONS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDDIMENSIONS;
            break;
        case D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDRESOURCE;
            break;
        case D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_TOOMANYOBJECTS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_TOOMANYOBJECTS;
            break;
        case D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDARG_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_INVALIDARG_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATERENDERTARGETVIEW_OUTOFMEMORY_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERENDERTARGETVIEW_OUTOFMEMORY_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_UNRECOGNIZEDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_UNRECOGNIZEDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDDESC;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDDIMENSIONS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDDIMENSIONS;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDRESOURCE;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_TOOMANYOBJECTS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_TOOMANYOBJECTS;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDARG_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_INVALIDARG_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_OUTOFMEMORY_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILVIEW_OUTOFMEMORY_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_OUTOFMEMORY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_OUTOFMEMORY;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_TOOMANYELEMENTS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_TOOMANYELEMENTS;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INCOMPATIBLEFORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INCOMPATIBLEFORMAT;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSLOT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSLOT;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDINPUTSLOTCLASS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDINPUTSLOTCLASS;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_STEPRATESLOTCLASSMISMATCH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_STEPRATESLOTCLASSMISMATCH;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSLOTCLASSCHANGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSLOTCLASSCHANGE;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSTEPRATECHANGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDSTEPRATECHANGE;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDALIGNMENT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_INVALIDALIGNMENT;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_DUPLICATESEMANTIC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_DUPLICATESEMANTIC;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_UNPARSEABLEINPUTSIGNATURE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_UNPARSEABLEINPUTSIGNATURE;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_NULLSEMANTIC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_NULLSEMANTIC;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_MISSINGELEMENT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_MISSINGELEMENT;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_NULLDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_NULLDESC;
            break;
        case D3D_MESSAGE_ID_CREATEVERTEXSHADER_OUTOFMEMORY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEVERTEXSHADER_OUTOFMEMORY;
            break;
        case D3D_MESSAGE_ID_CREATEVERTEXSHADER_INVALIDSHADERBYTECODE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEVERTEXSHADER_INVALIDSHADERBYTECODE;
            break;
        case D3D_MESSAGE_ID_CREATEVERTEXSHADER_INVALIDSHADERTYPE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEVERTEXSHADER_INVALIDSHADERTYPE;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADER_OUTOFMEMORY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADER_OUTOFMEMORY;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADER_INVALIDSHADERBYTECODE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADER_INVALIDSHADERBYTECODE;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADER_INVALIDSHADERTYPE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADER_INVALIDSHADERTYPE;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTOFMEMORY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTOFMEMORY;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSHADERBYTECODE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSHADERBYTECODE;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSHADERTYPE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSHADERTYPE;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDNUMENTRIES:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDNUMENTRIES;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTPUTSTREAMSTRIDEUNUSED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTPUTSTREAMSTRIDEUNUSED;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_UNEXPECTEDDECL:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_UNEXPECTEDDECL;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_EXPECTEDDECL:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_EXPECTEDDECL;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTPUTSLOT0EXPECTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_OUTPUTSLOT0EXPECTED;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDOUTPUTSLOT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDOUTPUTSLOT;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_ONLYONEELEMENTPERSLOT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_ONLYONEELEMENTPERSLOT;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDCOMPONENTCOUNT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDCOMPONENTCOUNT;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSTARTCOMPONENTANDCOMPONENTCOUNT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDSTARTCOMPONENTANDCOMPONENTCOUNT;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDGAPDEFINITION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDGAPDEFINITION;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_REPEATEDOUTPUT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_REPEATEDOUTPUT;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDOUTPUTSTREAMSTRIDE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDOUTPUTSTREAMSTRIDE;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MISSINGSEMANTIC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MISSINGSEMANTIC;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MASKMISMATCH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MASKMISMATCH;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_CANTHAVEONLYGAPS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_CANTHAVEONLYGAPS;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_DECLTOOCOMPLEX:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_DECLTOOCOMPLEX;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MISSINGOUTPUTSIGNATURE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_MISSINGOUTPUTSIGNATURE;
            break;
        case D3D_MESSAGE_ID_CREATEPIXELSHADER_OUTOFMEMORY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEPIXELSHADER_OUTOFMEMORY;
            break;
        case D3D_MESSAGE_ID_CREATEPIXELSHADER_INVALIDSHADERBYTECODE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEPIXELSHADER_INVALIDSHADERBYTECODE;
            break;
        case D3D_MESSAGE_ID_CREATEPIXELSHADER_INVALIDSHADERTYPE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEPIXELSHADER_INVALIDSHADERTYPE;
            break;
        case D3D_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDFILLMODE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDFILLMODE;
            break;
        case D3D_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDCULLMODE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDCULLMODE;
            break;
        case D3D_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDDEPTHBIASCLAMP:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDDEPTHBIASCLAMP;
            break;
        case D3D_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDSLOPESCALEDDEPTHBIAS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_INVALIDSLOPESCALEDDEPTHBIAS;
            break;
        case D3D_MESSAGE_ID_CREATERASTERIZERSTATE_TOOMANYOBJECTS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_TOOMANYOBJECTS;
            break;
        case D3D_MESSAGE_ID_CREATERASTERIZERSTATE_NULLDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_NULLDESC;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDDEPTHWRITEMASK:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDDEPTHWRITEMASK;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDDEPTHFUNC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDDEPTHFUNC;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILFAILOP:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILFAILOP;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILZFAILOP:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILZFAILOP;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILPASSOP:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILPASSOP;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILFUNC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDFRONTFACESTENCILFUNC;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILFAILOP:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILFAILOP;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILZFAILOP:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILZFAILOP;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILPASSOP:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILPASSOP;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILFUNC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_INVALIDBACKFACESTENCILFUNC;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_TOOMANYOBJECTS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_TOOMANYOBJECTS;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_NULLDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_NULLDESC;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDSRCBLEND:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDSRCBLEND;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDDESTBLEND:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDDESTBLEND;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDBLENDOP:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDBLENDOP;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDSRCBLENDALPHA:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDSRCBLENDALPHA;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDDESTBLENDALPHA:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDDESTBLENDALPHA;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDBLENDOPALPHA:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDBLENDOPALPHA;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_INVALIDRENDERTARGETWRITEMASK:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_INVALIDRENDERTARGETWRITEMASK;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_TOOMANYOBJECTS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_TOOMANYOBJECTS;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_NULLDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_NULLDESC;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDFILTER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDFILTER;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSU:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSU;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSV:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSV;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSW:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDADDRESSW;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMIPLODBIAS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMIPLODBIAS;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMAXANISOTROPY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMAXANISOTROPY;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDCOMPARISONFUNC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDCOMPARISONFUNC;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMINLOD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMINLOD;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMAXLOD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_INVALIDMAXLOD;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_TOOMANYOBJECTS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_TOOMANYOBJECTS;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_NULLDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_NULLDESC;
            break;
        case D3D_MESSAGE_ID_CREATEQUERYORPREDICATE_INVALIDQUERY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEQUERYORPREDICATE_INVALIDQUERY;
            break;
        case D3D_MESSAGE_ID_CREATEQUERYORPREDICATE_INVALIDMISCFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEQUERYORPREDICATE_INVALIDMISCFLAGS;
            break;
        case D3D_MESSAGE_ID_CREATEQUERYORPREDICATE_UNEXPECTEDMISCFLAG:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEQUERYORPREDICATE_UNEXPECTEDMISCFLAG;
            break;
        case D3D_MESSAGE_ID_CREATEQUERYORPREDICATE_NULLDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEQUERYORPREDICATE_NULLDESC;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_TOPOLOGY_UNRECOGNIZED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_TOPOLOGY_UNRECOGNIZED;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_TOPOLOGY_UNDEFINED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_TOPOLOGY_UNDEFINED;
            break;
        case D3D_MESSAGE_ID_IASETVERTEXBUFFERS_INVALIDBUFFER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_IASETVERTEXBUFFERS_INVALIDBUFFER;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_OFFSET_TOO_LARGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_OFFSET_TOO_LARGE;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_IASETINDEXBUFFER_INVALIDBUFFER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_IASETINDEXBUFFER_INVALIDBUFFER;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_FORMAT_INVALID:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_FORMAT_INVALID;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_OFFSET_TOO_LARGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_OFFSET_TOO_LARGE;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_OFFSET_UNALIGNED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_OFFSET_UNALIGNED;
            break;
        case D3D_MESSAGE_ID_DEVICE_VSSETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_VSSETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D_MESSAGE_ID_VSSETCONSTANTBUFFERS_INVALIDBUFFER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_VSSETCONSTANTBUFFERS_INVALIDBUFFER;
            break;
        case D3D_MESSAGE_ID_DEVICE_VSSETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_VSSETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_VSSETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_VSSETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_GSSETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_GSSETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D_MESSAGE_ID_GSSETCONSTANTBUFFERS_INVALIDBUFFER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_GSSETCONSTANTBUFFERS_INVALIDBUFFER;
            break;
        case D3D_MESSAGE_ID_DEVICE_GSSETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_GSSETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_GSSETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_GSSETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_SOSETTARGETS_INVALIDBUFFER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SOSETTARGETS_INVALIDBUFFER;
            break;
        case D3D_MESSAGE_ID_DEVICE_SOSETTARGETS_OFFSET_UNALIGNED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_SOSETTARGETS_OFFSET_UNALIGNED;
            break;
        case D3D_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D_MESSAGE_ID_PSSETCONSTANTBUFFERS_INVALIDBUFFER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_PSSETCONSTANTBUFFERS_INVALIDBUFFER;
            break;
        case D3D_MESSAGE_ID_DEVICE_PSSETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_PSSETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_PSSETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_PSSETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_INVALIDVIEWPORT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_INVALIDVIEWPORT;
            break;
        case D3D_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_INVALIDSCISSOR:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_INVALIDSCISSOR;
            break;
        case D3D_MESSAGE_ID_CLEARRENDERTARGETVIEW_DENORMFLUSH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CLEARRENDERTARGETVIEW_DENORMFLUSH;
            break;
        case D3D_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_DENORMFLUSH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_DENORMFLUSH;
            break;
        case D3D_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_INVALID:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_INVALID;
            break;
        case D3D_MESSAGE_ID_DEVICE_IAGETVERTEXBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IAGETVERTEXBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_VSGETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_VSGETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_VSGETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_VSGETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_VSGETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_VSGETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_GSGETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_GSGETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_GSGETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_GSGETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_GSGETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_GSGETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_SOGETTARGETS_BUFFERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_SOGETTARGETS_BUFFERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_PSGETSHADERRESOURCES_VIEWS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_PSGETSHADERRESOURCES_VIEWS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_PSGETCONSTANTBUFFERS_BUFFERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_PSGETCONSTANTBUFFERS_BUFFERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_PSGETSAMPLERS_SAMPLERS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_PSGETSAMPLERS_SAMPLERS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_RSGETVIEWPORTS_VIEWPORTS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RSGETVIEWPORTS_VIEWPORTS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_RSGETSCISSORRECTS_RECTS_EMPTY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RSGETSCISSORRECTS_RECTS_EMPTY;
            break;
        case D3D_MESSAGE_ID_DEVICE_GENERATEMIPS_RESOURCE_INVALID:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_GENERATEMIPS_RESOURCE_INVALID;
            break;
        case D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDDESTINATIONSUBRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDDESTINATIONSUBRESOURCE;
            break;
        case D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCESUBRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCESUBRESOURCE;
            break;
        case D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCEBOX:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCEBOX;
            break;
        case D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCE;
            break;
        case D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDDESTINATIONSTATE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDDESTINATIONSTATE;
            break;
        case D3D_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCESTATE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYSUBRESOURCEREGION_INVALIDSOURCESTATE;
            break;
        case D3D_MESSAGE_ID_COPYRESOURCE_INVALIDSOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYRESOURCE_INVALIDSOURCE;
            break;
        case D3D_MESSAGE_ID_COPYRESOURCE_INVALIDDESTINATIONSTATE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYRESOURCE_INVALIDDESTINATIONSTATE;
            break;
        case D3D_MESSAGE_ID_COPYRESOURCE_INVALIDSOURCESTATE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYRESOURCE_INVALIDSOURCESTATE;
            break;
        case D3D_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONSUBRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONSUBRESOURCE;
            break;
        case D3D_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONBOX:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONBOX;
            break;
        case D3D_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONSTATE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_UPDATESUBRESOURCE_INVALIDDESTINATIONSTATE;
            break;
        case D3D_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_DESTINATION_INVALID:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_DESTINATION_INVALID;
            break;
        case D3D_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_DESTINATION_SUBRESOURCE_INVALID:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_DESTINATION_SUBRESOURCE_INVALID;
            break;
        case D3D_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_SOURCE_INVALID:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_SOURCE_INVALID;
            break;
        case D3D_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_SOURCE_SUBRESOURCE_INVALID:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_SOURCE_SUBRESOURCE_INVALID;
            break;
        case D3D_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_FORMAT_INVALID:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RESOLVESUBRESOURCE_FORMAT_INVALID;
            break;
        case D3D_MESSAGE_ID_BUFFER_MAP_INVALIDMAPTYPE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_BUFFER_MAP_INVALIDMAPTYPE;
            break;
        case D3D_MESSAGE_ID_BUFFER_MAP_INVALIDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_BUFFER_MAP_INVALIDFLAGS;
            break;
        case D3D_MESSAGE_ID_BUFFER_MAP_ALREADYMAPPED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_BUFFER_MAP_ALREADYMAPPED;
            break;
        case D3D_MESSAGE_ID_BUFFER_MAP_DEVICEREMOVED_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_BUFFER_MAP_DEVICEREMOVED_RETURN;
            break;
        case D3D_MESSAGE_ID_BUFFER_UNMAP_NOTMAPPED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_BUFFER_UNMAP_NOTMAPPED;
            break;
        case D3D_MESSAGE_ID_TEXTURE1D_MAP_INVALIDMAPTYPE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE1D_MAP_INVALIDMAPTYPE;
            break;
        case D3D_MESSAGE_ID_TEXTURE1D_MAP_INVALIDSUBRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE1D_MAP_INVALIDSUBRESOURCE;
            break;
        case D3D_MESSAGE_ID_TEXTURE1D_MAP_INVALIDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE1D_MAP_INVALIDFLAGS;
            break;
        case D3D_MESSAGE_ID_TEXTURE1D_MAP_ALREADYMAPPED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE1D_MAP_ALREADYMAPPED;
            break;
        case D3D_MESSAGE_ID_TEXTURE1D_MAP_DEVICEREMOVED_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE1D_MAP_DEVICEREMOVED_RETURN;
            break;
        case D3D_MESSAGE_ID_TEXTURE1D_UNMAP_INVALIDSUBRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE1D_UNMAP_INVALIDSUBRESOURCE;
            break;
        case D3D_MESSAGE_ID_TEXTURE1D_UNMAP_NOTMAPPED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE1D_UNMAP_NOTMAPPED;
            break;
        case D3D_MESSAGE_ID_TEXTURE2D_MAP_INVALIDMAPTYPE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE2D_MAP_INVALIDMAPTYPE;
            break;
        case D3D_MESSAGE_ID_TEXTURE2D_MAP_INVALIDSUBRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE2D_MAP_INVALIDSUBRESOURCE;
            break;
        case D3D_MESSAGE_ID_TEXTURE2D_MAP_INVALIDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE2D_MAP_INVALIDFLAGS;
            break;
        case D3D_MESSAGE_ID_TEXTURE2D_MAP_ALREADYMAPPED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE2D_MAP_ALREADYMAPPED;
            break;
        case D3D_MESSAGE_ID_TEXTURE2D_MAP_DEVICEREMOVED_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE2D_MAP_DEVICEREMOVED_RETURN;
            break;
        case D3D_MESSAGE_ID_TEXTURE2D_UNMAP_INVALIDSUBRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE2D_UNMAP_INVALIDSUBRESOURCE;
            break;
        case D3D_MESSAGE_ID_TEXTURE2D_UNMAP_NOTMAPPED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE2D_UNMAP_NOTMAPPED;
            break;
        case D3D_MESSAGE_ID_TEXTURE3D_MAP_INVALIDMAPTYPE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE3D_MAP_INVALIDMAPTYPE;
            break;
        case D3D_MESSAGE_ID_TEXTURE3D_MAP_INVALIDSUBRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE3D_MAP_INVALIDSUBRESOURCE;
            break;
        case D3D_MESSAGE_ID_TEXTURE3D_MAP_INVALIDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE3D_MAP_INVALIDFLAGS;
            break;
        case D3D_MESSAGE_ID_TEXTURE3D_MAP_ALREADYMAPPED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE3D_MAP_ALREADYMAPPED;
            break;
        case D3D_MESSAGE_ID_TEXTURE3D_MAP_DEVICEREMOVED_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE3D_MAP_DEVICEREMOVED_RETURN;
            break;
        case D3D_MESSAGE_ID_TEXTURE3D_UNMAP_INVALIDSUBRESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE3D_UNMAP_INVALIDSUBRESOURCE;
            break;
        case D3D_MESSAGE_ID_TEXTURE3D_UNMAP_NOTMAPPED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXTURE3D_UNMAP_NOTMAPPED;
            break;
        case D3D_MESSAGE_ID_CHECKFORMATSUPPORT_FORMAT_DEPRECATED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CHECKFORMATSUPPORT_FORMAT_DEPRECATED;
            break;
        case D3D_MESSAGE_ID_CHECKMULTISAMPLEQUALITYLEVELS_FORMAT_DEPRECATED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CHECKMULTISAMPLEQUALITYLEVELS_FORMAT_DEPRECATED;
            break;
        case D3D_MESSAGE_ID_SETEXCEPTIONMODE_UNRECOGNIZEDFLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SETEXCEPTIONMODE_UNRECOGNIZEDFLAGS;
            break;
        case D3D_MESSAGE_ID_SETEXCEPTIONMODE_INVALIDARG_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SETEXCEPTIONMODE_INVALIDARG_RETURN;
            break;
        case D3D_MESSAGE_ID_SETEXCEPTIONMODE_DEVICEREMOVED_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SETEXCEPTIONMODE_DEVICEREMOVED_RETURN;
            break;
        case D3D_MESSAGE_ID_REF_SIMULATING_INFINITELY_FAST_HARDWARE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_REF_SIMULATING_INFINITELY_FAST_HARDWARE;
            break;
        case D3D_MESSAGE_ID_REF_THREADING_MODE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_REF_THREADING_MODE;
            break;
        case D3D_MESSAGE_ID_REF_UMDRIVER_EXCEPTION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_REF_UMDRIVER_EXCEPTION;
            break;
        case D3D_MESSAGE_ID_REF_KMDRIVER_EXCEPTION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_REF_KMDRIVER_EXCEPTION;
            break;
        case D3D_MESSAGE_ID_REF_HARDWARE_EXCEPTION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_REF_HARDWARE_EXCEPTION;
            break;
        case D3D_MESSAGE_ID_REF_ACCESSING_INDEXABLE_TEMP_OUT_OF_RANGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_REF_ACCESSING_INDEXABLE_TEMP_OUT_OF_RANGE;
            break;
        case D3D_MESSAGE_ID_REF_PROBLEM_PARSING_SHADER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_REF_PROBLEM_PARSING_SHADER;
            break;
        case D3D_MESSAGE_ID_REF_OUT_OF_MEMORY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_REF_OUT_OF_MEMORY;
            break;
        case D3D_MESSAGE_ID_REF_INFO:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_REF_INFO;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_VERTEXPOS_OVERFLOW:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEXPOS_OVERFLOW;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAWINDEXED_INDEXPOS_OVERFLOW:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAWINDEXED_INDEXPOS_OVERFLOW;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAWINSTANCED_VERTEXPOS_OVERFLOW:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAWINSTANCED_VERTEXPOS_OVERFLOW;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAWINSTANCED_INSTANCEPOS_OVERFLOW:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAWINSTANCED_INSTANCEPOS_OVERFLOW;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAWINDEXEDINSTANCED_INSTANCEPOS_OVERFLOW:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAWINDEXEDINSTANCED_INSTANCEPOS_OVERFLOW;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAWINDEXEDINSTANCED_INDEXPOS_OVERFLOW:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAWINDEXEDINSTANCED_INDEXPOS_OVERFLOW;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_SHADER_NOT_SET:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_SHADER_NOT_SET;
            break;
        case D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_SEMANTICNAME_NOT_FOUND:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_SEMANTICNAME_NOT_FOUND;
            break;
        case D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_REGISTERINDEX:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_REGISTERINDEX;
            break;
        case D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_COMPONENTTYPE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_COMPONENTTYPE;
            break;
        case D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_REGISTERMASK:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_REGISTERMASK;
            break;
        case D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_SYSTEMVALUE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_SYSTEMVALUE;
            break;
        case D3D_MESSAGE_ID_DEVICE_SHADER_LINKAGE_NEVERWRITTEN_ALWAYSREADS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_SHADER_LINKAGE_NEVERWRITTEN_ALWAYSREADS;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_NOT_SET:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_NOT_SET;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_INPUTLAYOUT_NOT_SET:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_INPUTLAYOUT_NOT_SET;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_CONSTANT_BUFFER_NOT_SET:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_CONSTANT_BUFFER_NOT_SET;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_CONSTANT_BUFFER_TOO_SMALL:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_CONSTANT_BUFFER_TOO_SMALL;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_SAMPLER_NOT_SET:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_SAMPLER_NOT_SET;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_SHADERRESOURCEVIEW_NOT_SET:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_SHADERRESOURCEVIEW_NOT_SET;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_VIEW_DIMENSION_MISMATCH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_VIEW_DIMENSION_MISMATCH;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_STRIDE_TOO_SMALL:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_STRIDE_TOO_SMALL;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_TOO_SMALL:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_TOO_SMALL;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_NOT_SET:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_NOT_SET;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_FORMAT_INVALID:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_FORMAT_INVALID;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_TOO_SMALL:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_TOO_SMALL;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_GS_INPUT_PRIMITIVE_MISMATCH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_GS_INPUT_PRIMITIVE_MISMATCH;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_RETURN_TYPE_MISMATCH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_RETURN_TYPE_MISMATCH;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_POSITION_NOT_PRESENT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_POSITION_NOT_PRESENT;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_OUTPUT_STREAM_NOT_SET:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_OUTPUT_STREAM_NOT_SET;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_BOUND_RESOURCE_MAPPED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_BOUND_RESOURCE_MAPPED;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_INVALID_PRIMITIVETOPOLOGY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_INVALID_PRIMITIVETOPOLOGY;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_OFFSET_UNALIGNED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_OFFSET_UNALIGNED;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_VERTEX_STRIDE_UNALIGNED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_VERTEX_STRIDE_UNALIGNED;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_INDEX_OFFSET_UNALIGNED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_INDEX_OFFSET_UNALIGNED;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_OUTPUT_STREAM_OFFSET_UNALIGNED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_OUTPUT_STREAM_OFFSET_UNALIGNED;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_LD_UNSUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_LD_UNSUPPORTED;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_SAMPLE_UNSUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_SAMPLE_UNSUPPORTED;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_SAMPLE_C_UNSUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_SAMPLE_C_UNSUPPORTED;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_MULTISAMPLE_UNSUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_MULTISAMPLE_UNSUPPORTED;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_SO_TARGETS_BOUND_WITHOUT_SOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_SO_TARGETS_BOUND_WITHOUT_SOURCE;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_SO_STRIDE_LARGER_THAN_BUFFER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_SO_STRIDE_LARGER_THAN_BUFFER;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_OM_RENDER_TARGET_DOES_NOT_SUPPORT_BLENDING:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_OM_RENDER_TARGET_DOES_NOT_SUPPORT_BLENDING;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_OM_DUAL_SOURCE_BLENDING_CAN_ONLY_HAVE_RENDER_TARGET_0:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_OM_DUAL_SOURCE_BLENDING_CAN_ONLY_HAVE_RENDER_TARGET_0;
            break;
        case D3D_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_AT_FAULT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_AT_FAULT;
            break;
        case D3D_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_POSSIBLY_AT_FAULT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_POSSIBLY_AT_FAULT;
            break;
        case D3D_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_NOT_AT_FAULT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_REMOVAL_PROCESS_NOT_AT_FAULT;
            break;
        case D3D_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_INVALIDARG_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_INVALIDARG_RETURN;
            break;
        case D3D_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_OUTOFMEMORY_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_OUTOFMEMORY_RETURN;
            break;
        case D3D_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_BADINTERFACE_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_OPEN_SHARED_RESOURCE_BADINTERFACE_RETURN;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_VIEWPORT_NOT_SET:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_VIEWPORT_NOT_SET;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_TRAILING_DIGIT_IN_SEMANTIC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_TRAILING_DIGIT_IN_SEMANTIC;
            break;
        case D3D_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_TRAILING_DIGIT_IN_SEMANTIC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_TRAILING_DIGIT_IN_SEMANTIC;
            break;
        case D3D_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_DENORMFLUSH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_DENORMFLUSH;
            break;
        case D3D_MESSAGE_ID_OMSETRENDERTARGETS_INVALIDVIEW:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_OMSETRENDERTARGETS_INVALIDVIEW;
            break;
        case D3D_MESSAGE_ID_DEVICE_SETTEXTFILTERSIZE_INVALIDDIMENSIONS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_SETTEXTFILTERSIZE_INVALIDDIMENSIONS;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_SAMPLER_MISMATCH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_SAMPLER_MISMATCH;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_TYPE_MISMATCH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_TYPE_MISMATCH;
            break;
        case D3D_MESSAGE_ID_BLENDSTATE_GETDESC_LEGACY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_BLENDSTATE_GETDESC_LEGACY;
            break;
        case D3D_MESSAGE_ID_SHADERRESOURCEVIEW_GETDESC_LEGACY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SHADERRESOURCEVIEW_GETDESC_LEGACY;
            break;
        case D3D_MESSAGE_ID_CREATEQUERY_OUTOFMEMORY_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEQUERY_OUTOFMEMORY_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATEPREDICATE_OUTOFMEMORY_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEPREDICATE_OUTOFMEMORY_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATECOUNTER_OUTOFRANGE_COUNTER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATECOUNTER_OUTOFRANGE_COUNTER;
            break;
        case D3D_MESSAGE_ID_CREATECOUNTER_SIMULTANEOUS_ACTIVE_COUNTERS_EXHAUSTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATECOUNTER_SIMULTANEOUS_ACTIVE_COUNTERS_EXHAUSTED;
            break;
        case D3D_MESSAGE_ID_CREATECOUNTER_UNSUPPORTED_WELLKNOWN_COUNTER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATECOUNTER_UNSUPPORTED_WELLKNOWN_COUNTER;
            break;
        case D3D_MESSAGE_ID_CREATECOUNTER_OUTOFMEMORY_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATECOUNTER_OUTOFMEMORY_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATECOUNTER_NONEXCLUSIVE_RETURN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATECOUNTER_NONEXCLUSIVE_RETURN;
            break;
        case D3D_MESSAGE_ID_CREATECOUNTER_NULLDESC:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATECOUNTER_NULLDESC;
            break;
        case D3D_MESSAGE_ID_CHECKCOUNTER_OUTOFRANGE_COUNTER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CHECKCOUNTER_OUTOFRANGE_COUNTER;
            break;
        case D3D_MESSAGE_ID_CHECKCOUNTER_UNSUPPORTED_WELLKNOWN_COUNTER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CHECKCOUNTER_UNSUPPORTED_WELLKNOWN_COUNTER;
            break;
        case D3D_MESSAGE_ID_SETPREDICATION_INVALID_PREDICATE_STATE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SETPREDICATION_INVALID_PREDICATE_STATE;
            break;
        case D3D_MESSAGE_ID_QUERY_BEGIN_UNSUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_QUERY_BEGIN_UNSUPPORTED;
            break;
        case D3D_MESSAGE_ID_PREDICATE_BEGIN_DURING_PREDICATION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_PREDICATE_BEGIN_DURING_PREDICATION;
            break;
        case D3D_MESSAGE_ID_QUERY_BEGIN_DUPLICATE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_QUERY_BEGIN_DUPLICATE;
            break;
        case D3D_MESSAGE_ID_QUERY_BEGIN_ABANDONING_PREVIOUS_RESULTS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_QUERY_BEGIN_ABANDONING_PREVIOUS_RESULTS;
            break;
        case D3D_MESSAGE_ID_PREDICATE_END_DURING_PREDICATION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_PREDICATE_END_DURING_PREDICATION;
            break;
        case D3D_MESSAGE_ID_QUERY_END_ABANDONING_PREVIOUS_RESULTS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_QUERY_END_ABANDONING_PREVIOUS_RESULTS;
            break;
        case D3D_MESSAGE_ID_QUERY_END_WITHOUT_BEGIN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_QUERY_END_WITHOUT_BEGIN;
            break;
        case D3D_MESSAGE_ID_QUERY_GETDATA_INVALID_DATASIZE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_QUERY_GETDATA_INVALID_DATASIZE;
            break;
        case D3D_MESSAGE_ID_QUERY_GETDATA_INVALID_FLAGS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_QUERY_GETDATA_INVALID_FLAGS;
            break;
        case D3D_MESSAGE_ID_QUERY_GETDATA_INVALID_CALL:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_QUERY_GETDATA_INVALID_CALL;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_PS_OUTPUT_TYPE_MISMATCH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_PS_OUTPUT_TYPE_MISMATCH;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_GATHER_UNSUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_GATHER_UNSUPPORTED;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_INVALID_USE_OF_CENTER_MULTISAMPLE_PATTERN:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_INVALID_USE_OF_CENTER_MULTISAMPLE_PATTERN;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_STRIDE_TOO_LARGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_STRIDE_TOO_LARGE;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_INVALIDRANGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_INVALIDRANGE;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_EMPTY_LAYOUT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_EMPTY_LAYOUT;
            break;
        case D3D_MESSAGE_ID_DEVICE_DRAW_RESOURCE_SAMPLE_COUNT_MISMATCH:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_SAMPLE_COUNT_MISMATCH;
            break;
        case D3D_MESSAGE_ID_D3D_MESSAGES_END:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_D3D10_MESSAGES_END;
            break;
        case D3D_MESSAGE_ID_D3D_L9_MESSAGES_START:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_D3D10L9_MESSAGES_START;
            break;
        case D3D_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_STENCIL_NO_TWO_SIDED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEDEPTHSTENCILSTATE_STENCIL_NO_TWO_SIDED;
            break;
        case D3D_MESSAGE_ID_CREATERASTERIZERSTATE_DepthBiasClamp_NOT_SUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_DepthBiasClamp_NOT_SUPPORTED;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_NO_COMPARISON_SUPPORT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_NO_COMPARISON_SUPPORT;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_EXCESSIVE_ANISOTROPY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_EXCESSIVE_ANISOTROPY;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_BORDER_OUT_OF_RANGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_BORDER_OUT_OF_RANGE;
            break;
        case D3D_MESSAGE_ID_VSSETSAMPLERS_NOT_SUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_VSSETSAMPLERS_NOT_SUPPORTED;
            break;
        case D3D_MESSAGE_ID_VSSETSAMPLERS_TOO_MANY_SAMPLERS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_VSSETSAMPLERS_TOO_MANY_SAMPLERS;
            break;
        case D3D_MESSAGE_ID_PSSETSAMPLERS_TOO_MANY_SAMPLERS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_PSSETSAMPLERS_TOO_MANY_SAMPLERS;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_NO_ARRAYS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_NO_ARRAYS;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_NO_VB_AND_IB_BIND:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_NO_VB_AND_IB_BIND;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_NO_TEXTURE_1D:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_NO_TEXTURE_1D;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_DIMENSION_OUT_OF_RANGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_DIMENSION_OUT_OF_RANGE;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_NOT_BINDABLE_AS_SHADER_RESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_NOT_BINDABLE_AS_SHADER_RESOURCE;
            break;
        case D3D_MESSAGE_ID_OMSETRENDERTARGETS_TOO_MANY_RENDER_TARGETS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_OMSETRENDERTARGETS_TOO_MANY_RENDER_TARGETS;
            break;
        case D3D_MESSAGE_ID_OMSETRENDERTARGETS_NO_DIFFERING_BIT_DEPTHS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_OMSETRENDERTARGETS_NO_DIFFERING_BIT_DEPTHS;
            break;
        case D3D_MESSAGE_ID_IASETVERTEXBUFFERS_BAD_BUFFER_INDEX:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_IASETVERTEXBUFFERS_BAD_BUFFER_INDEX;
            break;
        case D3D_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_TOO_MANY_VIEWPORTS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RSSETVIEWPORTS_TOO_MANY_VIEWPORTS;
            break;
        case D3D_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_ADJACENCY_UNSUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_IASETPRIMITIVETOPOLOGY_ADJACENCY_UNSUPPORTED;
            break;
        case D3D_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_TOO_MANY_SCISSORS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_TOO_MANY_SCISSORS;
            break;
        case D3D_MESSAGE_ID_COPYRESOURCE_ONLY_TEXTURE_2D_WITHIN_GPU_MEMORY:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYRESOURCE_ONLY_TEXTURE_2D_WITHIN_GPU_MEMORY;
            break;
        case D3D_MESSAGE_ID_COPYRESOURCE_NO_TEXTURE_3D_READBACK:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYRESOURCE_NO_TEXTURE_3D_READBACK;
            break;
        case D3D_MESSAGE_ID_COPYRESOURCE_NO_TEXTURE_ONLY_READBACK:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_COPYRESOURCE_NO_TEXTURE_ONLY_READBACK;
            break;
        case D3D_MESSAGE_ID_CREATEINPUTLAYOUT_UNSUPPORTED_FORMAT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEINPUTLAYOUT_UNSUPPORTED_FORMAT;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_NO_ALPHA_TO_COVERAGE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_NO_ALPHA_TO_COVERAGE;
            break;
        case D3D_MESSAGE_ID_CREATERASTERIZERSTATE_DepthClipEnable_MUST_BE_TRUE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERASTERIZERSTATE_DepthClipEnable_MUST_BE_TRUE;
            break;
        case D3D_MESSAGE_ID_DRAWINDEXED_STARTINDEXLOCATION_MUST_BE_POSITIVE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DRAWINDEXED_STARTINDEXLOCATION_MUST_BE_POSITIVE;
            break;
        case D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_MUST_USE_LOWEST_LOD:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_MUST_USE_LOWEST_LOD;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_MINLOD_MUST_NOT_BE_FRACTIONAL:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_MINLOD_MUST_NOT_BE_FRACTIONAL;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_MAXLOD_MUST_BE_FLT_MAX:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_MAXLOD_MUST_BE_FLT_MAX;
            break;
        case D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_FIRSTARRAYSLICE_MUST_BE_ZERO:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_FIRSTARRAYSLICE_MUST_BE_ZERO;
            break;
        case D3D_MESSAGE_ID_CREATESHADERRESOURCEVIEW_CUBES_MUST_HAVE_6_SIDES:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESHADERRESOURCEVIEW_CUBES_MUST_HAVE_6_SIDES;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_NOT_BINDABLE_AS_RENDER_TARGET:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_NOT_BINDABLE_AS_RENDER_TARGET;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_NO_DWORD_INDEX_BUFFER:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_NO_DWORD_INDEX_BUFFER;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_MSAA_PRECLUDES_SHADER_RESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_MSAA_PRECLUDES_SHADER_RESOURCE;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_PRESENTATION_PRECLUDES_SHADER_RESOURCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_PRESENTATION_PRECLUDES_SHADER_RESOURCE;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_NO_INDEPENDENT_BLEND_ENABLE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_NO_INDEPENDENT_BLEND_ENABLE;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_NO_INDEPENDENT_WRITE_MASKS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_NO_INDEPENDENT_WRITE_MASKS;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_NO_STREAM_OUT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_NO_STREAM_OUT;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_ONLY_VB_IB_FOR_BUFFERS:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_ONLY_VB_IB_FOR_BUFFERS;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_NO_AUTOGEN_FOR_VOLUMES:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_NO_AUTOGEN_FOR_VOLUMES;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_DXGI_FORMAT_R8G8B8A8_CANNOT_BE_SHARED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_DXGI_FORMAT_R8G8B8A8_CANNOT_BE_SHARED;
            break;
        case D3D_MESSAGE_ID_VSSHADERRESOURCES_NOT_SUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_VSSHADERRESOURCES_NOT_SUPPORTED;
            break;
        case D3D_MESSAGE_ID_GEOMETRY_SHADER_NOT_SUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_GEOMETRY_SHADER_NOT_SUPPORTED;
            break;
        case D3D_MESSAGE_ID_STREAM_OUT_NOT_SUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_STREAM_OUT_NOT_SUPPORTED;
            break;
        case D3D_MESSAGE_ID_TEXT_FILTER_NOT_SUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_TEXT_FILTER_NOT_SUPPORTED;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_NO_SEPARATE_ALPHA_BLEND:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_NO_SEPARATE_ALPHA_BLEND;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_NO_MRT_BLEND:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_NO_MRT_BLEND;
            break;
        case D3D_MESSAGE_ID_CREATEBLENDSTATE_OPERATION_NOT_SUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATEBLENDSTATE_OPERATION_NOT_SUPPORTED;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_NO_MIRRORONCE:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_NO_MIRRORONCE;
            break;
        case D3D_MESSAGE_ID_DRAWINSTANCED_NOT_SUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DRAWINSTANCED_NOT_SUPPORTED;
            break;
        case D3D_MESSAGE_ID_DRAWINDEXEDINSTANCED_NOT_SUPPORTED_BELOW_9_3:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DRAWINDEXEDINSTANCED_NOT_SUPPORTED_BELOW_9_3;
            break;
        case D3D_MESSAGE_ID_DRAWINDEXED_POINTLIST_UNSUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DRAWINDEXED_POINTLIST_UNSUPPORTED;
            break;
        case D3D_MESSAGE_ID_SETBLENDSTATE_SAMPLE_MASK_CANNOT_BE_ZERO:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SETBLENDSTATE_SAMPLE_MASK_CANNOT_BE_ZERO;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_DIMENSION_EXCEEDS_FEATURE_LEVEL_DEFINITION:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_DIMENSION_EXCEEDS_FEATURE_LEVEL_DEFINITION;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_ONLY_SINGLE_MIP_LEVEL_DEPTH_STENCIL_SUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_ONLY_SINGLE_MIP_LEVEL_DEPTH_STENCIL_SUPPORTED;
            break;
        case D3D_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_NEGATIVESCISSOR:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_DEVICE_RSSETSCISSORRECTS_NEGATIVESCISSOR;
            break;
        case D3D_MESSAGE_ID_SLOT_ZERO_MUST_BE_D3D_INPUT_PER_VERTEX_DATA:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_SLOT_ZERO_MUST_BE_D3D10_INPUT_PER_VERTEX_DATA;
            break;
        case D3D_MESSAGE_ID_CREATERESOURCE_NON_POW_2_MIPMAP:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATERESOURCE_NON_POW_2_MIPMAP;
            break;
        case D3D_MESSAGE_ID_CREATESAMPLERSTATE_BORDER_NOT_SUPPORTED:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_CREATESAMPLERSTATE_BORDER_NOT_SUPPORTED;
            break;
        case D3D_MESSAGE_ID_OMSETRENDERTARGETS_NO_SRGB_MRT:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_OMSETRENDERTARGETS_NO_SRGB_MRT;
            break;
        case D3D_MESSAGE_ID_D3D_L9_MESSAGES_END:
            *d3d_message_id_10 = D3D10_MESSAGE_ID_D3D10L9_MESSAGES_END;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_DRIVER_TYPE *d3d_driver_type, D3D10_DRIVER_TYPE d3d_driver_type_10 )
{
    switch( d3d_driver_type_10 )
    {
        default:
            *d3d_driver_type = (D3D_DRIVER_TYPE)d3d_driver_type_10;
            break;
        case D3D10_DRIVER_TYPE_HARDWARE:
            *d3d_driver_type = D3D_DRIVER_TYPE_HARDWARE;
            break;
        case D3D10_DRIVER_TYPE_REFERENCE:
            *d3d_driver_type = D3D_DRIVER_TYPE_REFERENCE;
            break;
        case D3D10_DRIVER_TYPE_NULL:
            *d3d_driver_type = D3D_DRIVER_TYPE_NULL;
            break;
        case D3D10_DRIVER_TYPE_SOFTWARE:
            *d3d_driver_type = D3D_DRIVER_TYPE_SOFTWARE;
            break;
        case D3D10_DRIVER_TYPE_WARP:
            *d3d_driver_type = D3D_DRIVER_TYPE_WARP;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_DRIVER_TYPE *d3d_driver_type_10, D3D_DRIVER_TYPE d3d_driver_type )
{
    switch( d3d_driver_type )
    {
        default:
            *d3d_driver_type_10 = (D3D10_DRIVER_TYPE)d3d_driver_type;
            break;
        case D3D_DRIVER_TYPE_HARDWARE:
            *d3d_driver_type_10 = D3D10_DRIVER_TYPE_HARDWARE;
            break;
        case D3D_DRIVER_TYPE_REFERENCE:
            *d3d_driver_type_10 = D3D10_DRIVER_TYPE_REFERENCE;
            break;
        case D3D_DRIVER_TYPE_NULL:
            *d3d_driver_type_10 = D3D10_DRIVER_TYPE_NULL;
            break;
        case D3D_DRIVER_TYPE_SOFTWARE:
            *d3d_driver_type_10 = D3D10_DRIVER_TYPE_SOFTWARE;
            break;
        case D3D_DRIVER_TYPE_WARP:
            *d3d_driver_type_10 = D3D10_DRIVER_TYPE_WARP;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_FEATURE_LEVEL1 *d3d_feature_level1, D3D10_FEATURE_LEVEL1 d3d_feature_level1_10 )
{
    switch( d3d_feature_level1_10 )
    {
        default:
            *d3d_feature_level1 = (D3D_FEATURE_LEVEL1)d3d_feature_level1_10;
            break;
        case D3D10_FEATURE_LEVEL_10_0:
            *d3d_feature_level1 = D3D_FEATURE_LEVEL_10_0;
            break;
        case D3D10_FEATURE_LEVEL_10_1:
            *d3d_feature_level1 = D3D_FEATURE_LEVEL_10_1;
            break;
        case D3D10_FEATURE_LEVEL_9_1:
            *d3d_feature_level1 = D3D_FEATURE_LEVEL_9_1;
            break;
        case D3D10_FEATURE_LEVEL_9_2:
            *d3d_feature_level1 = D3D_FEATURE_LEVEL_9_2;
            break;
        case D3D10_FEATURE_LEVEL_9_3:
            *d3d_feature_level1 = D3D_FEATURE_LEVEL_9_3;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_FEATURE_LEVEL1 *d3d_feature_level1_10, D3D_FEATURE_LEVEL1 d3d_feature_level1 )
{
    switch( d3d_feature_level1 )
    {
        default:
            *d3d_feature_level1_10 = (D3D10_FEATURE_LEVEL1)d3d_feature_level1;
            break;
        case D3D_FEATURE_LEVEL_10_0:
            *d3d_feature_level1_10 = D3D10_FEATURE_LEVEL_10_0;
            break;
        case D3D_FEATURE_LEVEL_10_1:
            *d3d_feature_level1_10 = D3D10_FEATURE_LEVEL_10_1;
            break;
        case D3D_FEATURE_LEVEL_9_1:
            *d3d_feature_level1_10 = D3D10_FEATURE_LEVEL_9_1;
            break;
        case D3D_FEATURE_LEVEL_9_2:
            *d3d_feature_level1_10 = D3D10_FEATURE_LEVEL_9_2;
            break;
        case D3D_FEATURE_LEVEL_9_3:
            *d3d_feature_level1_10 = D3D10_FEATURE_LEVEL_9_3;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_STANDARD_MULTISAMPLE_QUALITY_LEVELS *d3d_standard_multisample_quality_levels, D3D10_STANDARD_MULTISAMPLE_QUALITY_LEVELS d3d_standard_multisample_quality_levels_10 )
{
    switch( d3d_standard_multisample_quality_levels_10 )
    {
        default:
            *d3d_standard_multisample_quality_levels = (D3D_STANDARD_MULTISAMPLE_QUALITY_LEVELS)d3d_standard_multisample_quality_levels_10;
            break;
        case D3D10_STANDARD_MULTISAMPLE_PATTERN:
            *d3d_standard_multisample_quality_levels = D3D_STANDARD_MULTISAMPLE_PATTERN;
            break;
        case D3D10_CENTER_MULTISAMPLE_PATTERN:
            *d3d_standard_multisample_quality_levels = D3D_CENTER_MULTISAMPLE_PATTERN;
            break;
    }
}

void D3DWrapperConvertEnum( D3D10_STANDARD_MULTISAMPLE_QUALITY_LEVELS *d3d_standard_multisample_quality_levels_10, D3D_STANDARD_MULTISAMPLE_QUALITY_LEVELS d3d_standard_multisample_quality_levels )
{
    switch( d3d_standard_multisample_quality_levels )
    {
        default:
            *d3d_standard_multisample_quality_levels_10 = (D3D10_STANDARD_MULTISAMPLE_QUALITY_LEVELS)d3d_standard_multisample_quality_levels;
            break;
        case D3D_STANDARD_MULTISAMPLE_PATTERN:
            *d3d_standard_multisample_quality_levels_10 = D3D10_STANDARD_MULTISAMPLE_PATTERN;
            break;
        case D3D_CENTER_MULTISAMPLE_PATTERN:
            *d3d_standard_multisample_quality_levels_10 = D3D10_CENTER_MULTISAMPLE_PATTERN;
            break;
    }
}

//
// Struct Conversions
//

void D3DWrapperConvertStruct( D3D_INPUT_ELEMENT_DESC *d3d_input_element_desc, D3D10_INPUT_ELEMENT_DESC const *d3d_input_element_desc_10 )
{
    d3d_input_element_desc->SemanticName = d3d_input_element_desc_10->SemanticName;
    d3d_input_element_desc->SemanticIndex = d3d_input_element_desc_10->SemanticIndex;
    d3d_input_element_desc->Format = d3d_input_element_desc_10->Format;
    d3d_input_element_desc->InputSlot = d3d_input_element_desc_10->InputSlot;
    d3d_input_element_desc->AlignedByteOffset = d3d_input_element_desc_10->AlignedByteOffset;
    D3DWrapperConvertEnum( &d3d_input_element_desc->InputSlotClass, d3d_input_element_desc_10->InputSlotClass );
    d3d_input_element_desc->InstanceDataStepRate = d3d_input_element_desc_10->InstanceDataStepRate;
}

void D3DWrapperConvertStruct( D3D10_INPUT_ELEMENT_DESC *d3d_input_element_desc_10, D3D_INPUT_ELEMENT_DESC const *d3d_input_element_desc )
{
    d3d_input_element_desc_10->SemanticName = d3d_input_element_desc->SemanticName;
    d3d_input_element_desc_10->SemanticIndex = d3d_input_element_desc->SemanticIndex;
    d3d_input_element_desc_10->Format = d3d_input_element_desc->Format;
    d3d_input_element_desc_10->InputSlot = d3d_input_element_desc->InputSlot;
    d3d_input_element_desc_10->AlignedByteOffset = d3d_input_element_desc->AlignedByteOffset;
    D3DWrapperConvertEnum( &d3d_input_element_desc_10->InputSlotClass, d3d_input_element_desc->InputSlotClass );
    d3d_input_element_desc_10->InstanceDataStepRate = d3d_input_element_desc->InstanceDataStepRate;
}

void D3DWrapperConvertStruct( D3D_SO_DECLARATION_ENTRY *d3d_so_declaration_entry, D3D10_SO_DECLARATION_ENTRY const *d3d_so_declaration_entry_10 )
{
    d3d_so_declaration_entry->SemanticName = d3d_so_declaration_entry_10->SemanticName;
    d3d_so_declaration_entry->SemanticIndex = d3d_so_declaration_entry_10->SemanticIndex;
    d3d_so_declaration_entry->StartComponent = d3d_so_declaration_entry_10->StartComponent;
    d3d_so_declaration_entry->ComponentCount = d3d_so_declaration_entry_10->ComponentCount;
    d3d_so_declaration_entry->OutputSlot = d3d_so_declaration_entry_10->OutputSlot;
}

void D3DWrapperConvertStruct( D3D10_SO_DECLARATION_ENTRY *d3d_so_declaration_entry_10, D3D_SO_DECLARATION_ENTRY const *d3d_so_declaration_entry )
{
    d3d_so_declaration_entry_10->SemanticName = d3d_so_declaration_entry->SemanticName;
    d3d_so_declaration_entry_10->SemanticIndex = d3d_so_declaration_entry->SemanticIndex;
    d3d_so_declaration_entry_10->StartComponent = d3d_so_declaration_entry->StartComponent;
    d3d_so_declaration_entry_10->ComponentCount = d3d_so_declaration_entry->ComponentCount;
    d3d_so_declaration_entry_10->OutputSlot = d3d_so_declaration_entry->OutputSlot;
}

void D3DWrapperConvertStruct( D3D_VIEWPORT *d3d_viewport, D3D10_VIEWPORT const *d3d_viewport_10 )
{
    d3d_viewport->TopLeftX = d3d_viewport_10->TopLeftX;
    d3d_viewport->TopLeftY = d3d_viewport_10->TopLeftY;
    d3d_viewport->Width = d3d_viewport_10->Width;
    d3d_viewport->Height = d3d_viewport_10->Height;
    d3d_viewport->MinDepth = d3d_viewport_10->MinDepth;
    d3d_viewport->MaxDepth = d3d_viewport_10->MaxDepth;
}

void D3DWrapperConvertStruct( D3D10_VIEWPORT *d3d_viewport_10, D3D_VIEWPORT const *d3d_viewport )
{
    d3d_viewport_10->TopLeftX = d3d_viewport->TopLeftX;
    d3d_viewport_10->TopLeftY = d3d_viewport->TopLeftY;
    d3d_viewport_10->Width = d3d_viewport->Width;
    d3d_viewport_10->Height = d3d_viewport->Height;
    d3d_viewport_10->MinDepth = d3d_viewport->MinDepth;
    d3d_viewport_10->MaxDepth = d3d_viewport->MaxDepth;
}

void D3DWrapperConvertStruct( D3D_BOX *d3d_box, D3D10_BOX const *d3d_box_10 )
{
    d3d_box->left = d3d_box_10->left;
    d3d_box->top = d3d_box_10->top;
    d3d_box->front = d3d_box_10->front;
    d3d_box->right = d3d_box_10->right;
    d3d_box->bottom = d3d_box_10->bottom;
    d3d_box->back = d3d_box_10->back;
}

void D3DWrapperConvertStruct( D3D10_BOX *d3d_box_10, D3D_BOX const *d3d_box )
{
    d3d_box_10->left = d3d_box->left;
    d3d_box_10->top = d3d_box->top;
    d3d_box_10->front = d3d_box->front;
    d3d_box_10->right = d3d_box->right;
    d3d_box_10->bottom = d3d_box->bottom;
    d3d_box_10->back = d3d_box->back;
}

void D3DWrapperConvertStruct( D3D_DEPTH_STENCILOP_DESC *d3d_depth_stencilop_desc, D3D10_DEPTH_STENCILOP_DESC const *d3d_depth_stencilop_desc_10 )
{
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc->StencilFailOp, d3d_depth_stencilop_desc_10->StencilFailOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc->StencilDepthFailOp, d3d_depth_stencilop_desc_10->StencilDepthFailOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc->StencilPassOp, d3d_depth_stencilop_desc_10->StencilPassOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc->StencilFunc, d3d_depth_stencilop_desc_10->StencilFunc );
}

void D3DWrapperConvertStruct( D3D10_DEPTH_STENCILOP_DESC *d3d_depth_stencilop_desc_10, D3D_DEPTH_STENCILOP_DESC const *d3d_depth_stencilop_desc )
{
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc_10->StencilFailOp, d3d_depth_stencilop_desc->StencilFailOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc_10->StencilDepthFailOp, d3d_depth_stencilop_desc->StencilDepthFailOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc_10->StencilPassOp, d3d_depth_stencilop_desc->StencilPassOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc_10->StencilFunc, d3d_depth_stencilop_desc->StencilFunc );
}

void D3DWrapperConvertStruct( D3D_DEPTH_STENCIL_DESC *d3d_depth_stencil_desc, D3D10_DEPTH_STENCIL_DESC const *d3d_depth_stencil_desc_10 )
{
    d3d_depth_stencil_desc->DepthEnable = d3d_depth_stencil_desc_10->DepthEnable;
    D3DWrapperConvertEnum( &d3d_depth_stencil_desc->DepthWriteMask, d3d_depth_stencil_desc_10->DepthWriteMask );
    D3DWrapperConvertEnum( &d3d_depth_stencil_desc->DepthFunc, d3d_depth_stencil_desc_10->DepthFunc );
    d3d_depth_stencil_desc->StencilEnable = d3d_depth_stencil_desc_10->StencilEnable;
    d3d_depth_stencil_desc->StencilReadMask = d3d_depth_stencil_desc_10->StencilReadMask;
    d3d_depth_stencil_desc->StencilWriteMask = d3d_depth_stencil_desc_10->StencilWriteMask;
    D3DWrapperConvertStruct( &d3d_depth_stencil_desc->FrontFace, &d3d_depth_stencil_desc_10->FrontFace );
    D3DWrapperConvertStruct( &d3d_depth_stencil_desc->BackFace, &d3d_depth_stencil_desc_10->BackFace );
}

void D3DWrapperConvertStruct( D3D10_DEPTH_STENCIL_DESC *d3d_depth_stencil_desc_10, D3D_DEPTH_STENCIL_DESC const *d3d_depth_stencil_desc )
{
    d3d_depth_stencil_desc_10->DepthEnable = d3d_depth_stencil_desc->DepthEnable;
    D3DWrapperConvertEnum( &d3d_depth_stencil_desc_10->DepthWriteMask, d3d_depth_stencil_desc->DepthWriteMask );
    D3DWrapperConvertEnum( &d3d_depth_stencil_desc_10->DepthFunc, d3d_depth_stencil_desc->DepthFunc );
    d3d_depth_stencil_desc_10->StencilEnable = d3d_depth_stencil_desc->StencilEnable;
    d3d_depth_stencil_desc_10->StencilReadMask = d3d_depth_stencil_desc->StencilReadMask;
    d3d_depth_stencil_desc_10->StencilWriteMask = d3d_depth_stencil_desc->StencilWriteMask;
    D3DWrapperConvertStruct( &d3d_depth_stencil_desc_10->FrontFace, &d3d_depth_stencil_desc->FrontFace );
    D3DWrapperConvertStruct( &d3d_depth_stencil_desc_10->BackFace, &d3d_depth_stencil_desc->BackFace );
}

void D3DWrapperConvertStruct( D3D_BLEND_DESC *d3d_blend_desc, D3D10_BLEND_DESC const *d3d_blend_desc_10 )
{
    d3d_blend_desc->AlphaToCoverageEnable = d3d_blend_desc_10->AlphaToCoverageEnable;
    d3d_blend_desc->BlendEnable[7] = d3d_blend_desc_10->BlendEnable[7];
    d3d_blend_desc->BlendEnable[6] = d3d_blend_desc_10->BlendEnable[6];
    d3d_blend_desc->BlendEnable[5] = d3d_blend_desc_10->BlendEnable[5];
    d3d_blend_desc->BlendEnable[4] = d3d_blend_desc_10->BlendEnable[4];
    d3d_blend_desc->BlendEnable[3] = d3d_blend_desc_10->BlendEnable[3];
    d3d_blend_desc->BlendEnable[2] = d3d_blend_desc_10->BlendEnable[2];
    d3d_blend_desc->BlendEnable[1] = d3d_blend_desc_10->BlendEnable[1];
    d3d_blend_desc->BlendEnable[0] = d3d_blend_desc_10->BlendEnable[0];
    D3DWrapperConvertEnum( &d3d_blend_desc->SrcBlend, d3d_blend_desc_10->SrcBlend );
    D3DWrapperConvertEnum( &d3d_blend_desc->DestBlend, d3d_blend_desc_10->DestBlend );
    D3DWrapperConvertEnum( &d3d_blend_desc->BlendOp, d3d_blend_desc_10->BlendOp );
    D3DWrapperConvertEnum( &d3d_blend_desc->SrcBlendAlpha, d3d_blend_desc_10->SrcBlendAlpha );
    D3DWrapperConvertEnum( &d3d_blend_desc->DestBlendAlpha, d3d_blend_desc_10->DestBlendAlpha );
    D3DWrapperConvertEnum( &d3d_blend_desc->BlendOpAlpha, d3d_blend_desc_10->BlendOpAlpha );
    D3DFlagsConvertFrom10_COLOR_WRITE_ENABLE( &d3d_blend_desc->RenderTargetWriteMask[7], d3d_blend_desc_10->RenderTargetWriteMask[7] );
    D3DFlagsConvertFrom10_COLOR_WRITE_ENABLE( &d3d_blend_desc->RenderTargetWriteMask[6], d3d_blend_desc_10->RenderTargetWriteMask[6] );
    D3DFlagsConvertFrom10_COLOR_WRITE_ENABLE( &d3d_blend_desc->RenderTargetWriteMask[5], d3d_blend_desc_10->RenderTargetWriteMask[5] );
    D3DFlagsConvertFrom10_COLOR_WRITE_ENABLE( &d3d_blend_desc->RenderTargetWriteMask[4], d3d_blend_desc_10->RenderTargetWriteMask[4] );
    D3DFlagsConvertFrom10_COLOR_WRITE_ENABLE( &d3d_blend_desc->RenderTargetWriteMask[3], d3d_blend_desc_10->RenderTargetWriteMask[3] );
    D3DFlagsConvertFrom10_COLOR_WRITE_ENABLE( &d3d_blend_desc->RenderTargetWriteMask[2], d3d_blend_desc_10->RenderTargetWriteMask[2] );
    D3DFlagsConvertFrom10_COLOR_WRITE_ENABLE( &d3d_blend_desc->RenderTargetWriteMask[1], d3d_blend_desc_10->RenderTargetWriteMask[1] );
    D3DFlagsConvertFrom10_COLOR_WRITE_ENABLE( &d3d_blend_desc->RenderTargetWriteMask[0], d3d_blend_desc_10->RenderTargetWriteMask[0] );
}

void D3DWrapperConvertStruct( D3D10_BLEND_DESC *d3d_blend_desc_10, D3D_BLEND_DESC const *d3d_blend_desc )
{
    d3d_blend_desc_10->AlphaToCoverageEnable = d3d_blend_desc->AlphaToCoverageEnable;
    d3d_blend_desc_10->BlendEnable[7] = d3d_blend_desc->BlendEnable[7];
    d3d_blend_desc_10->BlendEnable[6] = d3d_blend_desc->BlendEnable[6];
    d3d_blend_desc_10->BlendEnable[5] = d3d_blend_desc->BlendEnable[5];
    d3d_blend_desc_10->BlendEnable[4] = d3d_blend_desc->BlendEnable[4];
    d3d_blend_desc_10->BlendEnable[3] = d3d_blend_desc->BlendEnable[3];
    d3d_blend_desc_10->BlendEnable[2] = d3d_blend_desc->BlendEnable[2];
    d3d_blend_desc_10->BlendEnable[1] = d3d_blend_desc->BlendEnable[1];
    d3d_blend_desc_10->BlendEnable[0] = d3d_blend_desc->BlendEnable[0];
    D3DWrapperConvertEnum( &d3d_blend_desc_10->SrcBlend, d3d_blend_desc->SrcBlend );
    D3DWrapperConvertEnum( &d3d_blend_desc_10->DestBlend, d3d_blend_desc->DestBlend );
    D3DWrapperConvertEnum( &d3d_blend_desc_10->BlendOp, d3d_blend_desc->BlendOp );
    D3DWrapperConvertEnum( &d3d_blend_desc_10->SrcBlendAlpha, d3d_blend_desc->SrcBlendAlpha );
    D3DWrapperConvertEnum( &d3d_blend_desc_10->DestBlendAlpha, d3d_blend_desc->DestBlendAlpha );
    D3DWrapperConvertEnum( &d3d_blend_desc_10->BlendOpAlpha, d3d_blend_desc->BlendOpAlpha );
    D3DFlagsConvertTo10_COLOR_WRITE_ENABLE( &d3d_blend_desc_10->RenderTargetWriteMask[7], d3d_blend_desc->RenderTargetWriteMask[7] );
    D3DFlagsConvertTo10_COLOR_WRITE_ENABLE( &d3d_blend_desc_10->RenderTargetWriteMask[6], d3d_blend_desc->RenderTargetWriteMask[6] );
    D3DFlagsConvertTo10_COLOR_WRITE_ENABLE( &d3d_blend_desc_10->RenderTargetWriteMask[5], d3d_blend_desc->RenderTargetWriteMask[5] );
    D3DFlagsConvertTo10_COLOR_WRITE_ENABLE( &d3d_blend_desc_10->RenderTargetWriteMask[4], d3d_blend_desc->RenderTargetWriteMask[4] );
    D3DFlagsConvertTo10_COLOR_WRITE_ENABLE( &d3d_blend_desc_10->RenderTargetWriteMask[3], d3d_blend_desc->RenderTargetWriteMask[3] );
    D3DFlagsConvertTo10_COLOR_WRITE_ENABLE( &d3d_blend_desc_10->RenderTargetWriteMask[2], d3d_blend_desc->RenderTargetWriteMask[2] );
    D3DFlagsConvertTo10_COLOR_WRITE_ENABLE( &d3d_blend_desc_10->RenderTargetWriteMask[1], d3d_blend_desc->RenderTargetWriteMask[1] );
    D3DFlagsConvertTo10_COLOR_WRITE_ENABLE( &d3d_blend_desc_10->RenderTargetWriteMask[0], d3d_blend_desc->RenderTargetWriteMask[0] );
}

void D3DWrapperConvertStruct( D3D_RASTERIZER_DESC *d3d_rasterizer_desc, D3D10_RASTERIZER_DESC const *d3d_rasterizer_desc_10 )
{
    D3DWrapperConvertEnum( &d3d_rasterizer_desc->FillMode, d3d_rasterizer_desc_10->FillMode );
    D3DWrapperConvertEnum( &d3d_rasterizer_desc->CullMode, d3d_rasterizer_desc_10->CullMode );
    d3d_rasterizer_desc->FrontCounterClockwise = d3d_rasterizer_desc_10->FrontCounterClockwise;
    d3d_rasterizer_desc->DepthBias = d3d_rasterizer_desc_10->DepthBias;
    d3d_rasterizer_desc->DepthBiasClamp = d3d_rasterizer_desc_10->DepthBiasClamp;
    d3d_rasterizer_desc->SlopeScaledDepthBias = d3d_rasterizer_desc_10->SlopeScaledDepthBias;
    d3d_rasterizer_desc->DepthClipEnable = d3d_rasterizer_desc_10->DepthClipEnable;
    d3d_rasterizer_desc->ScissorEnable = d3d_rasterizer_desc_10->ScissorEnable;
    d3d_rasterizer_desc->MultisampleEnable = d3d_rasterizer_desc_10->MultisampleEnable;
    d3d_rasterizer_desc->AntialiasedLineEnable = d3d_rasterizer_desc_10->AntialiasedLineEnable;
}

void D3DWrapperConvertStruct( D3D10_RASTERIZER_DESC *d3d_rasterizer_desc_10, D3D_RASTERIZER_DESC const *d3d_rasterizer_desc )
{
    D3DWrapperConvertEnum( &d3d_rasterizer_desc_10->FillMode, d3d_rasterizer_desc->FillMode );
    D3DWrapperConvertEnum( &d3d_rasterizer_desc_10->CullMode, d3d_rasterizer_desc->CullMode );
    d3d_rasterizer_desc_10->FrontCounterClockwise = d3d_rasterizer_desc->FrontCounterClockwise;
    d3d_rasterizer_desc_10->DepthBias = d3d_rasterizer_desc->DepthBias;
    d3d_rasterizer_desc_10->DepthBiasClamp = d3d_rasterizer_desc->DepthBiasClamp;
    d3d_rasterizer_desc_10->SlopeScaledDepthBias = d3d_rasterizer_desc->SlopeScaledDepthBias;
    d3d_rasterizer_desc_10->DepthClipEnable = d3d_rasterizer_desc->DepthClipEnable;
    d3d_rasterizer_desc_10->ScissorEnable = d3d_rasterizer_desc->ScissorEnable;
    d3d_rasterizer_desc_10->MultisampleEnable = d3d_rasterizer_desc->MultisampleEnable;
    d3d_rasterizer_desc_10->AntialiasedLineEnable = d3d_rasterizer_desc->AntialiasedLineEnable;
}

void D3DWrapperConvertStruct( D3D_SUBRESOURCE_DATA *d3d_subresource_data, D3D10_SUBRESOURCE_DATA const *d3d_subresource_data_10 )
{
    d3d_subresource_data->pSysMem = d3d_subresource_data_10->pSysMem;
    d3d_subresource_data->SysMemPitch = d3d_subresource_data_10->SysMemPitch;
    d3d_subresource_data->SysMemSlicePitch = d3d_subresource_data_10->SysMemSlicePitch;
}

void D3DWrapperConvertStruct( D3D10_SUBRESOURCE_DATA *d3d_subresource_data_10, D3D_SUBRESOURCE_DATA const *d3d_subresource_data )
{
    d3d_subresource_data_10->pSysMem = d3d_subresource_data->pSysMem;
    d3d_subresource_data_10->SysMemPitch = d3d_subresource_data->SysMemPitch;
    d3d_subresource_data_10->SysMemSlicePitch = d3d_subresource_data->SysMemSlicePitch;
}

void D3DWrapperConvertStruct( D3D_BUFFER_DESC *d3d_buffer_desc, D3D10_BUFFER_DESC const *d3d_buffer_desc_10 )
{
    d3d_buffer_desc->ByteWidth = d3d_buffer_desc_10->ByteWidth;
    D3DWrapperConvertEnum( &d3d_buffer_desc->Usage, d3d_buffer_desc_10->Usage );
    D3DFlagsConvertFrom10_BIND_FLAG( &d3d_buffer_desc->BindFlags, d3d_buffer_desc_10->BindFlags );
    D3DFlagsConvertFrom10_CPU_ACCESS_FLAG( &d3d_buffer_desc->CPUAccessFlags, d3d_buffer_desc_10->CPUAccessFlags );
    D3DFlagsConvertFrom10_RESOURCE_MISC_FLAG( &d3d_buffer_desc->MiscFlags, d3d_buffer_desc_10->MiscFlags );
}

void D3DWrapperConvertStruct( D3D10_BUFFER_DESC *d3d_buffer_desc_10, D3D_BUFFER_DESC const *d3d_buffer_desc )
{
    d3d_buffer_desc_10->ByteWidth = d3d_buffer_desc->ByteWidth;
    D3DWrapperConvertEnum( &d3d_buffer_desc_10->Usage, d3d_buffer_desc->Usage );
    D3DFlagsConvertTo10_BIND_FLAG( &d3d_buffer_desc_10->BindFlags, d3d_buffer_desc->BindFlags );
    D3DFlagsConvertTo10_CPU_ACCESS_FLAG( &d3d_buffer_desc_10->CPUAccessFlags, d3d_buffer_desc->CPUAccessFlags );
    D3DFlagsConvertTo10_RESOURCE_MISC_FLAG( &d3d_buffer_desc_10->MiscFlags, d3d_buffer_desc->MiscFlags );
}

void D3DWrapperConvertStruct( D3D_TEXTURE1D_DESC *d3d_texture1d_desc, D3D10_TEXTURE1D_DESC const *d3d_texture1d_desc_10 )
{
    d3d_texture1d_desc->Width = d3d_texture1d_desc_10->Width;
    d3d_texture1d_desc->MipLevels = d3d_texture1d_desc_10->MipLevels;
    d3d_texture1d_desc->ArraySize = d3d_texture1d_desc_10->ArraySize;
    d3d_texture1d_desc->Format = d3d_texture1d_desc_10->Format;
    D3DWrapperConvertEnum( &d3d_texture1d_desc->Usage, d3d_texture1d_desc_10->Usage );
    D3DFlagsConvertFrom10_BIND_FLAG( &d3d_texture1d_desc->BindFlags, d3d_texture1d_desc_10->BindFlags );
    D3DFlagsConvertFrom10_CPU_ACCESS_FLAG( &d3d_texture1d_desc->CPUAccessFlags, d3d_texture1d_desc_10->CPUAccessFlags );
    D3DFlagsConvertFrom10_RESOURCE_MISC_FLAG( &d3d_texture1d_desc->MiscFlags, d3d_texture1d_desc_10->MiscFlags );
}

void D3DWrapperConvertStruct( D3D10_TEXTURE1D_DESC *d3d_texture1d_desc_10, D3D_TEXTURE1D_DESC const *d3d_texture1d_desc )
{
    d3d_texture1d_desc_10->Width = d3d_texture1d_desc->Width;
    d3d_texture1d_desc_10->MipLevels = d3d_texture1d_desc->MipLevels;
    d3d_texture1d_desc_10->ArraySize = d3d_texture1d_desc->ArraySize;
    d3d_texture1d_desc_10->Format = d3d_texture1d_desc->Format;
    D3DWrapperConvertEnum( &d3d_texture1d_desc_10->Usage, d3d_texture1d_desc->Usage );
    D3DFlagsConvertTo10_BIND_FLAG( &d3d_texture1d_desc_10->BindFlags, d3d_texture1d_desc->BindFlags );
    D3DFlagsConvertTo10_CPU_ACCESS_FLAG( &d3d_texture1d_desc_10->CPUAccessFlags, d3d_texture1d_desc->CPUAccessFlags );
    D3DFlagsConvertTo10_RESOURCE_MISC_FLAG( &d3d_texture1d_desc_10->MiscFlags, d3d_texture1d_desc->MiscFlags );
}

void D3DWrapperConvertStruct( D3D_TEXTURE2D_DESC *d3d_texture2d_desc, D3D10_TEXTURE2D_DESC const *d3d_texture2d_desc_10 )
{
    d3d_texture2d_desc->Width = d3d_texture2d_desc_10->Width;
    d3d_texture2d_desc->Height = d3d_texture2d_desc_10->Height;
    d3d_texture2d_desc->MipLevels = d3d_texture2d_desc_10->MipLevels;
    d3d_texture2d_desc->ArraySize = d3d_texture2d_desc_10->ArraySize;
    d3d_texture2d_desc->Format = d3d_texture2d_desc_10->Format;
    d3d_texture2d_desc->SampleDesc = d3d_texture2d_desc_10->SampleDesc;
    D3DWrapperConvertEnum( &d3d_texture2d_desc->Usage, d3d_texture2d_desc_10->Usage );
    D3DFlagsConvertFrom10_BIND_FLAG( &d3d_texture2d_desc->BindFlags, d3d_texture2d_desc_10->BindFlags );
    D3DFlagsConvertFrom10_CPU_ACCESS_FLAG( &d3d_texture2d_desc->CPUAccessFlags, d3d_texture2d_desc_10->CPUAccessFlags );
    D3DFlagsConvertFrom10_RESOURCE_MISC_FLAG( &d3d_texture2d_desc->MiscFlags, d3d_texture2d_desc_10->MiscFlags );
}

void D3DWrapperConvertStruct( D3D10_TEXTURE2D_DESC *d3d_texture2d_desc_10, D3D_TEXTURE2D_DESC const *d3d_texture2d_desc )
{
    d3d_texture2d_desc_10->Width = d3d_texture2d_desc->Width;
    d3d_texture2d_desc_10->Height = d3d_texture2d_desc->Height;
    d3d_texture2d_desc_10->MipLevels = d3d_texture2d_desc->MipLevels;
    d3d_texture2d_desc_10->ArraySize = d3d_texture2d_desc->ArraySize;
    d3d_texture2d_desc_10->Format = d3d_texture2d_desc->Format;
    d3d_texture2d_desc_10->SampleDesc = d3d_texture2d_desc->SampleDesc;
    D3DWrapperConvertEnum( &d3d_texture2d_desc_10->Usage, d3d_texture2d_desc->Usage );
    D3DFlagsConvertTo10_BIND_FLAG( &d3d_texture2d_desc_10->BindFlags, d3d_texture2d_desc->BindFlags );
    D3DFlagsConvertTo10_CPU_ACCESS_FLAG( &d3d_texture2d_desc_10->CPUAccessFlags, d3d_texture2d_desc->CPUAccessFlags );
    D3DFlagsConvertTo10_RESOURCE_MISC_FLAG( &d3d_texture2d_desc_10->MiscFlags, d3d_texture2d_desc->MiscFlags );
}

void D3DWrapperConvertStruct( D3D_MAPPED_TEXTURE2D *d3d_mapped_texture2d, D3D10_MAPPED_TEXTURE2D const *d3d_mapped_texture2d_10 )
{
    d3d_mapped_texture2d->pData = d3d_mapped_texture2d_10->pData;
    d3d_mapped_texture2d->RowPitch = d3d_mapped_texture2d_10->RowPitch;
}

void D3DWrapperConvertStruct( D3D10_MAPPED_TEXTURE2D *d3d_mapped_texture2d_10, D3D_MAPPED_TEXTURE2D const *d3d_mapped_texture2d )
{
    d3d_mapped_texture2d_10->pData = d3d_mapped_texture2d->pData;
    d3d_mapped_texture2d_10->RowPitch = d3d_mapped_texture2d->RowPitch;
}

void D3DWrapperConvertStruct( D3D_TEXTURE3D_DESC *d3d_texture3d_desc, D3D10_TEXTURE3D_DESC const *d3d_texture3d_desc_10 )
{
    d3d_texture3d_desc->Width = d3d_texture3d_desc_10->Width;
    d3d_texture3d_desc->Height = d3d_texture3d_desc_10->Height;
    d3d_texture3d_desc->Depth = d3d_texture3d_desc_10->Depth;
    d3d_texture3d_desc->MipLevels = d3d_texture3d_desc_10->MipLevels;
    d3d_texture3d_desc->Format = d3d_texture3d_desc_10->Format;
    D3DWrapperConvertEnum( &d3d_texture3d_desc->Usage, d3d_texture3d_desc_10->Usage );
    D3DFlagsConvertFrom10_BIND_FLAG( &d3d_texture3d_desc->BindFlags, d3d_texture3d_desc_10->BindFlags );
    D3DFlagsConvertFrom10_CPU_ACCESS_FLAG( &d3d_texture3d_desc->CPUAccessFlags, d3d_texture3d_desc_10->CPUAccessFlags );
    D3DFlagsConvertFrom10_RESOURCE_MISC_FLAG( &d3d_texture3d_desc->MiscFlags, d3d_texture3d_desc_10->MiscFlags );
}

void D3DWrapperConvertStruct( D3D10_TEXTURE3D_DESC *d3d_texture3d_desc_10, D3D_TEXTURE3D_DESC const *d3d_texture3d_desc )
{
    d3d_texture3d_desc_10->Width = d3d_texture3d_desc->Width;
    d3d_texture3d_desc_10->Height = d3d_texture3d_desc->Height;
    d3d_texture3d_desc_10->Depth = d3d_texture3d_desc->Depth;
    d3d_texture3d_desc_10->MipLevels = d3d_texture3d_desc->MipLevels;
    d3d_texture3d_desc_10->Format = d3d_texture3d_desc->Format;
    D3DWrapperConvertEnum( &d3d_texture3d_desc_10->Usage, d3d_texture3d_desc->Usage );
    D3DFlagsConvertTo10_BIND_FLAG( &d3d_texture3d_desc_10->BindFlags, d3d_texture3d_desc->BindFlags );
    D3DFlagsConvertTo10_CPU_ACCESS_FLAG( &d3d_texture3d_desc_10->CPUAccessFlags, d3d_texture3d_desc->CPUAccessFlags );
    D3DFlagsConvertTo10_RESOURCE_MISC_FLAG( &d3d_texture3d_desc_10->MiscFlags, d3d_texture3d_desc->MiscFlags );
}

void D3DWrapperConvertStruct( D3D_MAPPED_TEXTURE3D *d3d_mapped_texture3d, D3D10_MAPPED_TEXTURE3D const *d3d_mapped_texture3d_10 )
{
    d3d_mapped_texture3d->pData = d3d_mapped_texture3d_10->pData;
    d3d_mapped_texture3d->RowPitch = d3d_mapped_texture3d_10->RowPitch;
    d3d_mapped_texture3d->DepthPitch = d3d_mapped_texture3d_10->DepthPitch;
}

void D3DWrapperConvertStruct( D3D10_MAPPED_TEXTURE3D *d3d_mapped_texture3d_10, D3D_MAPPED_TEXTURE3D const *d3d_mapped_texture3d )
{
    d3d_mapped_texture3d_10->pData = d3d_mapped_texture3d->pData;
    d3d_mapped_texture3d_10->RowPitch = d3d_mapped_texture3d->RowPitch;
    d3d_mapped_texture3d_10->DepthPitch = d3d_mapped_texture3d->DepthPitch;
}

void D3DWrapperConvertStruct( D3D_BUFFER_SRV *d3d_buffer_srv, D3D10_BUFFER_SRV const *d3d_buffer_srv_10 )
{
    d3d_buffer_srv->FirstElement = d3d_buffer_srv_10->FirstElement;
    d3d_buffer_srv->ElementOffset = d3d_buffer_srv_10->ElementOffset;
    d3d_buffer_srv->NumElements = d3d_buffer_srv_10->NumElements;
    d3d_buffer_srv->ElementWidth = d3d_buffer_srv_10->ElementWidth;
}

void D3DWrapperConvertStruct( D3D10_BUFFER_SRV *d3d_buffer_srv_10, D3D_BUFFER_SRV const *d3d_buffer_srv )
{
    d3d_buffer_srv_10->FirstElement = d3d_buffer_srv->FirstElement;
    d3d_buffer_srv_10->ElementOffset = d3d_buffer_srv->ElementOffset;
    d3d_buffer_srv_10->NumElements = d3d_buffer_srv->NumElements;
    d3d_buffer_srv_10->ElementWidth = d3d_buffer_srv->ElementWidth;
}

void D3DWrapperConvertStruct( D3D_TEX1D_SRV *d3d_tex1d_srv, D3D10_TEX1D_SRV const *d3d_tex1d_srv_10 )
{
    d3d_tex1d_srv->MostDetailedMip = d3d_tex1d_srv_10->MostDetailedMip;
    d3d_tex1d_srv->MipLevels = d3d_tex1d_srv_10->MipLevels;
}

void D3DWrapperConvertStruct( D3D10_TEX1D_SRV *d3d_tex1d_srv_10, D3D_TEX1D_SRV const *d3d_tex1d_srv )
{
    d3d_tex1d_srv_10->MostDetailedMip = d3d_tex1d_srv->MostDetailedMip;
    d3d_tex1d_srv_10->MipLevels = d3d_tex1d_srv->MipLevels;
}

void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_SRV *d3d_tex1d_array_srv, D3D10_TEX1D_ARRAY_SRV const *d3d_tex1d_array_srv_10 )
{
    d3d_tex1d_array_srv->MostDetailedMip = d3d_tex1d_array_srv_10->MostDetailedMip;
    d3d_tex1d_array_srv->MipLevels = d3d_tex1d_array_srv_10->MipLevels;
    d3d_tex1d_array_srv->FirstArraySlice = d3d_tex1d_array_srv_10->FirstArraySlice;
    d3d_tex1d_array_srv->ArraySize = d3d_tex1d_array_srv_10->ArraySize;
}

void D3DWrapperConvertStruct( D3D10_TEX1D_ARRAY_SRV *d3d_tex1d_array_srv_10, D3D_TEX1D_ARRAY_SRV const *d3d_tex1d_array_srv )
{
    d3d_tex1d_array_srv_10->MostDetailedMip = d3d_tex1d_array_srv->MostDetailedMip;
    d3d_tex1d_array_srv_10->MipLevels = d3d_tex1d_array_srv->MipLevels;
    d3d_tex1d_array_srv_10->FirstArraySlice = d3d_tex1d_array_srv->FirstArraySlice;
    d3d_tex1d_array_srv_10->ArraySize = d3d_tex1d_array_srv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX2D_SRV *d3d_tex2d_srv, D3D10_TEX2D_SRV const *d3d_tex2d_srv_10 )
{
    d3d_tex2d_srv->MostDetailedMip = d3d_tex2d_srv_10->MostDetailedMip;
    d3d_tex2d_srv->MipLevels = d3d_tex2d_srv_10->MipLevels;
}

void D3DWrapperConvertStruct( D3D10_TEX2D_SRV *d3d_tex2d_srv_10, D3D_TEX2D_SRV const *d3d_tex2d_srv )
{
    d3d_tex2d_srv_10->MostDetailedMip = d3d_tex2d_srv->MostDetailedMip;
    d3d_tex2d_srv_10->MipLevels = d3d_tex2d_srv->MipLevels;
}

void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_SRV *d3d_tex2d_array_srv, D3D10_TEX2D_ARRAY_SRV const *d3d_tex2d_array_srv_10 )
{
    d3d_tex2d_array_srv->MostDetailedMip = d3d_tex2d_array_srv_10->MostDetailedMip;
    d3d_tex2d_array_srv->MipLevels = d3d_tex2d_array_srv_10->MipLevels;
    d3d_tex2d_array_srv->FirstArraySlice = d3d_tex2d_array_srv_10->FirstArraySlice;
    d3d_tex2d_array_srv->ArraySize = d3d_tex2d_array_srv_10->ArraySize;
}

void D3DWrapperConvertStruct( D3D10_TEX2D_ARRAY_SRV *d3d_tex2d_array_srv_10, D3D_TEX2D_ARRAY_SRV const *d3d_tex2d_array_srv )
{
    d3d_tex2d_array_srv_10->MostDetailedMip = d3d_tex2d_array_srv->MostDetailedMip;
    d3d_tex2d_array_srv_10->MipLevels = d3d_tex2d_array_srv->MipLevels;
    d3d_tex2d_array_srv_10->FirstArraySlice = d3d_tex2d_array_srv->FirstArraySlice;
    d3d_tex2d_array_srv_10->ArraySize = d3d_tex2d_array_srv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX3D_SRV *d3d_tex3d_srv, D3D10_TEX3D_SRV const *d3d_tex3d_srv_10 )
{
    d3d_tex3d_srv->MostDetailedMip = d3d_tex3d_srv_10->MostDetailedMip;
    d3d_tex3d_srv->MipLevels = d3d_tex3d_srv_10->MipLevels;
}

void D3DWrapperConvertStruct( D3D10_TEX3D_SRV *d3d_tex3d_srv_10, D3D_TEX3D_SRV const *d3d_tex3d_srv )
{
    d3d_tex3d_srv_10->MostDetailedMip = d3d_tex3d_srv->MostDetailedMip;
    d3d_tex3d_srv_10->MipLevels = d3d_tex3d_srv->MipLevels;
}

void D3DWrapperConvertStruct( D3D_TEXCUBE_SRV *d3d_texcube_srv, D3D10_TEXCUBE_SRV const *d3d_texcube_srv_10 )
{
    d3d_texcube_srv->MostDetailedMip = d3d_texcube_srv_10->MostDetailedMip;
    d3d_texcube_srv->MipLevels = d3d_texcube_srv_10->MipLevels;
}

void D3DWrapperConvertStruct( D3D10_TEXCUBE_SRV *d3d_texcube_srv_10, D3D_TEXCUBE_SRV const *d3d_texcube_srv )
{
    d3d_texcube_srv_10->MostDetailedMip = d3d_texcube_srv->MostDetailedMip;
    d3d_texcube_srv_10->MipLevels = d3d_texcube_srv->MipLevels;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_SRV *d3d_tex2dms_srv, D3D10_TEX2DMS_SRV const *d3d_tex2dms_srv_10 )
{
    d3d_tex2dms_srv->UnusedField_NothingToDefine = d3d_tex2dms_srv_10->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D10_TEX2DMS_SRV *d3d_tex2dms_srv_10, D3D_TEX2DMS_SRV const *d3d_tex2dms_srv )
{
    d3d_tex2dms_srv_10->UnusedField_NothingToDefine = d3d_tex2dms_srv->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_SRV *d3d_tex2dms_array_srv, D3D10_TEX2DMS_ARRAY_SRV const *d3d_tex2dms_array_srv_10 )
{
    d3d_tex2dms_array_srv->FirstArraySlice = d3d_tex2dms_array_srv_10->FirstArraySlice;
    d3d_tex2dms_array_srv->ArraySize = d3d_tex2dms_array_srv_10->ArraySize;
}

void D3DWrapperConvertStruct( D3D10_TEX2DMS_ARRAY_SRV *d3d_tex2dms_array_srv_10, D3D_TEX2DMS_ARRAY_SRV const *d3d_tex2dms_array_srv )
{
    d3d_tex2dms_array_srv_10->FirstArraySlice = d3d_tex2dms_array_srv->FirstArraySlice;
    d3d_tex2dms_array_srv_10->ArraySize = d3d_tex2dms_array_srv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_SHADER_RESOURCE_VIEW_DESC *d3d_shader_resource_view_desc, D3D10_SHADER_RESOURCE_VIEW_DESC const *d3d_shader_resource_view_desc_10 )
{
    d3d_shader_resource_view_desc->Format = d3d_shader_resource_view_desc_10->Format;
    D3DWrapperConvertEnum( &d3d_shader_resource_view_desc->ViewDimension, d3d_shader_resource_view_desc_10->ViewDimension );
    switch( d3d_shader_resource_view_desc_10->ViewDimension )
    {
        case D3D10_SRV_DIMENSION_BUFFER:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Buffer, &d3d_shader_resource_view_desc_10->Buffer );
            break;
        case D3D10_SRV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture1D, &d3d_shader_resource_view_desc_10->Texture1D );
            break;
        case D3D10_SRV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture1DArray, &d3d_shader_resource_view_desc_10->Texture1DArray );
            break;
        case D3D10_SRV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture2D, &d3d_shader_resource_view_desc_10->Texture2D );
            break;
        case D3D10_SRV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture2DArray, &d3d_shader_resource_view_desc_10->Texture2DArray );
            break;
        case D3D10_SRV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture2DMS, &d3d_shader_resource_view_desc_10->Texture2DMS );
            break;
        case D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture2DMSArray, &d3d_shader_resource_view_desc_10->Texture2DMSArray );
            break;
        case D3D10_SRV_DIMENSION_TEXTURE3D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture3D, &d3d_shader_resource_view_desc_10->Texture3D );
            break;
        case D3D10_SRV_DIMENSION_TEXTURECUBE:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->TextureCube, &d3d_shader_resource_view_desc_10->TextureCube );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D10_SHADER_RESOURCE_VIEW_DESC *d3d_shader_resource_view_desc_10, D3D_SHADER_RESOURCE_VIEW_DESC const *d3d_shader_resource_view_desc )
{
    d3d_shader_resource_view_desc_10->Format = d3d_shader_resource_view_desc->Format;
    D3DWrapperConvertEnum( &d3d_shader_resource_view_desc_10->ViewDimension, d3d_shader_resource_view_desc->ViewDimension );
    switch( d3d_shader_resource_view_desc->ViewDimension )
    {
        case D3D_SRV_DIMENSION_BUFFER:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_10->Buffer, &d3d_shader_resource_view_desc->Buffer );
            break;
        case D3D_SRV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_10->Texture1D, &d3d_shader_resource_view_desc->Texture1D );
            break;
        case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_10->Texture1DArray, &d3d_shader_resource_view_desc->Texture1DArray );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_10->Texture2D, &d3d_shader_resource_view_desc->Texture2D );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_10->Texture2DArray, &d3d_shader_resource_view_desc->Texture2DArray );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_10->Texture2DMS, &d3d_shader_resource_view_desc->Texture2DMS );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_10->Texture2DMSArray, &d3d_shader_resource_view_desc->Texture2DMSArray );
            break;
        case D3D_SRV_DIMENSION_TEXTURE3D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_10->Texture3D, &d3d_shader_resource_view_desc->Texture3D );
            break;
        case D3D_SRV_DIMENSION_TEXTURECUBE:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_10->TextureCube, &d3d_shader_resource_view_desc->TextureCube );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D_BUFFER_RTV *d3d_buffer_rtv, D3D10_BUFFER_RTV const *d3d_buffer_rtv_10 )
{
    d3d_buffer_rtv->FirstElement = d3d_buffer_rtv_10->FirstElement;
    d3d_buffer_rtv->ElementOffset = d3d_buffer_rtv_10->ElementOffset;
    d3d_buffer_rtv->NumElements = d3d_buffer_rtv_10->NumElements;
    d3d_buffer_rtv->ElementWidth = d3d_buffer_rtv_10->ElementWidth;
}

void D3DWrapperConvertStruct( D3D10_BUFFER_RTV *d3d_buffer_rtv_10, D3D_BUFFER_RTV const *d3d_buffer_rtv )
{
    d3d_buffer_rtv_10->FirstElement = d3d_buffer_rtv->FirstElement;
    d3d_buffer_rtv_10->ElementOffset = d3d_buffer_rtv->ElementOffset;
    d3d_buffer_rtv_10->NumElements = d3d_buffer_rtv->NumElements;
    d3d_buffer_rtv_10->ElementWidth = d3d_buffer_rtv->ElementWidth;
}

void D3DWrapperConvertStruct( D3D_TEX1D_RTV *d3d_tex1d_rtv, D3D10_TEX1D_RTV const *d3d_tex1d_rtv_10 )
{
    d3d_tex1d_rtv->MipSlice = d3d_tex1d_rtv_10->MipSlice;
}

void D3DWrapperConvertStruct( D3D10_TEX1D_RTV *d3d_tex1d_rtv_10, D3D_TEX1D_RTV const *d3d_tex1d_rtv )
{
    d3d_tex1d_rtv_10->MipSlice = d3d_tex1d_rtv->MipSlice;
}

void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_RTV *d3d_tex1d_array_rtv, D3D10_TEX1D_ARRAY_RTV const *d3d_tex1d_array_rtv_10 )
{
    d3d_tex1d_array_rtv->MipSlice = d3d_tex1d_array_rtv_10->MipSlice;
    d3d_tex1d_array_rtv->FirstArraySlice = d3d_tex1d_array_rtv_10->FirstArraySlice;
    d3d_tex1d_array_rtv->ArraySize = d3d_tex1d_array_rtv_10->ArraySize;
}

void D3DWrapperConvertStruct( D3D10_TEX1D_ARRAY_RTV *d3d_tex1d_array_rtv_10, D3D_TEX1D_ARRAY_RTV const *d3d_tex1d_array_rtv )
{
    d3d_tex1d_array_rtv_10->MipSlice = d3d_tex1d_array_rtv->MipSlice;
    d3d_tex1d_array_rtv_10->FirstArraySlice = d3d_tex1d_array_rtv->FirstArraySlice;
    d3d_tex1d_array_rtv_10->ArraySize = d3d_tex1d_array_rtv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX2D_RTV *d3d_tex2d_rtv, D3D10_TEX2D_RTV const *d3d_tex2d_rtv_10 )
{
    d3d_tex2d_rtv->MipSlice = d3d_tex2d_rtv_10->MipSlice;
}

void D3DWrapperConvertStruct( D3D10_TEX2D_RTV *d3d_tex2d_rtv_10, D3D_TEX2D_RTV const *d3d_tex2d_rtv )
{
    d3d_tex2d_rtv_10->MipSlice = d3d_tex2d_rtv->MipSlice;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_RTV *d3d_tex2dms_rtv, D3D10_TEX2DMS_RTV const *d3d_tex2dms_rtv_10 )
{
    d3d_tex2dms_rtv->UnusedField_NothingToDefine = d3d_tex2dms_rtv_10->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D10_TEX2DMS_RTV *d3d_tex2dms_rtv_10, D3D_TEX2DMS_RTV const *d3d_tex2dms_rtv )
{
    d3d_tex2dms_rtv_10->UnusedField_NothingToDefine = d3d_tex2dms_rtv->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_RTV *d3d_tex2d_array_rtv, D3D10_TEX2D_ARRAY_RTV const *d3d_tex2d_array_rtv_10 )
{
    d3d_tex2d_array_rtv->MipSlice = d3d_tex2d_array_rtv_10->MipSlice;
    d3d_tex2d_array_rtv->FirstArraySlice = d3d_tex2d_array_rtv_10->FirstArraySlice;
    d3d_tex2d_array_rtv->ArraySize = d3d_tex2d_array_rtv_10->ArraySize;
}

void D3DWrapperConvertStruct( D3D10_TEX2D_ARRAY_RTV *d3d_tex2d_array_rtv_10, D3D_TEX2D_ARRAY_RTV const *d3d_tex2d_array_rtv )
{
    d3d_tex2d_array_rtv_10->MipSlice = d3d_tex2d_array_rtv->MipSlice;
    d3d_tex2d_array_rtv_10->FirstArraySlice = d3d_tex2d_array_rtv->FirstArraySlice;
    d3d_tex2d_array_rtv_10->ArraySize = d3d_tex2d_array_rtv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_RTV *d3d_tex2dms_array_rtv, D3D10_TEX2DMS_ARRAY_RTV const *d3d_tex2dms_array_rtv_10 )
{
    d3d_tex2dms_array_rtv->FirstArraySlice = d3d_tex2dms_array_rtv_10->FirstArraySlice;
    d3d_tex2dms_array_rtv->ArraySize = d3d_tex2dms_array_rtv_10->ArraySize;
}

void D3DWrapperConvertStruct( D3D10_TEX2DMS_ARRAY_RTV *d3d_tex2dms_array_rtv_10, D3D_TEX2DMS_ARRAY_RTV const *d3d_tex2dms_array_rtv )
{
    d3d_tex2dms_array_rtv_10->FirstArraySlice = d3d_tex2dms_array_rtv->FirstArraySlice;
    d3d_tex2dms_array_rtv_10->ArraySize = d3d_tex2dms_array_rtv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX3D_RTV *d3d_tex3d_rtv, D3D10_TEX3D_RTV const *d3d_tex3d_rtv_10 )
{
    d3d_tex3d_rtv->MipSlice = d3d_tex3d_rtv_10->MipSlice;
    d3d_tex3d_rtv->FirstWSlice = d3d_tex3d_rtv_10->FirstWSlice;
    d3d_tex3d_rtv->WSize = d3d_tex3d_rtv_10->WSize;
}

void D3DWrapperConvertStruct( D3D10_TEX3D_RTV *d3d_tex3d_rtv_10, D3D_TEX3D_RTV const *d3d_tex3d_rtv )
{
    d3d_tex3d_rtv_10->MipSlice = d3d_tex3d_rtv->MipSlice;
    d3d_tex3d_rtv_10->FirstWSlice = d3d_tex3d_rtv->FirstWSlice;
    d3d_tex3d_rtv_10->WSize = d3d_tex3d_rtv->WSize;
}

void D3DWrapperConvertStruct( D3D_RENDER_TARGET_VIEW_DESC *d3d_render_target_view_desc, D3D10_RENDER_TARGET_VIEW_DESC const *d3d_render_target_view_desc_10 )
{
    d3d_render_target_view_desc->Format = d3d_render_target_view_desc_10->Format;
    D3DWrapperConvertEnum( &d3d_render_target_view_desc->ViewDimension, d3d_render_target_view_desc_10->ViewDimension );
    switch( d3d_render_target_view_desc_10->ViewDimension )
    {
        case D3D10_RTV_DIMENSION_BUFFER:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Buffer, &d3d_render_target_view_desc_10->Buffer );
            break;
        case D3D10_RTV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture1D, &d3d_render_target_view_desc_10->Texture1D );
            break;
        case D3D10_RTV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture1DArray, &d3d_render_target_view_desc_10->Texture1DArray );
            break;
        case D3D10_RTV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture2D, &d3d_render_target_view_desc_10->Texture2D );
            break;
        case D3D10_RTV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture2DArray, &d3d_render_target_view_desc_10->Texture2DArray );
            break;
        case D3D10_RTV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture2DMS, &d3d_render_target_view_desc_10->Texture2DMS );
            break;
        case D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture2DMSArray, &d3d_render_target_view_desc_10->Texture2DMSArray );
            break;
        case D3D10_RTV_DIMENSION_TEXTURE3D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture3D, &d3d_render_target_view_desc_10->Texture3D );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D10_RENDER_TARGET_VIEW_DESC *d3d_render_target_view_desc_10, D3D_RENDER_TARGET_VIEW_DESC const *d3d_render_target_view_desc )
{
    d3d_render_target_view_desc_10->Format = d3d_render_target_view_desc->Format;
    D3DWrapperConvertEnum( &d3d_render_target_view_desc_10->ViewDimension, d3d_render_target_view_desc->ViewDimension );
    switch( d3d_render_target_view_desc->ViewDimension )
    {
        case D3D_RTV_DIMENSION_BUFFER:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_10->Buffer, &d3d_render_target_view_desc->Buffer );
            break;
        case D3D_RTV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_10->Texture1D, &d3d_render_target_view_desc->Texture1D );
            break;
        case D3D_RTV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_10->Texture1DArray, &d3d_render_target_view_desc->Texture1DArray );
            break;
        case D3D_RTV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_10->Texture2D, &d3d_render_target_view_desc->Texture2D );
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_10->Texture2DArray, &d3d_render_target_view_desc->Texture2DArray );
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_10->Texture2DMS, &d3d_render_target_view_desc->Texture2DMS );
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_10->Texture2DMSArray, &d3d_render_target_view_desc->Texture2DMSArray );
            break;
        case D3D_RTV_DIMENSION_TEXTURE3D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_10->Texture3D, &d3d_render_target_view_desc->Texture3D );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D_TEX1D_DSV *d3d_tex1d_dsv, D3D10_TEX1D_DSV const *d3d_tex1d_dsv_10 )
{
    d3d_tex1d_dsv->MipSlice = d3d_tex1d_dsv_10->MipSlice;
}

void D3DWrapperConvertStruct( D3D10_TEX1D_DSV *d3d_tex1d_dsv_10, D3D_TEX1D_DSV const *d3d_tex1d_dsv )
{
    d3d_tex1d_dsv_10->MipSlice = d3d_tex1d_dsv->MipSlice;
}

void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_DSV *d3d_tex1d_array_dsv, D3D10_TEX1D_ARRAY_DSV const *d3d_tex1d_array_dsv_10 )
{
    d3d_tex1d_array_dsv->MipSlice = d3d_tex1d_array_dsv_10->MipSlice;
    d3d_tex1d_array_dsv->FirstArraySlice = d3d_tex1d_array_dsv_10->FirstArraySlice;
    d3d_tex1d_array_dsv->ArraySize = d3d_tex1d_array_dsv_10->ArraySize;
}

void D3DWrapperConvertStruct( D3D10_TEX1D_ARRAY_DSV *d3d_tex1d_array_dsv_10, D3D_TEX1D_ARRAY_DSV const *d3d_tex1d_array_dsv )
{
    d3d_tex1d_array_dsv_10->MipSlice = d3d_tex1d_array_dsv->MipSlice;
    d3d_tex1d_array_dsv_10->FirstArraySlice = d3d_tex1d_array_dsv->FirstArraySlice;
    d3d_tex1d_array_dsv_10->ArraySize = d3d_tex1d_array_dsv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX2D_DSV *d3d_tex2d_dsv, D3D10_TEX2D_DSV const *d3d_tex2d_dsv_10 )
{
    d3d_tex2d_dsv->MipSlice = d3d_tex2d_dsv_10->MipSlice;
}

void D3DWrapperConvertStruct( D3D10_TEX2D_DSV *d3d_tex2d_dsv_10, D3D_TEX2D_DSV const *d3d_tex2d_dsv )
{
    d3d_tex2d_dsv_10->MipSlice = d3d_tex2d_dsv->MipSlice;
}

void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_DSV *d3d_tex2d_array_dsv, D3D10_TEX2D_ARRAY_DSV const *d3d_tex2d_array_dsv_10 )
{
    d3d_tex2d_array_dsv->MipSlice = d3d_tex2d_array_dsv_10->MipSlice;
    d3d_tex2d_array_dsv->FirstArraySlice = d3d_tex2d_array_dsv_10->FirstArraySlice;
    d3d_tex2d_array_dsv->ArraySize = d3d_tex2d_array_dsv_10->ArraySize;
}

void D3DWrapperConvertStruct( D3D10_TEX2D_ARRAY_DSV *d3d_tex2d_array_dsv_10, D3D_TEX2D_ARRAY_DSV const *d3d_tex2d_array_dsv )
{
    d3d_tex2d_array_dsv_10->MipSlice = d3d_tex2d_array_dsv->MipSlice;
    d3d_tex2d_array_dsv_10->FirstArraySlice = d3d_tex2d_array_dsv->FirstArraySlice;
    d3d_tex2d_array_dsv_10->ArraySize = d3d_tex2d_array_dsv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_DSV *d3d_tex2dms_dsv, D3D10_TEX2DMS_DSV const *d3d_tex2dms_dsv_10 )
{
    d3d_tex2dms_dsv->UnusedField_NothingToDefine = d3d_tex2dms_dsv_10->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D10_TEX2DMS_DSV *d3d_tex2dms_dsv_10, D3D_TEX2DMS_DSV const *d3d_tex2dms_dsv )
{
    d3d_tex2dms_dsv_10->UnusedField_NothingToDefine = d3d_tex2dms_dsv->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_DSV *d3d_tex2dms_array_dsv, D3D10_TEX2DMS_ARRAY_DSV const *d3d_tex2dms_array_dsv_10 )
{
    d3d_tex2dms_array_dsv->FirstArraySlice = d3d_tex2dms_array_dsv_10->FirstArraySlice;
    d3d_tex2dms_array_dsv->ArraySize = d3d_tex2dms_array_dsv_10->ArraySize;
}

void D3DWrapperConvertStruct( D3D10_TEX2DMS_ARRAY_DSV *d3d_tex2dms_array_dsv_10, D3D_TEX2DMS_ARRAY_DSV const *d3d_tex2dms_array_dsv )
{
    d3d_tex2dms_array_dsv_10->FirstArraySlice = d3d_tex2dms_array_dsv->FirstArraySlice;
    d3d_tex2dms_array_dsv_10->ArraySize = d3d_tex2dms_array_dsv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_DEPTH_STENCIL_VIEW_DESC *d3d_depth_stencil_view_desc, D3D10_DEPTH_STENCIL_VIEW_DESC const *d3d_depth_stencil_view_desc_10 )
{
    d3d_depth_stencil_view_desc->Format = d3d_depth_stencil_view_desc_10->Format;
    D3DWrapperConvertEnum( &d3d_depth_stencil_view_desc->ViewDimension, d3d_depth_stencil_view_desc_10->ViewDimension );
    switch( d3d_depth_stencil_view_desc_10->ViewDimension )
    {
        case D3D10_DSV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture1D, &d3d_depth_stencil_view_desc_10->Texture1D );
            break;
        case D3D10_DSV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture1DArray, &d3d_depth_stencil_view_desc_10->Texture1DArray );
            break;
        case D3D10_DSV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture2D, &d3d_depth_stencil_view_desc_10->Texture2D );
            break;
        case D3D10_DSV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture2DArray, &d3d_depth_stencil_view_desc_10->Texture2DArray );
            break;
        case D3D10_DSV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture2DMS, &d3d_depth_stencil_view_desc_10->Texture2DMS );
            break;
        case D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture2DMSArray, &d3d_depth_stencil_view_desc_10->Texture2DMSArray );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D10_DEPTH_STENCIL_VIEW_DESC *d3d_depth_stencil_view_desc_10, D3D_DEPTH_STENCIL_VIEW_DESC const *d3d_depth_stencil_view_desc )
{
    d3d_depth_stencil_view_desc_10->Format = d3d_depth_stencil_view_desc->Format;
    D3DWrapperConvertEnum( &d3d_depth_stencil_view_desc_10->ViewDimension, d3d_depth_stencil_view_desc->ViewDimension );
    switch( d3d_depth_stencil_view_desc->ViewDimension )
    {
        case D3D_DSV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_10->Texture1D, &d3d_depth_stencil_view_desc->Texture1D );
            break;
        case D3D_DSV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_10->Texture1DArray, &d3d_depth_stencil_view_desc->Texture1DArray );
            break;
        case D3D_DSV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_10->Texture2D, &d3d_depth_stencil_view_desc->Texture2D );
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_10->Texture2DArray, &d3d_depth_stencil_view_desc->Texture2DArray );
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_10->Texture2DMS, &d3d_depth_stencil_view_desc->Texture2DMS );
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_10->Texture2DMSArray, &d3d_depth_stencil_view_desc->Texture2DMSArray );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D_SAMPLER_DESC *d3d_sampler_desc, D3D10_SAMPLER_DESC const *d3d_sampler_desc_10 )
{
    D3DWrapperConvertEnum( &d3d_sampler_desc->Filter, d3d_sampler_desc_10->Filter );
    D3DWrapperConvertEnum( &d3d_sampler_desc->AddressU, d3d_sampler_desc_10->AddressU );
    D3DWrapperConvertEnum( &d3d_sampler_desc->AddressV, d3d_sampler_desc_10->AddressV );
    D3DWrapperConvertEnum( &d3d_sampler_desc->AddressW, d3d_sampler_desc_10->AddressW );
    d3d_sampler_desc->MipLODBias = d3d_sampler_desc_10->MipLODBias;
    d3d_sampler_desc->MaxAnisotropy = d3d_sampler_desc_10->MaxAnisotropy;
    D3DWrapperConvertEnum( &d3d_sampler_desc->ComparisonFunc, d3d_sampler_desc_10->ComparisonFunc );
    d3d_sampler_desc->BorderColor[3] = d3d_sampler_desc_10->BorderColor[3];
    d3d_sampler_desc->BorderColor[2] = d3d_sampler_desc_10->BorderColor[2];
    d3d_sampler_desc->BorderColor[1] = d3d_sampler_desc_10->BorderColor[1];
    d3d_sampler_desc->BorderColor[0] = d3d_sampler_desc_10->BorderColor[0];
    d3d_sampler_desc->MinLOD = d3d_sampler_desc_10->MinLOD;
    d3d_sampler_desc->MaxLOD = d3d_sampler_desc_10->MaxLOD;
}

void D3DWrapperConvertStruct( D3D10_SAMPLER_DESC *d3d_sampler_desc_10, D3D_SAMPLER_DESC const *d3d_sampler_desc )
{
    D3DWrapperConvertEnum( &d3d_sampler_desc_10->Filter, d3d_sampler_desc->Filter );
    D3DWrapperConvertEnum( &d3d_sampler_desc_10->AddressU, d3d_sampler_desc->AddressU );
    D3DWrapperConvertEnum( &d3d_sampler_desc_10->AddressV, d3d_sampler_desc->AddressV );
    D3DWrapperConvertEnum( &d3d_sampler_desc_10->AddressW, d3d_sampler_desc->AddressW );
    d3d_sampler_desc_10->MipLODBias = d3d_sampler_desc->MipLODBias;
    d3d_sampler_desc_10->MaxAnisotropy = d3d_sampler_desc->MaxAnisotropy;
    D3DWrapperConvertEnum( &d3d_sampler_desc_10->ComparisonFunc, d3d_sampler_desc->ComparisonFunc );
    d3d_sampler_desc_10->BorderColor[3] = d3d_sampler_desc->BorderColor[3];
    d3d_sampler_desc_10->BorderColor[2] = d3d_sampler_desc->BorderColor[2];
    d3d_sampler_desc_10->BorderColor[1] = d3d_sampler_desc->BorderColor[1];
    d3d_sampler_desc_10->BorderColor[0] = d3d_sampler_desc->BorderColor[0];
    d3d_sampler_desc_10->MinLOD = d3d_sampler_desc->MinLOD;
    d3d_sampler_desc_10->MaxLOD = d3d_sampler_desc->MaxLOD;
}

void D3DWrapperConvertStruct( D3D_QUERY_DESC *d3d_query_desc, D3D10_QUERY_DESC const *d3d_query_desc_10 )
{
    D3DWrapperConvertEnum( &d3d_query_desc->Query, d3d_query_desc_10->Query );
    D3DFlagsConvertFrom10_QUERY_MISC_FLAG( &d3d_query_desc->MiscFlags, d3d_query_desc_10->MiscFlags );
}

void D3DWrapperConvertStruct( D3D10_QUERY_DESC *d3d_query_desc_10, D3D_QUERY_DESC const *d3d_query_desc )
{
    D3DWrapperConvertEnum( &d3d_query_desc_10->Query, d3d_query_desc->Query );
    D3DFlagsConvertTo10_QUERY_MISC_FLAG( &d3d_query_desc_10->MiscFlags, d3d_query_desc->MiscFlags );
}

void D3DWrapperConvertStruct( D3D_QUERY_DATA_TIMESTAMP_DISJOINT *d3d_query_data_timestamp_disjoint, D3D10_QUERY_DATA_TIMESTAMP_DISJOINT const *d3d_query_data_timestamp_disjoint_10 )
{
    d3d_query_data_timestamp_disjoint->Frequency = d3d_query_data_timestamp_disjoint_10->Frequency;
    d3d_query_data_timestamp_disjoint->Disjoint = d3d_query_data_timestamp_disjoint_10->Disjoint;
}

void D3DWrapperConvertStruct( D3D10_QUERY_DATA_TIMESTAMP_DISJOINT *d3d_query_data_timestamp_disjoint_10, D3D_QUERY_DATA_TIMESTAMP_DISJOINT const *d3d_query_data_timestamp_disjoint )
{
    d3d_query_data_timestamp_disjoint_10->Frequency = d3d_query_data_timestamp_disjoint->Frequency;
    d3d_query_data_timestamp_disjoint_10->Disjoint = d3d_query_data_timestamp_disjoint->Disjoint;
}

void D3DWrapperConvertStruct( D3D_QUERY_DATA_PIPELINE_STATISTICS *d3d_query_data_pipeline_statistics, D3D10_QUERY_DATA_PIPELINE_STATISTICS const *d3d_query_data_pipeline_statistics_10 )
{
    d3d_query_data_pipeline_statistics->IAVertices = d3d_query_data_pipeline_statistics_10->IAVertices;
    d3d_query_data_pipeline_statistics->IAPrimitives = d3d_query_data_pipeline_statistics_10->IAPrimitives;
    d3d_query_data_pipeline_statistics->VSInvocations = d3d_query_data_pipeline_statistics_10->VSInvocations;
    d3d_query_data_pipeline_statistics->GSInvocations = d3d_query_data_pipeline_statistics_10->GSInvocations;
    d3d_query_data_pipeline_statistics->GSPrimitives = d3d_query_data_pipeline_statistics_10->GSPrimitives;
    d3d_query_data_pipeline_statistics->CInvocations = d3d_query_data_pipeline_statistics_10->CInvocations;
    d3d_query_data_pipeline_statistics->CPrimitives = d3d_query_data_pipeline_statistics_10->CPrimitives;
    d3d_query_data_pipeline_statistics->PSInvocations = d3d_query_data_pipeline_statistics_10->PSInvocations;
}

void D3DWrapperConvertStruct( D3D10_QUERY_DATA_PIPELINE_STATISTICS *d3d_query_data_pipeline_statistics_10, D3D_QUERY_DATA_PIPELINE_STATISTICS const *d3d_query_data_pipeline_statistics )
{
    d3d_query_data_pipeline_statistics_10->IAVertices = d3d_query_data_pipeline_statistics->IAVertices;
    d3d_query_data_pipeline_statistics_10->IAPrimitives = d3d_query_data_pipeline_statistics->IAPrimitives;
    d3d_query_data_pipeline_statistics_10->VSInvocations = d3d_query_data_pipeline_statistics->VSInvocations;
    d3d_query_data_pipeline_statistics_10->GSInvocations = d3d_query_data_pipeline_statistics->GSInvocations;
    d3d_query_data_pipeline_statistics_10->GSPrimitives = d3d_query_data_pipeline_statistics->GSPrimitives;
    d3d_query_data_pipeline_statistics_10->CInvocations = d3d_query_data_pipeline_statistics->CInvocations;
    d3d_query_data_pipeline_statistics_10->CPrimitives = d3d_query_data_pipeline_statistics->CPrimitives;
    d3d_query_data_pipeline_statistics_10->PSInvocations = d3d_query_data_pipeline_statistics->PSInvocations;
}

void D3DWrapperConvertStruct( D3D_QUERY_DATA_SO_STATISTICS *d3d_query_data_so_statistics, D3D10_QUERY_DATA_SO_STATISTICS const *d3d_query_data_so_statistics_10 )
{
    d3d_query_data_so_statistics->NumPrimitivesWritten = d3d_query_data_so_statistics_10->NumPrimitivesWritten;
    d3d_query_data_so_statistics->PrimitivesStorageNeeded = d3d_query_data_so_statistics_10->PrimitivesStorageNeeded;
}

void D3DWrapperConvertStruct( D3D10_QUERY_DATA_SO_STATISTICS *d3d_query_data_so_statistics_10, D3D_QUERY_DATA_SO_STATISTICS const *d3d_query_data_so_statistics )
{
    d3d_query_data_so_statistics_10->NumPrimitivesWritten = d3d_query_data_so_statistics->NumPrimitivesWritten;
    d3d_query_data_so_statistics_10->PrimitivesStorageNeeded = d3d_query_data_so_statistics->PrimitivesStorageNeeded;
}

void D3DWrapperConvertStruct( D3D_COUNTER_DESC *d3d_counter_desc, D3D10_COUNTER_DESC const *d3d_counter_desc_10 )
{
    D3DWrapperConvertEnum( &d3d_counter_desc->Counter, d3d_counter_desc_10->Counter );
    d3d_counter_desc->MiscFlags = d3d_counter_desc_10->MiscFlags;
}

void D3DWrapperConvertStruct( D3D10_COUNTER_DESC *d3d_counter_desc_10, D3D_COUNTER_DESC const *d3d_counter_desc )
{
    D3DWrapperConvertEnum( &d3d_counter_desc_10->Counter, d3d_counter_desc->Counter );
    d3d_counter_desc_10->MiscFlags = d3d_counter_desc->MiscFlags;
}

void D3DWrapperConvertStruct( D3D_COUNTER_INFO *d3d_counter_info, D3D10_COUNTER_INFO const *d3d_counter_info_10 )
{
    D3DWrapperConvertEnum( &d3d_counter_info->LastDeviceDependentCounter, d3d_counter_info_10->LastDeviceDependentCounter );
    d3d_counter_info->NumSimultaneousCounters = d3d_counter_info_10->NumSimultaneousCounters;
    d3d_counter_info->NumDetectableParallelUnits = d3d_counter_info_10->NumDetectableParallelUnits;
}

void D3DWrapperConvertStruct( D3D10_COUNTER_INFO *d3d_counter_info_10, D3D_COUNTER_INFO const *d3d_counter_info )
{
    D3DWrapperConvertEnum( &d3d_counter_info_10->LastDeviceDependentCounter, d3d_counter_info->LastDeviceDependentCounter );
    d3d_counter_info_10->NumSimultaneousCounters = d3d_counter_info->NumSimultaneousCounters;
    d3d_counter_info_10->NumDetectableParallelUnits = d3d_counter_info->NumDetectableParallelUnits;
}

void D3DWrapperConvertStruct( D3D_MESSAGE *d3d_message, D3D10_MESSAGE const *d3d_message_10 )
{
    D3DWrapperConvertEnum( &d3d_message->Category, d3d_message_10->Category );
    D3DWrapperConvertEnum( &d3d_message->Severity, d3d_message_10->Severity );
    D3DWrapperConvertEnum( &d3d_message->ID, d3d_message_10->ID );
    d3d_message->pDescription = (const char *)((BYTE*)(d3d_message) + sizeof(D3D_MESSAGE));
    memcpy((void*)d3d_message->pDescription, d3d_message_10->pDescription, d3d_message_10->DescriptionByteLength);
    d3d_message->DescriptionByteLength = d3d_message_10->DescriptionByteLength;
}

void D3DWrapperConvertStruct( D3D10_MESSAGE *d3d_message_10, D3D_MESSAGE const *d3d_message )
{
    D3DWrapperConvertEnum( &d3d_message_10->Category, d3d_message->Category );
    D3DWrapperConvertEnum( &d3d_message_10->Severity, d3d_message->Severity );
    D3DWrapperConvertEnum( &d3d_message_10->ID, d3d_message->ID );
    d3d_message_10->pDescription = (const char *)((BYTE*)(d3d_message_10) + sizeof(D3D_MESSAGE));
    memcpy((void*)d3d_message_10->pDescription, d3d_message->pDescription, d3d_message->DescriptionByteLength);
    d3d_message_10->DescriptionByteLength = d3d_message->DescriptionByteLength;
}

void D3DWrapperConvertStruct( D3D_INFO_QUEUE_FILTER_DESC *d3d_info_queue_filter_desc, D3D10_INFO_QUEUE_FILTER_DESC const *d3d_info_queue_filter_desc_10 )
{
    d3d_info_queue_filter_desc->NumCategories = d3d_info_queue_filter_desc_10->NumCategories;
    // BUGBUG - NEED CONVERSION FOR THIS POINTER PARAMETER: pCategoryList
    d3d_info_queue_filter_desc->NumSeverities = d3d_info_queue_filter_desc_10->NumSeverities;
    // BUGBUG - NEED CONVERSION FOR THIS POINTER PARAMETER: pSeverityList
    d3d_info_queue_filter_desc->NumIDs = d3d_info_queue_filter_desc_10->NumIDs;
    // BUGBUG - NEED CONVERSION FOR THIS POINTER PARAMETER: pIDList
}

void D3DWrapperConvertStruct( D3D10_INFO_QUEUE_FILTER_DESC *d3d_info_queue_filter_desc_10, D3D_INFO_QUEUE_FILTER_DESC const *d3d_info_queue_filter_desc )
{
    d3d_info_queue_filter_desc_10->NumCategories = d3d_info_queue_filter_desc->NumCategories;
    // BUGBUG - NEED CONVERSION FOR THIS POINTER PARAMETER: pCategoryList
    d3d_info_queue_filter_desc_10->NumSeverities = d3d_info_queue_filter_desc->NumSeverities;
    // BUGBUG - NEED CONVERSION FOR THIS POINTER PARAMETER: pSeverityList
    d3d_info_queue_filter_desc_10->NumIDs = d3d_info_queue_filter_desc->NumIDs;
    // BUGBUG - NEED CONVERSION FOR THIS POINTER PARAMETER: pIDList
}

void D3DWrapperConvertStruct( D3D_INFO_QUEUE_FILTER *d3d_info_queue_filter, D3D10_INFO_QUEUE_FILTER const *d3d_info_queue_filter_10 )
{
    D3DWrapperConvertStruct( &d3d_info_queue_filter->AllowList, &d3d_info_queue_filter_10->AllowList );
    D3DWrapperConvertStruct( &d3d_info_queue_filter->DenyList, &d3d_info_queue_filter_10->DenyList );
}

void D3DWrapperConvertStruct( D3D10_INFO_QUEUE_FILTER *d3d_info_queue_filter_10, D3D_INFO_QUEUE_FILTER const *d3d_info_queue_filter )
{
    D3DWrapperConvertStruct( &d3d_info_queue_filter_10->AllowList, &d3d_info_queue_filter->AllowList );
    D3DWrapperConvertStruct( &d3d_info_queue_filter_10->DenyList, &d3d_info_queue_filter->DenyList );
}

void D3DWrapperConvertStruct( D3D_RENDER_TARGET_BLEND_DESC1 *d3d_render_target_blend_desc1, D3D10_RENDER_TARGET_BLEND_DESC1 const *d3d_render_target_blend_desc1_10 )
{
    d3d_render_target_blend_desc1->BlendEnable = d3d_render_target_blend_desc1_10->BlendEnable;
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1->SrcBlend, d3d_render_target_blend_desc1_10->SrcBlend );
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1->DestBlend, d3d_render_target_blend_desc1_10->DestBlend );
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1->BlendOp, d3d_render_target_blend_desc1_10->BlendOp );
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1->SrcBlendAlpha, d3d_render_target_blend_desc1_10->SrcBlendAlpha );
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1->DestBlendAlpha, d3d_render_target_blend_desc1_10->DestBlendAlpha );
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1->BlendOpAlpha, d3d_render_target_blend_desc1_10->BlendOpAlpha );
    D3DFlagsConvertFrom10_COLOR_WRITE_ENABLE( &d3d_render_target_blend_desc1->RenderTargetWriteMask, d3d_render_target_blend_desc1_10->RenderTargetWriteMask );
}

void D3DWrapperConvertStruct( D3D10_RENDER_TARGET_BLEND_DESC1 *d3d_render_target_blend_desc1_10, D3D_RENDER_TARGET_BLEND_DESC1 const *d3d_render_target_blend_desc1 )
{
    d3d_render_target_blend_desc1_10->BlendEnable = d3d_render_target_blend_desc1->BlendEnable;
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1_10->SrcBlend, d3d_render_target_blend_desc1->SrcBlend );
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1_10->DestBlend, d3d_render_target_blend_desc1->DestBlend );
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1_10->BlendOp, d3d_render_target_blend_desc1->BlendOp );
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1_10->SrcBlendAlpha, d3d_render_target_blend_desc1->SrcBlendAlpha );
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1_10->DestBlendAlpha, d3d_render_target_blend_desc1->DestBlendAlpha );
    D3DWrapperConvertEnum( &d3d_render_target_blend_desc1_10->BlendOpAlpha, d3d_render_target_blend_desc1->BlendOpAlpha );
    D3DFlagsConvertTo10_COLOR_WRITE_ENABLE( &d3d_render_target_blend_desc1_10->RenderTargetWriteMask, d3d_render_target_blend_desc1->RenderTargetWriteMask );
}

void D3DWrapperConvertStruct( D3D_BLEND_DESC1 *d3d_blend_desc1, D3D10_BLEND_DESC1 const *d3d_blend_desc1_10 )
{
    d3d_blend_desc1->AlphaToCoverageEnable = d3d_blend_desc1_10->AlphaToCoverageEnable;
    d3d_blend_desc1->IndependentBlendEnable = d3d_blend_desc1_10->IndependentBlendEnable;
    D3DWrapperConvertStruct( &d3d_blend_desc1->RenderTarget[7], &d3d_blend_desc1_10->RenderTarget[7] );
    D3DWrapperConvertStruct( &d3d_blend_desc1->RenderTarget[6], &d3d_blend_desc1_10->RenderTarget[6] );
    D3DWrapperConvertStruct( &d3d_blend_desc1->RenderTarget[5], &d3d_blend_desc1_10->RenderTarget[5] );
    D3DWrapperConvertStruct( &d3d_blend_desc1->RenderTarget[4], &d3d_blend_desc1_10->RenderTarget[4] );
    D3DWrapperConvertStruct( &d3d_blend_desc1->RenderTarget[3], &d3d_blend_desc1_10->RenderTarget[3] );
    D3DWrapperConvertStruct( &d3d_blend_desc1->RenderTarget[2], &d3d_blend_desc1_10->RenderTarget[2] );
    D3DWrapperConvertStruct( &d3d_blend_desc1->RenderTarget[1], &d3d_blend_desc1_10->RenderTarget[1] );
    D3DWrapperConvertStruct( &d3d_blend_desc1->RenderTarget[0], &d3d_blend_desc1_10->RenderTarget[0] );
}

void D3DWrapperConvertStruct( D3D10_BLEND_DESC1 *d3d_blend_desc1_10, D3D_BLEND_DESC1 const *d3d_blend_desc1 )
{
    d3d_blend_desc1_10->AlphaToCoverageEnable = d3d_blend_desc1->AlphaToCoverageEnable;
    d3d_blend_desc1_10->IndependentBlendEnable = d3d_blend_desc1->IndependentBlendEnable;
    D3DWrapperConvertStruct( &d3d_blend_desc1_10->RenderTarget[7], &d3d_blend_desc1->RenderTarget[7] );
    D3DWrapperConvertStruct( &d3d_blend_desc1_10->RenderTarget[6], &d3d_blend_desc1->RenderTarget[6] );
    D3DWrapperConvertStruct( &d3d_blend_desc1_10->RenderTarget[5], &d3d_blend_desc1->RenderTarget[5] );
    D3DWrapperConvertStruct( &d3d_blend_desc1_10->RenderTarget[4], &d3d_blend_desc1->RenderTarget[4] );
    D3DWrapperConvertStruct( &d3d_blend_desc1_10->RenderTarget[3], &d3d_blend_desc1->RenderTarget[3] );
    D3DWrapperConvertStruct( &d3d_blend_desc1_10->RenderTarget[2], &d3d_blend_desc1->RenderTarget[2] );
    D3DWrapperConvertStruct( &d3d_blend_desc1_10->RenderTarget[1], &d3d_blend_desc1->RenderTarget[1] );
    D3DWrapperConvertStruct( &d3d_blend_desc1_10->RenderTarget[0], &d3d_blend_desc1->RenderTarget[0] );
}

void D3DWrapperConvertStruct( D3D_TEXCUBE_ARRAY_SRV1 *d3d_texcube_array_srv1, D3D10_TEXCUBE_ARRAY_SRV1 const *d3d_texcube_array_srv1_10 )
{
    d3d_texcube_array_srv1->MostDetailedMip = d3d_texcube_array_srv1_10->MostDetailedMip;
    d3d_texcube_array_srv1->MipLevels = d3d_texcube_array_srv1_10->MipLevels;
    d3d_texcube_array_srv1->First2DArrayFace = d3d_texcube_array_srv1_10->First2DArrayFace;
    d3d_texcube_array_srv1->NumCubes = d3d_texcube_array_srv1_10->NumCubes;
}

void D3DWrapperConvertStruct( D3D10_TEXCUBE_ARRAY_SRV1 *d3d_texcube_array_srv1_10, D3D_TEXCUBE_ARRAY_SRV1 const *d3d_texcube_array_srv1 )
{
    d3d_texcube_array_srv1_10->MostDetailedMip = d3d_texcube_array_srv1->MostDetailedMip;
    d3d_texcube_array_srv1_10->MipLevels = d3d_texcube_array_srv1->MipLevels;
    d3d_texcube_array_srv1_10->First2DArrayFace = d3d_texcube_array_srv1->First2DArrayFace;
    d3d_texcube_array_srv1_10->NumCubes = d3d_texcube_array_srv1->NumCubes;
}

void D3DWrapperConvertStruct( D3D_SHADER_RESOURCE_VIEW_DESC1 *d3d_shader_resource_view_desc1, D3D10_SHADER_RESOURCE_VIEW_DESC1 const *d3d_shader_resource_view_desc1_10 )
{
    d3d_shader_resource_view_desc1->Format = d3d_shader_resource_view_desc1_10->Format;
    D3DWrapperConvertEnum( &d3d_shader_resource_view_desc1->ViewDimension, d3d_shader_resource_view_desc1_10->ViewDimension );
    switch( d3d_shader_resource_view_desc1_10->ViewDimension )
    {
        case D3D10_1_SRV_DIMENSION_BUFFER:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1->Buffer, &d3d_shader_resource_view_desc1_10->Buffer );
            break;
        case D3D10_1_SRV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1->Texture1D, &d3d_shader_resource_view_desc1_10->Texture1D );
            break;
        case D3D10_1_SRV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1->Texture1DArray, &d3d_shader_resource_view_desc1_10->Texture1DArray );
            break;
        case D3D10_1_SRV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1->Texture2D, &d3d_shader_resource_view_desc1_10->Texture2D );
            break;
        case D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1->Texture2DArray, &d3d_shader_resource_view_desc1_10->Texture2DArray );
            break;
        case D3D10_1_SRV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1->Texture2DMS, &d3d_shader_resource_view_desc1_10->Texture2DMS );
            break;
        case D3D10_1_SRV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1->Texture2DMSArray, &d3d_shader_resource_view_desc1_10->Texture2DMSArray );
            break;
        case D3D10_1_SRV_DIMENSION_TEXTURE3D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1->Texture3D, &d3d_shader_resource_view_desc1_10->Texture3D );
            break;
        case D3D10_1_SRV_DIMENSION_TEXTURECUBE:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1->TextureCube, &d3d_shader_resource_view_desc1_10->TextureCube );
            break;
        case D3D10_1_SRV_DIMENSION_TEXTURECUBEARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1->TextureCubeArray, &d3d_shader_resource_view_desc1_10->TextureCubeArray );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D10_SHADER_RESOURCE_VIEW_DESC1 *d3d_shader_resource_view_desc1_10, D3D_SHADER_RESOURCE_VIEW_DESC1 const *d3d_shader_resource_view_desc1 )
{
    d3d_shader_resource_view_desc1_10->Format = d3d_shader_resource_view_desc1->Format;
    D3DWrapperConvertEnum( &d3d_shader_resource_view_desc1_10->ViewDimension, d3d_shader_resource_view_desc1->ViewDimension );
    switch( d3d_shader_resource_view_desc1->ViewDimension )
    {
        case D3D_SRV_DIMENSION_BUFFER:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1_10->Buffer, &d3d_shader_resource_view_desc1->Buffer );
            break;
        case D3D_SRV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1_10->Texture1D, &d3d_shader_resource_view_desc1->Texture1D );
            break;
        case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1_10->Texture1DArray, &d3d_shader_resource_view_desc1->Texture1DArray );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1_10->Texture2D, &d3d_shader_resource_view_desc1->Texture2D );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1_10->Texture2DArray, &d3d_shader_resource_view_desc1->Texture2DArray );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1_10->Texture2DMS, &d3d_shader_resource_view_desc1->Texture2DMS );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1_10->Texture2DMSArray, &d3d_shader_resource_view_desc1->Texture2DMSArray );
            break;
        case D3D_SRV_DIMENSION_TEXTURE3D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1_10->Texture3D, &d3d_shader_resource_view_desc1->Texture3D );
            break;
        case D3D_SRV_DIMENSION_TEXTURECUBE:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1_10->TextureCube, &d3d_shader_resource_view_desc1->TextureCube );
            break;
        case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc1_10->TextureCubeArray, &d3d_shader_resource_view_desc1->TextureCubeArray );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}


//
// Interface Conversions
//


////////////////////////////////////////////////////////////
//
// C10DeviceChildGeneric
//
////////////////////////////////////////////////////////////

class C10DeviceChildGeneric : public C10DeviceChild
{
    ID3D10DeviceChild *m_pDeviceChild;

public:
    C10DeviceChildGeneric( ID3D10DeviceChild *pDeviceChild );
    virtual ~C10DeviceChildGeneric();

    ////////////////////////////////////////////////////////////
    // Sub object accessor overrides.
    // These functions return NULL since this is a non-multi class.
    ////////////////////////////////////////////////////////////
    virtual int GetSubObjectCount() const { return 0; }
    void SetSubDeviceChild( int index, CDeviceChild *pDeviceChild ) {}
    virtual CDeviceChild *Src() { return NULL; }
    virtual CDeviceChild *Ref() { return NULL; }

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D10DeviceChild **pInterface ) { *pInterface = (ID3D10DeviceChild *) m_pDeviceChild; }
    virtual CDeviceChild *GetSubDeviceChild( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // GetDevice
    ////////////////////////////////////////////////////////////
    void GetDevice(  CDevice ** ppDevice );

    ////////////////////////////////////////////////////////////
    // GetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateDataInterface
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////
};


////////////////////////////////////////////////////////////
//
// C10DeviceChildGeneric
//
////////////////////////////////////////////////////////////

C10DeviceChildGeneric::C10DeviceChildGeneric( ID3D10DeviceChild *pDeviceChild ) : 
    m_pDeviceChild( pDeviceChild )
{
}


C10DeviceChildGeneric::~C10DeviceChildGeneric()
{
    if( m_pDeviceChild ) m_pDeviceChild->Release();
}


////////////////////////////////////////////////////////////
// C10DeviceChildGeneric::GetDevice()
////////////////////////////////////////////////////////////
void C10DeviceChildGeneric::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pDeviceChild );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pDeviceChild->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10DeviceChildGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10DeviceChildGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pDeviceChild );
    NRESULT rval = S_OK;
    rval = m_pDeviceChild->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10DeviceChildGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10DeviceChildGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pDeviceChild );
    NRESULT rval = S_OK;
    rval = m_pDeviceChild->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10DeviceChildGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10DeviceChildGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pDeviceChild );
    NRESULT rval = S_OK;
    rval = m_pDeviceChild->SetPrivateDataInterface(  guid,  pData );
    return rval;
}

void D3DWrapperConvertInterface( CDeviceChild **ppDeviceChild, ID3D10DeviceChild *pDeviceChild_10 )
{
    if( pDeviceChild_10 )
    {
        *ppDeviceChild = new C10DeviceChildGeneric( pDeviceChild_10 );
        if( !(*ppDeviceChild) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppDeviceChild)->AddRef();
    }
    else
    {
        *ppDeviceChild = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10DeviceChild **ppDeviceChild_10, CDeviceChild *pDeviceChild )
{
    if( pDeviceChild )
    {
        pDeviceChild->GetInterfacePtr( ppDeviceChild_10 );
    }
    else
    {
        *ppDeviceChild_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CDepthStencilState **ppDepthStencilState, ID3D10DepthStencilState *pDepthStencilState_10 )
{
    if( pDepthStencilState_10 )
    {
        *ppDepthStencilState = new C10DepthStencilState( pDepthStencilState_10 );
        if( !(*ppDepthStencilState) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppDepthStencilState)->AddRef();
    }
    else
    {
        *ppDepthStencilState = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10DepthStencilState **ppDepthStencilState_10, CDepthStencilState *pDepthStencilState )
{
    if( pDepthStencilState )
    {
        pDepthStencilState->GetInterfacePtr( ppDepthStencilState_10 );
    }
    else
    {
        *ppDepthStencilState_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CBlendState **ppBlendState, ID3D10BlendState *pBlendState_10 )
{
    if( pBlendState_10 )
    {
        *ppBlendState = new C10BlendState( pBlendState_10 );
        if( !(*ppBlendState) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppBlendState)->AddRef();
    }
    else
    {
        *ppBlendState = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10BlendState **ppBlendState_10, CBlendState *pBlendState )
{
    if( pBlendState )
    {
        pBlendState->GetInterfacePtr( ppBlendState_10 );
    }
    else
    {
        *ppBlendState_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CRasterizerState **ppRasterizerState, ID3D10RasterizerState *pRasterizerState_10 )
{
    if( pRasterizerState_10 )
    {
        *ppRasterizerState = new C10RasterizerState( pRasterizerState_10 );
        if( !(*ppRasterizerState) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppRasterizerState)->AddRef();
    }
    else
    {
        *ppRasterizerState = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10RasterizerState **ppRasterizerState_10, CRasterizerState *pRasterizerState )
{
    if( pRasterizerState )
    {
        pRasterizerState->GetInterfacePtr( ppRasterizerState_10 );
    }
    else
    {
        *ppRasterizerState_10 = NULL;
    }
}


////////////////////////////////////////////////////////////
//
// C10ResourceGeneric
//
////////////////////////////////////////////////////////////

class C10ResourceGeneric : public C10Resource
{
    ID3D10Resource *m_pResource;

public:
    C10ResourceGeneric( ID3D10Resource *pResource );
    virtual ~C10ResourceGeneric();

    ////////////////////////////////////////////////////////////
    // Sub object accessor overrides.
    // These functions return NULL since this is a non-multi class.
    ////////////////////////////////////////////////////////////
    virtual int GetSubObjectCount() const { return 0; }
    void SetSubResource( int index, CResource *pResource ) {}
    virtual CResource *Src() { return NULL; }
    virtual CResource *Ref() { return NULL; }

    //
    // CResource Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D10Resource **pInterface ) { *pInterface = (ID3D10Resource *) m_pResource; }
    virtual CResource *GetSubResource( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // GetType
    ////////////////////////////////////////////////////////////
    void GetType(  D3D_RESOURCE_DIMENSION * rType );

    ////////////////////////////////////////////////////////////
    // SetEvictionPriority
    ////////////////////////////////////////////////////////////
    void SetEvictionPriority(  UINT EvictionPriority );

    ////////////////////////////////////////////////////////////
    // GetEvictionPriority
    ////////////////////////////////////////////////////////////
    UINT GetEvictionPriority(  );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D10DeviceChild **pInterface ) { *pInterface = (ID3D10DeviceChild *) m_pResource; }
    virtual CDeviceChild *GetSubDeviceChild( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // GetDevice
    ////////////////////////////////////////////////////////////
    void GetDevice(  CDevice ** ppDevice );

    ////////////////////////////////////////////////////////////
    // GetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateDataInterface
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////
};


////////////////////////////////////////////////////////////
//
// C10ResourceGeneric
//
////////////////////////////////////////////////////////////

C10ResourceGeneric::C10ResourceGeneric( ID3D10Resource *pResource ) : 
    m_pResource( pResource )
{
}


C10ResourceGeneric::~C10ResourceGeneric()
{
    if( m_pResource ) m_pResource->Release();
}


////////////////////////////////////////////////////////////
// C10ResourceGeneric::GetType()
////////////////////////////////////////////////////////////
void C10ResourceGeneric::GetType(  D3D_RESOURCE_DIMENSION * rType )
{
    assert( m_pResource );
    D3D10_RESOURCE_DIMENSION temp_d3d10_resource_dimension;
    m_pResource->GetType(  &temp_d3d10_resource_dimension );
    D3DWrapperConvertEnum( rType, temp_d3d10_resource_dimension );
}


////////////////////////////////////////////////////////////
// C10ResourceGeneric::SetEvictionPriority()
////////////////////////////////////////////////////////////
void C10ResourceGeneric::SetEvictionPriority(  UINT EvictionPriority )
{
    assert( m_pResource );
    m_pResource->SetEvictionPriority(  EvictionPriority );
}


////////////////////////////////////////////////////////////
// C10ResourceGeneric::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT C10ResourceGeneric::GetEvictionPriority(  )
{
    assert( m_pResource );
    UINT rval = 0;
    rval = m_pResource->GetEvictionPriority(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10ResourceGeneric::GetDevice()
////////////////////////////////////////////////////////////
void C10ResourceGeneric::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pResource );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pResource->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10ResourceGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10ResourceGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pResource );
    NRESULT rval = S_OK;
    rval = m_pResource->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10ResourceGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10ResourceGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pResource );
    NRESULT rval = S_OK;
    rval = m_pResource->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10ResourceGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10ResourceGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pResource );
    NRESULT rval = S_OK;
    rval = m_pResource->SetPrivateDataInterface(  guid,  pData );
    return rval;
}

void D3DWrapperConvertInterface( CResource **ppResource, ID3D10Resource *pResource_10 )
{
    if( pResource_10 )
    {
        *ppResource = new C10ResourceGeneric( pResource_10 );
        if( !(*ppResource) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppResource)->AddRef();
    }
    else
    {
        *ppResource = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Resource **ppResource_10, CResource *pResource )
{
    if( pResource )
    {
        pResource->GetInterfacePtr( ppResource_10 );
    }
    else
    {
        *ppResource_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CBuffer **ppBuffer, ID3D10Buffer *pBuffer_10 )
{
    if( pBuffer_10 )
    {
        *ppBuffer = new C10Buffer( pBuffer_10 );
        if( !(*ppBuffer) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppBuffer)->AddRef();
    }
    else
    {
        *ppBuffer = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Buffer **ppBuffer_10, CBuffer *pBuffer )
{
    if( pBuffer )
    {
        pBuffer->GetInterfacePtr( ppBuffer_10 );
    }
    else
    {
        *ppBuffer_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CTexture1D **ppTexture1D, ID3D10Texture1D *pTexture1D_10 )
{
    if( pTexture1D_10 )
    {
        *ppTexture1D = new C10Texture1D( pTexture1D_10 );
        if( !(*ppTexture1D) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppTexture1D)->AddRef();
    }
    else
    {
        *ppTexture1D = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Texture1D **ppTexture1D_10, CTexture1D *pTexture1D )
{
    if( pTexture1D )
    {
        pTexture1D->GetInterfacePtr( ppTexture1D_10 );
    }
    else
    {
        *ppTexture1D_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CTexture2D **ppTexture2D, ID3D10Texture2D *pTexture2D_10 )
{
    if( pTexture2D_10 )
    {
        *ppTexture2D = new C10Texture2D( pTexture2D_10 );
        if( !(*ppTexture2D) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppTexture2D)->AddRef();
    }
    else
    {
        *ppTexture2D = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Texture2D **ppTexture2D_10, CTexture2D *pTexture2D )
{
    if( pTexture2D )
    {
        pTexture2D->GetInterfacePtr( ppTexture2D_10 );
    }
    else
    {
        *ppTexture2D_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CTexture3D **ppTexture3D, ID3D10Texture3D *pTexture3D_10 )
{
    if( pTexture3D_10 )
    {
        *ppTexture3D = new C10Texture3D( pTexture3D_10 );
        if( !(*ppTexture3D) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppTexture3D)->AddRef();
    }
    else
    {
        *ppTexture3D = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Texture3D **ppTexture3D_10, CTexture3D *pTexture3D )
{
    if( pTexture3D )
    {
        pTexture3D->GetInterfacePtr( ppTexture3D_10 );
    }
    else
    {
        *ppTexture3D_10 = NULL;
    }
}


////////////////////////////////////////////////////////////
//
// C10ViewGeneric
//
////////////////////////////////////////////////////////////

class C10ViewGeneric : public C10View
{
    ID3D10View *m_pView;

public:
    C10ViewGeneric( ID3D10View *pView );
    virtual ~C10ViewGeneric();

    ////////////////////////////////////////////////////////////
    // Sub object accessor overrides.
    // These functions return NULL since this is a non-multi class.
    ////////////////////////////////////////////////////////////
    virtual int GetSubObjectCount() const { return 0; }
    void SetSubView( int index, CView *pView ) {}
    virtual CView *Src() { return NULL; }
    virtual CView *Ref() { return NULL; }

    //
    // CView Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D10View **pInterface ) { *pInterface = (ID3D10View *) m_pView; }
    virtual CView *GetSubView( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // GetResource
    ////////////////////////////////////////////////////////////
    void GetResource(  CResource ** ppResource );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D10DeviceChild **pInterface ) { *pInterface = (ID3D10DeviceChild *) m_pView; }
    virtual CDeviceChild *GetSubDeviceChild( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // GetDevice
    ////////////////////////////////////////////////////////////
    void GetDevice(  CDevice ** ppDevice );

    ////////////////////////////////////////////////////////////
    // GetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateDataInterface
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////
};


////////////////////////////////////////////////////////////
//
// C10ViewGeneric
//
////////////////////////////////////////////////////////////

C10ViewGeneric::C10ViewGeneric( ID3D10View *pView ) : 
    m_pView( pView )
{
}


C10ViewGeneric::~C10ViewGeneric()
{
    if( m_pView ) m_pView->Release();
}


////////////////////////////////////////////////////////////
// C10ViewGeneric::GetResource()
////////////////////////////////////////////////////////////
void C10ViewGeneric::GetResource(  CResource ** ppResource )
{
    assert( m_pView );
    ID3D10Resource *temp_pResource = NULL;
    ID3D10Resource **ppResourceTemp = ppResource == NULL ? NULL : &temp_pResource;
    m_pView->GetResource(  ppResourceTemp );
    if( ppResource != NULL )
    {
        D3DWrapperConvertInterface( ppResource, *ppResourceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10ViewGeneric::GetDevice()
////////////////////////////////////////////////////////////
void C10ViewGeneric::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pView );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pView->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10ViewGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10ViewGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pView );
    NRESULT rval = S_OK;
    rval = m_pView->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10ViewGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10ViewGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pView );
    NRESULT rval = S_OK;
    rval = m_pView->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10ViewGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10ViewGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pView );
    NRESULT rval = S_OK;
    rval = m_pView->SetPrivateDataInterface(  guid,  pData );
    return rval;
}

void D3DWrapperConvertInterface( CView **ppView, ID3D10View *pView_10 )
{
    if( pView_10 )
    {
        *ppView = new C10ViewGeneric( pView_10 );
        if( !(*ppView) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppView)->AddRef();
    }
    else
    {
        *ppView = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10View **ppView_10, CView *pView )
{
    if( pView )
    {
        pView->GetInterfacePtr( ppView_10 );
    }
    else
    {
        *ppView_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CShaderResourceView **ppShaderResourceView, ID3D10ShaderResourceView *pShaderResourceView_10 )
{
    if( pShaderResourceView_10 )
    {
        *ppShaderResourceView = new C10ShaderResourceView( pShaderResourceView_10 );
        if( !(*ppShaderResourceView) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppShaderResourceView)->AddRef();
    }
    else
    {
        *ppShaderResourceView = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10ShaderResourceView **ppShaderResourceView_10, CShaderResourceView *pShaderResourceView )
{
    if( pShaderResourceView )
    {
        pShaderResourceView->GetInterfacePtr( ppShaderResourceView_10 );
    }
    else
    {
        *ppShaderResourceView_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CRenderTargetView **ppRenderTargetView, ID3D10RenderTargetView *pRenderTargetView_10 )
{
    if( pRenderTargetView_10 )
    {
        *ppRenderTargetView = new C10RenderTargetView( pRenderTargetView_10 );
        if( !(*ppRenderTargetView) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppRenderTargetView)->AddRef();
    }
    else
    {
        *ppRenderTargetView = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10RenderTargetView **ppRenderTargetView_10, CRenderTargetView *pRenderTargetView )
{
    if( pRenderTargetView )
    {
        pRenderTargetView->GetInterfacePtr( ppRenderTargetView_10 );
    }
    else
    {
        *ppRenderTargetView_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CDepthStencilView **ppDepthStencilView, ID3D10DepthStencilView *pDepthStencilView_10 )
{
    if( pDepthStencilView_10 )
    {
        *ppDepthStencilView = new C10DepthStencilView( pDepthStencilView_10 );
        if( !(*ppDepthStencilView) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppDepthStencilView)->AddRef();
    }
    else
    {
        *ppDepthStencilView = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10DepthStencilView **ppDepthStencilView_10, CDepthStencilView *pDepthStencilView )
{
    if( pDepthStencilView )
    {
        pDepthStencilView->GetInterfacePtr( ppDepthStencilView_10 );
    }
    else
    {
        *ppDepthStencilView_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CVertexShader **ppVertexShader, ID3D10VertexShader *pVertexShader_10 )
{
    if( pVertexShader_10 )
    {
        *ppVertexShader = new C10VertexShader( pVertexShader_10 );
        if( !(*ppVertexShader) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppVertexShader)->AddRef();
    }
    else
    {
        *ppVertexShader = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10VertexShader **ppVertexShader_10, CVertexShader *pVertexShader )
{
    if( pVertexShader )
    {
        pVertexShader->GetInterfacePtr( ppVertexShader_10 );
    }
    else
    {
        *ppVertexShader_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CGeometryShader **ppGeometryShader, ID3D10GeometryShader *pGeometryShader_10 )
{
    if( pGeometryShader_10 )
    {
        *ppGeometryShader = new C10GeometryShader( pGeometryShader_10 );
        if( !(*ppGeometryShader) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppGeometryShader)->AddRef();
    }
    else
    {
        *ppGeometryShader = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10GeometryShader **ppGeometryShader_10, CGeometryShader *pGeometryShader )
{
    if( pGeometryShader )
    {
        pGeometryShader->GetInterfacePtr( ppGeometryShader_10 );
    }
    else
    {
        *ppGeometryShader_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CPixelShader **ppPixelShader, ID3D10PixelShader *pPixelShader_10 )
{
    if( pPixelShader_10 )
    {
        *ppPixelShader = new C10PixelShader( pPixelShader_10 );
        if( !(*ppPixelShader) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppPixelShader)->AddRef();
    }
    else
    {
        *ppPixelShader = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10PixelShader **ppPixelShader_10, CPixelShader *pPixelShader )
{
    if( pPixelShader )
    {
        pPixelShader->GetInterfacePtr( ppPixelShader_10 );
    }
    else
    {
        *ppPixelShader_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CInputLayout **ppInputLayout, ID3D10InputLayout *pInputLayout_10 )
{
    if( pInputLayout_10 )
    {
        *ppInputLayout = new C10InputLayout( pInputLayout_10 );
        if( !(*ppInputLayout) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppInputLayout)->AddRef();
    }
    else
    {
        *ppInputLayout = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10InputLayout **ppInputLayout_10, CInputLayout *pInputLayout )
{
    if( pInputLayout )
    {
        pInputLayout->GetInterfacePtr( ppInputLayout_10 );
    }
    else
    {
        *ppInputLayout_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CSamplerState **ppSamplerState, ID3D10SamplerState *pSamplerState_10 )
{
    if( pSamplerState_10 )
    {
        *ppSamplerState = new C10SamplerState( pSamplerState_10 );
        if( !(*ppSamplerState) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppSamplerState)->AddRef();
    }
    else
    {
        *ppSamplerState = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10SamplerState **ppSamplerState_10, CSamplerState *pSamplerState )
{
    if( pSamplerState )
    {
        pSamplerState->GetInterfacePtr( ppSamplerState_10 );
    }
    else
    {
        *ppSamplerState_10 = NULL;
    }
}


////////////////////////////////////////////////////////////
//
// C10AsynchronousGeneric
//
////////////////////////////////////////////////////////////

class C10AsynchronousGeneric : public C10Asynchronous
{
    ID3D10Asynchronous *m_pAsynchronous;

public:
    C10AsynchronousGeneric( ID3D10Asynchronous *pAsynchronous );
    virtual ~C10AsynchronousGeneric();

    ////////////////////////////////////////////////////////////
    // Sub object accessor overrides.
    // These functions return NULL since this is a non-multi class.
    ////////////////////////////////////////////////////////////
    virtual int GetSubObjectCount() const { return 0; }
    void SetSubAsynchronous( int index, CAsynchronous *pAsynchronous ) {}
    virtual CAsynchronous *Src() { return NULL; }
    virtual CAsynchronous *Ref() { return NULL; }

    //
    // CAsynchronous Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D10Asynchronous **pInterface ) { *pInterface = (ID3D10Asynchronous *) m_pAsynchronous; }
    virtual CAsynchronous *GetSubAsynchronous( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // Begin
    ////////////////////////////////////////////////////////////
    void Begin(  );

    ////////////////////////////////////////////////////////////
    // End
    ////////////////////////////////////////////////////////////
    void End(  );

    ////////////////////////////////////////////////////////////
    // GetData
    ////////////////////////////////////////////////////////////
    NRESULT GetData(  void * pData, UINT DataSize, UINT GetDataFlags );

    ////////////////////////////////////////////////////////////
    // GetDataSize
    ////////////////////////////////////////////////////////////
    UINT GetDataSize(  );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D10DeviceChild **pInterface ) { *pInterface = (ID3D10DeviceChild *) m_pAsynchronous; }
    virtual CDeviceChild *GetSubDeviceChild( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // GetDevice
    ////////////////////////////////////////////////////////////
    void GetDevice(  CDevice ** ppDevice );

    ////////////////////////////////////////////////////////////
    // GetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateDataInterface
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////
};


////////////////////////////////////////////////////////////
//
// C10AsynchronousGeneric
//
////////////////////////////////////////////////////////////

C10AsynchronousGeneric::C10AsynchronousGeneric( ID3D10Asynchronous *pAsynchronous ) : 
    m_pAsynchronous( pAsynchronous )
{
}


C10AsynchronousGeneric::~C10AsynchronousGeneric()
{
    if( m_pAsynchronous ) m_pAsynchronous->Release();
}


////////////////////////////////////////////////////////////
// C10AsynchronousGeneric::Begin()
////////////////////////////////////////////////////////////
void C10AsynchronousGeneric::Begin(  )
{
    assert( m_pAsynchronous );
    m_pAsynchronous->Begin(  );
}


////////////////////////////////////////////////////////////
// C10AsynchronousGeneric::End()
////////////////////////////////////////////////////////////
void C10AsynchronousGeneric::End(  )
{
    assert( m_pAsynchronous );
    m_pAsynchronous->End(  );
}


////////////////////////////////////////////////////////////
// C10AsynchronousGeneric::GetData()
////////////////////////////////////////////////////////////
NRESULT C10AsynchronousGeneric::GetData(  void * pData, UINT DataSize, UINT GetDataFlags )
{
    assert( m_pAsynchronous );
    NRESULT rval = S_OK;
    UINT temp_GetDataFlags;
    D3DFlagsConvertTo10_ASYNC_GETDATA_FLAG( &temp_GetDataFlags, GetDataFlags );
    rval = m_pAsynchronous->GetData(  pData,  DataSize,  temp_GetDataFlags );
    return rval;
}


////////////////////////////////////////////////////////////
// C10AsynchronousGeneric::GetDataSize()
////////////////////////////////////////////////////////////
UINT C10AsynchronousGeneric::GetDataSize(  )
{
    assert( m_pAsynchronous );
    UINT rval = 0;
    rval = m_pAsynchronous->GetDataSize(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10AsynchronousGeneric::GetDevice()
////////////////////////////////////////////////////////////
void C10AsynchronousGeneric::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pAsynchronous );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pAsynchronous->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10AsynchronousGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10AsynchronousGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pAsynchronous );
    NRESULT rval = S_OK;
    rval = m_pAsynchronous->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10AsynchronousGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10AsynchronousGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pAsynchronous );
    NRESULT rval = S_OK;
    rval = m_pAsynchronous->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10AsynchronousGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10AsynchronousGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pAsynchronous );
    NRESULT rval = S_OK;
    rval = m_pAsynchronous->SetPrivateDataInterface(  guid,  pData );
    return rval;
}

void D3DWrapperConvertInterface( CAsynchronous **ppAsynchronous, ID3D10Asynchronous *pAsynchronous_10 )
{
    if( pAsynchronous_10 )
    {
        *ppAsynchronous = new C10AsynchronousGeneric( pAsynchronous_10 );
        if( !(*ppAsynchronous) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppAsynchronous)->AddRef();
    }
    else
    {
        *ppAsynchronous = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Asynchronous **ppAsynchronous_10, CAsynchronous *pAsynchronous )
{
    if( pAsynchronous )
    {
        pAsynchronous->GetInterfacePtr( ppAsynchronous_10 );
    }
    else
    {
        *ppAsynchronous_10 = NULL;
    }
}


////////////////////////////////////////////////////////////
//
// C10QueryGeneric
//
////////////////////////////////////////////////////////////

class C10QueryGeneric : public C10Query
{
    ID3D10Query *m_pQuery;

public:
    C10QueryGeneric( ID3D10Query *pQuery );
    virtual ~C10QueryGeneric();

    ////////////////////////////////////////////////////////////
    // Sub object accessor overrides.
    // These functions return NULL since this is a non-multi class.
    ////////////////////////////////////////////////////////////
    virtual int GetSubObjectCount() const { return 0; }
    void SetSubQuery( int index, CQuery *pQuery ) {}
    virtual CQuery *Src() { return NULL; }
    virtual CQuery *Ref() { return NULL; }

    //
    // CQuery Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D10Query **pInterface ) { *pInterface = (ID3D10Query *) m_pQuery; }
    virtual CQuery *GetSubQuery( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // GetDesc
    ////////////////////////////////////////////////////////////
    void GetDesc(  D3D_QUERY_DESC * pDesc );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CAsynchronous Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D10Asynchronous **pInterface ) { *pInterface = (ID3D10Asynchronous *) m_pQuery; }
    virtual CAsynchronous *GetSubAsynchronous( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // Begin
    ////////////////////////////////////////////////////////////
    void Begin(  );

    ////////////////////////////////////////////////////////////
    // End
    ////////////////////////////////////////////////////////////
    void End(  );

    ////////////////////////////////////////////////////////////
    // GetData
    ////////////////////////////////////////////////////////////
    NRESULT GetData(  void * pData, UINT DataSize, UINT GetDataFlags );

    ////////////////////////////////////////////////////////////
    // GetDataSize
    ////////////////////////////////////////////////////////////
    UINT GetDataSize(  );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D10DeviceChild **pInterface ) { *pInterface = (ID3D10DeviceChild *) m_pQuery; }
    virtual CDeviceChild *GetSubDeviceChild( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // GetDevice
    ////////////////////////////////////////////////////////////
    void GetDevice(  CDevice ** ppDevice );

    ////////////////////////////////////////////////////////////
    // GetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateData
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData );

    ////////////////////////////////////////////////////////////
    // SetPrivateDataInterface
    ////////////////////////////////////////////////////////////
    NRESULT SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////
};


////////////////////////////////////////////////////////////
//
// C10QueryGeneric
//
////////////////////////////////////////////////////////////

C10QueryGeneric::C10QueryGeneric( ID3D10Query *pQuery ) : 
    m_pQuery( pQuery )
{
}


C10QueryGeneric::~C10QueryGeneric()
{
    if( m_pQuery ) m_pQuery->Release();
}


////////////////////////////////////////////////////////////
// C10QueryGeneric::GetDesc()
////////////////////////////////////////////////////////////
void C10QueryGeneric::GetDesc(  D3D_QUERY_DESC * pDesc )
{
    assert( m_pQuery );
    BYTE *pDescByteTemp = NULL;
    D3D10_QUERY_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D10_QUERY_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D10_QUERY_DESC*)pDescByteTemp;
    }
    m_pQuery->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
}


////////////////////////////////////////////////////////////
// C10QueryGeneric::Begin()
////////////////////////////////////////////////////////////
void C10QueryGeneric::Begin(  )
{
    assert( m_pQuery );
    m_pQuery->Begin(  );
}


////////////////////////////////////////////////////////////
// C10QueryGeneric::End()
////////////////////////////////////////////////////////////
void C10QueryGeneric::End(  )
{
    assert( m_pQuery );
    m_pQuery->End(  );
}


////////////////////////////////////////////////////////////
// C10QueryGeneric::GetData()
////////////////////////////////////////////////////////////
NRESULT C10QueryGeneric::GetData(  void * pData, UINT DataSize, UINT GetDataFlags )
{
    assert( m_pQuery );
    NRESULT rval = S_OK;
    UINT temp_GetDataFlags;
    D3DFlagsConvertTo10_ASYNC_GETDATA_FLAG( &temp_GetDataFlags, GetDataFlags );
    rval = m_pQuery->GetData(  pData,  DataSize,  temp_GetDataFlags );
    return rval;
}


////////////////////////////////////////////////////////////
// C10QueryGeneric::GetDataSize()
////////////////////////////////////////////////////////////
UINT C10QueryGeneric::GetDataSize(  )
{
    assert( m_pQuery );
    UINT rval = 0;
    rval = m_pQuery->GetDataSize(  );
    return rval;
}


////////////////////////////////////////////////////////////
// C10QueryGeneric::GetDevice()
////////////////////////////////////////////////////////////
void C10QueryGeneric::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pQuery );
    ID3D10Device *temp_pDevice = NULL;
    ID3D10Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pQuery->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
}


////////////////////////////////////////////////////////////
// C10QueryGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10QueryGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pQuery );
    NRESULT rval = S_OK;
    rval = m_pQuery->GetPrivateData(  guid,  pDataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10QueryGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C10QueryGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pQuery );
    NRESULT rval = S_OK;
    rval = m_pQuery->SetPrivateData(  guid,  DataSize,  pData );
    return rval;
}


////////////////////////////////////////////////////////////
// C10QueryGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C10QueryGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pQuery );
    NRESULT rval = S_OK;
    rval = m_pQuery->SetPrivateDataInterface(  guid,  pData );
    return rval;
}

void D3DWrapperConvertInterface( CQuery **ppQuery, ID3D10Query *pQuery_10 )
{
    if( pQuery_10 )
    {
        *ppQuery = new C10QueryGeneric( pQuery_10 );
        if( !(*ppQuery) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppQuery)->AddRef();
    }
    else
    {
        *ppQuery = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Query **ppQuery_10, CQuery *pQuery )
{
    if( pQuery )
    {
        pQuery->GetInterfacePtr( ppQuery_10 );
    }
    else
    {
        *ppQuery_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CPredicate **ppPredicate, ID3D10Predicate *pPredicate_10 )
{
    if( pPredicate_10 )
    {
        *ppPredicate = new C10Predicate( pPredicate_10 );
        if( !(*ppPredicate) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppPredicate)->AddRef();
    }
    else
    {
        *ppPredicate = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Predicate **ppPredicate_10, CPredicate *pPredicate )
{
    if( pPredicate )
    {
        pPredicate->GetInterfacePtr( ppPredicate_10 );
    }
    else
    {
        *ppPredicate_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CCounter **ppCounter, ID3D10Counter *pCounter_10 )
{
    if( pCounter_10 )
    {
        *ppCounter = new C10Counter( pCounter_10 );
        if( !(*ppCounter) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppCounter)->AddRef();
    }
    else
    {
        *ppCounter = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Counter **ppCounter_10, CCounter *pCounter )
{
    if( pCounter )
    {
        pCounter->GetInterfacePtr( ppCounter_10 );
    }
    else
    {
        *ppCounter_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CDevice **ppDevice, ID3D10Device *pDevice_10 )
{
    if( pDevice_10 )
    {
        *ppDevice = new C10Device( pDevice_10 );
        if( !(*ppDevice) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppDevice)->AddRef();
    }
    else
    {
        *ppDevice = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Device **ppDevice_10, CDevice *pDevice )
{
    if( pDevice )
    {
        pDevice->GetInterfacePtr( ppDevice_10 );
    }
    else
    {
        *ppDevice_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CMultithread **ppMultithread, ID3D10Multithread *pMultithread_10 )
{
    if( pMultithread_10 )
    {
        *ppMultithread = new C10Multithread( pMultithread_10 );
        if( !(*ppMultithread) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppMultithread)->AddRef();
    }
    else
    {
        *ppMultithread = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Multithread **ppMultithread_10, CMultithread *pMultithread )
{
    if( pMultithread )
    {
        pMultithread->GetInterfacePtr( ppMultithread_10 );
    }
    else
    {
        *ppMultithread_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CDebug **ppDebug, ID3D10Debug *pDebug_10 )
{
    if( pDebug_10 )
    {
        *ppDebug = new C10Debug( pDebug_10 );
        if( !(*ppDebug) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppDebug)->AddRef();
    }
    else
    {
        *ppDebug = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Debug **ppDebug_10, CDebug *pDebug )
{
    if( pDebug )
    {
        pDebug->GetInterfacePtr( ppDebug_10 );
    }
    else
    {
        *ppDebug_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CSwitchToRef **ppSwitchToRef, ID3D10SwitchToRef *pSwitchToRef_10 )
{
    if( pSwitchToRef_10 )
    {
        *ppSwitchToRef = new C10SwitchToRef( pSwitchToRef_10 );
        if( !(*ppSwitchToRef) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppSwitchToRef)->AddRef();
    }
    else
    {
        *ppSwitchToRef = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10SwitchToRef **ppSwitchToRef_10, CSwitchToRef *pSwitchToRef )
{
    if( pSwitchToRef )
    {
        pSwitchToRef->GetInterfacePtr( ppSwitchToRef_10 );
    }
    else
    {
        *ppSwitchToRef_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CInfoQueue **ppInfoQueue, ID3D10InfoQueue *pInfoQueue_10 )
{
    if( pInfoQueue_10 )
    {
        *ppInfoQueue = new C10InfoQueue( pInfoQueue_10 );
        if( !(*ppInfoQueue) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppInfoQueue)->AddRef();
    }
    else
    {
        *ppInfoQueue = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10InfoQueue **ppInfoQueue_10, CInfoQueue *pInfoQueue )
{
    if( pInfoQueue )
    {
        pInfoQueue->GetInterfacePtr( ppInfoQueue_10 );
    }
    else
    {
        *ppInfoQueue_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CBlob **ppBlob, ID3D10Blob *pBlob_10 )
{
    if( pBlob_10 )
    {
        *ppBlob = new C10Blob( pBlob_10 );
        if( !(*ppBlob) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppBlob)->AddRef();
    }
    else
    {
        *ppBlob = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Blob **ppBlob_10, CBlob *pBlob )
{
    if( pBlob )
    {
        pBlob->GetInterfacePtr( ppBlob_10 );
    }
    else
    {
        *ppBlob_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CBlendState1 **ppBlendState1, ID3D10BlendState1 *pBlendState1_10 )
{
    if( pBlendState1_10 )
    {
        *ppBlendState1 = new C10BlendState1( pBlendState1_10 );
        if( !(*ppBlendState1) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppBlendState1)->AddRef();
    }
    else
    {
        *ppBlendState1 = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10BlendState1 **ppBlendState1_10, CBlendState1 *pBlendState1 )
{
    if( pBlendState1 )
    {
        pBlendState1->GetInterfacePtr( ppBlendState1_10 );
    }
    else
    {
        *ppBlendState1_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CShaderResourceView1 **ppShaderResourceView1, ID3D10ShaderResourceView1 *pShaderResourceView1_10 )
{
    if( pShaderResourceView1_10 )
    {
        *ppShaderResourceView1 = new C10ShaderResourceView1( pShaderResourceView1_10 );
        if( !(*ppShaderResourceView1) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppShaderResourceView1)->AddRef();
    }
    else
    {
        *ppShaderResourceView1 = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10ShaderResourceView1 **ppShaderResourceView1_10, CShaderResourceView1 *pShaderResourceView1 )
{
    if( pShaderResourceView1 )
    {
        pShaderResourceView1->GetInterfacePtr( ppShaderResourceView1_10 );
    }
    else
    {
        *ppShaderResourceView1_10 = NULL;
    }
}

void D3DWrapperConvertInterface( CDevice **ppDevice1, ID3D10Device1 *pDevice1_10 )
{
    if( pDevice1_10 )
    {
        *ppDevice1 = new C10Device( pDevice1_10 );
        if( !(*ppDevice1) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppDevice1)->AddRef();
    }
    else
    {
        *ppDevice1 = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D10Device1 **ppDevice1_10, CDevice *pDevice1 )
{
    if( pDevice1 )
    {
        pDevice1->GetInterfacePtr( ppDevice1_10 );
    }
    else
    {
        *ppDevice1_10 = NULL;
    }
}
