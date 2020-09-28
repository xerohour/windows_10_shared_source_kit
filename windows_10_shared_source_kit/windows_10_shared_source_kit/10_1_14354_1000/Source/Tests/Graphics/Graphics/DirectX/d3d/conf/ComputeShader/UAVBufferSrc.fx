// UAVBufferSrc.fx

#ifndef DEFINE_SET

	#define THREAD_COUNT_X 100
	#define THREAD_COUNT_Y 100
	#define THREAD_COUNT_Z 100
	#define DISPATCH_COUNT_X 100
	#define DISPATCH_COUNT_Y 100
	#define DISPATCH_COUNT_Z 100
	#define VIEWPORT_WIDTH 100
	#define VIEWPORT_HEIGHT 100
	#define BUFFER_BYTE_WIDTH 16 * 16 * 4
	#define SRC_BYTEADDRESSBUFFER 1
	#define RESULT_PATTERN 12, 1, 2 ,3
	#define PIXEL_SHADER_TEST 1
	#define PIXEL_SHADER_UAV_ONLY 1
	#define NUM_BLANKS 100
	
#endif


#ifdef SRC_BYTEADDRESSBUFFER

	ByteAddressBuffer g_inputBuffer;

#elif SRC_STRUCTUREDBUFFER_4UINT

	struct UIntGroup
	{
		uint a;
		uint b;
		uint2 c;
	};
	
	StructuredBuffer<UIntGroup> g_inputBuffer;

#elif SRC_STRUCTUREDBUFFER_2SINT

	struct SIntGroup
	{
		int a;
		int b;
	};
	
	StructuredBuffer<SIntGroup> g_inputBuffer;

#elif SRC_STRUCTUREDBUFFER_1FLOAT

	struct FloatGroup
	{
		float a;
	};
	
	StructuredBuffer<FloatGroup> g_inputBuffer;

#elif SRC_STRUCTUREDBUFFER_MIX

	struct MixStructGroup
	{
		bool b1;
		float3 f1;
		float f2;
		bool4 b2;
		int i1;
		uint2 u;
		int i2;
		int3 i3;
	};	
	
	bool VerifyMixGroup(
		MixStructGroup group,
		bool b1Val, float f1Val, float f2Val, float f3Val, float f4Val, bool b2Val, bool b3Val, bool b4Val, 
		bool b5Val, int i1Val, uint u1Val, uint u2Val, int i2Val, int i3Val, int i4Val, int i5Val) 
	{
		if ( group.b1 == b1Val && group.f1.x == f1Val && group.f1.y == f2Val && group.f1.z == f3Val  
			&& group.f2 == f4Val && group.b2.x == b2Val && group.b2.y == b3Val && group.b2.z == b4Val 
			&& group.b2.w == b5Val && group.i1 == i1Val && group.u.x == u1Val && group.u.y == u2Val 
			&& group.i2 == i2Val && group.i3.x == i3Val && group.i3.y == i4Val && group.i3.z == i5Val )
		{
			return true;
		}
		return false;
	}

	StructuredBuffer<MixStructGroup> g_inputBuffer;
	
#endif


// macros to help prevent code from being optimized out. g_inputBuffer should be all zero.
#define PREVENT_OPT_START_BLOCK( i ) i = ( i + 4 ) % BLANK_BYTES; if ( blankBuffer.Load( i ) == 0 ) { 
#define PREVENT_OPT_END_BLOCK }

ByteAddressBuffer blankBuffer;

struct ResultGroup
{
	uint result;
};

RWStructuredBuffer<ResultGroup> outUAV;


#ifdef PIXEL_SHADER_TEST


#if PIXEL_SHADER_UAV_ONLY == 1
void PSMain( float4 Pos : SV_POSITION )
#else
float PSMain( float4 Pos : SV_POSITION ) : SV_TARGET
#endif
{
	uint resultIndex = ( Pos.y - 0.5 ) * VIEWPORT_WIDTH + ( Pos.x - 0.5 );

#elif COMPUTE_SHADER_TEST


[numthreads(THREAD_COUNT_X,THREAD_COUNT_Y,THREAD_COUNT_Z)]
void CSMain( uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex )
{	
	// Shader verifies that the expected repeating pattern exists in the input resource

	uint totalThreadsPerGroup = THREAD_COUNT_X * THREAD_COUNT_Y * THREAD_COUNT_Z;
	uint totalGroup = DISPATCH_COUNT_X * DISPATCH_COUNT_Y * DISPATCH_COUNT_Z;
	uint resultIndex = totalThreadsPerGroup * ( groupID.z * DISPATCH_COUNT_X * DISPATCH_COUNT_Y + groupID.y * DISPATCH_COUNT_X + groupID.x ) + threadIndex;

#endif

	uint noOpt = 0; // counter to help avoid compiler opt. from getting rid of code
	bool testPass = true;

	PREVENT_OPT_START_BLOCK(noOpt); // not really needed, but useful to catch problems if this entire mechanism broke down

	// separate verification code for each resource type

#ifdef SRC_BYTEADDRESSBUFFER

	uint4 pattern = { RESULT_PATTERN };
	for ( uint i = 0; i < BUFFER_BYTE_WIDTH; i = i + 16 )
	{
		uint val1;
		uint2 val2;
		uint3 val3;
		uint4 val4;
		PREVENT_OPT_START_BLOCK(noOpt);
			val1 = g_inputBuffer.Load( i );
		PREVENT_OPT_END_BLOCK;
		PREVENT_OPT_START_BLOCK(noOpt);
			val2 = g_inputBuffer.Load2( i );
		PREVENT_OPT_END_BLOCK;
		PREVENT_OPT_START_BLOCK(noOpt);
			val3 = g_inputBuffer.Load3( i );
		PREVENT_OPT_END_BLOCK;
		PREVENT_OPT_START_BLOCK(noOpt);
			val4 = g_inputBuffer.Load4( i );
		PREVENT_OPT_END_BLOCK;
		
		if ( val1 != pattern.x
			|| val2.x != pattern.x || val2.y != pattern.y
			|| val3.x != pattern.x || val3.y != pattern.y || val3.z != pattern.z
			|| val4.x != pattern.x || val4.y != pattern.y || val4.z != pattern.z || val4.w != pattern.w )
		{
			testPass = false;
			break;
		}
	}

	// non-DWORD-aligned read
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load( 1 ); // should be overwritten at end of test anyway
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load( 2 );
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load( 3 );
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load2( 1 ).x;
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load2( 2 ).y;
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load2( 3 ).y;
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load3( 1 ).x;
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load3( 2 ).y;
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load3( 3 ).z;
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load4( 1 ).x;
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load4( 2 ).z;
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		outUAV[resultIndex].result = g_inputBuffer.Load4( 3 ).w;
	PREVENT_OPT_END_BLOCK;

	// out-of-bound read
	PREVENT_OPT_START_BLOCK(noOpt);
		if ( g_inputBuffer.Load( BUFFER_BYTE_WIDTH ) != 0 )
		{
			testPass = false;
		}
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		if ( g_inputBuffer.Load2( BUFFER_BYTE_WIDTH ).y != 0 )
		{
			testPass = false;
		}
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		if ( g_inputBuffer.Load3( BUFFER_BYTE_WIDTH ).z != 0 )
		{
			testPass = false;
		}
	PREVENT_OPT_END_BLOCK;
	PREVENT_OPT_START_BLOCK(noOpt);
		if ( g_inputBuffer.Load4( BUFFER_BYTE_WIDTH ).w != 0 )
		{
			testPass = false;
		}
	PREVENT_OPT_END_BLOCK;

	
#elif SRC_STRUCTUREDBUFFER_4UINT
	
	uint4 pattern = { RESULT_PATTERN };
	for ( uint i = 0; i < BUFFER_BYTE_WIDTH / 16 ; i++ )
	{
		if ( g_inputBuffer[i].a != pattern.x || g_inputBuffer[i].b != pattern.y || g_inputBuffer[i].c.x != pattern.z || g_inputBuffer[i].c.y != pattern.w )
		{
			testPass = false;
			break;
		}
	}

	if ( g_inputBuffer[BUFFER_BYTE_WIDTH / 16].c.y != 0 ) // out-of-bound read
	{
		testPass = false;
	}
	
#elif SRC_STRUCTUREDBUFFER_2SINT
	
	int2 pattern = { RESULT_PATTERN };
	for ( uint i = 0; i < BUFFER_BYTE_WIDTH / 8 ; i++ )
	{
		if ( g_inputBuffer[i].a != pattern.x || g_inputBuffer[i].b != pattern.y )
		{
			testPass = false;
			break;
		}
	}

	if ( g_inputBuffer[BUFFER_BYTE_WIDTH / 8].b != 0 ) // out-of-bound read
	{
		testPass = false;
	}

#elif SRC_STRUCTUREDBUFFER_1FLOAT
	
	float pattern = RESULT_PATTERN;
	for ( uint i = 0; i < BUFFER_BYTE_WIDTH / 4 ; i++ )
	{
		if ( g_inputBuffer[i].a != pattern )
		{
			testPass = false;
			break;
		}
	}

	if ( g_inputBuffer[BUFFER_BYTE_WIDTH / 4].a != 0 ) // out-of-bound read
	{
		testPass = false;
	}

#elif SRC_STRUCTUREDBUFFER_MIX
	
	for ( uint i = 0; i < BUFFER_BYTE_WIDTH / (16 * 4) ; i++ )
	{
		if ( VerifyMixGroup( g_inputBuffer[i], RESULT_PATTERN ) )
		{
			testPass = false;
			break;
		}
	}
		
	if ( g_inputBuffer[BUFFER_BYTE_WIDTH / ( 16 * 4 )].i1 != 0 ) // out-of-bound read
	{
		testPass = false;
	}


#endif

	if ( testPass )
	{
		outUAV[resultIndex].result = 1;
	}
	else
	{
		outUAV[resultIndex].result = 0xFFFFFFFF;
	}

	PREVENT_OPT_END_BLOCK;
	
#ifdef PIXEL_SHADER_TEST
#if PIXEL_SHADER_UAV_ONLY == 0
	return 1.0; //dummy output to the dummy rendertarget
#endif
#endif


}
