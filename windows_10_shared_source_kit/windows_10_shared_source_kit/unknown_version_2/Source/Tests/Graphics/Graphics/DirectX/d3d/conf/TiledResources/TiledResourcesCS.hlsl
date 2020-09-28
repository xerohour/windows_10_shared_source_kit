// Compute shader source for WGF11TiledResources

// Using this compute shader for Texture3D resources
#ifdef USE_TEXTURE3D
RWTexture3D<uint> sourceTexture : register(u0);	// Needs to be viewed as a R32_UINT UAV
RWTexture3D<uint> destTexture : register(u1);	// Needs to be viewed as a a R32_UINT UAV
#   define NUM_THREADS_X 8
#   define NUM_THREADS_Y 8
#   define NUM_THREADS_Z 1
#   define POS_TYPE uint3
#   define GET_POS_TYPE_LOCATION(inLoc) inLoc.xyz
#   define GET_LINEAR_BUFFER_LOCATION(inLoc) (inLoc.z * 512 * 512 + inLoc.y * 512 + inLoc.x)    // Input is a linear buffer that is 128 * 128 * 64 * 4 bytes in size

// Using this compute shader for Texture2D resources
#else
RWTexture2D<uint> sourceTexture : register(u0);	// Needs to be viewed as a R32_UINT UAV
RWTexture2D<uint> destTexture : register(u1);	// Needs to be viewed as a a R32_UINT UAV
#   define NUM_THREADS_X 8
#   define NUM_THREADS_Y 8
#   define NUM_THREADS_Z 1
#   define POS_TYPE uint2
#   define GET_POS_TYPE_LOCATION(inLoc) inLoc.xy
#   define GET_LINEAR_BUFFER_LOCATION(inLoc) (inLoc.y * 512 + inLoc.x)                          // Input is a linear buffer that is 512 * 512 * 4 bytes in size
#endif

ByteAddressBuffer rawBuffer : register(t0); // Used for testing ld_raw

struct MyStruct
{
    uint value;
};

StructuredBuffer<MyStruct> structBuffer : register(t0);

uint CheckForResidencyErrors(uint value, uint feedback)
{
    bool mapped = CheckAccessFullyMapped(feedback);

    if (mapped)
    {
        if (0 == value)
        {
            return 0x000000ff; // Red indicates false positive residency feedback
        }
    }
    else
    {
        if (0 != value)
        {
            return value | 0x000000ff; // Set Red channel to 1 to indicate false negative (or garbage data)
        }
    }

    return value;
}


[numthreads(NUM_THREADS_X, NUM_THREADS_Y, NUM_THREADS_Z)]
void CopyTexCSTiled(uint3 dispatchId : SV_DispatchThreadID)
{
    uint feedback;
    POS_TYPE pos = GET_POS_TYPE_LOCATION(dispatchId);
    uint value = sourceTexture.Load(pos, feedback);
    destTexture[GET_POS_TYPE_LOCATION(pos)] = CheckForResidencyErrors(value, feedback) | 0xff000000; // Sets the A channel to 0xff assuming texture data is really R8G8B8A8_UNORM
}

[numthreads(NUM_THREADS_X, NUM_THREADS_Y, NUM_THREADS_Z)]
void CopyTexCSNormal(uint3 dispatchId : SV_DispatchThreadID)
{
    POS_TYPE pos = GET_POS_TYPE_LOCATION(dispatchId);
    uint value = sourceTexture[pos];
    destTexture[GET_POS_TYPE_LOCATION(pos)] = value | 0xff000000; // Sets the A channel to 0xff assuming texture data is really R8G8B8A8_UNORM
}

[numthreads(NUM_THREADS_X, NUM_THREADS_Y, NUM_THREADS_Z)]
void ProcessRawBufferTiled(uint3 dispatchId : SV_DispatchThreadID)
{
    uint feedback;
    POS_TYPE pos = GET_POS_TYPE_LOCATION(dispatchId);
    uint byteIndex = 4 * (GET_LINEAR_BUFFER_LOCATION(pos));
    uint value = rawBuffer.Load(byteIndex, feedback); // Input is a linear buffer that is in size: 512 * 512 * 4 bytes (Texture2D) or 128 * 128 * 64 * 4 bytes (Texture3D)

    destTexture[GET_POS_TYPE_LOCATION(pos)] = CheckForResidencyErrors(value, feedback);
}

[numthreads(NUM_THREADS_X, NUM_THREADS_Y, NUM_THREADS_Z)]
void ProcessStructuredBufferTiled(uint3 dispatchId : SV_DispatchThreadID)
{
    uint feedback;
    POS_TYPE pos = GET_POS_TYPE_LOCATION(dispatchId);
    uint index = GET_LINEAR_BUFFER_LOCATION(pos);
    MyStruct data = structBuffer.Load(index, feedback); // Input is a linear buffer that is in size: 512 * 512 * 4 bytes (Texture2D) or 128 * 128 * 64 * 4 bytes (Texture3D)
    destTexture[GET_POS_TYPE_LOCATION(pos)] = CheckForResidencyErrors(data.value, feedback);
}

[numthreads(NUM_THREADS_X, NUM_THREADS_Y, NUM_THREADS_Z)]
void ProcessRawBufferNormal(uint3 dispatchId : SV_DispatchThreadID)
{
    POS_TYPE pos = GET_POS_TYPE_LOCATION(dispatchId);
    uint byteIndex = 4 * (GET_LINEAR_BUFFER_LOCATION(pos));
    uint value = rawBuffer.Load(byteIndex); // Input is a linear buffer that is in size: 512 * 512 * 4 bytes (Texture2D) or 128 * 128 * 64 * 4 bytes (Texture3D)
    destTexture[GET_POS_TYPE_LOCATION(pos)] = value;
}

[numthreads(NUM_THREADS_X, NUM_THREADS_Y, NUM_THREADS_Z)]
void ProcessStructuredBufferNormal(uint3 dispatchId : SV_DispatchThreadID)
{
    POS_TYPE pos = GET_POS_TYPE_LOCATION(dispatchId);
    uint index = GET_LINEAR_BUFFER_LOCATION(pos);
    MyStruct data = structBuffer.Load(index); // Input is a linear buffer that is in size: 512 * 512 * 4 bytes (Texture2D) or 128 * 128 * 64 * 4 bytes (Texture3D)
    destTexture[GET_POS_TYPE_LOCATION(pos)] = data.value;
}
