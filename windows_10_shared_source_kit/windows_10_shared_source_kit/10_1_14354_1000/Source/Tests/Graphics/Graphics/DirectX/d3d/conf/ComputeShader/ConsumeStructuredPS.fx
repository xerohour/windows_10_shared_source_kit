#ifndef DEFINES_SET
	#define STRUCT_SIZE 4
	#define PIXEL_COUNT_X 80
	#define PIXEL_COUNT_Y 50
#endif

#define PIXEL_COUNT PIXEL_COUNT_X * PIXEL_COUNT_Y

struct ConsumeType
{
	uint pixelX : PixelX;
	uint pixelY : pixelY;
	uint pixelColCount : ColCount;
	uint data[STRUCT_SIZE] : TestArray;
};

uniform const uint g_MaxConsumes;
uniform const uint g_MaxDynamicConsumes;

Buffer<uint> dynamicConsumeCounts; // must be randomized from the append version
RWBuffer<uint> totalCounts : register(u1); // must be cleared to zero before execution
ConsumeStructuredBuffer<ConsumeType> ux;

// buffer UAV has a randomized set of consume counts.
float main( float4 pos : SV_Position ): SV_TARGET
{
	uint index = (pos.y - 0.5) * PIXEL_COUNT_X + (pos.x - 0.5);

	if( index < g_MaxConsumes )
	{
		uint numConsumes = 1;
		if( index < g_MaxDynamicConsumes )
		{
			numConsumes = dynamicConsumeCounts[index];
		}
		
		for( uint i = 0; i < numConsumes; ++i )
		{
			ConsumeType ct = ux.Consume();
			uint value;
			// if ct is full of garbage, then the location we're checking in the buffer will
			// be out of bounds and should theirfor return zero.  So, all error reads will be written
			// to the [0] location of the result UAV.
			uint dest = ct.pixelY * ct.pixelColCount + ct.pixelX;
			dest = dest < PIXEL_COUNT ? dest : 1; // detect weird error.
			InterlockedAdd( totalCounts[dest], 1, value );
		}
	}
	return 1.0; //dummy output to the dummy rendertarget
}
