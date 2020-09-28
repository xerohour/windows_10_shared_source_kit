// Resources
Texture2D<uint> srcTex2D;

// Vertex Shader
struct VSIn
{
	float2 pos : pos;
	float2 texCoord : texCoord;
};

struct VSOut
{
	float4 pos : SV_Position;
	float2 texCoord : TEXCOORD0;
};

VSOut VSMain(VSIn input)
{
	VSOut output;
	
	output.pos = float4( 0, 0, 0, 1.0 );
	output.pos.xy = input.pos.xy;

	output.texCoord = input.texCoord;

	return output;
}

// Pixel Shader
struct PSOut
{
	uint4 color[8] : SV_Target;
};

PSOut PSMain(VSOut input)
{
	PSOut output;

	// Convert the texCoord [0-1] to a sample position index [0-width/height]
	uint2 index = input.texCoord.xy * srcTex2D.Length.xy;
	uint color = srcTex2D[uint2(index.x, index.y)];

	for( uint i = 0; i < 8; i++ )
	{
		// The src texture is R32_UINT, replicate that accross all components
		// Let the HW handle the format conversion
		output.color[i] = uint4(color, color, color, color);
	}

	return output;
}

PSOut PSMainInitRT(VSOut input)
{
	PSOut output;

	// Note the swizzled coordinates
	// This is so the columns and rows switch.
	// E.g. Row from src texture becomes the column of the RT

	// Convert the texCoord [0-1] to a sample position index [0-width/height]
	uint2 index = input.texCoord.xy * srcTex2D.Length.xy;
	uint color = srcTex2D[uint2(index.y, index.x)];

	for( uint i = 0; i < 8; i++ )
	{
		// The src texture is R32_UINT, replicate that accross all components
		// Let the HW handle the format conversion
		output.color[i] = uint4(color, color, color, color);
	}

	return output;
}
