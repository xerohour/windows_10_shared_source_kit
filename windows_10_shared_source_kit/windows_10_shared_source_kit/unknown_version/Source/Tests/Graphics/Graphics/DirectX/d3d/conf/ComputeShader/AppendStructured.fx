#ifndef DEFINES_SET
	#define STRUCT_SIZE 4
	#define THREAD_COUNT_X 1024
	#define THREAD_COUNT_Y 1
	#define THREAD_COUNT_Z 1
	#define GROUP_COUNT_X 64
	#define GROUP_COUNT_Y 1
	#define GROUP_COUNT_Z 1
	#define APPEND_STORAGE_CLASS globallycoherent // should fail to compile
	#define USE_DYNAMIC_COUNTS 1
#endif

#define THREAD_COUNT THREAD_COUNT_X * THREAD_COUNT_Y * THREAD_COUNT_Z
#define GROUP_COUNT GROUP_COUNT_X * GROUP_COUNT_Y * GROUP_COUNT_Z
#define GROUP_AND_THREAD_SIZE GROUP_COUNT * THREAD_COUNT

struct AppendType
{
	uint groupIndex;
	uint threadIndex;
	uint threadCount;
	uint data[STRUCT_SIZE] : TestArray;
};

cbuffer gdata : register(b0)
{
	uint g_DispatchIndex;
};

Buffer<uint> dynamicAppendCounts;
/*APPEND_STORAGE_CLASS*/ AppendStructuredBuffer<AppendType> ux;

// buffer UAV has a randomized set of append counts.
[numthreads(THREAD_COUNT_X,THREAD_COUNT_Y,THREAD_COUNT_Z)]
void main( uint pid : SV_GroupIndex, uint3 gid : SV_GroupID )
{
	uint groupIndex = 
		gid.z * GROUP_COUNT_Y * GROUP_COUNT_X 
		+ gid.y * GROUP_COUNT_X
		+ gid.x;
	uint offset = groupIndex * THREAD_COUNT + GROUP_AND_THREAD_SIZE * g_DispatchIndex;
	uint index = pid + offset;

	uint numAppends = dynamicAppendCounts[index];
	for( uint i = 0; i < numAppends; ++i )
	{
		AppendType at = (AppendType)0;
		at.groupIndex = groupIndex + g_DispatchIndex * GROUP_COUNT;
		at.threadIndex = pid;
		at.threadCount = THREAD_COUNT;
		at.data[0] = i;
		ux.Append( at );
	}
}
