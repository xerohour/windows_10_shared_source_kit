// UAVTyped.fx

#ifdef UAV_RWBUF
RWBuffer<TYPE> texUAV;
#elif UAV_TEXTURE1D
RWTexture1D<TYPE> texUAV;
#elif UAV_TEXTURE1D_ARRAY
RWTexture1DArray<TYPE> texUAV;
#elif UAV_TEXTURE2D
RWTexture2D<TYPE> texUAV;
#elif UAV_TEXTURE2D_ARRAY
RWTexture2DArray<TYPE> texUAV;
#elif UAV_TEXTURE3D
RWTexture3D<TYPE> texUAV;
#endif

#ifndef THREADCOUNTX
#define THREADCOUNTX 1
#endif
#ifndef THREADCOUNTY
#define THREADCOUNTY 1
#endif
#ifndef THREADCOUNTZ
#define THREADCOUNTZ 1
#endif
#ifndef VIEWPORT_WIDTH
#define VIEWPORT_WIDTH 1
#endif
#ifndef DEPTH_SLICE 
#define DEPTH_SLICE 0
#endif

#ifndef SCALEFACTOR
#define SCALEFACTOR 128
#endif

[numthreads(THREADCOUNTX, THREADCOUNTY, THREADCOUNTZ)]
void CSMain(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex)
{
#if INTEGER_FORMAT == 1
    float div = 1.0f;
#else 
    float div = SCALEFACTOR;
#endif

#ifdef UAV_WRITE

#ifdef UAV_TEXTURE1D
    texUAV[threadID.x] = (float)(threadIndex % SCALEFACTOR) / div;
#elif UAV_TEXTURE1D_ARRAY
    texUAV[threadID.xy] = (float)(threadIndex % SCALEFACTOR) / div;
#elif UAV_TEXTURE2D
    texUAV[threadID.xy] = (float)(threadIndex % SCALEFACTOR) / div;
#elif UAV_TEXTURE2D_ARRAY
    texUAV[threadID.xyz] = (float)(threadIndex % SCALEFACTOR) / div;
#elif UAV_TEXTURE3D
    texUAV[threadID.xyz] = (float)(threadIndex % SCALEFACTOR) / div;
#endif

#elif UAV_READ

#ifdef UAV_RWBUF
    texUAV[threadID.x] = threadIndex;
    DeviceMemoryBarrier();
    texUAV[threadID.x] = ((texUAV[threadID.x] + 1) % SCALEFACTOR) / div;
#elif UAV_TEXTURE1D
    texUAV[threadID.x] = threadIndex;
    DeviceMemoryBarrier();
    texUAV[threadID.x] = ((texUAV[threadID.x] + 1) % SCALEFACTOR) / div;
#elif UAV_TEXTURE1D_ARRAY
    texUAV[threadID.xy] = threadIndex;
    DeviceMemoryBarrier();
    texUAV[threadID.xy] = ((texUAV[threadID.xy] + 1) % SCALEFACTOR) / div;
#elif UAV_TEXTURE2D
    texUAV[threadID.xy] = threadIndex;
    DeviceMemoryBarrier();
    texUAV[threadID.xy] = ((texUAV[threadID.xy] + 1) % SCALEFACTOR) / div;
#elif UAV_TEXTURE2D_ARRAY
    texUAV[threadID.xyz] = threadIndex;
    DeviceMemoryBarrier();
    texUAV[threadID.xyz] = ((texUAV[threadID.xyz] + 1) % SCALEFACTOR) / div;
#elif UAV_TEXTURE3D
    texUAV[threadID.xyz] = threadIndex;
    DeviceMemoryBarrier();
    texUAV[threadID.xyz] = ((texUAV[threadID.xyz] + 1) % SCALEFACTOR) / div;
#endif

#elif UAV_READ_OOB

#ifdef UAV_TEXTURE1D
    texUAV[threadID.x] = texUAV[-1];
#elif UAV_TEXTURE1D_ARRAY
    texUAV[threadID.xy] = texUAV[uint2(-1, -1)];
#elif UAV_TEXTURE2D
    texUAV[threadID.xy] = texUAV[uint2(-1, -1)];
#elif UAV_TEXTURE2D_ARRAY
    texUAV[threadID.xyz] = texUAV[uint3(-1, -1, -1)];
#elif UAV_TEXTURE3D
    texUAV[threadID.xyz] = texUAV[uint3(-1, -1, -1)];
#endif

#endif
}

float4 VSMain(float4 input : POSITION) : SV_POSITION
{
    return input;
}

#if PIXEL_SHADER_UAV_ONLY == 1
void PSMain(float4 Pos : SV_POSITION)
#else
float PSMain(float4 Pos : SV_POSITION) : SV_TARGET
#endif
{
    float3 offsetPos = float3(Pos.x - 0.5f, Pos.y - 0.5f, DEPTH_SLICE);
    uint index = offsetPos.y * VIEWPORT_WIDTH + offsetPos.x;

#if INTEGER_FORMAT == 1
    float div = 1.0f;
#else 
    float div = SCALEFACTOR;
#endif

#ifdef UAV_WRITE

#ifdef UAV_TEXTURE1D
    texUAV[offsetPos.x] = (float)(index % SCALEFACTOR) / div;
#elif UAV_TEXTURE1D_ARRAY
    texUAV[offsetPos.xy] = (float)(index % SCALEFACTOR) / div;
#elif UAV_TEXTURE2D
    texUAV[offsetPos.xy] = (float)(index % SCALEFACTOR) / div;
#elif UAV_TEXTURE2D_ARRAY
    texUAV[offsetPos.xyz] = (float)(index % SCALEFACTOR) / div;
#elif UAV_TEXTURE3D
    texUAV[offsetPos.xyz] = (float)(index % SCALEFACTOR) / div;
#endif

#elif UAV_READ

#ifdef UAV_RWBUF
    texUAV[offsetPos.x] = index;
    DeviceMemoryBarrier();
    texUAV[offsetPos.x] = ((texUAV[offsetPos.x] + 1) % SCALEFACTOR) / div;
#elif UAV_TEXTURE1D
    texUAV[offsetPos.x] = index;
    DeviceMemoryBarrier();
    texUAV[offsetPos.x] = ((texUAV[offsetPos.x] + 1) % SCALEFACTOR) / div;
#elif UAV_TEXTURE1D_ARRAY
    texUAV[offsetPos.xy] = index;
    DeviceMemoryBarrier();
    texUAV[offsetPos.xy] = ((texUAV[offsetPos.xy] + 1) % SCALEFACTOR) / div;
#elif UAV_TEXTURE2D
    texUAV[offsetPos.xy] = index;
    DeviceMemoryBarrier();
    texUAV[offsetPos.xy] = ((texUAV[offsetPos.xy] + 1) % SCALEFACTOR) / div;
#elif UAV_TEXTURE2D_ARRAY
    texUAV[offsetPos.xyz] = index;
    DeviceMemoryBarrier();
    texUAV[offsetPos.xyz] = ((texUAV[offsetPos.xyz] + 1) % SCALEFACTOR) / div;
#elif UAV_TEXTURE3D
    texUAV[offsetPos.xyz] = index;
    DeviceMemoryBarrier();
    texUAV[offsetPos.xyz] = ((texUAV[offsetPos.xyz] + 1) % SCALEFACTOR) / div;
#endif

#elif UAV_READ_OOB

#ifdef UAV_TEXTURE1D
    texUAV[offsetPos.x] = texUAV[-1];
#elif UAV_TEXTURE1D_ARRAY
    texUAV[offsetPos.xy] = texUAV[uint2(-1, -1)];
#elif UAV_TEXTURE2D
    texUAV[offsetPos.xy] = texUAV[uint2(-1, -1)];
#elif UAV_TEXTURE2D_ARRAY
    texUAV[offsetPos.xyz] = texUAV[uint3(-1, -1, -1)];
#elif UAV_TEXTURE3D
    texUAV[offsetPos.xyz] = texUAV[uint3(-1, -1, -1)];
#endif

#endif


#if PIXEL_SHADER_UAV_ONLY == 0
    return 1.0; //dummy output to the dummy rendertarget
#endif

}