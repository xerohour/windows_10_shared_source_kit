///////////////////////////////////////////////////////
// Buffer:  cbPerObject
//
// Purpose:
// Constant buffer that holds world, view, and
// projection matrices.
///////////////////////////////////////////////////////
cbuffer cbPerObject : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
};


///////////////////////////////////////////////////////
// Structure:  VS_OUTPUT
//
// Purpose:
// Vertex shader output.
///////////////////////////////////////////////////////
struct VS_OUTPUT
{
	float4 Position: SV_POSITION;
	float4 WorldPosition: TEXCOORD0;
	float4 Normal: TEXCOORD1;
	float4 Color: COLOR0;
};


///////////////////////////////////////////////////////
// Shader:  ShadedObjectVertexShader
//
// Purpose:
// Processes vertices with a position, normal, and a
// fixed color.
///////////////////////////////////////////////////////
VS_OUTPUT ShadedObjectVertexShader
(
	float4 Position: POSITION,
	float4 Normal: NORMAL,
	float4 Color: COLOR
)
{
	VS_OUTPUT Output = (VS_OUTPUT) 0;

	Output.WorldPosition = mul( Position, World );

	Output.Position = Output.WorldPosition;
	Output.Position = mul( Output.Position, View );
	Output.Position = mul( Output.Position, Projection );

	Output.Normal = normalize( mul( Normal, World ) );

	Output.Color = Color;

	return Output;
}
