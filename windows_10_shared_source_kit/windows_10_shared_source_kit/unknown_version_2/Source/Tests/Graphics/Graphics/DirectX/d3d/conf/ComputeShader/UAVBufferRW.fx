// UAVBufferRW.fx

#ifndef DEFINE_SET

	#define UAV_RWBYTEADDRESSBUFFER 1
	#define BUFFER_BYTE_WIDTH_PER_THREAD 16 * 18 * 4
	#define THREAD_COUNT_X 1
	#define THREAD_COUNT_Y 1
	#define THREAD_COUNT_Z 1
	#define DISPATCH_COUNT_X 1
	#define DISPATCH_COUNT_Y 1
	#define DISPATCH_COUNT_Z 1
	#define VIEWPORT_WIDTH 10
	#define VIEWPORT_HEIGHT 10
	#define PIXEL_SHADER_TEST 1
	#define PIXEL_SHADER_UAV_ONLY 1
	#define DEFAULT_BUFFER_SIZE 1024
	
#endif

#ifdef UAV_RWBYTEADDRESSBUFFER
	
	#define COMP_1_TYPE uint
	#define COMP_2_TYPE uint2
	#define COMP_3_TYPE uint3
	#define COMP_4_TYPE uint4

	#define STORE_1_TEST( _uav, _index, _value ) _uav.Store( (_index) * 4, _value )
	#define STORE_2_TEST( _uav, _index, _value ) _uav.Store2( (_index) * 4, _value )
	#define STORE_3_TEST( _uav, _index, _value ) _uav.Store3( (_index) * 4, _value )
	#define STORE_4_TEST( _uav, _index, _value ) _uav.Store4( (_index) * 4, _value )

	#define LOAD_1_TEST( _uav, _index, _value ) _value = _uav.Load( (_index) * 4 )
	#define LOAD_2_TEST( _uav, _index, _value ) _value = _uav.Load2( (_index) * 4 )
	#define LOAD_3_TEST( _uav, _index, _value ) _value = _uav.Load3( (_index) * 4 )
	#define LOAD_4_TEST( _uav, _index, _value ) _value = _uav.Load4( (_index) * 4 )

	RWByteAddressBuffer bufUAV;

#elif UAV_RWSTRUCTUREDBUFFER_4UINT

	#define COMP_1_TYPE uint
	#define COMP_2_TYPE uint2
	#define COMP_3_TYPE uint3
	#define COMP_4_TYPE uint4
	
	#define STORE_1_TEST( _uav, _index, _value ) _uav[(_index) / 4].a = _value;
	#define STORE_2_TEST( _uav, _index, _value ) _uav[(_index) / 4].a = _value.x; _uav[(_index) / 4].b = _value.y;
	#define STORE_3_TEST( _uav, _index, _value ) _uav[(_index) / 4].a = _value.x; _uav[(_index) / 4].b = _value.y; _uav[(_index) / 4].c.x = _value.z;
	#define STORE_4_TEST( _uav, _index, _value ) _uav[(_index) / 4].a = _value.x; _uav[(_index) / 4].b = _value.y; _uav[(_index) / 4].c.x = _value.z; _uav[(_index) / 4].c.y = _value.w;

	#define LOAD_1_TEST( _uav, _index, _value ) _value = _uav[(_index) / 4].a;
	#define LOAD_2_TEST( _uav, _index, _value ) _value.x = _uav[(_index) / 4].a; _value.y = _uav[(_index) / 4].b;
	#define LOAD_3_TEST( _uav, _index, _value ) _value.x = _uav[(_index) / 4].a; _value.y = _uav[(_index) / 4].b; _value.z = _uav[(_index) / 4].c.x;
	#define LOAD_4_TEST( _uav, _index, _value ) _value.x = _uav[(_index) / 4].a; _value.y = _uav[(_index) / 4].b; _value.z = _uav[(_index) / 4].c.x; _value.w = _uav[(_index) / 4].c.y;

	struct UIntGroup
	{
		uint a;
		uint b;
		uint2 c;
	};
	
	RWStructuredBuffer<UIntGroup> bufUAV;	

#elif UAV_RWSTRUCTUREDBUFFER_2SINT

	#define COMP_1_TYPE int
	#define COMP_2_TYPE int2
	#define COMP_3_TYPE int3
	#define COMP_4_TYPE int4
	
	#define STORE_1_TEST( _uav, _index, _value ) _uav[(_index) / 2].a = _value;
	#define STORE_2_TEST( _uav, _index, _value ) _uav[(_index) / 2].a = _value.x; _uav[(_index) / 2].b = _value.y;
	#define STORE_3_TEST( _uav, _index, _value ) _uav[(_index) / 2].a = _value.x; _uav[(_index) / 2].b = _value.y; _uav[(_index) / 2 + 1].a = _value.z;
	#define STORE_4_TEST( _uav, _index, _value ) _uav[(_index) / 2].a = _value.x; _uav[(_index) / 2].b = _value.y; _uav[(_index) / 2 + 1].a = _value.z; _uav[(_index) / 2 + 1].b = _value.w;

	#define LOAD_1_TEST( _uav, _index, _value ) _value = _uav[(_index) / 2].a;
	#define LOAD_2_TEST( _uav, _index, _value ) _value.x = _uav[(_index) / 2].a; _value.y = _uav[(_index) / 2].b;
	#define LOAD_3_TEST( _uav, _index, _value ) _value.x = _uav[(_index) / 2].a; _value.y = _uav[(_index) / 2].b; _value.z = _uav[(_index) / 2 + 1].a;
	#define LOAD_4_TEST( _uav, _index, _value ) _value.x = _uav[(_index) / 2].a; _value.y = _uav[(_index) / 2].b; _value.z = _uav[(_index) / 2 + 1].a; _value.w = _uav[(_index) / 2 + 1].b;

	struct SIntGroup
	{
		int a;
		int b;
	};
	
	RWStructuredBuffer<SIntGroup> bufUAV;	

#elif UAV_RWSTRUCTUREDBUFFER_1FLOAT

	#define COMP_1_TYPE float
	#define COMP_2_TYPE float2
	#define COMP_3_TYPE float3
	#define COMP_4_TYPE float4
	
	#define STORE_1_TEST( _uav, _index, _value ) _uav[_index].a = _value;
	#define STORE_2_TEST( _uav, _index, _value ) _uav[_index].a = _value.x; _uav[_index+1].a = _value.y;
	#define STORE_3_TEST( _uav, _index, _value ) _uav[_index].a = _value.x; _uav[_index+1].a = _value.y; _uav[_index+2].a = _value.z;
	#define STORE_4_TEST( _uav, _index, _value ) _uav[_index].a = _value.x; _uav[_index+1].a = _value.y; _uav[_index+2].a = _value.z; _uav[_index+3].a = _value.w;

	#define LOAD_1_TEST( _uav, _index, _value ) _value = _uav[_index].a;
	#define LOAD_2_TEST( _uav, _index, _value ) _value.x = _uav[_index].a; _value.y = _uav[_index+1].a;
	#define LOAD_3_TEST( _uav, _index, _value ) _value.x = _uav[_index].a; _value.y = _uav[_index+1].a; _value.z = _uav[_index+2].a;
	#define LOAD_4_TEST( _uav, _index, _value ) _value.x = _uav[_index].a; _value.y = _uav[_index+1].a; _value.z = _uav[_index+2].a; _value.w = _uav[_index+3].a;

	struct FloatGroup
	{
		float a;
	};
	
	RWStructuredBuffer<FloatGroup> bufUAV;	

#elif UAV_RWBUFFER_UINT

	#define COMP_1_TYPE uint
	#define COMP_2_TYPE uint2
	#define COMP_3_TYPE uint3
	#define COMP_4_TYPE uint4

	#define STORE_1_TEST( _uav, _index, _value ) _uav[_index] = _value;
	#define STORE_2_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y;
	#define STORE_3_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y; _uav[_index + 2] = _value.z;
	#define STORE_4_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y; _uav[_index + 2] = _value.z; _uav[_index + 3] = _value.w;
	
	#define LOAD_1_TEST( _uav, _index, _value ) _value = _uav[_index];
	#define LOAD_2_TEST( _uav, _index, _value ) _value.x = _uav[_index]; _value.y = _uav[_index+1];
	#define LOAD_3_TEST( _uav, _index, _value ) _value.x = _uav[_index]; _value.y = _uav[_index+1]; _value.z = _uav[_index+2];
	#define LOAD_4_TEST( _uav, _index, _value ) _value.x = _uav[_index]; _value.y = _uav[_index+1]; _value.z = _uav[_index+2]; _value.w = _uav[_index+3];

	RWBuffer<uint> bufUAV;

#elif UAV_RWBUFFER_UINT2

	// load is not supported for multi-component UINT.
	// And xyzw are implicitly masked.

	#define COMP_1_TYPE uint
	#define COMP_2_TYPE uint2
	#define COMP_3_TYPE uint3
	#define COMP_4_TYPE uint4

	#define STORE_1_TEST( _uav, _index, _value ) _uav[_index] = _value;
	#define STORE_2_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y;
	#define STORE_3_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y; _uav[_index + 2] = _value.z;
	#define STORE_4_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y; _uav[_index + 2] = _value.z; _uav[_index + 3] = _value.w;

	#define LOAD_1_TEST( _uav, _index, _value ) _value = 0;
	#define LOAD_2_TEST( _uav, _index, _value ) _value = 0; 
	#define LOAD_3_TEST( _uav, _index, _value ) _value.x = 0; _value.y = 0; _value.z = 0;
	#define LOAD_4_TEST( _uav, _index, _value ) _value.x = 0; _value.y = 0; _value.z = 0; _value.w = 0;

	RWBuffer<uint2> bufUAV;

#elif UAV_RWBUFFER_FLOAT

	#define COMP_1_TYPE float
	#define COMP_2_TYPE float2
	#define COMP_3_TYPE float3
	#define COMP_4_TYPE float4

	#define STORE_1_TEST( _uav, _index, _value ) _uav[_index] = _value;
	#define STORE_2_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y;
	#define STORE_3_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y; _uav[_index + 2] = _value.z;
	#define STORE_4_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y; _uav[_index + 2] = _value.z; _uav[_index + 3] = _value.w;
	
	#define LOAD_1_TEST( _uav, _index, _value ) _value = _uav[_index];
	#define LOAD_2_TEST( _uav, _index, _value ) _value.x = _uav[_index]; _value.y = _uav[_index+1];
	#define LOAD_3_TEST( _uav, _index, _value ) _value.x = _uav[_index]; _value.y = _uav[_index+1]; _value.z = _uav[_index+2];
	#define LOAD_4_TEST( _uav, _index, _value ) _value.x = _uav[_index]; _value.y = _uav[_index+1]; _value.z = _uav[_index+2]; _value.w = _uav[_index+3];

	RWBuffer<float> bufUAV;

#elif UAV_RWBUFFER_FLOAT4

	#define COMP_1_TYPE float
	#define COMP_2_TYPE float2
	#define COMP_3_TYPE float3
	#define COMP_4_TYPE float4

	#define STORE_1_TEST( _uav, _index, _value ) _uav[_index] = _value;
	#define STORE_2_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y;
	#define STORE_3_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y; _uav[_index + 2] = _value.z;
	#define STORE_4_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y; _uav[_index + 2] = _value.z; _uav[_index + 3] = _value.w;

	#define LOAD_1_TEST( _uav, _index, _value ) _value = 0;
	#define LOAD_2_TEST( _uav, _index, _value ) _value = 0; 
	#define LOAD_3_TEST( _uav, _index, _value ) _value.x = 0; _value.y = 0; _value.z = 0;
	#define LOAD_4_TEST( _uav, _index, _value ) _value.x = 0; _value.y = 0; _value.z = 0; _value.w = 0;

	RWBuffer<float4> bufUAV;

#elif UAV_RWBUFFER_SINT

	#define COMP_1_TYPE int
	#define COMP_2_TYPE int2
	#define COMP_3_TYPE int3
	#define COMP_4_TYPE int4

	#define STORE_1_TEST( _uav, _index, _value ) _uav[_index] = _value;
	#define STORE_2_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y;
	#define STORE_3_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y; _uav[_index + 2] = _value.z;
	#define STORE_4_TEST( _uav, _index, _value ) _uav[_index] = _value.x; _uav[_index + 1] = _value.y; _uav[_index + 2] = _value.z; _uav[_index + 3] = _value.w;

	#define LOAD_1_TEST( _uav, _index, _value ) _value = _uav[_index];
	#define LOAD_2_TEST( _uav, _index, _value ) _value.x = _uav[_index]; _value.y = _uav[_index+1];
	#define LOAD_3_TEST( _uav, _index, _value ) _value.x = _uav[_index]; _value.y = _uav[_index+1]; _value.z = _uav[_index+2];
	#define LOAD_4_TEST( _uav, _index, _value ) _value.x = _uav[_index]; _value.y = _uav[_index+1]; _value.z = _uav[_index+2]; _value.w = _uav[_index+3];
	
	RWBuffer<int> bufUAV;

#elif UAV_RWSTRUCTUREDBUFFER_MIX

	struct MixGroup
	{
		bool b1;
		float f1;
		float3 f2;
		bool4 b2;
		int i1;
		uint2 u;
		int i2;
		int3 i3;
	};	
	
	RWStructuredBuffer<MixGroup> bufUAV;

#endif

// macros to help prevent code from being optimized out. g_inputBuffer should be all zero.
#define PREVENT_OPT_START_BLOCK( i ) i = ( i + 4 ) % DEFAULT_BUFFER_SIZE; if ( g_inputBuffer.Load( i ) == 0 ) { 
#define PREVENT_OPT_END_BLOCK }



ByteAddressBuffer g_inputBuffer; // all zeros

#ifdef PIXEL_SHADER_TEST

#if PIXEL_SHADER_UAV_ONLY == 1
void PSMain( float4 Pos : SV_POSITION )
#else
float PSMain( float4 Pos : SV_POSITION ) : SV_TARGET
#endif
{
	uint dwordPerThread = BUFFER_BYTE_WIDTH_PER_THREAD / 4;
	uint offset = dwordPerThread * (( Pos.y - 0.5 ) * VIEWPORT_WIDTH + ( Pos.x - 0.5 ));
	uint endOfUAV = dwordPerThread * VIEWPORT_WIDTH * VIEWPORT_HEIGHT;

#elif COMPUTE_SHADER_TEST

[numthreads(THREAD_COUNT_X,THREAD_COUNT_Y,THREAD_COUNT_Z)]
void CSMain(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex)
{
	// Test increments what already exists in the UAV by the following pattern per thread.
	// pattern:	1,0,0,0,    5,6,0,0,    9,10,11,0,    13,14,15,16
	//          17,0,0,0,   21,22,0,0   25,26,27,0,   29,30,31,32 ...
	//
	// e.g. if the 1st integer is originally 6, it becomes 6+1==7.  If the 17th int is 8, it becomse 8+17==25.
	

	// calculate offset value for this thread to read from and write to 
	uint dwordPerThread = BUFFER_BYTE_WIDTH_PER_THREAD / 4;
	uint totalThreadsPerGroup = THREAD_COUNT_X * THREAD_COUNT_Y * THREAD_COUNT_Z;
	uint totalGroup = DISPATCH_COUNT_X * DISPATCH_COUNT_Y * DISPATCH_COUNT_Z;
	uint offset = dwordPerThread * totalThreadsPerGroup * (groupID.z * DISPATCH_COUNT_X * DISPATCH_COUNT_Y + groupID.y * DISPATCH_COUNT_X + groupID.x) + dwordPerThread * threadIndex;
	uint endOfUAV = dwordPerThread * totalThreadsPerGroup * totalGroup;

#endif

	uint noOpt = 0; // used to help prevent compiler opt.
	PREVENT_OPT_START_BLOCK(noOpt); // first block not really needed - just so I'd know if something is massively wrong with this opt. check feature

#ifdef UAV_RWSTRUCTUREDBUFFER_MIX

	// this structure is too complicated to try the general test I wrote in the else block...writing a separate test for it.
	for ( uint i = 0; i < dwordPerThread / 16; ++i )
	{
		uint mod = i % 16;
		int index = offset / 16 + i;
		
		// really long ugly block, but don't see any nicer way....
		
		bufUAV[index].b1 = bufUAV[index].b1 ? false : true; // bool doesn't go well with the pattern either....i guess we'll just flip it
		bufUAV[index].f2.z = bufUAV[index].f2.z + i * 16 + 5;
		bufUAV[index].b2.x = bufUAV[index].b2.x ? false : true;
		bufUAV[index].b2.w = bufUAV[index].b2.w ? false : true;
		bufUAV[index].i1 = bufUAV[index].i1 + i * 16 + 10;
		bufUAV[index].u.x = bufUAV[index].u.x + i * 16 + 11;
		bufUAV[index].i2 = bufUAV[index].i2 + i * 16 + 13;
		bufUAV[index].i3.x = bufUAV[index].i3.x + i * 16 + 14;
		bufUAV[index].i3.y = bufUAV[index].i3.y + i * 16 + 15;
		bufUAV[index].i3.z = bufUAV[index].i3.z + i * 16 + 16;
	}
	
#elif defined( UAV_RWBUFFER_UINT2 ) || defined( UAV_RWBUFFER_FLOAT4 )

	for ( uint i = 0; i < dwordPerThread; ++i )
	{
		// Load operation not allowed for multi-component typed UAV.

		uint mod = i % 16;
	
		if ( mod == 0 )
		{
			STORE_1_TEST( bufUAV, offset + i, i + 1 );
		}
		else if ( mod == 4 )
		{
			COMP_2_TYPE val = { i + 1, i + 2 };
			STORE_2_TEST( bufUAV, offset + i, val );
		}
		else if ( mod == 8 )
		{
			COMP_3_TYPE val = { i + 1, i + 2, i + 3 };
			STORE_3_TEST( bufUAV, offset + i, val );
		}
		else if ( mod == 12 )
		{
			COMP_4_TYPE val = { i + 1, i + 2, i + 3, i + 4 };
			STORE_4_TEST( bufUAV, offset + i, val );
		}
	}

#else // the generic scenario
		
	for ( uint i = 0; i < dwordPerThread; ++i )
	{
		// Try multiple consecutive load/store. In general, the following reads the original value in UAV, 
		// increment it by index, write it back into UAV, read it out again, increment it by 1 more, 
		// and then write it into UAV again.  If any step gives the wrong result, should store the wrong
		// value into UAV and cause the test to fail.

		uint mod = i % 16;
	
		if ( mod == 0 )
		{
			COMP_1_TYPE temp;
			PREVENT_OPT_START_BLOCK(noOpt);
				LOAD_1_TEST( bufUAV, offset + i, temp );
				STORE_1_TEST( bufUAV, offset + i, temp + i );
			PREVENT_OPT_END_BLOCK;

			PREVENT_OPT_START_BLOCK(noOpt);
				LOAD_1_TEST( bufUAV, offset + i, temp );
				STORE_1_TEST( bufUAV, offset + i, temp+1 );			
			PREVENT_OPT_END_BLOCK;
		}
		else if ( mod == 4 )
		{
			COMP_2_TYPE temp;
			PREVENT_OPT_START_BLOCK(noOpt);
				LOAD_2_TEST( bufUAV, offset + i, temp );
				COMP_2_TYPE val = { temp.x + i, temp.y + i + 1 };
				STORE_2_TEST( bufUAV, offset + i, val );
			PREVENT_OPT_END_BLOCK;

			PREVENT_OPT_START_BLOCK(noOpt);
				LOAD_2_TEST( bufUAV, offset + i, temp );
				temp.x += 1;
				temp.y += 1;
				STORE_2_TEST( bufUAV, offset + i, temp );
			PREVENT_OPT_END_BLOCK;
		}
		else if ( mod == 8 )
		{
			COMP_3_TYPE temp;
			PREVENT_OPT_START_BLOCK(noOpt);
				LOAD_3_TEST( bufUAV, offset + i, temp );
				COMP_3_TYPE val = { temp.x + i, temp.y + i + 1, temp.z + i + 2 };
				STORE_3_TEST( bufUAV, offset + i, val );
			PREVENT_OPT_END_BLOCK;

			PREVENT_OPT_START_BLOCK(noOpt);
				LOAD_3_TEST( bufUAV, offset + i, temp );
				temp.x += 1;
				temp.y += 1;
				temp.z += 1;
				STORE_3_TEST( bufUAV, offset + i, temp );
			PREVENT_OPT_END_BLOCK;
		}
		else if ( mod == 12 )
		{
			COMP_4_TYPE temp;
			PREVENT_OPT_START_BLOCK(noOpt);
				LOAD_4_TEST( bufUAV, offset + i, temp );
				COMP_4_TYPE val = { temp.x + i, temp.y + i + 1, temp.z + i + 2, temp.w + i + 3 };
				STORE_4_TEST( bufUAV, offset + i, val );
			PREVENT_OPT_END_BLOCK;

			PREVENT_OPT_START_BLOCK(noOpt);
				LOAD_4_TEST( bufUAV, offset + i, temp );
				temp.x += 1;
				temp.y += 1;
				temp.z += 1;
				temp.w += 1;
				STORE_4_TEST( bufUAV, offset + i, temp );
			PREVENT_OPT_END_BLOCK;
		}
		
	#ifdef UAV_RWBYTEADDRESSBUFFER

/*	writing to non-aligned adresses results in undefined results, according to the spec
		// test reading and writing at non-DWORD-aligned spaces
		if ( mod == 0 )
		{
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store( offset + i * 4 + 1, bufUAV.Load( offset + i * 4 + 1 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store( offset + i * 4 + 1, bufUAV.Load( offset + i * 4 + 2 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store( offset + i * 4 + 1, bufUAV.Load( offset + i * 4 + 3 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
		}
		else if ( mod == 4 )
		{
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store2( offset + i * 4 - 2, bufUAV.Load2( offset + i * 4 + 1 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store2( offset + i * 4 - 2, bufUAV.Load2( offset + i * 4 + 2 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store2( offset + i * 4 - 2, bufUAV.Load2( offset + i * 4 + 3 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
		}
		else if ( mod == 8 )
		{
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store3( offset + i * 4 - 1, bufUAV.Load3( offset + i * 4 + 1 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store3( offset + i * 4 - 1, bufUAV.Load3( offset + i * 4 + 2 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store3( offset + i * 4 - 1, bufUAV.Load3( offset + i * 4 + 3 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
		}
		else if ( mod == 12 )
		{
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store4( offset + i * 4 - 3, bufUAV.Load4( offset + i * 4 + 1 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store4( offset + i * 4 - 3, bufUAV.Load4( offset + i * 4 + 2 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
			PREVENT_OPT_START_BLOCK(noOpt);
				bufUAV.Store4( offset + i * 4 - 3, bufUAV.Load4( offset + i * 4 + 3 ) ); // should have no effect
			PREVENT_OPT_END_BLOCK;
		}
*/		
	#endif

	}
	
#endif

#ifndef UAV_RWSTRUCTUREDBUFFER_MIX // too messy to try for the MixGroup struct test case
	
	if( 0 == offset ) 
	{
		// out-of-bound writes - should have no effect
		// Use precise key word for the following variables to prevent the device from optimising the read operations
		// The compiler will set refactoringAllowed global flag by default. For shader 4x, the presence of key word 
		// will turn off the refactoringAllowed global flag. For shader 5.0, precise won't turn off the global 
		// refactoringAllowed, but it makes fine-grained overriding for those variables that are declared as precise.
		precise COMP_1_TYPE val1 = 1245; // some random values - would cause test to fail if actually stored into UAV later
		precise COMP_2_TYPE val2 = { 12356, 1864 };
		precise COMP_3_TYPE val3 = { 154, 1325, 13515 };
		precise COMP_4_TYPE val4 = { 7863, 122, 124, 5353 };

		PREVENT_OPT_START_BLOCK(noOpt);
			STORE_1_TEST( bufUAV, endOfUAV + 4, val1 );
		PREVENT_OPT_END_BLOCK;
		PREVENT_OPT_START_BLOCK(noOpt);
			STORE_2_TEST( bufUAV, endOfUAV + 4, val2 );
		PREVENT_OPT_END_BLOCK;
		PREVENT_OPT_START_BLOCK(noOpt);
			STORE_3_TEST( bufUAV, endOfUAV + 4, val3 );
		PREVENT_OPT_END_BLOCK;
		PREVENT_OPT_START_BLOCK(noOpt);
			STORE_4_TEST( bufUAV, endOfUAV + 4, val4 );
		PREVENT_OPT_END_BLOCK;

		// out-of-bound read - should return zero.
		// For typed UAVs that doesn't support Load, the macro has been defined to set val to zero.
		LOAD_1_TEST( bufUAV, endOfUAV + 4, val1 );
		LOAD_2_TEST( bufUAV, endOfUAV + 4, val2 );
		LOAD_3_TEST( bufUAV, endOfUAV + 4, val3 );
		LOAD_4_TEST( bufUAV, endOfUAV + 4, val4 );

		if ( val1 != 0 )
		{
			STORE_1_TEST( bufUAV, 0, 0 ); // test will fail - expected first value is not 0
		}
		if ( val2.x != 0 || val2.y != 0 )
		{
			val2.x = 0;
			STORE_2_TEST( bufUAV, 0, val2 ); // test will fail - expected first value is not 0
		}
		if ( val3.x != 0 || val3.y != 0 || val3.z != 0 )
		{
			val3.x = 0;
			STORE_3_TEST( bufUAV, 0, val3 ); // test will fail - expected first value is not 0
		}
		if ( val4.x != 0 || val4.y != 0 || val4.z != 0 || val4.w != 0 )
		{
			val4.x = 0;
			STORE_4_TEST( bufUAV, 0, val4 ); // test will fail - expected first value is not 0
		}
	}

#endif

	PREVENT_OPT_END_BLOCK;
	
#ifdef PIXEL_SHADER_TEST
#if PIXEL_SHADER_UAV_ONLY == 0
	return 1.0; //dummy output to the dummy rendertarget
#endif
#endif

}
