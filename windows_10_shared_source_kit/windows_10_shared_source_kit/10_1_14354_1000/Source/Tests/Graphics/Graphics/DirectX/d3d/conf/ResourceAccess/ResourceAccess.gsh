// ResourceAccess Geometry Shader

#define GS_DECL [maxvertexcount(6)]

struct GSIn
{
	float4 vPos	: SV_Position;
	float2 vTexCoords : texCoord;
};

struct GSOut
{
	float4 vPos : SV_Position;
	float2 vTexCoords : texCoord;
};

cbuffer cb0 : register( b0 )
{	
	float4		mxTrans;
};
 
GS_DECL void GShaderMain( inout TriangleStream<GSOut> pStream, point GSIn input[ 1 ] )
{
	GSOut vertex;
		
	// Top-right triangle of quad
	vertex.vPos = float4( 1.f, 0.f, 1.f, 1.f );
	vertex.vPos.xy = vertex.vPos.xy * mxTrans.zw + mxTrans.xy;
	vertex.vTexCoords = float2( 1.f, 0.f );
	pStream.Append( vertex );
	
	vertex.vPos = float4( 1.f, 1.f, 1.f, 1.f );
	vertex.vPos.xy = vertex.vPos.xy * mxTrans.zw + mxTrans.xy;
	vertex.vTexCoords = float2( 1.f, 1.f );
	pStream.Append( vertex );
	
	vertex.vPos = float4( 0.f, 0.f, 1.f, 1.f );
	vertex.vPos.xy = vertex.vPos.xy * mxTrans.zw + mxTrans.xy;
	vertex.vTexCoords = float2( 0.0f, 0.0f );
	pStream.Append( vertex );
	
	// Bottom-left triangle of quad
	vertex.vPos = float4( 0.f, 1.f, 1.f, 1.f );
	vertex.vPos.xy = vertex.vPos.xy * mxTrans.zw + mxTrans.xy;
	vertex.vTexCoords = float2( 0.f, 1.f );
	pStream.Append( vertex );	
}
				