#ifndef DEFINES_SET
	#define STRUCT_SIZE 4
	#define THREAD_COUNT_X 1024
	#define THREAD_COUNT_Y 1
	#define THREAD_COUNT_Z 1
	#define GROUP_COUNT_X 64
	#define GROUP_COUNT_Y 1
	#define GROUP_COUNT_Z 1
	#define CONSUME_STORAGE_CLASS globallycoherent // should fail to compile
	#define USE_DYNAMIC_COUNTS 1
#endif

#define THREAD_COUNT THREAD_COUNT_X * THREAD_COUNT_Y * THREAD_COUNT_Z
#define GROUP_COUNT GROUP_COUNT_X * GROUP_COUNT_Y * GROUP_COUNT_Z
#define GROUP_AND_THREAD_SIZE GROUP_COUNT * THREAD_COUNT

struct ConsumeType
{
	uint groupIndex;
	uint threadIndex;
	uint threadCount;
	uint data[STRUCT_SIZE] : TestArray;
};

cbuffer bGlobal : register(b0)
{
	uniform const uint g_MaxConsumes;
	uniform const uint g_MaxDynamicConsumes;
}

Buffer<uint> dynamicConsumeCounts; // must be randomized from the append version
RWBuffer<uint> totalCounts : register(u0); // must be cleared to zero before execution
/*CONSUME_STORAGE_CLASS*/ ConsumeStructuredBuffer<ConsumeType> ux;

// buffer UAV has a randomized set of consume counts.
[numthreads(THREAD_COUNT_X,THREAD_COUNT_Y,THREAD_COUNT_Z)]
void main( uint pid : SV_GroupIndex, uint3 gid : SV_GroupID )
{
	uint groupIndex = 
		gid.z * GROUP_COUNT_Y * GROUP_COUNT_X 
		+ gid.y * GROUP_COUNT_X
		+ gid.x;
	uint offset = groupIndex * THREAD_COUNT;
	uint index = pid + offset;

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
			// All out of bounds writes will be written to index 1 as an
			// error catch all.
			uint dest = ct.groupIndex * ct.threadCount + ct.threadIndex;
			dest = dest < GROUP_AND_THREAD_SIZE ? dest : 1;
			InterlockedAdd( totalCounts[dest], 1, value );
		}
	}
}
