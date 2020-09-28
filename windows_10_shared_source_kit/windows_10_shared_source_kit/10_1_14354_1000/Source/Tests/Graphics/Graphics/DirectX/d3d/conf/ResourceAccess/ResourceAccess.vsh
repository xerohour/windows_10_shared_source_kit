// ResourceAccess Vertex Shader


struct VSIn
{
	float2 vPos	: pos;
	float4 vTexCoords : texCoord;
};

struct VSOut
{
	float4 vPos : SV_Position;
	float2 vTexCoords : texCoord;
};

cbuffer cb0 : register( b0 )
{	
	float4		mxTrans;
};

VSOut VShaderMain( VSIn input )
{
	VSOut output;
	
	// just to slow down processing enough to get DXGI_ERROR_WAS_STILL_DRAWING
	for( int i = 0; i < 300; i++ )
	{
		output.vPos = float4( 0.f, 0.f, 0.f, 1.f );
		output.vTexCoords = float2( 0.f, 0.f );
	}
	
	return output;
}