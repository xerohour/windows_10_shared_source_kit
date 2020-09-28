// UAVBufferTyped.fx

#ifndef DEFINE_SET

	#define UAV_RWBUFFER_UINT 1
	#define BUFFER_BYTE_WIDTH_PER_THREAD 16 * 18 * 4
	#define NUMBER_OF_ELEMENT_PER_THREAD 100
	#define THREAD_COUNT_X 1
	#define THREAD_COUNT_Y 1
	#define THREAD_COUNT_Z 1
	#define DISPATCH_COUNT_X 1
	#define DISPATCH_COUNT_Y 1
	#define DISPATCH_COUNT_Z 1
	#define EXPECTED_RESULT_1 0
	#define EXPECTED_RESULT_2 1
	#define VIEWPORT_WIDTH 10
	#define VIEWPORT_HEIGHT 10
	#define PIXEL_SHADER_TEST 1
	
#endif

#ifdef UAV_RWBUFFER_UINT

	RWBuffer<uint> bufUAV;

#elif UAV_RWBUFFER_FLOAT

	RWBuffer<float> bufUAV;

#elif UAV_RWBUFFER_SINT

	RWBuffer<int> bufUAV;

#endif

#ifdef PIXEL_SHADER_TEST


#if PIXEL_SHADER_UAV_ONLY == 1
void PSMain( float4 Pos : SV_POSITION )
#else
float PSMain( float4 Pos : SV_POSITION ) : SV_TARGET
#endif
{
	uint offset = NUMBER_OF_ELEMENT_PER_THREAD * (( Pos.y - 0.5 ) * VIEWPORT_WIDTH + ( Pos.x - 0.5 ));
	uint endOfBuv = NUMBER_OF_ELEMENT_PER_THREAD * VIEWPORT_WIDTH * VIEWPORT_HEIGHT;

#elif COMPUTE_SHADER_TEST


[numthreads(THREAD_COUNT_X,THREAD_COUNT_Y,THREAD_COUNT_Z)]
void CSMain(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex)
{
	uint totalThreadsPerGroup = THREAD_COUNT_X * THREAD_COUNT_Y * THREAD_COUNT_Z;
	uint offset = NUMBER_OF_ELEMENT_PER_THREAD * (totalThreadsPerGroup * (groupID.z * DISPATCH_COUNT_X * DISPATCH_COUNT_Y + groupID.y * DISPATCH_COUNT_X + groupID.x) + threadIndex);
	uint endOfBuv = NUMBER_OF_ELEMENT_PER_THREAD * THREAD_COUNT_X * THREAD_COUNT_Y * THREAD_COUNT_Z * DISPATCH_COUNT_X * DISPATCH_COUNT_Y * DISPATCH_COUNT_Z;


#endif

	// Shader alternately writes two values into the UAV. Each thread writes into its 
	// own section of the UAV.  
	// Keep in mind that store_uav_typed always have a .xyzw mask, so each channel will have
	// the same value after each write.
	
	for ( uint i = 0; i < NUMBER_OF_ELEMENT_PER_THREAD; ++i )
	{
		if (i % 2 == 0)
		{
			bufUAV[offset + i] = EXPECTED_RESULT_1;
		}
		else
		{
			bufUAV[offset + i] = EXPECTED_RESULT_2;
		}
	}

	bufUAV[endOfBuv + offset] = 0; // out-of-bound write
	
#ifdef PIXEL_SHADER_TEST
#if PIXEL_SHADER_UAV_ONLY == 0
	return 1.0; //dummy output to the dummy rendertarget
#endif
#endif


}
