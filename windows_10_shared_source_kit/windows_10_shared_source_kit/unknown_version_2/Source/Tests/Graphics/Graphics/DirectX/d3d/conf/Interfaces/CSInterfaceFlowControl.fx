#ifndef DEFINES_SET
	// rosource width needs to be at least this long
	#define THREAD_COUNT_X 768
	#define THREAD_COUNT_Y 1
	#define THREAD_COUNT_Z 1
	#define GROUP_COUNT_X 64
	#define GROUP_COUNT_Y 1
	#define GROUP_COUNT_Z 1
#endif

#define THREAD_COUNT THREAD_COUNT_X * THREAD_COUNT_Y * THREAD_COUNT_Z
#define GROUP_COUNT GROUP_COUNT_X * GROUP_COUNT_Y * GROUP_COUNT_Z
#define GROUP_AND_THREAD_SIZE GROUP_COUNT * THREAD_COUNT
#define NUM_INTERFACE_SLOTS 253

interface ThreadTest
{
	uint Execute();
};

class Test0 : ThreadTest
{
	uint data;
	uint Execute()
	{
		return data;
	}
};

class Test1 : ThreadTest 
{
	uint data;
	uint moreData;
	uint Execute()
	{
		return data + moreData * 7 % 19;
	}
};

class Test2 : ThreadTest
{	
	uint data;
	uint Execute()
	{
		return data << 11 + 8;
	}
};

class Test3 : ThreadTest
{
	Buffer<uint> data;
	uint index;
	uint Execute()
	{
		return data[index];
	}
};

ThreadTest g_threadBodies[NUM_INTERFACE_SLOTS];
RWBuffer<uint> g_Results;

[numthreads(THREAD_COUNT_X,THREAD_COUNT_Y,THREAD_COUNT_Z)]
void main( uint pid : SV_GroupIndex )
{
	g_Results[pid] = g_threadBodies[pid % NUM_INTERFACE_SLOTS].Execute();
}
