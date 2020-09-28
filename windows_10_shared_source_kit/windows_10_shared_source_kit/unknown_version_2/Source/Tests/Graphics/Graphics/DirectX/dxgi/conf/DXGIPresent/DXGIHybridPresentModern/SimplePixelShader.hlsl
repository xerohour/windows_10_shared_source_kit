Texture2D myTex2D;
sampler samp;

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 texCoord : texCoord;
};

struct PixelShaderOutput
{
	float4 color : SV_TARGET;
};

PixelShaderOutput main(PixelShaderInput input)
{
	PixelShaderOutput output;
	output.color = myTex2D.Sample(samp, input.texCoord);
	return output;
}
