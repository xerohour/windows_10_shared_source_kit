// PrimType.sh
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

struct VSIn
{
	uint4 data : data;
};

struct VSOut
{
	uint4 data : data;
};

struct GSIn
{
	uint4 data : data;
	uint PrimID : SV_PrimitiveID;
};

struct PSIn
{
	uint4 data;
};

uint Encode( uint i )
{
	return i + 1;
	//return ( i >> 1 ) ^ i;
}


VSOut VSPassThrough( VSIn input )
{
	VSOut output;
	
	output.data = input.data;
	
	return output;
};


VSOut VSChangeData( VSIn input )
{
	VSOut output;
	
	output.data = Encode( input.data );
	
	return output;
};

