// Vertex shader source for WGF11TiledResources

// Use this shader for Texture3D resources
#ifdef USE_TEXTURE3D
#   define UV_COORD_TYPE float3
#   define MAKE_UV_COORD(u, v) float3(u, v, 0.f)
#   define ASSIGN_UV(uvCoord, slice) float3(uvCoord.xy, slice)

// Use this shader for Texture2D resources
#else
#   define UV_COORD_TYPE float2
#   define MAKE_UV_COORD(u, v) float2(u, v)
#   define ASSIGN_UV(uvCoord, unused) float2(uvCoord)
#endif

cbuffer VS_CONST_INPUT : register(b0)
{
    // Position [0,1] of the quad to render relative to 3D volume
    // This is relevant only for Texture3D, while should be set
    // to 0 for Texture2D
    float quadSlicePosition = 0.f; // the slice
    uint numSlicesSRV = 64.f; // typically we use 64 slices total, so it's a good default
    uint numSlicesRTV = 64.f;
};

struct VS_OUTPUT
{
    float4               position     : SV_POSITION;
    UV_COORD_TYPE        uv           : TEXCOORD0;
    nointerpolation uint RTSlice      : RenderTargetArrayIndex;
    nointerpolation uint numSlicesSRV : NumSlicesSRV;
    nointerpolation uint numSlicesRTV : NumSlicesRTV;
};

static const float2 vertexPositions[4] =
{
    float2(-1.0, 1.0),
    float2(-1.0, -1.0),
    float2(1.0, 1.0),
    float2(1.0, -1.0)
};

static const UV_COORD_TYPE vertexUVCoords[4] =
{
    MAKE_UV_COORD(0.0, 0.0),
    MAKE_UV_COORD(0.0, 1.0),
    MAKE_UV_COORD(1.0, 0.0),
    MAKE_UV_COORD(1.0, 1.0)
};

VS_OUTPUT CopyTexVS(uint vertex : SV_VertexID)
{
    VS_OUTPUT output;
    output.position.xy = vertexPositions[vertex];
    output.position.zw = float2(quadSlicePosition, 1.0);
    output.uv = ASSIGN_UV(vertexUVCoords[vertex], quadSlicePosition);
    output.RTSlice = 0; // assigned in GS
    output.numSlicesSRV = numSlicesSRV;
    output.numSlicesRTV = numSlicesRTV;

    return output;
}
