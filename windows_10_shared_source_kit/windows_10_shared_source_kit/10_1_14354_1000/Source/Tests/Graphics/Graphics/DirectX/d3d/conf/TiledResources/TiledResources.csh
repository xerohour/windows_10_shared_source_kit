// Compute shader source for WGF11TiledResources

RWTexture2D<uint> sourceTexture;	// Needs to be viewed as a R32_UINT UAV
RWTexture2D<uint> destTexture;		// Needs to be viewed as a a R32_UINT UAV

[numthreads(8, 8, 1)]
void CopyTexCS(uint3 dispatchId : SV_DispatchThreadID)
{
	uint2 pos = dispatchId.xy;
	uint value = sourceTexture[pos];
	value = value ^ 0x000000ff; // invert the bits in the red channel assuming texture data is really R8B8B8A8_UNORM
	destTexture[pos] = value | 0xff000000; // Sets the A channel to 0xff assuming texture data is really R8B8B8A8_UNORM
}