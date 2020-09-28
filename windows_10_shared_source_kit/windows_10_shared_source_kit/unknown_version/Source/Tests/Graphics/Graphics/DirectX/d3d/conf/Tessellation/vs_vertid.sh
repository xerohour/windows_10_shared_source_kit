struct VS_OUT
{
	float4  pos : pos;
	uint    vertID : vertexID;
};

VS_OUT  vs_main( uint vertexID : SV_VertexID )
{
	VS_OUT  output;
	
	output.pos = float4( -1, -1, 0, 1.0 );
	output.vertID = vertexID;
	
	if (vertexID == 0)
		output.pos.xy = float2( 0, 0 );

	if (vertexID == 1)
		output.pos.xy = float2( 1, 0 );

	if (vertexID == 2)
		output.pos.xy = float2( 0, 1 );

	if (vertexID == 3)
		output.pos.xy = float2( 1, 1 );


	return output;
}
