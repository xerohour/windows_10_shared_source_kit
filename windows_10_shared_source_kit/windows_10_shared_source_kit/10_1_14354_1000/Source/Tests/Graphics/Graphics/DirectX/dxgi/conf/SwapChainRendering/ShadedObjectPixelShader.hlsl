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
// Shader:  ShadedObjectPixelShader
//
// Purpose:
// Calcluates pixel color applying shading to an object
// using a simple wrap-shading technique.
///////////////////////////////////////////////////////
float4 ShadedObjectPixelShader
(
	VS_OUTPUT Input
) : SV_Target
{
	// Light position in world coordinates.
	float4 LightPosition = float4( 0.0, 4.0, -3.0, 1.0 );

	// Normalize the light position.
	float4 NormalizedLightPosition = normalize( LightPosition - Input.WorldPosition );

	// Calculate the output color for the pixel.
	// Start by scaling each channel of the input
	// color by the degree of alignment between
	// the light and the surface normal.
	float4 Diffuse = Input.Color * dot( NormalizedLightPosition, Input.Normal );

	// Now shift the color channels from the ranges [-1, 1]
	// to ranges [0, 1] and fix the alpha channel at 1.
	Diffuse.x = min( .5 * Diffuse.x + .5, 1 );
	Diffuse.y = min( .5 * Diffuse.y + .5, 1 );
	Diffuse.z = min( .5 * Diffuse.z + .5, 1 );
	Diffuse.w = 1.f;

	// Return the output color.
	return Diffuse;
}
