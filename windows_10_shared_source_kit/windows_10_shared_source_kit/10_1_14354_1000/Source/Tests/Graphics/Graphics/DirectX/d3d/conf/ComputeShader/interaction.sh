// This file is included by the C files too, in which case NO_HLSL is defined

#ifdef NOHLSL
#define UINT_TYPE   unsigned int
#else
#define UINT_TYPE   uint
#endif

struct SResourceDimensionConstants
{
    UINT_TYPE Divisor;
    UINT_TYPE w;
    UINT_TYPE h;
    UINT_TYPE d;
    UINT_TYPE ArrayIndex;
};

#ifndef NOHLSL

SResourceDimensionConstants ResDims : register(b0);


struct BufferStruct
{
    uint Value;
};

Texture1D       SRV1D_at_0        : register(t0);
Texture1DArray  SRV1DArray_at_0   : register(t0);
Texture2D       SRV2D_at_0        : register(t0);
Texture2DArray  SRV2DArray_at_0   : register(t0);
Texture3D       SRV3D_at_0        : register(t0);
Buffer          SRVBuffer_at_0    : register(t0);

SamplerState SamplerPoint : register(s0);

StructuredBuffer<BufferStruct> StructuredBuff_at_1 : register(t1);

RWStructuredBuffer<BufferStruct> UAVStructured_at_0: UAVStructured_at_0: register(u0);
RWStructuredBuffer<BufferStruct> UAVStructured_at_1: UAVStructured_at_1: register(u1);
RWBuffer<uint>                    UAVTyped_at_0     : register(u0);
RWBuffer<uint>                    UAVTypedFloat_at_0: UAVTypedFloat_at_0: register(u0);
RWBuffer<uint>                    UAVTyped_at_1     : register(u1);

RWTexture1D<uint>                 UAVTexture1D_at_0         : register(u0);
RWTexture1DArray<uint>            UAVTexture1DArray_at_0    : register(u0);
RWTexture2D<uint>                 UAVTexture2D_at_0         : register(u0);
RWTexture2DArray<uint>            UAVTexture2DArray_at_0    : register(u0);
RWTexture3D<uint>                 UAVTexture3D_at_0         : register(u0);

RWTexture1D<uint>                 UAVTexture1D_at_1         : register(u1);
RWTexture1DArray<uint>            UAVTexture1DArray_at_1    : register(u1);
RWTexture2D<uint>                 UAVTexture2D_at_1         : register(u1);
RWTexture2DArray<uint>            UAVTexture2DArray_at_1    : register(u1);
RWTexture3D<uint>                 UAVTexture3D_at_1         : register(u1);

RWTexture1D<uint>                 UAVTexture1D_at_3 : register(u3);

ByteAddressBuffer                 RawBufferSRV_at_0 : register(t0);
RWByteAddressBuffer               RWRawBufferUAV_at_0 : register(u0);
RWByteAddressBuffer               RWRawBufferUAV_at_1 : register(u1);

//---------------------------------------------------------------------------------------------------------------------------------
uint PackCoordinates( uint3 gid, uint ArrayIndex )
{
    return gid.x | (gid.y << 8) | (gid.z << 16) | (ResDims.ArrayIndex << 24);
}

float4 UnpackUint( uint val )
{
    return float4( val & 0xff, (val >> 8) &0xff, (val >>16) & 0xff, (val >> 24) & 0xff );
}

//---------------------------------------------------------------------------------------------------------------------------------
// Compute shaders


[numthreads(1, 1, 1)]
void CSUavStructuredBuffer(uint3 gid : SV_GroupID )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    
    BufferStruct buf;
    buf.Value = PackCoordinates(gid, ResDims.ArrayIndex);
    UAVStructured_at_0[ 
        gid.x +
        gid.y * Pitch +
        gid.z * SlicePitch ] = buf;
}

[numthreads(1, 1, 1)]
void CSUavTypedBuffer(uint3 gid : SV_GroupID )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    
    UAVTyped_at_0[ 
        gid.x +
        gid.y * Pitch +
        gid.z * SlicePitch ] = PackCoordinates( gid, ResDims.ArrayIndex );
}

[numthreads(1, 1, 1)]
void CSUavRawBuffer(uint3 gid : SV_GroupID )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    
    RWRawBufferUAV_at_0.Store( 
        (gid.x +
        gid.y * Pitch +
        gid.z * SlicePitch) * 4 /*sizeof(DWORD) */ , PackCoordinates( gid, ResDims.ArrayIndex ) );
}

[numthreads(1, 1, 1)]
void CSUavTexture1D(uint3 gid : SV_GroupID )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    
    UAVTexture1D_at_0[ gid.x ] = PackCoordinates( gid, ResDims.ArrayIndex );
}

[numthreads(1, 1, 1)]
void CSUavTexture1DArray(uint3 gid : SV_GroupID )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    
    UAVTexture1DArray_at_0[ float2( gid.x, ResDims.ArrayIndex) ] = PackCoordinates( gid, ResDims.ArrayIndex );
}

[numthreads(1, 1, 1)]
void CSUavTexture2D(uint3 gid : SV_GroupID )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    
    UAVTexture2D_at_0[ gid.xy ] = PackCoordinates( gid, ResDims.ArrayIndex );
}

[numthreads(1, 1, 1)]
void CSUavTexture2DArray(uint3 gid : SV_GroupID )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    
    UAVTexture2DArray_at_0[ float3( gid.xy, ResDims.ArrayIndex)  ] = PackCoordinates( gid, ResDims.ArrayIndex );
}

[numthreads(1, 1, 1)]
void CSUavTexture3D(uint3 gid : SV_GroupID )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    
    UAVTexture3D_at_0[ gid.xyz ] = PackCoordinates( gid, ResDims.ArrayIndex );
}

[numthreads(1, 1, 1)]
void CSUavReadTypedBuffer(uint3 gid : SV_GroupID )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    
    UAVTexture3D_at_1[ gid.xyz ] = UAVTypedFloat_at_0[ gid.x + gid.y * Pitch + gid.z * SlicePitch ];
}

//---------------------------------------------------------------------------------------------------------------------------------
//These are the two CS read cases that run on CS_4_0, so can use only one UAV bind slot.
//The buffer is double-sized, and the results are placed at the end of the buffer.

[numthreads(1, 1, 1)]
void CSUavReadStructuredBuffer(uint3 gid : SV_GroupID )
{
    BufferStruct buf;
    
    buf = UAVStructured_at_0[ gid.x ];
        
    UAVStructured_at_0[ gid.x + ResDims.w/2 ] = buf;
}

[numthreads(1, 1, 1)]
void CSUavReadRawBuffer(uint3 gid : SV_GroupID )
{
    uint ByteOffset = gid.x  * 4 /*sizeof(DWORD) */;
    
    RWRawBufferUAV_at_0.Store( (gid.x + ResDims.w/2) * 4,  RWRawBufferUAV_at_0.Load(ByteOffset) );
}
//---------------------------------------------------------------------------------------------------------------------------------


[numthreads(1, 1, 1)]
void CSUavReadTexture1D(uint3 gid : SV_GroupID )
{
    UAVTexture3D_at_1[ gid.xyz ] = UAVTexture1D_at_0[ gid.x ];
}

[numthreads(1, 1, 1)]
void CSUavReadTexture1DArray(uint3 gid : SV_GroupID )
{
    UAVTexture3D_at_1[ gid.xyz ] = UAVTexture1DArray_at_0[ float2( gid.x,  ResDims.ArrayIndex) ];
}

[numthreads(1, 1, 1)]
void CSUavReadTexture2D(uint3 gid : SV_GroupID )
{
    UAVTexture3D_at_1[ gid.xyz ] = UAVTexture2D_at_0[ gid.xy ];
}

[numthreads(1, 1, 1)]
void CSUavReadTexture2DArray(uint3 gid : SV_GroupID )
{
    UAVTexture3D_at_1[ gid.xyz ] = UAVTexture2DArray_at_0[ float3( gid.xy,  ResDims.ArrayIndex) ];
}

[numthreads(1, 1, 1)]
void CSUavReadTexture3D(uint3 gid : SV_GroupID )
{
    UAVTexture3D_at_1[ gid.xyz ] = UAVTexture3D_at_0[ gid.xyz ];
}
//---------------------------------------------------------------------------------------------------------------------------------
// Vertex shader
//

struct VSIn
{
    float3  PointSamplePosition   : PointSamplePosition;
    float3  TargetPointPosition   : TargetPointPosition;
    float3  RawPointCoordinates   : RawPointCoordinates;
    uint4  DataFromComputeBuffer : DataFromComputeBuffer;
};

struct VSOut
{
    float4  SysPosition : SV_Position;
    float   TargetPointZ : TargetPointZ;
    float3  PointSamplePosition : PointSamplePosition;
    float3  RawPointCoordinates   : RawPointCoordinates;
    float4  DataFromComputeBuffer : DataFromComputeBuffer; //only valid in IA read cases, otherwise zero and overwritten later in the pipeline
};

VSOut VSMain( VSIn input )
{
    VSOut output;
    output.SysPosition           = float4( input.TargetPointPosition.xy, 0, 1);
    output.TargetPointZ          = input.TargetPointPosition.z;
    output.PointSamplePosition   = input.PointSamplePosition;
    output.RawPointCoordinates   = input.RawPointCoordinates;
    output.DataFromComputeBuffer = input.DataFromComputeBuffer;
    return output;
}

VSOut VSSRV1DSample( VSIn input )
{
    VSOut output;
    output.SysPosition           = float4( input.TargetPointPosition.xy, 0, 1);
    output.TargetPointZ          = input.TargetPointPosition.z;
    output.PointSamplePosition   = input.PointSamplePosition;
    output.RawPointCoordinates   = input.RawPointCoordinates;
    output.DataFromComputeBuffer = SRV1D_at_0.SampleLevel( SamplerPoint,  input.PointSamplePosition.x, 0 ) * 255;
    return output;
}

VSOut VSSRV1DSampleArray( VSIn input )
{
    VSOut output;
    output.SysPosition           = float4( input.TargetPointPosition.xy, 0, 1);
    output.TargetPointZ          = input.TargetPointPosition.z;
    output.PointSamplePosition   = input.PointSamplePosition;
    output.RawPointCoordinates   = input.RawPointCoordinates;
    output.DataFromComputeBuffer = SRV1DArray_at_0.SampleLevel( SamplerPoint,  input.PointSamplePosition.xz, 0 ) * 255;
    return output;
}

VSOut VSSRV2DSample( VSIn input )
{
    VSOut output;
    output.SysPosition           = float4( input.TargetPointPosition.xy, 0, 1);
    output.TargetPointZ          = input.TargetPointPosition.z;
    output.PointSamplePosition   = input.PointSamplePosition;
    output.RawPointCoordinates   = input.RawPointCoordinates;
    output.DataFromComputeBuffer = SRV2D_at_0.SampleLevel( SamplerPoint,  input.PointSamplePosition.xy, 0 ) * 255;
    return output;
}

VSOut VSSRV2DSampleArray( VSIn input )
{
    VSOut output;
    output.SysPosition           = float4( input.TargetPointPosition.xy, 0, 1);
    output.TargetPointZ          = input.TargetPointPosition.z;
    output.PointSamplePosition   = input.PointSamplePosition;
    output.RawPointCoordinates   = input.RawPointCoordinates;
    output.DataFromComputeBuffer = SRV2DArray_at_0.SampleLevel( SamplerPoint,  input.PointSamplePosition.xyz , 0 ) * 255;
    return output;
}

VSOut VSSRV3DSample( VSIn input )
{
    VSOut output;
    output.SysPosition           = float4( input.TargetPointPosition.xy, 0, 1);
    output.TargetPointZ          = input.TargetPointPosition.z;
    output.PointSamplePosition   = input.PointSamplePosition;
    output.RawPointCoordinates   = input.RawPointCoordinates;
    output.DataFromComputeBuffer = SRV3D_at_0.SampleLevel( SamplerPoint,  input.PointSamplePosition.xyz, 0 ) * 255;
    return output;
}

VSOut VSSRVBufferSample( VSIn input )
{
    VSOut output;
    output.SysPosition           = float4( input.TargetPointPosition.xy, 0, 1);
    output.TargetPointZ          = input.TargetPointPosition.z;
    output.PointSamplePosition   = input.PointSamplePosition;
    output.RawPointCoordinates   = input.RawPointCoordinates;
    output.DataFromComputeBuffer = UnpackUint( SRVBuffer_at_0.Load( input.PointSamplePosition.x * ResDims.w ).x );
    return output;
}

VSOut VSSampleStructured( VSIn input )
{
    VSOut output;
    output.SysPosition         = float4( input.TargetPointPosition.xy, 0, 1);
    output.TargetPointZ        = input.TargetPointPosition.z;
    output.PointSamplePosition = input.PointSamplePosition;
    output.RawPointCoordinates   = input.RawPointCoordinates;
    BufferStruct buf           = StructuredBuff_at_1[ input.PointSamplePosition.x * ResDims.w ];
    output.DataFromComputeBuffer = UnpackUint( buf.Value );
    return output;
}

VSOut VSSampleRawSRV( VSIn input )
{
    VSOut output;
    output.SysPosition         = float4( input.TargetPointPosition.xy, 0, 1);
    output.TargetPointZ        = input.TargetPointPosition.z;
    output.RawPointCoordinates   = input.RawPointCoordinates;
    output.PointSamplePosition = input.PointSamplePosition;
    output.DataFromComputeBuffer = UnpackUint( RawBufferSRV_at_0.Load( (uint)(input.PointSamplePosition.x * ResDims.w) * 4 /* sizeof RGBA8 */ ) );
    return output;
}

//---------------------------------------------------------------------------------------------------------------------------------
// geometry shader
//

struct GSOut
{
    float4  SysPosition : SV_Position;
    float3  PointSamplePosition : PointSamplePosition;
    float4  DataFromComputeBuffer : DataFromComputeBuffer;
    float3  RawPointCoordinates   : RawPointCoordinates;
    nointerpolation uint rtArrayIndex: SV_RenderTargetArrayIndex;
};


[maxvertexcount (1)]
void GSMain(inout PointStream<GSOut> Stream, point VSOut input[1])
{
	GSOut output;
    output.SysPosition          = input[0].SysPosition;
    output.PointSamplePosition  = input[0].PointSamplePosition;
    output.rtArrayIndex         = input[0].TargetPointZ;
    output.DataFromComputeBuffer= input[0].DataFromComputeBuffer;
    output.RawPointCoordinates  = input[0].RawPointCoordinates;
    Stream.Append(output);
}

//---------------------------------------------------------------------------------------------------------------------------------
// pixel shaders
//

struct PSOut
{
    float4  color   :   SV_Target;
};

PSOut PSPassthrough( GSOut input )
{
    PSOut output;
    output.color = input.DataFromComputeBuffer / 255;
    return output;
}

PSOut PSWriteRT( GSOut input )
{
    PSOut output;
    output.color = float4( input.RawPointCoordinates / 255, 0 );
    return output;
}

PSOut PSWriteRTArray( GSOut input )
{
    PSOut output;
    output.color = float4( input.RawPointCoordinates.xy / 255, 0, (float)ResDims.ArrayIndex / 255.0f );
    return output;
}

PSOut PSSRVBufferSample( GSOut input )
{
    PSOut output;
    output.color = SRVBuffer_at_0.Load( input.PointSamplePosition.x * ResDims.w );
    //Use this line to test:
    //output.color = uint4( input.PointSamplePosition, 0.f );
    return output;
}

PSOut PSSRV1DSample( GSOut input )
{
    PSOut output;
    output.color = SRV1D_at_0.Sample( SamplerPoint, input.PointSamplePosition.x );
    //Use this line to test:
    //output.color = uint4( input.PointSamplePosition, 0.f );
    return output;
}

PSOut PSSRV2DSample( GSOut input )
{
    PSOut output;
    output.color = SRV2D_at_0.Sample( SamplerPoint, input.PointSamplePosition.xy );
    //Use this line to test:
    //output.color = uint4( input.PointSamplePosition, 0.f );
    return output;
}

PSOut PSSRV1DArraySample( GSOut input )
{
    PSOut output;
    output.color = SRV1DArray_at_0.Sample( SamplerPoint, float2(input.PointSamplePosition.x, ResDims.ArrayIndex) );
    //Use this line to test:
    //output.color = uint4( input.PointSamplePosition, 0.f );
    return output;
}

PSOut PSSRV2DArraySample( GSOut input )
{
    PSOut output;
    output.color = SRV2DArray_at_0.Sample( SamplerPoint, float3( input.PointSamplePosition.xy, ResDims.ArrayIndex ) );
    //Use this line to test:
    //output.color = uint4( input.PointSamplePosition, 0.f );
    return output;
}

PSOut PSSRV3DSample( GSOut input )
{
    PSOut output;
    output.color = SRV3D_at_0.Sample( SamplerPoint, input.PointSamplePosition.xyz );
    //Use this line to test:
    //output.color = uint4( input.PointSamplePosition, 0.f );
    return output;
}

PSOut PSSampleStructured( GSOut input )
{
    BufferStruct buf = StructuredBuff_at_1[ input.PointSamplePosition.x * ResDims.w ];

    PSOut output;
    output.color = UnpackUint(buf.Value) / 255;
    
    return output;
}

PSOut PSSampleRawSRV( GSOut input )
{
    PSOut output;
    //output.color = RawBufferSRV_at_0.Load( (uint)(input.PointSamplePosition.x * ResDims.w) * 4 /* sizeof RGBA8 */ );
    output.color = UnpackUint( RawBufferSRV_at_0.Load( (uint)(input.PointSamplePosition.x * ResDims.w) * 4 /* sizeof RGBA8 */ ) ) / 255;
    
    return output;
}

PSOut PSUAVStructuredBuffer( GSOut input )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    
    BufferStruct buf;
    uint3 PixelLocation = uint3(
        input.PointSamplePosition.x * ResDims.w,
        input.PointSamplePosition.y * ResDims.h, 
        input.PointSamplePosition.z * ResDims.d );
        
    buf.Value = PackCoordinates( PixelLocation, ResDims.ArrayIndex );
    
    UAVStructured_at_1[ 
        PixelLocation.x +
        PixelLocation.y * Pitch +
        PixelLocation.z * SlicePitch ] = buf;
        
    PSOut output;
    output.color = uint4(0.f, 0.f, 0.f, 0.f );
    return output;
}

PSOut PSUAVTypedBuffer( GSOut input )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    uint3 PixelLocation = uint3(
        input.PointSamplePosition.x * ResDims.w,
        input.PointSamplePosition.y * ResDims.h, 
        input.PointSamplePosition.z * ResDims.d );
    
    UAVTyped_at_1[ 
        PixelLocation.x +
        PixelLocation.y * Pitch +
        PixelLocation.z * SlicePitch ] = PackCoordinates( PixelLocation, ResDims.ArrayIndex );
        
    PSOut output;
    output.color = uint4(0.f, 0.f, 0.f, 0.f );
    return output;
}

PSOut PSUAV1D( GSOut input )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    uint3 PixelLocation = uint3(
        input.PointSamplePosition.x * ResDims.w,
        input.PointSamplePosition.y * ResDims.h, 
        input.PointSamplePosition.z * ResDims.d );
    
    UAVTexture1D_at_1[ PixelLocation.x ] = PackCoordinates( PixelLocation, ResDims.ArrayIndex );
        
    PSOut output;
    output.color = uint4(0.f, 0.f, 0.f, 0.f );
    return output;
}

PSOut PSUAV1DArray( GSOut input )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    uint3 PixelLocation = uint3(
        input.PointSamplePosition.x * ResDims.w,
        input.PointSamplePosition.y * ResDims.h, 
        input.PointSamplePosition.z * ResDims.d );
    
    UAVTexture1DArray_at_1[ float2( PixelLocation.x, ResDims.ArrayIndex ) ] = PackCoordinates( PixelLocation, ResDims.ArrayIndex );
        
    PSOut output;
    output.color = uint4(0.f, 0.f, 0.f, 0.f );
    return output;
}

PSOut PSUAV2D( GSOut input )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    uint3 PixelLocation = uint3(
        input.PointSamplePosition.x * ResDims.w,
        input.PointSamplePosition.y * ResDims.h, 
        input.PointSamplePosition.z * ResDims.d );
    
    UAVTexture2D_at_1[ PixelLocation.xy ] = PackCoordinates( PixelLocation, ResDims.ArrayIndex );
        
    PSOut output;
    output.color = uint4(0.f, 0.f, 0.f, 0.f );
    return output;
}

PSOut PSUAV2DArray( GSOut input )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    uint3 PixelLocation = uint3(
        input.PointSamplePosition.x * ResDims.w,
        input.PointSamplePosition.y * ResDims.h, 
        input.PointSamplePosition.z * ResDims.d );
    
    UAVTexture2DArray_at_1[ float3( PixelLocation.xy, ResDims.ArrayIndex) ] = PackCoordinates( PixelLocation, ResDims.ArrayIndex );
        
    PSOut output;
    output.color = uint4(0.f, 0.f, 0.f, 0.f );
    return output;
}

PSOut PSUAV3D( GSOut input )
{
    uint Pitch = ResDims.w;
    uint SlicePitch = Pitch * ResDims.h;
    uint3 PixelLocation = uint3(
        input.PointSamplePosition.x * ResDims.w,
        input.PointSamplePosition.y * ResDims.h, 
        input.PointSamplePosition.z * ResDims.d );
    
    UAVTexture3D_at_1[ PixelLocation.xyz ] = PackCoordinates( PixelLocation, ResDims.ArrayIndex );
        
    PSOut output;
    output.color = uint4(0.f, 0.f, 0.f, 0.f );
    return output;
}

//---------------------------------------------------------------------------------------------------------------------------------
// game of life shader

int Wrap( int c, int a )
{
    return ( c + a ) & (255);
}

int WrappedOffset(int x, int y, int xOff, int yOff )
{
    return Wrap(x, xOff) + Wrap(y, yOff) * 256;
}

RWTexture2D<float4>                 UAVTexture2DUNORM_at_0         : register(u0);

[numthreads(1, 1, 1)]
void Life(uint3 gid : SV_GroupID )
{
    uint Pitch = 256 * 4;
    
    uint Dest = 0;
    
    int w = gid.x;
    int h = gid.y;
    
    uint cAliveNeighbors = 0;
    cAliveNeighbors += SRV2D_at_0.Load( int3( Wrap( w, -1 ), Wrap( h, -1 ), 0 ) ).x == 0 ? 0 : 1;
    cAliveNeighbors += SRV2D_at_0.Load( int3( Wrap( w, -1 ), Wrap( h,  1 ), 0 ) ).x == 0 ? 0 : 1;
    cAliveNeighbors += SRV2D_at_0.Load( int3( Wrap( w, -1 ), Wrap( h,  0 ), 0 ) ).x == 0 ? 0 : 1;
    cAliveNeighbors += SRV2D_at_0.Load( int3( Wrap( w,  1 ), Wrap( h, -1 ), 0 ) ).x == 0 ? 0 : 1;
    cAliveNeighbors += SRV2D_at_0.Load( int3( Wrap( w,  1 ), Wrap( h,  1 ), 0 ) ).x == 0 ? 0 : 1;
    cAliveNeighbors += SRV2D_at_0.Load( int3( Wrap( w,  1 ), Wrap( h,  0 ), 0 ) ).x == 0 ? 0 : 1;

    if ( SRV2D_at_0.Load( int3(w,h,0) ).x > .5 )
    {
        if ( cAliveNeighbors <= 1 || cAliveNeighbors >= 4 )
            Dest = 0;
        else
            Dest = 0xffffffff;
    }
    else
    {
        if ( cAliveNeighbors != 3 )
            Dest = 0;
        else
            Dest = 0xffffffff;
    }
    UAVTexture2DUNORM_at_0[ gid.xy ] = Dest;
}

#endif //NO_HLSL
