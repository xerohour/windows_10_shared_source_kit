struct VSIn
{
	float4 pos : POS;
	uint expectedId : ExpectedId;
	uint expectedIsFront : ExpectedIsFront;
};

struct VSOut
{
	float4 pos : SV_Position;
	uint expectedId : ExpectedId;
	uint expectedIsFront : ExpectedIsFront;
};

struct VSOutNoExpected
{
	float4 pos : SV_Position;
};


struct PSIn
{
	float4 pos : SV_Position;
	uint expectedId : ExpectedId;
	uint expectedIsFront : ExpectedIsFront;
	uint primId : SV_PrimitiveId;
};

struct PSInWithFrontFace
{
	float4 pos : SV_Position;
	uint expectedId : ExpectedId;
	uint expectedIsFront : ExpectedIsFront;
	uint primId : SV_PrimitiveId;
	uint isFrontFace : SV_IsFrontFace;
};

struct PSInNoExpected
{
	uint primId : SV_PrimitiveId;
};

struct PSOut
{
	uint4 color : SV_Target;
};

//
// Code
//

VSOut VS( VSIn input )
{
	return input; 
}

VSOutNoExpected VSNoExpected( VSIn input )
{
	VSOutNoExpected output;
	output.pos = input.pos;
	return output;
}


PSOut PS( PSIn input )
{
	PSOut output;
	
	output.color = uint4( 42, 163, input.primId, input.expectedId );
	
	if( input.primId == input.expectedId )
		output.color.x = 255;
	else
		output.color.x = 0;
		
	return output;
}

PSOut PSWithFrontFace( PSInWithFrontFace input )
{
	PSOut output;
	
	output.color = uint4( 42, 163, input.primId, input.expectedId );
	
	if( input.primId == input.expectedId )
		output.color.x = 255;
	else
		output.color.x = 0;

	if( input.isFrontFace == input.expectedIsFront )
		output.color.y = 255;
	else
		output.color.y = 0;

	return output;
}

PSOut PSNoExpected( PSInNoExpected input )
{
	PSOut output;
	
	output.color = uint4( 42, 163, 12, 211 );
	
	output.color.x = input.primId;
		
	return output;
}

