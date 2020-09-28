struct GSIn
{
	float4 pos : SV_Position;
	float2 tex : tex;
};
struct GSOut
{
	float4 pos : SV_Position;
	float2 tex : tex;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount (30)]
void main(inout TriangleStream<GSOut> Stream, triangle GSIn input[3])
{
	GSOut output;

	for (uint RTIndex = 0; RTIndex < 10; ++RTIndex)
	{
		output.RTIndex = RTIndex;
		
		for (uint iVertex = 0; iVertex < 3; ++iVertex)
		{
			output.pos = input[iVertex].pos;
			output.tex = input[iVertex].tex;
			
			Stream.Append(output);
		}
		
		Stream.RestartStrip();
	}
}