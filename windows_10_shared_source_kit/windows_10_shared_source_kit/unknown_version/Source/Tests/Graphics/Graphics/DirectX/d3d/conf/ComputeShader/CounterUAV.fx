struct record
{
	uint incrementID;
	uint decrementID;
	uint data;
};


//#define numThreadX 1
//#define numThreadY 1
//#define numThreadZ 1
//#define READ_FROM_APPEND

//-----------------------------------------------------------------------------------------------------------
// compute shaders

#ifdef WRITE_TO_COUNTER
RWStructuredBuffer<record> resultUAV; 
RWStructuredBuffer<record> counterUAV;

[numthreads(numThreadX, numThreadY, numThreadZ)] 
void CSMain(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) 
{ 
	uint counter = counterUAV.IncrementCounter();
	counterUAV[counter].incrementID = counter;
	counterUAV[counter].decrementID = 0;
	counterUAV[counter].data = counter;
	
	resultUAV[0].incrementID = 0;
	resultUAV[0].decrementID = 0;
	resultUAV[0].data = 0;
} 
#endif

#ifdef READ_FROM_COUNTER
RWStructuredBuffer<record> resultUAV; 
RWStructuredBuffer<record> counterUAV;

[numthreads(numThreadX, numThreadY, numThreadZ)] 
void CSMain(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) 
{ 
	uint counter = counterUAV.DecrementCounter();
	resultUAV[counter].incrementID = counterUAV[counter].incrementID;
	resultUAV[counter].decrementID = counter; 
	resultUAV[counter].data = counterUAV[counter].data;
}
#endif 

#ifdef WRITE_TO_APPEND
RWStructuredBuffer<record> resultUAV; 
AppendStructuredBuffer<record> appendUAV;
RWByteAddressBuffer gCounter;

[numthreads(numThreadX, numThreadY, numThreadZ)] 
void CSMain(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) 
{ 
	uint counter;
	gCounter.InterlockedAdd(0, 1, counter);
	
	record dataRec = {counter, 0, counter};
	appendUAV.Append(dataRec);
	
	resultUAV[0].incrementID = 0;
	resultUAV[0].decrementID = 0;
	resultUAV[0].data = 0;
}
#endif 

#ifdef READ_FROM_APPEND
RWStructuredBuffer<record> resultUAV; 
ConsumeStructuredBuffer<record> appendUAV;
RWByteAddressBuffer gCounter;

[numthreads(numThreadX, numThreadY, numThreadZ)] 
void CSMain(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) 
{ 
	uint counter;
	gCounter.InterlockedAdd(0, -1, counter);
	counter -= 1;
	
	record dataRec;
	dataRec = appendUAV.Consume();
	
	resultUAV[counter].incrementID = dataRec.incrementID;
	resultUAV[counter].decrementID = counter;
	resultUAV[counter].data = dataRec.data;
}
#endif 

//-----------------------------------------------------------------------------------------------------------
// pixel shaders

#ifdef PS_WRITE_TO_COUNTER
RWStructuredBuffer<record> resultUAV; 
RWStructuredBuffer<record> counterUAV;

#if PIXEL_SHADER_UAV_ONLY == 1
void PSMain( float4 Pos : SV_POSITION )
#else
float PSMain( float4 Pos : SV_POSITION ) : SV_TARGET
#endif
{ 
	uint counter = counterUAV.IncrementCounter();
	counterUAV[counter].incrementID = counter;
	counterUAV[counter].decrementID = 0;
	counterUAV[counter].data = counter;
	
	resultUAV[0].incrementID = 0;
	resultUAV[0].decrementID = 0;
	resultUAV[0].data = 0;
	
#if PIXEL_SHADER_UAV_ONLY == 0	
	return counter; // pixel shader needs a dummy render target to get invoked.
#endif
} 
#endif

#ifdef PS_READ_FROM_COUNTER
RWStructuredBuffer<record> resultUAV; 
RWStructuredBuffer<record> counterUAV;

#if PIXEL_SHADER_UAV_ONLY == 1
void PSMain( float4 Pos : SV_POSITION )
#else
float PSMain( float4 Pos : SV_POSITION ) : SV_TARGET
#endif
{ 
	uint counter = counterUAV.DecrementCounter();
	resultUAV[counter].incrementID = counterUAV[counter].incrementID;
	resultUAV[counter].decrementID = counter; 
	resultUAV[counter].data = counterUAV[counter].data;
	
#if PIXEL_SHADER_UAV_ONLY == 0	
	return counter; // pixel shader needs a dummy render target to get invoked.
#endif
}
#endif 

#ifdef PS_WRITE_TO_APPEND
RWStructuredBuffer<record> resultUAV; 
AppendStructuredBuffer<record> appendUAV;
RWByteAddressBuffer gCounter;

#if PIXEL_SHADER_UAV_ONLY == 1
void PSMain( float4 Pos : SV_POSITION )
#else
float PSMain( float4 Pos : SV_POSITION ) : SV_TARGET
#endif
{ 
	uint counter;
	gCounter.InterlockedAdd(0, 1, counter);
	
	record dataRec = {counter, 0, counter};
	appendUAV.Append(dataRec);
	
	resultUAV[0].incrementID = 0;
	resultUAV[0].decrementID = 0;
	resultUAV[0].data = 0;
	
#if PIXEL_SHADER_UAV_ONLY == 0	
	return counter; // pixel shader needs a dummy render target to get invoked.
#endif
}
#endif 

#ifdef PS_READ_FROM_APPEND
RWStructuredBuffer<record> resultUAV; 
ConsumeStructuredBuffer<record> appendUAV;
RWByteAddressBuffer gCounter;

#if PIXEL_SHADER_UAV_ONLY == 1
void PSMain( float4 Pos : SV_POSITION )
#else
float PSMain( float4 Pos : SV_POSITION ) : SV_TARGET
#endif
{ 
	uint counter;
	gCounter.InterlockedAdd(0, -1, counter);
	counter -= 1;
	
	record dataRec;
	dataRec = appendUAV.Consume();
	
	resultUAV[counter].incrementID = dataRec.incrementID;
	resultUAV[counter].decrementID = counter;
	resultUAV[counter].data = dataRec.data;

#if PIXEL_SHADER_UAV_ONLY == 0	
	return counter; // pixel shader needs a dummy render target to get invoked.
#endif
}
#endif 