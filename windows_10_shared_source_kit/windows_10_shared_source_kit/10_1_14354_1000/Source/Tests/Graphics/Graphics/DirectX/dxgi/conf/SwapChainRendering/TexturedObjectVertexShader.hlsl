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
// Shader:  TexturedObjectVertexShader
//
// Purpose:
// Trivial vertex shader that merely passes position
// and texture coordinates through.
///////////////////////////////////////////////////////
VS_OUTPUT TexturedObjectVertexShader
(
	float4 Position: POSITION,
	float2 TexCoord: TEXTURECOORD
)
{
	VS_OUTPUT Output = (VS_OUTPUT) 0;
	Output.Position = Position;
	Output.TexCoord = TexCoord;

	return Output;
}
