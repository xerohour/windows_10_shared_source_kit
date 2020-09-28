// RenderTargets Geometry Shader(s)

#define GS_DECL [maxvertexcount(6)]

struct GSIn
{
	float4 vPos	: SV_Position;
	float2 vTexCoords : texCoord;
};

struct GSOut
{
	float4	vPos : SV_Position;
	float2	vTexCoords : texCoord;
	uint	iRTIndex : SV_RenderTargetArrayIndex;
};

cbuffer cb0 : register( b0 )
{	
	float4		mxTrans;
	float4		junk1, junk2, junk3;
	float		fWidth;
	float		fHeight;
	uint		iRTIndex;
};
 
GS_DECL void GShaderMain( inout TriangleStream<GSOut> pStream, point GSIn input[ 1 ] )
{
	GSOut vertex;
	vertex.iRTIndex = iRTIndex;
	
	float texY = fHeight;
	if( fHeight == 1.f )
		texY = 0.f;
	
	// Top-right triangle of quad
	vertex.vPos = float4( fWidth, 0.f, 0.f, 1.f );
	vertex.vPos.xy = vertex.vPos.xy * mxTrans.zw + mxTrans.xy;
	vertex.vTexCoords = float2( fWidth, 0.f );
	pStream.Append( vertex );
	
	vertex.vPos = float4( fWidth, fHeight, 0.f, 1.f );
	vertex.vPos.xy = vertex.vPos.xy * mxTrans.zw + mxTrans.xy;
	vertex.vTexCoords = float2( fWidth, texY );
	pStream.Append( vertex );
	
	vertex.vPos = float4( 0.f, 0.f, 0.f, 1.f );
	vertex.vPos.xy = vertex.vPos.xy * mxTrans.zw + mxTrans.xy;
	vertex.vTexCoords = float2( 0.0f, 0.0f );
	pStream.Append( vertex );
	
	// Bottom-left triangle of quad
	vertex.vPos = float4( 0.f, fHeight, 0.f, 1.f );
	vertex.vPos.xy = vertex.vPos.xy * mxTrans.zw + mxTrans.xy;
	vertex.vTexCoords = float2( 0.f, texY );
	pStream.Append( vertex );	
}
				
