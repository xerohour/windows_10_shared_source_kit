//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D11Convert.h
//* 
//* Description: 
//*     Interface specific D3D wrapper class declarations.
//*
//*****************************************************************************

#pragma once
#include "d3d11.h"
#include "D3DWrappedTypes.h"
#include "D3DWrapped.h"




//
// Enum Conversions
//

extern void D3DWrapperConvertEnum( D3D_INPUT_CLASSIFICATION *d3d_input_classification, D3D11_INPUT_CLASSIFICATION d3d_input_classification_11 );
extern void D3DWrapperConvertEnum( D3D11_INPUT_CLASSIFICATION *d3d_input_classification_11, D3D_INPUT_CLASSIFICATION d3d_input_classification );

extern void D3DWrapperConvertEnum( D3D_FILL_MODE *d3d_fill_mode, D3D11_FILL_MODE d3d_fill_mode_11 );
extern void D3DWrapperConvertEnum( D3D11_FILL_MODE *d3d_fill_mode_11, D3D_FILL_MODE d3d_fill_mode );

extern void D3DWrapperConvertEnum( D3D_PRIMITIVE_TOPOLOGY *d3d_primitive_topology, D3D11_PRIMITIVE_TOPOLOGY d3d_primitive_topology_11 );
extern void D3DWrapperConvertEnum( D3D11_PRIMITIVE_TOPOLOGY *d3d_primitive_topology_11, D3D_PRIMITIVE_TOPOLOGY d3d_primitive_topology );

extern void D3DWrapperConvertEnum( D3D_PRIMITIVE *d3d_primitive, D3D11_PRIMITIVE d3d_primitive_11 );
extern void D3DWrapperConvertEnum( D3D11_PRIMITIVE *d3d_primitive_11, D3D_PRIMITIVE d3d_primitive );

extern void D3DWrapperConvertEnum( D3D_CULL_MODE *d3d_cull_mode, D3D11_CULL_MODE d3d_cull_mode_11 );
extern void D3DWrapperConvertEnum( D3D11_CULL_MODE *d3d_cull_mode_11, D3D_CULL_MODE d3d_cull_mode );

extern void D3DWrapperConvertEnum( D3D_RESOURCE_DIMENSION *d3d_resource_dimension, D3D11_RESOURCE_DIMENSION d3d_resource_dimension_11 );
extern void D3DWrapperConvertEnum( D3D11_RESOURCE_DIMENSION *d3d_resource_dimension_11, D3D_RESOURCE_DIMENSION d3d_resource_dimension );

extern void D3DWrapperConvertEnum( D3D_SRV_DIMENSION *d3d_srv_dimension, D3D11_SRV_DIMENSION d3d_srv_dimension_11 );
extern void D3DWrapperConvertEnum( D3D11_SRV_DIMENSION *d3d_srv_dimension_11, D3D_SRV_DIMENSION d3d_srv_dimension );

extern void D3DWrapperConvertEnum( D3D_DSV_DIMENSION *d3d_dsv_dimension, D3D11_DSV_DIMENSION d3d_dsv_dimension_11 );
extern void D3DWrapperConvertEnum( D3D11_DSV_DIMENSION *d3d_dsv_dimension_11, D3D_DSV_DIMENSION d3d_dsv_dimension );

extern void D3DWrapperConvertEnum( D3D_RTV_DIMENSION *d3d_rtv_dimension, D3D11_RTV_DIMENSION d3d_rtv_dimension_11 );
extern void D3DWrapperConvertEnum( D3D11_RTV_DIMENSION *d3d_rtv_dimension_11, D3D_RTV_DIMENSION d3d_rtv_dimension );

extern void D3DWrapperConvertEnum( D3D_USAGE *d3d_usage, D3D11_USAGE d3d_usage_11 );
extern void D3DWrapperConvertEnum( D3D11_USAGE *d3d_usage_11, D3D_USAGE d3d_usage );

extern void D3DFlagsConvertFrom11_BIND_FLAG( UINT *bind_flag, UINT bind_flag_11 );
extern void D3DFlagsConvertTo11_BIND_FLAG( UINT *bind_flag_11, UINT bind_flag );

extern void D3DFlagsConvertFrom11_CPU_ACCESS_FLAG( UINT *cpu_access_flag, UINT cpu_access_flag_11 );
extern void D3DFlagsConvertTo11_CPU_ACCESS_FLAG( UINT *cpu_access_flag_11, UINT cpu_access_flag );

extern void D3DFlagsConvertFrom11_RESOURCE_MISC_FLAG( UINT *resource_misc_flag, UINT resource_misc_flag_11 );
extern void D3DFlagsConvertTo11_RESOURCE_MISC_FLAG( UINT *resource_misc_flag_11, UINT resource_misc_flag );

extern void D3DWrapperConvertEnum( D3D_MAP *d3d_map, D3D11_MAP d3d_map_11 );
extern void D3DWrapperConvertEnum( D3D11_MAP *d3d_map_11, D3D_MAP d3d_map );

extern void D3DFlagsConvertFrom11_MAP_FLAG( UINT *map_flag, UINT map_flag_11 );
extern void D3DFlagsConvertTo11_MAP_FLAG( UINT *map_flag_11, UINT map_flag );

extern void D3DFlagsConvertFrom11_RAISE_FLAG( UINT *raise_flag, UINT raise_flag_11 );
extern void D3DFlagsConvertTo11_RAISE_FLAG( UINT *raise_flag_11, UINT raise_flag );

extern void D3DFlagsConvertFrom11_CLEAR_FLAG( UINT *clear_flag, UINT clear_flag_11 );
extern void D3DFlagsConvertTo11_CLEAR_FLAG( UINT *clear_flag_11, UINT clear_flag );

extern void D3DWrapperConvertEnum( D3D_COMPARISON_FUNC *d3d_comparison_func, D3D11_COMPARISON_FUNC d3d_comparison_func_11 );
extern void D3DWrapperConvertEnum( D3D11_COMPARISON_FUNC *d3d_comparison_func_11, D3D_COMPARISON_FUNC d3d_comparison_func );

extern void D3DWrapperConvertEnum( D3D_DEPTH_WRITE_MASK *d3d_depth_write_mask, D3D11_DEPTH_WRITE_MASK d3d_depth_write_mask_11 );
extern void D3DWrapperConvertEnum( D3D11_DEPTH_WRITE_MASK *d3d_depth_write_mask_11, D3D_DEPTH_WRITE_MASK d3d_depth_write_mask );

extern void D3DWrapperConvertEnum( D3D_STENCIL_OP *d3d_stencil_op, D3D11_STENCIL_OP d3d_stencil_op_11 );
extern void D3DWrapperConvertEnum( D3D11_STENCIL_OP *d3d_stencil_op_11, D3D_STENCIL_OP d3d_stencil_op );

extern void D3DWrapperConvertEnum( D3D_BLEND *d3d_blend, D3D11_BLEND d3d_blend_11 );
extern void D3DWrapperConvertEnum( D3D11_BLEND *d3d_blend_11, D3D_BLEND d3d_blend );

extern void D3DWrapperConvertEnum( D3D_BLEND_OP *d3d_blend_op, D3D11_BLEND_OP d3d_blend_op_11 );
extern void D3DWrapperConvertEnum( D3D11_BLEND_OP *d3d_blend_op_11, D3D_BLEND_OP d3d_blend_op );

extern void D3DFlagsConvertFrom11_COLOR_WRITE_ENABLE( UINT8 *color_write_enable, UINT8 color_write_enable_11 );
extern void D3DFlagsConvertTo11_COLOR_WRITE_ENABLE( UINT8 *color_write_enable_11, UINT8 color_write_enable );

extern void D3DWrapperConvertEnum( D3D_TEXTURECUBE_FACE *d3d_texturecube_face, D3D11_TEXTURECUBE_FACE d3d_texturecube_face_11 );
extern void D3DWrapperConvertEnum( D3D11_TEXTURECUBE_FACE *d3d_texturecube_face_11, D3D_TEXTURECUBE_FACE d3d_texturecube_face );

extern void D3DWrapperConvertEnum( D3D_FILTER *d3d_filter, D3D11_FILTER d3d_filter_11 );
extern void D3DWrapperConvertEnum( D3D11_FILTER *d3d_filter_11, D3D_FILTER d3d_filter );

extern void D3DWrapperConvertEnum( D3D_FILTER_TYPE *d3d_filter_type, D3D11_FILTER_TYPE d3d_filter_type_11 );
extern void D3DWrapperConvertEnum( D3D11_FILTER_TYPE *d3d_filter_type_11, D3D_FILTER_TYPE d3d_filter_type );

extern void D3DWrapperConvertEnum( D3D_TEXTURE_ADDRESS_MODE *d3d_texture_address_mode, D3D11_TEXTURE_ADDRESS_MODE d3d_texture_address_mode_11 );
extern void D3DWrapperConvertEnum( D3D11_TEXTURE_ADDRESS_MODE *d3d_texture_address_mode_11, D3D_TEXTURE_ADDRESS_MODE d3d_texture_address_mode );

extern void D3DFlagsConvertFrom11_FORMAT_SUPPORT( UINT *format_support, UINT format_support_11 );
extern void D3DFlagsConvertTo11_FORMAT_SUPPORT( UINT *format_support_11, UINT format_support );

extern void D3DFlagsConvertFrom11_ASYNC_GETDATA_FLAG( UINT *async_getdata_flag, UINT async_getdata_flag_11 );
extern void D3DFlagsConvertTo11_ASYNC_GETDATA_FLAG( UINT *async_getdata_flag_11, UINT async_getdata_flag );

extern void D3DWrapperConvertEnum( D3D_QUERY *d3d_query, D3D11_QUERY d3d_query_11 );
extern void D3DWrapperConvertEnum( D3D11_QUERY *d3d_query_11, D3D_QUERY d3d_query );

extern void D3DFlagsConvertFrom11_QUERY_MISC_FLAG( UINT *query_misc_flag, UINT query_misc_flag_11 );
extern void D3DFlagsConvertTo11_QUERY_MISC_FLAG( UINT *query_misc_flag_11, UINT query_misc_flag );

extern void D3DWrapperConvertEnum( D3D_COUNTER *d3d_counter, D3D11_COUNTER d3d_counter_11 );
extern void D3DWrapperConvertEnum( D3D11_COUNTER *d3d_counter_11, D3D_COUNTER d3d_counter );

extern void D3DWrapperConvertEnum( D3D_COUNTER_TYPE *d3d_counter_type, D3D11_COUNTER_TYPE d3d_counter_type_11 );
extern void D3DWrapperConvertEnum( D3D11_COUNTER_TYPE *d3d_counter_type_11, D3D_COUNTER_TYPE d3d_counter_type );

extern void D3DWrapperConvertEnum( D3D_STANDARD_MULTISAMPLE_QUALITY_LEVELS *d3d_standard_multisample_quality_levels, D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS d3d_standard_multisample_quality_levels_11 );
extern void D3DWrapperConvertEnum( D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS *d3d_standard_multisample_quality_levels_11, D3D_STANDARD_MULTISAMPLE_QUALITY_LEVELS d3d_standard_multisample_quality_levels );

extern void D3DFlagsConvertFrom11_CREATE_DEVICE_FLAG( UINT *create_device_flag, UINT create_device_flag_11 );
extern void D3DFlagsConvertTo11_CREATE_DEVICE_FLAG( UINT *create_device_flag_11, UINT create_device_flag );

extern void D3DWrapperConvertEnum( D3D_MESSAGE_CATEGORY *d3d_message_category, D3D11_MESSAGE_CATEGORY d3d_message_category_11 );
extern void D3DWrapperConvertEnum( D3D11_MESSAGE_CATEGORY *d3d_message_category_11, D3D_MESSAGE_CATEGORY d3d_message_category );

extern void D3DWrapperConvertEnum( D3D_MESSAGE_SEVERITY *d3d_message_severity, D3D11_MESSAGE_SEVERITY d3d_message_severity_11 );
extern void D3DWrapperConvertEnum( D3D11_MESSAGE_SEVERITY *d3d_message_severity_11, D3D_MESSAGE_SEVERITY d3d_message_severity );

extern void D3DWrapperConvertEnum( D3D_DRIVER_TYPE *d3d_driver_type, D3D_DRIVER_TYPE d3d_driver_type_11 );

extern void D3DWrapperConvertEnum( D3D_FEATURE_LEVEL *d3d_feature_level, D3D_FEATURE_LEVEL d3d_feature_level_11 );

extern void D3DWrapperConvertEnum( D3D_MESSAGE_ID *d3d_message_id, D3D11_MESSAGE_ID d3d_message_id_11 );
extern void D3DWrapperConvertEnum( D3D11_MESSAGE_ID *d3d_message_id_11, D3D_MESSAGE_ID d3d_message_id );

//
// Struct Conversions
//

extern void D3DWrapperConvertStruct( D3D_INPUT_ELEMENT_DESC *d3d_input_element_desc, D3D11_INPUT_ELEMENT_DESC const *d3d_input_element_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_INPUT_ELEMENT_DESC *d3d_input_element_desc_11, D3D_INPUT_ELEMENT_DESC const *d3d_input_element_desc );

extern void D3DWrapperConvertStruct( D3D_SO_DECLARATION_ENTRY *d3d_so_declaration_entry, D3D11_SO_DECLARATION_ENTRY const *d3d_so_declaration_entry_11 );
extern void D3DWrapperConvertStruct( D3D11_SO_DECLARATION_ENTRY *d3d_so_declaration_entry_11, D3D_SO_DECLARATION_ENTRY const *d3d_so_declaration_entry );

extern void D3DWrapperConvertStruct( D3D_VIEWPORT *d3d_viewport, D3D11_VIEWPORT const *d3d_viewport_11 );
extern void D3DWrapperConvertStruct( D3D11_VIEWPORT *d3d_viewport_11, D3D_VIEWPORT const *d3d_viewport );

extern void D3DWrapperConvertStruct( D3D_BOX *d3d_box, D3D11_BOX const *d3d_box_11 );
extern void D3DWrapperConvertStruct( D3D11_BOX *d3d_box_11, D3D_BOX const *d3d_box );

extern void D3DWrapperConvertStruct( D3D_DEPTH_STENCILOP_DESC *d3d_depth_stencilop_desc, D3D11_DEPTH_STENCILOP_DESC const *d3d_depth_stencilop_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_DEPTH_STENCILOP_DESC *d3d_depth_stencilop_desc_11, D3D_DEPTH_STENCILOP_DESC const *d3d_depth_stencilop_desc );

extern void D3DWrapperConvertStruct( D3D_DEPTH_STENCIL_DESC *d3d_depth_stencil_desc, D3D11_DEPTH_STENCIL_DESC const *d3d_depth_stencil_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_DEPTH_STENCIL_DESC *d3d_depth_stencil_desc_11, D3D_DEPTH_STENCIL_DESC const *d3d_depth_stencil_desc );

extern void D3DWrapperConvertStruct( D3D_BLEND_DESC *d3d_blend_desc, D3D11_BLEND_DESC const *d3d_blend_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_BLEND_DESC *d3d_blend_desc_11, D3D_BLEND_DESC const *d3d_blend_desc );

extern void D3DWrapperConvertStruct( D3D_RASTERIZER_DESC *d3d_rasterizer_desc, D3D11_RASTERIZER_DESC const *d3d_rasterizer_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_RASTERIZER_DESC *d3d_rasterizer_desc_11, D3D_RASTERIZER_DESC const *d3d_rasterizer_desc );

extern void D3DWrapperConvertStruct( D3D_SUBRESOURCE_DATA *d3d_subresource_data, D3D11_SUBRESOURCE_DATA const *d3d_subresource_data_11 );
extern void D3DWrapperConvertStruct( D3D11_SUBRESOURCE_DATA *d3d_subresource_data_11, D3D_SUBRESOURCE_DATA const *d3d_subresource_data );

extern void D3DWrapperConvertStruct( D3D_BUFFER_DESC *d3d_buffer_desc, D3D11_BUFFER_DESC const *d3d_buffer_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_BUFFER_DESC *d3d_buffer_desc_11, D3D_BUFFER_DESC const *d3d_buffer_desc );

extern void D3DWrapperConvertStruct( D3D_TEXTURE1D_DESC *d3d_texture1d_desc, D3D11_TEXTURE1D_DESC const *d3d_texture1d_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_TEXTURE1D_DESC *d3d_texture1d_desc_11, D3D_TEXTURE1D_DESC const *d3d_texture1d_desc );

extern void D3DWrapperConvertStruct( D3D_TEXTURE2D_DESC *d3d_texture2d_desc, D3D11_TEXTURE2D_DESC const *d3d_texture2d_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_TEXTURE2D_DESC *d3d_texture2d_desc_11, D3D_TEXTURE2D_DESC const *d3d_texture2d_desc );

extern void D3DWrapperConvertStruct( D3D_TEXTURE3D_DESC *d3d_texture3d_desc, D3D11_TEXTURE3D_DESC const *d3d_texture3d_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_TEXTURE3D_DESC *d3d_texture3d_desc_11, D3D_TEXTURE3D_DESC const *d3d_texture3d_desc );

extern void D3DWrapperConvertStruct( D3D_BUFFER_SRV *d3d_buffer_srv, D3D11_BUFFER_SRV const *d3d_buffer_srv_11 );
extern void D3DWrapperConvertStruct( D3D11_BUFFER_SRV *d3d_buffer_srv_11, D3D_BUFFER_SRV const *d3d_buffer_srv );

extern void D3DWrapperConvertStruct( D3D_TEX1D_SRV *d3d_tex1d_srv, D3D11_TEX1D_SRV const *d3d_tex1d_srv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX1D_SRV *d3d_tex1d_srv_11, D3D_TEX1D_SRV const *d3d_tex1d_srv );

extern void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_SRV *d3d_tex1d_array_srv, D3D11_TEX1D_ARRAY_SRV const *d3d_tex1d_array_srv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX1D_ARRAY_SRV *d3d_tex1d_array_srv_11, D3D_TEX1D_ARRAY_SRV const *d3d_tex1d_array_srv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_SRV *d3d_tex2d_srv, D3D11_TEX2D_SRV const *d3d_tex2d_srv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2D_SRV *d3d_tex2d_srv_11, D3D_TEX2D_SRV const *d3d_tex2d_srv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_SRV *d3d_tex2d_array_srv, D3D11_TEX2D_ARRAY_SRV const *d3d_tex2d_array_srv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2D_ARRAY_SRV *d3d_tex2d_array_srv_11, D3D_TEX2D_ARRAY_SRV const *d3d_tex2d_array_srv );

extern void D3DWrapperConvertStruct( D3D_TEX3D_SRV *d3d_tex3d_srv, D3D11_TEX3D_SRV const *d3d_tex3d_srv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX3D_SRV *d3d_tex3d_srv_11, D3D_TEX3D_SRV const *d3d_tex3d_srv );

extern void D3DWrapperConvertStruct( D3D_TEXCUBE_SRV *d3d_texcube_srv, D3D11_TEXCUBE_SRV const *d3d_texcube_srv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEXCUBE_SRV *d3d_texcube_srv_11, D3D_TEXCUBE_SRV const *d3d_texcube_srv );

extern void D3DWrapperConvertStruct( D3D_TEXCUBE_ARRAY_SRV1 *d3d_texcube_array_srv1, D3D11_TEXCUBE_ARRAY_SRV const *d3d_texcube_array_srv1_11 );
extern void D3DWrapperConvertStruct( D3D11_TEXCUBE_ARRAY_SRV *d3d_texcube_array_srv1_11, D3D_TEXCUBE_ARRAY_SRV1 const *d3d_texcube_array_srv1 );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_SRV *d3d_tex2dms_srv, D3D11_TEX2DMS_SRV const *d3d_tex2dms_srv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2DMS_SRV *d3d_tex2dms_srv_11, D3D_TEX2DMS_SRV const *d3d_tex2dms_srv );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_SRV *d3d_tex2dms_array_srv, D3D11_TEX2DMS_ARRAY_SRV const *d3d_tex2dms_array_srv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2DMS_ARRAY_SRV *d3d_tex2dms_array_srv_11, D3D_TEX2DMS_ARRAY_SRV const *d3d_tex2dms_array_srv );

extern void D3DWrapperConvertStruct( D3D_SHADER_RESOURCE_VIEW_DESC *d3d_shader_resource_view_desc, D3D11_SHADER_RESOURCE_VIEW_DESC const *d3d_shader_resource_view_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_SHADER_RESOURCE_VIEW_DESC *d3d_shader_resource_view_desc_11, D3D_SHADER_RESOURCE_VIEW_DESC const *d3d_shader_resource_view_desc );

extern void D3DWrapperConvertStruct( D3D_BUFFER_RTV *d3d_buffer_rtv, D3D11_BUFFER_RTV const *d3d_buffer_rtv_11 );
extern void D3DWrapperConvertStruct( D3D11_BUFFER_RTV *d3d_buffer_rtv_11, D3D_BUFFER_RTV const *d3d_buffer_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX1D_RTV *d3d_tex1d_rtv, D3D11_TEX1D_RTV const *d3d_tex1d_rtv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX1D_RTV *d3d_tex1d_rtv_11, D3D_TEX1D_RTV const *d3d_tex1d_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_RTV *d3d_tex1d_array_rtv, D3D11_TEX1D_ARRAY_RTV const *d3d_tex1d_array_rtv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX1D_ARRAY_RTV *d3d_tex1d_array_rtv_11, D3D_TEX1D_ARRAY_RTV const *d3d_tex1d_array_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_RTV *d3d_tex2d_rtv, D3D11_TEX2D_RTV const *d3d_tex2d_rtv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2D_RTV *d3d_tex2d_rtv_11, D3D_TEX2D_RTV const *d3d_tex2d_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_RTV *d3d_tex2dms_rtv, D3D11_TEX2DMS_RTV const *d3d_tex2dms_rtv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2DMS_RTV *d3d_tex2dms_rtv_11, D3D_TEX2DMS_RTV const *d3d_tex2dms_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_RTV *d3d_tex2d_array_rtv, D3D11_TEX2D_ARRAY_RTV const *d3d_tex2d_array_rtv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2D_ARRAY_RTV *d3d_tex2d_array_rtv_11, D3D_TEX2D_ARRAY_RTV const *d3d_tex2d_array_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_RTV *d3d_tex2dms_array_rtv, D3D11_TEX2DMS_ARRAY_RTV const *d3d_tex2dms_array_rtv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2DMS_ARRAY_RTV *d3d_tex2dms_array_rtv_11, D3D_TEX2DMS_ARRAY_RTV const *d3d_tex2dms_array_rtv );

extern void D3DWrapperConvertStruct( D3D_TEX3D_RTV *d3d_tex3d_rtv, D3D11_TEX3D_RTV const *d3d_tex3d_rtv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX3D_RTV *d3d_tex3d_rtv_11, D3D_TEX3D_RTV const *d3d_tex3d_rtv );

extern void D3DWrapperConvertStruct( D3D_RENDER_TARGET_VIEW_DESC *d3d_render_target_view_desc, D3D11_RENDER_TARGET_VIEW_DESC const *d3d_render_target_view_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_RENDER_TARGET_VIEW_DESC *d3d_render_target_view_desc_11, D3D_RENDER_TARGET_VIEW_DESC const *d3d_render_target_view_desc );

extern void D3DWrapperConvertStruct( D3D_TEX1D_DSV *d3d_tex1d_dsv, D3D11_TEX1D_DSV const *d3d_tex1d_dsv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX1D_DSV *d3d_tex1d_dsv_11, D3D_TEX1D_DSV const *d3d_tex1d_dsv );

extern void D3DWrapperConvertStruct( D3D_TEX1D_ARRAY_DSV *d3d_tex1d_array_dsv, D3D11_TEX1D_ARRAY_DSV const *d3d_tex1d_array_dsv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX1D_ARRAY_DSV *d3d_tex1d_array_dsv_11, D3D_TEX1D_ARRAY_DSV const *d3d_tex1d_array_dsv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_DSV *d3d_tex2d_dsv, D3D11_TEX2D_DSV const *d3d_tex2d_dsv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2D_DSV *d3d_tex2d_dsv_11, D3D_TEX2D_DSV const *d3d_tex2d_dsv );

extern void D3DWrapperConvertStruct( D3D_TEX2D_ARRAY_DSV *d3d_tex2d_array_dsv, D3D11_TEX2D_ARRAY_DSV const *d3d_tex2d_array_dsv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2D_ARRAY_DSV *d3d_tex2d_array_dsv_11, D3D_TEX2D_ARRAY_DSV const *d3d_tex2d_array_dsv );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_DSV *d3d_tex2dms_dsv, D3D11_TEX2DMS_DSV const *d3d_tex2dms_dsv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2DMS_DSV *d3d_tex2dms_dsv_11, D3D_TEX2DMS_DSV const *d3d_tex2dms_dsv );

extern void D3DWrapperConvertStruct( D3D_TEX2DMS_ARRAY_DSV *d3d_tex2dms_array_dsv, D3D11_TEX2DMS_ARRAY_DSV const *d3d_tex2dms_array_dsv_11 );
extern void D3DWrapperConvertStruct( D3D11_TEX2DMS_ARRAY_DSV *d3d_tex2dms_array_dsv_11, D3D_TEX2DMS_ARRAY_DSV const *d3d_tex2dms_array_dsv );

extern void D3DWrapperConvertStruct( D3D_DEPTH_STENCIL_VIEW_DESC *d3d_depth_stencil_view_desc, D3D11_DEPTH_STENCIL_VIEW_DESC const *d3d_depth_stencil_view_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_DEPTH_STENCIL_VIEW_DESC *d3d_depth_stencil_view_desc_11, D3D_DEPTH_STENCIL_VIEW_DESC const *d3d_depth_stencil_view_desc );

extern void D3DWrapperConvertStruct( D3D_SAMPLER_DESC *d3d_sampler_desc, D3D11_SAMPLER_DESC const *d3d_sampler_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_SAMPLER_DESC *d3d_sampler_desc_11, D3D_SAMPLER_DESC const *d3d_sampler_desc );

extern void D3DWrapperConvertStruct( D3D_QUERY_DESC *d3d_query_desc, D3D11_QUERY_DESC const *d3d_query_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_QUERY_DESC *d3d_query_desc_11, D3D_QUERY_DESC const *d3d_query_desc );

extern void D3DWrapperConvertStruct( D3D_QUERY_DATA_TIMESTAMP_DISJOINT *d3d_query_data_timestamp_disjoint, D3D11_QUERY_DATA_TIMESTAMP_DISJOINT const *d3d_query_data_timestamp_disjoint_11 );
extern void D3DWrapperConvertStruct( D3D11_QUERY_DATA_TIMESTAMP_DISJOINT *d3d_query_data_timestamp_disjoint_11, D3D_QUERY_DATA_TIMESTAMP_DISJOINT const *d3d_query_data_timestamp_disjoint );

extern void D3DWrapperConvertStruct( D3D_QUERY_DATA_PIPELINE_STATISTICS *d3d_query_data_pipeline_statistics, D3D11_QUERY_DATA_PIPELINE_STATISTICS const *d3d_query_data_pipeline_statistics_11 );
extern void D3DWrapperConvertStruct( D3D11_QUERY_DATA_PIPELINE_STATISTICS *d3d_query_data_pipeline_statistics_11, D3D_QUERY_DATA_PIPELINE_STATISTICS const *d3d_query_data_pipeline_statistics );

extern void D3DWrapperConvertStruct( D3D_QUERY_DATA_SO_STATISTICS *d3d_query_data_so_statistics, D3D11_QUERY_DATA_SO_STATISTICS const *d3d_query_data_so_statistics_11 );
extern void D3DWrapperConvertStruct( D3D11_QUERY_DATA_SO_STATISTICS *d3d_query_data_so_statistics_11, D3D_QUERY_DATA_SO_STATISTICS const *d3d_query_data_so_statistics );

extern void D3DWrapperConvertStruct( D3D_COUNTER_DESC *d3d_counter_desc, D3D11_COUNTER_DESC const *d3d_counter_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_COUNTER_DESC *d3d_counter_desc_11, D3D_COUNTER_DESC const *d3d_counter_desc );

extern void D3DWrapperConvertStruct( D3D_COUNTER_INFO *d3d_counter_info, D3D11_COUNTER_INFO const *d3d_counter_info_11 );
extern void D3DWrapperConvertStruct( D3D11_COUNTER_INFO *d3d_counter_info_11, D3D_COUNTER_INFO const *d3d_counter_info );

extern void D3DWrapperConvertStruct( D3D_MESSAGE *d3d_message, D3D11_MESSAGE const *d3d_message_11 );
extern void D3DWrapperConvertStruct( D3D11_MESSAGE *d3d_message_11, D3D_MESSAGE const *d3d_message );

extern void D3DWrapperConvertStruct( D3D_INFO_QUEUE_FILTER_DESC *d3d_info_queue_filter_desc, D3D11_INFO_QUEUE_FILTER_DESC const *d3d_info_queue_filter_desc_11 );
extern void D3DWrapperConvertStruct( D3D11_INFO_QUEUE_FILTER_DESC *d3d_info_queue_filter_desc_11, D3D_INFO_QUEUE_FILTER_DESC const *d3d_info_queue_filter_desc );

extern void D3DWrapperConvertStruct( D3D_INFO_QUEUE_FILTER *d3d_info_queue_filter, D3D11_INFO_QUEUE_FILTER const *d3d_info_queue_filter_11 );
extern void D3DWrapperConvertStruct( D3D11_INFO_QUEUE_FILTER *d3d_info_queue_filter_11, D3D_INFO_QUEUE_FILTER const *d3d_info_queue_filter );

extern void D3DWrapperConvertStruct( D3D_MAPPED_TEXTURE2D *d3d_mapped_texture2d, D3D11_MAPPED_TEXTURE2D const *d3d_mapped_texture2d_11 );
extern void D3DWrapperConvertStruct( D3D11_MAPPED_TEXTURE2D *d3d_mapped_texture2d_11, D3D_MAPPED_TEXTURE2D const *d3d_mapped_texture2d );

extern void D3DWrapperConvertStruct( D3D_MAPPED_TEXTURE3D *d3d_mapped_texture3d, D3D11_MAPPED_TEXTURE3D const *d3d_mapped_texture3d_11 );
extern void D3DWrapperConvertStruct( D3D11_MAPPED_TEXTURE3D *d3d_mapped_texture3d_11, D3D_MAPPED_TEXTURE3D const *d3d_mapped_texture3d );


//
// Interface Conversions
//

extern void D3DWrapperConvertInterface( CDeviceChild **ppDeviceChild, ID3D11DeviceChild *pDeviceChild_11 );
extern void D3DWrapperConvertInterface( ID3D11DeviceChild **ppDeviceChild_11, CDeviceChild *pDeviceChild );

extern void D3DWrapperConvertInterface( CDepthStencilState **ppDepthStencilState, ID3D11DepthStencilState *pDepthStencilState_11 );
extern void D3DWrapperConvertInterface( ID3D11DepthStencilState **ppDepthStencilState_11, CDepthStencilState *pDepthStencilState );

extern void D3DWrapperConvertInterface( CBlendState **ppBlendState, ID3D11BlendState *pBlendState_11 );
extern void D3DWrapperConvertInterface( ID3D11BlendState **ppBlendState_11, CBlendState *pBlendState );

extern void D3DWrapperConvertInterface( CRasterizerState **ppRasterizerState, ID3D11RasterizerState *pRasterizerState_11 );
extern void D3DWrapperConvertInterface( ID3D11RasterizerState **ppRasterizerState_11, CRasterizerState *pRasterizerState );

extern void D3DWrapperConvertInterface( CResource **ppResource, ID3D11Resource *pResource_11 );
extern void D3DWrapperConvertInterface( ID3D11Resource **ppResource_11, CResource *pResource );

extern void D3DWrapperConvertInterface( CBuffer **ppBuffer, ID3D11Buffer *pBuffer_11 );
extern void D3DWrapperConvertInterface( ID3D11Buffer **ppBuffer_11, CBuffer *pBuffer );

extern void D3DWrapperConvertInterface( CTexture1D **ppTexture1D, ID3D11Texture1D *pTexture1D_11 );
extern void D3DWrapperConvertInterface( ID3D11Texture1D **ppTexture1D_11, CTexture1D *pTexture1D );

extern void D3DWrapperConvertInterface( CTexture2D **ppTexture2D, ID3D11Texture2D *pTexture2D_11 );
extern void D3DWrapperConvertInterface( ID3D11Texture2D **ppTexture2D_11, CTexture2D *pTexture2D );

extern void D3DWrapperConvertInterface( CTexture3D **ppTexture3D, ID3D11Texture3D *pTexture3D_11 );
extern void D3DWrapperConvertInterface( ID3D11Texture3D **ppTexture3D_11, CTexture3D *pTexture3D );

extern void D3DWrapperConvertInterface( CView **ppView, ID3D11View *pView_11 );
extern void D3DWrapperConvertInterface( ID3D11View **ppView_11, CView *pView );

extern void D3DWrapperConvertInterface( CShaderResourceView **ppShaderResourceView, ID3D11ShaderResourceView *pShaderResourceView_11 );
extern void D3DWrapperConvertInterface( ID3D11ShaderResourceView **ppShaderResourceView_11, CShaderResourceView *pShaderResourceView );

extern void D3DWrapperConvertInterface( CRenderTargetView **ppRenderTargetView, ID3D11RenderTargetView *pRenderTargetView_11 );
extern void D3DWrapperConvertInterface( ID3D11RenderTargetView **ppRenderTargetView_11, CRenderTargetView *pRenderTargetView );

extern void D3DWrapperConvertInterface( CDepthStencilView **ppDepthStencilView, ID3D11DepthStencilView *pDepthStencilView_11 );
extern void D3DWrapperConvertInterface( ID3D11DepthStencilView **ppDepthStencilView_11, CDepthStencilView *pDepthStencilView );

extern void D3DWrapperConvertInterface( CVertexShader **ppVertexShader, ID3D11VertexShader *pVertexShader_11 );
extern void D3DWrapperConvertInterface( ID3D11VertexShader **ppVertexShader_11, CVertexShader *pVertexShader );

extern void D3DWrapperConvertInterface( CGeometryShader **ppGeometryShader, ID3D11GeometryShader *pGeometryShader_11 );
extern void D3DWrapperConvertInterface( ID3D11GeometryShader **ppGeometryShader_11, CGeometryShader *pGeometryShader );

extern void D3DWrapperConvertInterface( CPixelShader **ppPixelShader, ID3D11PixelShader *pPixelShader_11 );
extern void D3DWrapperConvertInterface( ID3D11PixelShader **ppPixelShader_11, CPixelShader *pPixelShader );

extern void D3DWrapperConvertInterface( CInputLayout **ppInputLayout, ID3D11InputLayout *pInputLayout_11 );
extern void D3DWrapperConvertInterface( ID3D11InputLayout **ppInputLayout_11, CInputLayout *pInputLayout );

extern void D3DWrapperConvertInterface( CSamplerState **ppSamplerState, ID3D11SamplerState *pSamplerState_11 );
extern void D3DWrapperConvertInterface( ID3D11SamplerState **ppSamplerState_11, CSamplerState *pSamplerState );

extern void D3DWrapperConvertInterface( CAsynchronous **ppAsynchronous, ID3D11Asynchronous *pAsynchronous_11 );
extern void D3DWrapperConvertInterface( ID3D11Asynchronous **ppAsynchronous_11, CAsynchronous *pAsynchronous );

extern void D3DWrapperConvertInterface( CQuery **ppQuery, ID3D11Query *pQuery_11 );
extern void D3DWrapperConvertInterface( ID3D11Query **ppQuery_11, CQuery *pQuery );

extern void D3DWrapperConvertInterface( CPredicate **ppPredicate, ID3D11Predicate *pPredicate_11 );
extern void D3DWrapperConvertInterface( ID3D11Predicate **ppPredicate_11, CPredicate *pPredicate );

extern void D3DWrapperConvertInterface( CCounter **ppCounter, ID3D11Counter *pCounter_11 );
extern void D3DWrapperConvertInterface( ID3D11Counter **ppCounter_11, CCounter *pCounter );

extern void D3DWrapperConvertInterface( CDevice **ppDevice, ID3D11Device *pDevice_11 );
extern void D3DWrapperConvertInterface( ID3D11Device **ppDevice_11, CDevice *pDevice );

extern void D3DWrapperConvertInterface( CDebug **ppDebug, ID3D11Debug *pDebug_11 );
extern void D3DWrapperConvertInterface( ID3D11Debug **ppDebug_11, CDebug *pDebug );

extern void D3DWrapperConvertInterface( CSwitchToRef **ppSwitchToRef, ID3D11SwitchToRef *pSwitchToRef_11 );
extern void D3DWrapperConvertInterface( ID3D11SwitchToRef **ppSwitchToRef_11, CSwitchToRef *pSwitchToRef );

extern void D3DWrapperConvertInterface( CInfoQueue **ppInfoQueue, ID3D11InfoQueue *pInfoQueue_11 );
extern void D3DWrapperConvertInterface( ID3D11InfoQueue **ppInfoQueue_11, CInfoQueue *pInfoQueue );
