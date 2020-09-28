#ifndef DEFINES_SET
	#define STRUCT_SIZE 4
	#define PIXEL_COUNT_X 80
	#define PIXEL_COUNT_Y 50
#endif

#define PIXEL_COUNT PIXEL_COUNT_X * PIXEL_COUNT_Y

struct AppendType
{
	uint pixelX : PixelX;
	uint pixelY : pixelY;
	uint pixelColCount : RowCount;
	uint data[STRUCT_SIZE] : TestArray;
};

Buffer<uint> dynamicAppendCounts;
AppendStructuredBuffer<AppendType> ux;

// buffer UAV has a randomized set of append counts.
float main( float4 pos : SV_Position ): SV_TARGET
{
	uint index = (pos.y - 0.5) * PIXEL_COUNT_X + (pos.x - 0.5);

	uint numAppends = dynamicAppendCounts[index];
	for( uint i = 0; i < numAppends; ++i )
	{
		AppendType at = (AppendType)0;
		at.pixelX = (pos.x - 0.5);
		at.pixelY = (pos.y - 0.5);
		at.pixelColCount = PIXEL_COUNT_X;
		at.data[0] = i;
		ux.Append( at );
	}
	return 1.0; //dummy output to the dummy rendertarget
}
