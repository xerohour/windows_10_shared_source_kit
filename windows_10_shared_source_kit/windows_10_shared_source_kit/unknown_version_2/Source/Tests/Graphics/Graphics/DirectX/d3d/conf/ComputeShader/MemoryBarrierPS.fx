#ifndef MACRO_SET
	#define VIEWPORT_WIDTH 128
	#define VIEWPORT_HEIGHT 64
	#define UAV_DCLS  RWStructuredBuffer<uint> u1; \
					  RWStructuredBuffer<uint> u2; \
					  
	#define UAV_WRITE uint index1 = {pid}; \
					  uint index2 = {pid}; \
					  u1[index1] = pid;\
					  u2[index2] = pid;\
					  
	#define UAV_READ  uint indexOther1 = {otherid}; \
	                  uint indexOther2 = {otherid}; \
					  uint sig_2 = u2[indexOther1]; \
					  uint sig_1 = u1[indexOther2]; \
					  
#endif

// All cleared to 0s
UAV_DCLS
RWBuffer<uint> u_result;

float PSMain( float4 Pos : SV_POSITION ) : SV_TARGET
{
	uint faults = 0;
	uint pid = ( Pos.y - 0.5 ) * VIEWPORT_WIDTH + ( Pos.x - 0.5 );
	
	UAV_WRITE
	
	DeviceMemoryBarrier(); // this is the only type of memory barrier allowed for pixel shader
	for ( uint j = 0; j < 32; j++) // repeat more rounds to hope other threads to finish memory flush
	{
		for ( uint i = 0; i < 16; i++)
		{
			uint otherid = (pid * 19 + i) % (VIEWPORT_WIDTH * VIEWPORT_HEIGHT);
			
			UAV_READ
	     
			if( sig_2 == otherid && sig_1 != otherid )
				faults++;
		}
	}
   u_result[pid] = faults;
   
   return faults;
} 
