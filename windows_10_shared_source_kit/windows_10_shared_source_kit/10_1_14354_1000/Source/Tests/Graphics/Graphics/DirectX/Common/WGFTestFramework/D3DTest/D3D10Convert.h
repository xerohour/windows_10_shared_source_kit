//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D10Convert.h
//* 
//* Description: 
//*     Interface specific D3D wrapper class declarations.
//*
//*****************************************************************************

// Ignore Prefast warnings 26060 (Range Precondition violation) and 28930 (Unused assignment of pointer variable)
#pragma warning( disable: 26060 )
#pragma warning( disable: 28930 )

#pragma once
#include "d3d10_1.h"
#include "D3DWrappedTypes.h"
#include "D3DWrapped.h"




//
// Enum Conversions
//

extern void D3DWrapperConvertEnum( D3D_INPUT_CLASSIFICATION *d3d_input_classification, D3D10_INPUT_CLASSIFICATION d3d_input_classification_10 );
extern void D3DWrapperConvertEnum( D3D10_INPUT_CLASSIFICATION *d3d_input_classification_10, D3D_INPUT_CLASSIFICATION d3d_input_classification );

extern void D3DWrapperConvertEnum( D3D_FILL_MODE *d3d_fill_mode, D3D10_FILL_MODE d3d_fill_mode_10 );
extern void D3DWrapperConvertEnum( D3D10_FILL_MODE *d3d_fill_mode_10, D3D_FILL_MODE d3d_fill_mode );

extern void D3DWrapperConvertEnum( D3D_PRIMITIVE_TOPOLOGY *d3d_primitive_topology, D3D_PRIMITIVE_TOPOLOGY d3d_primitive_topology_10 );
extern void D3DWrapperConvertEnum( D3D_PRIMITIVE_TOPOLOGY *d3d_primitive_topology_10, D3D_PRIMITIVE_TOPOLOGY d3d_primitive_topology );

extern void D3DWrapperConvertEnum( D3D_PRIMITIVE *d3d_primitive, D3D_PRIMITIVE d3d_primitive_10 );
extern void D3DWrapperConvertEnum( D3D_PRIMITIVE *d3d_primitive_10, D3D_PRIMITIVE d3d_primitive );

extern void D3DWrapperConvertEnum( D3D_CULL_MODE *d3d_cull_mode, D3D10_CULL_MODE d3d_cull_mode_10 );
extern void D3DWrapperConvertEnum( D3D10_CULL_MODE *d3d_cull_mode_10, D3D_CULL_MODE d3d_cull_mode );

extern void D3DWrapperConvertEnum( D3D_RESOURCE_DIMENSION *d3d_resource_dimension, D3D10_RESOURCE_DIMENSION d3d_resource_dimension_10 );
extern void D3DWrapperConvertEnum( D3D10_RESOURCE_DIMENSION *d3d_resource_dimension_10, D3D_RESOURCE_DIMENSION d3d_resource_dimension );

extern void D3DWrapperConvertEnum( D3D_SRV_DIMENSION *d3d_srv_dimension, D3D_SRV_DIMENSION d3d_srv_dimension_10 );
extern void D3DWrapperConvertEnum( D3D_SRV_DIMENSION *d3d_srv_dimension_10, D3D_SRV_DIMENSION d3d_srv_dimension );

extern void D3DWrapperConvertEnum( D3D_DSV_DIMENSION *d3d_dsv_dimension, D3D10_DSV_DIMENSION d3d_dsv_dimension_10 );
extern void D3DWrapperConvertEnum( D3D10_DSV_DIMENSION *d3d_dsv_dimension_10, D3D_DSV_DIMENSION d3d_dsv_dimension );

extern void D3DWrapperConvertEnum( D3D_RTV_DIMENSION *d3d_rtv_dimension, D3D10_RTV_DIMENSION d3d_rtv_dimension_10 );
extern void D3DWrapperConvertEnum( D3D10_RTV_DIMENSION *d3d_rtv_dimension_10, D3D_RTV_DIMENSION d3d_rtv_dimension );

extern void D3DWrapperConvertEnum( D3D_USAGE *d3d_usage, D3D10_USAGE d3d_usage_10 );
extern void D3DWrapperConvertEnum( D3D10_USAGE *d3d_usage_10, D3D_USAGE d3d_usage );

extern void D3DFlagsConvertFrom10_BIND_FLAG( UINT *bind_flag, UINT bind_flag_10 );
extern void D3DFlagsConvertTo10_BIND_FLAG( UINT *bind_flag_10, UINT bind_flag );

extern void D3DFlagsConvertFrom10_CPU_ACCESS_FLAG( UINT *cpu_access_flag, UINT cpu_access_flag_10 );
extern void D3DFlagsConvertTo10_CPU_ACCESS_FLAG( UINT *cpu_access_flag_10, UINT cpu_access_flag );

extern void D3DFlagsConvertFrom10_RESOURCE_MISC_FLAG( UINT *resource_misc_flag, UINT resource_misc_flag_10 );
extern void D3DFlagsConvertTo10_RESOURCE_MISC_FLAG( UINT *resource_misc_flag_10, UINT resource_misc_flag );

extern void D3DWrapperConvertEnum( D3D_MAP *d3d_map, D3D10_MAP d3d_map_10 );
extern void D3DWrapperConvertEnum( D3D10_MAP *d3d_map_10, D3D_MAP d3d_map );

extern void D3DFlagsConvertFrom10_MAP_FLAG( UINT *map_flag, UINT map_flag_10 );
extern void D3DFlagsConvertTo10_MAP_FLAG( UINT *map_flag_10, UINT map_flag );

extern void D3DFlagsConvertFrom10_RAISE_FLAG( UINT *raise_flag, UINT raise_flag_10 );
extern void D3DFlagsConvertTo10_RAISE_FLAG( UINT *raise_flag_10, UINT raise_flag );

extern void D3DFlagsConvertFrom10_CLEAR_FLAG( UINT *clear_flag, UINT clear_flag_10 );
extern void D3DFlagsConvertTo10_CLEAR_FLAG( UINT *clear_flag_10, UINT clear_flag );

extern void D3DWrapperConvertEnum( D3D_COMPARISON_FUNC *d3d_comparison_func, D3D10_COMPARISON_FUNC d3d_comparison_func_10 );
extern void D3DWrapperConvertEnum( D3D10_COMPARISON_FUNC *d3d_comparison_func_10, D3D_COMPARISON_FUNC d3d_comparison_func );

extern void D3DWrapperConvertEnum( D3D_DEPTH_WRITE_MASK *d3d_depth_write_mask, D3D10_DEPTH_WRITE_MASK d3d_depth_write_mask_10 );
extern void D3DWrapperConvertEnum( D3D10_DEPTH_WRITE_MASK *d3d_depth_write_mask_10, D3D_DEPTH_WRITE_MASK d3d_depth_write_mask );

extern void D3DWrapperConvertEnum( D3D_STENCIL_OP *d3d_stencil_op, D3D10_STENCIL_OP d3d_stencil_op_10 );
extern void D3DWrapperConvertEnum( D3D10_STENCIL_OP *d3d_stencil_op_10, D3D_STENCIL_OP d3d_stencil_op );

extern void D3DWrapperConvertEnum( D3D_BLEND *d3d_blend, D3D10_BLEND d3d_blend_10 );
extern void D3DWrapperConvertEnum( D3D10_BLEND *d3d_blend_10, D3D_BLEND d3d_blend );

extern void D3DWrapperConvertEnum( D3D_BLEND_OP *d3d_blend_op, D3D10_BLEND_OP d3d_blend_op_10 );
extern void D3DWrapperConvertEnum( D3D10_BLEND_OP *d3d_blend_op_10, D3D_BLEND_OP d3d_blend_op );

extern void D3DFlagsConvertFrom10_COLOR_WRITE_ENABLE( UINT8 *color_write_enable, UINT8 color_write_enable_10 );
extern void D3DFlagsConvertTo10_COLOR_WRITE_ENABLE( UINT8 *color_write_enable_10, UINT8 color_write_enable );

extern void D3DWrapperConvertEnum( D3D_TEXTURECUBE_FACE *d3d_texturecube_face, D3D10_TEXTURECUBE_FACE d3d_texturecube_face_10 );
extern void D3DWrapperConvertEnum( D3D10_TEXTURECUBE_FACE *d3d_texturecube_face_10, D3D_TEXTURECUBE_FACE d3d_texturecube_face );

extern void D3DWrapperConvertEnum( D3D_FILTER *d3d_filter, D3D10_FILTER d3d_filter_10 );
extern void D3DWrapperConvertEnum( D3D10_FILTER *d3d_filter_10, D3D_FILTER d3d_filter );

extern void D3DWrapperConvertEnum( D3D_FILTER_TYPE *d3d_filter_type, D3D10_FILTER_TYPE d3d_filter_type_10 );
extern void D3DWrapperConvertEnum( D3D10_FILTER_TYPE *d3d_filter_type_10, D3D_FILTER_TYPE d3d_filter_type );

extern void D3DWrapperConvertEnum( D3D_TEXTURE_ADDRESS_MODE *d3d_texture_address_mode, D3D10_TEXTURE_ADDRESS_MODE d3d_texture_address_mode_10 );
extern void D3DWrapperConvertEnum( D3D10_TEXTURE_ADDRESS_MODE *d3d_texture_address_mode_10, D3D_TEXTURE_ADDRESS_MODE d3d_texture_address_mode );

extern void D3DFlagsConvertFrom10_FORMAT_SUPPORT( UINT *format_support, UINT format_support_10 );
extern void D3DFlagsConvertTo10_FORMAT_SUPPORT( UINT *format_support_10, UINT format_support );

extern void D3DFlagsConvertFrom10_ASYNC_GETDATA_FLAG( UINT *async_getdata_flag, UINT async_getdata_flag_10 );
extern void D3DFlagsConvertTo10_ASYNC_GETDATA_FLAG( UINT *async_getdata_flag_10, UINT async_getdata_flag );

extern void D3DWrapperConvertEnum( D3D_QUERY *d3d_query, D3D10_QUERY d3d_query_10 );
extern void D3DWrapperConvertEnum( D3D10_QUERY *d3d_query_10, D3D_QUERY d3d_query );

extern void D3DFlagsConvertFrom10_QUERY_MISC_FLAG( UINT *query_misc_flag, UINT query_misc_flag_10 );
extern void D3DFlagsConvertTo10_QUERY_MISC_FLAG( UINT *query_misc_flag_10, UINT query_misc_flag );

extern void D3DWrapperConvertEnum( D3D_COUNTER *d3d_counter, D3D10_COUNTER d3d_counter_10 );
extern void D3DWrapperConvertEnum( D3D10_COUNTER *d3d_counter_10, D3D_COUNTER d3d_counter );

extern void D3DWrapperConvertEnum( D3D_COUNTER_TYPE *d3d_counter_type, D3D10_COUNTER_TYPE d3d_counter_type_10 );
extern void D3DWrapperConvertEnum( D3D10_COUNTER_TYPE *d3d_counter_type_10, D3D_COUNTER_TYPE d3d_counter_type );

extern void D3DFlagsConvertFrom10_CREATE_DEVICE_FLAG( UINT *create_device_flag, UINT create_device_flag_10 );
extern void D3DFlagsConvertTo10_CREATE_DEVICE_FLAG( UINT *create_device_flag_10, UINT create_device_flag );

extern void D3DWrapperConvertEnum( D3D_MESSAGE_CATEGORY *d3d_message_category, D3D10_MESSAGE_CATEGORY d3d_message_category_10 );
extern void D3DWrapperConvertEnum( D3D10_MESSAGE_CATEGORY *d3d_message_category_10, D3D_MESSAGE_CATEGORY d3d_message_category );

extern void D3DWrapperConvertEnum( D3D_MESSAGE_SEVERITY *d3d_message_severity, D3D10_MESSAGE_SEVERITY d3d_message_severity_10 );
extern void D3DWrapperConvertEnum( D3D10_MESSAGE_SEVERITY *d3d_message_severity_10, D3D_MESSAGE_SEVERITY d3d_message_severity );

extern void D3DWrapperConvertEnum( D3D_MESSAGE_ID *d3d_message_id, D3D10_MESSAGE_ID d3d_message_id_10 );
extern void D3DWrapperConvertEnum( D3D10_MESSAGE_ID *d3d_message_id_10, D3D_MESSAGE_ID d3d_message_id );

extern void D3DWrapperConvertEnum( D3D_DRIVER_TYPE *d3d_driver_type, D3D10_DRIVER_TYPE d3d_driver_type_10 );
extern void D3DWrapperConvertEnum( D3D10_DRIVER_TYPE *d3d_driver_type_10, D3D_DRIVER_TYPE d3d_driver_type );

extern void D3DWrapperConvertEnum( D3D_FEATURE_LEVEL1 *d3d_feature_level1, D3D10_FEATURE_LEVEL1 d3d_feature_level1_10 );
extern void D3DWrapperConvertEnum( D3D10_FEATURE_LEVEL1 *d3d_feature_level1_10, D3D_FEATURE_LEVEL1 d3d_feature_level1 );

extern void D3DWrapperConvertEnum( D3D_STANDARD_MULTISAMPLE_QUALITY_LEVELS *d3d_standard_multisample_quality_levels, D3D10_STANDARD_MULTISAMPLE_QUALITY_LEVELS d3d_standard_multisample_quality_levels_10 );
extern void D3DWrapperConvertEnum( D3D10_STANDARD_MULTISAMPLE_QUALITY_LEVELS *d3d_standard_multisample_quality_levels_10, D3D_STANDARD_MULTISAMPLE_QUALITY_LEVELS d3d_standard_multisample_quality_levels );

//
// Struct Conversions
//

extern void D3DWrapperConvertStruct( D3D_INPUT_ELEMENT_DESC *d3d_input_element_desc, D3D10_INPUT_ELEMENT_DESC const *d3d_input_element_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_INPUT_ELEMENT_DESC *d3d_input_element_desc_10, D3D_INPUT_ELEMENT_DESC const *d3d_input_element_desc );

extern void D3DWrapperConvertStruct( D3D_SO_DECLARATION_ENTRY *d3d_so_declaration_entry, D3D10_SO_DECLARATION_ENTRY const *d3d_so_declaration_entry_10 );
extern void D3DWrapperConvertStruct( D3D10_SO_DECLARATION_ENTRY *d3d_so_declaration_entry_10, D3D_SO_DECLARATION_ENTRY const *d3d_so_declaration_entry );

extern void D3DWrapperConvertStruct( D3D_VIEWPORT *d3d_viewport, D3D10_VIEWPORT const *d3d_viewport_10 );
extern void D3DWrapperConvertStruct( D3D10_VIEWPORT *d3d_viewport_10, D3D_VIEWPORT const *d3d_viewport );

extern void D3DWrapperConvertStruct( D3D_BOX *d3d_box, D3D10_BOX const *d3d_box_10 );
extern void D3DWrapperConvertStruct( D3D10_BOX *d3d_box_10, D3D_BOX const *d3d_box );

extern void D3DWrapperConvertStruct( D3D_DEPTH_STENCILOP_DESC *d3d_depth_stencilop_desc, D3D10_DEPTH_STENCILOP_DESC const *d3d_depth_stencilop_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_DEPTH_STENCILOP_DESC *d3d_depth_stencilop_desc_10, D3D_DEPTH_STENCILOP_DESC const *d3d_depth_stencilop_desc );

extern void D3DWrapperConvertStruct( D3D_DEPTH_STENCIL_DESC *d3d_depth_stencil_desc, D3D10_DEPTH_STENCIL_DESC const *d3d_depth_stencil_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_DEPTH_STENCIL_DESC *d3d_depth_stencil_desc_10, D3D_DEPTH_STENCIL_DESC const *d3d_depth_stencil_desc );

extern void D3DWrapperConvertStruct( D3D_BLEND_DESC *d3d_blend_desc, D3D10_BLEND_DESC const *d3d_blend_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_BLEND_DESC *d3d_blend_desc_10, D3D_BLEND_DESC const *d3d_blend_desc );

extern void D3DWrapperConvertStruct( D3D_RASTERIZER_DESC *d3d_rasterizer_desc, D3D10_RASTERIZER_DESC const *d3d_rasterizer_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_RASTERIZER_DESC *d3d_rasterizer_desc_10, D3D_RASTERIZER_DESC const *d3d_rasterizer_desc );

extern void D3DWrapperConvertStruct( D3D_SUBRESOURCE_DATA *d3d_subresource_data, D3D10_SUBRESOURCE_DATA const *d3d_subresource_data_10 );
extern void D3DWrapperConvertStruct( D3D10_SUBRESOURCE_DATA *d3d_subresource_data_10, D3D_SUBRESOURCE_DATA const *d3d_subresource_data );

extern void D3DWrapperConvertStruct( D3D_BUFFER_DESC *d3d_buffer_desc, D3D10_BUFFER_DESC const *d3d_buffer_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_BUFFER_DESC *d3d_buffer_desc_10, D3D_BUFFER_DESC const *d3d_buffer_desc );

extern void D3DWrapperConvertStruct( D3D_TEXTURE1D_DESC *d3d_texture1d_desc, D3D10_TEXTURE1D_DESC const *d3d_texture1d_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_TEXTURE1D_DESC *d3d_texture1d_desc_10, D3D_TEXTURE1D_DESC const *d3d_texture1d_desc );

extern void D3DWrapperConvertStruct( D3D_TEXTURE2D_DESC *d3d_texture2d_desc, D3D10_TEXTURE2D_DESC const *d3d_texture2d_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_TEXTURE2D_DESC *d3d_texture2d_desc_10, D3D_TEXTURE2D_DESC const *d3d_texture2d_desc );

extern void D3DWrapperConvertStruct( D3D_MAPPED_TEXTURE2D *d3d_mapped_texture2d, D3D10_MAPPED_TEXTURE2D const *d3d_mapped_texture2d_10 );
extern void D3DWrapperConvertStruct( D3D10_MAPPED_TEXTURE2D *d3d_mapped_texture2d_10, D3D_MAPPED_TEXTURE2D const *d3d_mapped_texture2d );

extern void D3DWrapperConvertStruct( D3D_TEXTURE3D_DESC *d3d_texture3d_desc, D3D10_TEXTURE3D_DESC const *d3d_texture3d_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_TEXTURE3D_DESC *d3d_texture3d_desc_10, D3D_TEXTURE3D_DESC const *d3d_texture3d_desc );

extern void D3DWrapperConvertStruct( D3D_MAPPED_TEXTURE3D *d3d_mapped_texture3d, D3D10_MAPPED_TEXTURE3D const *d3d_mapped_texture3d_10 );
extern void D3DWrapperConvertStruct( D3D10_MAPPED_TEXTURE3D *d3d_mapped_texture3d_10, D3D_MAPPED_TEXTURE3D const *d3d_mapped_texture3d );

extern void D3DWrapperConvertStruct( D3D_BUFFER_SRV *d3d_buffer_srv, D3D10_BUFFER_SRV const *d3d_buffer_srv_10 );
extern void D3DWrapperConvertStruct( D3D10_BUFFER_SRV *d3d_buffer_srv_10, D3D_BUFFER_SRV const *d3d_buffer_srv );

extern void D3DWrapperConvertStruct( D3D_TEX1D_SRV *d3d_tex1d_srv, D3D10_TEX1D_SRV const *d3d_tex1d_srv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX1D_SRV *d3d_tex1d_srv_10, D3D_TEX1D_SRV const *d3d_tex1d_srv );

extern void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_SRV *d3d_tex1d_array_srv, D3D10_TEX1D_ARRAY_SRV const *d3d_tex1d_array_srv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX1D_ARRAY_SRV *d3d_tex1d_array_srv_10, D3D_TEX1D_ARRAY_SRV const *d3d_tex1d_array_srv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_SRV *d3d_tex2d_srv, D3D10_TEX2D_SRV const *d3d_tex2d_srv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2D_SRV *d3d_tex2d_srv_10, D3D_TEX2D_SRV const *d3d_tex2d_srv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_SRV *d3d_tex2d_array_srv, D3D10_TEX2D_ARRAY_SRV const *d3d_tex2d_array_srv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2D_ARRAY_SRV *d3d_tex2d_array_srv_10, D3D_TEX2D_ARRAY_SRV const *d3d_tex2d_array_srv );

extern void D3DWrapperConvertStruct( D3D_TEX3D_SRV *d3d_tex3d_srv, D3D10_TEX3D_SRV const *d3d_tex3d_srv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX3D_SRV *d3d_tex3d_srv_10, D3D_TEX3D_SRV const *d3d_tex3d_srv );

extern void D3DWrapperConvertStruct( D3D_TEXCUBE_SRV *d3d_texcube_srv, D3D10_TEXCUBE_SRV const *d3d_texcube_srv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEXCUBE_SRV *d3d_texcube_srv_10, D3D_TEXCUBE_SRV const *d3d_texcube_srv );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_SRV *d3d_tex2dms_srv, D3D10_TEX2DMS_SRV const *d3d_tex2dms_srv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2DMS_SRV *d3d_tex2dms_srv_10, D3D_TEX2DMS_SRV const *d3d_tex2dms_srv );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_SRV *d3d_tex2dms_array_srv, D3D10_TEX2DMS_ARRAY_SRV const *d3d_tex2dms_array_srv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2DMS_ARRAY_SRV *d3d_tex2dms_array_srv_10, D3D_TEX2DMS_ARRAY_SRV const *d3d_tex2dms_array_srv );

extern void D3DWrapperConvertStruct( D3D_SHADER_RESOURCE_VIEW_DESC *d3d_shader_resource_view_desc, D3D10_SHADER_RESOURCE_VIEW_DESC const *d3d_shader_resource_view_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_SHADER_RESOURCE_VIEW_DESC *d3d_shader_resource_view_desc_10, D3D_SHADER_RESOURCE_VIEW_DESC const *d3d_shader_resource_view_desc );

extern void D3DWrapperConvertStruct( D3D_BUFFER_RTV *d3d_buffer_rtv, D3D10_BUFFER_RTV const *d3d_buffer_rtv_10 );
extern void D3DWrapperConvertStruct( D3D10_BUFFER_RTV *d3d_buffer_rtv_10, D3D_BUFFER_RTV const *d3d_buffer_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX1D_RTV *d3d_tex1d_rtv, D3D10_TEX1D_RTV const *d3d_tex1d_rtv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX1D_RTV *d3d_tex1d_rtv_10, D3D_TEX1D_RTV const *d3d_tex1d_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_RTV *d3d_tex1d_array_rtv, D3D10_TEX1D_ARRAY_RTV const *d3d_tex1d_array_rtv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX1D_ARRAY_RTV *d3d_tex1d_array_rtv_10, D3D_TEX1D_ARRAY_RTV const *d3d_tex1d_array_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_RTV *d3d_tex2d_rtv, D3D10_TEX2D_RTV const *d3d_tex2d_rtv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2D_RTV *d3d_tex2d_rtv_10, D3D_TEX2D_RTV const *d3d_tex2d_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_RTV *d3d_tex2dms_rtv, D3D10_TEX2DMS_RTV const *d3d_tex2dms_rtv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2DMS_RTV *d3d_tex2dms_rtv_10, D3D_TEX2DMS_RTV const *d3d_tex2dms_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_RTV *d3d_tex2d_array_rtv, D3D10_TEX2D_ARRAY_RTV const *d3d_tex2d_array_rtv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2D_ARRAY_RTV *d3d_tex2d_array_rtv_10, D3D_TEX2D_ARRAY_RTV const *d3d_tex2d_array_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_RTV *d3d_tex2dms_array_rtv, D3D10_TEX2DMS_ARRAY_RTV const *d3d_tex2dms_array_rtv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2DMS_ARRAY_RTV *d3d_tex2dms_array_rtv_10, D3D_TEX2DMS_ARRAY_RTV const *d3d_tex2dms_array_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX3D_RTV *d3d_tex3d_rtv, D3D10_TEX3D_RTV const *d3d_tex3d_rtv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX3D_RTV *d3d_tex3d_rtv_10, D3D_TEX3D_RTV const *d3d_tex3d_rtv );

extern void D3DWrapperConvertStruct( D3D_RENDER_TARGET_VIEW_DESC *d3d_render_target_view_desc, D3D10_RENDER_TARGET_VIEW_DESC const *d3d_render_target_view_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_RENDER_TARGET_VIEW_DESC *d3d_render_target_view_desc_10, D3D_RENDER_TARGET_VIEW_DESC const *d3d_render_target_view_desc );

extern void D3DWrapperConvertStruct( D3D_TEX1D_DSV *d3d_tex1d_dsv, D3D10_TEX1D_DSV const *d3d_tex1d_dsv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX1D_DSV *d3d_tex1d_dsv_10, D3D_TEX1D_DSV const *d3d_tex1d_dsv );

extern void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_DSV *d3d_tex1d_array_dsv, D3D10_TEX1D_ARRAY_DSV const *d3d_tex1d_array_dsv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX1D_ARRAY_DSV *d3d_tex1d_array_dsv_10, D3D_TEX1D_ARRAY_DSV const *d3d_tex1d_array_dsv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_DSV *d3d_tex2d_dsv, D3D10_TEX2D_DSV const *d3d_tex2d_dsv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2D_DSV *d3d_tex2d_dsv_10, D3D_TEX2D_DSV const *d3d_tex2d_dsv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_DSV *d3d_tex2d_array_dsv, D3D10_TEX2D_ARRAY_DSV const *d3d_tex2d_array_dsv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2D_ARRAY_DSV *d3d_tex2d_array_dsv_10, D3D_TEX2D_ARRAY_DSV const *d3d_tex2d_array_dsv );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_DSV *d3d_tex2dms_dsv, D3D10_TEX2DMS_DSV const *d3d_tex2dms_dsv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2DMS_DSV *d3d_tex2dms_dsv_10, D3D_TEX2DMS_DSV const *d3d_tex2dms_dsv );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_DSV *d3d_tex2dms_array_dsv, D3D10_TEX2DMS_ARRAY_DSV const *d3d_tex2dms_array_dsv_10 );
extern void D3DWrapperConvertStruct( D3D10_TEX2DMS_ARRAY_DSV *d3d_tex2dms_array_dsv_10, D3D_TEX2DMS_ARRAY_DSV const *d3d_tex2dms_array_dsv );

extern void D3DWrapperConvertStruct( D3D_DEPTH_STENCIL_VIEW_DESC *d3d_depth_stencil_view_desc, D3D10_DEPTH_STENCIL_VIEW_DESC const *d3d_depth_stencil_view_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_DEPTH_STENCIL_VIEW_DESC *d3d_depth_stencil_view_desc_10, D3D_DEPTH_STENCIL_VIEW_DESC const *d3d_depth_stencil_view_desc );

extern void D3DWrapperConvertStruct( D3D_SAMPLER_DESC *d3d_sampler_desc, D3D10_SAMPLER_DESC const *d3d_sampler_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_SAMPLER_DESC *d3d_sampler_desc_10, D3D_SAMPLER_DESC const *d3d_sampler_desc );

extern void D3DWrapperConvertStruct( D3D_QUERY_DESC *d3d_query_desc, D3D10_QUERY_DESC const *d3d_query_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_QUERY_DESC *d3d_query_desc_10, D3D_QUERY_DESC const *d3d_query_desc );

extern void D3DWrapperConvertStruct( D3D_QUERY_DATA_TIMESTAMP_DISJOINT *d3d_query_data_timestamp_disjoint, D3D10_QUERY_DATA_TIMESTAMP_DISJOINT const *d3d_query_data_timestamp_disjoint_10 );
extern void D3DWrapperConvertStruct( D3D10_QUERY_DATA_TIMESTAMP_DISJOINT *d3d_query_data_timestamp_disjoint_10, D3D_QUERY_DATA_TIMESTAMP_DISJOINT const *d3d_query_data_timestamp_disjoint );

extern void D3DWrapperConvertStruct( D3D_QUERY_DATA_PIPELINE_STATISTICS *d3d_query_data_pipeline_statistics, D3D10_QUERY_DATA_PIPELINE_STATISTICS const *d3d_query_data_pipeline_statistics_10 );
extern void D3DWrapperConvertStruct( D3D10_QUERY_DATA_PIPELINE_STATISTICS *d3d_query_data_pipeline_statistics_10, D3D_QUERY_DATA_PIPELINE_STATISTICS const *d3d_query_data_pipeline_statistics );

extern void D3DWrapperConvertStruct( D3D_QUERY_DATA_SO_STATISTICS *d3d_query_data_so_statistics, D3D10_QUERY_DATA_SO_STATISTICS const *d3d_query_data_so_statistics_10 );
extern void D3DWrapperConvertStruct( D3D10_QUERY_DATA_SO_STATISTICS *d3d_query_data_so_statistics_10, D3D_QUERY_DATA_SO_STATISTICS const *d3d_query_data_so_statistics );

extern void D3DWrapperConvertStruct( D3D_COUNTER_DESC *d3d_counter_desc, D3D10_COUNTER_DESC const *d3d_counter_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_COUNTER_DESC *d3d_counter_desc_10, D3D_COUNTER_DESC const *d3d_counter_desc );

extern void D3DWrapperConvertStruct( D3D_COUNTER_INFO *d3d_counter_info, D3D10_COUNTER_INFO const *d3d_counter_info_10 );
extern void D3DWrapperConvertStruct( D3D10_COUNTER_INFO *d3d_counter_info_10, D3D_COUNTER_INFO const *d3d_counter_info );

extern void D3DWrapperConvertStruct( D3D_MESSAGE *d3d_message, D3D10_MESSAGE const *d3d_message_10 );
extern void D3DWrapperConvertStruct( D3D10_MESSAGE *d3d_message_10, D3D_MESSAGE const *d3d_message );

extern void D3DWrapperConvertStruct( D3D_INFO_QUEUE_FILTER_DESC *d3d_info_queue_filter_desc, D3D10_INFO_QUEUE_FILTER_DESC const *d3d_info_queue_filter_desc_10 );
extern void D3DWrapperConvertStruct( D3D10_INFO_QUEUE_FILTER_DESC *d3d_info_queue_filter_desc_10, D3D_INFO_QUEUE_FILTER_DESC const *d3d_info_queue_filter_desc );

extern void D3DWrapperConvertStruct( D3D_INFO_QUEUE_FILTER *d3d_info_queue_filter, D3D10_INFO_QUEUE_FILTER const *d3d_info_queue_filter_10 );
extern void D3DWrapperConvertStruct( D3D10_INFO_QUEUE_FILTER *d3d_info_queue_filter_10, D3D_INFO_QUEUE_FILTER const *d3d_info_queue_filter );

extern void D3DWrapperConvertStruct( D3D_RENDER_TARGET_BLEND_DESC1 *d3d_render_target_blend_desc1, D3D10_RENDER_TARGET_BLEND_DESC1 const *d3d_render_target_blend_desc1_10 );
extern void D3DWrapperConvertStruct( D3D10_RENDER_TARGET_BLEND_DESC1 *d3d_render_target_blend_desc1_10, D3D_RENDER_TARGET_BLEND_DESC1 const *d3d_render_target_blend_desc1 );

extern void D3DWrapperConvertStruct( D3D_BLEND_DESC1 *d3d_blend_desc1, D3D10_BLEND_DESC1 const *d3d_blend_desc1_10 );
extern void D3DWrapperConvertStruct( D3D10_BLEND_DESC1 *d3d_blend_desc1_10, D3D_BLEND_DESC1 const *d3d_blend_desc1 );

extern void D3DWrapperConvertStruct( D3D_TEXCUBE_ARRAY_SRV1 *d3d_texcube_array_srv1, D3D10_TEXCUBE_ARRAY_SRV1 const *d3d_texcube_array_srv1_10 );
extern void D3DWrapperConvertStruct( D3D10_TEXCUBE_ARRAY_SRV1 *d3d_texcube_array_srv1_10, D3D_TEXCUBE_ARRAY_SRV1 const *d3d_texcube_array_srv1 );

extern void D3DWrapperConvertStruct( D3D_SHADER_RESOURCE_VIEW_DESC1 *d3d_shader_resource_view_desc1, D3D10_SHADER_RESOURCE_VIEW_DESC1 const *d3d_shader_resource_view_desc1_10 );
extern void D3DWrapperConvertStruct( D3D10_SHADER_RESOURCE_VIEW_DESC1 *d3d_shader_resource_view_desc1_10, D3D_SHADER_RESOURCE_VIEW_DESC1 const *d3d_shader_resource_view_desc1 );


//
// Interface Conversions
//

extern void D3DWrapperConvertInterface( CDeviceChild **ppDeviceChild, ID3D10DeviceChild *pDeviceChild_10 );
extern void D3DWrapperConvertInterface( ID3D10DeviceChild **ppDeviceChild_10, CDeviceChild *pDeviceChild );

extern void D3DWrapperConvertInterface( CDepthStencilState **ppDepthStencilState, ID3D10DepthStencilState *pDepthStencilState_10 );
extern void D3DWrapperConvertInterface( ID3D10DepthStencilState **ppDepthStencilState_10, CDepthStencilState *pDepthStencilState );

extern void D3DWrapperConvertInterface( CBlendState **ppBlendState, ID3D10BlendState *pBlendState_10 );
extern void D3DWrapperConvertInterface( ID3D10BlendState **ppBlendState_10, CBlendState *pBlendState );

extern void D3DWrapperConvertInterface( CRasterizerState **ppRasterizerState, ID3D10RasterizerState *pRasterizerState_10 );
extern void D3DWrapperConvertInterface( ID3D10RasterizerState **ppRasterizerState_10, CRasterizerState *pRasterizerState );

extern void D3DWrapperConvertInterface( CResource **ppResource, ID3D10Resource *pResource_10 );
extern void D3DWrapperConvertInterface( ID3D10Resource **ppResource_10, CResource *pResource );

extern void D3DWrapperConvertInterface( CBuffer **ppBuffer, ID3D10Buffer *pBuffer_10 );
extern void D3DWrapperConvertInterface( ID3D10Buffer **ppBuffer_10, CBuffer *pBuffer );

extern void D3DWrapperConvertInterface( CTexture1D **ppTexture1D, ID3D10Texture1D *pTexture1D_10 );
extern void D3DWrapperConvertInterface( ID3D10Texture1D **ppTexture1D_10, CTexture1D *pTexture1D );

extern void D3DWrapperConvertInterface( CTexture2D **ppTexture2D, ID3D10Texture2D *pTexture2D_10 );
extern void D3DWrapperConvertInterface( ID3D10Texture2D **ppTexture2D_10, CTexture2D *pTexture2D );

extern void D3DWrapperConvertInterface( CTexture3D **ppTexture3D, ID3D10Texture3D *pTexture3D_10 );
extern void D3DWrapperConvertInterface( ID3D10Texture3D **ppTexture3D_10, CTexture3D *pTexture3D );

extern void D3DWrapperConvertInterface( CView **ppView, ID3D10View *pView_10 );
extern void D3DWrapperConvertInterface( ID3D10View **ppView_10, CView *pView );

extern void D3DWrapperConvertInterface( CShaderResourceView **ppShaderResourceView, ID3D10ShaderResourceView *pShaderResourceView_10 );
extern void D3DWrapperConvertInterface( ID3D10ShaderResourceView **ppShaderResourceView_10, CShaderResourceView *pShaderResourceView );

extern void D3DWrapperConvertInterface( CRenderTargetView **ppRenderTargetView, ID3D10RenderTargetView *pRenderTargetView_10 );
extern void D3DWrapperConvertInterface( ID3D10RenderTargetView **ppRenderTargetView_10, CRenderTargetView *pRenderTargetView );

extern void D3DWrapperConvertInterface( CDepthStencilView **ppDepthStencilView, ID3D10DepthStencilView *pDepthStencilView_10 );
extern void D3DWrapperConvertInterface( ID3D10DepthStencilView **ppDepthStencilView_10, CDepthStencilView *pDepthStencilView );

extern void D3DWrapperConvertInterface( CVertexShader **ppVertexShader, ID3D10VertexShader *pVertexShader_10 );
extern void D3DWrapperConvertInterface( ID3D10VertexShader **ppVertexShader_10, CVertexShader *pVertexShader );

extern void D3DWrapperConvertInterface( CGeometryShader **ppGeometryShader, ID3D10GeometryShader *pGeometryShader_10 );
extern void D3DWrapperConvertInterface( ID3D10GeometryShader **ppGeometryShader_10, CGeometryShader *pGeometryShader );

extern void D3DWrapperConvertInterface( CPixelShader **ppPixelShader, ID3D10PixelShader *pPixelShader_10 );
extern void D3DWrapperConvertInterface( ID3D10PixelShader **ppPixelShader_10, CPixelShader *pPixelShader );

extern void D3DWrapperConvertInterface( CInputLayout **ppInputLayout, ID3D10InputLayout *pInputLayout_10 );
extern void D3DWrapperConvertInterface( ID3D10InputLayout **ppInputLayout_10, CInputLayout *pInputLayout );

extern void D3DWrapperConvertInterface( CSamplerState **ppSamplerState, ID3D10SamplerState *pSamplerState_10 );
extern void D3DWrapperConvertInterface( ID3D10SamplerState **ppSamplerState_10, CSamplerState *pSamplerState );

extern void D3DWrapperConvertInterface( CAsynchronous **ppAsynchronous, ID3D10Asynchronous *pAsynchronous_10 );
extern void D3DWrapperConvertInterface( ID3D10Asynchronous **ppAsynchronous_10, CAsynchronous *pAsynchronous );

extern void D3DWrapperConvertInterface( CQuery **ppQuery, ID3D10Query *pQuery_10 );
extern void D3DWrapperConvertInterface( ID3D10Query **ppQuery_10, CQuery *pQuery );

extern void D3DWrapperConvertInterface( CPredicate **ppPredicate, ID3D10Predicate *pPredicate_10 );
extern void D3DWrapperConvertInterface( ID3D10Predicate **ppPredicate_10, CPredicate *pPredicate );

extern void D3DWrapperConvertInterface( CCounter **ppCounter, ID3D10Counter *pCounter_10 );
extern void D3DWrapperConvertInterface( ID3D10Counter **ppCounter_10, CCounter *pCounter );

extern void D3DWrapperConvertInterface( CDevice **ppDevice, ID3D10Device *pDevice_10 );
extern void D3DWrapperConvertInterface( ID3D10Device **ppDevice_10, CDevice *pDevice );

extern void D3DWrapperConvertInterface( CMultithread **ppMultithread, ID3D10Multithread *pMultithread_10 );
extern void D3DWrapperConvertInterface( ID3D10Multithread **ppMultithread_10, CMultithread *pMultithread );

extern void D3DWrapperConvertInterface( CDebug **ppDebug, ID3D10Debug *pDebug_10 );
extern void D3DWrapperConvertInterface( ID3D10Debug **ppDebug_10, CDebug *pDebug );

extern void D3DWrapperConvertInterface( CSwitchToRef **ppSwitchToRef, ID3D10SwitchToRef *pSwitchToRef_10 );
extern void D3DWrapperConvertInterface( ID3D10SwitchToRef **ppSwitchToRef_10, CSwitchToRef *pSwitchToRef );

extern void D3DWrapperConvertInterface( CInfoQueue **ppInfoQueue, ID3D10InfoQueue *pInfoQueue_10 );
extern void D3DWrapperConvertInterface( ID3D10InfoQueue **ppInfoQueue_10, CInfoQueue *pInfoQueue );

extern void D3DWrapperConvertInterface( CBlob **ppBlob, ID3D10Blob *pBlob_10 );
extern void D3DWrapperConvertInterface( ID3D10Blob **ppBlob_10, CBlob *pBlob );

extern void D3DWrapperConvertInterface( CBlendState1 **ppBlendState1, ID3D10BlendState1 *pBlendState1_10 );
extern void D3DWrapperConvertInterface( ID3D10BlendState1 **ppBlendState1_10, CBlendState1 *pBlendState1 );

extern void D3DWrapperConvertInterface( CShaderResourceView1 **ppShaderResourceView1, ID3D10ShaderResourceView1 *pShaderResourceView1_10 );
extern void D3DWrapperConvertInterface( ID3D10ShaderResourceView1 **ppShaderResourceView1_10, CShaderResourceView1 *pShaderResourceView1 );

extern void D3DWrapperConvertInterface( CDevice **ppDevice1, ID3D10Device1 *pDevice1_10 );
extern void D3DWrapperConvertInterface( ID3D10Device1 **ppDevice1_10, CDevice *pDevice1 );
