// RenderTargets Vertex Shader(s)

struct VSIn
{
	float2 vPos	: pos;
	float4 vTexCoords : texCoord;
};

struct VSOut
{
	//float2 vPos : pos;
	float4 vPos : SV_Position;
	float2 vTexCoords : texCoord;
};

cbuffer cb0 : register( b0 )
{	
	float4		mxTrans;
	//float4x4	mxTrans;
	//float		fWidth;
	//float		fHeight;
};

VSOut VShaderMain( VSIn input )
{
	VSOut output;
	
	output.vPos = float4( 0.f, 0.f, 0.f, 1.f );
	output.vTexCoords = float2( 0.f, 0.f );
	
	//output.vPos = float4( 0.f, 0.f, 0.f, 1.f );
	//output.vPos.xy = input.vPos * mxTrans.zw + mxTrans.xy;
	//output.vTexCoords = float2( input.vTexCoords.x, input.vTexCoords.y );
	
	return output;
}