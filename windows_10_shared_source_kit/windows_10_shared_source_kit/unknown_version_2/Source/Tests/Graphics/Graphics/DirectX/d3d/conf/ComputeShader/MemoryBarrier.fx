
// Use a structure that will max out the memory. (groups * num_threads)
#ifndef DEFINES_SET
	// rosource width needs to be at least this long
	#define GROUP_THREAD_COUNT_X 768
	#define GROUP_THREAD_COUNT_Y 1
	#define GROUP_THREAD_COUNT_Z 1
	#define GROUP_COUNT_X 64
	#define GROUP_COUNT_Y 1
	#define GROUP_COUNT_Z 1
	//#define GROUP_COUNT 64
	#define WINDOW_SIZE 0x8
	//#define UPDATE_COUNT 65
	//#define CHECK_TIME 16
	#define RESOURCE RWByteAddressBuffer
	#define RESOURCE_TYPE 2
	//#define DIMENSIONS 1
	//#define MAX_THREAD_GROUP_SHARED 512000
	#define IS_STRUCTURED 0
	#define IS_BYTE_ADDRESSED 1
	#define USE_GROUP_SHARE_FLAG 1
	#define STRUCT_SIZE 4
	#define MEMORY_BARRIER_TYPE 3
	#define USE_CROSS_GROUP_READS 0
#else
	#ifndef GROUP_THREAD_COUNT_X
	#error "Missing"
	#endif
	#ifndef GROUP_THREAD_COUNT_Y
	#error "Missing"
	#endif
	#ifndef GROUP_THREAD_COUNT_Z
	#error "Missing"
	#endif
	#ifndef GROUP_COUNT_X
	#error "Missing"
	#endif
	#ifndef GROUP_COUNT_Y
	#error "Missing"
	#endif
	#ifndef GROUP_COUNT_Z
	#error "Missing"
	#endif
	#ifndef WINDOW_SIZE
	#error "Missing"
	#endif
	#ifndef RESOURCE
	#error "Missing"
	#endif
	#ifndef RESOURCE_TYPE
	#error "Missing"
	#endif
	#ifndef IS_STRUCTURED
	#error "Missing"
	#endif
	#ifndef IS_BYTE_ADDRESSED
	#error "Missing"
	#endif
	#ifndef USE_GROUP_SHARE_FLAG
	#error "Missing"
	#endif
	#ifndef STRUCT_SIZE
	#error "Missing"
	#endif
	#ifndef MEMORY_BARRIER_TYPE
	#error "Missing"
	#endif
	#ifndef USE_CROSS_GROUP_READS
	#error "Missing"
	#endif
#endif

#pragma ruledisable "Hoist predicated code into outermost predicate"

#define GROUP_THREAD_COUNT GROUP_THREAD_COUNT_X * GROUP_THREAD_COUNT_Y * GROUP_THREAD_COUNT_Z
#define GROUP_COUNT GROUP_COUNT_X * GROUP_COUNT_Y * GROUP_COUNT_Z
#define GROUP_AND_THREAD_SIZE GROUP_COUNT * GROUP_THREAD_COUNT

#if RESOURCE_TYPE == 1
	#define UAV_DIMENSION_TYPE uint
#elif RESOURCE_TYPE == 2
	#define UAV_DIMENSION_TYPE uint
#elif RESOURCE_TYPE == 3
	#define UAV_DIMENSION_TYPE uint
#elif RESOURCE_TYPE == 4
	#define UAV_DIMENSION_TYPE uint
#elif RESOURCE_TYPE == 5
	#define UAV_DIMENSION_TYPE uint2
#elif RESOURCE_TYPE == 6
	#define UAV_DIMENSION_TYPE uint2
#elif RESOURCE_TYPE == 7
	#define UAV_DIMENSION_TYPE uint3
#elif RESOURCE_TYPE == 8
	#define UAV_DIMENSION_TYPE uint3
#else
	#error "unknown resource type"
#endif

#if USE_CROSS_GROUP_READS 
	#define UAV_DECORATION globallycoherent
	#define UAV_OFFSET ((groupIndex * 7) % GROUP_COUNT) * GROUP_THREAD_COUNT
#else
	#define UAV_DECORATION
	#define UAV_OFFSET offset
#endif

#if IS_STRUCTURED
	#if USE_GROUP_SHARE_FLAG
		struct SpecialType
		{
			uint myValue[ STRUCT_SIZE - 1 ];
			uint myResult;
		};
		void UAVStore( RESOURCE uav, UAV_DIMENSION_TYPE index, uint value )
		{
			#if STRUCT_SIZE > 1
			for( uint i = 0; i < STRUCT_SIZE - 1; ++i )
				uav[index].myValue[i] = value;
			#else
				uav[index].myValue[0] = value;
			#endif
		};
		#define UAVLoad( _uav, _index ) _uav[_index].myValue[STRUCT_SIZE-2];
	#else
		struct SpecialType
		{
			uint myFlag;
			uint myValue[ STRUCT_SIZE - 2];
			uint myResult;
		};
		void UAVStore( RESOURCE uav, UAV_DIMENSION_TYPE index, uint value )
		{
			#if STRUCT_SIZE > 2
			for( uint i = 0; i < STRUCT_SIZE - 2; ++i )
				uav[index].myValue[i] = value;
			#else
				uav[index].myValue[0] = value;
			#endif
		};
		#define UAVLoad( _uav, _index ) _uav[_index].myValue[STRUCT_SIZE-3]
	#endif
	#define UAVStoreResult( _uav, _index, _value) _uav[_index].myResult = _value
#elif IS_BYTE_ADDRESSED
	#define UAVStore( _uav, _index, _value ) _uav.Store( _index * 4, _value )
	#define UAVLoad( _uav, _index ) _uav.Load( _index * 4 )
	#define UAVStoreResult UAVStore
#else 
	#define UAVStore( _uav, _index, _value ) _uav[_index] = _value
	#define UAVLoad( _uav, _index ) _uav[_index]
	#define UAVStoreResult UAVStore
#endif

UAV_DECORATION RESOURCE ux;

#if RESOURCE_TYPE == 1
	#define FIX( _i ) _i
#elif RESOURCE_TYPE == 2
	#define FIX( _i ) _i
#elif RESOURCE_TYPE == 3
	#define FIX( _i ) _i
#elif RESOURCE_TYPE == 4
	#define FIX( _i ) _i
#elif RESOURCE_TYPE == 5
	uint2 FIX( uint i )
	{
		uint width = 0;
		uint elements = 0;
		ux.GetDimensions( width, elements );
		return uint2( i % width, i / width );
	}
#elif RESOURCE_TYPE == 6
	uint2 FIX( uint i )
	{
		uint width = 0;
		uint height = 0;
		ux.GetDimensions( width, height );
		return uint2( i % width , i / width );
	}
#elif RESOURCE_TYPE == 7
	uint3 FIX( uint i )
	{
		uint width = 0;
		uint height = 0;
		uint elements = 0;
		ux.GetDimensions( width, height, elements );
		return uint3( (i % (width * height)) % width, (i % (width * height)) / width, i / ( width * height ) );
	}
#elif RESOURCE_TYPE == 8
	uint3 FIX( uint i )
	{
		uint width = 0;
		uint height = 0;
		uint depth = 0;
		ux.GetDimensions( width, height, depth );
		return uint3( (i % (width * height)) % width, (i % (width * height)) / width, i / ( width * height ) );
	}
#else
	#error "unknown resource type"
#endif

#if USE_GROUP_SHARE_FLAG
	#define PID pid
	#define FLAG_OFFSET 0
	#define FLAG s_SharedMemory
	#define GX s_SharedMemory
	#define FLAG_FIX( _index ) _index
	#define FLAG_MEMORY_BARRIER GroupMemoryBarrier()
	struct GroupShareStuct
	{
		#if MEMORY_BARRIER_TYPE != 2 && MEMORY_BARRIER_TYPE != 6
			uint gx;
		#endif
		uint flag;
	};
	// DX10 must only operate within its own window of groupshared memory.
	groupshared static GroupShareStuct s_SharedMemory[GROUP_THREAD_COUNT];

	// flag is stored in group memory
	#define StoreGX( _array, _index, _value ) _array[_index].gx = _value
	#define LoadGX( _array, _index ) _array[_index].gx
	#define StoreFlag( _array, _index, _value ) _array[_index].flag = _value
	#define LoadFlag( _array, _index ) _array[_index].flag

#else
	#define PID index
	#define FLAG_OFFSET UAV_OFFSET
	#define FLAG ux
	#define GX s_SharedMemory
	#define FLAG_MEMORY_BARRIER DeviceMemoryBarrier()
	//#if MEMORY_BARRIER_TYPE == 3
	#if (MEMORY_BARRIER_TYPE != 2) && (MEMORY_BARRIER_TYPE != 6) && (USE_CROSS_GROUP_READS == 0)
		groupshared static uint s_SharedMemory[GROUP_THREAD_COUNT];
	#endif

	#define StoreGX( _array, _index, _value ) _array[_index] = _value
	#define LoadGX( _array, _index ) _array[_index]
	#if IS_STRUCTURED
		#define FLAG_FIX( _index ) FIX( _index )
	#else
		#define FLAG_FIX( _index ) FIX( (_index + GROUP_AND_THREAD_SIZE) )
	#endif
	#if IS_STRUCTURED
		// flag is stored in the structure
		#define StoreFlag( _uav, _index, _value ) _uav[_index].myFlag = _value
		#define LoadFlag( _uav, _index ) _uav[_index].myFlag
	#elif IS_BYTE_ADDRESSED
		// flag is stored in the second half of the uav
		#define StoreFlag( _uav, _index, _value ) _uav.Store( (_index) * 4, _value )
		#define LoadFlag( _uav, _index ) _uav.Load( (_index) * 4 )
	#else
		// flag is stored in the second half of the uav
		#define StoreFlag( _uav, _index, _value ) _uav[_index] = _value
		#define LoadFlag( _uav, _index ) _uav[_index]
	#endif
#endif

// finding the location to store result
#if IS_STRUCTURED
		#define RESULT_FIX( _index ) FIX( _index )
#else
		#define RESULT_FIX( _index ) FIX( (_index + RESULT_OFFSET) )
#endif

#if MEMORY_BARRIER_TYPE == 3
	#define MEMORY_BARRIER AllMemoryBarrier()
#elif MEMORY_BARRIER_TYPE == 1
	#define MEMORY_BARRIER GroupMemoryBarrier()
#elif MEMORY_BARRIER_TYPE == 2
	#define MEMORY_BARRIER DeviceMemoryBarrier()
#elif MEMORY_BARRIER_TYPE == 4
	#define MEMORY_BARRIER __test_intrin1()
#elif MEMORY_BARRIER_TYPE == 5
	#define MEMORY_BARRIER __test_intrin2()
#elif MEMORY_BARRIER_TYPE == 6
	#define MEMORY_BARRIER __test_intrin3()
#elif MEMORY_BARRIER_TYPE == 7
	#define MEMORY_BARRIER __test_intrin4()
#else
	#error "unknown memory barrier type"
#endif

uint UPDATE_COUNT;
uint CHECK_TIME;

// UAV types
// Texture UAV's are DX11 only
//RWTexture1D<uint4> ux; 
//RWTexture1DArray<uint4>;
//RWTexture2D<uint4>;
//RWTexture2DArray<uint4>;
//RWTexture3D<uint4>;
//RWBuffer<uint4> ux;

// Structured and Byte Address buffers work in DX10 and DX11
//RWStructuredBuffer<>;
//RWByteAddressBuffer;

// AllMemoryBarrier() -- groupshare and UAV
// DeviceMemoryBarrier() -- UAV only
// GroupMemoryBarrier -- groupshare only

// This algorithm attempts to find failures in the memory barriers to flush
// both groupshared and UAV both within a group and across groups.  
// It relies on one of either the UAV or groupshared
// to fail to flush on the memory barrier.  The algorithm is limitted to searching
// just within a window of itself whether in the same group or not.
// Several considerations must be accounted for:  The reqest is for GROUP_THREAD_COUNT to 
// be the maximum 1k size.  Hardware will not be able to support that initially, so it 
// will be splitting up these threads into blocks and executing them as far as possible 
// until a sync point is found.  Because of this, the algorithm needs to look for 
// failures within a small window of its current location if it is going to find any
// problems at all.  Somewhere between 8 and 16 seems about right.
// No sync can cause all groups to syncronize, so the cross group detection will 
// be even less likely to work..... !!!
[numthreads(GROUP_THREAD_COUNT_X,GROUP_THREAD_COUNT_Y,GROUP_THREAD_COUNT_Z)]
void main( uint pid : SV_GroupIndex, uint3 gid : SV_GroupID )
{
	// calculate the group index
	uint groupIndex = 
		gid.z * GROUP_COUNT_Y * GROUP_COUNT_X 
		+ gid.y * GROUP_COUNT_X
		+ gid.x;
	uint offset = groupIndex * GROUP_THREAD_COUNT;
	uint index = pid + offset;
	uint faults = 0;

	// look for change in flag, but no change in gx or ux
	// Pick a location to examine within a small window of the current location.
	precise int base = (pid / WINDOW_SIZE) * WINDOW_SIZE;
	int otherid = 0;
	if( base + WINDOW_SIZE > GROUP_THREAD_COUNT )
	{
		otherid = base + ((pid + 5) % (GROUP_THREAD_COUNT - base));
	}
	else
	{
		otherid = base + ((pid + 5) % WINDOW_SIZE);
	}

	// Zero the memory first
	#if MEMORY_BARRIER_TYPE != 1
		UAVStore( ux, FIX(index), 0 );
	#endif
	#if (MEMORY_BARRIER_TYPE != 2) && (MEMORY_BARRIER_TYPE != 6) && (USE_CROSS_GROUP_READS == 0)
		StoreGX( GX, pid, 0 );
	#endif
	StoreFlag( FLAG, FLAG_FIX(PID), 0 );

	// Run an incomplete number of updates and check for failures in the memory barrier
	[loop]
	#if USE_CROSS_GROUP_READS
		for( uint j = 1; j < (UPDATE_COUNT * groupIndex) % 73; ++j )
		{
	#else
		for ( uint j = 1; j < UPDATE_COUNT; ++j )  
		{
			AllMemoryBarrierWithGroupSync(); // must sync here 
	#endif
		
		#if MEMORY_BARRIER_TYPE != 1
			UAVStore( ux, FIX(index), j );
		#endif
		#if (MEMORY_BARRIER_TYPE != 2) && (MEMORY_BARRIER_TYPE != 6) && (USE_CROSS_GROUP_READS == 0)
			StoreGX( GX, pid, j );
		#endif
		MEMORY_BARRIER; // use the memory barrier being tested.

		// tell other threads that ux and gx have been updated.
		StoreFlag( FLAG, FLAG_FIX(PID), j );

		[loop]
		for ( uint i = 0; i < CHECK_TIME; ++i)
		{
			uint sig_f = LoadFlag( FLAG, FLAG_FIX((otherid + FLAG_OFFSET)) );
			
			MEMORY_BARRIER; // use the memory barrier being tested.
			
			#if (MEMORY_BARRIER_TYPE != 2) && (MEMORY_BARRIER_TYPE != 6) && (USE_CROSS_GROUP_READS == 0)
				uint sig_g = LoadGX( GX, otherid );
			#endif

			#if MEMORY_BARRIER_TYPE != 1
				uint sig_u = UAVLoad( ux, FIX((otherid + UAV_OFFSET)) );
			#endif


			#if (MEMORY_BARRIER_TYPE != 2) && (MEMORY_BARRIER_TYPE != 6) && (USE_CROSS_GROUP_READS == 0)
				if ( sig_f == j && sig_g != j ) // short circuiting important
					++faults;
				if( sig_g == j-1 && sig_f != j-1 ) // short circuiting important
					++faults;

			
			#endif
			#if MEMORY_BARRIER_TYPE == 3
				#if USE_CROSS_GROUP_READS
					if( sig_f != sig_u && sig_f == sig_u + 1 )
						++faults;
				#else
					if ( sig_f == j && sig_u != j ) // short circuiting important
						++faults;
					if( sig_u == j-1 && sig_f != j-1 ) // short circuiting important
						++faults;
				#endif
			#endif
			
			// Compile time memory barrier choice	
			FLAG_MEMORY_BARRIER;  // force the flag to flush if it hasn't already
		}
	}	
	
	UAVStoreResult( ux, RESULT_FIX(index), faults ); // Store the faults for examination.
}

//	AllMemoryBarrier();
//		1) Write data to both the uav and to groupshared memory.
//		2) Use a seperate flag to indicate the writes are complete. 
//		3) Use another thread to look for the flag to be set
//			- When the flag is set, make sure the initial writes to the uav
//			  and to the groupshared memory were flushed.
//	
//		Will this test work with globallycoherent uav's?
//		-	yes, only when both the flag and the data are in a uav.
//			In this case, the groupshared memory data would have to 
//			be ignored.
//			The offset for the read would have to point to a different group
//			The lack of a cross group sync is a problem... how will we 
//			know what values should be in the uav?  We're limited to
//			a single check for if( flag != uav && flag == uav + 1 )
//	
//	DeviceMemoryBarrier();
//		1) Write data to a uav
//		2) Write data to a flag to indicate writes are complete
//		3) Use another thread to look for the flag to be set
//			- When the flag is set, make sure the initial writes to the uav
//			  were flushed.
//	
//		Will this test work on globallycoherent uav's?
//		-	yes, this can when both the flag and the data store are uav.
//			The offset for the read would have to point to a different group
//			The lack of a cross group sync is a problem... how will we 
//			know what values should be in the uav?  We're limited to
//			a single check for if( flag != uav && flag == uav + 1 )
//	
//	GroupMemoryBarrier();
//		1) Write data to groupshared memory
//		2) Write data to a flag to indicate writes are complete
//		3) Use another thread to look for the flag to be set
//			- when the flag is set, make sure the initial writes to the 
//			  groupshared memory were flushed.
//	
//		This will not work with globallycoherent tests -- no uav's to use.
//
//	REF executes one Group at a time, but hardware might do several.
//		There is no way to sync threads between groups, so the 
//		storage location of the restul needs to be different from the
//		data location to prevent false faults from being detected in
//		the cross group test cases.
