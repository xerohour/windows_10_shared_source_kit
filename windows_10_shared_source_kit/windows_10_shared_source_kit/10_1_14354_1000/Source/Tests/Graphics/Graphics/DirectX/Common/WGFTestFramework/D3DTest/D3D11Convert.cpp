//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D11Convert.cpp
//* 
//* Description: 
//*     Interface specific D3D wrapper class declarations.
//*
//*****************************************************************************


#include "pch.h"
#include "D3D11Wrapped.h"
#include "D3D11Convert.h"


////////////////////////////////////////////////////////////
//
// Conversion Functions
//
////////////////////////////////////////////////////////////


//
// Enum Conversions
//

void D3DWrapperConvertEnum( D3D_INPUT_CLASSIFICATION *d3d_input_classification, D3D11_INPUT_CLASSIFICATION d3d_input_classification_11 )
{
    switch( d3d_input_classification_11 )
    {
        default:
            *d3d_input_classification = (D3D_INPUT_CLASSIFICATION)d3d_input_classification_11;
            break;
        case D3D11_INPUT_PER_VERTEX_DATA:
            *d3d_input_classification = D3D_INPUT_PER_VERTEX_DATA;
            break;
        case D3D11_INPUT_PER_INSTANCE_DATA:
            *d3d_input_classification = D3D_INPUT_PER_INSTANCE_DATA;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_INPUT_CLASSIFICATION *d3d_input_classification_11, D3D_INPUT_CLASSIFICATION d3d_input_classification )
{
    switch( d3d_input_classification )
    {
        default:
            *d3d_input_classification_11 = (D3D11_INPUT_CLASSIFICATION)d3d_input_classification;
            break;
        case D3D_INPUT_PER_VERTEX_DATA:
            *d3d_input_classification_11 = D3D11_INPUT_PER_VERTEX_DATA;
            break;
        case D3D_INPUT_PER_INSTANCE_DATA:
            *d3d_input_classification_11 = D3D11_INPUT_PER_INSTANCE_DATA;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_FILL_MODE *d3d_fill_mode, D3D11_FILL_MODE d3d_fill_mode_11 )
{
    switch( d3d_fill_mode_11 )
    {
        default:
            *d3d_fill_mode = (D3D_FILL_MODE)d3d_fill_mode_11;
            break;
        case D3D11_FILL_WIREFRAME:
            *d3d_fill_mode = D3D_FILL_WIREFRAME;
            break;
        case D3D11_FILL_SOLID:
            *d3d_fill_mode = D3D_FILL_SOLID;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_FILL_MODE *d3d_fill_mode_11, D3D_FILL_MODE d3d_fill_mode )
{
    switch( d3d_fill_mode )
    {
        default:
            *d3d_fill_mode_11 = (D3D11_FILL_MODE)d3d_fill_mode;
            break;
        case D3D_FILL_WIREFRAME:
            *d3d_fill_mode_11 = D3D11_FILL_WIREFRAME;
            break;
        case D3D_FILL_SOLID:
            *d3d_fill_mode_11 = D3D11_FILL_SOLID;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_CULL_MODE *d3d_cull_mode, D3D11_CULL_MODE d3d_cull_mode_11 )
{
    switch( d3d_cull_mode_11 )
    {
        default:
            *d3d_cull_mode = (D3D_CULL_MODE)d3d_cull_mode_11;
            break;
        case D3D11_CULL_NONE:
            *d3d_cull_mode = D3D_CULL_NONE;
            break;
        case D3D11_CULL_FRONT:
            *d3d_cull_mode = D3D_CULL_FRONT;
            break;
        case D3D11_CULL_BACK:
            *d3d_cull_mode = D3D_CULL_BACK;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_CULL_MODE *d3d_cull_mode_11, D3D_CULL_MODE d3d_cull_mode )
{
    switch( d3d_cull_mode )
    {
        default:
            *d3d_cull_mode_11 = (D3D11_CULL_MODE)d3d_cull_mode;
            break;
        case D3D_CULL_NONE:
            *d3d_cull_mode_11 = D3D11_CULL_NONE;
            break;
        case D3D_CULL_FRONT:
            *d3d_cull_mode_11 = D3D11_CULL_FRONT;
            break;
        case D3D_CULL_BACK:
            *d3d_cull_mode_11 = D3D11_CULL_BACK;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_RESOURCE_DIMENSION *d3d_resource_dimension, D3D11_RESOURCE_DIMENSION d3d_resource_dimension_11 )
{
    switch( d3d_resource_dimension_11 )
    {
        default:
            *d3d_resource_dimension = (D3D_RESOURCE_DIMENSION)d3d_resource_dimension_11;
            break;
        case D3D11_RESOURCE_DIMENSION_UNKNOWN:
            *d3d_resource_dimension = D3D_RESOURCE_DIMENSION_UNKNOWN;
            break;
        case D3D11_RESOURCE_DIMENSION_BUFFER:
            *d3d_resource_dimension = D3D_RESOURCE_DIMENSION_BUFFER;
            break;
        case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
            *d3d_resource_dimension = D3D_RESOURCE_DIMENSION_TEXTURE1D;
            break;
        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
            *d3d_resource_dimension = D3D_RESOURCE_DIMENSION_TEXTURE2D;
            break;
        case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
            *d3d_resource_dimension = D3D_RESOURCE_DIMENSION_TEXTURE3D;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_RESOURCE_DIMENSION *d3d_resource_dimension_11, D3D_RESOURCE_DIMENSION d3d_resource_dimension )
{
    switch( d3d_resource_dimension )
    {
        default:
            *d3d_resource_dimension_11 = (D3D11_RESOURCE_DIMENSION)d3d_resource_dimension;
            break;
        case D3D_RESOURCE_DIMENSION_UNKNOWN:
            *d3d_resource_dimension_11 = D3D11_RESOURCE_DIMENSION_UNKNOWN;
            break;
        case D3D_RESOURCE_DIMENSION_BUFFER:
            *d3d_resource_dimension_11 = D3D11_RESOURCE_DIMENSION_BUFFER;
            break;
        case D3D_RESOURCE_DIMENSION_TEXTURE1D:
            *d3d_resource_dimension_11 = D3D11_RESOURCE_DIMENSION_TEXTURE1D;
            break;
        case D3D_RESOURCE_DIMENSION_TEXTURE2D:
            *d3d_resource_dimension_11 = D3D11_RESOURCE_DIMENSION_TEXTURE2D;
            break;
        case D3D_RESOURCE_DIMENSION_TEXTURE3D:
            *d3d_resource_dimension_11 = D3D11_RESOURCE_DIMENSION_TEXTURE3D;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_DSV_DIMENSION *d3d_dsv_dimension, D3D11_DSV_DIMENSION d3d_dsv_dimension_11 )
{
    switch( d3d_dsv_dimension_11 )
    {
        default:
            *d3d_dsv_dimension = (D3D_DSV_DIMENSION)d3d_dsv_dimension_11;
            break;
        case D3D11_DSV_DIMENSION_UNKNOWN:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_UNKNOWN;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE1D:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE1D;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE1DARRAY;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2D:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE2D;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE2DARRAY;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DMS:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE2DMS;
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
            *d3d_dsv_dimension = D3D_DSV_DIMENSION_TEXTURE2DMSARRAY;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_DSV_DIMENSION *d3d_dsv_dimension_11, D3D_DSV_DIMENSION d3d_dsv_dimension )
{
    switch( d3d_dsv_dimension )
    {
        default:
            *d3d_dsv_dimension_11 = (D3D11_DSV_DIMENSION)d3d_dsv_dimension;
            break;
        case D3D_DSV_DIMENSION_UNKNOWN:
            *d3d_dsv_dimension_11 = D3D11_DSV_DIMENSION_UNKNOWN;
            break;
        case D3D_DSV_DIMENSION_TEXTURE1D:
            *d3d_dsv_dimension_11 = D3D11_DSV_DIMENSION_TEXTURE1D;
            break;
        case D3D_DSV_DIMENSION_TEXTURE1DARRAY:
            *d3d_dsv_dimension_11 = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
            break;
        case D3D_DSV_DIMENSION_TEXTURE2D:
            *d3d_dsv_dimension_11 = D3D11_DSV_DIMENSION_TEXTURE2D;
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DARRAY:
            *d3d_dsv_dimension_11 = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DMS:
            *d3d_dsv_dimension_11 = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DMSARRAY:
            *d3d_dsv_dimension_11 = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_RTV_DIMENSION *d3d_rtv_dimension, D3D11_RTV_DIMENSION d3d_rtv_dimension_11 )
{
    switch( d3d_rtv_dimension_11 )
    {
        default:
            *d3d_rtv_dimension = (D3D_RTV_DIMENSION)d3d_rtv_dimension_11;
            break;
        case D3D11_RTV_DIMENSION_UNKNOWN:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_UNKNOWN;
            break;
        case D3D11_RTV_DIMENSION_BUFFER:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_BUFFER;
            break;
        case D3D11_RTV_DIMENSION_TEXTURE1D:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE1D;
            break;
        case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE1DARRAY;
            break;
        case D3D11_RTV_DIMENSION_TEXTURE2D:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE2D;
            break;
        case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE2DARRAY;
            break;
        case D3D11_RTV_DIMENSION_TEXTURE2DMS:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE2DMS;
            break;
        case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE2DMSARRAY;
            break;
        case D3D11_RTV_DIMENSION_TEXTURE3D:
            *d3d_rtv_dimension = D3D_RTV_DIMENSION_TEXTURE3D;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_RTV_DIMENSION *d3d_rtv_dimension_11, D3D_RTV_DIMENSION d3d_rtv_dimension )
{
    switch( d3d_rtv_dimension )
    {
        default:
            *d3d_rtv_dimension_11 = (D3D11_RTV_DIMENSION)d3d_rtv_dimension;
            break;
        case D3D_RTV_DIMENSION_UNKNOWN:
            *d3d_rtv_dimension_11 = D3D11_RTV_DIMENSION_UNKNOWN;
            break;
        case D3D_RTV_DIMENSION_BUFFER:
            *d3d_rtv_dimension_11 = D3D11_RTV_DIMENSION_BUFFER;
            break;
        case D3D_RTV_DIMENSION_TEXTURE1D:
            *d3d_rtv_dimension_11 = D3D11_RTV_DIMENSION_TEXTURE1D;
            break;
        case D3D_RTV_DIMENSION_TEXTURE1DARRAY:
            *d3d_rtv_dimension_11 = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
            break;
        case D3D_RTV_DIMENSION_TEXTURE2D:
            *d3d_rtv_dimension_11 = D3D11_RTV_DIMENSION_TEXTURE2D;
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DARRAY:
            *d3d_rtv_dimension_11 = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DMS:
            *d3d_rtv_dimension_11 = D3D11_RTV_DIMENSION_TEXTURE2DMS;
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DMSARRAY:
            *d3d_rtv_dimension_11 = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
            break;
        case D3D_RTV_DIMENSION_TEXTURE3D:
            *d3d_rtv_dimension_11 = D3D11_RTV_DIMENSION_TEXTURE3D;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_USAGE *d3d_usage, D3D11_USAGE d3d_usage_11 )
{
    switch( d3d_usage_11 )
    {
        default:
            *d3d_usage = (D3D_USAGE)d3d_usage_11;
            break;
        case D3D11_USAGE_DEFAULT:
            *d3d_usage = D3D_USAGE_DEFAULT;
            break;
        case D3D11_USAGE_IMMUTABLE:
            *d3d_usage = D3D_USAGE_IMMUTABLE;
            break;
        case D3D11_USAGE_DYNAMIC:
            *d3d_usage = D3D_USAGE_DYNAMIC;
            break;
        case D3D11_USAGE_STAGING:
            *d3d_usage = D3D_USAGE_STAGING;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_USAGE *d3d_usage_11, D3D_USAGE d3d_usage )
{
    switch( d3d_usage )
    {
        default:
            *d3d_usage_11 = (D3D11_USAGE)d3d_usage;
            break;
        case D3D_USAGE_DEFAULT:
            *d3d_usage_11 = D3D11_USAGE_DEFAULT;
            break;
        case D3D_USAGE_IMMUTABLE:
            *d3d_usage_11 = D3D11_USAGE_IMMUTABLE;
            break;
        case D3D_USAGE_DYNAMIC:
            *d3d_usage_11 = D3D11_USAGE_DYNAMIC;
            break;
        case D3D_USAGE_STAGING:
            *d3d_usage_11 = D3D11_USAGE_STAGING;
            break;
    }
}

void D3DFlagsConvertFrom11_BIND_FLAG( UINT *bind_flag, UINT bind_flag_11 )
{
    *bind_flag = 0;
    if( bind_flag_11 & D3D11_BIND_VERTEX_BUFFER )
        *bind_flag |= D3D_BIND_VERTEX_BUFFER;
    if( bind_flag_11 & D3D11_BIND_INDEX_BUFFER )
        *bind_flag |= D3D_BIND_INDEX_BUFFER;
    if( bind_flag_11 & D3D11_BIND_CONSTANT_BUFFER )
        *bind_flag |= D3D_BIND_CONSTANT_BUFFER;
    if( bind_flag_11 & D3D11_BIND_SHADER_RESOURCE )
        *bind_flag |= D3D_BIND_SHADER_RESOURCE;
    if( bind_flag_11 & D3D11_BIND_STREAM_OUTPUT )
        *bind_flag |= D3D_BIND_STREAM_OUTPUT;
    if( bind_flag_11 & D3D11_BIND_RENDER_TARGET )
        *bind_flag |= D3D_BIND_RENDER_TARGET;
    if( bind_flag_11 & D3D11_BIND_DEPTH_STENCIL )
        *bind_flag |= D3D_BIND_DEPTH_STENCIL;
}

void D3DFlagsConvertTo11_BIND_FLAG( UINT *bind_flag_11, UINT bind_flag )
{
    *bind_flag_11 = 0;
    if( bind_flag & D3D_BIND_VERTEX_BUFFER )
        *bind_flag_11 |= D3D11_BIND_VERTEX_BUFFER;
    if( bind_flag & D3D_BIND_INDEX_BUFFER )
        *bind_flag_11 |= D3D11_BIND_INDEX_BUFFER;
    if( bind_flag & D3D_BIND_CONSTANT_BUFFER )
        *bind_flag_11 |= D3D11_BIND_CONSTANT_BUFFER;
    if( bind_flag & D3D_BIND_SHADER_RESOURCE )
        *bind_flag_11 |= D3D11_BIND_SHADER_RESOURCE;
    if( bind_flag & D3D_BIND_STREAM_OUTPUT )
        *bind_flag_11 |= D3D11_BIND_STREAM_OUTPUT;
    if( bind_flag & D3D_BIND_RENDER_TARGET )
        *bind_flag_11 |= D3D11_BIND_RENDER_TARGET;
    if( bind_flag & D3D_BIND_DEPTH_STENCIL )
        *bind_flag_11 |= D3D11_BIND_DEPTH_STENCIL;
}

void D3DFlagsConvertFrom11_CPU_ACCESS_FLAG( UINT *cpu_access_flag, UINT cpu_access_flag_11 )
{
    *cpu_access_flag = 0;
    if( cpu_access_flag_11 & D3D11_CPU_ACCESS_WRITE )
        *cpu_access_flag |= D3D_CPU_ACCESS_WRITE;
    if( cpu_access_flag_11 & D3D11_CPU_ACCESS_READ )
        *cpu_access_flag |= D3D_CPU_ACCESS_READ;
}

void D3DFlagsConvertTo11_CPU_ACCESS_FLAG( UINT *cpu_access_flag_11, UINT cpu_access_flag )
{
    *cpu_access_flag_11 = 0;
    if( cpu_access_flag & D3D_CPU_ACCESS_WRITE )
        *cpu_access_flag_11 |= D3D11_CPU_ACCESS_WRITE;
    if( cpu_access_flag & D3D_CPU_ACCESS_READ )
        *cpu_access_flag_11 |= D3D11_CPU_ACCESS_READ;
}

void D3DFlagsConvertFrom11_RESOURCE_MISC_FLAG( UINT *resource_misc_flag, UINT resource_misc_flag_11 )
{
    *resource_misc_flag = 0;
    if( resource_misc_flag_11 & D3D11_RESOURCE_MISC_GENERATE_MIPS )
        *resource_misc_flag |= D3D_RESOURCE_MISC_GENERATE_MIPS;
    if( resource_misc_flag_11 & D3D11_RESOURCE_MISC_SHARED )
        *resource_misc_flag |= D3D_RESOURCE_MISC_SHARED;
    if( resource_misc_flag_11 & D3D11_RESOURCE_MISC_TEXTURECUBE )
        *resource_misc_flag |= D3D_RESOURCE_MISC_TEXTURECUBE;
    if( resource_misc_flag_11 & D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX )
        *resource_misc_flag |= D3D_RESOURCE_MISC_SHARED_KEYEDMUTEX;
    if( resource_misc_flag_11 & D3D11_RESOURCE_MISC_GDI_COMPATIBLE )
        *resource_misc_flag |= D3D_RESOURCE_MISC_GDI_COMPATIBLE;
    if( resource_misc_flag_11 & D3D11_RESOURCE_MISC_HW_PROTECTED )
        *resource_misc_flag |= D3D_RESOURCE_MISC_HW_PROTECTED;
}

void D3DFlagsConvertTo11_RESOURCE_MISC_FLAG( UINT *resource_misc_flag_11, UINT resource_misc_flag )
{
    *resource_misc_flag_11 = 0;
    if( resource_misc_flag & D3D_RESOURCE_MISC_GENERATE_MIPS )
        *resource_misc_flag_11 |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
    if( resource_misc_flag & D3D_RESOURCE_MISC_SHARED )
        *resource_misc_flag_11 |= D3D11_RESOURCE_MISC_SHARED;
    if( resource_misc_flag & D3D_RESOURCE_MISC_TEXTURECUBE )
        *resource_misc_flag_11 |= D3D11_RESOURCE_MISC_TEXTURECUBE;
    if( resource_misc_flag & D3D_RESOURCE_MISC_SHARED_KEYEDMUTEX )
        *resource_misc_flag_11 |= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
    if( resource_misc_flag & D3D_RESOURCE_MISC_GDI_COMPATIBLE )
        *resource_misc_flag_11 |= D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
    if( resource_misc_flag & D3D_RESOURCE_MISC_HW_PROTECTED )
        *resource_misc_flag_11 |= D3D11_RESOURCE_MISC_HW_PROTECTED;
}

void D3DWrapperConvertEnum( D3D_MAP *d3d_map, D3D11_MAP d3d_map_11 )
{
    switch( d3d_map_11 )
    {
        default:
            *d3d_map = (D3D_MAP)d3d_map_11;
            break;
        case D3D11_MAP_READ:
            *d3d_map = D3D_MAP_READ;
            break;
        case D3D11_MAP_WRITE:
            *d3d_map = D3D_MAP_WRITE;
            break;
        case D3D11_MAP_READ_WRITE:
            *d3d_map = D3D_MAP_READ_WRITE;
            break;
        case D3D11_MAP_WRITE_DISCARD:
            *d3d_map = D3D_MAP_WRITE_DISCARD;
            break;
        case D3D11_MAP_WRITE_NO_OVERWRITE:
            *d3d_map = D3D_MAP_WRITE_NO_OVERWRITE;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_MAP *d3d_map_11, D3D_MAP d3d_map )
{
    switch( d3d_map )
    {
        default:
            *d3d_map_11 = (D3D11_MAP)d3d_map;
            break;
        case D3D_MAP_READ:
            *d3d_map_11 = D3D11_MAP_READ;
            break;
        case D3D_MAP_WRITE:
            *d3d_map_11 = D3D11_MAP_WRITE;
            break;
        case D3D_MAP_READ_WRITE:
            *d3d_map_11 = D3D11_MAP_READ_WRITE;
            break;
        case D3D_MAP_WRITE_DISCARD:
            *d3d_map_11 = D3D11_MAP_WRITE_DISCARD;
            break;
        case D3D_MAP_WRITE_NO_OVERWRITE:
            *d3d_map_11 = D3D11_MAP_WRITE_NO_OVERWRITE;
            break;
    }
}

void D3DFlagsConvertFrom11_MAP_FLAG( UINT *map_flag, UINT map_flag_11 )
{
    *map_flag = 0;
    if( map_flag_11 & D3D11_MAP_FLAG_DO_NOT_WAIT )
        *map_flag |= D3D_MAP_FLAG_DO_NOT_WAIT;
}

void D3DFlagsConvertTo11_MAP_FLAG( UINT *map_flag_11, UINT map_flag )
{
    *map_flag_11 = 0;
    if( map_flag & D3D_MAP_FLAG_DO_NOT_WAIT )
        *map_flag_11 |= D3D11_MAP_FLAG_DO_NOT_WAIT;
}

void D3DFlagsConvertFrom11_RAISE_FLAG( UINT *raise_flag, UINT raise_flag_11 )
{
    *raise_flag = 0;
    if( raise_flag_11 & D3D11_RAISE_FLAG_DRIVER_INTERNAL_ERROR )
        *raise_flag |= D3D_RAISE_FLAG_DRIVER_INTERNAL_ERROR;
}

void D3DFlagsConvertTo11_RAISE_FLAG( UINT *raise_flag_11, UINT raise_flag )
{
    *raise_flag_11 = 0;
    if( raise_flag & D3D_RAISE_FLAG_DRIVER_INTERNAL_ERROR )
        *raise_flag_11 |= D3D11_RAISE_FLAG_DRIVER_INTERNAL_ERROR;
}

void D3DFlagsConvertFrom11_CLEAR_FLAG( UINT *clear_flag, UINT clear_flag_11 )
{
    *clear_flag = 0;
    if( clear_flag_11 & D3D11_CLEAR_DEPTH )
        *clear_flag |= D3D_CLEAR_DEPTH;
    if( clear_flag_11 & D3D11_CLEAR_STENCIL )
        *clear_flag |= D3D_CLEAR_STENCIL;
}

void D3DFlagsConvertTo11_CLEAR_FLAG( UINT *clear_flag_11, UINT clear_flag )
{
    *clear_flag_11 = 0;
    if( clear_flag & D3D_CLEAR_DEPTH )
        *clear_flag_11 |= D3D11_CLEAR_DEPTH;
    if( clear_flag & D3D_CLEAR_STENCIL )
        *clear_flag_11 |= D3D11_CLEAR_STENCIL;
}

void D3DWrapperConvertEnum( D3D_COMPARISON_FUNC *d3d_comparison_func, D3D11_COMPARISON_FUNC d3d_comparison_func_11 )
{
    switch( d3d_comparison_func_11 )
    {
        default:
            *d3d_comparison_func = (D3D_COMPARISON_FUNC)d3d_comparison_func_11;
            break;
        case D3D11_COMPARISON_NEVER:
            *d3d_comparison_func = D3D_COMPARISON_NEVER;
            break;
        case D3D11_COMPARISON_LESS:
            *d3d_comparison_func = D3D_COMPARISON_LESS;
            break;
        case D3D11_COMPARISON_EQUAL:
            *d3d_comparison_func = D3D_COMPARISON_EQUAL;
            break;
        case D3D11_COMPARISON_LESS_EQUAL:
            *d3d_comparison_func = D3D_COMPARISON_LESS_EQUAL;
            break;
        case D3D11_COMPARISON_GREATER:
            *d3d_comparison_func = D3D_COMPARISON_GREATER;
            break;
        case D3D11_COMPARISON_NOT_EQUAL:
            *d3d_comparison_func = D3D_COMPARISON_NOT_EQUAL;
            break;
        case D3D11_COMPARISON_GREATER_EQUAL:
            *d3d_comparison_func = D3D_COMPARISON_GREATER_EQUAL;
            break;
        case D3D11_COMPARISON_ALWAYS:
            *d3d_comparison_func = D3D_COMPARISON_ALWAYS;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_COMPARISON_FUNC *d3d_comparison_func_11, D3D_COMPARISON_FUNC d3d_comparison_func )
{
    switch( d3d_comparison_func )
    {
        default:
            *d3d_comparison_func_11 = (D3D11_COMPARISON_FUNC)d3d_comparison_func;
            break;
        case D3D_COMPARISON_NEVER:
            *d3d_comparison_func_11 = D3D11_COMPARISON_NEVER;
            break;
        case D3D_COMPARISON_LESS:
            *d3d_comparison_func_11 = D3D11_COMPARISON_LESS;
            break;
        case D3D_COMPARISON_EQUAL:
            *d3d_comparison_func_11 = D3D11_COMPARISON_EQUAL;
            break;
        case D3D_COMPARISON_LESS_EQUAL:
            *d3d_comparison_func_11 = D3D11_COMPARISON_LESS_EQUAL;
            break;
        case D3D_COMPARISON_GREATER:
            *d3d_comparison_func_11 = D3D11_COMPARISON_GREATER;
            break;
        case D3D_COMPARISON_NOT_EQUAL:
            *d3d_comparison_func_11 = D3D11_COMPARISON_NOT_EQUAL;
            break;
        case D3D_COMPARISON_GREATER_EQUAL:
            *d3d_comparison_func_11 = D3D11_COMPARISON_GREATER_EQUAL;
            break;
        case D3D_COMPARISON_ALWAYS:
            *d3d_comparison_func_11 = D3D11_COMPARISON_ALWAYS;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_DEPTH_WRITE_MASK *d3d_depth_write_mask, D3D11_DEPTH_WRITE_MASK d3d_depth_write_mask_11 )
{
    switch( d3d_depth_write_mask_11 )
    {
        default:
            *d3d_depth_write_mask = (D3D_DEPTH_WRITE_MASK)d3d_depth_write_mask_11;
            break;
        case D3D11_DEPTH_WRITE_MASK_ZERO:
            *d3d_depth_write_mask = D3D_DEPTH_WRITE_MASK_ZERO;
            break;
        case D3D11_DEPTH_WRITE_MASK_ALL:
            *d3d_depth_write_mask = D3D_DEPTH_WRITE_MASK_ALL;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_DEPTH_WRITE_MASK *d3d_depth_write_mask_11, D3D_DEPTH_WRITE_MASK d3d_depth_write_mask )
{
    switch( d3d_depth_write_mask )
    {
        default:
            *d3d_depth_write_mask_11 = (D3D11_DEPTH_WRITE_MASK)d3d_depth_write_mask;
            break;
        case D3D_DEPTH_WRITE_MASK_ZERO:
            *d3d_depth_write_mask_11 = D3D11_DEPTH_WRITE_MASK_ZERO;
            break;
        case D3D_DEPTH_WRITE_MASK_ALL:
            *d3d_depth_write_mask_11 = D3D11_DEPTH_WRITE_MASK_ALL;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_STENCIL_OP *d3d_stencil_op, D3D11_STENCIL_OP d3d_stencil_op_11 )
{
    switch( d3d_stencil_op_11 )
    {
        default:
            *d3d_stencil_op = (D3D_STENCIL_OP)d3d_stencil_op_11;
            break;
        case D3D11_STENCIL_OP_KEEP:
            *d3d_stencil_op = D3D_STENCIL_OP_KEEP;
            break;
        case D3D11_STENCIL_OP_ZERO:
            *d3d_stencil_op = D3D_STENCIL_OP_ZERO;
            break;
        case D3D11_STENCIL_OP_REPLACE:
            *d3d_stencil_op = D3D_STENCIL_OP_REPLACE;
            break;
        case D3D11_STENCIL_OP_INCR_SAT:
            *d3d_stencil_op = D3D_STENCIL_OP_INCR_SAT;
            break;
        case D3D11_STENCIL_OP_DECR_SAT:
            *d3d_stencil_op = D3D_STENCIL_OP_DECR_SAT;
            break;
        case D3D11_STENCIL_OP_INVERT:
            *d3d_stencil_op = D3D_STENCIL_OP_INVERT;
            break;
        case D3D11_STENCIL_OP_INCR:
            *d3d_stencil_op = D3D_STENCIL_OP_INCR;
            break;
        case D3D11_STENCIL_OP_DECR:
            *d3d_stencil_op = D3D_STENCIL_OP_DECR;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_STENCIL_OP *d3d_stencil_op_11, D3D_STENCIL_OP d3d_stencil_op )
{
    switch( d3d_stencil_op )
    {
        default:
            *d3d_stencil_op_11 = (D3D11_STENCIL_OP)d3d_stencil_op;
            break;
        case D3D_STENCIL_OP_KEEP:
            *d3d_stencil_op_11 = D3D11_STENCIL_OP_KEEP;
            break;
        case D3D_STENCIL_OP_ZERO:
            *d3d_stencil_op_11 = D3D11_STENCIL_OP_ZERO;
            break;
        case D3D_STENCIL_OP_REPLACE:
            *d3d_stencil_op_11 = D3D11_STENCIL_OP_REPLACE;
            break;
        case D3D_STENCIL_OP_INCR_SAT:
            *d3d_stencil_op_11 = D3D11_STENCIL_OP_INCR_SAT;
            break;
        case D3D_STENCIL_OP_DECR_SAT:
            *d3d_stencil_op_11 = D3D11_STENCIL_OP_DECR_SAT;
            break;
        case D3D_STENCIL_OP_INVERT:
            *d3d_stencil_op_11 = D3D11_STENCIL_OP_INVERT;
            break;
        case D3D_STENCIL_OP_INCR:
            *d3d_stencil_op_11 = D3D11_STENCIL_OP_INCR;
            break;
        case D3D_STENCIL_OP_DECR:
            *d3d_stencil_op_11 = D3D11_STENCIL_OP_DECR;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_BLEND *d3d_blend, D3D11_BLEND d3d_blend_11 )
{
    switch( d3d_blend_11 )
    {
        default:
            *d3d_blend = (D3D_BLEND)d3d_blend_11;
            break;
        case D3D11_BLEND_ZERO:
            *d3d_blend = D3D_BLEND_ZERO;
            break;
        case D3D11_BLEND_ONE:
            *d3d_blend = D3D_BLEND_ONE;
            break;
        case D3D11_BLEND_SRC_COLOR:
            *d3d_blend = D3D_BLEND_SRC_COLOR;
            break;
        case D3D11_BLEND_INV_SRC_COLOR:
            *d3d_blend = D3D_BLEND_INV_SRC_COLOR;
            break;
        case D3D11_BLEND_SRC_ALPHA:
            *d3d_blend = D3D_BLEND_SRC_ALPHA;
            break;
        case D3D11_BLEND_INV_SRC_ALPHA:
            *d3d_blend = D3D_BLEND_INV_SRC_ALPHA;
            break;
        case D3D11_BLEND_DEST_ALPHA:
            *d3d_blend = D3D_BLEND_DEST_ALPHA;
            break;
        case D3D11_BLEND_INV_DEST_ALPHA:
            *d3d_blend = D3D_BLEND_INV_DEST_ALPHA;
            break;
        case D3D11_BLEND_DEST_COLOR:
            *d3d_blend = D3D_BLEND_DEST_COLOR;
            break;
        case D3D11_BLEND_INV_DEST_COLOR:
            *d3d_blend = D3D_BLEND_INV_DEST_COLOR;
            break;
        case D3D11_BLEND_SRC_ALPHA_SAT:
            *d3d_blend = D3D_BLEND_SRC_ALPHA_SAT;
            break;
        case D3D11_BLEND_BLEND_FACTOR:
            *d3d_blend = D3D_BLEND_BLEND_FACTOR;
            break;
        case D3D11_BLEND_INV_BLEND_FACTOR:
            *d3d_blend = D3D_BLEND_INV_BLEND_FACTOR;
            break;
        case D3D11_BLEND_SRC1_COLOR:
            *d3d_blend = D3D_BLEND_SRC1_COLOR;
            break;
        case D3D11_BLEND_INV_SRC1_COLOR:
            *d3d_blend = D3D_BLEND_INV_SRC1_COLOR;
            break;
        case D3D11_BLEND_SRC1_ALPHA:
            *d3d_blend = D3D_BLEND_SRC1_ALPHA;
            break;
        case D3D11_BLEND_INV_SRC1_ALPHA:
            *d3d_blend = D3D_BLEND_INV_SRC1_ALPHA;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_BLEND *d3d_blend_11, D3D_BLEND d3d_blend )
{
    switch( d3d_blend )
    {
        default:
            *d3d_blend_11 = (D3D11_BLEND)d3d_blend;
            break;
        case D3D_BLEND_ZERO:
            *d3d_blend_11 = D3D11_BLEND_ZERO;
            break;
        case D3D_BLEND_ONE:
            *d3d_blend_11 = D3D11_BLEND_ONE;
            break;
        case D3D_BLEND_SRC_COLOR:
            *d3d_blend_11 = D3D11_BLEND_SRC_COLOR;
            break;
        case D3D_BLEND_INV_SRC_COLOR:
            *d3d_blend_11 = D3D11_BLEND_INV_SRC_COLOR;
            break;
        case D3D_BLEND_SRC_ALPHA:
            *d3d_blend_11 = D3D11_BLEND_SRC_ALPHA;
            break;
        case D3D_BLEND_INV_SRC_ALPHA:
            *d3d_blend_11 = D3D11_BLEND_INV_SRC_ALPHA;
            break;
        case D3D_BLEND_DEST_ALPHA:
            *d3d_blend_11 = D3D11_BLEND_DEST_ALPHA;
            break;
        case D3D_BLEND_INV_DEST_ALPHA:
            *d3d_blend_11 = D3D11_BLEND_INV_DEST_ALPHA;
            break;
        case D3D_BLEND_DEST_COLOR:
            *d3d_blend_11 = D3D11_BLEND_DEST_COLOR;
            break;
        case D3D_BLEND_INV_DEST_COLOR:
            *d3d_blend_11 = D3D11_BLEND_INV_DEST_COLOR;
            break;
        case D3D_BLEND_SRC_ALPHA_SAT:
            *d3d_blend_11 = D3D11_BLEND_SRC_ALPHA_SAT;
            break;
        case D3D_BLEND_BLEND_FACTOR:
            *d3d_blend_11 = D3D11_BLEND_BLEND_FACTOR;
            break;
        case D3D_BLEND_INV_BLEND_FACTOR:
            *d3d_blend_11 = D3D11_BLEND_INV_BLEND_FACTOR;
            break;
        case D3D_BLEND_SRC1_COLOR:
            *d3d_blend_11 = D3D11_BLEND_SRC1_COLOR;
            break;
        case D3D_BLEND_INV_SRC1_COLOR:
            *d3d_blend_11 = D3D11_BLEND_INV_SRC1_COLOR;
            break;
        case D3D_BLEND_SRC1_ALPHA:
            *d3d_blend_11 = D3D11_BLEND_SRC1_ALPHA;
            break;
        case D3D_BLEND_INV_SRC1_ALPHA:
            *d3d_blend_11 = D3D11_BLEND_INV_SRC1_ALPHA;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_BLEND_OP *d3d_blend_op, D3D11_BLEND_OP d3d_blend_op_11 )
{
    switch( d3d_blend_op_11 )
    {
        default:
            *d3d_blend_op = (D3D_BLEND_OP)d3d_blend_op_11;
            break;
        case D3D11_BLEND_OP_ADD:
            *d3d_blend_op = D3D_BLEND_OP_ADD;
            break;
        case D3D11_BLEND_OP_SUBTRACT:
            *d3d_blend_op = D3D_BLEND_OP_SUBTRACT;
            break;
        case D3D11_BLEND_OP_REV_SUBTRACT:
            *d3d_blend_op = D3D_BLEND_OP_REV_SUBTRACT;
            break;
        case D3D11_BLEND_OP_MIN:
            *d3d_blend_op = D3D_BLEND_OP_MIN;
            break;
        case D3D11_BLEND_OP_MAX:
            *d3d_blend_op = D3D_BLEND_OP_MAX;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_BLEND_OP *d3d_blend_op_11, D3D_BLEND_OP d3d_blend_op )
{
    switch( d3d_blend_op )
    {
        default:
            *d3d_blend_op_11 = (D3D11_BLEND_OP)d3d_blend_op;
            break;
        case D3D_BLEND_OP_ADD:
            *d3d_blend_op_11 = D3D11_BLEND_OP_ADD;
            break;
        case D3D_BLEND_OP_SUBTRACT:
            *d3d_blend_op_11 = D3D11_BLEND_OP_SUBTRACT;
            break;
        case D3D_BLEND_OP_REV_SUBTRACT:
            *d3d_blend_op_11 = D3D11_BLEND_OP_REV_SUBTRACT;
            break;
        case D3D_BLEND_OP_MIN:
            *d3d_blend_op_11 = D3D11_BLEND_OP_MIN;
            break;
        case D3D_BLEND_OP_MAX:
            *d3d_blend_op_11 = D3D11_BLEND_OP_MAX;
            break;
    }
}

void D3DFlagsConvertFrom11_COLOR_WRITE_ENABLE( UINT8 *color_write_enable, UINT8 color_write_enable_11 )
{
    *color_write_enable = 0;
    if( color_write_enable_11 & D3D11_COLOR_WRITE_ENABLE_RED )
        *color_write_enable |= D3D_COLOR_WRITE_ENABLE_RED;
    if( color_write_enable_11 & D3D11_COLOR_WRITE_ENABLE_GREEN )
        *color_write_enable |= D3D_COLOR_WRITE_ENABLE_GREEN;
    if( color_write_enable_11 & D3D11_COLOR_WRITE_ENABLE_BLUE )
        *color_write_enable |= D3D_COLOR_WRITE_ENABLE_BLUE;
    if( color_write_enable_11 & D3D11_COLOR_WRITE_ENABLE_ALPHA )
        *color_write_enable |= D3D_COLOR_WRITE_ENABLE_ALPHA;
}

void D3DFlagsConvertTo11_COLOR_WRITE_ENABLE( UINT8 *color_write_enable_11, UINT8 color_write_enable )
{
    *color_write_enable_11 = 0;
    if( color_write_enable & D3D_COLOR_WRITE_ENABLE_RED )
        *color_write_enable_11 |= D3D11_COLOR_WRITE_ENABLE_RED;
    if( color_write_enable & D3D_COLOR_WRITE_ENABLE_GREEN )
        *color_write_enable_11 |= D3D11_COLOR_WRITE_ENABLE_GREEN;
    if( color_write_enable & D3D_COLOR_WRITE_ENABLE_BLUE )
        *color_write_enable_11 |= D3D11_COLOR_WRITE_ENABLE_BLUE;
    if( color_write_enable & D3D_COLOR_WRITE_ENABLE_ALPHA )
        *color_write_enable_11 |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
}

void D3DWrapperConvertEnum( D3D_TEXTURECUBE_FACE *d3d_texturecube_face, D3D11_TEXTURECUBE_FACE d3d_texturecube_face_11 )
{
    switch( d3d_texturecube_face_11 )
    {
        default:
            *d3d_texturecube_face = (D3D_TEXTURECUBE_FACE)d3d_texturecube_face_11;
            break;
        case D3D11_TEXTURECUBE_FACE_POSITIVE_X:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_POSITIVE_X;
            break;
        case D3D11_TEXTURECUBE_FACE_NEGATIVE_X:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_NEGATIVE_X;
            break;
        case D3D11_TEXTURECUBE_FACE_POSITIVE_Y:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_POSITIVE_Y;
            break;
        case D3D11_TEXTURECUBE_FACE_NEGATIVE_Y:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_NEGATIVE_Y;
            break;
        case D3D11_TEXTURECUBE_FACE_POSITIVE_Z:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_POSITIVE_Z;
            break;
        case D3D11_TEXTURECUBE_FACE_NEGATIVE_Z:
            *d3d_texturecube_face = D3D_TEXTURECUBE_FACE_NEGATIVE_Z;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_TEXTURECUBE_FACE *d3d_texturecube_face_11, D3D_TEXTURECUBE_FACE d3d_texturecube_face )
{
    switch( d3d_texturecube_face )
    {
        default:
            *d3d_texturecube_face_11 = (D3D11_TEXTURECUBE_FACE)d3d_texturecube_face;
            break;
        case D3D_TEXTURECUBE_FACE_POSITIVE_X:
            *d3d_texturecube_face_11 = D3D11_TEXTURECUBE_FACE_POSITIVE_X;
            break;
        case D3D_TEXTURECUBE_FACE_NEGATIVE_X:
            *d3d_texturecube_face_11 = D3D11_TEXTURECUBE_FACE_NEGATIVE_X;
            break;
        case D3D_TEXTURECUBE_FACE_POSITIVE_Y:
            *d3d_texturecube_face_11 = D3D11_TEXTURECUBE_FACE_POSITIVE_Y;
            break;
        case D3D_TEXTURECUBE_FACE_NEGATIVE_Y:
            *d3d_texturecube_face_11 = D3D11_TEXTURECUBE_FACE_NEGATIVE_Y;
            break;
        case D3D_TEXTURECUBE_FACE_POSITIVE_Z:
            *d3d_texturecube_face_11 = D3D11_TEXTURECUBE_FACE_POSITIVE_Z;
            break;
        case D3D_TEXTURECUBE_FACE_NEGATIVE_Z:
            *d3d_texturecube_face_11 = D3D11_TEXTURECUBE_FACE_NEGATIVE_Z;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_FILTER *d3d_filter, D3D11_FILTER d3d_filter_11 )
{
    switch( d3d_filter_11 )
    {
        default:
            *d3d_filter = (D3D_FILTER)d3d_filter_11;
            break;
        case D3D11_FILTER_MIN_MAG_MIP_POINT:
            *d3d_filter = D3D_FILTER_MIN_MAG_MIP_POINT;
            break;
        case D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_MIN_MAG_POINT_MIP_LINEAR;
            break;
        case D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
            *d3d_filter = D3D_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
            break;
        case D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_MIN_POINT_MAG_MIP_LINEAR;
            break;
        case D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT:
            *d3d_filter = D3D_FILTER_MIN_LINEAR_MAG_MIP_POINT;
            break;
        case D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            break;
        case D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT:
            *d3d_filter = D3D_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            break;
        case D3D11_FILTER_MIN_MAG_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        case D3D11_FILTER_ANISOTROPIC:
            *d3d_filter = D3D_FILTER_ANISOTROPIC;
            break;
        case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
            break;
        case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
            break;
        case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
            break;
        case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
            break;
        case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
            break;
        case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            break;
        case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
            break;
        case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
            *d3d_filter = D3D_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
            break;
        case D3D11_FILTER_COMPARISON_ANISOTROPIC:
            *d3d_filter = D3D_FILTER_COMPARISON_ANISOTROPIC;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_FILTER *d3d_filter_11, D3D_FILTER d3d_filter )
{
    switch( d3d_filter )
    {
        default:
            *d3d_filter_11 = (D3D11_FILTER)d3d_filter;
            break;
        case D3D_FILTER_MIN_MAG_MIP_POINT:
            *d3d_filter_11 = D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
        case D3D_FILTER_MIN_MAG_POINT_MIP_LINEAR:
            *d3d_filter_11 = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
            break;
        case D3D_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
            *d3d_filter_11 = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
            break;
        case D3D_FILTER_MIN_POINT_MAG_MIP_LINEAR:
            *d3d_filter_11 = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
            break;
        case D3D_FILTER_MIN_LINEAR_MAG_MIP_POINT:
            *d3d_filter_11 = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
            break;
        case D3D_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
            *d3d_filter_11 = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            break;
        case D3D_FILTER_MIN_MAG_LINEAR_MIP_POINT:
            *d3d_filter_11 = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            break;
        case D3D_FILTER_MIN_MAG_MIP_LINEAR:
            *d3d_filter_11 = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        case D3D_FILTER_ANISOTROPIC:
            *d3d_filter_11 = D3D11_FILTER_ANISOTROPIC;
            break;
        case D3D_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
            *d3d_filter_11 = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
            break;
        case D3D_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
            *d3d_filter_11 = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
            break;
        case D3D_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
            *d3d_filter_11 = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
            break;
        case D3D_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
            *d3d_filter_11 = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
            break;
        case D3D_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
            *d3d_filter_11 = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
            break;
        case D3D_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
            *d3d_filter_11 = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            break;
        case D3D_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
            *d3d_filter_11 = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
            break;
        case D3D_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
            *d3d_filter_11 = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
            break;
        case D3D_FILTER_COMPARISON_ANISOTROPIC:
            *d3d_filter_11 = D3D11_FILTER_COMPARISON_ANISOTROPIC;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_FILTER_TYPE *d3d_filter_type, D3D11_FILTER_TYPE d3d_filter_type_11 )
{
    switch( d3d_filter_type_11 )
    {
        default:
            *d3d_filter_type = (D3D_FILTER_TYPE)d3d_filter_type_11;
            break;
        case D3D11_FILTER_TYPE_POINT:
            *d3d_filter_type = D3D_FILTER_TYPE_POINT;
            break;
        case D3D11_FILTER_TYPE_LINEAR:
            *d3d_filter_type = D3D_FILTER_TYPE_LINEAR;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_FILTER_TYPE *d3d_filter_type_11, D3D_FILTER_TYPE d3d_filter_type )
{
    switch( d3d_filter_type )
    {
        default:
            *d3d_filter_type_11 = (D3D11_FILTER_TYPE)d3d_filter_type;
            break;
        case D3D_FILTER_TYPE_POINT:
            *d3d_filter_type_11 = D3D11_FILTER_TYPE_POINT;
            break;
        case D3D_FILTER_TYPE_LINEAR:
            *d3d_filter_type_11 = D3D11_FILTER_TYPE_LINEAR;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_TEXTURE_ADDRESS_MODE *d3d_texture_address_mode, D3D11_TEXTURE_ADDRESS_MODE d3d_texture_address_mode_11 )
{
    switch( d3d_texture_address_mode_11 )
    {
        default:
            *d3d_texture_address_mode = (D3D_TEXTURE_ADDRESS_MODE)d3d_texture_address_mode_11;
            break;
        case D3D11_TEXTURE_ADDRESS_WRAP:
            *d3d_texture_address_mode = D3D_TEXTURE_ADDRESS_WRAP;
            break;
        case D3D11_TEXTURE_ADDRESS_MIRROR:
            *d3d_texture_address_mode = D3D_TEXTURE_ADDRESS_MIRROR;
            break;
        case D3D11_TEXTURE_ADDRESS_CLAMP:
            *d3d_texture_address_mode = D3D_TEXTURE_ADDRESS_CLAMP;
            break;
        case D3D11_TEXTURE_ADDRESS_BORDER:
            *d3d_texture_address_mode = D3D_TEXTURE_ADDRESS_BORDER;
            break;
        case D3D11_TEXTURE_ADDRESS_MIRROR_ONCE:
            *d3d_texture_address_mode = D3D_TEXTURE_ADDRESS_MIRROR_ONCE;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_TEXTURE_ADDRESS_MODE *d3d_texture_address_mode_11, D3D_TEXTURE_ADDRESS_MODE d3d_texture_address_mode )
{
    switch( d3d_texture_address_mode )
    {
        default:
            *d3d_texture_address_mode_11 = (D3D11_TEXTURE_ADDRESS_MODE)d3d_texture_address_mode;
            break;
        case D3D_TEXTURE_ADDRESS_WRAP:
            *d3d_texture_address_mode_11 = D3D11_TEXTURE_ADDRESS_WRAP;
            break;
        case D3D_TEXTURE_ADDRESS_MIRROR:
            *d3d_texture_address_mode_11 = D3D11_TEXTURE_ADDRESS_MIRROR;
            break;
        case D3D_TEXTURE_ADDRESS_CLAMP:
            *d3d_texture_address_mode_11 = D3D11_TEXTURE_ADDRESS_CLAMP;
            break;
        case D3D_TEXTURE_ADDRESS_BORDER:
            *d3d_texture_address_mode_11 = D3D11_TEXTURE_ADDRESS_BORDER;
            break;
        case D3D_TEXTURE_ADDRESS_MIRROR_ONCE:
            *d3d_texture_address_mode_11 = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
            break;
    }
}

void D3DFlagsConvertFrom11_FORMAT_SUPPORT( UINT *format_support, UINT format_support_11 )
{
    *format_support = 0;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_BUFFER )
        *format_support |= D3D_FORMAT_SUPPORT_BUFFER;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER )
        *format_support |= D3D_FORMAT_SUPPORT_IA_VERTEX_BUFFER;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER )
        *format_support |= D3D_FORMAT_SUPPORT_IA_INDEX_BUFFER;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_SO_BUFFER )
        *format_support |= D3D_FORMAT_SUPPORT_SO_BUFFER;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_TEXTURE1D )
        *format_support |= D3D_FORMAT_SUPPORT_TEXTURE1D;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_TEXTURE2D )
        *format_support |= D3D_FORMAT_SUPPORT_TEXTURE2D;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_TEXTURE3D )
        *format_support |= D3D_FORMAT_SUPPORT_TEXTURE3D;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_TEXTURECUBE )
        *format_support |= D3D_FORMAT_SUPPORT_TEXTURECUBE;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_SHADER_LOAD )
        *format_support |= D3D_FORMAT_SUPPORT_SHADER_LOAD;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE )
        *format_support |= D3D_FORMAT_SUPPORT_SHADER_SAMPLE;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON )
        *format_support |= D3D_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT )
        *format_support |= D3D_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_MIP )
        *format_support |= D3D_FORMAT_SUPPORT_MIP;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN )
        *format_support |= D3D_FORMAT_SUPPORT_MIP_AUTOGEN;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_RENDER_TARGET )
        *format_support |= D3D_FORMAT_SUPPORT_RENDER_TARGET;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_BLENDABLE )
        *format_support |= D3D_FORMAT_SUPPORT_BLENDABLE;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL )
        *format_support |= D3D_FORMAT_SUPPORT_DEPTH_STENCIL;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_CPU_LOCKABLE )
        *format_support |= D3D_FORMAT_SUPPORT_CPU_LOCKABLE;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE )
        *format_support |= D3D_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_DISPLAY )
        *format_support |= D3D_FORMAT_SUPPORT_DISPLAY;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT )
        *format_support |= D3D_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET )
        *format_support |= D3D_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD )
        *format_support |= D3D_FORMAT_SUPPORT_MULTISAMPLE_LOAD;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_SHADER_GATHER )
        *format_support |= D3D_FORMAT_SUPPORT_SHADER_GATHER;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST )
        *format_support |= D3D_FORMAT_SUPPORT_BACK_BUFFER_CAST;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW )
        *format_support |= D3D_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON )
        *format_support |= D3D_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_DECODER_OUTPUT )
        *format_support |= D3D_FORMAT_SUPPORT_DECODER_OUTPUT;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT )
        *format_support |= D3D_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT )
        *format_support |= D3D_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
    if( format_support_11 & D3D11_FORMAT_SUPPORT_VIDEO_ENCODER )
        *format_support |= D3D11_FORMAT_SUPPORT_VIDEO_ENCODER;
}

void D3DFlagsConvertTo11_FORMAT_SUPPORT( UINT *format_support_11, UINT format_support )
{
    *format_support_11 = 0;
    if( format_support & D3D_FORMAT_SUPPORT_BUFFER )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_BUFFER;
    if( format_support & D3D_FORMAT_SUPPORT_IA_VERTEX_BUFFER )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER;
    if( format_support & D3D_FORMAT_SUPPORT_IA_INDEX_BUFFER )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER;
    if( format_support & D3D_FORMAT_SUPPORT_SO_BUFFER )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_SO_BUFFER;
    if( format_support & D3D_FORMAT_SUPPORT_TEXTURE1D )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_TEXTURE1D;
    if( format_support & D3D_FORMAT_SUPPORT_TEXTURE2D )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_TEXTURE2D;
    if( format_support & D3D_FORMAT_SUPPORT_TEXTURE3D )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_TEXTURE3D;
    if( format_support & D3D_FORMAT_SUPPORT_TEXTURECUBE )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_TEXTURECUBE;
    if( format_support & D3D_FORMAT_SUPPORT_SHADER_LOAD )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_SHADER_LOAD;
    if( format_support & D3D_FORMAT_SUPPORT_SHADER_SAMPLE )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE;
    if( format_support & D3D_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON;
    if( format_support & D3D_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT;
    if( format_support & D3D_FORMAT_SUPPORT_MIP )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_MIP;
    if( format_support & D3D_FORMAT_SUPPORT_MIP_AUTOGEN )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_MIP_AUTOGEN;
    if( format_support & D3D_FORMAT_SUPPORT_RENDER_TARGET )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_RENDER_TARGET;
    if( format_support & D3D_FORMAT_SUPPORT_BLENDABLE )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_BLENDABLE;
    if( format_support & D3D_FORMAT_SUPPORT_DEPTH_STENCIL )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_DEPTH_STENCIL;
    if( format_support & D3D_FORMAT_SUPPORT_CPU_LOCKABLE )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_CPU_LOCKABLE;
    if( format_support & D3D_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE;
    if( format_support & D3D_FORMAT_SUPPORT_DISPLAY )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_DISPLAY;
    if( format_support & D3D_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT;
    if( format_support & D3D_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET;
    if( format_support & D3D_FORMAT_SUPPORT_MULTISAMPLE_LOAD )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD;
    if( format_support & D3D_FORMAT_SUPPORT_SHADER_GATHER )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
    if( format_support & D3D_FORMAT_SUPPORT_BACK_BUFFER_CAST )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST;
    if( format_support & D3D_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW )
        *format_support_11 |= D3D_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW;
    if( format_support & D3D_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON )
        *format_support_11 |= D3D_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON;
    if( format_support & D3D_FORMAT_SUPPORT_DECODER_OUTPUT )
        *format_support_11 |= D3D_FORMAT_SUPPORT_DECODER_OUTPUT;
    if( format_support & D3D_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT )
        *format_support_11 |= D3D_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT;
    if( format_support & D3D_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT )
        *format_support_11 |= D3D_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
    if( format_support & D3D_FORMAT_SUPPORT_VIDEO_ENCODER )
        *format_support_11 |= D3D11_FORMAT_SUPPORT_VIDEO_ENCODER;
}

void D3DFlagsConvertFrom11_ASYNC_GETDATA_FLAG( UINT *async_getdata_flag, UINT async_getdata_flag_11 )
{
    *async_getdata_flag = 0;
    if( async_getdata_flag_11 & D3D11_ASYNC_GETDATA_DONOTFLUSH )
        *async_getdata_flag |= D3D_ASYNC_GETDATA_DONOTFLUSH;
}

void D3DFlagsConvertTo11_ASYNC_GETDATA_FLAG( UINT *async_getdata_flag_11, UINT async_getdata_flag )
{
    *async_getdata_flag_11 = 0;
    if( async_getdata_flag & D3D_ASYNC_GETDATA_DONOTFLUSH )
        *async_getdata_flag_11 |= D3D11_ASYNC_GETDATA_DONOTFLUSH;
}

void D3DWrapperConvertEnum( D3D_QUERY *d3d_query, D3D11_QUERY d3d_query_11 )
{
    switch( d3d_query_11 )
    {
        default:
            *d3d_query = (D3D_QUERY)d3d_query_11;
            break;
        case D3D11_QUERY_EVENT:
            *d3d_query = D3D_QUERY_EVENT;
            break;
        case D3D11_QUERY_OCCLUSION:
            *d3d_query = D3D_QUERY_OCCLUSION;
            break;
        case D3D11_QUERY_TIMESTAMP:
            *d3d_query = D3D_QUERY_TIMESTAMP;
            break;
        case D3D11_QUERY_TIMESTAMP_DISJOINT:
            *d3d_query = D3D_QUERY_TIMESTAMP_DISJOINT;
            break;
        case D3D11_QUERY_PIPELINE_STATISTICS:
            *d3d_query = D3D_QUERY_PIPELINE_STATISTICS;
            break;
        case D3D11_QUERY_OCCLUSION_PREDICATE:
            *d3d_query = D3D_QUERY_OCCLUSION_PREDICATE;
            break;
        case D3D11_QUERY_SO_STATISTICS:
            *d3d_query = D3D_QUERY_SO_STATISTICS;
            break;
        case D3D11_QUERY_SO_OVERFLOW_PREDICATE:
            *d3d_query = D3D_QUERY_SO_OVERFLOW_PREDICATE;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_QUERY *d3d_query_11, D3D_QUERY d3d_query )
{
    switch( d3d_query )
    {
        default:
            *d3d_query_11 = (D3D11_QUERY)d3d_query;
            break;
        case D3D_QUERY_EVENT:
            *d3d_query_11 = D3D11_QUERY_EVENT;
            break;
        case D3D_QUERY_OCCLUSION:
            *d3d_query_11 = D3D11_QUERY_OCCLUSION;
            break;
        case D3D_QUERY_TIMESTAMP:
            *d3d_query_11 = D3D11_QUERY_TIMESTAMP;
            break;
        case D3D_QUERY_TIMESTAMP_DISJOINT:
            *d3d_query_11 = D3D11_QUERY_TIMESTAMP_DISJOINT;
            break;
        case D3D_QUERY_PIPELINE_STATISTICS:
            *d3d_query_11 = D3D11_QUERY_PIPELINE_STATISTICS;
            break;
        case D3D_QUERY_OCCLUSION_PREDICATE:
            *d3d_query_11 = D3D11_QUERY_OCCLUSION_PREDICATE;
            break;
        case D3D_QUERY_SO_STATISTICS:
            *d3d_query_11 = D3D11_QUERY_SO_STATISTICS;
            break;
        case D3D_QUERY_SO_OVERFLOW_PREDICATE:
            *d3d_query_11 = D3D11_QUERY_SO_OVERFLOW_PREDICATE;
            break;
    }
}

void D3DFlagsConvertFrom11_QUERY_MISC_FLAG( UINT *query_misc_flag, UINT query_misc_flag_11 )
{
    *query_misc_flag = 0;
    if( query_misc_flag_11 & D3D11_QUERY_MISC_PREDICATEHINT )
        *query_misc_flag |= D3D_QUERY_MISC_PREDICATEHINT;
}

void D3DFlagsConvertTo11_QUERY_MISC_FLAG( UINT *query_misc_flag_11, UINT query_misc_flag )
{
    *query_misc_flag_11 = 0;
    if( query_misc_flag & D3D_QUERY_MISC_PREDICATEHINT )
        *query_misc_flag_11 |= D3D11_QUERY_MISC_PREDICATEHINT;
}

void D3DWrapperConvertEnum( D3D_COUNTER *d3d_counter, D3D11_COUNTER d3d_counter_11 )
{
    switch( d3d_counter_11 )
    {
        default:
            if (d3d_counter_11 >= D3D11_COUNTER_DEVICE_DEPENDENT_0)
                *d3d_counter = (D3D_COUNTER)((d3d_counter_11 - D3D11_COUNTER_DEVICE_DEPENDENT_0) + D3D_COUNTER_DEVICE_DEPENDENT_0);
			else if ( d3d_counter_11 == 0 ) // no counters are supported
				*d3d_counter = (D3D_COUNTER)0;
            else
                assert(!"Unknown enum value");
            break;
        case D3D11_COUNTER_DEVICE_DEPENDENT_0:
            *d3d_counter = D3D_COUNTER_DEVICE_DEPENDENT_0;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_COUNTER *d3d_counter_11, D3D_COUNTER d3d_counter )
{
    switch( d3d_counter )
    {
        default:
            if (d3d_counter >= D3D_COUNTER_DEVICE_DEPENDENT_0)
                *d3d_counter_11 = (D3D11_COUNTER)((d3d_counter - D3D_COUNTER_DEVICE_DEPENDENT_0) + D3D11_COUNTER_DEVICE_DEPENDENT_0);
			else if ( d3d_counter == 0 ) // no counters are supported
				*d3d_counter_11 = (D3D11_COUNTER)0;
            else
                assert(!"Unknown enum value");
            break;
        case D3D_COUNTER_DEVICE_DEPENDENT_0:
            *d3d_counter_11 = D3D11_COUNTER_DEVICE_DEPENDENT_0;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_COUNTER_TYPE *d3d_counter_type, D3D11_COUNTER_TYPE d3d_counter_type_11 )
{
    switch( d3d_counter_type_11 )
    {
        default:
            *d3d_counter_type = (D3D_COUNTER_TYPE)d3d_counter_type_11;
            break;
        case D3D11_COUNTER_TYPE_FLOAT32:
            *d3d_counter_type = D3D_COUNTER_TYPE_FLOAT32;
            break;
        case D3D11_COUNTER_TYPE_UINT16:
            *d3d_counter_type = D3D_COUNTER_TYPE_UINT16;
            break;
        case D3D11_COUNTER_TYPE_UINT32:
            *d3d_counter_type = D3D_COUNTER_TYPE_UINT32;
            break;
        case D3D11_COUNTER_TYPE_UINT64:
            *d3d_counter_type = D3D_COUNTER_TYPE_UINT64;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_COUNTER_TYPE *d3d_counter_type_11, D3D_COUNTER_TYPE d3d_counter_type )
{
    switch( d3d_counter_type )
    {
        default:
            *d3d_counter_type_11 = (D3D11_COUNTER_TYPE)d3d_counter_type;
            break;
        case D3D_COUNTER_TYPE_FLOAT32:
            *d3d_counter_type_11 = D3D11_COUNTER_TYPE_FLOAT32;
            break;
        case D3D_COUNTER_TYPE_UINT16:
            *d3d_counter_type_11 = D3D11_COUNTER_TYPE_UINT16;
            break;
        case D3D_COUNTER_TYPE_UINT32:
            *d3d_counter_type_11 = D3D11_COUNTER_TYPE_UINT32;
            break;
        case D3D_COUNTER_TYPE_UINT64:
            *d3d_counter_type_11 = D3D11_COUNTER_TYPE_UINT64;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_STANDARD_MULTISAMPLE_QUALITY_LEVELS *d3d_standard_multisample_quality_levels, D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS d3d_standard_multisample_quality_levels_11 )
{
    switch( d3d_standard_multisample_quality_levels_11 )
    {
        default:
            *d3d_standard_multisample_quality_levels = (D3D_STANDARD_MULTISAMPLE_QUALITY_LEVELS)d3d_standard_multisample_quality_levels_11;
            break;
        case D3D11_STANDARD_MULTISAMPLE_PATTERN:
            *d3d_standard_multisample_quality_levels = D3D_STANDARD_MULTISAMPLE_PATTERN;
            break;
        case D3D11_CENTER_MULTISAMPLE_PATTERN:
            *d3d_standard_multisample_quality_levels = D3D_CENTER_MULTISAMPLE_PATTERN;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS *d3d_standard_multisample_quality_levels_11, D3D_STANDARD_MULTISAMPLE_QUALITY_LEVELS d3d_standard_multisample_quality_levels )
{
    switch( d3d_standard_multisample_quality_levels )
    {
        default:
            *d3d_standard_multisample_quality_levels_11 = (D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS)d3d_standard_multisample_quality_levels;
            break;
        case D3D_STANDARD_MULTISAMPLE_PATTERN:
            *d3d_standard_multisample_quality_levels_11 = D3D11_STANDARD_MULTISAMPLE_PATTERN;
            break;
        case D3D_CENTER_MULTISAMPLE_PATTERN:
            *d3d_standard_multisample_quality_levels_11 = D3D11_CENTER_MULTISAMPLE_PATTERN;
            break;
    }
}

void D3DFlagsConvertFrom11_CREATE_DEVICE_FLAG( UINT *create_device_flag, UINT create_device_flag_11 )
{
    *create_device_flag = 0;
    if( create_device_flag_11 & D3D11_CREATE_DEVICE_SINGLETHREADED )
        *create_device_flag |= D3D_CREATE_DEVICE_SINGLETHREADED;
    if( create_device_flag_11 & D3D11_CREATE_DEVICE_DEBUG )
        *create_device_flag |= D3D_CREATE_DEVICE_DEBUG;
    if( create_device_flag_11 & D3D11_CREATE_DEVICE_SWITCH_TO_REF )
        *create_device_flag |= D3D_CREATE_DEVICE_SWITCH_TO_REF;
    if( create_device_flag_11 & D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS )
        *create_device_flag |= D3D_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
    if( create_device_flag_11 & D3D11_CREATE_DEVICE_BGRA_SUPPORT )
        *create_device_flag |= D3D_CREATE_DEVICE_BGRA_SUPPORT;
}

void D3DFlagsConvertTo11_CREATE_DEVICE_FLAG( UINT *create_device_flag_11, UINT create_device_flag )
{
    *create_device_flag_11 = 0;
    if( create_device_flag & D3D_CREATE_DEVICE_SINGLETHREADED )
        *create_device_flag_11 |= D3D11_CREATE_DEVICE_SINGLETHREADED;
    if( create_device_flag & D3D_CREATE_DEVICE_DEBUG )
        *create_device_flag_11 |= D3D11_CREATE_DEVICE_DEBUG;
    if( create_device_flag & D3D_CREATE_DEVICE_SWITCH_TO_REF )
        *create_device_flag_11 |= D3D11_CREATE_DEVICE_SWITCH_TO_REF;
    if( create_device_flag & D3D_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS )
        *create_device_flag_11 |= D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
    if( create_device_flag & D3D_CREATE_DEVICE_BGRA_SUPPORT )
        *create_device_flag_11 |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
}

void D3DWrapperConvertEnum( D3D_MESSAGE_CATEGORY *d3d_message_category, D3D11_MESSAGE_CATEGORY d3d_message_category_11 )
{
    switch( d3d_message_category_11 )
    {
        default:
            *d3d_message_category = (D3D_MESSAGE_CATEGORY)d3d_message_category_11;
            break;
        case D3D11_MESSAGE_CATEGORY_APPLICATION_DEFINED:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_APPLICATION_DEFINED;
            break;
        case D3D11_MESSAGE_CATEGORY_MISCELLANEOUS:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_MISCELLANEOUS;
            break;
        case D3D11_MESSAGE_CATEGORY_INITIALIZATION:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_INITIALIZATION;
            break;
        case D3D11_MESSAGE_CATEGORY_CLEANUP:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_CLEANUP;
            break;
        case D3D11_MESSAGE_CATEGORY_COMPILATION:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_COMPILATION;
            break;
        case D3D11_MESSAGE_CATEGORY_STATE_CREATION:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_STATE_CREATION;
            break;
        case D3D11_MESSAGE_CATEGORY_STATE_SETTING:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_STATE_SETTING;
            break;
        case D3D11_MESSAGE_CATEGORY_STATE_GETTING:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_STATE_GETTING;
            break;
        case D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_RESOURCE_MANIPULATION;
            break;
        case D3D11_MESSAGE_CATEGORY_EXECUTION:
            *d3d_message_category = D3D_MESSAGE_CATEGORY_EXECUTION;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_MESSAGE_CATEGORY *d3d_message_category_11, D3D_MESSAGE_CATEGORY d3d_message_category )
{
    switch( d3d_message_category )
    {
        default:
            *d3d_message_category_11 = (D3D11_MESSAGE_CATEGORY)d3d_message_category;
            break;
        case D3D_MESSAGE_CATEGORY_APPLICATION_DEFINED:
            *d3d_message_category_11 = D3D11_MESSAGE_CATEGORY_APPLICATION_DEFINED;
            break;
        case D3D_MESSAGE_CATEGORY_MISCELLANEOUS:
            *d3d_message_category_11 = D3D11_MESSAGE_CATEGORY_MISCELLANEOUS;
            break;
        case D3D_MESSAGE_CATEGORY_INITIALIZATION:
            *d3d_message_category_11 = D3D11_MESSAGE_CATEGORY_INITIALIZATION;
            break;
        case D3D_MESSAGE_CATEGORY_CLEANUP:
            *d3d_message_category_11 = D3D11_MESSAGE_CATEGORY_CLEANUP;
            break;
        case D3D_MESSAGE_CATEGORY_COMPILATION:
            *d3d_message_category_11 = D3D11_MESSAGE_CATEGORY_COMPILATION;
            break;
        case D3D_MESSAGE_CATEGORY_STATE_CREATION:
            *d3d_message_category_11 = D3D11_MESSAGE_CATEGORY_STATE_CREATION;
            break;
        case D3D_MESSAGE_CATEGORY_STATE_SETTING:
            *d3d_message_category_11 = D3D11_MESSAGE_CATEGORY_STATE_SETTING;
            break;
        case D3D_MESSAGE_CATEGORY_STATE_GETTING:
            *d3d_message_category_11 = D3D11_MESSAGE_CATEGORY_STATE_GETTING;
            break;
        case D3D_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
            *d3d_message_category_11 = D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION;
            break;
        case D3D_MESSAGE_CATEGORY_EXECUTION:
            *d3d_message_category_11 = D3D11_MESSAGE_CATEGORY_EXECUTION;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_MESSAGE_SEVERITY *d3d_message_severity, D3D11_MESSAGE_SEVERITY d3d_message_severity_11 )
{
    switch( d3d_message_severity_11 )
    {
        default:
            *d3d_message_severity = (D3D_MESSAGE_SEVERITY)d3d_message_severity_11;
            break;
        case D3D11_MESSAGE_SEVERITY_CORRUPTION:
            *d3d_message_severity = D3D_MESSAGE_SEVERITY_CORRUPTION;
            break;
        case D3D11_MESSAGE_SEVERITY_ERROR:
            *d3d_message_severity = D3D_MESSAGE_SEVERITY_ERROR;
            break;
        case D3D11_MESSAGE_SEVERITY_WARNING:
            *d3d_message_severity = D3D_MESSAGE_SEVERITY_WARNING;
            break;
        case D3D11_MESSAGE_SEVERITY_INFO:
            *d3d_message_severity = D3D_MESSAGE_SEVERITY_INFO;
            break;
    }
}

void D3DWrapperConvertEnum( D3D11_MESSAGE_SEVERITY *d3d_message_severity_11, D3D_MESSAGE_SEVERITY d3d_message_severity )
{
    switch( d3d_message_severity )
    {
        default:
            *d3d_message_severity_11 = (D3D11_MESSAGE_SEVERITY)d3d_message_severity;
            break;
        case D3D_MESSAGE_SEVERITY_CORRUPTION:
            *d3d_message_severity_11 = D3D11_MESSAGE_SEVERITY_CORRUPTION;
            break;
        case D3D_MESSAGE_SEVERITY_ERROR:
            *d3d_message_severity_11 = D3D11_MESSAGE_SEVERITY_ERROR;
            break;
        case D3D_MESSAGE_SEVERITY_WARNING:
            *d3d_message_severity_11 = D3D11_MESSAGE_SEVERITY_WARNING;
            break;
        case D3D_MESSAGE_SEVERITY_INFO:
            *d3d_message_severity_11 = D3D11_MESSAGE_SEVERITY_INFO;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_DRIVER_TYPE *d3d_driver_type, D3D_DRIVER_TYPE d3d_driver_type_11 )
{
    switch( d3d_driver_type_11 )
    {
        default:
            *d3d_driver_type = (D3D_DRIVER_TYPE)d3d_driver_type_11;
            break;
        case D3D_DRIVER_TYPE_UNKNOWN:
            *d3d_driver_type = D3D_DRIVER_TYPE_UNKNOWN;
            break;
        case D3D_DRIVER_TYPE_HARDWARE:
            *d3d_driver_type = D3D_DRIVER_TYPE_HARDWARE;
            break;
        case D3D_DRIVER_TYPE_REFERENCE:
            *d3d_driver_type = D3D_DRIVER_TYPE_REFERENCE;
            break;
        case D3D_DRIVER_TYPE_NULL:
            *d3d_driver_type = D3D_DRIVER_TYPE_NULL;
            break;
        case D3D_DRIVER_TYPE_SOFTWARE:
            *d3d_driver_type = D3D_DRIVER_TYPE_SOFTWARE;
            break;
        case D3D_DRIVER_TYPE_WARP:
            *d3d_driver_type = D3D_DRIVER_TYPE_WARP;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_FEATURE_LEVEL *d3d_feature_level, D3D_FEATURE_LEVEL d3d_feature_level_11 )
{
    switch( d3d_feature_level_11 )
    {
        default:
            *d3d_feature_level = (D3D_FEATURE_LEVEL)d3d_feature_level_11;
            break;
        case D3D_FEATURE_LEVEL_9_1:
            *d3d_feature_level = D3D_FEATURE_LEVEL_9_1;
            break;
        case D3D_FEATURE_LEVEL_9_2:
            *d3d_feature_level = D3D_FEATURE_LEVEL_9_2;
            break;
        case D3D_FEATURE_LEVEL_9_3:
            *d3d_feature_level = D3D_FEATURE_LEVEL_9_3;
            break;
        case D3D_FEATURE_LEVEL_10_0:
            *d3d_feature_level = D3D_FEATURE_LEVEL_10_0;
            break;
        case D3D_FEATURE_LEVEL_10_1:
            *d3d_feature_level = D3D_FEATURE_LEVEL_10_1;
            break;
        case D3D_FEATURE_LEVEL_11_0:
            *d3d_feature_level = D3D_FEATURE_LEVEL_11_0;
            break;
    }
}

void D3DWrapperConvertEnum( D3D_MESSAGE_ID *d3d_message_id, D3D11_MESSAGE_ID d3d_message_id_11 )
{
    *d3d_message_id = (D3D_MESSAGE_ID)d3d_message_id_11;
}

void D3DWrapperConvertEnum( D3D11_MESSAGE_ID *d3d_message_id_11, D3D_MESSAGE_ID d3d_message_id )
{
    *d3d_message_id_11 = (D3D11_MESSAGE_ID)d3d_message_id;
}

//
// Struct Conversions
//

void D3DWrapperConvertStruct( D3D_INPUT_ELEMENT_DESC *d3d_input_element_desc, D3D11_INPUT_ELEMENT_DESC const *d3d_input_element_desc_11 )
{
    d3d_input_element_desc->SemanticName = d3d_input_element_desc_11->SemanticName;
    d3d_input_element_desc->SemanticIndex = d3d_input_element_desc_11->SemanticIndex;
    d3d_input_element_desc->Format = d3d_input_element_desc_11->Format;
    d3d_input_element_desc->InputSlot = d3d_input_element_desc_11->InputSlot;
    d3d_input_element_desc->AlignedByteOffset = d3d_input_element_desc_11->AlignedByteOffset;
    D3DWrapperConvertEnum( &d3d_input_element_desc->InputSlotClass, d3d_input_element_desc_11->InputSlotClass );
    d3d_input_element_desc->InstanceDataStepRate = d3d_input_element_desc_11->InstanceDataStepRate;
}

void D3DWrapperConvertStruct( D3D11_INPUT_ELEMENT_DESC *d3d_input_element_desc_11, D3D_INPUT_ELEMENT_DESC const *d3d_input_element_desc )
{
    d3d_input_element_desc_11->SemanticName = d3d_input_element_desc->SemanticName;
    d3d_input_element_desc_11->SemanticIndex = d3d_input_element_desc->SemanticIndex;
    d3d_input_element_desc_11->Format = d3d_input_element_desc->Format;
    d3d_input_element_desc_11->InputSlot = d3d_input_element_desc->InputSlot;
    d3d_input_element_desc_11->AlignedByteOffset = d3d_input_element_desc->AlignedByteOffset;
    D3DWrapperConvertEnum( &d3d_input_element_desc_11->InputSlotClass, d3d_input_element_desc->InputSlotClass );
    d3d_input_element_desc_11->InstanceDataStepRate = d3d_input_element_desc->InstanceDataStepRate;
}

void D3DWrapperConvertStruct( D3D_SO_DECLARATION_ENTRY *d3d_so_declaration_entry, D3D11_SO_DECLARATION_ENTRY const *d3d_so_declaration_entry_11 )
{
    d3d_so_declaration_entry->SemanticName = d3d_so_declaration_entry_11->SemanticName;
    d3d_so_declaration_entry->SemanticIndex = d3d_so_declaration_entry_11->SemanticIndex;
    d3d_so_declaration_entry->StartComponent = d3d_so_declaration_entry_11->StartComponent;
    d3d_so_declaration_entry->ComponentCount = d3d_so_declaration_entry_11->ComponentCount;
    d3d_so_declaration_entry->OutputSlot = d3d_so_declaration_entry_11->OutputSlot;
}

void D3DWrapperConvertStruct( D3D11_SO_DECLARATION_ENTRY *d3d_so_declaration_entry_11, D3D_SO_DECLARATION_ENTRY const *d3d_so_declaration_entry )
{
    d3d_so_declaration_entry_11->Stream = 0;
    d3d_so_declaration_entry_11->SemanticName = d3d_so_declaration_entry->SemanticName;
    d3d_so_declaration_entry_11->SemanticIndex = d3d_so_declaration_entry->SemanticIndex;
    d3d_so_declaration_entry_11->StartComponent = d3d_so_declaration_entry->StartComponent;
    d3d_so_declaration_entry_11->ComponentCount = d3d_so_declaration_entry->ComponentCount;
    d3d_so_declaration_entry_11->OutputSlot = d3d_so_declaration_entry->OutputSlot;
}

void D3DWrapperConvertStruct( D3D_BOX *d3d_box, D3D11_BOX const *d3d_box_11 )
{
    d3d_box->left = d3d_box_11->left;
    d3d_box->top = d3d_box_11->top;
    d3d_box->front = d3d_box_11->front;
    d3d_box->right = d3d_box_11->right;
    d3d_box->bottom = d3d_box_11->bottom;
    d3d_box->back = d3d_box_11->back;
}

void D3DWrapperConvertStruct( D3D11_BOX *d3d_box_11, D3D_BOX const *d3d_box )
{
    d3d_box_11->left = d3d_box->left;
    d3d_box_11->top = d3d_box->top;
    d3d_box_11->front = d3d_box->front;
    d3d_box_11->right = d3d_box->right;
    d3d_box_11->bottom = d3d_box->bottom;
    d3d_box_11->back = d3d_box->back;
}

void D3DWrapperConvertStruct( D3D_DEPTH_STENCILOP_DESC *d3d_depth_stencilop_desc, D3D11_DEPTH_STENCILOP_DESC const *d3d_depth_stencilop_desc_11 )
{
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc->StencilFailOp, d3d_depth_stencilop_desc_11->StencilFailOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc->StencilDepthFailOp, d3d_depth_stencilop_desc_11->StencilDepthFailOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc->StencilPassOp, d3d_depth_stencilop_desc_11->StencilPassOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc->StencilFunc, d3d_depth_stencilop_desc_11->StencilFunc );
}

void D3DWrapperConvertStruct( D3D11_DEPTH_STENCILOP_DESC *d3d_depth_stencilop_desc_11, D3D_DEPTH_STENCILOP_DESC const *d3d_depth_stencilop_desc )
{
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc_11->StencilFailOp, d3d_depth_stencilop_desc->StencilFailOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc_11->StencilDepthFailOp, d3d_depth_stencilop_desc->StencilDepthFailOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc_11->StencilPassOp, d3d_depth_stencilop_desc->StencilPassOp );
    D3DWrapperConvertEnum( &d3d_depth_stencilop_desc_11->StencilFunc, d3d_depth_stencilop_desc->StencilFunc );
}

void D3DWrapperConvertStruct( D3D_DEPTH_STENCIL_DESC *d3d_depth_stencil_desc, D3D11_DEPTH_STENCIL_DESC const *d3d_depth_stencil_desc_11 )
{
    d3d_depth_stencil_desc->DepthEnable = d3d_depth_stencil_desc_11->DepthEnable;
    D3DWrapperConvertEnum( &d3d_depth_stencil_desc->DepthWriteMask, d3d_depth_stencil_desc_11->DepthWriteMask );
    D3DWrapperConvertEnum( &d3d_depth_stencil_desc->DepthFunc, d3d_depth_stencil_desc_11->DepthFunc );
    d3d_depth_stencil_desc->StencilEnable = d3d_depth_stencil_desc_11->StencilEnable;
    d3d_depth_stencil_desc->StencilReadMask = d3d_depth_stencil_desc_11->StencilReadMask;
    d3d_depth_stencil_desc->StencilWriteMask = d3d_depth_stencil_desc_11->StencilWriteMask;
    D3DWrapperConvertStruct( &d3d_depth_stencil_desc->FrontFace, &d3d_depth_stencil_desc_11->FrontFace );
    D3DWrapperConvertStruct( &d3d_depth_stencil_desc->BackFace, &d3d_depth_stencil_desc_11->BackFace );
}

void D3DWrapperConvertStruct( D3D11_DEPTH_STENCIL_DESC *d3d_depth_stencil_desc_11, D3D_DEPTH_STENCIL_DESC const *d3d_depth_stencil_desc )
{
    d3d_depth_stencil_desc_11->DepthEnable = d3d_depth_stencil_desc->DepthEnable;
    D3DWrapperConvertEnum( &d3d_depth_stencil_desc_11->DepthWriteMask, d3d_depth_stencil_desc->DepthWriteMask );
    D3DWrapperConvertEnum( &d3d_depth_stencil_desc_11->DepthFunc, d3d_depth_stencil_desc->DepthFunc );
    d3d_depth_stencil_desc_11->StencilEnable = d3d_depth_stencil_desc->StencilEnable;
    d3d_depth_stencil_desc_11->StencilReadMask = d3d_depth_stencil_desc->StencilReadMask;
    d3d_depth_stencil_desc_11->StencilWriteMask = d3d_depth_stencil_desc->StencilWriteMask;
    D3DWrapperConvertStruct( &d3d_depth_stencil_desc_11->FrontFace, &d3d_depth_stencil_desc->FrontFace );
    D3DWrapperConvertStruct( &d3d_depth_stencil_desc_11->BackFace, &d3d_depth_stencil_desc->BackFace );
}

void D3DWrapperConvertStruct( D3D_RASTERIZER_DESC *d3d_rasterizer_desc, D3D11_RASTERIZER_DESC const *d3d_rasterizer_desc_11 )
{
    D3DWrapperConvertEnum( &d3d_rasterizer_desc->FillMode, d3d_rasterizer_desc_11->FillMode );
    D3DWrapperConvertEnum( &d3d_rasterizer_desc->CullMode, d3d_rasterizer_desc_11->CullMode );
    d3d_rasterizer_desc->FrontCounterClockwise = d3d_rasterizer_desc_11->FrontCounterClockwise;
    d3d_rasterizer_desc->DepthBias = d3d_rasterizer_desc_11->DepthBias;
    d3d_rasterizer_desc->DepthBiasClamp = d3d_rasterizer_desc_11->DepthBiasClamp;
    d3d_rasterizer_desc->SlopeScaledDepthBias = d3d_rasterizer_desc_11->SlopeScaledDepthBias;
    d3d_rasterizer_desc->DepthClipEnable = d3d_rasterizer_desc_11->DepthClipEnable;
    d3d_rasterizer_desc->ScissorEnable = d3d_rasterizer_desc_11->ScissorEnable;
    d3d_rasterizer_desc->MultisampleEnable = d3d_rasterizer_desc_11->MultisampleEnable;
    d3d_rasterizer_desc->AntialiasedLineEnable = d3d_rasterizer_desc_11->AntialiasedLineEnable;
}

void D3DWrapperConvertStruct( D3D11_RASTERIZER_DESC *d3d_rasterizer_desc_11, D3D_RASTERIZER_DESC const *d3d_rasterizer_desc )
{
    D3DWrapperConvertEnum( &d3d_rasterizer_desc_11->FillMode, d3d_rasterizer_desc->FillMode );
    D3DWrapperConvertEnum( &d3d_rasterizer_desc_11->CullMode, d3d_rasterizer_desc->CullMode );
    d3d_rasterizer_desc_11->FrontCounterClockwise = d3d_rasterizer_desc->FrontCounterClockwise;
    d3d_rasterizer_desc_11->DepthBias = d3d_rasterizer_desc->DepthBias;
    d3d_rasterizer_desc_11->DepthBiasClamp = d3d_rasterizer_desc->DepthBiasClamp;
    d3d_rasterizer_desc_11->SlopeScaledDepthBias = d3d_rasterizer_desc->SlopeScaledDepthBias;
    d3d_rasterizer_desc_11->DepthClipEnable = d3d_rasterizer_desc->DepthClipEnable;
    d3d_rasterizer_desc_11->ScissorEnable = d3d_rasterizer_desc->ScissorEnable;
    d3d_rasterizer_desc_11->MultisampleEnable = d3d_rasterizer_desc->MultisampleEnable;
    d3d_rasterizer_desc_11->AntialiasedLineEnable = d3d_rasterizer_desc->AntialiasedLineEnable;
}

void D3DWrapperConvertStruct( D3D_SUBRESOURCE_DATA *d3d_subresource_data, D3D11_SUBRESOURCE_DATA const *d3d_subresource_data_11 )
{
    d3d_subresource_data->pSysMem = d3d_subresource_data_11->pSysMem;
    d3d_subresource_data->SysMemPitch = d3d_subresource_data_11->SysMemPitch;
    d3d_subresource_data->SysMemSlicePitch = d3d_subresource_data_11->SysMemSlicePitch;
}

void D3DWrapperConvertStruct( D3D11_SUBRESOURCE_DATA *d3d_subresource_data_11, D3D_SUBRESOURCE_DATA const *d3d_subresource_data )
{
    d3d_subresource_data_11->pSysMem = d3d_subresource_data->pSysMem;
    d3d_subresource_data_11->SysMemPitch = d3d_subresource_data->SysMemPitch;
    d3d_subresource_data_11->SysMemSlicePitch = d3d_subresource_data->SysMemSlicePitch;
}

void D3DWrapperConvertStruct( D3D_BUFFER_DESC *d3d_buffer_desc, D3D11_BUFFER_DESC const *d3d_buffer_desc_11 )
{
    d3d_buffer_desc->ByteWidth = d3d_buffer_desc_11->ByteWidth;
    D3DWrapperConvertEnum( &d3d_buffer_desc->Usage, d3d_buffer_desc_11->Usage );
    D3DFlagsConvertFrom11_BIND_FLAG( &d3d_buffer_desc->BindFlags, d3d_buffer_desc_11->BindFlags );
    D3DFlagsConvertFrom11_CPU_ACCESS_FLAG( &d3d_buffer_desc->CPUAccessFlags, d3d_buffer_desc_11->CPUAccessFlags );
    D3DFlagsConvertFrom11_RESOURCE_MISC_FLAG( &d3d_buffer_desc->MiscFlags, d3d_buffer_desc_11->MiscFlags );
}

void D3DWrapperConvertStruct( D3D11_BUFFER_DESC *d3d_buffer_desc_11, D3D_BUFFER_DESC const *d3d_buffer_desc )
{
    d3d_buffer_desc_11->ByteWidth = d3d_buffer_desc->ByteWidth;
    D3DWrapperConvertEnum( &d3d_buffer_desc_11->Usage, d3d_buffer_desc->Usage );
    D3DFlagsConvertTo11_BIND_FLAG( &d3d_buffer_desc_11->BindFlags, d3d_buffer_desc->BindFlags );
    D3DFlagsConvertTo11_CPU_ACCESS_FLAG( &d3d_buffer_desc_11->CPUAccessFlags, d3d_buffer_desc->CPUAccessFlags );
    D3DFlagsConvertTo11_RESOURCE_MISC_FLAG( &d3d_buffer_desc_11->MiscFlags, d3d_buffer_desc->MiscFlags );
    d3d_buffer_desc_11->StructureByteStride = 0;
}

void D3DWrapperConvertStruct( D3D_TEXTURE1D_DESC *d3d_texture1d_desc, D3D11_TEXTURE1D_DESC const *d3d_texture1d_desc_11 )
{
    d3d_texture1d_desc->Width = d3d_texture1d_desc_11->Width;
    d3d_texture1d_desc->MipLevels = d3d_texture1d_desc_11->MipLevels;
    d3d_texture1d_desc->ArraySize = d3d_texture1d_desc_11->ArraySize;
    d3d_texture1d_desc->Format = d3d_texture1d_desc_11->Format;
    D3DWrapperConvertEnum( &d3d_texture1d_desc->Usage, d3d_texture1d_desc_11->Usage );
    D3DFlagsConvertFrom11_BIND_FLAG( &d3d_texture1d_desc->BindFlags, d3d_texture1d_desc_11->BindFlags );
    D3DFlagsConvertFrom11_CPU_ACCESS_FLAG( &d3d_texture1d_desc->CPUAccessFlags, d3d_texture1d_desc_11->CPUAccessFlags );
    D3DFlagsConvertFrom11_RESOURCE_MISC_FLAG( &d3d_texture1d_desc->MiscFlags, d3d_texture1d_desc_11->MiscFlags );
}

void D3DWrapperConvertStruct( D3D11_TEXTURE1D_DESC *d3d_texture1d_desc_11, D3D_TEXTURE1D_DESC const *d3d_texture1d_desc )
{
    d3d_texture1d_desc_11->Width = d3d_texture1d_desc->Width;
    d3d_texture1d_desc_11->MipLevels = d3d_texture1d_desc->MipLevels;
    d3d_texture1d_desc_11->ArraySize = d3d_texture1d_desc->ArraySize;
    d3d_texture1d_desc_11->Format = d3d_texture1d_desc->Format;
    D3DWrapperConvertEnum( &d3d_texture1d_desc_11->Usage, d3d_texture1d_desc->Usage );
    D3DFlagsConvertTo11_BIND_FLAG( &d3d_texture1d_desc_11->BindFlags, d3d_texture1d_desc->BindFlags );
    D3DFlagsConvertTo11_CPU_ACCESS_FLAG( &d3d_texture1d_desc_11->CPUAccessFlags, d3d_texture1d_desc->CPUAccessFlags );
    D3DFlagsConvertTo11_RESOURCE_MISC_FLAG( &d3d_texture1d_desc_11->MiscFlags, d3d_texture1d_desc->MiscFlags );
}

void D3DWrapperConvertStruct( D3D_TEXTURE2D_DESC *d3d_texture2d_desc, D3D11_TEXTURE2D_DESC const *d3d_texture2d_desc_11 )
{
    d3d_texture2d_desc->Width = d3d_texture2d_desc_11->Width;
    d3d_texture2d_desc->Height = d3d_texture2d_desc_11->Height;
    d3d_texture2d_desc->MipLevels = d3d_texture2d_desc_11->MipLevels;
    d3d_texture2d_desc->ArraySize = d3d_texture2d_desc_11->ArraySize;
    d3d_texture2d_desc->Format = d3d_texture2d_desc_11->Format;
    d3d_texture2d_desc->SampleDesc = d3d_texture2d_desc_11->SampleDesc;
    D3DWrapperConvertEnum( &d3d_texture2d_desc->Usage, d3d_texture2d_desc_11->Usage );
    D3DFlagsConvertFrom11_BIND_FLAG( &d3d_texture2d_desc->BindFlags, d3d_texture2d_desc_11->BindFlags );
    D3DFlagsConvertFrom11_CPU_ACCESS_FLAG( &d3d_texture2d_desc->CPUAccessFlags, d3d_texture2d_desc_11->CPUAccessFlags );
    D3DFlagsConvertFrom11_RESOURCE_MISC_FLAG( &d3d_texture2d_desc->MiscFlags, d3d_texture2d_desc_11->MiscFlags );
}

void D3DWrapperConvertStruct( D3D11_TEXTURE2D_DESC *d3d_texture2d_desc_11, D3D_TEXTURE2D_DESC const *d3d_texture2d_desc )
{
    d3d_texture2d_desc_11->Width = d3d_texture2d_desc->Width;
    d3d_texture2d_desc_11->Height = d3d_texture2d_desc->Height;
    d3d_texture2d_desc_11->MipLevels = d3d_texture2d_desc->MipLevels;
    d3d_texture2d_desc_11->ArraySize = d3d_texture2d_desc->ArraySize;
    d3d_texture2d_desc_11->Format = d3d_texture2d_desc->Format;
    d3d_texture2d_desc_11->SampleDesc = d3d_texture2d_desc->SampleDesc;
    D3DWrapperConvertEnum( &d3d_texture2d_desc_11->Usage, d3d_texture2d_desc->Usage );
    D3DFlagsConvertTo11_BIND_FLAG( &d3d_texture2d_desc_11->BindFlags, d3d_texture2d_desc->BindFlags );
    D3DFlagsConvertTo11_CPU_ACCESS_FLAG( &d3d_texture2d_desc_11->CPUAccessFlags, d3d_texture2d_desc->CPUAccessFlags );
    D3DFlagsConvertTo11_RESOURCE_MISC_FLAG( &d3d_texture2d_desc_11->MiscFlags, d3d_texture2d_desc->MiscFlags );
}

void D3DWrapperConvertStruct( D3D_TEXTURE3D_DESC *d3d_texture3d_desc, D3D11_TEXTURE3D_DESC const *d3d_texture3d_desc_11 )
{
    d3d_texture3d_desc->Width = d3d_texture3d_desc_11->Width;
    d3d_texture3d_desc->Height = d3d_texture3d_desc_11->Height;
    d3d_texture3d_desc->Depth = d3d_texture3d_desc_11->Depth;
    d3d_texture3d_desc->MipLevels = d3d_texture3d_desc_11->MipLevels;
    d3d_texture3d_desc->Format = d3d_texture3d_desc_11->Format;
    D3DWrapperConvertEnum( &d3d_texture3d_desc->Usage, d3d_texture3d_desc_11->Usage );
    D3DFlagsConvertFrom11_BIND_FLAG( &d3d_texture3d_desc->BindFlags, d3d_texture3d_desc_11->BindFlags );
    D3DFlagsConvertFrom11_CPU_ACCESS_FLAG( &d3d_texture3d_desc->CPUAccessFlags, d3d_texture3d_desc_11->CPUAccessFlags );
    D3DFlagsConvertFrom11_RESOURCE_MISC_FLAG( &d3d_texture3d_desc->MiscFlags, d3d_texture3d_desc_11->MiscFlags );
}

void D3DWrapperConvertStruct( D3D11_TEXTURE3D_DESC *d3d_texture3d_desc_11, D3D_TEXTURE3D_DESC const *d3d_texture3d_desc )
{
    d3d_texture3d_desc_11->Width = d3d_texture3d_desc->Width;
    d3d_texture3d_desc_11->Height = d3d_texture3d_desc->Height;
    d3d_texture3d_desc_11->Depth = d3d_texture3d_desc->Depth;
    d3d_texture3d_desc_11->MipLevels = d3d_texture3d_desc->MipLevels;
    d3d_texture3d_desc_11->Format = d3d_texture3d_desc->Format;
    D3DWrapperConvertEnum( &d3d_texture3d_desc_11->Usage, d3d_texture3d_desc->Usage );
    D3DFlagsConvertTo11_BIND_FLAG( &d3d_texture3d_desc_11->BindFlags, d3d_texture3d_desc->BindFlags );
    D3DFlagsConvertTo11_CPU_ACCESS_FLAG( &d3d_texture3d_desc_11->CPUAccessFlags, d3d_texture3d_desc->CPUAccessFlags );
    D3DFlagsConvertTo11_RESOURCE_MISC_FLAG( &d3d_texture3d_desc_11->MiscFlags, d3d_texture3d_desc->MiscFlags );
}

void D3DWrapperConvertStruct( D3D_BUFFER_SRV *d3d_buffer_srv, D3D11_BUFFER_SRV const *d3d_buffer_srv_11 )
{
}

void D3DWrapperConvertStruct( D3D11_BUFFER_SRV *d3d_buffer_srv_11, D3D_BUFFER_SRV const *d3d_buffer_srv )
{
}

void D3DWrapperConvertStruct( D3D_TEX1D_SRV *d3d_tex1d_srv, D3D11_TEX1D_SRV const *d3d_tex1d_srv_11 )
{
    d3d_tex1d_srv->MostDetailedMip = d3d_tex1d_srv_11->MostDetailedMip;
    d3d_tex1d_srv->MipLevels = d3d_tex1d_srv_11->MipLevels;
}

void D3DWrapperConvertStruct( D3D11_TEX1D_SRV *d3d_tex1d_srv_11, D3D_TEX1D_SRV const *d3d_tex1d_srv )
{
    d3d_tex1d_srv_11->MostDetailedMip = d3d_tex1d_srv->MostDetailedMip;
    d3d_tex1d_srv_11->MipLevels = d3d_tex1d_srv->MipLevels;
}

void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_SRV *d3d_tex1d_array_srv, D3D11_TEX1D_ARRAY_SRV const *d3d_tex1d_array_srv_11 )
{
    d3d_tex1d_array_srv->MostDetailedMip = d3d_tex1d_array_srv_11->MostDetailedMip;
    d3d_tex1d_array_srv->MipLevels = d3d_tex1d_array_srv_11->MipLevels;
    d3d_tex1d_array_srv->FirstArraySlice = d3d_tex1d_array_srv_11->FirstArraySlice;
    d3d_tex1d_array_srv->ArraySize = d3d_tex1d_array_srv_11->ArraySize;
}

void D3DWrapperConvertStruct( D3D11_TEX1D_ARRAY_SRV *d3d_tex1d_array_srv_11, D3D_TEX1D_ARRAY_SRV const *d3d_tex1d_array_srv )
{
    d3d_tex1d_array_srv_11->MostDetailedMip = d3d_tex1d_array_srv->MostDetailedMip;
    d3d_tex1d_array_srv_11->MipLevels = d3d_tex1d_array_srv->MipLevels;
    d3d_tex1d_array_srv_11->FirstArraySlice = d3d_tex1d_array_srv->FirstArraySlice;
    d3d_tex1d_array_srv_11->ArraySize = d3d_tex1d_array_srv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX2D_SRV *d3d_tex2d_srv, D3D11_TEX2D_SRV const *d3d_tex2d_srv_11 )
{
    d3d_tex2d_srv->MostDetailedMip = d3d_tex2d_srv_11->MostDetailedMip;
    d3d_tex2d_srv->MipLevels = d3d_tex2d_srv_11->MipLevels;
}

void D3DWrapperConvertStruct( D3D11_TEX2D_SRV *d3d_tex2d_srv_11, D3D_TEX2D_SRV const *d3d_tex2d_srv )
{
    d3d_tex2d_srv_11->MostDetailedMip = d3d_tex2d_srv->MostDetailedMip;
    d3d_tex2d_srv_11->MipLevels = d3d_tex2d_srv->MipLevels;
}

void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_SRV *d3d_tex2d_array_srv, D3D11_TEX2D_ARRAY_SRV const *d3d_tex2d_array_srv_11 )
{
    d3d_tex2d_array_srv->MostDetailedMip = d3d_tex2d_array_srv_11->MostDetailedMip;
    d3d_tex2d_array_srv->MipLevels = d3d_tex2d_array_srv_11->MipLevels;
    d3d_tex2d_array_srv->FirstArraySlice = d3d_tex2d_array_srv_11->FirstArraySlice;
    d3d_tex2d_array_srv->ArraySize = d3d_tex2d_array_srv_11->ArraySize;
}

void D3DWrapperConvertStruct( D3D11_TEX2D_ARRAY_SRV *d3d_tex2d_array_srv_11, D3D_TEX2D_ARRAY_SRV const *d3d_tex2d_array_srv )
{
    d3d_tex2d_array_srv_11->MostDetailedMip = d3d_tex2d_array_srv->MostDetailedMip;
    d3d_tex2d_array_srv_11->MipLevels = d3d_tex2d_array_srv->MipLevels;
    d3d_tex2d_array_srv_11->FirstArraySlice = d3d_tex2d_array_srv->FirstArraySlice;
    d3d_tex2d_array_srv_11->ArraySize = d3d_tex2d_array_srv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX3D_SRV *d3d_tex3d_srv, D3D11_TEX3D_SRV const *d3d_tex3d_srv_11 )
{
    d3d_tex3d_srv->MostDetailedMip = d3d_tex3d_srv_11->MostDetailedMip;
    d3d_tex3d_srv->MipLevels = d3d_tex3d_srv_11->MipLevels;
}

void D3DWrapperConvertStruct( D3D11_TEX3D_SRV *d3d_tex3d_srv_11, D3D_TEX3D_SRV const *d3d_tex3d_srv )
{
    d3d_tex3d_srv_11->MostDetailedMip = d3d_tex3d_srv->MostDetailedMip;
    d3d_tex3d_srv_11->MipLevels = d3d_tex3d_srv->MipLevels;
}

void D3DWrapperConvertStruct( D3D_TEXCUBE_SRV *d3d_texcube_srv, D3D11_TEXCUBE_SRV const *d3d_texcube_srv_11 )
{
    d3d_texcube_srv->MostDetailedMip = d3d_texcube_srv_11->MostDetailedMip;
    d3d_texcube_srv->MipLevels = d3d_texcube_srv_11->MipLevels;
}

void D3DWrapperConvertStruct( D3D11_TEXCUBE_SRV *d3d_texcube_srv_11, D3D_TEXCUBE_SRV const *d3d_texcube_srv )
{
    d3d_texcube_srv_11->MostDetailedMip = d3d_texcube_srv->MostDetailedMip;
    d3d_texcube_srv_11->MipLevels = d3d_texcube_srv->MipLevels;
}

void D3DWrapperConvertStruct( D3D_TEXCUBE_ARRAY_SRV1 *d3d_texcube_array_srv1, D3D11_TEXCUBE_ARRAY_SRV const *d3d_texcube_array_srv1_11 )
{
    d3d_texcube_array_srv1->MostDetailedMip = d3d_texcube_array_srv1_11->MostDetailedMip;
    d3d_texcube_array_srv1->MipLevels = d3d_texcube_array_srv1_11->MipLevels;
    d3d_texcube_array_srv1->First2DArrayFace = d3d_texcube_array_srv1_11->First2DArrayFace;
    d3d_texcube_array_srv1->NumCubes = d3d_texcube_array_srv1_11->NumCubes;
}

void D3DWrapperConvertStruct( D3D11_TEXCUBE_ARRAY_SRV *d3d_texcube_array_srv1_11, D3D_TEXCUBE_ARRAY_SRV1 const *d3d_texcube_array_srv1 )
{
    d3d_texcube_array_srv1_11->MostDetailedMip = d3d_texcube_array_srv1->MostDetailedMip;
    d3d_texcube_array_srv1_11->MipLevels = d3d_texcube_array_srv1->MipLevels;
    d3d_texcube_array_srv1_11->First2DArrayFace = d3d_texcube_array_srv1->First2DArrayFace;
    d3d_texcube_array_srv1_11->NumCubes = d3d_texcube_array_srv1->NumCubes;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_SRV *d3d_tex2dms_srv, D3D11_TEX2DMS_SRV const *d3d_tex2dms_srv_11 )
{
    d3d_tex2dms_srv->UnusedField_NothingToDefine = d3d_tex2dms_srv_11->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D11_TEX2DMS_SRV *d3d_tex2dms_srv_11, D3D_TEX2DMS_SRV const *d3d_tex2dms_srv )
{
    d3d_tex2dms_srv_11->UnusedField_NothingToDefine = d3d_tex2dms_srv->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_SRV *d3d_tex2dms_array_srv, D3D11_TEX2DMS_ARRAY_SRV const *d3d_tex2dms_array_srv_11 )
{
    d3d_tex2dms_array_srv->FirstArraySlice = d3d_tex2dms_array_srv_11->FirstArraySlice;
    d3d_tex2dms_array_srv->ArraySize = d3d_tex2dms_array_srv_11->ArraySize;
}

void D3DWrapperConvertStruct( D3D11_TEX2DMS_ARRAY_SRV *d3d_tex2dms_array_srv_11, D3D_TEX2DMS_ARRAY_SRV const *d3d_tex2dms_array_srv )
{
    d3d_tex2dms_array_srv_11->FirstArraySlice = d3d_tex2dms_array_srv->FirstArraySlice;
    d3d_tex2dms_array_srv_11->ArraySize = d3d_tex2dms_array_srv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_SHADER_RESOURCE_VIEW_DESC *d3d_shader_resource_view_desc, D3D11_SHADER_RESOURCE_VIEW_DESC const *d3d_shader_resource_view_desc_11 )
{
    d3d_shader_resource_view_desc->Format = d3d_shader_resource_view_desc_11->Format;
    D3DWrapperConvertEnum( &d3d_shader_resource_view_desc->ViewDimension, d3d_shader_resource_view_desc_11->ViewDimension );
    switch( d3d_shader_resource_view_desc_11->ViewDimension )
    {
        case D3D11_SRV_DIMENSION_BUFFER:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Buffer, &d3d_shader_resource_view_desc_11->Buffer );
            break;
        case D3D11_SRV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture1D, &d3d_shader_resource_view_desc_11->Texture1D );
            break;
        case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture1DArray, &d3d_shader_resource_view_desc_11->Texture1DArray );
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture2D, &d3d_shader_resource_view_desc_11->Texture2D );
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture2DArray, &d3d_shader_resource_view_desc_11->Texture2DArray );
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture2DMS, &d3d_shader_resource_view_desc_11->Texture2DMS );
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture2DMSArray, &d3d_shader_resource_view_desc_11->Texture2DMSArray );
            break;
        case D3D11_SRV_DIMENSION_TEXTURE3D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->Texture3D, &d3d_shader_resource_view_desc_11->Texture3D );
            break;
        case D3D11_SRV_DIMENSION_TEXTURECUBE:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc->TextureCube, &d3d_shader_resource_view_desc_11->TextureCube );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D11_SHADER_RESOURCE_VIEW_DESC *d3d_shader_resource_view_desc_11, D3D_SHADER_RESOURCE_VIEW_DESC const *d3d_shader_resource_view_desc )
{
    d3d_shader_resource_view_desc_11->Format = d3d_shader_resource_view_desc->Format;
    D3DWrapperConvertEnum( &d3d_shader_resource_view_desc_11->ViewDimension, d3d_shader_resource_view_desc->ViewDimension );
    switch( d3d_shader_resource_view_desc->ViewDimension )
    {
        case D3D_SRV_DIMENSION_BUFFER:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_11->Buffer, &d3d_shader_resource_view_desc->Buffer );
            break;
        case D3D_SRV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_11->Texture1D, &d3d_shader_resource_view_desc->Texture1D );
            break;
        case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_11->Texture1DArray, &d3d_shader_resource_view_desc->Texture1DArray );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_11->Texture2D, &d3d_shader_resource_view_desc->Texture2D );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_11->Texture2DArray, &d3d_shader_resource_view_desc->Texture2DArray );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_11->Texture2DMS, &d3d_shader_resource_view_desc->Texture2DMS );
            break;
        case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_11->Texture2DMSArray, &d3d_shader_resource_view_desc->Texture2DMSArray );
            break;
        case D3D_SRV_DIMENSION_TEXTURE3D:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_11->Texture3D, &d3d_shader_resource_view_desc->Texture3D );
            break;
        case D3D_SRV_DIMENSION_TEXTURECUBE:
            D3DWrapperConvertStruct( &d3d_shader_resource_view_desc_11->TextureCube, &d3d_shader_resource_view_desc->TextureCube );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D_BUFFER_RTV *d3d_buffer_rtv, D3D11_BUFFER_RTV const *d3d_buffer_rtv_11 )
{
}

void D3DWrapperConvertStruct( D3D11_BUFFER_RTV *d3d_buffer_rtv_11, D3D_BUFFER_RTV const *d3d_buffer_rtv )
{
}

void D3DWrapperConvertStruct( D3D_TEX1D_RTV *d3d_tex1d_rtv, D3D11_TEX1D_RTV const *d3d_tex1d_rtv_11 )
{
    d3d_tex1d_rtv->MipSlice = d3d_tex1d_rtv_11->MipSlice;
}

void D3DWrapperConvertStruct( D3D11_TEX1D_RTV *d3d_tex1d_rtv_11, D3D_TEX1D_RTV const *d3d_tex1d_rtv )
{
    d3d_tex1d_rtv_11->MipSlice = d3d_tex1d_rtv->MipSlice;
}

void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_RTV *d3d_tex1d_array_rtv, D3D11_TEX1D_ARRAY_RTV const *d3d_tex1d_array_rtv_11 )
{
    d3d_tex1d_array_rtv->MipSlice = d3d_tex1d_array_rtv_11->MipSlice;
    d3d_tex1d_array_rtv->FirstArraySlice = d3d_tex1d_array_rtv_11->FirstArraySlice;
    d3d_tex1d_array_rtv->ArraySize = d3d_tex1d_array_rtv_11->ArraySize;
}

void D3DWrapperConvertStruct( D3D11_TEX1D_ARRAY_RTV *d3d_tex1d_array_rtv_11, D3D_TEX1D_ARRAY_RTV const *d3d_tex1d_array_rtv )
{
    d3d_tex1d_array_rtv_11->MipSlice = d3d_tex1d_array_rtv->MipSlice;
    d3d_tex1d_array_rtv_11->FirstArraySlice = d3d_tex1d_array_rtv->FirstArraySlice;
    d3d_tex1d_array_rtv_11->ArraySize = d3d_tex1d_array_rtv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX2D_RTV *d3d_tex2d_rtv, D3D11_TEX2D_RTV const *d3d_tex2d_rtv_11 )
{
    d3d_tex2d_rtv->MipSlice = d3d_tex2d_rtv_11->MipSlice;
}

void D3DWrapperConvertStruct( D3D11_TEX2D_RTV *d3d_tex2d_rtv_11, D3D_TEX2D_RTV const *d3d_tex2d_rtv )
{
    d3d_tex2d_rtv_11->MipSlice = d3d_tex2d_rtv->MipSlice;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_RTV *d3d_tex2dms_rtv, D3D11_TEX2DMS_RTV const *d3d_tex2dms_rtv_11 )
{
    d3d_tex2dms_rtv->UnusedField_NothingToDefine = d3d_tex2dms_rtv_11->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D11_TEX2DMS_RTV *d3d_tex2dms_rtv_11, D3D_TEX2DMS_RTV const *d3d_tex2dms_rtv )
{
    d3d_tex2dms_rtv_11->UnusedField_NothingToDefine = d3d_tex2dms_rtv->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_RTV *d3d_tex2d_array_rtv, D3D11_TEX2D_ARRAY_RTV const *d3d_tex2d_array_rtv_11 )
{
    d3d_tex2d_array_rtv->MipSlice = d3d_tex2d_array_rtv_11->MipSlice;
    d3d_tex2d_array_rtv->FirstArraySlice = d3d_tex2d_array_rtv_11->FirstArraySlice;
    d3d_tex2d_array_rtv->ArraySize = d3d_tex2d_array_rtv_11->ArraySize;
}

void D3DWrapperConvertStruct( D3D11_TEX2D_ARRAY_RTV *d3d_tex2d_array_rtv_11, D3D_TEX2D_ARRAY_RTV const *d3d_tex2d_array_rtv )
{
    d3d_tex2d_array_rtv_11->MipSlice = d3d_tex2d_array_rtv->MipSlice;
    d3d_tex2d_array_rtv_11->FirstArraySlice = d3d_tex2d_array_rtv->FirstArraySlice;
    d3d_tex2d_array_rtv_11->ArraySize = d3d_tex2d_array_rtv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_RTV *d3d_tex2dms_array_rtv, D3D11_TEX2DMS_ARRAY_RTV const *d3d_tex2dms_array_rtv_11 )
{
    d3d_tex2dms_array_rtv->FirstArraySlice = d3d_tex2dms_array_rtv_11->FirstArraySlice;
    d3d_tex2dms_array_rtv->ArraySize = d3d_tex2dms_array_rtv_11->ArraySize;
}

void D3DWrapperConvertStruct( D3D11_TEX2DMS_ARRAY_RTV *d3d_tex2dms_array_rtv_11, D3D_TEX2DMS_ARRAY_RTV const *d3d_tex2dms_array_rtv )
{
    d3d_tex2dms_array_rtv_11->FirstArraySlice = d3d_tex2dms_array_rtv->FirstArraySlice;
    d3d_tex2dms_array_rtv_11->ArraySize = d3d_tex2dms_array_rtv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX3D_RTV *d3d_tex3d_rtv, D3D11_TEX3D_RTV const *d3d_tex3d_rtv_11 )
{
    d3d_tex3d_rtv->MipSlice = d3d_tex3d_rtv_11->MipSlice;
    d3d_tex3d_rtv->FirstWSlice = d3d_tex3d_rtv_11->FirstWSlice;
    d3d_tex3d_rtv->WSize = d3d_tex3d_rtv_11->WSize;
}

void D3DWrapperConvertStruct( D3D11_TEX3D_RTV *d3d_tex3d_rtv_11, D3D_TEX3D_RTV const *d3d_tex3d_rtv )
{
    d3d_tex3d_rtv_11->MipSlice = d3d_tex3d_rtv->MipSlice;
    d3d_tex3d_rtv_11->FirstWSlice = d3d_tex3d_rtv->FirstWSlice;
    d3d_tex3d_rtv_11->WSize = d3d_tex3d_rtv->WSize;
}

void D3DWrapperConvertStruct( D3D_RENDER_TARGET_VIEW_DESC *d3d_render_target_view_desc, D3D11_RENDER_TARGET_VIEW_DESC const *d3d_render_target_view_desc_11 )
{
    d3d_render_target_view_desc->Format = d3d_render_target_view_desc_11->Format;
    D3DWrapperConvertEnum( &d3d_render_target_view_desc->ViewDimension, d3d_render_target_view_desc_11->ViewDimension );
    switch( d3d_render_target_view_desc_11->ViewDimension )
    {
        case D3D11_RTV_DIMENSION_BUFFER:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Buffer, &d3d_render_target_view_desc_11->Buffer );
            break;
        case D3D11_RTV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture1D, &d3d_render_target_view_desc_11->Texture1D );
            break;
        case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture1DArray, &d3d_render_target_view_desc_11->Texture1DArray );
            break;
        case D3D11_RTV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture2D, &d3d_render_target_view_desc_11->Texture2D );
            break;
        case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture2DArray, &d3d_render_target_view_desc_11->Texture2DArray );
            break;
        case D3D11_RTV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture2DMS, &d3d_render_target_view_desc_11->Texture2DMS );
            break;
        case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture2DMSArray, &d3d_render_target_view_desc_11->Texture2DMSArray );
            break;
        case D3D11_RTV_DIMENSION_TEXTURE3D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc->Texture3D, &d3d_render_target_view_desc_11->Texture3D );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D11_RENDER_TARGET_VIEW_DESC *d3d_render_target_view_desc_11, D3D_RENDER_TARGET_VIEW_DESC const *d3d_render_target_view_desc )
{
    d3d_render_target_view_desc_11->Format = d3d_render_target_view_desc->Format;
    D3DWrapperConvertEnum( &d3d_render_target_view_desc_11->ViewDimension, d3d_render_target_view_desc->ViewDimension );
    switch( d3d_render_target_view_desc->ViewDimension )
    {
        case D3D_RTV_DIMENSION_BUFFER:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_11->Buffer, &d3d_render_target_view_desc->Buffer );
            break;
        case D3D_RTV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_11->Texture1D, &d3d_render_target_view_desc->Texture1D );
            break;
        case D3D_RTV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_11->Texture1DArray, &d3d_render_target_view_desc->Texture1DArray );
            break;
        case D3D_RTV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_11->Texture2D, &d3d_render_target_view_desc->Texture2D );
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_11->Texture2DArray, &d3d_render_target_view_desc->Texture2DArray );
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_11->Texture2DMS, &d3d_render_target_view_desc->Texture2DMS );
            break;
        case D3D_RTV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_11->Texture2DMSArray, &d3d_render_target_view_desc->Texture2DMSArray );
            break;
        case D3D_RTV_DIMENSION_TEXTURE3D:
            D3DWrapperConvertStruct( &d3d_render_target_view_desc_11->Texture3D, &d3d_render_target_view_desc->Texture3D );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D_TEX1D_DSV *d3d_tex1d_dsv, D3D11_TEX1D_DSV const *d3d_tex1d_dsv_11 )
{
    d3d_tex1d_dsv->MipSlice = d3d_tex1d_dsv_11->MipSlice;
}

void D3DWrapperConvertStruct( D3D11_TEX1D_DSV *d3d_tex1d_dsv_11, D3D_TEX1D_DSV const *d3d_tex1d_dsv )
{
    d3d_tex1d_dsv_11->MipSlice = d3d_tex1d_dsv->MipSlice;
}

void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_DSV *d3d_tex1d_array_dsv, D3D11_TEX1D_ARRAY_DSV const *d3d_tex1d_array_dsv_11 )
{
    d3d_tex1d_array_dsv->MipSlice = d3d_tex1d_array_dsv_11->MipSlice;
    d3d_tex1d_array_dsv->FirstArraySlice = d3d_tex1d_array_dsv_11->FirstArraySlice;
    d3d_tex1d_array_dsv->ArraySize = d3d_tex1d_array_dsv_11->ArraySize;
}

void D3DWrapperConvertStruct( D3D11_TEX1D_ARRAY_DSV *d3d_tex1d_array_dsv_11, D3D_TEX1D_ARRAY_DSV const *d3d_tex1d_array_dsv )
{
    d3d_tex1d_array_dsv_11->MipSlice = d3d_tex1d_array_dsv->MipSlice;
    d3d_tex1d_array_dsv_11->FirstArraySlice = d3d_tex1d_array_dsv->FirstArraySlice;
    d3d_tex1d_array_dsv_11->ArraySize = d3d_tex1d_array_dsv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX2D_DSV *d3d_tex2d_dsv, D3D11_TEX2D_DSV const *d3d_tex2d_dsv_11 )
{
    d3d_tex2d_dsv->MipSlice = d3d_tex2d_dsv_11->MipSlice;
}

void D3DWrapperConvertStruct( D3D11_TEX2D_DSV *d3d_tex2d_dsv_11, D3D_TEX2D_DSV const *d3d_tex2d_dsv )
{
    d3d_tex2d_dsv_11->MipSlice = d3d_tex2d_dsv->MipSlice;
}

void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_DSV *d3d_tex2d_array_dsv, D3D11_TEX2D_ARRAY_DSV const *d3d_tex2d_array_dsv_11 )
{
    d3d_tex2d_array_dsv->MipSlice = d3d_tex2d_array_dsv_11->MipSlice;
    d3d_tex2d_array_dsv->FirstArraySlice = d3d_tex2d_array_dsv_11->FirstArraySlice;
    d3d_tex2d_array_dsv->ArraySize = d3d_tex2d_array_dsv_11->ArraySize;
}

void D3DWrapperConvertStruct( D3D11_TEX2D_ARRAY_DSV *d3d_tex2d_array_dsv_11, D3D_TEX2D_ARRAY_DSV const *d3d_tex2d_array_dsv )
{
    d3d_tex2d_array_dsv_11->MipSlice = d3d_tex2d_array_dsv->MipSlice;
    d3d_tex2d_array_dsv_11->FirstArraySlice = d3d_tex2d_array_dsv->FirstArraySlice;
    d3d_tex2d_array_dsv_11->ArraySize = d3d_tex2d_array_dsv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_DSV *d3d_tex2dms_dsv, D3D11_TEX2DMS_DSV const *d3d_tex2dms_dsv_11 )
{
    d3d_tex2dms_dsv->UnusedField_NothingToDefine = d3d_tex2dms_dsv_11->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D11_TEX2DMS_DSV *d3d_tex2dms_dsv_11, D3D_TEX2DMS_DSV const *d3d_tex2dms_dsv )
{
    d3d_tex2dms_dsv_11->UnusedField_NothingToDefine = d3d_tex2dms_dsv->UnusedField_NothingToDefine;
}

void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_DSV *d3d_tex2dms_array_dsv, D3D11_TEX2DMS_ARRAY_DSV const *d3d_tex2dms_array_dsv_11 )
{
    d3d_tex2dms_array_dsv->FirstArraySlice = d3d_tex2dms_array_dsv_11->FirstArraySlice;
    d3d_tex2dms_array_dsv->ArraySize = d3d_tex2dms_array_dsv_11->ArraySize;
}

void D3DWrapperConvertStruct( D3D11_TEX2DMS_ARRAY_DSV *d3d_tex2dms_array_dsv_11, D3D_TEX2DMS_ARRAY_DSV const *d3d_tex2dms_array_dsv )
{
    d3d_tex2dms_array_dsv_11->FirstArraySlice = d3d_tex2dms_array_dsv->FirstArraySlice;
    d3d_tex2dms_array_dsv_11->ArraySize = d3d_tex2dms_array_dsv->ArraySize;
}

void D3DWrapperConvertStruct( D3D_DEPTH_STENCIL_VIEW_DESC *d3d_depth_stencil_view_desc, D3D11_DEPTH_STENCIL_VIEW_DESC const *d3d_depth_stencil_view_desc_11 )
{
    d3d_depth_stencil_view_desc->Format = d3d_depth_stencil_view_desc_11->Format;
    D3DWrapperConvertEnum( &d3d_depth_stencil_view_desc->ViewDimension, d3d_depth_stencil_view_desc_11->ViewDimension );
    switch( d3d_depth_stencil_view_desc_11->ViewDimension )
    {
        case D3D11_DSV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture1D, &d3d_depth_stencil_view_desc_11->Texture1D );
            break;
        case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture1DArray, &d3d_depth_stencil_view_desc_11->Texture1DArray );
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture2D, &d3d_depth_stencil_view_desc_11->Texture2D );
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture2DArray, &d3d_depth_stencil_view_desc_11->Texture2DArray );
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture2DMS, &d3d_depth_stencil_view_desc_11->Texture2DMS );
            break;
        case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc->Texture2DMSArray, &d3d_depth_stencil_view_desc_11->Texture2DMSArray );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D11_DEPTH_STENCIL_VIEW_DESC *d3d_depth_stencil_view_desc_11, D3D_DEPTH_STENCIL_VIEW_DESC const *d3d_depth_stencil_view_desc )
{
    d3d_depth_stencil_view_desc_11->Format = d3d_depth_stencil_view_desc->Format;
    D3DWrapperConvertEnum( &d3d_depth_stencil_view_desc_11->ViewDimension, d3d_depth_stencil_view_desc->ViewDimension );
    d3d_depth_stencil_view_desc_11->Flags = 0;
    switch( d3d_depth_stencil_view_desc->ViewDimension )
    {
        case D3D_DSV_DIMENSION_TEXTURE1D:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_11->Texture1D, &d3d_depth_stencil_view_desc->Texture1D );
            break;
        case D3D_DSV_DIMENSION_TEXTURE1DARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_11->Texture1DArray, &d3d_depth_stencil_view_desc->Texture1DArray );
            break;
        case D3D_DSV_DIMENSION_TEXTURE2D:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_11->Texture2D, &d3d_depth_stencil_view_desc->Texture2D );
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_11->Texture2DArray, &d3d_depth_stencil_view_desc->Texture2DArray );
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DMS:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_11->Texture2DMS, &d3d_depth_stencil_view_desc->Texture2DMS );
            break;
        case D3D_DSV_DIMENSION_TEXTURE2DMSARRAY:
            D3DWrapperConvertStruct( &d3d_depth_stencil_view_desc_11->Texture2DMSArray, &d3d_depth_stencil_view_desc->Texture2DMSArray );
            break;
        default:
            assert( !"Invalid resource dimension value" );
            break;
    }
}

void D3DWrapperConvertStruct( D3D_SAMPLER_DESC *d3d_sampler_desc, D3D11_SAMPLER_DESC const *d3d_sampler_desc_11 )
{
    D3DWrapperConvertEnum( &d3d_sampler_desc->Filter, d3d_sampler_desc_11->Filter );
    D3DWrapperConvertEnum( &d3d_sampler_desc->AddressU, d3d_sampler_desc_11->AddressU );
    D3DWrapperConvertEnum( &d3d_sampler_desc->AddressV, d3d_sampler_desc_11->AddressV );
    D3DWrapperConvertEnum( &d3d_sampler_desc->AddressW, d3d_sampler_desc_11->AddressW );
    d3d_sampler_desc->MipLODBias = d3d_sampler_desc_11->MipLODBias;
    d3d_sampler_desc->MaxAnisotropy = d3d_sampler_desc_11->MaxAnisotropy;
    D3DWrapperConvertEnum( &d3d_sampler_desc->ComparisonFunc, d3d_sampler_desc_11->ComparisonFunc );
    d3d_sampler_desc->BorderColor[3] = d3d_sampler_desc_11->BorderColor[3];
    d3d_sampler_desc->BorderColor[2] = d3d_sampler_desc_11->BorderColor[2];
    d3d_sampler_desc->BorderColor[1] = d3d_sampler_desc_11->BorderColor[1];
    d3d_sampler_desc->BorderColor[0] = d3d_sampler_desc_11->BorderColor[0];
    d3d_sampler_desc->MinLOD = d3d_sampler_desc_11->MinLOD;
    d3d_sampler_desc->MaxLOD = d3d_sampler_desc_11->MaxLOD;
}

void D3DWrapperConvertStruct( D3D11_SAMPLER_DESC *d3d_sampler_desc_11, D3D_SAMPLER_DESC const *d3d_sampler_desc )
{
    D3DWrapperConvertEnum( &d3d_sampler_desc_11->Filter, d3d_sampler_desc->Filter );
    D3DWrapperConvertEnum( &d3d_sampler_desc_11->AddressU, d3d_sampler_desc->AddressU );
    D3DWrapperConvertEnum( &d3d_sampler_desc_11->AddressV, d3d_sampler_desc->AddressV );
    D3DWrapperConvertEnum( &d3d_sampler_desc_11->AddressW, d3d_sampler_desc->AddressW );
    d3d_sampler_desc_11->MipLODBias = d3d_sampler_desc->MipLODBias;
    d3d_sampler_desc_11->MaxAnisotropy = d3d_sampler_desc->MaxAnisotropy;
    D3DWrapperConvertEnum( &d3d_sampler_desc_11->ComparisonFunc, d3d_sampler_desc->ComparisonFunc );
    d3d_sampler_desc_11->BorderColor[3] = d3d_sampler_desc->BorderColor[3];
    d3d_sampler_desc_11->BorderColor[2] = d3d_sampler_desc->BorderColor[2];
    d3d_sampler_desc_11->BorderColor[1] = d3d_sampler_desc->BorderColor[1];
    d3d_sampler_desc_11->BorderColor[0] = d3d_sampler_desc->BorderColor[0];
    d3d_sampler_desc_11->MinLOD = d3d_sampler_desc->MinLOD;
    d3d_sampler_desc_11->MaxLOD = d3d_sampler_desc->MaxLOD;
}

void D3DWrapperConvertStruct( D3D_QUERY_DESC *d3d_query_desc, D3D11_QUERY_DESC const *d3d_query_desc_11 )
{
    D3DWrapperConvertEnum( &d3d_query_desc->Query, d3d_query_desc_11->Query );
    D3DFlagsConvertFrom11_QUERY_MISC_FLAG( &d3d_query_desc->MiscFlags, d3d_query_desc_11->MiscFlags );
}

void D3DWrapperConvertStruct( D3D11_QUERY_DESC *d3d_query_desc_11, D3D_QUERY_DESC const *d3d_query_desc )
{
    D3DWrapperConvertEnum( &d3d_query_desc_11->Query, d3d_query_desc->Query );
    D3DFlagsConvertTo11_QUERY_MISC_FLAG( &d3d_query_desc_11->MiscFlags, d3d_query_desc->MiscFlags );
}

void D3DWrapperConvertStruct( D3D_QUERY_DATA_TIMESTAMP_DISJOINT *d3d_query_data_timestamp_disjoint, D3D11_QUERY_DATA_TIMESTAMP_DISJOINT const *d3d_query_data_timestamp_disjoint_11 )
{
    d3d_query_data_timestamp_disjoint->Frequency = d3d_query_data_timestamp_disjoint_11->Frequency;
    d3d_query_data_timestamp_disjoint->Disjoint = d3d_query_data_timestamp_disjoint_11->Disjoint;
}

void D3DWrapperConvertStruct( D3D11_QUERY_DATA_TIMESTAMP_DISJOINT *d3d_query_data_timestamp_disjoint_11, D3D_QUERY_DATA_TIMESTAMP_DISJOINT const *d3d_query_data_timestamp_disjoint )
{
    d3d_query_data_timestamp_disjoint_11->Frequency = d3d_query_data_timestamp_disjoint->Frequency;
    d3d_query_data_timestamp_disjoint_11->Disjoint = d3d_query_data_timestamp_disjoint->Disjoint;
}

void D3DWrapperConvertStruct( D3D_QUERY_DATA_PIPELINE_STATISTICS *d3d_query_data_pipeline_statistics, D3D11_QUERY_DATA_PIPELINE_STATISTICS const *d3d_query_data_pipeline_statistics_11 )
{
    d3d_query_data_pipeline_statistics->IAVertices = d3d_query_data_pipeline_statistics_11->IAVertices;
    d3d_query_data_pipeline_statistics->IAPrimitives = d3d_query_data_pipeline_statistics_11->IAPrimitives;
    d3d_query_data_pipeline_statistics->VSInvocations = d3d_query_data_pipeline_statistics_11->VSInvocations;
    d3d_query_data_pipeline_statistics->GSInvocations = d3d_query_data_pipeline_statistics_11->GSInvocations;
    d3d_query_data_pipeline_statistics->GSPrimitives = d3d_query_data_pipeline_statistics_11->GSPrimitives;
    d3d_query_data_pipeline_statistics->CInvocations = d3d_query_data_pipeline_statistics_11->CInvocations;
    d3d_query_data_pipeline_statistics->CPrimitives = d3d_query_data_pipeline_statistics_11->CPrimitives;
    d3d_query_data_pipeline_statistics->PSInvocations = d3d_query_data_pipeline_statistics_11->PSInvocations;
}

void D3DWrapperConvertStruct( D3D11_QUERY_DATA_PIPELINE_STATISTICS *d3d_query_data_pipeline_statistics_11, D3D_QUERY_DATA_PIPELINE_STATISTICS const *d3d_query_data_pipeline_statistics )
{
    d3d_query_data_pipeline_statistics_11->IAVertices = d3d_query_data_pipeline_statistics->IAVertices;
    d3d_query_data_pipeline_statistics_11->IAPrimitives = d3d_query_data_pipeline_statistics->IAPrimitives;
    d3d_query_data_pipeline_statistics_11->VSInvocations = d3d_query_data_pipeline_statistics->VSInvocations;
    d3d_query_data_pipeline_statistics_11->GSInvocations = d3d_query_data_pipeline_statistics->GSInvocations;
    d3d_query_data_pipeline_statistics_11->GSPrimitives = d3d_query_data_pipeline_statistics->GSPrimitives;
    d3d_query_data_pipeline_statistics_11->CInvocations = d3d_query_data_pipeline_statistics->CInvocations;
    d3d_query_data_pipeline_statistics_11->CPrimitives = d3d_query_data_pipeline_statistics->CPrimitives;
    d3d_query_data_pipeline_statistics_11->PSInvocations = d3d_query_data_pipeline_statistics->PSInvocations;
}

void D3DWrapperConvertStruct( D3D_QUERY_DATA_SO_STATISTICS *d3d_query_data_so_statistics, D3D11_QUERY_DATA_SO_STATISTICS const *d3d_query_data_so_statistics_11 )
{
    d3d_query_data_so_statistics->NumPrimitivesWritten = d3d_query_data_so_statistics_11->NumPrimitivesWritten;
    d3d_query_data_so_statistics->PrimitivesStorageNeeded = d3d_query_data_so_statistics_11->PrimitivesStorageNeeded;
}

void D3DWrapperConvertStruct( D3D11_QUERY_DATA_SO_STATISTICS *d3d_query_data_so_statistics_11, D3D_QUERY_DATA_SO_STATISTICS const *d3d_query_data_so_statistics )
{
    d3d_query_data_so_statistics_11->NumPrimitivesWritten = d3d_query_data_so_statistics->NumPrimitivesWritten;
    d3d_query_data_so_statistics_11->PrimitivesStorageNeeded = d3d_query_data_so_statistics->PrimitivesStorageNeeded;
}

void D3DWrapperConvertStruct( D3D_COUNTER_DESC *d3d_counter_desc, D3D11_COUNTER_DESC const *d3d_counter_desc_11 )
{
    D3DWrapperConvertEnum( &d3d_counter_desc->Counter, d3d_counter_desc_11->Counter );
    d3d_counter_desc->MiscFlags = d3d_counter_desc_11->MiscFlags;
}

void D3DWrapperConvertStruct( D3D11_COUNTER_DESC *d3d_counter_desc_11, D3D_COUNTER_DESC const *d3d_counter_desc )
{
    D3DWrapperConvertEnum( &d3d_counter_desc_11->Counter, d3d_counter_desc->Counter );
    d3d_counter_desc_11->MiscFlags = d3d_counter_desc->MiscFlags;
}

void D3DWrapperConvertStruct( D3D_COUNTER_INFO *d3d_counter_info, D3D11_COUNTER_INFO const *d3d_counter_info_11 )
{
    D3DWrapperConvertEnum( &d3d_counter_info->LastDeviceDependentCounter, d3d_counter_info_11->LastDeviceDependentCounter );
    d3d_counter_info->NumSimultaneousCounters = d3d_counter_info_11->NumSimultaneousCounters;
    d3d_counter_info->NumDetectableParallelUnits = d3d_counter_info_11->NumDetectableParallelUnits;
}

void D3DWrapperConvertStruct( D3D11_COUNTER_INFO *d3d_counter_info_11, D3D_COUNTER_INFO const *d3d_counter_info )
{
    D3DWrapperConvertEnum( &d3d_counter_info_11->LastDeviceDependentCounter, d3d_counter_info->LastDeviceDependentCounter );
    d3d_counter_info_11->NumSimultaneousCounters = d3d_counter_info->NumSimultaneousCounters;
    d3d_counter_info_11->NumDetectableParallelUnits = d3d_counter_info->NumDetectableParallelUnits;
}

void D3DWrapperConvertStruct( D3D_MESSAGE *d3d_message, D3D11_MESSAGE const *d3d_message_11 )
{
    D3DWrapperConvertEnum( &d3d_message->Category, d3d_message_11->Category );
    D3DWrapperConvertEnum( &d3d_message->Severity, d3d_message_11->Severity );
    D3DWrapperConvertEnum( &d3d_message->ID, d3d_message_11->ID );
    d3d_message->pDescription = (const char *)((BYTE*)(d3d_message) + sizeof(D3D_MESSAGE));
    memcpy((void*)d3d_message->pDescription, d3d_message_11->pDescription, d3d_message_11->DescriptionByteLength);
    d3d_message->DescriptionByteLength = d3d_message_11->DescriptionByteLength;
}

void D3DWrapperConvertStruct( D3D11_MESSAGE *d3d_message_11, D3D_MESSAGE const *d3d_message )
{
    D3DWrapperConvertEnum( &d3d_message_11->Category, d3d_message->Category );
    D3DWrapperConvertEnum( &d3d_message_11->Severity, d3d_message->Severity );
    D3DWrapperConvertEnum( &d3d_message_11->ID, d3d_message->ID );
    d3d_message_11->pDescription = (const char *)((BYTE*)(d3d_message_11) + sizeof(D3D11_MESSAGE));
    memcpy((void*)d3d_message_11->pDescription, d3d_message->pDescription, d3d_message->DescriptionByteLength);
    d3d_message_11->DescriptionByteLength = d3d_message->DescriptionByteLength;
}

void D3DWrapperConvertStruct( D3D_INFO_QUEUE_FILTER_DESC *d3d_info_queue_filter_desc, D3D11_INFO_QUEUE_FILTER_DESC const *d3d_info_queue_filter_desc_11 )
{
    d3d_info_queue_filter_desc->NumCategories = d3d_info_queue_filter_desc_11->NumCategories;
    // BUGBUG - NEED CONVERSION FOR THIS POINTER PARAMETER: pCategoryList
    d3d_info_queue_filter_desc->NumSeverities = d3d_info_queue_filter_desc_11->NumSeverities;
    // BUGBUG - NEED CONVERSION FOR THIS POINTER PARAMETER: pSeverityList
    d3d_info_queue_filter_desc->NumIDs = d3d_info_queue_filter_desc_11->NumIDs;
    // BUGBUG - NEED CONVERSION FOR THIS POINTER PARAMETER: pIDList
}

void D3DWrapperConvertStruct( D3D11_INFO_QUEUE_FILTER_DESC *d3d_info_queue_filter_desc_11, D3D_INFO_QUEUE_FILTER_DESC const *d3d_info_queue_filter_desc )
{
    d3d_info_queue_filter_desc_11->NumCategories = d3d_info_queue_filter_desc->NumCategories;
    // BUGBUG - NEED CONVERSION FOR THIS PARAMETER: d3d_info_queue_filter_desc
    d3d_info_queue_filter_desc_11->NumSeverities = d3d_info_queue_filter_desc->NumSeverities;
    // BUGBUG - NEED CONVERSION FOR THIS PARAMETER: d3d_info_queue_filter_desc
    d3d_info_queue_filter_desc_11->NumIDs = d3d_info_queue_filter_desc->NumIDs;
    // BUGBUG - NEED CONVERSION FOR THIS PARAMETER: d3d_info_queue_filter_desc
}

void D3DWrapperConvertStruct( D3D_INFO_QUEUE_FILTER *d3d_info_queue_filter, D3D11_INFO_QUEUE_FILTER const *d3d_info_queue_filter_11 )
{
    D3DWrapperConvertStruct( &d3d_info_queue_filter->AllowList, &d3d_info_queue_filter_11->AllowList );
    D3DWrapperConvertStruct( &d3d_info_queue_filter->DenyList, &d3d_info_queue_filter_11->DenyList );
}

void D3DWrapperConvertStruct( D3D11_INFO_QUEUE_FILTER *d3d_info_queue_filter_11, D3D_INFO_QUEUE_FILTER const *d3d_info_queue_filter )
{
    D3DWrapperConvertStruct( &d3d_info_queue_filter_11->AllowList, &d3d_info_queue_filter->AllowList );
    D3DWrapperConvertStruct( &d3d_info_queue_filter_11->DenyList, &d3d_info_queue_filter->DenyList );
}

void D3DWrapperConvertStruct( D3D_MAPPED_TEXTURE2D *d3d_mapped_texture2d, D3D11_MAPPED_TEXTURE2D const *d3d_mapped_texture2d_11 )
{
    d3d_mapped_texture2d->pData = d3d_mapped_texture2d_11->pData;
    d3d_mapped_texture2d->RowPitch = d3d_mapped_texture2d_11->RowPitch;
}

void D3DWrapperConvertStruct( D3D11_MAPPED_TEXTURE2D *d3d_mapped_texture2d_11, D3D_MAPPED_TEXTURE2D const *d3d_mapped_texture2d )
{
    d3d_mapped_texture2d_11->pData = d3d_mapped_texture2d->pData;
    d3d_mapped_texture2d_11->RowPitch = d3d_mapped_texture2d->RowPitch;
}

void D3DWrapperConvertStruct( D3D_MAPPED_TEXTURE3D *d3d_mapped_texture3d, D3D11_MAPPED_TEXTURE3D const *d3d_mapped_texture3d_11 )
{
    d3d_mapped_texture3d->pData = d3d_mapped_texture3d_11->pData;
    d3d_mapped_texture3d->RowPitch = d3d_mapped_texture3d_11->RowPitch;
    d3d_mapped_texture3d->DepthPitch = d3d_mapped_texture3d_11->DepthPitch;
}

void D3DWrapperConvertStruct( D3D11_MAPPED_TEXTURE3D *d3d_mapped_texture3d_11, D3D_MAPPED_TEXTURE3D const *d3d_mapped_texture3d )
{
    d3d_mapped_texture3d_11->pData = d3d_mapped_texture3d->pData;
    d3d_mapped_texture3d_11->RowPitch = d3d_mapped_texture3d->RowPitch;
    d3d_mapped_texture3d_11->DepthPitch = d3d_mapped_texture3d->DepthPitch;
}


//
// Interface Conversions
//


////////////////////////////////////////////////////////////
//
// C11DeviceChildGeneric
//
////////////////////////////////////////////////////////////

class C11DeviceChildGeneric : public C11DeviceChild
{
    ID3D11DeviceChild *m_pDeviceChild;

public:
    C11DeviceChildGeneric( ID3D11DeviceChild *pDeviceChild );
    virtual ~C11DeviceChildGeneric();

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
    virtual void GetInterfacePtr( ID3D11DeviceChild **pInterface ) { *pInterface = (ID3D11DeviceChild *) m_pDeviceChild; }
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
// C11DeviceChildGeneric
//
////////////////////////////////////////////////////////////

C11DeviceChildGeneric::C11DeviceChildGeneric( ID3D11DeviceChild *pDeviceChild ) : 
    m_pDeviceChild( pDeviceChild )
{
}


C11DeviceChildGeneric::~C11DeviceChildGeneric()
{
    if( m_pDeviceChild ) m_pDeviceChild->Release();
}


////////////////////////////////////////////////////////////
// C11DeviceChildGeneric::GetDevice()
////////////////////////////////////////////////////////////
void C11DeviceChildGeneric::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pDeviceChild );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pDeviceChild->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11DeviceChildGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11DeviceChildGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pDeviceChild );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pDeviceChild->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11DeviceChildGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11DeviceChildGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pDeviceChild );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pDeviceChild->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11DeviceChildGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11DeviceChildGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pDeviceChild );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pDeviceChild->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}

void D3DWrapperConvertInterface( CDeviceChild **ppDeviceChild, ID3D11DeviceChild *pDeviceChild_11 )
{
    if( pDeviceChild_11 )
    {
        *ppDeviceChild = new C11DeviceChildGeneric( pDeviceChild_11 );
        if( !(*ppDeviceChild) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppDeviceChild)->AddRef();
    }
    else
    {
        *ppDeviceChild = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11DeviceChild **ppDeviceChild_11, CDeviceChild *pDeviceChild )
{
    if( pDeviceChild )
    {
        pDeviceChild->GetInterfacePtr( ppDeviceChild_11 );
    }
    else
    {
        *ppDeviceChild_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CDepthStencilState **ppDepthStencilState, ID3D11DepthStencilState *pDepthStencilState_11 )
{
    if( pDepthStencilState_11 )
    {
        *ppDepthStencilState = new C11DepthStencilState( pDepthStencilState_11 );
        if( !(*ppDepthStencilState) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppDepthStencilState)->AddRef();
    }
    else
    {
        *ppDepthStencilState = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11DepthStencilState **ppDepthStencilState_11, CDepthStencilState *pDepthStencilState )
{
    if( pDepthStencilState )
    {
        pDepthStencilState->GetInterfacePtr( ppDepthStencilState_11 );
    }
    else
    {
        *ppDepthStencilState_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CBlendState **ppBlendState, ID3D11BlendState *pBlendState_11 )
{
    if( pBlendState_11 )
    {
        *ppBlendState = new C11BlendState( pBlendState_11 );
        if( !(*ppBlendState) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppBlendState)->AddRef();
    }
    else
    {
        *ppBlendState = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11BlendState **ppBlendState_11, CBlendState *pBlendState )
{
    if( pBlendState )
    {
        pBlendState->GetInterfacePtr( ppBlendState_11 );
    }
    else
    {
        *ppBlendState_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CRasterizerState **ppRasterizerState, ID3D11RasterizerState *pRasterizerState_11 )
{
    if( pRasterizerState_11 )
    {
        *ppRasterizerState = new C11RasterizerState( pRasterizerState_11 );
        if( !(*ppRasterizerState) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppRasterizerState)->AddRef();
    }
    else
    {
        *ppRasterizerState = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11RasterizerState **ppRasterizerState_11, CRasterizerState *pRasterizerState )
{
    if( pRasterizerState )
    {
        pRasterizerState->GetInterfacePtr( ppRasterizerState_11 );
    }
    else
    {
        *ppRasterizerState_11 = NULL;
    }
}


////////////////////////////////////////////////////////////
//
// C11ResourceGeneric
//
////////////////////////////////////////////////////////////

class C11ResourceGeneric : public C11Resource
{
    ID3D11Resource *m_pResource;

public:
    C11ResourceGeneric( ID3D11Resource *pResource );
    virtual ~C11ResourceGeneric();

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
    virtual void GetInterfacePtr( ID3D11Resource **pInterface ) { *pInterface = (ID3D11Resource *) m_pResource; }
    virtual CResource *GetSubResource( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // GetType
    ////////////////////////////////////////////////////////////
    void GetType(  D3D_RESOURCE_DIMENSION * pResourceDimension );

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
    virtual void GetInterfacePtr( ID3D11DeviceChild **pInterface ) { *pInterface = (ID3D11DeviceChild *) m_pResource; }
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
// C11ResourceGeneric
//
////////////////////////////////////////////////////////////

C11ResourceGeneric::C11ResourceGeneric( ID3D11Resource *pResource ) : 
    m_pResource( pResource )
{
}


C11ResourceGeneric::~C11ResourceGeneric()
{
    if( m_pResource ) m_pResource->Release();
}


////////////////////////////////////////////////////////////
// C11ResourceGeneric::GetType()
////////////////////////////////////////////////////////////
void C11ResourceGeneric::GetType(  D3D_RESOURCE_DIMENSION * pResourceDimension )
{
    assert( m_pResource );
    D3D11_RESOURCE_DIMENSION temp_d3d11_resource_dimension;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pResource->GetType(  &temp_d3d11_resource_dimension );
    D3DWrapperConvertEnum( pResourceDimension, temp_d3d11_resource_dimension );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11ResourceGeneric::SetEvictionPriority()
////////////////////////////////////////////////////////////
void C11ResourceGeneric::SetEvictionPriority(  UINT EvictionPriority )
{
    assert( m_pResource );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pResource->SetEvictionPriority(  EvictionPriority );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11ResourceGeneric::GetEvictionPriority()
////////////////////////////////////////////////////////////
UINT C11ResourceGeneric::GetEvictionPriority(  )
{
    assert( m_pResource );
    UINT rval = 0;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pResource->GetEvictionPriority(  );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11ResourceGeneric::GetDevice()
////////////////////////////////////////////////////////////
void C11ResourceGeneric::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pResource );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pResource->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11ResourceGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11ResourceGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pResource );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pResource->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11ResourceGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11ResourceGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pResource );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pResource->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11ResourceGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11ResourceGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pResource );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pResource->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}

void D3DWrapperConvertInterface( CResource **ppResource, ID3D11Resource *pResource_11 )
{
    if( pResource_11 )
    {
        *ppResource = new C11ResourceGeneric( pResource_11 );
        if( !(*ppResource) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppResource)->AddRef();
    }
    else
    {
        *ppResource = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11Resource **ppResource_11, CResource *pResource )
{
    if( pResource )
    {
        pResource->GetInterfacePtr( ppResource_11 );
    }
    else
    {
        *ppResource_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CBuffer **ppBuffer, ID3D11Buffer *pBuffer_11 )
{
    if( pBuffer_11 )
    {
        *ppBuffer = new C11Buffer( pBuffer_11 );
        if( !(*ppBuffer) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppBuffer)->AddRef();
    }
    else
    {
        *ppBuffer = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11Buffer **ppBuffer_11, CBuffer *pBuffer )
{
    if( pBuffer )
    {
        pBuffer->GetInterfacePtr( ppBuffer_11 );
    }
    else
    {
        *ppBuffer_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CTexture1D **ppTexture1D, ID3D11Texture1D *pTexture1D_11 )
{
    if( pTexture1D_11 )
    {
        *ppTexture1D = new C11Texture1D( pTexture1D_11 );
        if( !(*ppTexture1D) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppTexture1D)->AddRef();
    }
    else
    {
        *ppTexture1D = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11Texture1D **ppTexture1D_11, CTexture1D *pTexture1D )
{
    if( pTexture1D )
    {
        pTexture1D->GetInterfacePtr( ppTexture1D_11 );
    }
    else
    {
        *ppTexture1D_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CTexture2D **ppTexture2D, ID3D11Texture2D *pTexture2D_11 )
{
    if( pTexture2D_11 )
    {
        *ppTexture2D = new C11Texture2D( pTexture2D_11 );
        if( !(*ppTexture2D) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppTexture2D)->AddRef();
    }
    else
    {
        *ppTexture2D = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11Texture2D **ppTexture2D_11, CTexture2D *pTexture2D )
{
    if( pTexture2D )
    {
        pTexture2D->GetInterfacePtr( ppTexture2D_11 );
    }
    else
    {
        *ppTexture2D_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CTexture3D **ppTexture3D, ID3D11Texture3D *pTexture3D_11 )
{
    if( pTexture3D_11 )
    {
        *ppTexture3D = new C11Texture3D( pTexture3D_11 );
        if( !(*ppTexture3D) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppTexture3D)->AddRef();
    }
    else
    {
        *ppTexture3D = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11Texture3D **ppTexture3D_11, CTexture3D *pTexture3D )
{
    if( pTexture3D )
    {
        pTexture3D->GetInterfacePtr( ppTexture3D_11 );
    }
    else
    {
        *ppTexture3D_11 = NULL;
    }
}


////////////////////////////////////////////////////////////
//
// C11ViewGeneric
//
////////////////////////////////////////////////////////////

class C11ViewGeneric : public C11View
{
    ID3D11View *m_pView;

public:
    C11ViewGeneric( ID3D11View *pView );
    virtual ~C11ViewGeneric();

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
    virtual void GetInterfacePtr( ID3D11View **pInterface ) { *pInterface = (ID3D11View *) m_pView; }
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
    virtual void GetInterfacePtr( ID3D11DeviceChild **pInterface ) { *pInterface = (ID3D11DeviceChild *) m_pView; }
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
// C11ViewGeneric
//
////////////////////////////////////////////////////////////

C11ViewGeneric::C11ViewGeneric( ID3D11View *pView ) : 
    m_pView( pView )
{
}


C11ViewGeneric::~C11ViewGeneric()
{
    if( m_pView ) m_pView->Release();
}


////////////////////////////////////////////////////////////
// C11ViewGeneric::GetResource()
////////////////////////////////////////////////////////////
void C11ViewGeneric::GetResource(  CResource ** ppResource )
{
    assert( m_pView );
    ID3D11Resource *temp_pResource = NULL;
    ID3D11Resource **ppResourceTemp = ppResource == NULL ? NULL : &temp_pResource;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pView->GetResource(  ppResourceTemp );
    if( ppResource != NULL )
    {
        D3DWrapperConvertInterface( ppResource, *ppResourceTemp );
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11ViewGeneric::GetDevice()
////////////////////////////////////////////////////////////
void C11ViewGeneric::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pView );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pView->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11ViewGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11ViewGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pView->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11ViewGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11ViewGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pView->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11ViewGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11ViewGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pView );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pView->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}

void D3DWrapperConvertInterface( CView **ppView, ID3D11View *pView_11 )
{
    if( pView_11 )
    {
        *ppView = new C11ViewGeneric( pView_11 );
        if( !(*ppView) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppView)->AddRef();
    }
    else
    {
        *ppView = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11View **ppView_11, CView *pView )
{
    if( pView )
    {
        pView->GetInterfacePtr( ppView_11 );
    }
    else
    {
        *ppView_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CShaderResourceView **ppShaderResourceView, ID3D11ShaderResourceView *pShaderResourceView_11 )
{
    if( pShaderResourceView_11 )
    {
        *ppShaderResourceView = new C11ShaderResourceView( pShaderResourceView_11 );
        if( !(*ppShaderResourceView) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppShaderResourceView)->AddRef();
    }
    else
    {
        *ppShaderResourceView = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11ShaderResourceView **ppShaderResourceView_11, CShaderResourceView *pShaderResourceView )
{
    if( pShaderResourceView )
    {
        pShaderResourceView->GetInterfacePtr( ppShaderResourceView_11 );
    }
    else
    {
        *ppShaderResourceView_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CRenderTargetView **ppRenderTargetView, ID3D11RenderTargetView *pRenderTargetView_11 )
{
    if( pRenderTargetView_11 )
    {
        *ppRenderTargetView = new C11RenderTargetView( pRenderTargetView_11 );
        if( !(*ppRenderTargetView) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppRenderTargetView)->AddRef();
    }
    else
    {
        *ppRenderTargetView = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11RenderTargetView **ppRenderTargetView_11, CRenderTargetView *pRenderTargetView )
{
    if( pRenderTargetView )
    {
        pRenderTargetView->GetInterfacePtr( ppRenderTargetView_11 );
    }
    else
    {
        *ppRenderTargetView_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CDepthStencilView **ppDepthStencilView, ID3D11DepthStencilView *pDepthStencilView_11 )
{
    if( pDepthStencilView_11 )
    {
        *ppDepthStencilView = new C11DepthStencilView( pDepthStencilView_11 );
        if( !(*ppDepthStencilView) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppDepthStencilView)->AddRef();
    }
    else
    {
        *ppDepthStencilView = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11DepthStencilView **ppDepthStencilView_11, CDepthStencilView *pDepthStencilView )
{
    if( pDepthStencilView )
    {
        pDepthStencilView->GetInterfacePtr( ppDepthStencilView_11 );
    }
    else
    {
        *ppDepthStencilView_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CVertexShader **ppVertexShader, ID3D11VertexShader *pVertexShader_11 )
{
    if( pVertexShader_11 )
    {
        *ppVertexShader = new C11VertexShader( pVertexShader_11 );
        if( !(*ppVertexShader) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppVertexShader)->AddRef();
    }
    else
    {
        *ppVertexShader = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11VertexShader **ppVertexShader_11, CVertexShader *pVertexShader )
{
    if( pVertexShader )
    {
        pVertexShader->GetInterfacePtr( ppVertexShader_11 );
    }
    else
    {
        *ppVertexShader_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CGeometryShader **ppGeometryShader, ID3D11GeometryShader *pGeometryShader_11 )
{
    if( pGeometryShader_11 )
    {
        *ppGeometryShader = new C11GeometryShader( pGeometryShader_11 );
        if( !(*ppGeometryShader) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppGeometryShader)->AddRef();
    }
    else
    {
        *ppGeometryShader = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11GeometryShader **ppGeometryShader_11, CGeometryShader *pGeometryShader )
{
    if( pGeometryShader )
    {
        pGeometryShader->GetInterfacePtr( ppGeometryShader_11 );
    }
    else
    {
        *ppGeometryShader_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CPixelShader **ppPixelShader, ID3D11PixelShader *pPixelShader_11 )
{
    if( pPixelShader_11 )
    {
        *ppPixelShader = new C11PixelShader( pPixelShader_11 );
        if( !(*ppPixelShader) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppPixelShader)->AddRef();
    }
    else
    {
        *ppPixelShader = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11PixelShader **ppPixelShader_11, CPixelShader *pPixelShader )
{
    if( pPixelShader )
    {
        pPixelShader->GetInterfacePtr( ppPixelShader_11 );
    }
    else
    {
        *ppPixelShader_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CInputLayout **ppInputLayout, ID3D11InputLayout *pInputLayout_11 )
{
    if( pInputLayout_11 )
    {
        *ppInputLayout = new C11InputLayout( pInputLayout_11 );
        if( !(*ppInputLayout) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppInputLayout)->AddRef();
    }
    else
    {
        *ppInputLayout = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11InputLayout **ppInputLayout_11, CInputLayout *pInputLayout )
{
    if( pInputLayout )
    {
        pInputLayout->GetInterfacePtr( ppInputLayout_11 );
    }
    else
    {
        *ppInputLayout_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CSamplerState **ppSamplerState, ID3D11SamplerState *pSamplerState_11 )
{
    if( pSamplerState_11 )
    {
        *ppSamplerState = new C11SamplerState( pSamplerState_11 );
        if( !(*ppSamplerState) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppSamplerState)->AddRef();
    }
    else
    {
        *ppSamplerState = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11SamplerState **ppSamplerState_11, CSamplerState *pSamplerState )
{
    if( pSamplerState )
    {
        pSamplerState->GetInterfacePtr( ppSamplerState_11 );
    }
    else
    {
        *ppSamplerState_11 = NULL;
    }
}


////////////////////////////////////////////////////////////
//
// C11AsynchronousGeneric
//
////////////////////////////////////////////////////////////

class C11AsynchronousGeneric : public C11Asynchronous
{
    ID3D11Asynchronous *m_pAsynchronous;

public:
    C11AsynchronousGeneric( ID3D11Asynchronous *pAsynchronous );
    virtual ~C11AsynchronousGeneric();

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
    virtual void GetInterfacePtr( ID3D11Asynchronous **pInterface ) { *pInterface = (ID3D11Asynchronous *) m_pAsynchronous; }
    virtual CAsynchronous *GetSubAsynchronous( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // GetDataSize
    ////////////////////////////////////////////////////////////
    UINT GetDataSize(  );

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
    NRESULT GetData(  void* pData, UINT DataSize, UINT GetDataFlags );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D11DeviceChild **pInterface ) { *pInterface = (ID3D11DeviceChild *) m_pAsynchronous; }
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
// C11AsynchronousGeneric
//
////////////////////////////////////////////////////////////

C11AsynchronousGeneric::C11AsynchronousGeneric( ID3D11Asynchronous *pAsynchronous ) : 
    m_pAsynchronous( pAsynchronous )
{
}


C11AsynchronousGeneric::~C11AsynchronousGeneric()
{
    if( m_pAsynchronous ) m_pAsynchronous->Release();
}


////////////////////////////////////////////////////////////
// C11AsynchronousGeneric::GetDataSize()
////////////////////////////////////////////////////////////
UINT C11AsynchronousGeneric::GetDataSize(  )
{
    assert( m_pAsynchronous );
    UINT rval = 0;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pAsynchronous->GetDataSize(  );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11AsynchronousGeneric::Begin()
////////////////////////////////////////////////////////////
void C11AsynchronousGeneric::Begin(  )
{
    assert( m_pAsynchronous );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->Begin(  m_pAsynchronous );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11AsynchronousGeneric::End()
////////////////////////////////////////////////////////////
void C11AsynchronousGeneric::End(  )
{
    assert( m_pAsynchronous );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->End(  m_pAsynchronous );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11AsynchronousGeneric::GetData()
////////////////////////////////////////////////////////////
NRESULT C11AsynchronousGeneric::GetData(  void* pData, UINT DataSize, UINT GetDataFlags )
{
    assert( m_pAsynchronous );
    NRESULT rval = S_OK;
    UINT temp_GetDataFlags;
    D3DFlagsConvertTo11_ASYNC_GETDATA_FLAG( &temp_GetDataFlags, GetDataFlags );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = pTempDevice11->ExecDeferredContext( );
    if (FAILED(rval))
    {
        return rval;
    }
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = pImmContext->GetData(  m_pAsynchronous,  pData,  DataSize,  temp_GetDataFlags );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11AsynchronousGeneric::GetDevice()
////////////////////////////////////////////////////////////
void C11AsynchronousGeneric::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pAsynchronous );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pAsynchronous->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11AsynchronousGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11AsynchronousGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pAsynchronous );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pAsynchronous->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11AsynchronousGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11AsynchronousGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pAsynchronous );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pAsynchronous->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11AsynchronousGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11AsynchronousGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pAsynchronous );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pAsynchronous->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}

void D3DWrapperConvertInterface( CAsynchronous **ppAsynchronous, ID3D11Asynchronous *pAsynchronous_11 )
{
    if( pAsynchronous_11 )
    {
        *ppAsynchronous = new C11AsynchronousGeneric( pAsynchronous_11 );
        if( !(*ppAsynchronous) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppAsynchronous)->AddRef();
    }
    else
    {
        *ppAsynchronous = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11Asynchronous **ppAsynchronous_11, CAsynchronous *pAsynchronous )
{
    if( pAsynchronous )
    {
        pAsynchronous->GetInterfacePtr( ppAsynchronous_11 );
    }
    else
    {
        *ppAsynchronous_11 = NULL;
    }
}


////////////////////////////////////////////////////////////
//
// C11QueryGeneric
//
////////////////////////////////////////////////////////////

class C11QueryGeneric : public C11Query
{
    ID3D11Query *m_pQuery;

public:
    C11QueryGeneric( ID3D11Query *pQuery );
    virtual ~C11QueryGeneric();

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
    virtual void GetInterfacePtr( ID3D11Query **pInterface ) { *pInterface = (ID3D11Query *) m_pQuery; }
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
    virtual void GetInterfacePtr( ID3D11Asynchronous **pInterface ) { *pInterface = (ID3D11Asynchronous *) m_pQuery; }
    virtual CAsynchronous *GetSubAsynchronous( int index ) { return NULL; }

    ////////////////////////////////////////////////////////////
    // GetDataSize
    ////////////////////////////////////////////////////////////
    UINT GetDataSize(  );

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
    NRESULT GetData(  void* pData, UINT DataSize, UINT GetDataFlags );


    ////////////////////////////////////////////////////////////
    // Extra Methods
    ////////////////////////////////////////////////////////////

    //
    // CDeviceChild Overrides
    //

    ////////////////////////////////////////////////////////////
    // GetInterfacePtr
    ////////////////////////////////////////////////////////////
    virtual void GetInterfacePtr( ID3D11DeviceChild **pInterface ) { *pInterface = (ID3D11DeviceChild *) m_pQuery; }
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
// C11QueryGeneric
//
////////////////////////////////////////////////////////////

C11QueryGeneric::C11QueryGeneric( ID3D11Query *pQuery ) : 
    m_pQuery( pQuery )
{
}


C11QueryGeneric::~C11QueryGeneric()
{
    if( m_pQuery ) m_pQuery->Release();
}


////////////////////////////////////////////////////////////
// C11QueryGeneric::GetDesc()
////////////////////////////////////////////////////////////
void C11QueryGeneric::GetDesc(  D3D_QUERY_DESC * pDesc )
{
    assert( m_pQuery );
    BYTE *pDescByteTemp = NULL;
    D3D11_QUERY_DESC *pDescTemp = NULL;
    SIZE_T pDescSizeBuffer = sizeof(D3D11_QUERY_DESC);
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        pDescByteTemp = new BYTE[pDescSizeBuffer];
        pDescTemp = (D3D11_QUERY_DESC*)pDescByteTemp;
    }
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    m_pQuery->GetDesc(  pDescTemp );
    if( pDesc != NULL && pDescSizeBuffer != 0 )
    {
        D3DWrapperConvertStruct( pDesc, pDescTemp );
        delete[] pDescByteTemp;
    }
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11QueryGeneric::GetDataSize()
////////////////////////////////////////////////////////////
UINT C11QueryGeneric::GetDataSize(  )
{
    assert( m_pQuery );
    UINT rval = 0;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pQuery->GetDataSize(  );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11QueryGeneric::Begin()
////////////////////////////////////////////////////////////
void C11QueryGeneric::Begin(  )
{
    assert( m_pQuery );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->Begin(  m_pQuery );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11QueryGeneric::End()
////////////////////////////////////////////////////////////
void C11QueryGeneric::End(  )
{
    assert( m_pQuery );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    ID3D11DeviceContext* pDefContext = pTempDevice11->GetDefContext();
    pDefContext->End(  m_pQuery );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
}


////////////////////////////////////////////////////////////
// C11QueryGeneric::GetData()
////////////////////////////////////////////////////////////
NRESULT C11QueryGeneric::GetData(  void* pData, UINT DataSize, UINT GetDataFlags )
{
    assert( m_pQuery );
    NRESULT rval = S_OK;
    UINT temp_GetDataFlags;
    D3DFlagsConvertTo11_ASYNC_GETDATA_FLAG( &temp_GetDataFlags, GetDataFlags );
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = pTempDevice11->ExecDeferredContext( );
    if (FAILED(rval))
    {
        return rval;
    }
    ID3D11DeviceContext* pImmContext = pTempDevice11->GetImmContext();
    rval = pImmContext->GetData(  m_pQuery,  pData,  DataSize,  temp_GetDataFlags );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11QueryGeneric::GetDevice()
////////////////////////////////////////////////////////////
void C11QueryGeneric::GetDevice(  CDevice ** ppDevice )
{
    assert( m_pQuery );
    ID3D11Device *temp_pDevice = NULL;
    ID3D11Device **ppDeviceTemp = ppDevice == NULL ? NULL : &temp_pDevice;
    m_pQuery->GetDevice(  ppDeviceTemp );
    if( ppDevice != NULL )
    {
        D3DWrapperConvertInterface( ppDevice, *ppDeviceTemp );
    }
    if (temp_pDevice != NULL) temp_pDevice->Release();
}


////////////////////////////////////////////////////////////
// C11QueryGeneric::GetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11QueryGeneric::GetPrivateData(  REFGUID guid, UINT * pDataSize, void * pData )
{
    assert( m_pQuery );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pQuery->GetPrivateData(  guid,  pDataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11QueryGeneric::SetPrivateData()
////////////////////////////////////////////////////////////
NRESULT C11QueryGeneric::SetPrivateData(  REFGUID guid, UINT DataSize, const void * pData )
{
    assert( m_pQuery );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pQuery->SetPrivateData(  guid,  DataSize,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}


////////////////////////////////////////////////////////////
// C11QueryGeneric::SetPrivateDataInterface()
////////////////////////////////////////////////////////////
NRESULT C11QueryGeneric::SetPrivateDataInterface(  REFGUID guid, const IUnknown * pData )
{
    assert( m_pQuery );
    NRESULT rval = S_OK;
    C11Device* pTempDevice11 = NULL;
    this->GetDevice( (CDevice**)(&pTempDevice11) );
    rval = m_pQuery->SetPrivateDataInterface(  guid,  pData );
    if (pTempDevice11 != NULL) pTempDevice11->Release();
    return rval;
}

void D3DWrapperConvertInterface( CQuery **ppQuery, ID3D11Query *pQuery_11 )
{
    if( pQuery_11 )
    {
        *ppQuery = new C11QueryGeneric( pQuery_11 );
        if( !(*ppQuery) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppQuery)->AddRef();
    }
    else
    {
        *ppQuery = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11Query **ppQuery_11, CQuery *pQuery )
{
    if( pQuery )
    {
        pQuery->GetInterfacePtr( ppQuery_11 );
    }
    else
    {
        *ppQuery_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CPredicate **ppPredicate, ID3D11Predicate *pPredicate_11 )
{
    if( pPredicate_11 )
    {
        *ppPredicate = new C11Predicate( pPredicate_11 );
        if( !(*ppPredicate) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppPredicate)->AddRef();
    }
    else
    {
        *ppPredicate = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11Predicate **ppPredicate_11, CPredicate *pPredicate )
{
    if( pPredicate )
    {
        pPredicate->GetInterfacePtr( ppPredicate_11 );
    }
    else
    {
        *ppPredicate_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CCounter **ppCounter, ID3D11Counter *pCounter_11 )
{
    if( pCounter_11 )
    {
        *ppCounter = new C11Counter( pCounter_11 );
        if( !(*ppCounter) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppCounter)->AddRef();
    }
    else
    {
        *ppCounter = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11Counter **ppCounter_11, CCounter *pCounter )
{
    if( pCounter )
    {
        pCounter->GetInterfacePtr( ppCounter_11 );
    }
    else
    {
        *ppCounter_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CDevice **ppDevice, ID3D11Device *pDevice_11 )
{
    if( pDevice_11 )
    {
        UINT dataSize = sizeof(*ppDevice);
        HRESULT hr = pDevice_11->GetPrivateData(C11DeviceGuid, &dataSize, (void*)(ppDevice));
        assert(SUCCEEDED(hr) && *ppDevice != NULL);
        (*ppDevice)->AddRef();
    }
    else
    {
        *ppDevice = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11Device **ppDevice_11, CDevice *pDevice )
{
    if( pDevice )
    {
        pDevice->GetInterfacePtr( ppDevice_11 );
    }
    else
    {
        *ppDevice_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CDebug **ppDebug, ID3D11Debug *pDebug_11 )
{
    if( pDebug_11 )
    {
        *ppDebug = new C11Debug( pDebug_11 );
        if( !(*ppDebug) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppDebug)->AddRef();
    }
    else
    {
        *ppDebug = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11Debug **ppDebug_11, CDebug *pDebug )
{
    if( pDebug )
    {
        pDebug->GetInterfacePtr( ppDebug_11 );
    }
    else
    {
        *ppDebug_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CSwitchToRef **ppSwitchToRef, ID3D11SwitchToRef *pSwitchToRef_11 )
{
    if( pSwitchToRef_11 )
    {
        *ppSwitchToRef = new C11SwitchToRef( pSwitchToRef_11 );
        if( !(*ppSwitchToRef) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppSwitchToRef)->AddRef();
    }
    else
    {
        *ppSwitchToRef = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11SwitchToRef **ppSwitchToRef_11, CSwitchToRef *pSwitchToRef )
{
    if( pSwitchToRef )
    {
        pSwitchToRef->GetInterfacePtr( ppSwitchToRef_11 );
    }
    else
    {
        *ppSwitchToRef_11 = NULL;
    }
}

void D3DWrapperConvertInterface( CInfoQueue **ppInfoQueue, ID3D11InfoQueue *pInfoQueue_11 )
{
    if( pInfoQueue_11 )
    {
        *ppInfoQueue = new C11InfoQueue( pInfoQueue_11 );
        if( !(*ppInfoQueue) ) throw( WRAPPER_OUT_OF_MEMORY );
        (*ppInfoQueue)->AddRef();
    }
    else
    {
        *ppInfoQueue = NULL;
    }
}

void D3DWrapperConvertInterface( ID3D11InfoQueue **ppInfoQueue_11, CInfoQueue *pInfoQueue )
{
    if( pInfoQueue )
    {
        pInfoQueue->GetInterfacePtr( ppInfoQueue_11 );
    }
    else
    {
        *ppInfoQueue_11 = NULL;
    }
}
