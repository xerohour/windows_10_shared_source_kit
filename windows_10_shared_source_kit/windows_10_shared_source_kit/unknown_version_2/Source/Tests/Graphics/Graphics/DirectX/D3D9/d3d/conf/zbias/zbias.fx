

struct PS_OUTPUT
{
	float4 col	: COLOR;
	float depth : DEPTH;
};


PS_OUTPUT changeDepth (float4 incolor : COLOR)
{
	PS_OUTPUT retVal;
	
	retVal.depth = 0.55f;
	retVal.col = incolor;
	
	return retVal;
}

technique ChangeDepthTech
{
	Pass P0
	{
		PixelShader = compile ps_2_0 changeDepth();
	}
}