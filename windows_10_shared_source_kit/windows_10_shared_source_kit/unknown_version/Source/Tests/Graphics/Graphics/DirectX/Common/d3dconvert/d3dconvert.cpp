#include "d3dconvert.h"

using namespace D3DConvert;
namespace D3DConvert {
HRESULT Convert( D3D10_INPUT_CLASSIFICATION &out, D3D11_INPUT_CLASSIFICATION in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_INPUT_PER_VERTEX_DATA:
		out = D3D10_INPUT_PER_VERTEX_DATA;
		break;
	case D3D11_INPUT_PER_INSTANCE_DATA:
		out = D3D10_INPUT_PER_INSTANCE_DATA;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_INPUT_CLASSIFICATION &out, D3D10_INPUT_CLASSIFICATION in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_INPUT_PER_VERTEX_DATA:
		out = D3D11_INPUT_PER_VERTEX_DATA;
		break;
	case D3D10_INPUT_PER_INSTANCE_DATA:
		out = D3D11_INPUT_PER_INSTANCE_DATA;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_INPUT_ELEMENT_DESC &out, const D3D11_INPUT_ELEMENT_DESC &in )
{
	HRESULT result = S_OK;

	out.SemanticName = in.SemanticName;
	out.SemanticIndex = in.SemanticIndex;
	out.Format = in.Format;
	out.InputSlot = in.InputSlot;
	out.AlignedByteOffset = in.AlignedByteOffset;
	if( FAILED( Convert( out.InputSlotClass, in.InputSlotClass ) ) )
		result = E_FAIL;
	out.InstanceDataStepRate = in.InstanceDataStepRate;

	return result;
}

HRESULT Convert( D3D11_INPUT_ELEMENT_DESC &out, const D3D10_INPUT_ELEMENT_DESC &in )
{
	HRESULT result = S_OK;

	out.SemanticName = in.SemanticName;
	out.SemanticIndex = in.SemanticIndex;
	out.Format = in.Format;
	out.InputSlot = in.InputSlot;
	out.AlignedByteOffset = in.AlignedByteOffset;
	if( FAILED( Convert( out.InputSlotClass, in.InputSlotClass ) ) )
		result = E_FAIL;
	out.InstanceDataStepRate = in.InstanceDataStepRate;

	return result;
}

HRESULT Convert( D3D10_FILL_MODE &out, D3D11_FILL_MODE in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_FILL_WIREFRAME:
		out = D3D10_FILL_WIREFRAME;
		break;
	case D3D11_FILL_SOLID:
		out = D3D10_FILL_SOLID;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_FILL_MODE &out, D3D10_FILL_MODE in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_FILL_WIREFRAME:
		out = D3D11_FILL_WIREFRAME;
		break;
	case D3D10_FILL_SOLID:
		out = D3D11_FILL_SOLID;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_CULL_MODE &out, D3D11_CULL_MODE in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_CULL_NONE:
		out = D3D10_CULL_NONE;
		break;
	case D3D11_CULL_FRONT:
		out = D3D10_CULL_FRONT;
		break;
	case D3D11_CULL_BACK:
		out = D3D10_CULL_BACK;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_CULL_MODE &out, D3D10_CULL_MODE in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_CULL_NONE:
		out = D3D11_CULL_NONE;
		break;
	case D3D10_CULL_FRONT:
		out = D3D11_CULL_FRONT;
		break;
	case D3D10_CULL_BACK:
		out = D3D11_CULL_BACK;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_SO_DECLARATION_ENTRY &out, const D3D11_SO_DECLARATION_ENTRY &in )
{
	HRESULT result = S_OK;

	if( in.Stream != 0 )
		result = E_FAIL;

	out.SemanticName = in.SemanticName;
	out.SemanticIndex = in.SemanticIndex;
	out.StartComponent = in.StartComponent;
	out.ComponentCount = in.ComponentCount;
	out.OutputSlot = in.OutputSlot;

	return result;
}

HRESULT Convert( D3D11_SO_DECLARATION_ENTRY &out, const D3D10_SO_DECLARATION_ENTRY &in )
{
	HRESULT result = S_OK;

	out.Stream = 0; // Set Stream index to zero
	out.SemanticName = in.SemanticName;
	out.SemanticIndex = in.SemanticIndex;
	out.StartComponent = in.StartComponent;
	out.ComponentCount = in.ComponentCount;
	out.OutputSlot = in.OutputSlot;

	return result;
}

HRESULT Convert( D3D10_VIEWPORT &out, const D3D11_VIEWPORT &in )
{
	HRESULT result = S_OK;

	out.TopLeftX = (INT) in.TopLeftX;
	out.TopLeftY = (INT) in.TopLeftY;
	out.Width = (UINT) in.Width;
	out.Height = (UINT) in.Height;
	out.MinDepth = in.MinDepth;
	out.MaxDepth = in.MaxDepth;

	return result;
}

HRESULT Convert( D3D11_VIEWPORT &out, const D3D10_VIEWPORT &in )
{
	HRESULT result = S_OK;

	out.TopLeftX = (float) in.TopLeftX;
	out.TopLeftY = (float) in.TopLeftY;
	out.Width = (float) in.Width;
	out.Height = (float) in.Height;
	out.MinDepth = in.MinDepth;
	out.MaxDepth = in.MaxDepth;

	return result;
}

HRESULT Convert( D3D10_RESOURCE_DIMENSION &out, D3D11_RESOURCE_DIMENSION in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_RESOURCE_DIMENSION_UNKNOWN:
		out = D3D10_RESOURCE_DIMENSION_UNKNOWN;
		break;
	case D3D11_RESOURCE_DIMENSION_BUFFER:
		out = D3D10_RESOURCE_DIMENSION_BUFFER;
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D	:
		out = D3D10_RESOURCE_DIMENSION_TEXTURE1D;
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		out = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		out = D3D10_RESOURCE_DIMENSION_TEXTURE3D;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_RESOURCE_DIMENSION &out, D3D10_RESOURCE_DIMENSION in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_RESOURCE_DIMENSION_UNKNOWN:
		out = D3D11_RESOURCE_DIMENSION_UNKNOWN;
		break;
	case D3D10_RESOURCE_DIMENSION_BUFFER:
		out = D3D11_RESOURCE_DIMENSION_BUFFER;
		break;
	case D3D10_RESOURCE_DIMENSION_TEXTURE1D	:
		out = D3D11_RESOURCE_DIMENSION_TEXTURE1D;
		break;
	case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
		out = D3D11_RESOURCE_DIMENSION_TEXTURE2D;
		break;
	case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
		out = D3D11_RESOURCE_DIMENSION_TEXTURE3D;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_DSV_DIMENSION &out, D3D11_DSV_DIMENSION in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_DSV_DIMENSION_UNKNOWN:
		out = D3D10_DSV_DIMENSION_UNKNOWN;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE1D:
		out = D3D10_DSV_DIMENSION_TEXTURE1D;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
		out = D3D10_DSV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2D:
		out = D3D10_DSV_DIMENSION_TEXTURE2D;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
		out = D3D10_DSV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2DMS:
		out = D3D10_DSV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
		out = D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_DSV_DIMENSION &out, D3D10_DSV_DIMENSION in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_DSV_DIMENSION_UNKNOWN:
		out = D3D11_DSV_DIMENSION_UNKNOWN;
		break;
	case D3D10_DSV_DIMENSION_TEXTURE1D:
		out = D3D11_DSV_DIMENSION_TEXTURE1D;
		break;
	case D3D10_DSV_DIMENSION_TEXTURE1DARRAY:
		out = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D10_DSV_DIMENSION_TEXTURE2D:
		out = D3D11_DSV_DIMENSION_TEXTURE2D;
		break;
	case D3D10_DSV_DIMENSION_TEXTURE2DARRAY:
		out = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D10_DSV_DIMENSION_TEXTURE2DMS:
		out = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY:
		out = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_RTV_DIMENSION &out, D3D11_RTV_DIMENSION in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_RTV_DIMENSION_UNKNOWN:
		out = D3D10_RTV_DIMENSION_UNKNOWN;
		break;
	case D3D11_RTV_DIMENSION_BUFFER:
		out = D3D10_RTV_DIMENSION_BUFFER;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE1D:
		out = D3D10_RTV_DIMENSION_TEXTURE1D;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
		out = D3D10_RTV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE2D:
		out = D3D10_RTV_DIMENSION_TEXTURE2D;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
		out = D3D10_RTV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE2DMS:
		out = D3D10_RTV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
		out = D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE3D:
		out = D3D10_RTV_DIMENSION_TEXTURE3D;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_RTV_DIMENSION &out, D3D10_RTV_DIMENSION in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_RTV_DIMENSION_UNKNOWN:
		out = D3D11_RTV_DIMENSION_UNKNOWN;
		break;
	case D3D10_RTV_DIMENSION_BUFFER:
		out = D3D11_RTV_DIMENSION_BUFFER;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE1D:
		out = D3D11_RTV_DIMENSION_TEXTURE1D;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE1DARRAY:
		out = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE2D:
		out = D3D11_RTV_DIMENSION_TEXTURE2D;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE2DARRAY:
		out = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE2DMS:
		out = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY:
		out = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE3D:
		out = D3D11_RTV_DIMENSION_TEXTURE3D;
		break;
	}

	return result;
}



HRESULT Convert( D3D10_USAGE &out, D3D11_USAGE in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_USAGE_DEFAULT:
		out = D3D10_USAGE_DEFAULT;
		break;
	case D3D11_USAGE_IMMUTABLE:
		out = D3D10_USAGE_IMMUTABLE;
		break;
	case D3D11_USAGE_DYNAMIC:
		out = D3D10_USAGE_DYNAMIC;
		break;
	case D3D11_USAGE_STAGING:
		out = D3D10_USAGE_STAGING;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_USAGE &out, D3D10_USAGE in )
{
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_USAGE_DEFAULT:
		out = D3D11_USAGE_DEFAULT;
		break;
	case D3D10_USAGE_IMMUTABLE:
		out = D3D11_USAGE_IMMUTABLE;
		break;
	case D3D10_USAGE_DYNAMIC:
		out = D3D11_USAGE_DYNAMIC;
		break;
	case D3D10_USAGE_STAGING:
		out = D3D11_USAGE_STAGING;
		break;
	}

	return result;
}

HRESULT ConvertBindFlag11to10( UINT &out, UINT in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D11_BIND_VERTEX_BUFFER )
		out |= D3D10_BIND_VERTEX_BUFFER;
	if( in & D3D11_BIND_INDEX_BUFFER )
		out |= D3D10_BIND_INDEX_BUFFER;
	if( in & D3D11_BIND_CONSTANT_BUFFER )
		out |= D3D10_BIND_CONSTANT_BUFFER;
	if( in & D3D11_BIND_SHADER_RESOURCE )
		out |= D3D10_BIND_SHADER_RESOURCE;
	if( in & D3D11_BIND_STREAM_OUTPUT )
		out |= D3D10_BIND_STREAM_OUTPUT;
	if( in & D3D11_BIND_RENDER_TARGET )
		out |= D3D10_BIND_RENDER_TARGET;
	if( in & D3D11_BIND_DEPTH_STENCIL )
		out |= D3D10_BIND_DEPTH_STENCIL;
	if( in & D3D11_BIND_UNORDERED_ACCESS )
		result = S_FALSE; // Ignore
	if( in & D3D11_BIND_DECODER )
		result = S_FALSE; // Ignore

	return result;
}

HRESULT ConvertBindFlag10to11( UINT &out, UINT in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D10_BIND_VERTEX_BUFFER )
		out |= D3D11_BIND_VERTEX_BUFFER;
	if( in & D3D10_BIND_INDEX_BUFFER )
		out |= D3D11_BIND_INDEX_BUFFER;
	if( in & D3D10_BIND_CONSTANT_BUFFER )
		out |= D3D11_BIND_CONSTANT_BUFFER;
	if( in & D3D10_BIND_SHADER_RESOURCE )
		out |= D3D11_BIND_SHADER_RESOURCE;
	if( in & D3D10_BIND_STREAM_OUTPUT )
		out |= D3D11_BIND_STREAM_OUTPUT;
	if( in & D3D10_BIND_RENDER_TARGET )
		out |= D3D11_BIND_RENDER_TARGET;
	if( in & D3D10_BIND_DEPTH_STENCIL )
		out |= D3D11_BIND_DEPTH_STENCIL;

	return result;
}

HRESULT ConvertCpuAccessFlag11to10( UINT &out, UINT in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D11_CPU_ACCESS_READ )
		out |= D3D10_CPU_ACCESS_READ;
	if( in & D3D11_CPU_ACCESS_WRITE )
		out |= D3D10_CPU_ACCESS_WRITE;

	return result;
}

HRESULT ConvertCpuAccessFlag10to11( UINT &out, UINT in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D10_CPU_ACCESS_READ )
		out |= D3D11_CPU_ACCESS_READ;
	if( in & D3D10_CPU_ACCESS_WRITE )
		out |= D3D11_CPU_ACCESS_WRITE;

	return result;
}

HRESULT ConvertResourceMiscFlag11to10( UINT &out, UINT in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D11_RESOURCE_MISC_GENERATE_MIPS )
		out |= D3D10_RESOURCE_MISC_GENERATE_MIPS;
	if( in & D3D11_RESOURCE_MISC_SHARED )
		out |= D3D10_RESOURCE_MISC_SHARED;
	if( in & D3D11_RESOURCE_MISC_TEXTURECUBE )
		out |= D3D10_RESOURCE_MISC_TEXTURECUBE;
	if( in & D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX )
		out |= D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX;
	if( in & D3D11_RESOURCE_MISC_GDI_COMPATIBLE )
		out |= D3D10_RESOURCE_MISC_GDI_COMPATIBLE;

	// Non convertable flags
	if( in & D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS )
		result = S_FALSE;

	// not published
	if( in & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
		result = S_FALSE;
	if( in & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
		result = E_FAIL;
	if( in & D3D11_RESOURCE_MISC_RESOURCE_CLAMP )
		result = E_FAIL;

	return result;
}

HRESULT ConvertResourceMiscFlag10to11( UINT &out, UINT in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D10_RESOURCE_MISC_GENERATE_MIPS )
		out |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	if( in & D3D10_RESOURCE_MISC_SHARED )
		out |= D3D11_RESOURCE_MISC_SHARED;
	if( in & D3D10_RESOURCE_MISC_TEXTURECUBE )
		out |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	if( in & D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX )
		out |= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
	if( in & D3D10_RESOURCE_MISC_GDI_COMPATIBLE )
		out |= D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

	return result;
}

HRESULT Convert( D3D10_MAP &out, D3D11_MAP in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_MAP_READ:
		out = D3D10_MAP_READ;
		break;
	case D3D11_MAP_WRITE:
		out = D3D10_MAP_WRITE;
		break;
	case D3D11_MAP_READ_WRITE:
		out = D3D10_MAP_READ_WRITE;
		break;
	case D3D11_MAP_WRITE_DISCARD:
		out = D3D10_MAP_WRITE_DISCARD;
		break;
	case D3D11_MAP_WRITE_NO_OVERWRITE:
		out = D3D10_MAP_WRITE_NO_OVERWRITE;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_MAP &out, D3D10_MAP in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_MAP_READ:
		out = D3D11_MAP_READ;
		break;
	case D3D10_MAP_WRITE:
		out = D3D11_MAP_WRITE;
		break;
	case D3D10_MAP_READ_WRITE:
		out = D3D11_MAP_READ_WRITE;
		break;
	case D3D10_MAP_WRITE_DISCARD:
		out = D3D11_MAP_WRITE_DISCARD;
		break;
	case D3D10_MAP_WRITE_NO_OVERWRITE:
		out = D3D11_MAP_WRITE_NO_OVERWRITE;
		break;
	}

	return result;
}

HRESULT ConvertMapFlag11to10( long &out, long in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D11_MAP_FLAG_DO_NOT_WAIT )
		out |= D3D10_MAP_FLAG_DO_NOT_WAIT;

	return result;
}

HRESULT ConvertMapFlag10to11( long &out, long in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D10_MAP_FLAG_DO_NOT_WAIT )
		out |= D3D11_MAP_FLAG_DO_NOT_WAIT;

	return result;
}

HRESULT ConvertRaiseFlag11to10( long &out, long in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D11_RAISE_FLAG_DRIVER_INTERNAL_ERROR )
		out |= D3D10_RAISE_FLAG_DRIVER_INTERNAL_ERROR;

	return result;
}

HRESULT ConvertRaiseFlag10to11( long &out, long in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D10_RAISE_FLAG_DRIVER_INTERNAL_ERROR )
		out |= D3D11_RAISE_FLAG_DRIVER_INTERNAL_ERROR;

	return result;
}

HRESULT ConvertClearFlag11to10( long &out, long in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D11_CLEAR_DEPTH )
		out |= D3D10_CLEAR_DEPTH;
	if( in & D3D11_CLEAR_STENCIL )
		out |= D3D10_CLEAR_STENCIL;

	return result;
}

HRESULT ConvertClearFlag10to11( long &out, long in )
{
	HRESULT result = S_OK;

	out = 0;

	if( in & D3D10_CLEAR_DEPTH )
		out |= D3D11_CLEAR_DEPTH;
	if( in & D3D10_CLEAR_STENCIL )
		out |= D3D11_CLEAR_STENCIL;

	return result;
}

HRESULT Convert( D3D10_RECT &out, const D3D11_RECT &in )
{
	// Both of these are typedef RECT so just copy
	out = in;
	return S_OK;
}

#if 0
// Function not needed since D3D11_RECT and D3D10_RECT are both typedef RECT.
// Leaving in for completeness
HRESULT Convert( D3D11_RECT &out, const D3D10_RECT &in )
{
	// Both of these are typedef RECT so just copy
	out = in;
	return S_OK;
}
#endif

HRESULT Convert( D3D10_BOX &out, const D3D11_BOX &in )
{
	HRESULT result = S_OK;

	out.left = in.left;
	out.right = in.right;
	out.bottom = in.bottom;
	out.top = in.top;
	out.back = in.back;
	out.front = in.front;

	return result;
}

HRESULT Convert( D3D11_BOX &out, const D3D10_BOX &in )
{
	HRESULT result = S_OK;

	out.left = in.left;
	out.right = in.right;
	out.bottom = in.bottom;
	out.top = in.top;
	out.back = in.back;
	out.front = in.front;

	return result;
}

HRESULT Convert( D3D11_COMPARISON_FUNC &out, D3D10_COMPARISON_FUNC in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_COMPARISON_NEVER:
		out = D3D11_COMPARISON_NEVER;
		break;
	case D3D10_COMPARISON_LESS:
		out = D3D11_COMPARISON_LESS;
		break;
	case D3D10_COMPARISON_EQUAL:
		out = D3D11_COMPARISON_EQUAL;
		break;
	case D3D10_COMPARISON_LESS_EQUAL:
		out = D3D11_COMPARISON_LESS_EQUAL;
		break;
	case D3D10_COMPARISON_GREATER:
		out = D3D11_COMPARISON_GREATER;
		break;
	case D3D10_COMPARISON_NOT_EQUAL:
		out = D3D11_COMPARISON_NOT_EQUAL;
		break;
	case D3D10_COMPARISON_GREATER_EQUAL:
		out = D3D11_COMPARISON_GREATER_EQUAL;
		break;
	case D3D10_COMPARISON_ALWAYS:
		out = D3D11_COMPARISON_ALWAYS;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_COMPARISON_FUNC &out, D3D11_COMPARISON_FUNC in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_COMPARISON_NEVER:
		out = D3D10_COMPARISON_NEVER;
		break;
	case D3D11_COMPARISON_LESS:
		out = D3D10_COMPARISON_LESS;
		break;
	case D3D11_COMPARISON_LESS_EQUAL:
		out = D3D10_COMPARISON_LESS_EQUAL;
		break;
	case D3D11_COMPARISON_EQUAL:
		out = D3D10_COMPARISON_EQUAL;
		break;
	case D3D11_COMPARISON_GREATER:
		out = D3D10_COMPARISON_GREATER;
		break;
	case D3D11_COMPARISON_NOT_EQUAL:
		out = D3D10_COMPARISON_NOT_EQUAL;
		break;
	case D3D11_COMPARISON_GREATER_EQUAL:
		out = D3D10_COMPARISON_GREATER_EQUAL;
		break;
	case D3D11_COMPARISON_ALWAYS:
		out = D3D10_COMPARISON_ALWAYS;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_DEPTH_WRITE_MASK &out, D3D10_DEPTH_WRITE_MASK in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_DEPTH_WRITE_MASK_ZERO:
		out = D3D11_DEPTH_WRITE_MASK_ZERO;
		break;
	case D3D10_DEPTH_WRITE_MASK_ALL:
		out = D3D11_DEPTH_WRITE_MASK_ALL;
		break;
	}
	return result;
}

HRESULT Convert( D3D10_DEPTH_WRITE_MASK &out, D3D11_DEPTH_WRITE_MASK in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_DEPTH_WRITE_MASK_ZERO:
		out = D3D10_DEPTH_WRITE_MASK_ZERO;
		break;
	case D3D11_DEPTH_WRITE_MASK_ALL:
		out = D3D10_DEPTH_WRITE_MASK_ALL;
		break;
	}
	return result;
}

HRESULT Convert( D3D11_STENCIL_OP &out, D3D10_STENCIL_OP in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_STENCIL_OP_KEEP:
		out = D3D11_STENCIL_OP_KEEP;
		break;
	case D3D10_STENCIL_OP_ZERO:
		out = D3D11_STENCIL_OP_ZERO;
		break;
	case D3D10_STENCIL_OP_REPLACE:
		out = D3D11_STENCIL_OP_REPLACE;
		break;
	case D3D10_STENCIL_OP_INCR_SAT:
		out = D3D11_STENCIL_OP_INCR_SAT;
		break;
	case D3D10_STENCIL_OP_DECR_SAT:
		out = D3D11_STENCIL_OP_DECR_SAT;
		break;
	case D3D10_STENCIL_OP_INVERT:
		out = D3D11_STENCIL_OP_INVERT;
		break;
	case D3D10_STENCIL_OP_INCR:
		out = D3D11_STENCIL_OP_INCR;
		break;
	case D3D10_STENCIL_OP_DECR:
		out = D3D11_STENCIL_OP_DECR;
		break;
	}
	return result;
}

HRESULT Convert( D3D10_STENCIL_OP &out, D3D11_STENCIL_OP in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_STENCIL_OP_KEEP:
		out = D3D10_STENCIL_OP_KEEP;
		break;
	case D3D11_STENCIL_OP_ZERO:
		out = D3D10_STENCIL_OP_ZERO;
		break;
	case D3D11_STENCIL_OP_REPLACE:
		out = D3D10_STENCIL_OP_REPLACE;
		break;
	case D3D11_STENCIL_OP_INCR_SAT:
		out = D3D10_STENCIL_OP_INCR_SAT;
		break;
	case D3D11_STENCIL_OP_DECR_SAT:
		out = D3D10_STENCIL_OP_DECR_SAT;
		break;
	case D3D11_STENCIL_OP_INVERT:
		out = D3D10_STENCIL_OP_INVERT;
		break;
	case D3D11_STENCIL_OP_INCR:
		out = D3D10_STENCIL_OP_INCR;
		break;
	case D3D11_STENCIL_OP_DECR:
		out = D3D10_STENCIL_OP_DECR;
		break;
	}
	return result;
}

HRESULT Convert( D3D11_DEPTH_STENCILOP_DESC &out, const D3D10_DEPTH_STENCILOP_DESC &in )
{
	HRESULT result = S_OK;

	if( FAILED( Convert( out.StencilFailOp, in.StencilFailOp ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.StencilDepthFailOp, in.StencilDepthFailOp ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.StencilPassOp, in.StencilPassOp ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.StencilFunc, in.StencilFunc ) ) )
		result = E_FAIL;

	return result;
}
HRESULT Convert( D3D10_DEPTH_STENCILOP_DESC &out, const D3D11_DEPTH_STENCILOP_DESC &in )
{
	HRESULT result = S_OK;

	if( FAILED( Convert( out.StencilFailOp, in.StencilFailOp ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.StencilDepthFailOp, in.StencilDepthFailOp ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.StencilPassOp, in.StencilPassOp ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.StencilFunc, in.StencilFunc ) ) )
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D11_DEPTH_STENCIL_DESC &out, const D3D10_DEPTH_STENCIL_DESC &in )
{
	HRESULT result = S_OK;

	out.DepthEnable = in.DepthEnable;
	if( FAILED( Convert( out.DepthWriteMask, in.DepthWriteMask ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.DepthFunc, in.DepthFunc ) ) )
		result = E_FAIL;
	out.StencilEnable = in.StencilEnable;
	out.StencilReadMask = in.StencilReadMask;
	out.StencilWriteMask = in.StencilWriteMask;
	if( FAILED( Convert( out.FrontFace, in.FrontFace ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.BackFace, in.BackFace ) ) )
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D10_DEPTH_STENCIL_DESC &out, const D3D11_DEPTH_STENCIL_DESC &in )
{
	HRESULT result = S_OK;

	out.DepthEnable = in.DepthEnable;
	if( FAILED( Convert( out.DepthWriteMask, in.DepthWriteMask ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.DepthFunc, in.DepthFunc ) ) )
		result = E_FAIL;
	out.StencilEnable = in.StencilEnable;
	out.StencilReadMask = in.StencilReadMask;
	out.StencilWriteMask = in.StencilWriteMask;
	if( FAILED( Convert( out.FrontFace, in.FrontFace ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.BackFace, in.BackFace ) ) )
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D11_BLEND &out, D3D10_BLEND in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_BLEND_ZERO:	//1
		out = D3D11_BLEND_ZERO;
		break;
	case D3D10_BLEND_ONE:	//2
		out = D3D11_BLEND_ONE;
		break;
	case D3D10_BLEND_SRC_COLOR:	//3
		out = D3D11_BLEND_SRC_COLOR;
		break;
	case D3D10_BLEND_INV_SRC_COLOR:	//4
		out = D3D11_BLEND_INV_SRC_COLOR;
		break;
	case D3D10_BLEND_SRC_ALPHA:	//5
		out = D3D11_BLEND_SRC_ALPHA;
		break;
	case D3D10_BLEND_INV_SRC_ALPHA:	//6
		out = D3D11_BLEND_INV_SRC_ALPHA;
		break;
	case D3D10_BLEND_DEST_ALPHA:	//7
		out = D3D11_BLEND_DEST_ALPHA;
		break;
	case D3D10_BLEND_INV_DEST_ALPHA:	//8
		out = D3D11_BLEND_INV_DEST_ALPHA;
		break;
	case D3D10_BLEND_DEST_COLOR:	//9
		out = D3D11_BLEND_DEST_COLOR;
		break;
	case D3D10_BLEND_INV_DEST_COLOR:	//10
		out = D3D11_BLEND_INV_DEST_COLOR;
		break;
	case D3D10_BLEND_SRC_ALPHA_SAT:	//11
		out = D3D11_BLEND_SRC_ALPHA_SAT;
		break;
	case D3D10_BLEND_BLEND_FACTOR:	//14
		out = D3D11_BLEND_BLEND_FACTOR;
		break;
	case D3D10_BLEND_INV_BLEND_FACTOR  :	//15
		out = D3D11_BLEND_INV_BLEND_FACTOR;
		break;
	case D3D10_BLEND_SRC1_COLOR:	//16
		out = D3D11_BLEND_SRC1_COLOR;
		break;
	case D3D10_BLEND_INV_SRC1_COLOR:	//17
		out = D3D11_BLEND_INV_SRC1_COLOR;
		break;
	case D3D10_BLEND_SRC1_ALPHA:	//18
		out = D3D11_BLEND_SRC1_ALPHA;
		break;
	case D3D10_BLEND_INV_SRC1_ALPHA:	//19
		out = D3D11_BLEND_INV_SRC1_ALPHA;
		break;

	}
	return result;
}

HRESULT Convert( D3D10_BLEND &out, D3D11_BLEND in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_BLEND_ZERO:	//1
		out = D3D10_BLEND_ZERO;
		break;
	case D3D11_BLEND_ONE:	//2
		out = D3D10_BLEND_ONE;
		break;
	case D3D11_BLEND_SRC_COLOR:	//3
		out = D3D10_BLEND_SRC_COLOR;
		break;
	case D3D11_BLEND_INV_SRC_COLOR:	//4
		out = D3D10_BLEND_INV_SRC_COLOR;
		break;
	case D3D11_BLEND_SRC_ALPHA:	//5
		out = D3D10_BLEND_SRC_ALPHA;
		break;
	case D3D11_BLEND_INV_SRC_ALPHA:	//6
		out = D3D10_BLEND_INV_SRC_ALPHA;
		break;
	case D3D11_BLEND_DEST_ALPHA:	//7
		out = D3D10_BLEND_DEST_ALPHA;
		break;
	case D3D11_BLEND_INV_DEST_ALPHA:	//8
		out = D3D10_BLEND_INV_DEST_ALPHA;
		break;
	case D3D11_BLEND_DEST_COLOR:	//9
		out = D3D10_BLEND_DEST_COLOR;
		break;
	case D3D11_BLEND_INV_DEST_COLOR:	//10
		out = D3D10_BLEND_INV_DEST_COLOR;
		break;
	case D3D11_BLEND_SRC_ALPHA_SAT:	//11
		out = D3D10_BLEND_SRC_ALPHA_SAT;
		break;
	case D3D11_BLEND_BLEND_FACTOR:	//14
		out = D3D10_BLEND_BLEND_FACTOR;
		break;
	case D3D11_BLEND_INV_BLEND_FACTOR:	//15
		out = D3D10_BLEND_INV_BLEND_FACTOR;
		break;
	case D3D11_BLEND_SRC1_COLOR:	//16
		out = D3D10_BLEND_SRC1_COLOR;
		break;
	case D3D11_BLEND_INV_SRC1_COLOR:	//17
		out = D3D10_BLEND_INV_SRC1_COLOR;
		break;
	case D3D11_BLEND_SRC1_ALPHA:	//18
		out = D3D10_BLEND_SRC1_ALPHA;
		break;
	case D3D11_BLEND_INV_SRC1_ALPHA:	//19
		out = D3D10_BLEND_INV_SRC1_ALPHA;
		break;

	}
	return result;
}

HRESULT Convert( D3D11_BLEND_OP  &out, D3D10_BLEND_OP  in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_BLEND_OP_ADD:	//1
		out = D3D11_BLEND_OP_ADD;
		break;
	case D3D10_BLEND_OP_SUBTRACT:	//2
		out = D3D11_BLEND_OP_SUBTRACT;
		break;
	case D3D10_BLEND_OP_REV_SUBTRACT :	//3
		out = D3D11_BLEND_OP_REV_SUBTRACT ;
		break;
	case D3D10_BLEND_OP_MIN:	//4
		out = D3D11_BLEND_OP_MIN;
		break;
	case D3D10_BLEND_OP_MAX:	//5
		out = D3D11_BLEND_OP_MAX;
		break;
	}
	return result;
}

HRESULT Convert( D3D10_BLEND_OP  &out, D3D11_BLEND_OP  in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_BLEND_OP_ADD:	//1
		out = D3D10_BLEND_OP_ADD;
		break;
	case D3D11_BLEND_OP_SUBTRACT:	//2
		out = D3D10_BLEND_OP_SUBTRACT;
		break;
	case D3D11_BLEND_OP_REV_SUBTRACT :	//3
		out = D3D10_BLEND_OP_REV_SUBTRACT ;
		break;
	case D3D11_BLEND_OP_MIN:	//4
		out = D3D10_BLEND_OP_MIN;
		break;
	case D3D11_BLEND_OP_MAX:	//5
		out = D3D10_BLEND_OP_MAX;
		break;
	}
	return result;
}

HRESULT Convert( D3D11_COLOR_WRITE_ENABLE &out, D3D10_COLOR_WRITE_ENABLE in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_COLOR_WRITE_ENABLE_RED:	//1
		out = D3D11_COLOR_WRITE_ENABLE_RED;
		break;
	case D3D10_COLOR_WRITE_ENABLE_GREEN:	//2
		out = D3D11_COLOR_WRITE_ENABLE_GREEN;
		break;
	case D3D10_COLOR_WRITE_ENABLE_BLUE:	//4
		out = D3D11_COLOR_WRITE_ENABLE_BLUE;
		break;
	case D3D10_COLOR_WRITE_ENABLE_ALPHA:	//8
		out = D3D11_COLOR_WRITE_ENABLE_ALPHA;
		break;
	case D3D10_COLOR_WRITE_ENABLE_ALL:	//8
		out = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	}
	return result;
}

HRESULT Convert( D3D10_COLOR_WRITE_ENABLE &out, D3D11_COLOR_WRITE_ENABLE in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_COLOR_WRITE_ENABLE_RED:	//1
		out = D3D10_COLOR_WRITE_ENABLE_RED;
		break;
	case D3D11_COLOR_WRITE_ENABLE_GREEN:	//2
		out = D3D10_COLOR_WRITE_ENABLE_GREEN;
		break;
	case D3D11_COLOR_WRITE_ENABLE_BLUE:	//4
		out = D3D10_COLOR_WRITE_ENABLE_BLUE;
		break;
	case D3D11_COLOR_WRITE_ENABLE_ALPHA:	//8
		out = D3D10_COLOR_WRITE_ENABLE_ALPHA;
		break;
	case D3D11_COLOR_WRITE_ENABLE_ALL:	//8
		out = D3D10_COLOR_WRITE_ENABLE_ALL;
		break;
	}
	return result;
}

HRESULT Convert( D3D11_RENDER_TARGET_BLEND_DESC1 &out, const D3D11_RENDER_TARGET_BLEND_DESC &in )
{
	HRESULT result = S_OK;

	out.BlendEnable = in.BlendEnable;
	out.LogicOpEnable = false;	// D3D11_RENDER_TARGET_BLEND_DESC does not have this field. Disable it.
	out.SrcBlend = in.SrcBlend;
	out.DestBlend = in.DestBlend;
	out.BlendOp = in.BlendOp;
	out.SrcBlendAlpha = in.SrcBlendAlpha;
	out.DestBlendAlpha = in.DestBlendAlpha;
	out.BlendOpAlpha = in.BlendOpAlpha;
	out.LogicOp = D3D11_LOGIC_OP_CLEAR;		// D3D11_RENDER_TARGET_BLEND_DESC does not have this field
	out.RenderTargetWriteMask = in.RenderTargetWriteMask;

	return result;
}

HRESULT Convert( D3D11_RENDER_TARGET_BLEND_DESC &out, const D3D11_RENDER_TARGET_BLEND_DESC1 &in )
{
	HRESULT result = S_OK;

	if( in.LogicOpEnable != false || in.LogicOp != D3D11_LOGIC_OP_CLEAR )
		result = S_FALSE;

	out.BlendEnable = in.BlendEnable;
	out.SrcBlend = in.SrcBlend;
	out.DestBlend = in.DestBlend;
	out.BlendOp = in.BlendOp;
	out.SrcBlendAlpha = in.SrcBlendAlpha;
	out.DestBlendAlpha = in.DestBlendAlpha;
	out.BlendOpAlpha = in.BlendOpAlpha;
	out.RenderTargetWriteMask = in.RenderTargetWriteMask;

	return result;
}

HRESULT Convert( D3D11_RENDER_TARGET_BLEND_DESC &out, const D3D10_RENDER_TARGET_BLEND_DESC1 &in )
{
	HRESULT result = S_OK;

	out.BlendEnable = in.BlendEnable;
	if( FAILED( Convert( out.SrcBlend, in.SrcBlend ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.DestBlend, in.DestBlend ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.BlendOp, in.BlendOp ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.SrcBlendAlpha, in.SrcBlendAlpha ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.DestBlendAlpha, in.DestBlendAlpha ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.BlendOpAlpha, in.BlendOpAlpha ) ) )
		result = E_FAIL;
	out.RenderTargetWriteMask = in.RenderTargetWriteMask;

	return result;
}

HRESULT Convert( D3D10_RENDER_TARGET_BLEND_DESC1 &out, const D3D11_RENDER_TARGET_BLEND_DESC &in )
{
	HRESULT result = S_OK;

	out.BlendEnable = in.BlendEnable;
	if( FAILED( Convert( out.SrcBlend, in.SrcBlend ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.DestBlend, in.DestBlend ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.BlendOp, in.BlendOp ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.SrcBlendAlpha, in.SrcBlendAlpha ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.DestBlendAlpha, in.DestBlendAlpha ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out.BlendOpAlpha, in.BlendOpAlpha ) ) )
		result = E_FAIL;
	out.RenderTargetWriteMask = in.RenderTargetWriteMask;

	return result;
}

HRESULT Convert( D3D10_RENDER_TARGET_BLEND_DESC1 &out, const D3D11_RENDER_TARGET_BLEND_DESC1 &in )
{
	HRESULT result = S_OK;

	D3D11_RENDER_TARGET_BLEND_DESC tmp11;
	if( FAILED( Convert( tmp11, in ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out, tmp11 ) ) )
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D11_RENDER_TARGET_BLEND_DESC1 &out, const D3D10_RENDER_TARGET_BLEND_DESC1 &in )
{
	HRESULT result = S_OK;

	D3D11_RENDER_TARGET_BLEND_DESC tmp11;
	if( FAILED( Convert( tmp11, in ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out, tmp11 ) ) )
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D11_BLEND_DESC1 &out, const D3D11_BLEND_DESC &in )
{
	HRESULT result = S_OK;

	out.AlphaToCoverageEnable = in.AlphaToCoverageEnable;
	out.IndependentBlendEnable = in.IndependentBlendEnable;
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
	{
		if( FAILED( Convert(out.RenderTarget[i], in.RenderTarget[i])))
			result = E_FAIL;
	}

	return result;
}

HRESULT Convert( D3D11_BLEND_DESC &out, const D3D11_BLEND_DESC1 &in )
{
	HRESULT result = S_OK;

	out.AlphaToCoverageEnable = in.AlphaToCoverageEnable;
	out.IndependentBlendEnable = in.IndependentBlendEnable;
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
	{
		if( FAILED( Convert(out.RenderTarget[i], in.RenderTarget[i])))
			result = E_FAIL;
	}

	return result;
}

HRESULT Convert( D3D11_BLEND_DESC1 &out, const D3D10_BLEND_DESC1 &in )
{
	HRESULT result = S_OK;

	out.AlphaToCoverageEnable = in.AlphaToCoverageEnable;
	out.IndependentBlendEnable = in.IndependentBlendEnable;
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
	{
		if( FAILED( Convert(out.RenderTarget[i], in.RenderTarget[i])))
			result = E_FAIL;
	}

	return result;
}

HRESULT Convert( D3D10_BLEND_DESC1 &out, const D3D11_BLEND_DESC1 &in )
{
	HRESULT result = S_OK;

	out.AlphaToCoverageEnable = in.AlphaToCoverageEnable;
	out.IndependentBlendEnable = in.IndependentBlendEnable;
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
	{
		if( FAILED( Convert(out.RenderTarget[i], in.RenderTarget[i])))
			result = E_FAIL;
	}

	return result;
}

HRESULT Convert( D3D11_BLEND_DESC1 &out, const D3D10_BLEND_DESC &in )
{
	HRESULT result = S_OK;

	out.AlphaToCoverageEnable = in.AlphaToCoverageEnable;
	// default is true
	out.IndependentBlendEnable = TRUE;
	for (UINT i = 0; i < min( D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT); ++i )
	{
		out.RenderTarget[i].BlendEnable = in.BlendEnable[ i ];

		if( FAILED( Convert(out.RenderTarget[i].SrcBlend, in.SrcBlend)))
			result = E_FAIL;
		if( FAILED( Convert(out.RenderTarget[i].DestBlend, in.DestBlend)))
			result = E_FAIL;
		if( FAILED( Convert(out.RenderTarget[i].BlendOp, in.BlendOp)))
			result = E_FAIL;
		if( FAILED( Convert(out.RenderTarget[i].SrcBlendAlpha, in.SrcBlendAlpha)))
			result = E_FAIL;
		if( FAILED( Convert(out.RenderTarget[i].DestBlendAlpha, in.DestBlendAlpha)))
			result = E_FAIL;
		if( FAILED( Convert(out.RenderTarget[i].BlendOpAlpha, in.BlendOpAlpha)))
			result = E_FAIL;
		out.RenderTarget[i].RenderTargetWriteMask = in.RenderTargetWriteMask[i];
	}

	return result;
}

HRESULT Convert( D3D10_BLEND_DESC &out, const D3D11_BLEND_DESC1 &in )
{
	HRESULT result = S_OK;

	out.AlphaToCoverageEnable = in.AlphaToCoverageEnable;
	for (UINT i = 0; i < min( D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT); ++i )
	{
		out.BlendEnable[ i ] = in.RenderTarget[i].BlendEnable;
		out.RenderTargetWriteMask[i] = in.RenderTarget[i].RenderTargetWriteMask;
	}
	//echoing d3d10.1's conversion from BLEND_DESC1 to BLEND_DESC:
	UINT first_enabled = 0;
	for (UINT i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
	{
		if ( in.RenderTarget[i].BlendEnable )
		{
			first_enabled = i;
			break;
		}
	}	

	if( FAILED( Convert(out.SrcBlend, in.RenderTarget[first_enabled].SrcBlend )))
		result = E_FAIL;
	if( FAILED( Convert(out.DestBlend, in.RenderTarget[first_enabled].DestBlend)))
		result = E_FAIL;
	if( FAILED( Convert(out.BlendOp, in.RenderTarget[first_enabled].BlendOp)))
		result = E_FAIL;
	if( FAILED( Convert(out.SrcBlendAlpha, in.RenderTarget[first_enabled].SrcBlendAlpha)))
		result = E_FAIL;
	if( FAILED( Convert(out.DestBlendAlpha, in.RenderTarget[first_enabled].DestBlendAlpha)))
		result = E_FAIL;
	if( FAILED( Convert(out.BlendOpAlpha, in.RenderTarget[first_enabled].BlendOpAlpha)))
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D10_BLEND_DESC1 &out, const D3D11_BLEND_DESC &in )
{
	HRESULT result = S_OK;

	D3D11_BLEND_DESC1 tmp11_1;
	if( FAILED( Convert( tmp11_1, in ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out, tmp11_1 ) ) )
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D11_BLEND_DESC &out, const D3D10_BLEND_DESC1 &in )
{
	HRESULT result = S_OK;

	D3D11_BLEND_DESC1 tmp11_1;
	if( FAILED( Convert( tmp11_1, in ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out, tmp11_1 ) ) )
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D10_BLEND_DESC &out, const D3D11_BLEND_DESC &in )
{
	HRESULT result = S_OK;

	D3D11_BLEND_DESC1 tmp11_1;
	if( FAILED( Convert( tmp11_1, in ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out, tmp11_1 ) ) )
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D11_BLEND_DESC &out, const D3D10_BLEND_DESC &in )
{
	HRESULT result = S_OK;

	D3D11_BLEND_DESC1 tmp11_1;
	if( FAILED( Convert( tmp11_1, in ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out, tmp11_1 ) ) )
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D11_RASTERIZER_DESC &out, const D3D11_RASTERIZER_DESC1 &in )
{
	HRESULT result = S_OK;

	if( in.ForcedSampleCount != 0 )
		result = E_FAIL;

	out.FillMode = in.FillMode;
	out.CullMode = in.CullMode;
	out.FrontCounterClockwise = in.FrontCounterClockwise;
	out.DepthBias = in.DepthBias;
	out.DepthBiasClamp = in.DepthBiasClamp;
	out.SlopeScaledDepthBias = in.SlopeScaledDepthBias;
	out.DepthClipEnable = in.DepthClipEnable;
	out.ScissorEnable = in.ScissorEnable;
	out.MultisampleEnable = in.MultisampleEnable;
	out.AntialiasedLineEnable = in.AntialiasedLineEnable;

	return result;
}

HRESULT Convert( D3D11_RASTERIZER_DESC1 &out, const D3D11_RASTERIZER_DESC &in )
{
	HRESULT result = S_OK;

	out.FillMode = in.FillMode;
	out.CullMode = in.CullMode;
	out.FrontCounterClockwise = in.FrontCounterClockwise;
	out.DepthBias = in.DepthBias;
	out.DepthBiasClamp = in.DepthBiasClamp;
	out.SlopeScaledDepthBias = in.SlopeScaledDepthBias;
	out.DepthClipEnable = in.DepthClipEnable;
	out.ScissorEnable = in.ScissorEnable;
	out.MultisampleEnable = in.MultisampleEnable;
	out.AntialiasedLineEnable = in.AntialiasedLineEnable;
	out.ForcedSampleCount = 0; // default. D3D11_RASTERIZER_DESC does not have this field

	return result;
}

HRESULT Convert( D3D10_RASTERIZER_DESC &out, const D3D11_RASTERIZER_DESC &in )
{
	HRESULT result = S_OK;

	if( FAILED( Convert( out.FillMode, in.FillMode)))
		result = E_FAIL;
	if( FAILED( Convert( out.CullMode, in.CullMode)))
		result = E_FAIL;
	out.FrontCounterClockwise = in.FrontCounterClockwise;
	out.DepthBias = in.DepthBias;
	out.DepthBiasClamp = in.DepthBiasClamp;
	out.SlopeScaledDepthBias = in.SlopeScaledDepthBias;
	out.DepthClipEnable = in.DepthClipEnable;
	out.ScissorEnable = in.ScissorEnable;
	out.MultisampleEnable = in.MultisampleEnable;
	out.AntialiasedLineEnable = in.AntialiasedLineEnable;

	return result;
}

HRESULT Convert( D3D11_RASTERIZER_DESC &out, const D3D10_RASTERIZER_DESC &in )
{
	HRESULT result = S_OK;

	if( FAILED( Convert( out.FillMode, in.FillMode)))
		result = E_FAIL;
	if( FAILED( Convert( out.CullMode, in.CullMode)))
		result = E_FAIL;
	out.FrontCounterClockwise = in.FrontCounterClockwise;
	out.DepthBias = in.DepthBias;
	out.DepthBiasClamp = in.DepthBiasClamp;
	out.SlopeScaledDepthBias = in.SlopeScaledDepthBias;
	out.DepthClipEnable = in.DepthClipEnable;
	out.ScissorEnable = in.ScissorEnable;
	out.MultisampleEnable = in.MultisampleEnable;
	out.AntialiasedLineEnable = in.AntialiasedLineEnable;

	return result;
}

HRESULT Convert( D3D11_RASTERIZER_DESC1 &out, const D3D10_RASTERIZER_DESC &in )
{
	HRESULT result = S_OK;

	D3D11_RASTERIZER_DESC tmp11;
	if( FAILED( Convert( tmp11, in ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out, tmp11 ) ) )
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D10_RASTERIZER_DESC &out, const D3D11_RASTERIZER_DESC1 &in )
{
	HRESULT result = S_OK;

	D3D11_RASTERIZER_DESC tmp11;
	if( FAILED( Convert( tmp11, in ) ) )
		result = E_FAIL;
	if( FAILED( Convert( out, tmp11 ) ) )
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D11_SUBRESOURCE_DATA &out, const D3D10_SUBRESOURCE_DATA &in )
{
	HRESULT result = S_OK;

	out.pSysMem = in.pSysMem;
	out.SysMemPitch = in.SysMemPitch;
	out.SysMemPitch = in.SysMemSlicePitch;

	return result;
}

HRESULT Convert( D3D10_SUBRESOURCE_DATA &out, const D3D11_SUBRESOURCE_DATA &in )
{
	HRESULT result = S_OK;

	out.pSysMem = in.pSysMem;
	out.SysMemPitch = in.SysMemPitch;
	out.SysMemPitch = in.SysMemSlicePitch;

	return result;
}

HRESULT Convert( D3D11_BUFFER_DESC &out, const D3D10_BUFFER_DESC &in )
{
	HRESULT result = S_OK;

	out.ByteWidth = in.ByteWidth;
	if( FAILED( Convert( out.Usage, in.Usage ) ))
		result = E_FAIL;
	result = ConvertBindFlag10to11( out.BindFlags, in.BindFlags);
	result = ConvertCpuAccessFlag10to11( out.CPUAccessFlags, in.CPUAccessFlags);
	result = ConvertResourceMiscFlag10to11( out.MiscFlags, in.MiscFlags);
	out.StructureByteStride = 0;

	return result;
}

HRESULT Convert( D3D10_BUFFER_DESC &out, const D3D11_BUFFER_DESC &in )
{
	HRESULT result = S_OK;

	if( in.StructureByteStride != 0 )
		result = E_FAIL;

	out.ByteWidth = in.ByteWidth;
	if( FAILED( Convert( out.Usage, in.Usage ) ))
		result = E_FAIL;
	result = ConvertBindFlag11to10( out.BindFlags, in.BindFlags);
	result = ConvertCpuAccessFlag11to10( out.CPUAccessFlags, in.CPUAccessFlags);
	result = ConvertResourceMiscFlag11to10( out.MiscFlags, in.MiscFlags);

	return result;
}

HRESULT Convert( D3D11_TEXTURE1D_DESC &out, const D3D10_TEXTURE1D_DESC &in )
{
	HRESULT result = S_OK;

	out.Width = in.Width;
	out.MipLevels = in.MipLevels;
	out.ArraySize = in.ArraySize;
	out.Format = in.Format;

	if( FAILED( Convert( out.Usage, in.Usage ) ))
		result = E_FAIL;
	result = ConvertBindFlag10to11( out.BindFlags, in.BindFlags);
	result = ConvertCpuAccessFlag10to11( out.CPUAccessFlags, in.CPUAccessFlags);
	result = ConvertResourceMiscFlag10to11( out.MiscFlags, in.MiscFlags);

	return result;
}

HRESULT Convert( D3D10_TEXTURE1D_DESC &out, const D3D11_TEXTURE1D_DESC &in )
{
	HRESULT result = S_OK;

	out.Width = in.Width;
	out.MipLevels = in.MipLevels;
	out.ArraySize = in.ArraySize;
	out.Format = in.Format;

	if( FAILED( Convert( out.Usage, in.Usage ) ))
		result = E_FAIL;
	result = ConvertBindFlag11to10( out.BindFlags, in.BindFlags);
	result = ConvertCpuAccessFlag11to10( out.CPUAccessFlags, in.CPUAccessFlags);
	result = ConvertResourceMiscFlag11to10( out.MiscFlags, in.MiscFlags);

	return result;
}

HRESULT Convert( D3D11_TEXTURE2D_DESC &out, const D3D10_TEXTURE2D_DESC &in )
{
	HRESULT result = S_OK;

	out.Width = in.Width;
	out.Height = in.Height;
	out.MipLevels = in.MipLevels;
	out.ArraySize = in.ArraySize;
	out.Format = in.Format;
	out.SampleDesc = in.SampleDesc;

	if( FAILED( Convert( out.Usage, in.Usage ) ))
		result = E_FAIL;
	result = ConvertBindFlag10to11( out.BindFlags, in.BindFlags);
	result = ConvertCpuAccessFlag10to11( out.CPUAccessFlags, in.CPUAccessFlags);
	result = ConvertResourceMiscFlag10to11( out.MiscFlags, in.MiscFlags);

	return result;
}

HRESULT Convert( D3D10_TEXTURE2D_DESC &out, const D3D11_TEXTURE2D_DESC &in )
{
	HRESULT result = S_OK;

	out.Width = in.Width;
	out.Height = in.Height;
	out.MipLevels = in.MipLevels;
	out.ArraySize = in.ArraySize;
	out.Format = in.Format;
	out.SampleDesc = in.SampleDesc;

	if( FAILED( Convert( out.Usage, in.Usage ) ))
		result = E_FAIL;
	result = ConvertBindFlag11to10( out.BindFlags, in.BindFlags);
	result = ConvertCpuAccessFlag11to10( out.CPUAccessFlags, in.CPUAccessFlags);
	result = ConvertResourceMiscFlag11to10( out.MiscFlags, in.MiscFlags);

	return result;
}

HRESULT Convert( D3D11_TEXTURE3D_DESC &out, const D3D10_TEXTURE3D_DESC &in )
{
	HRESULT result = S_OK;

	out.Width = in.Width;
	out.Height = in.Height;
	out.Depth = in.Depth;
	out.MipLevels = in.MipLevels;
	out.Format = in.Format;

	if( FAILED( Convert( out.Usage, in.Usage ) ))
		result = E_FAIL;
	result = ConvertBindFlag10to11( out.BindFlags, in.BindFlags);
	result = ConvertCpuAccessFlag10to11( out.CPUAccessFlags, in.CPUAccessFlags);
	result = ConvertResourceMiscFlag10to11( out.MiscFlags, in.MiscFlags);

	return result;
}

HRESULT Convert( D3D10_TEXTURE3D_DESC &out, const D3D11_TEXTURE3D_DESC &in )
{
	HRESULT result = S_OK;

	out.Width = in.Width;
	out.Height = in.Height;
	out.Depth = in.Depth;
	out.MipLevels = in.MipLevels;
	out.Format = in.Format;

	if( FAILED( Convert( out.Usage, in.Usage ) ))
		result = E_FAIL;
	result = ConvertBindFlag11to10( out.BindFlags, in.BindFlags);
	result = ConvertCpuAccessFlag11to10( out.CPUAccessFlags, in.CPUAccessFlags);
	result = ConvertResourceMiscFlag11to10( out.MiscFlags, in.MiscFlags);

	return result;
}

HRESULT Convert( D3D11_TEXTURECUBE_FACE  &out, D3D10_TEXTURECUBE_FACE  in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_TEXTURECUBE_FACE_POSITIVE_X:	//0
		out = D3D11_TEXTURECUBE_FACE_POSITIVE_X;
		break;
	case D3D10_TEXTURECUBE_FACE_NEGATIVE_X:	//1
		out = D3D11_TEXTURECUBE_FACE_NEGATIVE_X;
		break;
	case D3D10_TEXTURECUBE_FACE_POSITIVE_Y:	//2
		out = D3D11_TEXTURECUBE_FACE_POSITIVE_Y;
		break;
	case D3D10_TEXTURECUBE_FACE_NEGATIVE_Y :	//3
		out = D3D11_TEXTURECUBE_FACE_NEGATIVE_Y ;
		break;
	case D3D10_TEXTURECUBE_FACE_POSITIVE_Z:	//4
		out = D3D11_TEXTURECUBE_FACE_POSITIVE_Z;
		break;
	case D3D10_TEXTURECUBE_FACE_NEGATIVE_Z:	//5
		out = D3D11_TEXTURECUBE_FACE_NEGATIVE_Z;
		break;
	}
	return result;
}

HRESULT Convert( D3D10_TEXTURECUBE_FACE  &out, D3D11_TEXTURECUBE_FACE  in )
{	
	HRESULT result = S_OK;

	switch( in )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_TEXTURECUBE_FACE_POSITIVE_X:	//0
		out = D3D10_TEXTURECUBE_FACE_POSITIVE_X;
		break;
	case D3D11_TEXTURECUBE_FACE_NEGATIVE_X:	//1
		out = D3D10_TEXTURECUBE_FACE_NEGATIVE_X;
		break;
	case D3D11_TEXTURECUBE_FACE_POSITIVE_Y:	//2
		out = D3D10_TEXTURECUBE_FACE_POSITIVE_Y;
		break;
	case D3D11_TEXTURECUBE_FACE_NEGATIVE_Y :	//3
		out = D3D10_TEXTURECUBE_FACE_NEGATIVE_Y ;
		break;
	case D3D11_TEXTURECUBE_FACE_POSITIVE_Z:	//4
		out = D3D10_TEXTURECUBE_FACE_POSITIVE_Z;
		break;
	case D3D11_TEXTURECUBE_FACE_NEGATIVE_Z:	//5
		out = D3D10_TEXTURECUBE_FACE_NEGATIVE_Z;
		break;
	}
	return result;
}

HRESULT Convert( D3D11_BUFFER_SRV &out, const D3D10_BUFFER_SRV &in )
{
	out.ElementOffset = in.ElementOffset;
	out.ElementWidth = in.ElementWidth;

	return S_OK;
}

HRESULT Convert( D3D10_BUFFER_SRV &out, const D3D11_BUFFER_SRV &in )
{
	out.ElementOffset = in.ElementOffset;
	out.ElementWidth = in.ElementWidth;

	return S_OK;
}

HRESULT Convert( D3D11_TEX1D_SRV &out, const D3D10_TEX1D_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;

	return S_OK;
}

HRESULT Convert( D3D10_TEX1D_SRV &out, const D3D11_TEX1D_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;

	return S_OK;
}

HRESULT Convert( D3D11_TEX1D_ARRAY_SRV &out, const D3D10_TEX1D_ARRAY_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;

	return S_OK;
}

HRESULT Convert( D3D10_TEX1D_ARRAY_SRV &out, const D3D11_TEX1D_ARRAY_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;

	return S_OK;
}

HRESULT Convert( D3D11_TEX2D_SRV &out, const D3D10_TEX2D_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;

	return S_OK;
}

HRESULT Convert( D3D10_TEX2D_SRV &out, const D3D11_TEX2D_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;

	return S_OK;
}

HRESULT Convert( D3D11_TEX2D_ARRAY_SRV &out, const D3D10_TEX2D_ARRAY_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;

	return S_OK;
}

HRESULT Convert( D3D10_TEX2D_ARRAY_SRV &out, const D3D11_TEX2D_ARRAY_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;

	return S_OK;
}

HRESULT Convert( D3D11_TEX3D_SRV &out, const D3D10_TEX3D_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;

	return S_OK;
}

HRESULT Convert( D3D10_TEX3D_SRV &out, const D3D11_TEX3D_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;

	return S_OK;
}

HRESULT Convert( D3D11_TEXCUBE_SRV &out, const D3D10_TEXCUBE_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;

	return S_OK;
}

HRESULT Convert( D3D10_TEXCUBE_SRV &out, const D3D11_TEXCUBE_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;

	return S_OK;
}

HRESULT Convert( D3D11_TEXCUBE_ARRAY_SRV &out, const D3D10_TEXCUBE_ARRAY_SRV1 &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;
	out.First2DArrayFace = in.First2DArrayFace;
	out.NumCubes = in.NumCubes;

	return S_OK;
}

HRESULT Convert( D3D10_TEXCUBE_ARRAY_SRV1 &out, const D3D11_TEXCUBE_ARRAY_SRV &in )
{
	out.MostDetailedMip = in.MostDetailedMip;
	out.MipLevels = in.MipLevels;
	out.First2DArrayFace = in.First2DArrayFace;
	out.NumCubes = in.NumCubes;

	return S_OK;
}

HRESULT Convert( D3D11_TEX2DMS_SRV &out, const D3D10_TEX2DMS_SRV &in )
{
	out.UnusedField_NothingToDefine = in.UnusedField_NothingToDefine;
	return S_OK;
}

HRESULT Convert( D3D10_TEX2DMS_SRV &out, const D3D11_TEX2DMS_SRV &in )
{
	out.UnusedField_NothingToDefine = in.UnusedField_NothingToDefine;
	return S_OK;
}

HRESULT Convert( D3D11_TEX2DMS_ARRAY_SRV &out, const D3D10_TEX2DMS_ARRAY_SRV &in )
{
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;

	return S_OK;
}

HRESULT Convert( D3D10_TEX2DMS_ARRAY_SRV &out, const D3D11_TEX2DMS_ARRAY_SRV &in )
{
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;

	return S_OK;
}

HRESULT Convert( D3D11_SHADER_RESOURCE_VIEW_DESC &out, const D3D10_SHADER_RESOURCE_VIEW_DESC1 &in )
{
	HRESULT result = S_OK;

	out.Format = in.Format;
	switch( in.ViewDimension )
	{
	default:
		result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_UNKNOWN:	//0
		out.ViewDimension = D3D_SRV_DIMENSION_UNKNOWN;
		break;
	case D3D_SRV_DIMENSION_BUFFER:	//1
		out.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
		if( FAILED( Convert( out.Buffer, in.Buffer )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE1D:	//2
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1D;
		if( FAILED( Convert( out.Texture1D, in.Texture1D )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY:	//3
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1DARRAY;
		if( FAILED( Convert( out.Texture1DArray, in.Texture1DArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2D:	//4
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		if( FAILED( Convert( out.Texture2D, in.Texture2D )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY:	//5
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
		if( FAILED( Convert( out.Texture2DArray, in.Texture2DArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DMS:	//6
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
		if( FAILED( Convert( out.Texture2DMS, in.Texture2DMS )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:	//7
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMSARRAY;
		if( FAILED( Convert( out.Texture2DMSArray, in.Texture2DMSArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE3D:	//8
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
		if( FAILED( Convert( out.Texture3D, in.Texture3D )) )
			result = E_FAIL;
		break;
	case D3D11_SRV_DIMENSION_TEXTURECUBE:	//9
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
		if( FAILED( Convert( out.TextureCube, in.TextureCube )) )
			result = E_FAIL;
		break;
	case D3D11_SRV_DIMENSION_TEXTURECUBEARRAY:	//10
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBEARRAY;
		if( FAILED( Convert( out.TextureCubeArray, in.TextureCubeArray )) )
			result = E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_SHADER_RESOURCE_VIEW_DESC1 &out, const D3D11_SHADER_RESOURCE_VIEW_DESC &in )
{
	HRESULT result = S_OK;

	out.Format = in.Format;
	switch( in.ViewDimension )
	{
	default:
		result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_UNKNOWN:	//0
		out.ViewDimension = D3D_SRV_DIMENSION_UNKNOWN;
		break;
	case D3D_SRV_DIMENSION_BUFFER:	//1
		out.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
		if( FAILED( Convert( out.Buffer, in.Buffer )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE1D:	//2
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1D;
		if( FAILED( Convert( out.Texture1D, in.Texture1D )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY:	//3
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1DARRAY;
		if( FAILED( Convert( out.Texture1DArray, in.Texture1DArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2D:	//4
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		if( FAILED( Convert( out.Texture2D, in.Texture2D )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY:	//5
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
		if( FAILED( Convert( out.Texture2DArray, in.Texture2DArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DMS:	//6
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
		if( FAILED( Convert( out.Texture2DMS, in.Texture2DMS )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:	//7
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMSARRAY;
		if( FAILED( Convert( out.Texture2DMSArray, in.Texture2DMSArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE3D:	//8
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
		if( FAILED( Convert( out.Texture3D, in.Texture3D )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURECUBE:	//9
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
		if( FAILED( Convert( out.TextureCube, in.TextureCube )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:	//10
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBEARRAY;
		if( FAILED( Convert( out.TextureCubeArray, in.TextureCubeArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_BUFFEREX:	//11
		result = E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_SHADER_RESOURCE_VIEW_DESC &out, const D3D10_SHADER_RESOURCE_VIEW_DESC1 &in )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC temp;
	if( Convert( temp, in ) == S_OK )
		return Convert( out, temp );
	return E_FAIL;
}

HRESULT Convert( D3D10_SHADER_RESOURCE_VIEW_DESC1 &out, const D3D10_SHADER_RESOURCE_VIEW_DESC &in )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC temp;
	if( Convert( temp, in ) == S_OK )
		return Convert( out, temp );
	return E_FAIL;
}

HRESULT Convert( D3D11_SHADER_RESOURCE_VIEW_DESC &out, const D3D10_SHADER_RESOURCE_VIEW_DESC &in )
{
	HRESULT result = S_OK;

	out.Format = in.Format;
	switch( in.ViewDimension )
	{
	default:
		result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_UNKNOWN:	//0
		out.ViewDimension = D3D_SRV_DIMENSION_UNKNOWN;
		break;
	case D3D_SRV_DIMENSION_BUFFER:	//1
		out.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
		if( FAILED( Convert( out.Buffer, in.Buffer )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE1D:	//2
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1D;
		if( FAILED( Convert( out.Texture1D, in.Texture1D )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY:	//3
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1DARRAY;
		if( FAILED( Convert( out.Texture1DArray, in.Texture1DArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2D:	//4
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		if( FAILED( Convert( out.Texture2D, in.Texture2D )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY:	//5
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
		if( FAILED( Convert( out.Texture2DArray, in.Texture2DArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DMS:	//6
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
		if( FAILED( Convert( out.Texture2DMS, in.Texture2DMS )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:	//7
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMSARRAY;
		if( FAILED( Convert( out.Texture2DMSArray, in.Texture2DMSArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE3D:	//8
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
		if( FAILED( Convert( out.Texture3D, in.Texture3D )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURECUBE:	//9
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
		if( FAILED( Convert( out.TextureCube, in.TextureCube )) )
			result = E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_SHADER_RESOURCE_VIEW_DESC &out, const D3D11_SHADER_RESOURCE_VIEW_DESC &in )
{
	HRESULT result = S_OK;

	out.Format = in.Format;
	switch( in.ViewDimension )
	{
	default:
		result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_UNKNOWN:	//0
		out.ViewDimension = D3D_SRV_DIMENSION_UNKNOWN;
		break;
	case D3D_SRV_DIMENSION_BUFFER:	//1
		out.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
		if( FAILED( Convert( out.Buffer, in.Buffer )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE1D:	//2
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1D;
		if( FAILED( Convert( out.Texture1D, in.Texture1D )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY:	//3
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1DARRAY;
		if( FAILED( Convert( out.Texture1DArray, in.Texture1DArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2D:	//4
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		if( FAILED( Convert( out.Texture2D, in.Texture2D )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY:	//5
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
		if( FAILED( Convert( out.Texture2DArray, in.Texture2DArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DMS:	//6
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
		if( FAILED( Convert( out.Texture2DMS, in.Texture2DMS )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:	//7
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMSARRAY;
		if( FAILED( Convert( out.Texture2DMSArray, in.Texture2DMSArray )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURE3D:	//8
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
		if( FAILED( Convert( out.Texture3D, in.Texture3D )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURECUBE:	//9
		out.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
		if( FAILED( Convert( out.TextureCube, in.TextureCube )) )
			result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:	//10
		result = E_FAIL;
		break;
	case D3D_SRV_DIMENSION_BUFFEREX:	//10
		result = E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_BUFFER_RTV &out, const D3D10_BUFFER_RTV &in )
{
	out.ElementOffset = in.ElementOffset;
	out.ElementWidth = in.ElementWidth;
	return S_OK;
}

HRESULT Convert( D3D10_BUFFER_RTV &out, const D3D11_BUFFER_RTV &in )
{
	out.ElementOffset = in.ElementOffset;
	out.ElementWidth = in.ElementWidth;
	return S_OK;
}

HRESULT Convert( D3D11_TEX1D_RTV &out, const D3D10_TEX1D_RTV &in )
{
	out.MipSlice = in.MipSlice;
	return S_OK;
}

HRESULT Convert( D3D10_TEX1D_RTV &out, const D3D11_TEX1D_RTV &in )
{
	out.MipSlice = in.MipSlice;
	return S_OK;
}

HRESULT Convert( D3D11_TEX1D_ARRAY_RTV &out, const D3D10_TEX1D_ARRAY_RTV &in )
{
	out.MipSlice = in.MipSlice;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}
HRESULT Convert( D3D10_TEX1D_ARRAY_RTV &out, const D3D11_TEX1D_ARRAY_RTV &in )
{
	out.MipSlice = in.MipSlice;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}
HRESULT Convert( D3D11_TEX2D_RTV &out, const D3D10_TEX2D_RTV &in )
{
	out.MipSlice = in.MipSlice;
	return S_OK;
}
HRESULT Convert( D3D10_TEX2D_RTV &out, const D3D11_TEX2D_RTV &in )
{
	out.MipSlice = in.MipSlice;
	return S_OK;
}
HRESULT Convert( D3D11_TEX2DMS_RTV &out, const D3D10_TEX2DMS_RTV &in )
{
	out.UnusedField_NothingToDefine = in.UnusedField_NothingToDefine;
	return S_OK;
}
HRESULT Convert( D3D10_TEX2DMS_RTV &out, const D3D11_TEX2DMS_RTV &in )
{
	out.UnusedField_NothingToDefine = in.UnusedField_NothingToDefine;
	return S_OK;
}

HRESULT Convert( D3D11_TEX2D_ARRAY_RTV &out, const D3D10_TEX2D_ARRAY_RTV &in )
{
	out.MipSlice = in.MipSlice;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}
HRESULT Convert( D3D10_TEX2D_ARRAY_RTV &out, const D3D11_TEX2D_ARRAY_RTV &in )
{
	out.MipSlice = in.MipSlice;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}
HRESULT Convert( D3D11_TEX2DMS_ARRAY_RTV &out, const D3D10_TEX2DMS_ARRAY_RTV &in )
{
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}
HRESULT Convert( D3D10_TEX2DMS_ARRAY_RTV &out, const D3D11_TEX2DMS_ARRAY_RTV &in )
{
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}
HRESULT Convert( D3D11_TEX3D_RTV &out, const D3D10_TEX3D_RTV &in )
{
	out.MipSlice = in.MipSlice;
	out.FirstWSlice = in.FirstWSlice;
	out.WSize = in.WSize;
	return S_OK;
}
HRESULT Convert( D3D10_TEX3D_RTV &out, const D3D11_TEX3D_RTV &in )
{
	out.MipSlice = in.MipSlice;
	out.FirstWSlice = in.FirstWSlice;
	out.WSize = in.WSize;
	return S_OK;
}
HRESULT Convert( D3D10_RENDER_TARGET_VIEW_DESC &out, const D3D11_RENDER_TARGET_VIEW_DESC &in )
{
	HRESULT result = S_OK;

	out.Format = in.Format;
	if( FAILED( Convert( out.ViewDimension, in.ViewDimension )) )
		return E_FAIL;

	switch( in.ViewDimension )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_RTV_DIMENSION_UNKNOWN:	//0
		break;
	case D3D11_RTV_DIMENSION_BUFFER:	//1
		if( FAILED( Convert( out.Buffer, in.Buffer )) )
			result = E_FAIL;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE1D:	//2
		if( FAILED( Convert( out.Texture1D, in.Texture1D )) )
			result = E_FAIL;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:	
		if( FAILED( Convert( out.Texture1DArray, in.Texture1DArray )) )
			result = E_FAIL;
	case D3D11_RTV_DIMENSION_TEXTURE2D:	
		if( FAILED( Convert( out.Texture2D, in.Texture2D )) )
			result = E_FAIL;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:	//5
		if( FAILED( Convert( out.Texture2DArray, in.Texture2DArray )) )
			result = E_FAIL;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE2DMS:	
		if( FAILED( Convert( out.Texture2DMS, in.Texture2DMS )) )
			result = E_FAIL;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:	
		if( FAILED( Convert( out.Texture2DMSArray, in.Texture2DMSArray )) )
			result = E_FAIL;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE3D:	//8
		if( FAILED( Convert( out.Texture3D, in.Texture3D )) )
			result = E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_RENDER_TARGET_VIEW_DESC &out, const D3D10_RENDER_TARGET_VIEW_DESC &in )
{
	HRESULT result = S_OK;

	out.Format = in.Format;
	if( FAILED( Convert( out.ViewDimension, in.ViewDimension )) )
		return E_FAIL;

	switch( in.ViewDimension )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_RTV_DIMENSION_UNKNOWN:	//0
		break;
	case D3D10_RTV_DIMENSION_BUFFER:	//1
		if( FAILED( Convert( out.Buffer, in.Buffer )) )
			result = E_FAIL;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE1D:	//2
		if( FAILED( Convert( out.Texture1D, in.Texture1D )) )
			result = E_FAIL;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE1DARRAY:	
		if( FAILED( Convert( out.Texture1DArray, in.Texture1DArray )) )
			result = E_FAIL;
	case D3D10_RTV_DIMENSION_TEXTURE2D:	
		if( FAILED( Convert( out.Texture2D, in.Texture2D )) )
			result = E_FAIL;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE2DARRAY:	//5
		if( FAILED( Convert( out.Texture2DArray, in.Texture2DArray )) )
			result = E_FAIL;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE2DMS:	
		if( FAILED( Convert( out.Texture2DMS, in.Texture2DMS )) )
			result = E_FAIL;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY:	
		if( FAILED( Convert( out.Texture2DMSArray, in.Texture2DMSArray )) )
			result = E_FAIL;
		break;
	case D3D10_RTV_DIMENSION_TEXTURE3D:	//8
		if( FAILED( Convert( out.Texture3D, in.Texture3D )) )
			result = E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_TEX1D_DSV &out, const D3D10_TEX1D_DSV &in )
{
	out.MipSlice = in.MipSlice;
	return S_OK;
}

HRESULT Convert( D3D10_TEX1D_DSV &out, const D3D11_TEX1D_DSV &in )
{
	out.MipSlice = in.MipSlice;
	return S_OK;
}

HRESULT Convert( D3D11_TEX1D_ARRAY_DSV &out, const D3D10_TEX1D_ARRAY_DSV &in )
{
	out.MipSlice = in.MipSlice;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}
HRESULT Convert( D3D10_TEX1D_ARRAY_DSV &out, const D3D11_TEX1D_ARRAY_DSV &in )
{
	out.MipSlice = in.MipSlice;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}
HRESULT Convert( D3D11_TEX2D_DSV &out, const D3D10_TEX2D_DSV &in )
{
	out.MipSlice = in.MipSlice;
	return S_OK;
}
HRESULT Convert( D3D10_TEX2D_DSV &out, const D3D11_TEX2D_DSV &in )
{
	out.MipSlice = in.MipSlice;
	return S_OK;
}
HRESULT Convert( D3D11_TEX2DMS_DSV &out, const D3D10_TEX2DMS_DSV &in )
{
	out.UnusedField_NothingToDefine = in.UnusedField_NothingToDefine;
	return S_OK;
}
HRESULT Convert( D3D10_TEX2DMS_DSV &out, const D3D11_TEX2DMS_DSV &in )
{
	out.UnusedField_NothingToDefine = in.UnusedField_NothingToDefine;
	return S_OK;
}

HRESULT Convert( D3D11_TEX2D_ARRAY_DSV &out, const D3D10_TEX2D_ARRAY_DSV &in )
{
	out.MipSlice = in.MipSlice;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}
HRESULT Convert( D3D10_TEX2D_ARRAY_DSV &out, const D3D11_TEX2D_ARRAY_DSV &in )
{
	out.MipSlice = in.MipSlice;
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}
HRESULT Convert( D3D11_TEX2DMS_ARRAY_DSV &out, const D3D10_TEX2DMS_ARRAY_DSV &in )
{
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}
HRESULT Convert( D3D10_TEX2DMS_ARRAY_DSV &out, const D3D11_TEX2DMS_ARRAY_DSV &in )
{
	out.FirstArraySlice = in.FirstArraySlice;
	out.ArraySize = in.ArraySize;
	return S_OK;
}

HRESULT Convert( D3D10_DEPTH_STENCIL_VIEW_DESC &out, const D3D11_DEPTH_STENCIL_VIEW_DESC &in )
{
	HRESULT result = S_OK;

	out.Format = in.Format;
	if( FAILED( Convert( out.ViewDimension, in.ViewDimension )) )
		return E_FAIL;
	// D3D11_DEPTH_STENCIL_VIEW_DESC::Flags must be zero, since these flags restrict writes to depth-stencil resources.
	if( in.Flags != 0 )
		return E_FAIL;

	switch( in.ViewDimension )
	{
	default:
		result = E_FAIL;
		break;
	case D3D11_DSV_DIMENSION_UNKNOWN:	//0
		break;
	case D3D11_DSV_DIMENSION_TEXTURE1D:	//2
		if( FAILED( Convert( out.Texture1D, in.Texture1D )) )
			result = E_FAIL;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:	
		if( FAILED( Convert( out.Texture1DArray, in.Texture1DArray )) )
			result = E_FAIL;
	case D3D11_DSV_DIMENSION_TEXTURE2D:	
		if( FAILED( Convert( out.Texture2D, in.Texture2D )) )
			result = E_FAIL;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:	//5
		if( FAILED( Convert( out.Texture2DArray, in.Texture2DArray )) )
			result = E_FAIL;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2DMS:	
		if( FAILED( Convert( out.Texture2DMS, in.Texture2DMS )) )
			result = E_FAIL;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:	
		if( FAILED( Convert( out.Texture2DMSArray, in.Texture2DMSArray )) )
			result = E_FAIL;
		break;
	}

	return result;
}
HRESULT Convert( D3D11_DEPTH_STENCIL_VIEW_DESC &out, const D3D10_DEPTH_STENCIL_VIEW_DESC &in )
{
	HRESULT result = S_OK;

	out.Format = in.Format;
	if( FAILED( Convert( out.ViewDimension, in.ViewDimension )) )
		return E_FAIL;

	switch( in.ViewDimension )
	{
	default:
		result = E_FAIL;
		break;
	case D3D10_DSV_DIMENSION_UNKNOWN:	//0
		break;
	case D3D10_DSV_DIMENSION_TEXTURE1D:	//2
		if( FAILED( Convert( out.Texture1D, in.Texture1D )) )
			result = E_FAIL;
		break;
	case D3D10_DSV_DIMENSION_TEXTURE1DARRAY:	
		if( FAILED( Convert( out.Texture1DArray, in.Texture1DArray )) )
			result = E_FAIL;
	case D3D10_DSV_DIMENSION_TEXTURE2D:	
		if( FAILED( Convert( out.Texture2D, in.Texture2D )) )
			result = E_FAIL;
		break;
	case D3D10_DSV_DIMENSION_TEXTURE2DARRAY:	//5
		if( FAILED( Convert( out.Texture2DArray, in.Texture2DArray )) )
			result = E_FAIL;
		break;
	case D3D10_DSV_DIMENSION_TEXTURE2DMS:	
		if( FAILED( Convert( out.Texture2DMS, in.Texture2DMS )) )
			result = E_FAIL;
		break;
	case D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY:	
		if( FAILED( Convert( out.Texture2DMSArray, in.Texture2DMSArray )) )
			result = E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_FILTER &out, D3D11_FILTER in )
{
	HRESULT result = S_OK;

	out = D3D10_FILTER_MIN_MAG_MIP_POINT;

	switch (in)
	{
	case D3D11_FILTER_MIN_MAG_MIP_POINT: out = D3D10_FILTER_MIN_MAG_MIP_POINT; break;
	case D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR: out = D3D10_FILTER_MIN_MAG_POINT_MIP_LINEAR; break;
	case D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT: out = D3D10_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT; break;
	case D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR: out = D3D10_FILTER_MIN_POINT_MAG_MIP_LINEAR; break;
	case D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT: out = D3D10_FILTER_MIN_LINEAR_MAG_MIP_POINT; break;
	case D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR: out = D3D10_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR; break;
	case D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT: out = D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT; break;
	case D3D11_FILTER_MIN_MAG_MIP_LINEAR: out = D3D10_FILTER_MIN_MAG_MIP_LINEAR; break;
	case D3D11_FILTER_ANISOTROPIC: out = D3D10_FILTER_ANISOTROPIC; break;
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT: out = D3D10_FILTER_COMPARISON_MIN_MAG_MIP_POINT; break;
	case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR: out = D3D10_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR; break;
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT: out = D3D10_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT; break;
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR: out = D3D10_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR; break;
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT: out = D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT; break;
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR: out = D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR; break;
	case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT: out = D3D10_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; break;
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR: out = D3D10_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; break;
	case D3D11_FILTER_COMPARISON_ANISOTROPIC: out = D3D10_FILTER_COMPARISON_ANISOTROPIC; break;
	default: result=E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_FILTER &out, D3D10_FILTER in )
{
	HRESULT result = S_OK;

	out = D3D11_FILTER_MIN_MAG_MIP_POINT;

	switch (in)
	{
	case D3D10_FILTER_MIN_MAG_MIP_POINT: out = D3D11_FILTER_MIN_MAG_MIP_POINT; break;
	case D3D10_FILTER_MIN_MAG_POINT_MIP_LINEAR: out = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR; break;
	case D3D10_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT: out = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT; break;
	case D3D10_FILTER_MIN_POINT_MAG_MIP_LINEAR: out = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR; break;
	case D3D10_FILTER_MIN_LINEAR_MAG_MIP_POINT: out = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT; break;
	case D3D10_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR: out = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR; break;
	case D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT: out = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; break;
	case D3D10_FILTER_MIN_MAG_MIP_LINEAR: out = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
	case D3D10_FILTER_ANISOTROPIC: out = D3D11_FILTER_ANISOTROPIC; break;
	case D3D10_FILTER_COMPARISON_MIN_MAG_MIP_POINT: out = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT; break;
	case D3D10_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR: out = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR; break;
	case D3D10_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT: out = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT; break;
	case D3D10_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR: out = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR; break;
	case D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT: out = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT; break;
	case D3D10_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR: out = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR; break;
	case D3D10_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT: out = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; break;
	case D3D10_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR: out = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; break;
	case D3D10_FILTER_COMPARISON_ANISOTROPIC: out = D3D11_FILTER_COMPARISON_ANISOTROPIC; break;
	default: result=E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_FILTER_TYPE &out, D3D10_FILTER_TYPE in )
{
	HRESULT result = S_OK;
	switch (in)
	{
	case D3D10_FILTER_TYPE_POINT: out = D3D11_FILTER_TYPE_POINT; break;
	case D3D10_FILTER_TYPE_LINEAR: out = D3D11_FILTER_TYPE_LINEAR; break;
	default: result=E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_FILTER_TYPE &out, D3D11_FILTER_TYPE in )
{
	HRESULT result = S_OK;
	switch (in)
	{
	case D3D11_FILTER_TYPE_POINT: out = D3D10_FILTER_TYPE_POINT; break;
	case D3D11_FILTER_TYPE_LINEAR: out = D3D10_FILTER_TYPE_LINEAR; break;
	default: result=E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_TEXTURE_ADDRESS_MODE &out, D3D10_TEXTURE_ADDRESS_MODE in )
{
	HRESULT result = S_OK;
	switch (in)
	{
	case D3D10_TEXTURE_ADDRESS_WRAP: out = D3D11_TEXTURE_ADDRESS_WRAP; break;
	case D3D10_TEXTURE_ADDRESS_MIRROR: out = D3D11_TEXTURE_ADDRESS_MIRROR; break;
	case D3D10_TEXTURE_ADDRESS_CLAMP: out = D3D11_TEXTURE_ADDRESS_CLAMP; break;
	case D3D10_TEXTURE_ADDRESS_BORDER: out = D3D11_TEXTURE_ADDRESS_BORDER; break;
	case D3D10_TEXTURE_ADDRESS_MIRROR_ONCE: out = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE; break;
	default: result=E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D10_TEXTURE_ADDRESS_MODE &out, D3D11_TEXTURE_ADDRESS_MODE in )
{
	HRESULT result = S_OK;
	switch (in)
	{
	case D3D11_TEXTURE_ADDRESS_WRAP: out = D3D10_TEXTURE_ADDRESS_WRAP; break;
	case D3D11_TEXTURE_ADDRESS_MIRROR: out = D3D10_TEXTURE_ADDRESS_MIRROR; break;
	case D3D11_TEXTURE_ADDRESS_CLAMP: out = D3D10_TEXTURE_ADDRESS_CLAMP; break;
	case D3D11_TEXTURE_ADDRESS_BORDER: out = D3D10_TEXTURE_ADDRESS_BORDER; break;
	case D3D11_TEXTURE_ADDRESS_MIRROR_ONCE: out = D3D10_TEXTURE_ADDRESS_MIRROR_ONCE; break;
	default: result=E_FAIL;
		break;
	}

	return result;
}

HRESULT Convert( D3D11_SAMPLER_DESC &out, const D3D10_SAMPLER_DESC &in )
{
	HRESULT result = S_OK;

	if( FAILED( Convert( out.Filter, in.Filter )))
		result = E_FAIL;
	if( FAILED( Convert( out.AddressU, in.AddressU )))
		result = E_FAIL;
	if( FAILED( Convert( out.AddressV, in.AddressV )))
		result = E_FAIL;
	if( FAILED( Convert( out.AddressW, in.AddressW )))
		result = E_FAIL;

	out.MipLODBias = in.MipLODBias;
	out.MaxAnisotropy = in.MaxAnisotropy;
	if( FAILED( Convert( out.ComparisonFunc, in.ComparisonFunc )))
		result = E_FAIL;
	for( int i=0; i<4; ++i )
		out.BorderColor[i] = in.BorderColor[i];
	out.MinLOD = in.MinLOD;
	out.MaxLOD = in.MaxLOD;

	return result;
}

HRESULT Convert( D3D10_SAMPLER_DESC &out, const D3D11_SAMPLER_DESC &in )
{
	HRESULT result = S_OK;

	if( FAILED( Convert( out.Filter, in.Filter )))
		result = E_FAIL;
	if( FAILED( Convert( out.AddressU, in.AddressU )))
		result = E_FAIL;
	if( FAILED( Convert( out.AddressV, in.AddressV )))
		result = E_FAIL;
	if( FAILED( Convert( out.AddressW, in.AddressW )))
		result = E_FAIL;

	out.MipLODBias = in.MipLODBias;
	out.MaxAnisotropy = in.MaxAnisotropy;
	if( FAILED( Convert( out.ComparisonFunc, in.ComparisonFunc )))
		result = E_FAIL;
	for( int i=0; i<4; ++i )
		out.BorderColor[i] = in.BorderColor[i];
	out.MinLOD = in.MinLOD;
	out.MaxLOD = in.MaxLOD;

	return result;
}

HRESULT Convert( D3D11_FORMAT_SUPPORT &out, D3D10_FORMAT_SUPPORT in )
{
	HRESULT result = S_OK; 
	switch (in)
	{
	case D3D10_FORMAT_SUPPORT_BUFFER: out = D3D11_FORMAT_SUPPORT_BUFFER; break;
	case D3D10_FORMAT_SUPPORT_IA_VERTEX_BUFFER: out = D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER; break;
	case D3D10_FORMAT_SUPPORT_IA_INDEX_BUFFER: out = D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER; break;
	case D3D10_FORMAT_SUPPORT_SO_BUFFER: out = D3D11_FORMAT_SUPPORT_SO_BUFFER; break;
	case D3D10_FORMAT_SUPPORT_TEXTURE1D: out = D3D11_FORMAT_SUPPORT_TEXTURE1D; break;
	case D3D10_FORMAT_SUPPORT_TEXTURE2D: out = D3D11_FORMAT_SUPPORT_TEXTURE2D; break;
	case D3D10_FORMAT_SUPPORT_TEXTURE3D: out = D3D11_FORMAT_SUPPORT_TEXTURE3D; break;
	case D3D10_FORMAT_SUPPORT_TEXTURECUBE: out = D3D11_FORMAT_SUPPORT_TEXTURECUBE; break;
	case D3D10_FORMAT_SUPPORT_SHADER_LOAD: out = D3D11_FORMAT_SUPPORT_SHADER_LOAD; break;
	case D3D10_FORMAT_SUPPORT_SHADER_SAMPLE: out = D3D11_FORMAT_SUPPORT_SHADER_SAMPLE; break;
	case D3D10_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON: out = D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON; break;
	case D3D10_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT: out = D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT; break;
	case D3D10_FORMAT_SUPPORT_MIP: out = D3D11_FORMAT_SUPPORT_MIP; break;
	case D3D10_FORMAT_SUPPORT_MIP_AUTOGEN: out = D3D11_FORMAT_SUPPORT_MIP_AUTOGEN; break;
	case D3D10_FORMAT_SUPPORT_RENDER_TARGET: out = D3D11_FORMAT_SUPPORT_RENDER_TARGET; break;
	case D3D10_FORMAT_SUPPORT_BLENDABLE: out = D3D11_FORMAT_SUPPORT_BLENDABLE; break;
	case D3D10_FORMAT_SUPPORT_DEPTH_STENCIL: out = D3D11_FORMAT_SUPPORT_DEPTH_STENCIL; break;
	case D3D10_FORMAT_SUPPORT_CPU_LOCKABLE: out = D3D11_FORMAT_SUPPORT_CPU_LOCKABLE; break;
	case D3D10_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE: out = D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE; break;
	case D3D10_FORMAT_SUPPORT_DISPLAY: out = D3D11_FORMAT_SUPPORT_DISPLAY; break;
	case D3D10_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT: out = D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT; break;
	case D3D10_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET: out = D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET; break;
	case D3D10_FORMAT_SUPPORT_MULTISAMPLE_LOAD: out = D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD; break;
	case D3D10_FORMAT_SUPPORT_SHADER_GATHER: out = D3D11_FORMAT_SUPPORT_SHADER_GATHER; break;
	case D3D10_FORMAT_SUPPORT_BACK_BUFFER_CAST: out = D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST; break;

	default: result=E_FAIL;
	}
	return result;
}

HRESULT Convert( D3D10_FORMAT_SUPPORT &out, D3D11_FORMAT_SUPPORT in )
{
	HRESULT result = S_OK;
	switch (in)
	{
	case D3D11_FORMAT_SUPPORT_BUFFER: out = D3D10_FORMAT_SUPPORT_BUFFER; break;
	case D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER: out = D3D10_FORMAT_SUPPORT_IA_VERTEX_BUFFER; break;
	case D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER: out = D3D10_FORMAT_SUPPORT_IA_INDEX_BUFFER; break;
	case D3D11_FORMAT_SUPPORT_SO_BUFFER: out = D3D10_FORMAT_SUPPORT_SO_BUFFER; break;
	case D3D11_FORMAT_SUPPORT_TEXTURE1D: out = D3D10_FORMAT_SUPPORT_TEXTURE1D; break;
	case D3D11_FORMAT_SUPPORT_TEXTURE2D: out = D3D10_FORMAT_SUPPORT_TEXTURE2D; break;
	case D3D11_FORMAT_SUPPORT_TEXTURE3D: out = D3D10_FORMAT_SUPPORT_TEXTURE3D; break;
	case D3D11_FORMAT_SUPPORT_TEXTURECUBE: out = D3D10_FORMAT_SUPPORT_TEXTURECUBE; break;
	case D3D11_FORMAT_SUPPORT_SHADER_LOAD: out = D3D10_FORMAT_SUPPORT_SHADER_LOAD; break;
	case D3D11_FORMAT_SUPPORT_SHADER_SAMPLE: out = D3D10_FORMAT_SUPPORT_SHADER_SAMPLE; break;
	case D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON: out = D3D10_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON; break;
	case D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT: out = D3D10_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT; break;
	case D3D11_FORMAT_SUPPORT_MIP: out = D3D10_FORMAT_SUPPORT_MIP; break;
	case D3D11_FORMAT_SUPPORT_MIP_AUTOGEN: out = D3D10_FORMAT_SUPPORT_MIP_AUTOGEN; break;
	case D3D11_FORMAT_SUPPORT_RENDER_TARGET: out = D3D10_FORMAT_SUPPORT_RENDER_TARGET; break;
	case D3D11_FORMAT_SUPPORT_BLENDABLE: out = D3D10_FORMAT_SUPPORT_BLENDABLE; break;
	case D3D11_FORMAT_SUPPORT_DEPTH_STENCIL: out = D3D10_FORMAT_SUPPORT_DEPTH_STENCIL; break;
	case D3D11_FORMAT_SUPPORT_CPU_LOCKABLE: out = D3D10_FORMAT_SUPPORT_CPU_LOCKABLE; break;
	case D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE: out = D3D10_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE; break;
	case D3D11_FORMAT_SUPPORT_DISPLAY: out = D3D10_FORMAT_SUPPORT_DISPLAY; break;
	case D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT: out = D3D10_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT; break;
	case D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET: out = D3D10_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET; break;
	case D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD: out = D3D10_FORMAT_SUPPORT_MULTISAMPLE_LOAD; break;
	case D3D11_FORMAT_SUPPORT_SHADER_GATHER: out = D3D10_FORMAT_SUPPORT_SHADER_GATHER; break;
	case D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST: out = D3D10_FORMAT_SUPPORT_BACK_BUFFER_CAST; break;
		// incompatible flags
	case D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW: 
	case D3D11_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON: 
	case D3D11_FORMAT_SUPPORT_DECODER_OUTPUT: 
	case D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT: 
	case D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT: 
		result = S_FALSE;
		break;
	default: result=E_FAIL;
	}
	return result;
}

HRESULT ConvertAsyncGetDataFlag11to10( UINT &out, UINT in )
{
	out = 0;

	if( in & D3D11_ASYNC_GETDATA_DONOTFLUSH )
		out |= D3D10_ASYNC_GETDATA_DONOTFLUSH;

	return S_OK;
}
HRESULT ConvertAsyncGetDataFlag10to11( UINT &out, UINT in )
{
	out = 0;

	if( in & D3D10_ASYNC_GETDATA_DONOTFLUSH )
		out |= D3D11_ASYNC_GETDATA_DONOTFLUSH;

	return S_OK;
}

HRESULT Convert( D3D11_QUERY &out, D3D10_QUERY in )
{
	HRESULT result = S_OK; 
	out = D3D11_QUERY_EVENT;
	switch (in)
	{
	case D3D10_QUERY_EVENT: out = D3D11_QUERY_EVENT; break;
	case D3D10_QUERY_OCCLUSION: out = D3D11_QUERY_OCCLUSION; break;
	case D3D10_QUERY_TIMESTAMP: out = D3D11_QUERY_TIMESTAMP; break;
	case D3D10_QUERY_TIMESTAMP_DISJOINT: out = D3D11_QUERY_TIMESTAMP_DISJOINT; break;
	case D3D10_QUERY_PIPELINE_STATISTICS: out = D3D11_QUERY_PIPELINE_STATISTICS; break;
	case D3D10_QUERY_OCCLUSION_PREDICATE: out = D3D11_QUERY_OCCLUSION_PREDICATE; break;
	case D3D10_QUERY_SO_STATISTICS: out = D3D11_QUERY_SO_STATISTICS; break;
	case D3D10_QUERY_SO_OVERFLOW_PREDICATE: out = D3D11_QUERY_SO_OVERFLOW_PREDICATE; break;
	default: result=E_FAIL;
	}
	return result;
}
HRESULT Convert( D3D10_QUERY &out, D3D11_QUERY in )
{
	HRESULT result = S_OK; 
	out = D3D10_QUERY_EVENT;
	switch (in)
	{
	case D3D11_QUERY_EVENT: out = D3D10_QUERY_EVENT; break;
	case D3D11_QUERY_OCCLUSION: out = D3D10_QUERY_OCCLUSION; break;
	case D3D11_QUERY_TIMESTAMP: out = D3D10_QUERY_TIMESTAMP; break;
	case D3D11_QUERY_TIMESTAMP_DISJOINT: out = D3D10_QUERY_TIMESTAMP_DISJOINT; break;
	case D3D11_QUERY_PIPELINE_STATISTICS: out = D3D10_QUERY_PIPELINE_STATISTICS; break;
	case D3D11_QUERY_OCCLUSION_PREDICATE: out = D3D10_QUERY_OCCLUSION_PREDICATE; break;
	case D3D11_QUERY_SO_STATISTICS: out = D3D10_QUERY_SO_STATISTICS; break;
	case D3D11_QUERY_SO_OVERFLOW_PREDICATE: out = D3D10_QUERY_SO_OVERFLOW_PREDICATE; break;
		// incompatible flags 
	case D3D11_QUERY_SO_STATISTICS_STREAM0: 
	case D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM0: 
	case D3D11_QUERY_SO_STATISTICS_STREAM1: 
	case D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM1: 
	case D3D11_QUERY_SO_STATISTICS_STREAM2: 
	case D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM2: 
	case D3D11_QUERY_SO_STATISTICS_STREAM3: 
	case D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM3: 
		result = S_FALSE;
		break;
	default: result=E_FAIL;
	}
	return result;
}

HRESULT ConvertQueryMiscFlag11to10( UINT &out, UINT in )
{
	out = 0;

	if( in & D3D11_QUERY_MISC_PREDICATEHINT )
		out |= D3D10_QUERY_MISC_PREDICATEHINT;

	return S_OK;
}
HRESULT ConvertQueryMiscFlag10to11( UINT &out, UINT in )
{
	out = 0;

	if( in & D3D10_QUERY_MISC_PREDICATEHINT )
		out |= D3D11_QUERY_MISC_PREDICATEHINT;

	return S_OK;
}
HRESULT Convert( D3D11_QUERY_DESC &out, const D3D10_QUERY_DESC &in )
{
	HRESULT result = S_OK;

	if( FAILED( Convert( out.Query, in.Query )))
		result = E_FAIL;
	if( FAILED( ConvertQueryMiscFlag10to11( out.MiscFlags, in.MiscFlags )))
		result = E_FAIL;

	return result;
}
HRESULT Convert( D3D10_QUERY_DESC &out, const D3D11_QUERY_DESC &in )
{
	HRESULT result = S_OK;

	if( FAILED( Convert( out.Query, in.Query )))
		result = E_FAIL;
	if( FAILED( ConvertQueryMiscFlag11to10( out.MiscFlags, in.MiscFlags )))
		result = E_FAIL;

	return result;
}

HRESULT Convert( D3D11_QUERY_DATA_TIMESTAMP_DISJOINT &out, const D3D10_QUERY_DATA_TIMESTAMP_DISJOINT &in )
{
	out.Frequency = in.Frequency;
	out.Disjoint = in.Disjoint;
	return S_OK;
}
HRESULT Convert( D3D10_QUERY_DATA_TIMESTAMP_DISJOINT &out, const D3D11_QUERY_DATA_TIMESTAMP_DISJOINT &in )
{
	out.Frequency = in.Frequency;
	out.Disjoint = in.Disjoint;
	return S_OK;
}

HRESULT Convert( D3D11_QUERY_DATA_PIPELINE_STATISTICS &out, const D3D10_QUERY_DATA_PIPELINE_STATISTICS &in )
{
	out.IAVertices = in.IAVertices;
	out.IAPrimitives = in.IAPrimitives;
	out.VSInvocations = in.VSInvocations;
	out.GSInvocations = in.GSInvocations;
	out.GSPrimitives = in.GSPrimitives;
	out.CInvocations = in.CInvocations;
	out.CPrimitives = in.CPrimitives;
	out.PSInvocations = in.PSInvocations;
	// D3D11 only stats
	out.HSInvocations = 0;
	out.DSInvocations = 0;
	out.CSInvocations = 0;
	return S_OK;
}
HRESULT Convert( D3D10_QUERY_DATA_PIPELINE_STATISTICS &out, const D3D11_QUERY_DATA_PIPELINE_STATISTICS &in )
{
	HRESULT result = S_OK;
	out.IAVertices = in.IAVertices;
	out.IAPrimitives = in.IAPrimitives;
	out.VSInvocations = in.VSInvocations;
	out.GSInvocations = in.GSInvocations;
	out.GSPrimitives = in.GSPrimitives;
	out.CInvocations = in.CInvocations;
	out.CPrimitives = in.CPrimitives;
	out.PSInvocations = in.PSInvocations;
	// D3D11 only stats
	if( in.HSInvocations != 0 || in.DSInvocations || in.CSInvocations )
		result = S_FALSE;
	return S_OK;
}

HRESULT Convert( D3D11_QUERY_DATA_SO_STATISTICS &out, const D3D10_QUERY_DATA_SO_STATISTICS &in )
{
	out.NumPrimitivesWritten = in.NumPrimitivesWritten;
	out.PrimitivesStorageNeeded = in.PrimitivesStorageNeeded;
	return S_OK;
}
HRESULT Convert( D3D10_QUERY_DATA_SO_STATISTICS &out, const D3D11_QUERY_DATA_SO_STATISTICS &in )
{
	out.NumPrimitivesWritten = in.NumPrimitivesWritten;
	out.PrimitivesStorageNeeded = in.PrimitivesStorageNeeded;
	return S_OK;
}

HRESULT Convert( D3D11_COUNTER &out, D3D10_COUNTER in )
{
	HRESULT result = S_OK; 
	switch (in)
	{
	case D3D10_COUNTER_DEVICE_DEPENDENT_0: out = D3D11_COUNTER_DEVICE_DEPENDENT_0; break;
		// Non convertable enums 
	case D3D10_COUNTER_GPU_IDLE: 
	case D3D10_COUNTER_VERTEX_PROCESSING: 
	case D3D10_COUNTER_GEOMETRY_PROCESSING: 
	case D3D10_COUNTER_PIXEL_PROCESSING: 
	case D3D10_COUNTER_OTHER_GPU_PROCESSING: 
	case D3D10_COUNTER_HOST_ADAPTER_BANDWIDTH_UTILIZATION: 
	case D3D10_COUNTER_LOCAL_VIDMEM_BANDWIDTH_UTILIZATION: 
	case D3D10_COUNTER_TRIANGLE_SETUP_THROUGHPUT_UTILIZATION: 
	case D3D10_COUNTER_FILLRATE_THROUGHPUT_UTILIZATION: 
	case D3D10_COUNTER_VS_MEMORY_LIMITED: 
	case D3D10_COUNTER_VS_COMPUTATION_LIMITED: 
	case D3D10_COUNTER_GS_MEMORY_LIMITED: 
	case D3D10_COUNTER_GS_COMPUTATION_LIMITED: 
	case D3D10_COUNTER_PS_MEMORY_LIMITED: 
	case D3D10_COUNTER_PS_COMPUTATION_LIMITED: 
	case D3D10_COUNTER_POST_TRANSFORM_CACHE_HIT_RATE: 
	case D3D10_COUNTER_TEXTURE_CACHE_HIT_RATE: 
		result = S_FALSE;
		break;
	default: result=E_FAIL;
	}
	return result;
}
HRESULT Convert( D3D10_COUNTER &out, D3D11_COUNTER in )
{
	HRESULT result = S_OK; 
	switch (in)
	{
	case D3D11_COUNTER_DEVICE_DEPENDENT_0: out = D3D10_COUNTER_DEVICE_DEPENDENT_0; break;
	default: result=E_FAIL;
	}
	return result;
}

HRESULT Convert( D3D11_COUNTER_TYPE &out, D3D10_COUNTER_TYPE in )
{
	HRESULT result = S_OK; 
	switch (in)
	{
	case D3D10_COUNTER_TYPE_FLOAT32: out = D3D11_COUNTER_TYPE_FLOAT32; break;
	case D3D10_COUNTER_TYPE_UINT16: out = D3D11_COUNTER_TYPE_UINT16; break;
	case D3D10_COUNTER_TYPE_UINT32: out = D3D11_COUNTER_TYPE_UINT32; break;
	case D3D10_COUNTER_TYPE_UINT64: out = D3D11_COUNTER_TYPE_UINT64; break;
	default: result=E_FAIL;
	}
	return result;
}
HRESULT Convert( D3D10_COUNTER_TYPE &out, D3D11_COUNTER_TYPE in )
{
	HRESULT result = S_OK; 
	switch (in)
	{
	case D3D11_COUNTER_TYPE_FLOAT32: out = D3D10_COUNTER_TYPE_FLOAT32; break;
	case D3D11_COUNTER_TYPE_UINT16: out = D3D10_COUNTER_TYPE_UINT16; break;
	case D3D11_COUNTER_TYPE_UINT32: out = D3D10_COUNTER_TYPE_UINT32; break;
	case D3D11_COUNTER_TYPE_UINT64: out = D3D10_COUNTER_TYPE_UINT64; break;
	default: result=E_FAIL;
	}
	return result;
}
HRESULT Convert( D3D11_COUNTER_DESC &out, const D3D10_COUNTER_DESC &in )
{
	HRESULT result = S_OK;
	if(FAILED(Convert(out.Counter, in.Counter)))
		result = E_FAIL;
	out.MiscFlags = in.MiscFlags;
	return result;
}
HRESULT Convert( D3D10_COUNTER_DESC &out, const D3D11_COUNTER_DESC &in )
{
	HRESULT result = S_OK;
	if(FAILED(Convert(out.Counter, in.Counter)))
		result = E_FAIL;
	out.MiscFlags = in.MiscFlags;
	return result;
	return S_OK;
}
HRESULT Convert( D3D11_COUNTER_INFO &out, const D3D10_COUNTER_INFO &in )
{
	HRESULT result = S_OK;
	if(FAILED(Convert(out.LastDeviceDependentCounter, in.LastDeviceDependentCounter)))
		result = E_FAIL;
	out.NumSimultaneousCounters = in.NumSimultaneousCounters;
	out.NumDetectableParallelUnits = in.NumDetectableParallelUnits;
	return result;
}
HRESULT Convert( D3D10_COUNTER_INFO &out, const D3D11_COUNTER_INFO &in )
{
	HRESULT result = S_OK;
	if(FAILED(Convert(out.LastDeviceDependentCounter, in.LastDeviceDependentCounter)))
		result = E_FAIL;
	out.NumSimultaneousCounters = in.NumSimultaneousCounters;
	out.NumDetectableParallelUnits = in.NumDetectableParallelUnits;
	return result;
}

HRESULT Convert( D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS &out, D3D10_STANDARD_MULTISAMPLE_QUALITY_LEVELS in )
{
	HRESULT result = S_OK; 
	switch (in)
	{
	case D3D10_STANDARD_MULTISAMPLE_PATTERN: out = D3D11_STANDARD_MULTISAMPLE_PATTERN; break;
	case D3D10_CENTER_MULTISAMPLE_PATTERN: out = D3D11_CENTER_MULTISAMPLE_PATTERN; break;
	default: result=E_FAIL;
	}
	return result;
}
HRESULT Convert( D3D10_STANDARD_MULTISAMPLE_QUALITY_LEVELS &out, D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS in )
{
	HRESULT result = S_OK; 
	switch (in)
	{
	case D3D11_STANDARD_MULTISAMPLE_PATTERN: out = D3D10_STANDARD_MULTISAMPLE_PATTERN; break;
	case D3D11_CENTER_MULTISAMPLE_PATTERN: out = D3D10_CENTER_MULTISAMPLE_PATTERN; break;
	default: result=E_FAIL;
	}
	return result;
}

HRESULT ConvertCreateDeviceFlag11to10( UINT &out, UINT in )
{
	HRESULT result = S_OK;
	out = 0;

	if( in & D3D11_CREATE_DEVICE_SINGLETHREADED )
		out |= D3D10_CREATE_DEVICE_SINGLETHREADED;
	if( in & D3D11_CREATE_DEVICE_DEBUG )
		out |= D3D10_CREATE_DEVICE_DEBUG;
	if( in & D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS )
		out |= D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
	if( in & D3D11_CREATE_DEVICE_BGRA_SUPPORT )
		out |= D3D10_CREATE_DEVICE_BGRA_SUPPORT;
	if( in & D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY )
		out |= D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY;
	// Incompatible enums 
	if( in & D3D11_CREATE_DEVICE_DEBUGGABLE )	// D3D11_CREATE_DEVICE_DEBUGGABLE	= 0x40
		out |= D3D11_CREATE_DEVICE_DEBUGGABLE;	// D3D10_CREATE_DEVICE_DEBUGGABLE	= 0x400

	return result;
}

HRESULT ConvertCreateDeviceFlag10to11( UINT &out, UINT in )
{
	HRESULT result = S_OK;
	out = 0;

	if( in & D3D10_CREATE_DEVICE_SINGLETHREADED )
		out |= D3D11_CREATE_DEVICE_SINGLETHREADED;
	if( in & D3D10_CREATE_DEVICE_DEBUG )
		out |= D3D11_CREATE_DEVICE_DEBUG;
	if( in & D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS )
		out |= D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
	if( in & D3D10_CREATE_DEVICE_BGRA_SUPPORT )
		out |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	// Incompatible enums 
	if( in & D3D10_CREATE_DEVICE_STRICT_VALIDATION )	// deprecated
		result = S_FALSE;
	if( in & D3D10_CREATE_DEVICE_ALLOW_NULL_FROM_MAP )
		result = S_FALSE;
	if( in & D3D10_CREATE_DEVICE_DEBUGGABLE ) // D3D10_CREATE_DEVICE_DEBUGGABLE	= 0x400
		out |= D3D10_CREATE_DEVICE_DEBUGGABLE;	// D3D11_CREATE_DEVICE_DEBUGGABLE	= 0x40

	return result;
}
}
#if 0
#endif


