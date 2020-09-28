struct VSOut
{
    uint dummy : dum;
};

VSOut VSEntry()
{
	VSOut output = {1};
	return output;
}

cbuffer cb0 : register(b0)
{
	uint global_rtidx;
};

struct GSOut
{
    float4 pos   : SV_Position;
    float4 tex   : tex;
    uint   rtidx : SV_RenderTargetArrayIndex;
};

[maxvertexcount (4)]
void GSEntry( inout TriangleStream<GSOut> stream, point VSOut input[1] )
{
    GSOut v0 = { float4(-1.0f, -1.0f, 1.0f, 1.0f), float4(0.0f, 0.0f, 0.0f, 0.0f), global_rtidx };
    stream.Append(v0);
    
    GSOut v1 = { float4(-1.0f,  1.0f, 1.0f, 1.0f), float4(0.0f, 1.0f, 0.0f, 0.0f), global_rtidx };
    stream.Append(v1);
    
    GSOut v2 = { float4( 1.0f, -1.0f, 1.0f, 1.0f), float4(1.0f, 0.0f, 0.0f, 0.0f), global_rtidx };
    stream.Append(v2);
    
    GSOut v3 = { float4( 1.0f,  1.0f, 1.0f, 1.0f), float4(1.0f, 1.0f, 0.0f, 0.0f), global_rtidx };
    stream.Append(v3);
}

Texture1D tex1d;
Texture2D tex2d;
Texture3D tex3d;
SamplerState samp;

float4 PSEntry1D( GSOut input ) : SV_TARGET0
{
    return tex1d.SampleLevel( samp, input.tex, 0.0f ); // the last parameter is 0 to make the hardware always try to pick the largest mip level
}

float4 PSEntry2D( GSOut input ) : SV_TARGET0
{
    return tex2d.SampleLevel( samp, input.tex, 0.0f ); // the last parameter is 0 to make the hardware always try to pick the largest mip level
}

float4 PSEntry3D( GSOut input ) : SV_TARGET0
{
    return tex3d.SampleLevel( samp, input.tex, 0.0f ); // the last parameter is 0 to make the hardware always try to pick the largest mip level
}