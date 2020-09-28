Texture2D DiffuseTexture;
SamplerState DiffuseTextureSampler;


///////////////////////////////////////////////////////
// Structure:  VS_OUTPUT
//
// Purpose:
// Vertex shader output.
///////////////////////////////////////////////////////
struct VS_OUTPUT
{
	float4 Position: SV_POSITION;
	float2 TexCoord: TEXCOORD0;
};


///////////////////////////////////////////////////////
// Shader:  TexturedObjectPixelShader
//
// Purpose:
// Pixel shader that calculates pixel colors by doing
// trivial UV sampling of a texture.
///////////////////////////////////////////////////////
float4 TexturedObjectPixelShader
(
	VS_OUTPUT Input
) : SV_Target
{
	return DiffuseTexture.Sample( DiffuseTextureSampler, Input.TexCoord );
}
