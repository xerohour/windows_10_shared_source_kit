// geometry shader to select render target array index
struct GSIn
{
	float4 pos		: SV_Position;
	float  odepth   : odepth;
};

struct GSOut
{
	float4 pos		 : SV_Position;
	float  odepth    : odepth;
	uint   rt_index  : SV_RenderTargetArrayIndex;
};

cbuffer cb0 : register( b0 )
{	
	float4		inv_vp;		    // inverse viewport
	float		default_depth;  // used when odepth is enabled
	uint		rt_index;		// render target array index
};

[maxvertexcount(3)]
void main(inout TriangleStream<GSOut> stream, triangle GSIn input[3])
{
	GSOut output;
	output.rt_index = rt_index;
	
	output.pos = input[0].pos;
	output.odepth = input[0].odepth;
	stream.Append(output);

	output.pos = input[1].pos;
	output.odepth = input[1].odepth;
	stream.Append(output);
	
	output.pos = input[2].pos;
	output.odepth = input[2].odepth;
	stream.Append(output);	
}